#include <e32debug.h>
#include <badesca.h>
#include <bautils.h>
#include <f32file.h>
#include "LogManServer_CommandManager.h"
#include "LogManServer_Server.h"
#include "loggingdefs.h"
// Constants
_LIT8(KStrUnknownCommand, "\nUnknown command. Try 'list' or 'help'.\n");
_LIT8(KStrNotEnoughParameters, "Not enough parameters\n");
_LIT(KFmtDirEntry, "%02d.%02d.%04d  %02d:%02d  %14S %S\n");
_LIT(KFmtDirInfo, "\nDirectory of %S");
_LIT(KFmtCopyErr, "\nFailed to copy '%S', err:%d\n");
_LIT(KStrDir, "<DIR>         ");
_LIT8(KStrFileExistsQuery, "\nFile exists. Overwrite? n\\y\\a\\c:");
_LIT8(KStrInvalidAnswer, "\nInvalid answer.\n");
_LIT8
		(KStrPathNotFound,
				"\nThe system cannot find the path specified. Err:%d\n");
_LIT8(KFmtErrDetected, "\nError:%d\n");
// TODO: Generate
#define NL "\r\n"
_LIT8(KStrNewLine, NL);
_LIT8(KCmdList, "list");
_LIT8(KCmdHelp, "help");
_LIT8(KCmdCopy, "cp");
_LIT8(KCmdLs, "ls");
_LIT8(KCmdDir, "dir");
_LIT8(KCmdExec, "exec");
_LIT8(KCmdKill, "kill");
_LIT8(KCmdPut, "put");
_LIT8(KCmdGet, "get");
_LIT8(KCmdListProcess, "ps");
_LIT8(KStrHelpMsg, ""
	"list                   - List possible commands\n"
	"dir/ls <path>          - List contents of a directory\n"
	"cp <source> <target>   - Copy files in device.\n"
	"put <target>           - File transfer to device\n"
	"get <source>[startpos] - Echo file contents. If startpos is negative,\n"
	"                         position is calculated from the end of file.\n"
	"exec <target>[<args>]  - Launch executable with given arguments.\n"
	"kill <process>         - Kill process. Wildcard supported.\n"
	"ps <process>           - List processes. Wildcard supported.\n"
	"\n"
	);

const TChar KCharDash = '\\';
const TChar KDoubleColon = ':';

CLoggingServerCommandManager::CLoggingServerCommandManager(
		CLoggingServerServer* aLoggingServerServer) :
	CActive(0), iLoggingServerServer(aLoggingServerServer)
{

}

void CLoggingServerCommandManager::ConstructL()
{
	CActiveScheduler::Add(this);
	iTimer.CreateLocal();
	iCommandBuffer.Create(512);
}

void CLoggingServerCommandManager::DoCancel()
{
	iTimer.Cancel();
}

void CLoggingServerCommandManager::Start()
{
	Cancel();
	iTimer.After(iStatus, TTimeIntervalMicroSeconds32(100));
	SetActive();
}

void CLoggingServerCommandManager::SocketState(TInt aNewState)
{
}
;

void CLoggingServerCommandManager::SocketData(TDesC8& aData)
{

	for (TInt i = 0; i < aData.Length(); i++)
	{
		if (aData[i] == '\r')
			continue;

		if (aData[i] == '\n')
		{
			this->HandleCommand();
			// Clear buffer
			iCommandBuffer.Delete(0, iCommandBuffer.Length());
			break;
		}
		else
		{
			iCommandBuffer.Append(aData[i]);
		}
	}

}

void CLoggingServerCommandManager::RunL()
{

	if (iLoggingServerServer->IsClosing())
	{
		PRINTF ("CLoggingServerCommandManager stopping")
		return;
	}
	else
	{
		//iLoggingServerServer->ReadSocket();

		TRequestStatus readstatus = KRequestPending;
		RComm& comm = iLoggingServerServer->iSerialComm;
		TBuf8<1> buf;

		do
		{
			comm.Read(readstatus, 100, buf, 1);
			User::WaitForRequest(readstatus);

			if (readstatus == KErrNone)
			{
				// Echo back
				iLoggingServerServer->SendMessage(buf);

				if (buf[0] == '\n' || buf[0] == '\r')
				{
					this->HandleCommand();
					// Clear buffer
					iCommandBuffer.Delete(0, iCommandBuffer.Length() );
				}
				else
				{
					iCommandBuffer.Append(buf[0]);
				}
			}
		} while (readstatus == KErrNone && !iLoggingServerServer->IsClosing() );

		Start();
	}
}

CLoggingServerCommandManager::~CLoggingServerCommandManager()
{
	Cancel();
	iCommandBuffer.Close();
}

/** Get parameter from parameter list
 * @parameter aParameters Parameter array
 * @parameter aBuf      Reference to a buffer to be filled with the path
 * @parameter aIndex    Index in aParameters to read
 */
void GetArgLC(RArray<RBuf8>& aParameters, RBuf& aBuf, TInt aIndex)
{
	User::LeaveIfError(aBuf.Create(aParameters[1].Length()));
	CleanupClosePushL(aBuf);
	aBuf.Copy(aParameters[aIndex]);
	aBuf.Trim();
}

/**
 * Parse path argument.
 * @param aParameters   Parameter array
 * @param aFs           File server session( connected )
 * @param aBuf          Reference to a buffer to be filled with the path
 * @param aIndex        Index in aParameters to be read.
 * @return ETrue if path is a directory.
 */
TBool GetPathArgLC(RArray<RBuf8>& aParameters, RFs& aFs, RBuf& aBuf, TInt aIndex)
{
	User::LeaveIfError(aBuf.Create(aParameters[1].Length() + 1));
	CleanupClosePushL(aBuf);
	aBuf.Copy(aParameters[aIndex]);

	// If folder, ensure there is dash at the end.
	TBool isDir = EFalse;
	BaflUtils::IsFolder(aFs, aBuf, isDir);
	if (!isDir)
	{
		// BaflUtils says it's not directory if drive is given( ex. C: )
		isDir = (aBuf[aBuf.Length() - 1] == KDoubleColon);
	}
	if ((isDir || aBuf[aBuf.Length() - 1] == KDoubleColon) && aBuf[aBuf.Length() - 1] != KCharDash)
	{
		// Add dash or get dir does not list folder
		aBuf.Append(KCharDash);
	}

	aBuf.Trim();
	return isDir;
}

/** Send error message and Leave on error
 * @param aErr  Error code.
 */
void CLoggingServerCommandManager::LeaveIfFailedL(TInt aErr)
{
	if (aErr != KErrNone)
	{
		TMessageBuffer8 tmp;
		tmp.Format(KFmtErrDetected, aErr);
		iLoggingServerServer->SendMessage(tmp);
		User::Leave(aErr);
	}
}

/** Handle process starting command */
TInt CLoggingServerCommandManager::HandleCmdExecL(RArray<RBuf8>& aParameters,
		RFs& aFs)
{
	if (aParameters.Count() < 2)
	{
		iLoggingServerServer->SendMessage(KStrNotEnoughParameters);
		return KErrArgument;
	}

	RBuf path;
	GetPathArgLC(aParameters, aFs, path, 1);

	RBuf command;
	if (aParameters.Count() >= 3)
	{
		TInt size = 0;
		// Determine size for the buffer;
		for (TInt i = 2; i < aParameters.Count(); i++)
		{
			size += aParameters[i].Length();
		}

		// Create the buffer
		LeaveIfFailedL(command.Create(size + aParameters.Count()));
		CleanupClosePushL(command);

		for (TInt i = 2; i < aParameters.Count(); i++)
		{
			TFullName toUnicode;
			toUnicode.Copy(aParameters[i]);
			command.Append(toUnicode);
			if (i < aParameters.Count() - 1)
				command.Append(_L(" ") );
		}
	}
	else
	{
		LeaveIfFailedL(command.Create(1));
		CleanupClosePushL(command);
	}

	RProcess process;
	LeaveIfFailedL(process.Create(path, command));
	CleanupClosePushL(process);
	process.Resume();
	_LIT(KFmtProcessLaunchedMsg, "'%S' started.\n");
	//TMessageBuffer8 tmp;
	RBuf msg;
	if (msg.Create(128 + process.FileName().Length()) == KErrNone)
	{
		msg.Format(KFmtProcessLaunchedMsg, &(process.FileName()));
		iLoggingServerServer->SendMessage(msg);
		msg.Close();
	}

	CleanupStack::PopAndDestroy(&process);
	CleanupStack::PopAndDestroy(&command);
	CleanupStack::PopAndDestroy(&path);

}

/** Handle helper command */
TInt CLoggingServerCommandManager::HandleCmdListL()
{
	// TODO: Generate
	/**[[[cog
	 ]]]*/
	///[[[end]]]
	TBuf8<128> version;
	_LIT8(KFmtVersionInfo, "LogMan Shell v.%d.%02d.%04d ("__DATE__ " " __TIME__ ") Rev:%d\n");

	version.Format(KFmtVersionInfo, KLogServMajorVersionNumber,
	KLogServMinorVersionNumber, KLogServBuildVersionNumber,
	KRevisionNumber);
	iLoggingServerServer->SendMessage(version);

	RBuf8 line;
	line.Create(80);
	line.Fill('=');
	iLoggingServerServer->SendMessage(line);
	iLoggingServerServer->SendMessage(KStrNewLine);
	line.Close();

	iLoggingServerServer->SendMessage(KStrHelpMsg);
	iLoggingServerServer->SendMessage(KStrNewLine);
}

/** Handle file listing command */
TInt CLoggingServerCommandManager::HandleCmdLsL(RArray<RBuf8>& aParameters,
		RFs& aFs)
{
	//iLoggingServerServer->SendMessage(_L8("void CLoggingServerCommandManager::HandleCmdList()\n") );
	// TODO: Add current directory support
	if (aParameters.Count() < 2)
	{
		iLoggingServerServer->SendMessage(KStrNotEnoughParameters);
		return KErrArgument;
	}

	TInt i;
	TFullName totalPath;
	TMessageBuffer fileName;
	TInt err;

	// Convert to unicode
	RBuf path;
	GetPathArgLC(aParameters, aFs, path, 1);

	CDir* dirList;
	err = aFs.GetDir(path, KEntryAttMaskSupported, ESortByName, dirList);
	LeaveIfFailedL(err);

	CleanupStack::PushL(dirList);

	// Statistics
	TInt total_size = 0;
	TInt files = 0;
	TInt dirs = 0;

	TBuf<25> sizeOrDir;
	// Send directory path.
	{
		TMessageBuffer tmp;
		tmp.Format(KFmtDirInfo, &path);
		iLoggingServerServer->SendMessage(tmp);
		iLoggingServerServer->SendMessage(KStrNewLine);
		iLoggingServerServer->SendMessage(KStrNewLine);
	}

	for (i = 0; i < dirList->Count(); i++)
	{
		const TEntry& entry = (*dirList)[i];
		// Get date and time of modification
		TDateTime date = entry.iModified.DateTime();
		fileName = entry.iName;

		if (entry.IsDir())
		{
			sizeOrDir.Copy(KStrDir);
			dirs++;
		}
		else
		{
			sizeOrDir.Num(entry.iSize);
			files++;
			total_size += entry.iSize;
		}

		RBuf line;
		line.Create(35 + sizeOrDir.Length() + fileName.Length());
		// No leaving here
		line.Format(KFmtDirEntry, date.Day() + 1, date.Month(), date.Year(),
				date.Hour(), date.Minute(), &sizeOrDir, &fileName);
		iLoggingServerServer->SendMessage(line);

		line.Close();
	}

	// Send statistics
	{
		_LIT(KFmtStats, "%d Dir(s) %d File(s) %d bytes\n\n");
		TMessageBuffer tmp;
		tmp.Format(KFmtStats, dirs, files, total_size);
		iLoggingServerServer->SendMessage(tmp);
	}

	//iLoggingServerServer->SendMessage(fileName);

	CleanupStack::PopAndDestroy(2); // path, dirlist

	return KErrNone;
}

/** Handle process killing and listing */
TInt CLoggingServerCommandManager::HandleCmdKillAndFindL(
		RArray<RBuf8>& aParameters, TBool aDoKill)
{
	TFindProcess finder;
	RBuf pattern;
	if (aParameters.Count() >= 2)
	{

		GetArgLC(aParameters, pattern, 1);
		finder.Find(pattern);
	}

	TFullName processname;
	TMessageBuffer processinfo;
	_LIT(KFmtDeadProcessInfo,
			"%S Priority:%d" NL " ExitType:%d ExitCategory:%S(%d)");
	_LIT(KFmtAliveProcessInfo,
				"%S Priority:%d" NL
				" SID:%x" NL
			    " MemoryInfo:" NL
			    "  Code   base:%x size:%x" NL
			    "  Data   base:%x size:%x" NL
			    "  Init   base:%x size:%x" NL
			    "  Uninit base:%x size:%x" NL
			    " Threads:" NL
			);

	_LIT(KFmtThreadInfo, "  %S\r\n"
						 "   StackInfo:" NL
						 "    size:%x" NL
						 "    base:%x limit:%x expand:%x" NL
			);
	while (finder.Next(processname) == KErrNone)
	{

		RProcess process;
		TInt err = process.Open(finder);
		if (err == KErrNone)
		{

			if (process.ExitType() != EExitPending)
			{
				processinfo.Format(KFmtDeadProcessInfo, &processname,
						process.Priority(), process.ExitType(),
						&(process.ExitCategory()), process.ExitReason());

				iLoggingServerServer->SendMessage(processinfo);
			}
			// Give more info only when process name is given as a parameter
			else if ( aParameters.Count() >= 2 )
			{
				if (aDoKill)
				{
					iLoggingServerServer->SendMessage(processname);
					process.Kill(-1);
					iLoggingServerServer->SendMessage(_L(" killed") );
				}
				else
				{

					TProcessMemoryInfo info;
					process.GetMemoryInfo( info );

					TUid sid = process.SecureId();

					processinfo.Format(KFmtAliveProcessInfo, &processname,
									   process.Priority(),
									   sid.iUid,
									   info.iCodeBase,
									   info.iCodeSize,
									   info.iConstDataBase,
									   info.iConstDataSize,
									   info.iInitialisedDataBase,
									   info.iInitialisedDataSize,
									   info.iUninitialisedDataBase,
									   info.iUninitialisedDataSize
									   );
					iLoggingServerServer->SendMessage(processinfo);

					// Get threads
					TFindThread tfinder;
					TFullName threadname;
					while( tfinder.Next(threadname) == KErrNone ) {

						RThread thread;
						RProcess owner;

						if( thread.Open( tfinder ) == KErrNone )
						{
							TInt getProErr =  thread.Process( owner );

							if( owner.Id() == process.Id() )
							{
								TThreadStackInfo sinfo;
								thread.StackInfo(sinfo);

								processinfo.Format( KFmtThreadInfo, &threadname,
									( sinfo.iBase - sinfo.iLimit ),
									sinfo.iBase,
									sinfo.iLimit,
									sinfo.iExpandLimit
								);

								iLoggingServerServer->SendMessage(processinfo);
							}
							owner.Close();
							thread.Close();
						}
					}
				}
			}
			else{
				iLoggingServerServer->SendMessage(processname);
			}
			process.Close();

		}
		iLoggingServerServer->SendMessage(KStrNewLine);
	}
/*
	TFindLibrary findLib;
	TFullName libName;
	iLoggingServerServer->SendMessage(_L("Finding libs"));
	while (findLib.Next(libName)==KErrNone)
	{
		iLoggingServerServer->SendMessage( _L("Library") );
		iLoggingServerServer->SendMessage( libName );
		iLoggingServerServer->SendMessage(KStrNewLine);

		RLibrary lib;
		if( lib.Load( libName ) == KErrNone )
		{
			TInt ordinal = 1;
			TLibraryFunction func = lib.Lookup(ordinal);
			while( func != NULL )
			{
				TBuf<128> tmp;
				tmp.Format( _L("ordinal: %x" NL ), ordinal );
				iLoggingServerServer->SendMessage( tmp );

				ordinal += 1;
				func = lib.Lookup(ordinal);
			}

			TInt codesize = 0, datasize = 0;
			if( lib.GetRamSizes(codesize, datasize) )
			{
				TBuf<128> tmp;
				tmp.Format( _L("codesize:%x datasize:%x" NL ), codesize, datasize );
				iLoggingServerServer->SendMessage( tmp );
			}

		}
	}

	TFindChunk findChunk;
	findChunk.Find(_L("*"));

	TFullName chunkName;
	while (findChunk.Next(chunkName)!=KErrNone)
	{
		iLoggingServerServer->SendMessage( _L("Chunk") );
		iLoggingServerServer->SendMessage( chunkName );
		iLoggingServerServer->SendMessage(KStrNewLine);

		RChunk chunk;
		if (chunk.Open(findChunk)!=KErrNone)
			continue;

		TUint8* base=chunk.Base();
		TInt size=chunk.Size();
		chunk.Close();
	}
*/
	if (aParameters.Count() >= 2)
	{
		CleanupStack::PopAndDestroy(&pattern);
	}

}

#define max(a,b) (b<a)?a:b
#define min(a,b) (a<b)?a:b
#define BUFFER_SIZE 128

#define DEBUG( x ) iLoggingServerServer->SendMessage( _L8( x ) );

/** Handle file transfer from device
 *
 */
TInt CLoggingServerCommandManager::HandleCmdGetL(RArray<RBuf8>& aParameters,
		RFs& aFs)
{
	TInt err;

	if (aParameters.Count() < 2)
	{
		iLoggingServerServer->SendMessage(KStrNotEnoughParameters);
		return KErrArgument;
	}

	RBuf path_target;
	GetPathArgLC(aParameters, aFs, path_target, 1);

	TInt startpos = 0;
	if (aParameters.Count() > 2 )
	{
		RBuf _startpos;
		GetArgLC(aParameters, _startpos, 2);
		// Convert to number
		//TInt bytes = 0;
		TLex lexer(_startpos);
		err = lexer.Val(startpos);
		LeaveIfFailedL(err);

		CleanupStack::PopAndDestroy(); // buf
	}

	RFile targetfile;
	LeaveIfFailedL(targetfile.Open(aFs, path_target, EFileShareReadersOrWriters));
	CleanupClosePushL(targetfile);

	TInt size = 0;
	targetfile.Size(size);
	TBuf8<BUFFER_SIZE> tmp;

	// Set read position
	if( startpos < 0 ){
		// Get position from end if negative;
		startpos = Max( 0, size+startpos);
		size -= startpos;
	}

	targetfile.Seek(ESeekCurrent, startpos);
	do
	{
		err = targetfile.Read(tmp, BUFFER_SIZE);
		iLoggingServerServer->SendMessage(tmp);
		LeaveIfFailedL(err);
	}
	while( tmp.Length() > 0 );

	CleanupStack::PopAndDestroy(2); // buf, targetfile

}

/** Handle file transfer to device
 * Using very simple method. Client sends the path to the file and its size.
 * Then the given amount of data is received and written to the file.
 */
TInt CLoggingServerCommandManager::HandleCmdPutL(RArray<RBuf8>& aParameters,
		RFs& aFs)
{
	if (aParameters.Count() < 2)
	{
		iLoggingServerServer->SendMessage(KStrNotEnoughParameters);
		return KErrArgument;
	}

	RBuf path_target;
	GetPathArgLC(aParameters, aFs, path_target, 1);

	RFile targetfile;
	LeaveIfFailedL(targetfile.Replace(aFs, path_target, EFileWrite));
	CleanupClosePushL(targetfile);

	RBuf8 line;
	line.Create(BUFFER_SIZE);
	CleanupClosePushL(line);

	TInt err;
	RComm& comm = iLoggingServerServer->iSerialComm;
	TBool doQuery = ETrue;
	// Get size of the file
	while (doQuery)
	{
		TRequestStatus readstatus = KRequestPending;
		RComm & comm = iLoggingServerServer->iSerialComm;

		TBuf8<1> buf;
		comm.Read(readstatus, 1000, buf, 1);
		User::WaitForRequest(readstatus);
		if (readstatus == KErrNone)
		{
			if (buf[0] == ' ')
			{
				doQuery = EFalse;
			}
			else
			{
				TLex8 lexer(buf);
				TInt isval = 0;
				err = lexer.Val(isval);
				// Add if number
				if (err == KErrNone)
				{
					iLoggingServerServer->SendMessage(buf);
					line.Append(buf);
				}
			}
		}
	}

	// Convert to number
	TInt bytes = 0;
	TLex8 lexer(line);
	err = lexer.Val(bytes);
	LeaveIfFailedL(err);

	TInt received = 0;
	TInt cancelled = 0;
	TBuf8<BUFFER_SIZE> tmp;
	while (received < bytes)
	{
		TRequestStatus readstatus = KRequestPending;
		TInt buffersize= min( BUFFER_SIZE, bytes - received );

		comm.ReadOneOrMore(readstatus, tmp);
		User::WaitForRequest(readstatus);

		TBuf8<10> num;
		num.Num(readstatus.Int());
		iLoggingServerServer->SendMessage(num);
		iLoggingServerServer->SendMessage(KStrNewLine);
		//if (readstatus == KErrTimedOut)
		//{
		if (tmp.Length() > 0)
		{
			//iLoggingServerServer->SendMessage( line );
			received += tmp.Length();
			LeaveIfFailedL(targetfile.Write(tmp, tmp.Length()));

			TBuf8<10> num;
			num.Num(received);
			iLoggingServerServer->SendMessage(num);
			iLoggingServerServer->SendMessage(KStrNewLine);

			continue;
		}

		if (readstatus == KErrCancel)
		{
			cancelled++;
			/*
			 TBuf8<10> num;
			 num.Num(KErrCancel);
			 iLoggingServerServer->SendMessage( num );
			 */
			LeaveIfFailedL(readstatus.Int());
		}
		else
		{
			LeaveIfFailedL(readstatus.Int());
			break;
		}
	}

	CleanupStack::PopAndDestroy(3);

}

/** Handle file copying on device */
TInt CLoggingServerCommandManager::HandleCmdCopyFilesL(
		RArray<RBuf8>& aParameters, RFs& aFs)
{
	// TODO: Add current directory support
	if (aParameters.Count() < 3)
	{
		iLoggingServerServer->SendMessage(KStrNotEnoughParameters);
		return KErrArgument;
	}

	TInt i;
	TFullName totalPath;
	TMessageBuffer fileName;
	TInt err;

	// Convert to unicode
	// Get source path
	RBuf path_source;
	GetPathArgLC(aParameters, aFs, path_source, 1);

	// Get target path
	RBuf path_target;
	TInt isDir = GetPathArgLC(aParameters, aFs, path_target, 2);

	// Get directory listing
	CDir* dirList;
	err = aFs.GetDir(path_source, KEntryAttMaskSupported, ESortByName, dirList);
	LeaveIfFailedL(err);
	CleanupStack::PushL(dirList);

	// Check each directory entry.
	TInt copied = 0;
	TBool doOverwriteAll = EFalse;
	TBool doCopy = ETrue;
	for (i = 0; i < dirList->Count(); i++)
	{
		const TEntry& entry = (*dirList)[i];

		fileName = entry.iName;
		if (!entry.IsDir())
		{
			RBuf full_target;
			// TODO: Check errors
			if (isDir)
			{
				full_target.Create(path_target.Length() + fileName.Length()
						+ 10);
				full_target.Append(path_target); // Dash is added earlier
				full_target.Append(fileName);
			}
			else
			{
				full_target.Create(path_target.Length());
				full_target.Copy(path_target);
			}

			// Check if file exists and query permission
			if (!doOverwriteAll && BaflUtils::FileExists(aFs, full_target))
			{
				doCopy = EFalse;

				iLoggingServerServer->SendMessage(KStrFileExistsQuery);
				TBool doQuery = ETrue;
				while (doQuery)
				{
					TRequestStatus readstatus = KRequestPending;
					RComm& comm = iLoggingServerServer->iSerialComm;
					TBuf8<1> buf;
					comm.Read(readstatus, 1000, buf, 1);
					User::WaitForRequest(readstatus);
					if (readstatus == KErrNone)
					{
						// Echo back
						iLoggingServerServer->SendMessage(buf);
						iLoggingServerServer->SendMessage(KStrNewLine);

						doQuery = EFalse;
						switch (buf[0])
						{
						case 'y':
						{
							doCopy = ETrue;
							break;
						}
						case 'n':
						{
							doCopy = EFalse;
							break;
						}
						case 'a':
						{
							doOverwriteAll = ETrue;
							doCopy = ETrue;
							break;
						}
						case 'c':
						{
							i = dirList->Count();
							doCopy = EFalse;
							break;
						}
						default:
						{
							iLoggingServerServer->SendMessage(KStrInvalidAnswer);
							iLoggingServerServer->SendMessage(KStrFileExistsQuery);
							doQuery = ETrue;
							break;
						}
						}
					}
				}
			}

			// Do the copying
			if (doCopy)
			{
				err = BaflUtils::CopyFile(aFs, path_source, full_target);
				if (err != KErrNone)
				{
					TMessageBuffer tmp;
					tmp.Format(KFmtCopyErr, &fileName, err);
					iLoggingServerServer->SendMessage(tmp);
				}
				else
				{
					copied++;
				}

			}
			// Cleanup
			full_target.Close();
		}
	}

	// Send statistics

	{
		_LIT8(KFmtStats, "%d files copied.\n");
		TMessageBuffer8 tmp;
		tmp.Format(KFmtStats, copied);
		iLoggingServerServer->SendMessage(tmp);
	}

	CleanupStack::PopAndDestroy(3); // path_source, path_target, dirlist

	return KErrNone;
}

/** Add argument to argument array */
void AddToList(RArray<RBuf8>& aArgs, RBuf8& aArg)
{
	RBuf8 tmp;
	// This should be Closed when closing the RArray
	tmp.Create(aArg.Length());
	tmp.Copy(aArg);
	aArgs.Append(tmp);
	aArg.Delete(0, aArg.Length());
}

TInt CLoggingServerCommandManager::HandleCommand()
{
	RArray<RBuf8> args;
	TRAPD(err, HandleCommandL(args) );

	// Free data.
	for (TInt i = 0; i < args.Count(); i++)
	{
		RBuf8& tmp = args[i];
		tmp.Close();
	}

	args.Close();

	return err;
}

void CLoggingServerCommandManager::HandleCommandL(RArray<RBuf8>& aArgs)
{
	//iLoggingServerServer->SendMessage(_L8("void CLoggingServerCommandManager::HandleCommandL()") );
	RBuf8 arg;
	TInt len = iCommandBuffer.Length();
	arg.Create(len);

	// Create buffer.
	// Parse parameters.
	TInt argIndex = 0;
	for (TInt i = 0; i < iCommandBuffer.Length(); i++, argIndex++)
	{
		if (iCommandBuffer[i] == ' ')
		{
			AddToList(aArgs, arg);
			TInt argIndex = 0;
		}
		else
		{
			arg.Append(iCommandBuffer[i]);
		}
	}

	// Add last param
	if (arg.Length() > 0)
		AddToList(aArgs, arg);
	arg.Close();

	if( aArgs.Count() == 0 ) return;

	// Connect to the file server
	RFs fs;
	fs.Connect();
	CleanupClosePushL(fs);

	// First param has the command type
	if (aArgs[0].Compare(KCmdList) == 0 || aArgs[0].Compare(KCmdHelp) == 0)
	{
		HandleCmdListL();
	}
	else if (aArgs[0].Compare(KCmdLs) == 0 || aArgs[0].Compare(KCmdDir) == 0)
	{
		HandleCmdLsL(aArgs, fs);
	}
	else if (aArgs[0].Compare(KCmdCopy) == 0)
	{
		HandleCmdCopyFilesL(aArgs, fs);
	}
	else if (aArgs[0].Compare(KCmdExec) == 0)
	{
		HandleCmdExecL(aArgs, fs);
	}
	else if (aArgs[0].Compare(KCmdKill) == 0)
	{
		HandleCmdKillAndFindL(aArgs, ETrue);
	}
	else if (aArgs[0].Compare(KCmdListProcess) == 0)
	{
		HandleCmdKillAndFindL(aArgs, EFalse);
	}
	else if (aArgs[0].Compare(KCmdPut) == 0)
	{
		HandleCmdPutL(aArgs, fs);
	}
	else if (aArgs[0].Compare(KCmdGet) == 0)
	{
		HandleCmdGetL(aArgs, fs);
	}
	else
	{
		iLoggingServerServer->SendMessage(KStrUnknownCommand);
	}

	CleanupStack::PopAndDestroy(&fs);
}


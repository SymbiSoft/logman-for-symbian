#include <e32debug.h>
#include <BADESCA.H>
#include <BAUTILS.H>

#include "LogManServer_CommandManager.h"
#include "LogManServer_Server.h"

#include "loggingdefs.h"

CLoggingServerCommandManager::CLoggingServerCommandManager(
		CLoggingServerServer* aLoggingServerServer) :
	CActive(0), iLoggingServerServer(aLoggingServerServer)
{

}

void CLoggingServerCommandManager::ConstructL()
{
	CActiveScheduler::Add(this);
	iTimer.CreateLocal();
	iCommandBuffer.Create( 512);
}

void CLoggingServerCommandManager::DoCancel()
{
	iTimer.Cancel();
}

void CLoggingServerCommandManager::Start()
{
	Cancel();
	iTimer.After(iStatus, TTimeIntervalMicroSeconds32( 100) );
	SetActive();
}

void CLoggingServerCommandManager::RunL()
{
	//PRINTF( "void CLoggingServerCommandManager::RunL()" )
	//#ifdef  __WINS__
	//User::InfoPrint( _L( "CLoggingServerCommandManager" ) );
	//#endif

	//TBuf8<128> tmp;
	//tmp.Copy(_L8("CLoggingServerCommandManager RunL") );
	//iLoggingServerServer->SendMessage(tmp);
	if (iLoggingServerServer->IsClosing())
	{
		PRINTF("CLoggingServerCommandManager stopping")
		//Cancel();
		return;
	}
	else
	{
		TRequestStatus readstatus = KRequestPending;
		RComm& comm = iLoggingServerServer->iSerialComm;
		TBuf8<1> buf;

		//iLoggingServerServer->SendMessage(_L8( "SHELL receive\n") );

		//if (comm.QueryReceiveBuffer() > 0)
		//{
		//iLoggingServerServer->SendMessage(_L8( "QueryReceiveBuffer\n") );
		do
		{
			comm.Read(readstatus, 100, buf, 1);
			User::WaitForRequest(readstatus);

			//TBuf8<10> tmp;
			//tmp.Format( _L8("readstatus:%d"), readstatus.Int() );
			//iLoggingServerServer->SendMessage(tmp);

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
					//iLoggingServerServer->SendMessage(_L8( "Not line break\n") );
					iCommandBuffer.Append(buf[0]);
				}
			}
		} while (readstatus == KErrNone// && comm.QueryReceiveBuffer() > 0
				&& !iLoggingServerServer->IsClosing() );
		//}
		//iLoggingServerServer->SendMessage(_L8( "exit SHELL receive\n") );
		// Restart timer
		Start();
	}

}

CLoggingServerCommandManager::~CLoggingServerCommandManager()
{
	Cancel();
	iCommandBuffer.Close();
}

#include <f32file.h>


_LIT8(KStrUnknownCommand, "Unknown command. Try 'list' or 'help'.\n");
_LIT8(KStrNotEnoughParameters, "Not enough parameters\n");

// Command list
_LIT8(KStrNewLine, "\n");
_LIT8(KCmdList, "list");
_LIT8(KCmdCopy, "cp");
_LIT8(KCmdLs, "ls");
_LIT8(KCmdDir, "dir");
const TChar KCharDash = '\\';
const TChar KDoubleColon = ':';

TInt CLoggingServerCommandManager::HandleCmdListL()
{
	//PRINTF( "void CLoggingServerCommandManager::HandleCmdList()" )
	// TODO: Generate
	/**[[[cog	
	 ]]]
	 */
	///[[[end]]]

	iLoggingServerServer->SendMessage(_L8("list   - List possible commands\n") );
	iLoggingServerServer->SendMessage(_L8("dir/ls - List contents of a directory\n") );
	iLoggingServerServer->SendMessage(_L8("cp     - Copy files\n") );
}

TInt CLoggingServerCommandManager::HandleCmdLsL(RArray<RBuf8>& parameters)
{
	//iLoggingServerServer->SendMessage(_L8("void CLoggingServerCommandManager::HandleCmdList()\n") );
	// TODO: Add current directory support
	if (parameters.Count() < 2)
	{
		iLoggingServerServer->SendMessage(KStrNotEnoughParameters);
		return KErrArgument;
	}

	RFs fileSession;
	RFile file;

	TInt i;
	TFullName totalPath;
	TMessageBuffer fileName;
	TInt err;
	// Convert to unicode
	RBuf path;
	err = path.Create(parameters[1].Length() + 1);
	if (err != KErrNone)
		return err;

	CleanupClosePushL(path);

	path.Copy(parameters[1]);

	//
	// Connect to the file server
	//
	fileSession.Connect();
	CleanupClosePushL(fileSession);

	TBool isDir;
	BaflUtils::IsFolder(fileSession, path, isDir);

	if ( (isDir || path[path.Length() - 1 ] == KDoubleColon ) && path[path.Length() - 1 ] != KCharDash)
	{
		// Add dash or get dir does not list folder
		path.Append(KCharDash);
	}

	CDir* dirList;
	User::LeaveIfError(fileSession.GetDir(path, KEntryAttMaskSupported,
			ESortByName, dirList));
	CleanupStack::PushL(dirList);

	// Statistics
	TInt total_size = 0;
	TInt files = 0;
	TInt dirs = 0;

	_LIT(KFmtDirEntry, "%02d.%02d.%02d  %02d:%02d  %14S %S\n");
	TBuf<25> sizeOrDir;
	_LIT(KStrDir, "<DIR>         ");

	// Send directory path.
	{
		_LIT(KFmtDirInfo, "Directory of %S");
		TMessageBuffer tmp;
		tmp.Format(KFmtDirInfo, &path);
		iLoggingServerServer->SendMessage(tmp);
		iLoggingServerServer->SendMessage(KStrNewLine);
		iLoggingServerServer->SendMessage(KStrNewLine);
	}

	for (i=0; i<dirList->Count(); i++)
	{
		const TEntry& entry = (*dirList)[i];
		// Get date and time of modification
		TDateTime date = entry.iModified.DateTime();
		fileName = entry.iName;

		if (entry.IsDir() )
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
		line.Create( 35 + sizeOrDir.Length() + fileName.Length() );
		// No leaving here
		line.Format(KFmtDirEntry, date.Day(), date.Month(), date.Year(),
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

	CleanupStack::PopAndDestroy(3); // path ,fileSession, dirlist

	return KErrNone;
}

TInt CLoggingServerCommandManager::HandleCmdCopyFilesL(RArray<RBuf8>& parameters)
{
	//iLoggingServerServer->SendMessage(_L8("void CLoggingServerCommandManager::HandleCmdList()\n") );
	// TODO: Add current directory support
	if (parameters.Count() < 3)
	{
		iLoggingServerServer->SendMessage(KStrNotEnoughParameters);
		return KErrArgument;
	}

	RFs fileSession;
	RFile file;

	TInt i;
	TFullName totalPath;
	TMessageBuffer fileName;
	TInt err;

	// Convert to unicode
	// Get source path
	RBuf path_source;
	User::LeaveIfError(path_source.Create(parameters[1].Length() + 1) );
	CleanupClosePushL(path_source);
	path_source.Copy(parameters[1]);

	// Get target path
	RBuf path_target;
	User::LeaveIfError(path_target.Create(parameters[1].Length() + 1) );
	CleanupClosePushL(path_target);
	path_target.Copy(parameters[2]);

	// File server
	fileSession.Connect();
	CleanupClosePushL(fileSession);

	// Are we copying files from folder?
	TBool isDir;
	BaflUtils::IsFolder(fileSession, path_source, isDir);
	if ( !isDir)
		isDir = (path_source[path_source.Length() - 1 ] == KDoubleColon );
	if ( (isDir || path_source[path_source.Length() - 1 ] == KDoubleColon ) && path_source[path_source.Length() - 1 ] != KCharDash)
	{
		// Add dash or get dir does not list folder
		path_source.Append(KCharDash);
	}

	// Is target path folder?	
	BaflUtils::IsFolder(fileSession, path_target, isDir);
	if ( !isDir)
		isDir = (path_target[path_target.Length() - 1 ] == KDoubleColon );
	if (isDir && path_target[path_target.Length() - 1 ] != KCharDash)
	{
		// Add dash 
		path_target.Append(KCharDash);
	}

	CDir* dirList;
	User::LeaveIfError(fileSession.GetDir(path_source, KEntryAttMaskSupported,
			ESortByName, dirList));
	CleanupStack::PushL(dirList);
	TInt copied = 0;

	for (i=0; i<dirList->Count(); i++)
	{
		const TEntry& entry = (*dirList)[i];

		fileName = entry.iName;
		if ( !entry.IsDir() )
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

			// Do the copying
			err = BaflUtils::CopyFile(fileSession, path_source, full_target);
			if (err != KErrNone)
			{
				_LIT(KFmtCopyErr, "Failed to copy '%S', err:%d\n");
				TMessageBuffer tmp;
				tmp.Format(KFmtCopyErr, &fileName, err);
				iLoggingServerServer->SendMessage(tmp);
			}
			else
			{
				copied++;
			}
			full_target.Close();

		}
	}

	// Send statistics
	{
		_LIT(KFmtStats, "%d files copied.");
		TMessageBuffer tmp;
		tmp.Format(KFmtStats, copied);
		iLoggingServerServer->SendMessage(tmp);
	}

	//iLoggingServerServer->SendMessage(fileName);

	CleanupStack::PopAndDestroy(4); // path_source, path_target, fileSession, dirlist

	return KErrNone;
}


TInt CLoggingServerCommandManager::HandleCommand()
{
	RArray<RBuf8> args;
	
	TRAPD(err, HandleCommandL( args ) );
		
	// Free data.
	
	for( TInt i = 0; i < args.Count(); i++)
	{
		RBuf8& tmp = args[i];
		tmp.Close();
	}
	
	args.Close();
	
	return err;
}

void CLoggingServerCommandManager::HandleCommandL( RArray<RBuf8>& aArgs )
{
	//User::InfoPrint( _L("Handle command") );
	//iLoggingServerServer->SendMessage(_L8("void CLoggingServerCommandManager::HandleCommandL()") );
	
	RBuf8 arg;
	TInt len = iCommandBuffer.Length();
	arg.Create(len);

	// Create buffer.
	// Parse parameters.
	//TBool getArgs = EFalse;
	TInt argIndex = 0;
	for (TInt i = 0; i < iCommandBuffer.Length(); i++, argIndex++)
	{
		if (iCommandBuffer[i] == ' ')
		{
			//iLoggingServerServer->SendMessage(arg);

			// Copy
			RBuf8 tmp;
			// This should be Closed when closing the RArray
			tmp.Create(arg.Length() );
			tmp.Copy(arg);
			aArgs.Append(tmp);
			//tmp.Close();
			//getArgs = ETrue;
			TInt argIndex = 0;
			arg.Delete( 0, arg.Length() );
		}
		else
		{
			arg.Append(iCommandBuffer[i]);
		}
	}

	// Add last param
	{
		// Copy
		RBuf8 tmp;
		// This should be Closed when closing the RArray
		tmp.Create(arg.Length() );
		tmp.Copy(arg);
		aArgs.Append(tmp);
		//tmp.Close();
		arg.Close();
	}

	//iLoggingServerServer->SendMessage(args[0]);
	// First param has the command type	
	if ( aArgs[0].Compare(KCmdList) == 0)
	{
		HandleCmdListL();
	}
	else if (aArgs[0].Compare(KCmdLs) == 0 || aArgs[0].Compare(KCmdDir) == 0)
	{
		HandleCmdLsL(aArgs);
	}
	else if (aArgs[0].Compare(KCmdCopy) == 0)
	{
		HandleCmdCopyFilesL(aArgs);
	}
	else
	{
		iLoggingServerServer->SendMessage(KStrUnknownCommand);
	}	
}


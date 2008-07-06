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
				
				if (readstatus == KErrNone)
				{
					// Echo back
					iLoggingServerServer->SendMessage(buf);
					TBuf8<10> tmp;
					//tmp.Format( _L8("%d"), (TInt)buf[0]);
					//iLoggingServerServer->SendMessage(tmp);
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
TInt CLoggingServerCommandManager::HandleCommand()
{
	TRAPD(err, HandleCommandL() );
	return err;
}

_LIT8(KStrUnknownCommand, "Unknown command. Try 'list' or 'help'.\n");
_LIT8(KStrNotEnoughParameters, "Not enough parameters\n");

// Command list
_LIT8(KStrNewLine, "\n");
_LIT8(KCmdList, "list");
_LIT8(KCmdLs, "ls");
_LIT8(KCmdDir, "dir");
const TChar KCharDash = '\\';

TInt CLoggingServerCommandManager::HandleCmdListL()
{
	PRINTF( "void CLoggingServerCommandManager::HandleCmdList()" )
	// TODO: Generate
	/**[[[cog	
	 ]]]
	 */
	///[[[end]]]

	iLoggingServerServer->SendMessage( _L8("list  - List possible commands\n") );
	iLoggingServerServer->SendMessage(_L8("dir/ls - List contents of a directory\n") );
}

TInt CLoggingServerCommandManager::HandleCmdLsL(RArray<RBuf8>& parameters)
{
	iLoggingServerServer->SendMessage( _L8( "void CLoggingServerCommandManager::HandleCmdList()\n" ) );
	// TODO: Add current directory support
	if( parameters.Count() < 2 )
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

	//_LIT(KDirName, "C:\\");
	//_LIT(KFileSpec, "C:\\*.*");

	//
	// Connect to the file server
	//
	fileSession.Connect();
	CleanupClosePushL(fileSession);

	TBool isDir;
	BaflUtils::IsFolder(fileSession, path, isDir);

	if (isDir && path[path.Length() - 1 ] != KCharDash)
	{
		// Add dash or get dir does not list folder
		path.Append(KCharDash);
	}
 
	CDir* dirList;
	User::LeaveIfError(fileSession.GetDir(path, KEntryAttMaskSupported,
			ESortByName, dirList));
	CleanupStack::PushL(dirList);
	
	for (i=0; i<dirList->Count(); i++)
	{
		fileName = (*dirList)[i].iName;
		
		iLoggingServerServer->SendMessage(fileName);
		iLoggingServerServer->SendMessage(KStrNewLine);
	}

	CleanupStack::PopAndDestroy(&path);
	//
	// Close the connection with the file server
	// and destroy dirList
	//
	CleanupStack::PopAndDestroy(2);
	
	return KErrNone;
}

void CLoggingServerCommandManager::HandleCommandL()
{
	//User::InfoPrint( _L("Handle command") );
	iLoggingServerServer->SendMessage( _L8( "void CLoggingServerCommandManager::HandleCommandL()" ) );

	RArray<RBuf8> args;
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
			iLoggingServerServer->SendMessage( arg );

			// Copy
			RBuf8 tmp;
			// This should be Closed when closing the RArray
			tmp.Create(arg.Length() );
			tmp.Copy(arg);
			args.Append(tmp);
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
	args.Append(arg);

	iLoggingServerServer->SendMessage( args[0] );
	// First param has the command type	
	if (args[0].Compare(KCmdList) == 0)
	{
		HandleCmdListL();
	}
	else if (args[0].Compare(KCmdLs) == 0 || args[0].Compare(KCmdDir) == 0)
	{
		HandleCmdLsL(args);
	}
	else
	{
		iLoggingServerServer->SendMessage(KStrUnknownCommand);
	}
	arg.Close();
}


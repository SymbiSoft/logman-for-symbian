
#include <e32debug.h>
#include <BADESCA.H>

#include "LogManServer_CommandManager.h"
#include "LogManServer_Server.h"

#include "loggingdefs.h"

CLoggingServerCommandManager::CLoggingServerCommandManager(CLoggingServerServer* aLoggingServerServer)
        : CActive(0),iLoggingServerServer(aLoggingServerServer)
{

}

void CLoggingServerCommandManager::ConstructL()
{
    CActiveScheduler::Add(this);

}


void CLoggingServerCommandManager::DoCancel()
{

}

void CLoggingServerCommandManager::RunL()
{
    PRINTF( "void CLoggingServerCommandManager::RunL()" )
    User::InfoPrint( _L("CLoggingServerCommandManager RunL") );
    if (iLoggingServerServer->IsClosing())
    {
        PRINTF("CLoggingServerCommandManager stopping")
    }
    else
    {
        //iLoggingServerServer->iSerialComm->Read
    }
}

CLoggingServerCommandManager::~CLoggingServerCommandManager()
{
    Cancel();
}


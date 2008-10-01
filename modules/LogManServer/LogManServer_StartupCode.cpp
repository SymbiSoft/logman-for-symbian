

#include "../Common/loggingdefs.h"

#include "LogManServer_Defines.h"
#include "LogManServer_Server.h"

GLDEF_C TInt E32Main()
  {
	__UHEAP_MARK;

	//create a new clanupstack

	CTrapCleanup* cleanup=CTrapCleanup::New();

	if(cleanup == NULL)
	  CLoggingServerServer::PanicServer(ECreateTrapCleanup);

    //create an active scheduler
    //if this fails, panic
    CActiveScheduler *LoggingScheduler = new CActiveScheduler;
	__ASSERT_ALWAYS(LoggingScheduler != NULL, CLoggingServerServer::PanicServer(EMainSchedulerError));

	//installs the active scheduler
	CActiveScheduler::Install(LoggingScheduler);

	//create an instance of the ServerObject
	//if this fails, panic
	CLoggingServerServer* LoggingServer;
	TRAPD(result,LoggingServer = CLoggingServerServer::NewL());
	PRINTF( "Server created");
	if(result != KErrNone)
		__ASSERT_ALWAYS(LoggingServer != NULL,CLoggingServerServer::PanicServer(ESvrCreateServer));


	//Adds the server with the specified name to the active scheduler,
	//and issues the first request for messages
    PRINTF( "Server starting");
	TInt err = LoggingServer->Start(KLoggingServer);
	PRINTF( "Done starting");
	if(err != KErrNone)
	  CLoggingServerServer::PanicServer(ESvrStartServer);

    // FIXME: If the client rendezvous after the server, the Client gets stuck.
    //        As a simple fix, wait to allow client rendezvous before the server
    PRINTF( "Wait");
    User::After(5000);
    PRINTF( "Rendezvous");
	//Let the calling process know that we're ready to deal with requests
	RProcess::Rendezvous(KErrNone);


	//starts the active scheduler,
    PRINTF( "CActiveScheduler::Start();");
	CActiveScheduler::Start();

	//we shouldn't get here until the active scheduler has been stoped
	// by issue a call to CActiveScheduler::Stop

    PRINTF( "Closing server");
	//free resources

	delete LoggingServer;
	delete LoggingScheduler;
	delete cleanup;

	__UHEAP_MARKEND;

	return (KErrNone);
}


/*Code generation globals
 [[[cog
 import cog
 import loggingserver
 import cpptemplates
 ]]]*/
//[[[end]]]
/// TODO: Parameterize connection mode ( Shared/Exclusive )
#include "LogManServer_Server.h"
#include "LogManServer_Session.h"
#include "LogManServer_MessageQueue.h"
#include "LogManServer_CommandManager.h"
#include <f32file.h>
#include <badesca.h>
#include "../Common/loggingdefs.h"
//\Epoc32\release\winscw\udeb\ecacm.csy
//\Epoc32\release\winscw\udeb\ecuart.csy
//\Epoc32\release\winscw\udeb\ircomm.csy
//\Epoc32\release\winscw\udeb\loopback.csy
//\Epoc32\release\winscw\udeb\sockcomm.csy
//\Epoc32\release\winscw\udeb\winscsy.csy
//\Epoc32\release\winscw\udeb\btcomm.csy
//\Epoc32\release\winscw\udeb\csy27010.csy
_LIT(KConnFormat, "%S::%d");
_LIT(LDD_NAME,"ECOMM");
#if defined (__WINS__)
_LIT(PDD_NAME,"ECDRV");
#else
_LIT(PDD_NAME,"EUART1");
#endif
//constructor
#ifdef __WINS__
#include <e32debug.h>
_LIT(KDefaultPortName, "LOOPBACK" );
const TInt KDefaultPortNumber = 0;
#else
_LIT(KDefaultPortName, "ACM" );
/// 0 is reserved by PC-Suite
const TInt KDefaultPortNumber = 1;
#endif

/// 5 Seconds
const TInt KDefaultWriteTimeout = 5000000;
/// Sent when connection has been established.
_LIT8( KSerialStartedMsg, "LogMan Service: Serial connection established.\n" );

CLoggingServerServer* CLoggingServerServer::NewLC()
{
	CLoggingServerServer* self = new (ELeave) CLoggingServerServer(
			EPriorityStandard);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CLoggingServerServer* CLoggingServerServer::NewL()
{
	CLoggingServerServer* self = CLoggingServerServer::NewLC();
	CleanupStack::Pop(self);
	return self;
}

CLoggingServerServer::CLoggingServerServer(
		CActive::TPriority aActiveObjectPriority) :
	CServer2(aActiveObjectPriority), iIsSerialConnected(EFalse), iBytesSent(0),
			iIsClosing(EFalse), iSocketEngine(NULL)
{
	iConnectionInfo.iPort = KDefaultPortNumber;
}

//destructor
CLoggingServerServer::~CLoggingServerServer()
{
	if (iMessageQueue)
	{
		delete iMessageQueue;
		iMessageQueue = NULL;
	}

	if (iIsSerialConnected)
	{
		//iSerialComm.NotifyDataAvailableCancel();
	}

	if( iSocketEngine )
	{
		iSocketEngine->Stop();
		delete iSocketEngine;
		iSocketEngine = NULL;
	}

	if (iCommandManager)
	{
		iCommandManager->Cancel();
		delete iCommandManager;
		iCommandManager = NULL;
	}

	PRINTF( "Closing" );
	this->DisconnectSerial();

}

TInt CLoggingServerServer::InitializeSocketServer()
{
	if( iSocketEngine ) return KErrNone; // Already initialized

	TRAPD( err,
			iSocketEngine = CSocketEngine::NewL( *(this->iCommandManager) );
			iSocketEngine->StartListeningL(0);
	);
	return err;
}

void CLoggingServerServer::ConstructL()
{
	// Make first sure that underlying interface is active
	//StartInterfaceL( 0 );

	// The Connect() allocate local address automatically if
	// not bind yet.
	//iDataSocket.Connect( iAddress, iStatus );


	// Creates message queue for asynchronous messaging
	this->iMessageQueue = new (ELeave) CLoggingServerMessageQueue(this);
	CleanupStack::PushL(this->iMessageQueue);
	this->iMessageQueue->ConstructL();

	this->iCommandManager = new (ELeave) CLoggingServerCommandManager(this);
	CleanupStack::PushL(this->iCommandManager);
	this->iCommandManager->ConstructL();

	CleanupStack::Pop(2);

	// These require CommDD capability
#ifdef __HAS_COMMDD__
	PRINTF( "ConnectSerial");
	PRINTF( "LoadPhysicalDevice");
	User::LoadPhysicalDevice (PDD_NAME); // step 1
	PRINTF( "LoadLogicalDevice");
	User::LoadLogicalDevice (LDD_NAME); // step 2
	PRINTF( "StartC32");
	StartC32 (); // step 3
#endif

	/// Connect to server
	TInt err = KErrNone;

	TFullName tmp;
	tmp.Copy(KDefaultPortName);

	err = iServer.Connect();
	User::LeaveIfError(err);

#ifdef __WINS__ // No need to load module on device
	// iServer must be connected before this.
	this->LoadModule( tmp );
#endif
	this->iConnectionInfo.iPortName.Copy(tmp);
}

TBool CLoggingServerServer::IsClosing()
{
	return this->iIsClosing;
}

CSession2* CLoggingServerServer::NewSessionL(const TVersion& aVersion,
		const RMessage2& /*aMessage*/) const
{

	TVersion v(KLogServMajorVersionNumber, KLogServMinorVersionNumber,
			KLogServBuildVersionNumber);

	// Check that the version is OK
	// Compares two version objects and returns true if the test version
	// is less than the current version.
	//if (!User::QueryVersionSupported(v,aVersion))
	//    User::Leave(KErrNotSupported);

	//the connect message is delivered via the RMessage2 object passed.
	//Create the session.
	CLoggingServerSession* newsession = new (ELeave) CLoggingServerSession();

	return newsession;

}

void CLoggingServerServer::PanicServer(TLogServPanic aPanic)
{
	_LIT(KTxtServerPanic,"Logging Server Panic");
	User::Panic(KTxtServerPanic, aPanic);
}

TInt CLoggingServerServer::SendMessage(const TDesC& aBuffer)
{
	PRINTF( "SendMessage16" );
	TPtrC8 ptr(reinterpret_cast<const TUint8*> (aBuffer.Ptr()),
			(aBuffer.Length() * 2));
	TInt err = this->SendMessage(ptr);
	PRINTF( "Exit SendMessage16" );
	return err;
}

void CLoggingServerServer::SendSocketMessage( const TDesC8& aBuffer )
{
	if( iSocketEngine ) {
		iSocketEngine->Write(aBuffer);
	}
}

TInt CLoggingServerServer::SendMessage(const TDesC8& aBuffer)
{

	SendSocketMessage( aBuffer );

	// TODO: Notify client.
	if (!IsSerialConnected())
		return KErrDisconnected;

	PRINTF( "exit SendMessage")
	PRINTF( "Total bytes sent: %u", iBytesSent )

	TRequestStatus sendstatus = KRequestPending;

	// Send with timeout. TODO: Create interface to configure the timeout.
	iSerialComm.Write(sendstatus, KDefaultWriteTimeout, aBuffer);

	// Wait
	User::WaitForRequest(sendstatus);

	if (sendstatus.Int() == KErrNone)
	{
		iBytesSent += aBuffer.Length();
	}
	// else TODO: Should we set IsSerialConnected to EFalse?
	return sendstatus.Int();
}

void CLoggingServerServer::AddAsyncMessageL(const TDesC8& aBuffer)
{
	PRINTF( "AddAsyncMessageL" );PRINTF( "Adding asynchronous message %S", &aBuffer );
	this->iMessageQueue->AppendL(const_cast<TDesC8&> (aBuffer));
	this->iMessageQueue->Start();
}

TInt CLoggingServerServer::LoadModule(TFullName& aModuleName)
{

	TInt err = iServer.LoadCommModule(aModuleName);
	if (err != KErrNone)
		return err;

	PRINTF( "Result:%d", err );

	return err;
}

TBool CLoggingServerServer::IsSerialConnected()
{

	//PRINTF( "CLoggingServerServer::IsSerialConnected()" );
	if (!iIsSerialConnected)
	{
		//PRINTF( "Not connected" );
		return EFalse;
	}

	/* // Nice, but does not work.
	 PRINTF( "CLoggingServerServer::IsSerialConnected()" );
	 TUint bits = iPort.Signals( KSignalDTR );//| KSignalDSR ); // Check output signals

	 PRINTF( "bits: %d", bits );

	 if( bits == 0 ) {
	 PRINTF( "No bits read. No connection to other end." );
	 return EFalse;
	 }
	 */
	PRINTF( "Connected." );
	return ETrue;

}

void CLoggingServerServer::DisconnectSerial()
{
	PRINTF( "CLoggingServerServer::DisconnectSerial()" );
	iSerialComm.Close();
	//iServer.Close();

	iIsSerialConnected = EFalse;
}

TInt CLoggingServerServer::ConnectSerial()
{
	PRINTF( "CLoggingServerServer::ConnectSerial()" );

	TInt err = KErrNone;

	TFullName connection_string;

	connection_string.Format(KConnFormat, &(this->iConnectionInfo.iPortName),
			this->iConnectionInfo.iPort);

	TCommConfig ConfigBuf;
	TCommConfigV01& Config = ConfigBuf();

	Config.iRate = EBps115200;
	Config.iDataBits = EData8;
	Config.iStopBits = EStop1;
	Config.iParity = EParityNone;
	Config.iHandshake = 0;
	Config.iTerminatorCount = 0;

	PRINTF( "Opening" );

	TCommAccess mode = ECommShared;
	TCommRole role = ECommRoleDTE;

	err = iSerialComm.Open(iServer, connection_string, mode, role);

	PRINTF( "Result:%d", err );
	if (err != KErrNone)
		return err;

	iSerialComm.SetConfig(ConfigBuf);

	iIsSerialConnected = ETrue;

	PRINTF( "ConnectSerial exit" );

	// Send initial message.
	TMessageBuffer8 tmp;
	tmp.Copy(KSerialStartedMsg);
	err = SendMessage(tmp);
	if (err != KErrNone)
	{
		// Don't try to write messages
		iIsSerialConnected = EFalse;
	}
	else
	{
		// Enable command handler
		//#ifdef  __WINS__
		//        User::InfoPrint(_L("Enable notify"));
		//#endif

		//iSerialComm.NotifyDataAvailableCancel();
		//iCommandManager->iStatus = KRequestPending;
		//iSerialComm.NotifyDataAvailable( iCommandManager->iStatus );
		//iCommandManager->SetActive();
		//iCommandManager->iStatus = KRequestPending;

		// Data notifier does not work with LOOPBACK so I can't test things on simulator.
		// Using simple timer instead.
		iCommandManager->Start();

	}
	return err;
}

TUint32 CLoggingServerServer::BytesSent()
{
	return this->iBytesSent;
}

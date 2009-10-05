// System
#include    <e32svr.h>
#include    <commdbconnpref.h>

// Local
#include    "LogManServer_SocketEngine.h"
#include    "LogManServer_SocketWriter.h"
#include    "LogManServer_SocketReader.h"
#include    "../LogManClient/types.h"
#include    "LogManServer_SocketEngineObserver.h"
#include    "LogManServer_SocketTimeOutTimer.h"


// ==== Constructs

CSocketEngine* CSocketEngine::NewL(
		MSocketEngineObserver& aObserver)
{
	CSocketEngine* self = CSocketEngine::NewLC(aObserver);
	CleanupStack::Pop(self);
	return self;
}

CSocketEngine* CSocketEngine::NewLC(
		MSocketEngineObserver& aObserver)
{
	CSocketEngine* self = new (ELeave) CSocketEngine( aObserver );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSocketEngine::CSocketEngine(MSocketEngineObserver& aObserver) :
	CActive(EPriorityStandard), iObserver(aObserver)
{
	CActiveScheduler::Add( this);
}

CSocketEngine::~CSocketEngine()
{
	Cancel();

	if (iWriter)
	{
		iWriter->Cancel();
		delete iWriter;
	}

	if (iReader)
	{
		iReader->Cancel();
		delete iReader;
	}
	if( iTimer )
	{
		// Should be cancelled by Cancel() before.
		delete iTimer;
		iTimer = NULL;
	}
	
	iListenSocket.Close();
	iDataSocket.Close();
	iConnection.Close();
	iSocketServer.Close();
}

void CSocketEngine::ConstructL()
{
	iState = ENotConnected;

	iTimer = CSocketTimeOutTimer::NewL( *this);

	// Open channel to Socket Server
	User::LeaveIfError(iSocketServer.Connect() );

	iReader = CSocketReader::NewL(iDataSocket, *this);
	iWriter = CSocketWriter::NewL(iDataSocket);
}

// ==== Constructs END

void CSocketEngine::DoCancel()
{
	iTimer->Cancel();

	// Cancel appropriate request to socket
	switch (iState)
	{
	case EConnecting:
		iDataSocket.CancelConnect();
		break;

	case EListening:
		iListenSocket.CancelAccept();
		break;

	default:
		break;
	}

	iConnection.Close();
}

void CSocketEngine::RunL()
{
	iTimer->Cancel();

	switch (iState)
	{
	case EConnecting:
		if (iStatus.Int() == KErrNone)
		{
			ChangeStateAndNotify(EConnected);
			Read();
		}
		else
		{
			ChangeStateAndNotify(ENotConnected);
		}
		break;

	case EListening:
	{
		iListenSocket.Close();
		if (iStatus.Int() == KErrNone)
		{
			ChangeStateAndNotify(EConnected);
			Read();
		}
		else
		{
			ChangeStateAndNotify(ENotConnected);
		}
	}
		break;

	case ETimedOut:
		if (iStatus.Int() == KErrNone)
		{
			iObserver.SocketState(ETimedOut);
			iState = ENotConnected;
		}
		else
		{
			ChangeStateAndNotify(ENotConnected);
		}
		break;

	default:
		iObserver.SocketState(iStatus.Int() );
	}
}

CSocketEngine::TSocketEngineState CSocketEngine::SocketState(void)
{
	return iState;
}

void CSocketEngine::ConnectL(const TUint32 aIapId,
		const TInetAddr& aIPAddress)
{
	if (iState != ENotConnected)
	{
		return;
	}

	// Make first sure that underlying interface is active
	StartInterfaceL(aIapId);

	// Open a TCP socket
	User::LeaveIfError(iDataSocket.Open(iSocketServer, KAfInet, KSockStream,
			KProtocolInetTcp, iConnection) );

	iAddress = aIPAddress;
	iAddress.SetPort(KSocketLoggingPort);

	// The Connect() allocate local address automatically if
	// not bind yet.
	iDataSocket.Connect(iAddress, iStatus);

	iState = EConnecting;
	SetActive();
	iTimer->After(KTimeOut);
}

void CSocketEngine::Write(const TDesC8& aTxt)
{
	if (iState == EConnected && !iWriter->IsActive() )
	{
		iWriter->Write(aTxt);
	}
}

void CSocketEngine::Read()
{
	if (iState == EConnected && !iReader->IsActive() )
	{
		iReader->Read();
	}
}

void CSocketEngine::Stop()
{
	switch (iState)
	{
	case EConnected:
	{
		iWriter->Cancel();
		iReader->Cancel();
		TRequestStatus status;
		iDataSocket.Shutdown(RSocket::EImmediate, status);
		User::WaitForRequest(status);
		iConnection.Close();
		ChangeStateAndNotify(ENotConnected);
	}
		break;
	case EListening:
	case EConnecting:
		Cancel();
		break;
	default:
		break;
	}
}

TInetAddr& CSocketEngine::StartListeningL(const TUint32 aIapId)
{
	StartInterfaceL(aIapId);
	ResolveLocalIPAddressL();
	iAddress.SetPort(KSocketLoggingPort);
	User::LeaveIfError(iListenSocket.Open(iSocketServer, KAfInet, KSockStream,
			KProtocolInetTcp) );
	User::LeaveIfError(iDataSocket.Open(iSocketServer) );
	User::LeaveIfError(iListenSocket.Bind(iAddress) );

	User::LeaveIfError(iListenSocket.Listen( 1) );

	iListenSocket.Accept(iDataSocket, iStatus);
	iState = EListening;
	SetActive();

	return iAddress;
}


void CSocketEngine::StartInterfaceL(const TUint32 aIapId)
{
	if ( !iConnection.SubSessionHandle() )
	{
		User::LeaveIfError(iConnection.Open(iSocketServer) );
	}

	TCommDbConnPref prefs;

	prefs.SetDirection(ECommDbConnectionDirectionOutgoing);

	// Show the selection dialog on device only. Faster to develop on emulator.
#ifdef __WINS__
	prefs.SetIapId( aIapId );
	prefs.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
#endif
	User::LeaveIfError(iConnection.Start(prefs) );
}

void CSocketEngine::ResolveLocalIPAddressL()
{
	RHostResolver hostResolver;
	TNameEntry entry;

	User::LeaveIfError(hostResolver.Open(iSocketServer, KAfInet,
			KProtocolInetTcp, iConnection));
	CleanupClosePushL(hostResolver);

	User::LeaveIfError(hostResolver.GetByName(TPtrC(), entry) );
	if ( !TInetAddr::Cast( entry().iAddr ).IsWildAddr() )
	{
		iAddress = TInetAddr::Cast(entry().iAddr);
	}

	CleanupStack::PopAndDestroy(); // hostResolver
}

void CSocketEngine::TimerExpired()
{
	Cancel();
	iState = ETimedOut;

	TRequestStatus* status = &iStatus;
	SetActive();
	User::RequestComplete(status, KErrNone);
}

void CSocketEngine::MessageReceived(TDesC8& aBuffer)
{
	iObserver.SocketData(aBuffer);
}

void CSocketEngine::ErrorInReading(TInt aError)
{
	TRequestStatus* status = &iStatus;
	SetActive();
	User::RequestComplete(status, aError);
}


void CSocketEngine::WriteDone()
{
	// Nothing to do
}

void CSocketEngine::ErrorInWriting(TInt aError)
{
	TRequestStatus* status = &iStatus;
	SetActive();
	User::RequestComplete(status, aError);
}

void CSocketEngine::ChangeStateAndNotify(TSocketEngineState aNewState)
{
	iState = aNewState;
	iObserver.SocketState(iState);
}

// End of file

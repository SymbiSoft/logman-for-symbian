
#include    <in_sock.h>
#include    "LogManServer_SocketWriter.h"

// ==== Constructs
CSocketWriter* CSocketWriter::NewL(RSocket& aSocket)
{
	CSocketWriter* self = NewLC(aSocket);
	CleanupStack::Pop(self);
	return self;
}

CSocketWriter* CSocketWriter::NewLC(RSocket& aSocket)
{
	CSocketWriter* self = new (ELeave) CSocketWriter( aSocket );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}

CSocketWriter::CSocketWriter(RSocket& aSocket) :
	CActive(CActive::EPriorityStandard), iSocket(aSocket)
{
	CActiveScheduler::Add(this);
}

CSocketWriter::~CSocketWriter()
{
	Cancel();
	delete iBuffer;
}

void CSocketWriter::ConstructL()
{
}
// ==== Constructs END


void CSocketWriter::DoCancel()
{
	iSocket.CancelWrite();
}

void CSocketWriter::RunL()
{
	if (iBuffer)
	{
		delete iBuffer;
		iBuffer = NULL;
	}

	if (iStatus.Int() == KErrNone)
	{

	}
	else
	{

	}
}

// Copy the parameter to the member variable and start synchronous/asynchronous writing.
void CSocketWriter::Write(const TDesC8& aBuffer)
{
	if (iBuffer)
	{
		delete iBuffer;
		iBuffer = NULL;
	}

	TRAPD(err, iBuffer = aBuffer.AllocL() );

	if ( !IsActive() && err == KErrNone)
	{
		iSocket.Write( *iBuffer, iStatus);
		User::WaitForRequest(iStatus);
		// TODO: If async, call set active instead.
		//SetActive();
	}
}


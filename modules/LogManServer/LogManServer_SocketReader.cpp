#include    <in_sock.h>
#include    <e32svr.h>

#include    "LogManServer_SocketReader.h"
#include    "LogManServer_SocketReaderNotifier.h"

CSocketReader* CSocketReader::NewL(RSocket& aSocket,
		MSocketReaderNotifier& aNotifier)
{
	CSocketReader* self = NewLC(aSocket, aNotifier);
	CleanupStack::Pop(self);
	return self;
}

CSocketReader* CSocketReader::NewLC(RSocket& aSocket,
		MSocketReaderNotifier & aNotifier)
{
	CSocketReader* self = new (ELeave) CSocketReader(aSocket,
			aNotifier);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
}


CSocketReader::CSocketReader(RSocket& aSocket,
		MSocketReaderNotifier& aNotifier) :
	CActive(CActive::EPriorityStandard), iNotifier(aNotifier), iSocket(aSocket)
{
	CActiveScheduler::Add(this);
}

CSocketReader::~CSocketReader()
{
	Cancel();
}

void CSocketReader::ConstructL()
{
}

void CSocketReader::DoCancel()
{
	iSocket.CancelRead();
}

void CSocketReader::RunL()
{
	if (iStatus.Int() == KErrNone)
	{
		iMessage.Append(iBuffer);
		iBuffer.Zero();
		if (iMessage.Length() > 0)
		{
			iNotifier.MessageReceived(iMessage);
			iMessage.Zero();
		}
		Read();
	}
	else
	{
		iMessage.FillZ();
		iMessage.Zero();
		iNotifier.ErrorInReading(iStatus.Int());
	}
}

void CSocketReader::Read()
{
	if (!IsActive())
	{
		iSocket.Read(iBuffer, iStatus);
		SetActive();
	}
}


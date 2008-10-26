#include    "LogManServer_SocketTimeOutTimer.h"
#include    "LogManServer_SocketTimeOutNotify.h"

// ==== Constructs

CSocketTimeOutTimer* CSocketTimeOutTimer::NewL(
		MSocketTimeOutNotify& aTimeOutNotify)
{
	CSocketTimeOutTimer* self = new (ELeave) CSocketTimeOutTimer(aTimeOutNotify);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
}

void CSocketTimeOutTimer::ConstructL()
{
	CTimer::ConstructL();
	CActiveScheduler::Add(this);
}

CSocketTimeOutTimer::~CSocketTimeOutTimer()
{
	Cancel();
}

// ==== Constructs END

CSocketTimeOutTimer::CSocketTimeOutTimer(MSocketTimeOutNotify& aTimeOutNotify) :
	CTimer(EPriorityStandard), iNotifier(aTimeOutNotify)
{
}

void CSocketTimeOutTimer::RunL()
{
	// Tell the notifier that times out.
	iNotifier.TimerExpired();
}


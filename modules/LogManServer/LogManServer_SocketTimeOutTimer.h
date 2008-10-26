#ifndef CSOCKET_TIMEOUTIMER_H
#define CSOCKET_TIMEOUTIMER_H

#include    <e32base.h>

const TInt KTimeOutInSeconds(30);
const TInt KTimeOut(KTimeOutInSeconds * 1000000);

// Forward declarations
class MSocketTimeOutNotify;

/**
 * Implements timeout timer.
 */
class CSocketTimeOutTimer: public CTimer
{
public:

	/**
	 * Static constructor
	 * @param aTimeOutNotify The reference to the timeout notifier.
	 * @return A new instance of CSocketTimeOutTimer
	 */
	static CSocketTimeOutTimer* NewL(MSocketTimeOutNotify& aTimeOutNotify);

	/**
	 * Destructor.
	 */
	~CSocketTimeOutTimer();

protected:

	/**
	 * C++ default constructor.
	 * @param aTimeOutNotify The reference to the timeout notifier.
	 */
	CSocketTimeOutTimer(MSocketTimeOutNotify& aTimeOutNotify);

	/**
	 * Symbian 2nd phase constructor that might leave.
	 */
	void ConstructL();

private:
	// From base classes
	/**
	 * From CActive
	 */
	virtual void RunL();

private:
	// Member variables

	// The reference to the notifier
	MSocketTimeOutNotify& iNotifier;
};

#endif // CSOCKET_TIMEOUTIMER_H

#ifndef MSOCKET_TIMEOUTNOTIFY_H
#define MSOCKET_TIMEOUTNOTIFY_H

/**
 * Interface for notifing timer's timeout.
 */
class MSocketTimeOutNotify
{
public:
	virtual void TimerExpired() = 0;
};

#endif // MSOCKET_TIMEOUTNOTIFY_H

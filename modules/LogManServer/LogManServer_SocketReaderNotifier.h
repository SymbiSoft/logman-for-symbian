#ifndef __MSOCKETREADERNOTIFIER_H
#define __MSOCKETREADERNOTIFIER_H

/**
 * The socket reader notifier inteface.
 * Notifies about events in reader.
 */
class MSocketReaderNotifier
{
public:

	/**
	 * Called when the message has read from the socket.
	 * @param aBuffer Contains the readed buffer
	 */
	virtual void MessageReceived(TDesC8& aBuffer) = 0;

	/**
	 * Called if error occurs in reading from the socket.
	 * @param aError The error code
	 */
	virtual void ErrorInReading(TInt aError) = 0;
};

#endif // __MSOCKETREADERNOTIFIER_H

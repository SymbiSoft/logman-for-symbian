#ifndef __MSOCKETWRITERNOTIFIER_H
#define __MSOCKETWRITERNOTIFIER_H

/**
 * Interface for notifying about socket writer events.
 */
class MSocketWriterNotifier
{
public:

	/**
	 * Called when the message has been written to the socket.
	 */
	virtual void WriteDone() = 0;

	/**
	 * Called when the error has occured in writing.
	 */
	virtual void ErrorInWriting(TInt aError) = 0;
};
#endif // __MSOCKETWRITERNOTIFIER_H


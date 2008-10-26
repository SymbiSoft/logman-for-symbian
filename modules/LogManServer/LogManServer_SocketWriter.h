#ifndef __CSOCKETWRITER_H__
#define __CSOCKETWRITER_H__

#include <e32base.h>

// FORWARD DECLARATIONS
class RSocket;

/**
 * CSocketWriter
 * Writes data to the connected socket.
 */
class CSocketWriter: public CActive
{
public:

	/**
	 * NewL
	 * Static constructor
	 * @param aSocket The reference to the socket
	 * @param aNotifier The reference to the writer notifier
	 */
	static CSocketWriter* NewL(RSocket& aSocket);

	/**
	 * NewLC
	 * Static constructor. On return the instance is left to the CleanupStack.
	 * @param aSocket The reference to the socket
	 * @param aNotifier The reference to the writer notifier
	 */
	static CSocketWriter* NewLC(RSocket& aSocket);

	/**
	 * ~CSocketWriter
	 * Destructor
	 */
	~CSocketWriter();

public:

	/**
	 * Write
	 * Asynchronous function.
	 * Starts the asynchronous writing.
	 */
	void Write(const TDesC8& aBuffer);

protected:
	// from CActive

	/**
	 * From CActive
	 */
	void DoCancel();

	/**
	 * From CActive
	 */
	void RunL();

private:

	/**
	 * CSocketWriter
	 * Constructor
	 * @param aSocket The reference to the socket
	 */
	CSocketWriter(RSocket& aSocket);

	/**
	 * ConstructL
	 * Symbian 2nd phase constructor that might leave
	 */
	void ConstructL();

private:
	// Ref: Data socket
	RSocket& iSocket;

	// The write buffer
	HBufC8* iBuffer;
};

#endif // __CSOCKETWRITER_H__

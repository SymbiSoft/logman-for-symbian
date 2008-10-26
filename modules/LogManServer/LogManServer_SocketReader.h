#ifndef __CSOCKET_SOCKETREADER_H__
#define __CSOCKET_SOCKETREADER_H__

#include <e32base.h>

// FORWARD DECLARATIONS
class MSocketReaderNotifier;
class RSocket;

/**
 * CSocketReader
 * Reads data from the connected socket.
 */
class CSocketReader: public CActive
{
public:

	/**
	 * NewL
	 * Static constructor
	 * @param aSocket The reference to the socket
	 * @param aNotifier The reference to the reader notifier
	 */
	static CSocketReader* NewL(RSocket& aSocket,
			MSocketReaderNotifier& aNotifier);

	/**
	 * NewLC
	 * Static constructor. On return the instance is left to the CleanupStack.
	 * @param aSocket The reference to the socket
	 * @param aNotifier The reference to the reader notifier
	 */
	static CSocketReader* NewLC(RSocket& aSocket,
			MSocketReaderNotifier& aNotifier);

	/**
	 * ~CSocketReader
	 * Destructor
	 */
	~CSocketReader();

public:

	/**
	 * Read
	 * Asynchronous function.
	 * Starts the asynchronous reading.
	 */
	void Read();

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
	 * CSocketReader
	 * Constructor
	 * @param aSocket The reference to the socket
	 * @param aNotifier The reference to the reader notifier
	 */
	CSocketReader(RSocket& aSocket, MSocketReaderNotifier& aNotifier);

	/**
	 * ConstructL
	 * Symbian 2nd phase constructor that might leave
	 */
	void ConstructL();

private:
	// Member variables
	MSocketReaderNotifier& iNotifier;
	// Ref: Data socket
	RSocket& iSocket;

	// Buffers
	TBuf8<1> iBuffer;
	TBuf8<256> iMessage;
};

#endif // __CSOCKET_SOCKETREADER_H__

#ifndef __CSOCKET_SOCKETENGINE_H__
#define __CSOCKET_SOCKETENGINE_H__

#include    <in_sock.h>     // SInetAddr
#include    "LogManServer_SocketReaderNotifier.h"
#include    "LogManServer_SocketTimeOutNotify.h"

// FORWARD DECLARATIONS
class MSocketEngineObserver;
//class MSocketReaderNotifier;
class CSocketReader;
class CSocketWriter;
class CSocketTimeOutTimer;

// CLASS DECLARATIONS
/**
 * Implements socket connections.
 */
class CSocketEngine: public CActive,
		public MSocketTimeOutNotify,
		public MSocketReaderNotifier
//public MSocketWriterNotifier
{
public:

	/**
	 * Static constructor
	 * @param aObserver A reference to the socket engine observer.
	 */
	static CSocketEngine* NewL(MSocketEngineObserver& aObserver);

	/**
	 * Static constructor. On return the instance is left to the CleanupStack
	 * @param aObserver A reference to the socket engine observer.
	 */
	static CSocketEngine* NewLC(MSocketEngineObserver& aObserver);

	/**
	 * Destructor
	 */
	~CSocketEngine();

public:
	// From base classes

	/**
	 * Connecting the socket to the aIPAddress.
	 * @param aIapId An iap id used for connection
	 * @param aIPAddress An ip address where to connect.
	 */
	void ConnectL(const TUint32 aIapId, const TInetAddr& aIPAddress);

	/**
	 * Writes to the socket.
	 * @param aTxt A text to write to the socket.
	 */
	void Write(const TDesC8& aTxt);

	/**
	 * Reads from the socket.
	 */
	void Read();

	/**
	 * Closing the socket.
	 */
	void Stop();

	/**
	 * Starts listening local game port
	 * @param aIapId The IAP id of the active accesspoint.
	 * @return A local address
	 */
	TInetAddr& StartListeningL(const TUint32 aIapId);

public:

	/**
	 * Socket engine state enumerations.
	 */
	enum TSocketEngineState
	{
		ENotConnected, EConnecting, EConnected, EListening, ETimedOut
	};
	TSocketEngineState SocketState(void);
	
protected:
	// From base classes

	// From MSocketTimeOutNotify
	/**
	 * From MSocketTimeOutNotify
	 * Destructor
	 */
	void TimerExpired();

	// From MSocketReaderNotifier

	/**
	 * From MSocketReaderNotifier
	 */
	void MessageReceived(TDesC8& aBuffer);

	/**
	 * From MSocketReaderNotifier
	 */
	void ErrorInReading(TInt aError);

	// From MSocketWriterNotifier

	/**
	 * From MSocketWriterNotifier
	 */
	void WriteDone();

	/**
	 * From MSocketWriterNotifier
	 */
	void ErrorInWriting(TInt aError);

protected:
	// From base classes

	/**
	 * From CActive
	 * Cancels any outstanding request.
	 */
	void DoCancel();

	/**
	 * From CActive
	 * Handles the completion of the active request.
	 */
	void RunL();

private:
	// New functions

	/**
	 * Changes the new state to iState and notifies observer.
	 * @param aNewState A new state of socket engine
	 */
	void ChangeStateAndNotify(TSocketEngineState aNewState);

	/**
	 * Activates PDP ctx of passed iap
	 * @param aIapId The IAP id of the activated accesspoint.
	 */
	void StartInterfaceL(const TUint32 aIapId);

	/**
	 * Gets the local IP address with host resolver.
	 * The address is set to the iAddress variable.
	 */
	void ResolveLocalIPAddressL();

private:

	CSocketEngine(MSocketEngineObserver& aObserver);
	void ConstructL();

private:
	// Member variables

	// The reference to the socket engine observer
	MSocketEngineObserver& iObserver;

	// Sockets
	RSocketServ iSocketServer;
	RConnection iConnection;
	RSocket iDataSocket;
	RSocket iListenSocket;

	//
	CSocketReader* iReader;
	CSocketWriter* iWriter;
	CSocketTimeOutTimer* iTimer;

	// Socket engine's state
	TSocketEngineState iState;

	// The remote address
	TInetAddr iAddress;
};

#endif // __CSOCKET_SOCKETENGINE_H__

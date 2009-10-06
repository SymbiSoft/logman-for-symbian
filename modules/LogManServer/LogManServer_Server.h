#ifndef __LoggingServerServer_h__
#define __LoggingServerServer_h__

#include <c32comm.h>
#include <e32base.h>
#include <in_sock.h>
#include <commdbconnpref.h>

#include "LogManServer_Session.h"
#include "LogManServer_Defines.h"
#include "../LogManClient/types.h"
//#include "LogManServer_SocketWriter.h"
#include "LogManServer_SocketEngine.h"

class RSocket;

class CLogManMessageQueue;
class RComm;
class RCommServ;
class CLogManSession;
class CLogManCommandManager;

/// Struct containing serial connection type strings and port number on server.
struct SConnectionInfo {
    /**Name of the port to open.*/
    TPortName iPortName;
    /**Port number to open.*/
    TInt iPort;
    /**Address of remote logging server */
    TBuf<20> iRemoteAddr;
};

/// Struct containing socket connection type info
struct SSocketConnectionInfo {
	TUint32 iApId;
    // The remote address
    TInetAddr   iAddress;


};
/// LogMan server class
/** Service handling serial connection and sending of messages through it. */
class CLogManServer : public CServer2
{
public:
    /// Destructor
    virtual ~CLogManServer();

    /// Symbian 2-phase constructor
    static CLogManServer* NewLC();

    /// Symbian 2-phase constructor
    static CLogManServer* NewL();

    /** Create new session with the server */
    CSession2* NewSessionL(const TVersion& aVersion,const RMessage2& /*aMessage*/) const;
    /** Panic LogMan service */
    static void PanicServer(TLogServPanic aPanic);

    /** Create socket server and start listening for connections
     * @return KErrNone or system-wide error code, if fails.
     */
    TInt InitializeSocketServer();
    TInt ConnectToServer();
    /**
     * Send message through socket. If there is no connection, fails silently.
     */
    void SendSocketMessage( const TDesC8& aBuffer );

    /**
      * Sends data through serial connection.
      * @return KErrNone or system-wide error code, if sending fails.
      */
    TInt SendMessage( const TDesC8& aBuffer );
    /**
      * Sends data through serial connection.
      * @return KErrNone or system-wide error code, if sending fails.
      */
    TInt SendMessage( const TDesC& aBuffer );

    /**
      * Addes message to a queue for asynchronous sending.
      * (The client does not wait for the completion of the sending of the message. )
      */
    void AddAsyncMessageL( const TDesC8& aBuffer );

    /**
      * Create serial connection.
      */
    TInt ConnectSerial();

    /** Close serial connection */
    void DisconnectSerial();

    /**
     * @return ETrue if the serial connection is active
     */
    TBool IsSerialConnected();
    TBool IsSocketConnected();
    
    /**
     * @return Total bytes sent by the server.
     */
    TUint32 BytesSent();

    /** @return ETrue if service is closing. */
    TBool IsClosing();

    /** Load given serial module */
    TInt LoadModule( TFullName& aModuleName );

    /** Starts asynchronous socket read */
    void ReadSocket();

public:
    /// Holds data about the connection
    SConnectionInfo iConnectionInfo;
    /// Communications server
    RCommServ iServer;

private:
    /// Constructor
    CLogManServer(CActive::TPriority aActiveObjectPriority);

    /// 2nd phase constructor
    void ConstructL();

private:

    /// Holds and sends the asynchronous messages
    CLogManMessageQueue* iMessageQueue;

    /// Handles commands sent from PC
    CLogManCommandManager* iCommandManager;

    CSocketEngine* iSocketEngine;

    /// Serial comms
    RComm iSerialComm;
    /// True if the connection is active.
    TBool iIsSerialConnected;
    /// How many bytes the server has sent.
    TUint32 iBytesSent;
    /// True if server is closing
    TBool iIsClosing;

    friend class CLogManSession;
    friend class CLogManCommandManager;
};

#endif


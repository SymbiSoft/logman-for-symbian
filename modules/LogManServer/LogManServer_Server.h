#ifndef __LoggingServerServer_h__
#define __LoggingServerServer_h__

#include <c32comm.h>
#include <e32base.h>

#include "LogManServer_Session.h"
#include "LogManServer_Defines.h"
#include "../LogManClient/types.h"

class CLoggingServerMessageQueue;
class RComm;
class RCommServ;
class CLoggingServerSession;
class CLoggingServerCommandManager;

/// Struct containing serial connection type strings and port number on server.
struct SConnectionInfo {
    /**Name of the port to open.*/
    TPortName iPortName;
    /**Port number to open.*/
    TInt iPort;
};
/// LogMan server class
/** Service handling serial connection and sending of messages through it. */
class CLoggingServerServer : public CServer2
{
public:
    /// Destructor
    virtual ~CLoggingServerServer();

    /// Symbian 2-phase constructor
    static CLoggingServerServer* NewLC();

    /// Symbian 2-phase constructor
    static CLoggingServerServer* NewL();

    /** Create new session with the server */
    CSession2* CLoggingServerServer::NewSessionL(const TVersion& aVersion,const RMessage2& /*aMessage*/) const;
    /** Panic LogMan service */
    static void CLoggingServerServer::PanicServer(TLogServPanic aPanic);

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
    void AddAsyncMessageL( TMessageBuffer8& aBuffer );

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

    /**
     * @return Total bytes sent by the server.
     */
    TUint32 BytesSent();

    /** @return ETrue if service is closing. */
    TBool IsClosing();

    /** Load given serial module */
    TInt LoadModule( TFullName& aModuleName );

public:
    /// Holds data about the connection
    SConnectionInfo iConnectionInfo;
    /// Communications server
    RCommServ iServer;

private:
    /// Constructor
    CLoggingServerServer(CActive::TPriority aActiveObjectPriority);

    /// 2nd phase constructor
    void ConstructL();

private:

    /// Holds and sends the asynchronous messages
    CLoggingServerMessageQueue* iMessageQueue;
    /// Handles commands sent from PC
    CLoggingServerCommandManager* iCommandManager;

    /// Serial comms
    RComm iSerialComm;
    /// True if the connection is active.
    TBool iIsSerialConnected;
    /// How many bytes the server has sent.
    TUint32 iBytesSent;
    /// True if server is closing
    TBool iIsClosing;

    friend class CLoggingServerSession;
    friend class CLoggingServerCommandManager;
};

#endif


#ifndef __LOGMAN_INCLUDED__
#define __LOGMAN_INCLUDED__

// Declare C API
typedef int BOOL;

typedef struct {
    void* cppInstance;
} TLogMan;

// Declare C++ API
#ifdef __cplusplus

#ifndef __LOGMAN_TYPES_H__
#include <logman/types.h>
#endif

#if !defined( __WINSCW__ )
	#define __NON_INSTRUMENT_FUNCTION__   __attribute__((__no_instrument_function__))
	#define PTRACE_OFF        __NON_INSTRUMENT_FUNCTION__
#else
	#define PTRACE_OFF
#endif

_LIT(KLoggingServerExe,"LogMan.exe");

/// Increase if panics with text formatting.
const TInt KMaxFormatBufferSize  = 1024;

///Client for LogMan service
class RLogMan : public RSessionBase
{
public:


    /** Construct RLogMan */
    IMPORT_C RLogMan();

    /** Destroy RLogMan */
    IMPORT_C ~RLogMan();

    /** Connect to LogMan service
     *  @return A system-wide error code.
     */
    IMPORT_C TInt Connect();

    /** @return RLogMan version. */
    IMPORT_C TVersion Version() const;

    /** Close connection to LogMan service */
    IMPORT_C void Close();

    /**
      * Stop the LogMan server process.
      */
    IMPORT_C void StopService();

    /**
     Create and start LogMan server's socket listening server.
     @return KErrNone, if successful.
             Otherwise a system-wide error code.
    */
    IMPORT_C TInt StartSocketServer( );
    IMPORT_C TInt ConnectSocketServer();
    
    /**
     Connect LogMan server's serial connection using current Port and PortName settings.
     @return KErrNone, if successful.
             KErrTimeout if connection is not made soon enough. Connect the USB
             cable between device andd PC first.
             Otherwise a system-wide error code.

    */
    IMPORT_C TInt ConnectSerial();    
    
    /** @return ETrue if server's serial connection has been opened */
    IMPORT_C TBool IsSerialConnected();
    IMPORT_C TConnectionStatus ConnectionStatus(void);

    /** @return The amount of bytes sent by the server */
    IMPORT_C TUint32 BytesSent();

    /**
     * Disconnect LogMan server's serial connection.
     * @return A system-wide error code.
    */
    IMPORT_C TInt DisconnectSerial();


    /**
     * Load given serial module on server
     * @param aModuleName Name of the module to load
     * @return KErrNone, if successful. Otherwise a system-wide error code.
     */
    IMPORT_C TInt LoadModule( TFullName& aModuleName );

    /** Log information about stack usage */
    IMPORT_C TInt StackInfo();

    /** Log information about heap usage */
    IMPORT_C TInt HeapInfo();

    /** Utility to log both stack and heap usage */
    IMPORT_C TInt MemoryInfo();

    /** Return ETrue if shell is enabled */
    IMPORT_C TBool ShellEnabled();
    
    /** Enable or disable shell access */
    IMPORT_C TInt SetShellEnabled(TBool aData);
    
    /**
    High-level logging interface for simple one-line logging.
    Handles client init, message sending and closing.
    <PRE>
    Examples:
        RLogMan::Log(_L("Hello world"));

        _LIT(KTest, "Hello" );
        RLogMan::Log(_L("%S world"), EFalse, &KTest );
    </PRE>
    @param aFmt Either 8-bit or 16-bit descriptor. See TDes::Format for information about formatting.
    @param aDoAsync True to send asynchronously
    @param ... List of format data
    */
    template<class TFmt>
    static PTRACE_OFF TInt Log( const TFmt& aFmt, TBool aDoAsync = EFalse, ... );

    /**
    Format data and send.
    <PRE>
    Example:
        RLogMan::Writef(_L("Hello world"));

        _LIT(KTest, "Hello" );
        RLogMan::Writef(_L("%S world"), EFalse, &KTest );
    </PRE>
    @param aFmt Either 8-bit or 16-bit descriptor. See TDes::Format for information about formatting.
    @param aDoAsync True to send asynchronously
    @param ... List of format data
    */
    template<class TFmt>
    TInt PTRACE_OFF Writef(const TFmt& aFmt, TBool aDoAsync = EFalse, ... );



    /** Send testing log to server for writing.
    <PRE>
    Example:
        RLogMan::Write(_L("Hello world"));
    </PRE>
    @param aString  Data to write
    @param aDoAsync True to write data asynchronously in server
    @return KErrNone or one of the system-wide error codes.
    */
    IMPORT_C TInt Write( const char* aString, TBool aDoAsync = EFalse );
    IMPORT_C TInt Write(const TDesC8& aString, TBool aDoAsync = EFalse );
    IMPORT_C TInt Write(const TDesC16& aString, TBool aDoAsync = EFalse );

    /** Formats and sends text to server. For formatting see TDesC::Format
    @param aFmt Formatting text
    @param aSize Size of the used text buffer. Panics User 24, if the size is exceeded.
    @param aDoAsync True to write data asynchronously in server
    @param aList A pointer to an argument list.
    @return KErrNone or one of the system-wide error codes.
    */
    IMPORT_C TInt WriteFormatList(const char* aFmt, TInt aSize, TBool aDoAsync, VA_LIST aList );
    IMPORT_C TInt WriteFormatList(const TDesC8& aFmt, TInt aSize, TBool aDoAsync, VA_LIST aList );
    IMPORT_C TInt WriteFormatList(const TDesC16& aFmt, TInt aSize, TBool aDoAsync, VA_LIST aList );

    /** Used internally.
     *  Split the data to suitable chunks and send using RawSend.
     * If length of the message exceeds length of KLogServMessageSize, the message is
     * split into smaller chunks and sent separately.
     * @param aString  Data to send
     * @param aDoAsync True to write data asynchronously in server
     */
    TInt SendInChunks(const TDesC8& aString, TBool aDoAsync );

    /**
    Send the data to server using fixed 8-bit buffer.
    Client must be connected before use.
    @param aString Text to send to server for logging.
    @param aDoAsync ETrue to send the message asynchronously.
    @return KErrNone or one of the system-wide error codes.
    */
    TInt RawSend(const TMessageBuffer8& aString, TBool aDoAsync = EFalse );

    /** Get name of the current port. */
    IMPORT_C TPortName PortName( );
    /** Set name of the port to open. */
    IMPORT_C TInt SetPortName( TPortName& aPortName );

    /** Get current port number. */
    IMPORT_C TInt Port( );
    /** Set port number to open. */
    IMPORT_C TInt SetPort( TInt& aPort );
    
    /** Get local or remote host address depending on socket connection type(listen\connect). */
	IMPORT_C TPortName Host();
	/** Set remote host address. */
	IMPORT_C TInt SetHost( TPortName& aPortName );
	

};

template<class TFmt> TInt RLogMan::Log( const TFmt& aFmt, TBool aDoAsync, ... )
{
	VA_LIST ap;
	VA_START( ap, aDoAsync );

	RLogMan logman;

	TInt result = logman.Connect();
	if ( result == KErrNone) {
		logman.WriteFormatList( aFmt, KMaxFormatBufferSize, aDoAsync, ap );
		logman.Close();
	}
	VA_END(ap);

	return result;
}

template<class TFmt>
TInt RLogMan::Writef(const TFmt& aFmt, TBool aDoAsync, ...)
{

	VA_LIST ap;
	VA_START( ap, aDoAsync );

	TInt result = this->WriteFormatList(aFmt, KMaxFormatBufferSize, aDoAsync, ap);

	VA_END( ap );

	return result;
}

#define _EXTERN extern "C"
#else
#define _EXTERN
#endif

_EXTERN BOOL LogMan_Log(const char * aString, BOOL aAsync, ...);
/** Log information about stack usage */
_EXTERN int LogMan_StackInfo(TLogMan * aLogMan);
/** Log information about heap usage */
_EXTERN int LogMan_HeapInfo(TLogMan * aLogMan);
/** Utility to log both stack and heap usage */
_EXTERN int LogMan_MemoryInfo(TLogMan * aLogMan);
_EXTERN TLogMan* LogMan_New(void);
_EXTERN BOOL LogMan_Connect(TLogMan *aLogMan);
#undef _EXTERN 



// __LOGMAN_INCLUDED__
#endif 


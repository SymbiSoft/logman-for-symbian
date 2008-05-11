
#include "..\Common\loggingdefs.h"

#include "LogMan.h"
#include "e32debug.h"
#include <C32COMM.H> //TPortName

EXPORT_C RLogMan::RLogMan()
{
}

EXPORT_C RLogMan::~RLogMan()
{
    PRINTF("~RLogMan");
    PRINTF("Close");
    this->Close();
    PRINTF("Exit ~RLogMan");
}

EXPORT_C void RLogMan::StopService()
{
    SendReceive(ELogServStop);
}

EXPORT_C TInt RLogMan::Connect()
{
    PRINTF( "Connecting" );

    RProcess logginServerProcess;
    TFindServer findMyServer( KLoggingServer );
    TFullName servername;

    //tests whether there is already an instance running of the current server
    //if so, just establish a session, else startup server process before establishing a session

    TInt result = findMyServer.Next( servername );
    if ( result == KErrNone )
    {
        result = CreateSession(KLoggingServer,Version(),KDefaultMessageSlots);
    }
    else
    {
        TRequestStatus status = KRequestPending;
        result = logginServerProcess.Create(KLoggingServerExe,KNoArgs);
        PRINTF( "logginServerProcess.Create: %d", result );
        if ( result != KErrNone )
        {
            return result;
        }
        //resume execution for the server process, since the server isn't started we will enter the main32
        PRINTF( "Resume" );
        logginServerProcess.Resume();
        //issue a request to the server, this wont complete until the server is properly setup
        PRINTF( "Rendezvous" );
        logginServerProcess.Rendezvous(status);
        // Wait for the request to complete
        PRINTF( "WaitForRequest" );
        User::WaitForRequest(status);
        result = status.Int();
        PRINTF( "WaitForRequest:%d", result );

        //if everything went fine, we are now ready to establish a session with the server
        if ( result == KErrNone)
        {
            PRINTF( "CreateSession");
            result = CreateSession(KLoggingServer,Version(),KDefaultMessageSlots);
            PRINTF( "CreateSession: %d", result);
        }
    }
    PRINTF( "Connection: %d", result );
    return result;
}

EXPORT_C TBool RLogMan::IsSerialConnected()
{
    PRINTF( "RLogMan::IsSerialConnected()" );
    TBool result;
    TPckgBuf<TBool> pckg;
    TIpcArgs args(&pckg);
    SendReceive(EIsSerialConnected, args);

    result = pckg();

    PRINTF( "RLogMan::IsSerialConnected() result:%d", result );
    return result;

}


EXPORT_C TUint32 RLogMan::BytesSent()
{
    PRINTF( "RLogMan::BytesSent()" );
    TBool result;
    TPckgBuf<TUint32> pckg;
    TIpcArgs args(&pckg);
    SendReceive(EGetSentBytes, args);

    result = pckg();

    PRINTF( "RLogMan::BytesSent() result:%u", result );
    return result;

}

EXPORT_C TInt RLogMan::ConnectSerial()
{
    TIpcArgs args;
    return SendReceive(ELogServConnect, args);
}

EXPORT_C TInt RLogMan::DisconnectSerial()
{
    TIpcArgs args;
    return SendReceive(ELogServDisconnect, args);
}


EXPORT_C void RLogMan::Close()
{
    RSessionBase::Close();
}

EXPORT_C TVersion RLogMan::Version(void) const
{
    return(TVersion(KLogServMajorVersionNumber,KLogServMinorVersionNumber,KLogServBuildVersionNumber));
}

EXPORT_C TInt RLogMan::LoadModule( TFullName& aModuleName )
{
    TPckgBuf<TInt> resultPckg;

    TIpcArgs args( &resultPckg, &aModuleName );
    SendReceive(ELoadModule, args);

    return resultPckg();

}

EXPORT_C TInt RLogMan::WriteFormatList(const TDesC16& aFmt, TInt aSize, TBool aDoAsync, VA_LIST aList )
{

    RBuf16 string;
    TInt result = KErrNone;

    result = string.Create( aSize );
    if ( result == KErrNone )
    {
        string.FormatList( aFmt, aList );
        result = this->Write( string, aDoAsync );
        string.Close();
    }
    return result;
}


EXPORT_C TInt RLogMan::WriteFormatList(const TDesC8& aFmt, TInt aSize, TBool aDoAsync, VA_LIST aList )
{

    RBuf8 string;
    TInt result = KErrNone;

    result = string.Create( aSize );
    if ( result == KErrNone )
    {
        string.FormatList( aFmt, aList );
        result = this->Write( string, aDoAsync );
        string.Close();
    }
    return result;
}

EXPORT_C TInt RLogMan::Write(const TDesC16& aString, TBool aDoAsync )
{
#ifdef __WINS__
    /// Using RDebug on emulator since it is a lot more convenient,
    /// but allows using the same logging system on device also.
    RDebug::Print( aString );
#endif
    TPtrC8 ptr(reinterpret_cast<const TUint8*>(aString.Ptr()),(aString.Length()*2));
    return this->SendInChunks( ptr, aDoAsync );
}

EXPORT_C TInt RLogMan::Write(const TDesC8& aString, TBool aDoAsync )
{
#ifdef __WINS__
    /// Using RDebug on emulator since it is a lot more convenient,
    /// but allows using the same logging system on device also.
    RBuf tmp;
    TInt result = tmp.Create( aString.Length() );
    if ( result == KErrNone )
    {
        tmp.Copy( aString );
        RDebug::Print( tmp );
        tmp.Close();
    }
    //RDebug::Printf( reinterpret_cast<const char*>( aString.Ptr() ) );
#endif
    return this->SendInChunks( aString, aDoAsync );
}

TInt RLogMan::SendInChunks(const TDesC8& string, TBool aDoAsync )
{

    TInt length      = string.Length();
    TInt result      = KErrNone;
    TInt chunk_start = 0;

    //Send too long string in smaller chunks.
    while ( chunk_start < length )
    {
        TInt chunk_size = Min( length - chunk_start, KLogServMessageSize );
        // Create buffer
        TMessageBuffer8 buf;
        TPtrC8 tmp = string.Mid( chunk_start, chunk_size );
        buf.Copy(tmp);
        // Send the data to server
        result = this->RawSend(tmp, aDoAsync);
        if (result != KErrNone) break;
        chunk_start += chunk_size;
    }

    return result;
}

TInt RLogMan::RawSend(const TMessageBuffer8& aString, TBool aDoAsync )
{

    TIpcArgs args(&aString, aDoAsync);

    return SendReceive(ELogServSendMessage, args);
}


EXPORT_C TPortName RLogMan::PortName( )
{

    TPortName result;
    TPckgBuf<TPortName> pckg;
    TIpcArgs args(&pckg);
    SendReceive(EGetPortName, args);

    result = pckg();
    return result;

}
EXPORT_C TInt RLogMan::Port( )
{

    TInt result;
    TPckgBuf<TInt> pckg;
    TIpcArgs args(&pckg);
    SendReceive(EGetPort, args);

    result = pckg();
    return result;

}
EXPORT_C TInt RLogMan::SetPortName( TPortName& aData )
{
    TIpcArgs args(&aData );
    TInt result = SendReceive(ESetPortName, args);
    return result;
}
EXPORT_C TInt RLogMan::SetPort( TInt& aData )
{
    TIpcArgs args(aData );
    TInt result = SendReceive(ESetPort, args);
    return result;
}

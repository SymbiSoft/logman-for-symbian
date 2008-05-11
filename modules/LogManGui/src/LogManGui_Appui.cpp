
// INCLUDE FILES
#include <avkon.hrh>
#include <aknnotewrappers.h>
#include <stringloader.h>
#include <LogManGui.rsg>
#include <f32file.h>
#include <s32file.h>


#include "LogManGui.hrh"
#include "LogManGui.pan"
#include "LogManGui_AppUi.h"
#include "LogManGui_AppView.h"

#include "LogManGui_Utils.h"

#include <LogMan.h>
#include <logmanutils.h>


void CLoggingServerGuiAppUi::HandleForegroundEventL (TBool aForeground)
{
    LOGMAN_SENDLOGF( "HandleForegroundEventL %d", aForeground )
    UpdateServiceInfoL( );
}

void CLoggingServerGuiAppUi::UpdateServiceInfoL( )
{

    // Check if LogMan service is running
    RProcess logginServerProcess;
    TFindServer findMyServer( KLoggingServer );
    TFullName servername;

    TInt result = findMyServer.Next( servername );
    if ( result != KErrNone )
    {
        iAppView->iLogManServiceInfo.iRunning = EFalse;
        iAppView->iLogManServiceInfo.iConnected = KErrUnknown;
        iAppView->iLogManServiceInfo.iPort = 0;
        iAppView->iLogManServiceInfo.iPortName.Copy( KEmptyStr );
        return;
    }

    RLogMan log;
    User::LeaveIfError( log.Connect() );
    CleanupClosePushL( log );

    if ( iAppView->iLogManServiceInfo.iConnected == KErrNone )
    {
        if ( !log.IsSerialConnected() )
        {
            iAppView->iLogManServiceInfo.iConnected = KErrDisconnected;
        }
    }

    iAppView->iLogManServiceInfo.iRunning = ETrue;
    iAppView->iLogManServiceInfo.iPort = log.Port();
    iAppView->iLogManServiceInfo.iPortName.Copy( log.PortName() );
    iAppView->iLogManServiceInfo.iBytesSent = log.BytesSent();
    CleanupStack::PopAndDestroy( &log );
}

void CLoggingServerGuiAppUi::ConnectSerial(RLogMan& aLogMan)
{


    // Connect
    TInt err = aLogMan.ConnectSerial( );
    iAppView->iLogManServiceInfo.iConnected = err;

    if ( err  != KErrNone )
    {
        _LIT( KConnectFailed, "Service failed to connect serial." );

        // Stringify the error code.
        TName errcode;
        _LIT( KStrErrNotFound,     "KErrNotFound");
        _LIT( KStrErrTimedOut,     "KErrTimedOut");
        _LIT( KStrErrInUse,        "KErrInUse");
        _LIT( KStrErrNotSupported, "KErrNotSupported");
        _LIT( KStrErrAccessDenied, "KErrAccessDenied");
        _LIT( KStrPermissionDenied,"KErrPermissionDenied");
        _LIT( KStrErrLocked,       "KErrLocked");

        switch ( err )
        {
        case KErrTimedOut:
            errcode.Copy( KStrErrTimedOut );
            break;
        case KErrInUse:
            errcode.Copy( KStrErrInUse );
            break;
        case KErrNotFound:
            errcode.Copy( KStrErrNotFound );
            break;
        case KErrNotSupported:
            errcode.Copy( KStrErrNotSupported );
            break;
        case KErrAccessDenied:
            errcode.Copy( KStrErrAccessDenied );
            break;
        case KErrPermissionDenied:
            errcode.Copy( KStrPermissionDenied );
            break;
        case KErrLocked:
            errcode.Copy( KStrErrLocked );
            break;
        default:
            // Not mapped, show the numeric code then.
            errcode.Num( err );
        }

        _LIT( KErrorFmt, "Error:%S" );
        TName tmp;
        tmp.Format( KErrorFmt, &errcode );
        CEikonEnv::Static()->InfoWinL(KConnectFailed, tmp );
    }

}
void CLoggingServerGuiAppUi::HandleCommandL( TInt aCommand )
{
    // Handle here to avoid connection to RLogMan unnecesarily
    if ( aCommand == ELogServCmdMenuUpdateInfo )
    {
        UpdateServiceInfoL( );
        return;
    }

    switch ( aCommand )
    {
    case EEikCmdExit:
    case EAknSoftkeyExit:
        Exit();
        return;
    }

    RLogMan log;
    User::LeaveIfError( log.Connect() );
    CleanupClosePushL( log );

    switch ( aCommand )
    {
    case ELogServCmdMenuMainConnect:
    {
        ConnectSerial( log );

    }
    break;

    case ELogServCmdMenuMainDisconnect:
    {
        log.DisconnectSerial();
    }
    break;

    case ELogServCmdMenuMainStartService:
    {
        // Started by Connect

//        _LIT( KServiceStartedNoteMsg, "LogMan service process started" );
//        CEikonEnv::Static()->InfoWinL(KServiceStartedNoteMsg, KEmptyStr );
    }
    break;

    case ELogServCmdMenuMainCloseService:
    {
        log.StopService();
//        _LIT( KServiceStoppedNoteMsg, "LogMan service process closed" );
//        CEikonEnv::Static()->InfoWinL(KServiceStoppedNoteMsg, KEmptyStr );
    }
    break;

    case ELogServCmdMenuMainSendTest:
    {
        _LIT8(KTestMsg8, "This is 8-bit test message\n");
        _LIT(KTestMsg16, "This is 16-bit test message\n");
        log.Write( KTestMsg8 );
        log.Write( KTestMsg16 );

        _LIT8(KTestFormattedMsg8,  "This is %d-bit formatted message\n" );
        _LIT( KTestFormattedMsg16, "This is %d-bit formatted message\n" );
        log.Writef( KTestFormattedMsg8, EFalse, 8 );
        log.Writef( KTestFormattedMsg16, EFalse, 16 );

        _LIT( KTestStaticMsg8, "This is 8-bit message sent using static Log\n" );
        _LIT( KTestStaticMsg16, "This is 16-bit message sent using static Log\n" );
        RLogMan::Log( KTestStaticMsg8 );
        RLogMan::Log( KTestStaticMsg16 );

        _LIT8(KTestStaticFormattedMsg8,  "This is %d-bit formatted message sent using static Log\n" );
        _LIT( KTestStaticFormattedMsg16, "This is %d-bit formatted message sent using static Log\n" );
        RLogMan::Log( KTestStaticFormattedMsg8, EFalse, 8 );
        RLogMan::Log( KTestStaticFormattedMsg16, EFalse, 16 );

    }
    break;

    case ELogServCmdMenuMainSendTestAsync:
    {
        _LIT8(KTestMsg8Async, "This is 8-bit asynchronous test message\n");
        _LIT(KTestMsg16Async, "This is 16-bit asynchronous test message\n");
        log.Write( KTestMsg8Async , ETrue);
        log.Write( KTestMsg16Async, ETrue );

        _LIT8( KAsynchronous8,   "asynchronous" );
        _LIT16( KAsynchronous16, "asynchronous" );

        _LIT8(KTestFormattedMsg8Async,    "This is %d-bit %S formatted message\n" );
        _LIT16( KTestFormattedMsg16Async, "This is %d-bit %S formatted message\n" );

        log.Writef( KTestFormattedMsg8Async,  ETrue, 8, &KAsynchronous8 );
        log.Writef( KTestFormattedMsg16Async, ETrue, 16, &KAsynchronous16 );

        _LIT8( KTestStaticMsg8,   "This is 8-bit asynchronous message sent using static Log\n" );
        _LIT16( KTestStaticMsg16, "This is 16-bit asynchronous message sent using static Log\n" );
        RLogMan::Log( KTestStaticMsg8 );
        RLogMan::Log( KTestStaticMsg16 );

        _LIT8(KTestStaticFormattedMsg8,    "This is %d-bit %S formatted message sent using static Log\n" );
        _LIT16( KTestStaticFormattedMsg16, "This is %d-bit %S formatted message sent using static Log\n" );
        RLogMan::Log( KTestStaticFormattedMsg8, EFalse, 8, &KAsynchronous8 );
        RLogMan::Log( KTestStaticFormattedMsg16, EFalse, 16, &KAsynchronous16 );
    }
    break;

    case ELogServCmdMenuMainLoadSerial:
    {
        _LIT( KModuleQueryTxt, "Give serial module to load" );
        TFullName module;
        if ( GenericTextQueryL( KModuleQueryTxt, module ) )
        {
            log.LoadModule( module );
        }
    }
    break;

    case ELogServCmdMenuMainSetPortName:
    {
        _LIT( KModuleQueryTxt, "Enable serial port" );
        TPortName portname = log.PortName();
        if ( GenericTextQueryL( KModuleQueryTxt, portname ) )
        {
            log.SetPortName( portname );
            // Refresh connection
            log.DisconnectSerial();
            ConnectSerial( log );
        }
    }
    break;
    case ELogServCmdMenuMainSetPort:
    {
        _LIT( KModuleQueryTxt, "Set port" );
        TInt port = log.Port();
        if ( GenericNumberQueryL( KModuleQueryTxt, port ) )
        {
            log.SetPort( port );
            // Refresh connection
            log.DisconnectSerial();
            ConnectSerial( log );
        }
    }
    break;
    default:
        Panic( ELoggingServerGuiUi );
        break;
    }

    switch (aCommand)
    {
        // Don't update service info with these
    case EEikCmdExit:
    case EAknSoftkeyExit:
        break;

    default:
        UpdateServiceInfoL( );
    }

    CleanupStack::PopAndDestroy( &log );
}

void CLoggingServerGuiAppUi::HandleStatusPaneSizeChange()
{
    iAppView->SetRect( ClientRect() );
}


// ============================ Construction ===============================
void CLoggingServerGuiAppUi::ConstructL()
{
    // Initialise app UI with standard value.
    LOGMAN_SENDLINE(BaseConstructL(CAknAppUi::EAknEnableSkin);)

    LOGMAN_SENDLOG( "After BaseConstructL" )

    // Create view object
    iAppView = CLoggingServerGuiAppView::NewL( ClientRect() );

    UpdateServiceInfoL();
}

CLoggingServerGuiAppUi::CLoggingServerGuiAppUi()
{
    // No implementation required
}

CLoggingServerGuiAppUi::~CLoggingServerGuiAppUi()
{
    LOGMAN_SENDLOG( "CLoggingServerGuiAppUi" )
    if ( iAppView )
    {
        delete iAppView;
        iAppView = NULL;
    }
}
// ============================ End of construction ============================

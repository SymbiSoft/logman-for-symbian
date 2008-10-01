
// INCLUDE FILES
#include <coemain.h>
#include <aknutils.h>
#include "LogManGui_Appview.h"

_LIT( KPortNameTitle,   "Port name" );
_LIT( KPortTitle,       "Port" );
_LIT( KBytesTitle,      "Bytes sent" );

_LIT( KStatusTitle,     "Service Status" );
_LIT( KStatusRunning,   "Running");
_LIT( KStatusDisabled,  "Disabled");

_LIT( KConnectionTitle,      "Connection Status" );
_LIT( KConnectionActive,     "Active");
_LIT( KConnectionInactive,   "Inactive");
_LIT( KConnectionInUse,      "In use" );
_LIT( KConnectionConnecting, "Connecting" );
_LIT( KConnectionUnknown,    "Unknown error" );

/// Vertical space between title and it's value
const TInt KTitleValueHeightDiff = 2; // Pixels

/// Minimum horizontal position for label
const TInt KXDistance = 5; // Pixels

const TRgb KBlack( 0,0,0 );
const TRgb KRed( 255,0,0 );
const TRgb KGreen( 0,255,0 );
const TRgb KYellow( 255,255,128 );

void CLoggingServerGuiAppView::DrawTitleValuePair( CWindowGc& aGc, TPoint& aPos,
        const TDesC& aTitle,
        const TDesC& aValue,
        const TRgb& aValueColor ) const
{
    TInt fontheight;

    fontheight = iFont->HeightInPixels();
    aGc.UseFont(iFont);
    aPos.iY += fontheight + KTitleValueHeightDiff * 2;
    aGc.SetPenColor( KBlack );
    aGc.DrawText(aTitle, aPos );

    fontheight = iValueFont->HeightInPixels();
    aGc.UseFont(iValueFont);
    aPos.iY += fontheight + KTitleValueHeightDiff;
    aPos.iX += KXDistance;
    aGc.SetPenColor( aValueColor );
    aGc.DrawText(aValue, aPos);
    aPos.iX -= KXDistance;
}


void CLoggingServerGuiAppView::Draw( const TRect& /*aRect*/ ) const
{
    // Get the standard graphics context
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle( CGraphicsContext::ENullPen );
    gc.SetBrushColor( KRgbWhite);
    gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
    gc.Clear();

    // Color of value
    TRgb color(0,0,0);
    TFullName value;

    // Gets the control's extent
    gc.UseFont(iFont);

    TPoint pos( KXDistance, 0 );

    if ( iLogManServiceInfo.iRunning )
    {
        color = KGreen;
    }
    else
    {
        // All red if not running
        color = KRed;
        DrawTitleValuePair( gc, pos, KStatusTitle, KStatusDisabled, color );
        DrawTitleValuePair( gc, pos, KConnectionTitle, KConnectionInactive, color);
        DrawTitleValuePair( gc, pos, KPortNameTitle, iLogManServiceInfo.iPortName, color );
        value.Num( iLogManServiceInfo.iPort );
        DrawTitleValuePair( gc, pos, KPortTitle, value, color );
        return;
    }

    DrawTitleValuePair( gc, pos, KStatusTitle,KStatusRunning, KGreen );


    // Draw connection info
    switch( iLogManServiceInfo.iConnected ) {
        case KErrNone:
            value.Copy( KConnectionActive );
            color = KGreen;
        break;
        case KRequestPending:
            value.Copy( KConnectionConnecting );
            color = KYellow;
        break;
        default:
            value.Copy( KConnectionInactive );
            color = KRed;
    }
    DrawTitleValuePair( gc, pos, KConnectionTitle, value, color );

    // Draw port name and port number
    DrawTitleValuePair( gc, pos, KPortNameTitle, iLogManServiceInfo.iPortName, KBlack );

    value.Num( iLogManServiceInfo.iPort );
    DrawTitleValuePair( gc, pos, KPortTitle, value, KBlack );

    // Draw sent bytes
    value.Num( iLogManServiceInfo.iBytesSent );
    DrawTitleValuePair( gc, pos, KBytesTitle, value, KBlack );

}

// ============================ Constructor FUNCTIONS ===============================
CLoggingServerGuiAppView* CLoggingServerGuiAppView::NewL( const TRect& aRect )
{
    CLoggingServerGuiAppView* self = CLoggingServerGuiAppView::NewLC( aRect );
    CleanupStack::Pop( self );
    return self;
}

CLoggingServerGuiAppView* CLoggingServerGuiAppView::NewLC( const TRect& aRect )
{
    CLoggingServerGuiAppView* self = new ( ELeave ) CLoggingServerGuiAppView;
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    return self;
}

void CLoggingServerGuiAppView::ConstructL( const TRect& aRect )
{
    LOGMAN_INIT

    // Create a window for this application view
    CreateWindowL();

    // set the font
    iFont      = AknLayoutUtils::FontFromId(EAknLogicalFontPrimarySmallFont );
    iValueFont = AknLayoutUtils::FontFromId(EAknLogicalFontSecondaryFont);
    // Set the windows size
    SetRect( aRect );

    // Activate the window, which makes it ready to be drawn
    ActivateL();
    LOGMAN_LOG( "ConstructL exit" )
}

CLoggingServerGuiAppView::CLoggingServerGuiAppView()
{

}

CLoggingServerGuiAppView::~CLoggingServerGuiAppView()
{
    LOGMAN_CLOSE
}
// ============================ END Constructor FUNCTIONS ===============================

// End of File



// INCLUDE FILES
#include "LogManGui_Appui.h"
#include "LogManGui_Document.h"

CEikAppUi* CLoggingServerGuiDocument::CreateAppUiL()
{
    // Create the application user interface, and return a pointer to it;
    // the framework takes ownership of this object
    return ( static_cast <CEikAppUi*> ( new ( ELeave )
                                        CLogManAppUi ) );
}



/// ============================ Construction FUNCTIONS ===============================
CLoggingServerGuiDocument* CLoggingServerGuiDocument::NewL( CEikApplication&
        aApp )
{
    CLoggingServerGuiDocument* self = NewLC( aApp );
    CleanupStack::Pop( self );
    return self;
}

CLoggingServerGuiDocument* CLoggingServerGuiDocument::NewLC( CEikApplication&
        aApp )
{
    CLoggingServerGuiDocument* self =
        new ( ELeave ) CLoggingServerGuiDocument( aApp );

    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
}

void CLoggingServerGuiDocument::ConstructL()
{
    // No implementation required
}

CLoggingServerGuiDocument::CLoggingServerGuiDocument( CEikApplication& aApp )
        : CAknDocument( aApp )
{
    // No implementation required
}

CLoggingServerGuiDocument::~CLoggingServerGuiDocument()
{
    // No implementation required
}
/// ============================ END OF Construction FUNCTIONS ===============================

// End of File


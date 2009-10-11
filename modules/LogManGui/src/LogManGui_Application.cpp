
#include "LogManGui_Document.h"
#include "LogManGui_Application.h"

CApaDocument* CLoggingServerGuiApplication::CreateDocumentL()
{
    // Create an LoggingServerGui document, and return a pointer to it
    return (static_cast<CApaDocument*>
            ( CLoggingServerGuiDocument::NewL( *this ) ) );
}

TUid CLoggingServerGuiApplication::AppDllUid() const
{
    // Return the UID for the LoggingServerGui application
    return RProcess().SecureId();
}

// End of File


#ifndef __LoggingServerGuiAPPLICATION_H__
#define __LoggingServerGuiAPPLICATION_H__

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

// UID for the application;
// this should correspond to the uid defined in the mmp file
const TUid KUidLoggingServerGuiApp = { 0xAF111111 };

/// Application class for LogManGui
/**
* Provides factory to create concrete document object.
*/
class CLoggingServerGuiApplication : public CAknApplication
    {
    public: // Functions from base classes

        /**
        * From CApaApplication, AppDllUid.
        * @return Application's UID (KUidLoggingServerGuiApp).
        */
        TUid AppDllUid() const;

    protected: // Functions from base classes

        /**
        * From CApaApplication, CreateDocumentL.
        * Creates CLoggingServerGuiDocument document object. The returned
        * pointer in not owned by the CLoggingServerGuiApplication object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();
    };

#endif // __LoggingServerGuiAPPLICATION_H__

// End of File


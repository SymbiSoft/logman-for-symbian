
#ifndef __LoggingServerGuiDOCUMENT_H__
#define __LoggingServerGuiDOCUMENT_H__

// INCLUDES
#include <akndoc.h>

// FORWARD DECLARATIONS
class CLoggingServerGuiAppUi;
class CEikApplication;


// CLASS DECLARATION

/// Document part of the LogManGui
class CLoggingServerGuiDocument : public CAknDocument
    {
    public: // Constructors and destructor

        /**
        * NewL.
        * Two-phased constructor.
        * Construct a CLoggingServerGuiDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CLoggingServerGuiDocument.
        */
        static CLoggingServerGuiDocument* NewL( CEikApplication& aApp );

        /**
        * NewLC.
        * Two-phased constructor.
        * Construct a CLoggingServerGuiDocument for the AVKON application aApp
        * using two phase construction, and return a pointer
        * to the created object.
        * @param aApp Application creating this document.
        * @return A pointer to the created instance of CLoggingServerGuiDocument.
        */
        static CLoggingServerGuiDocument* NewLC( CEikApplication& aApp );

        /**
        * ~CLoggingServerGuiDocument
        * Virtual Destructor.
        */
        virtual ~CLoggingServerGuiDocument();

    public: // Functions from base classes

        /**
        * CreateAppUiL
        * From CEikDocument, CreateAppUiL.
        * Create a CLoggingServerGuiAppUi object and return a pointer to it.
        * The object returned is owned by the Uikon framework.
        * @return Pointer to created instance of AppUi.
        */
        CEikAppUi* CreateAppUiL();

    private: // Constructors

        /**
        * ConstructL
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CLoggingServerGuiDocument.
        * C++ default constructor.
        * @param aApp Application creating this document.
        */
        CLoggingServerGuiDocument( CEikApplication& aApp );

    };

#endif // __LoggingServerGuiDOCUMENT_H__

// End of File


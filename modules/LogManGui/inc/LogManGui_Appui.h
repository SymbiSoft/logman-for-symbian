#ifndef __LoggingServerGuiAPPUI_H__
#define __LoggingServerGuiAPPUI_H__

// INCLUDES
#include <aknappui.h>

// FORWARD DECLARATIONS
class CLoggingServerGuiAppView;

class RLogMan;

// CLASS DECLARATION
/// AppUi for LogManGui
/**
* Interacts with the user through the UI and request message processing
* from the handler class
*/
class CLoggingServerGuiAppUi : public CAknAppUi
    {
    public: // Constructors and destructor

        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * CLoggingServerGuiAppUi.
        * C++ default constructor. This needs to be public due to
        * the way the framework constructs the AppUi
        */
        CLoggingServerGuiAppUi();

        /**
        * ~CLoggingServerGuiAppUi.
        * Virtual Destructor.
        */
        virtual ~CLoggingServerGuiAppUi();

    private:  // Functions from base classes

        /**
        * From CEikAppUi, HandleCommandL.
        * Takes care of command handling.
        * @param aCommand Command to be handled.
        */
        void HandleCommandL( TInt aCommand );

        /**
        *  HandleStatusPaneSizeChange.
        *  Called by the framework when the application status pane
 		*  size is changed.
        */
		void HandleStatusPaneSizeChange();

		/// Updates service information to be displayed
		void UpdateServiceInfoL( );
    public:
        /** Updates service information when brough to foreground */
        void HandleForegroundEventL (TBool aForeground);
        /** Connect server's serial connection */
        void ConnectSerial(RLogMan& aLogMan);
        /**
         * Start server's socket server.
         */
        void StartSocketServer(RLogMan& aLogMan);
        void ConnectSocketServer(RLogMan& aLogMan);
        //CArrayFix<TCoeHelpContext>* HelpContextL() const;

    private: // Data

        /**
        * The application view
        * Owned by CLoggingServerGuiAppUi
        */
        CLoggingServerGuiAppView* iAppView;

    };

#endif // __LoggingServerGuiAPPUI_H__

// End of File


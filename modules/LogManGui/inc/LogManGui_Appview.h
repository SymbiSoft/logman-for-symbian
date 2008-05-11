#ifndef __LoggingServerGuiAPPVIEW_H__
#define __LoggingServerGuiAPPVIEW_H__

// INCLUDES
#include <coecntrl.h>

#include <logmanutils.h>

/// Container holding LogMan service information on UI.
class SLogManInformation {

    public:
    /// Name of the serial port.
    TFullName iPortName;
    /// Serial port number.
    TInt      iPort;
    /// Serial connection status of the server.
    TInt      iConnected;
    /// Is server running.
    TBool     iRunning;
    /// How many bytes the server has sent.
    TUint32   iBytesSent;
};

/// Draws the information about server
class CLoggingServerGuiAppView : public CCoeControl
    {
    private:
        /**
         * Draws title and value to screen. Value is intended a little.
         *
         * Example:
         *
         * Title
         *   Value
         *
         * @param aGc  Used Graphics context.
         * @param aPos Starting position for the Title
         * @param aTitle Title text
         * @param aValue Value text
         * @param aValueColor Color of the value text
         *
         */
        void DrawTitleValuePair( CWindowGc& aGc, TPoint& aPos,
                                 const TDesC& aTitle, const TDesC& aValue,
                                 const TRgb& aValueColor ) const;

    public: // New methods

        /**
        * NewL.
        * Two-phased constructor.
        * Create a CLoggingServerGuiAppView object, which will draw itself to aRect.
        * @param aRect The rectangle this view will be drawn to.
        * @return a pointer to the created instance of CLoggingServerGuiAppView.
        */
        static CLoggingServerGuiAppView* NewL( const TRect& aRect );

        /**
        * NewLC.
        * Two-phased constructor.
        * Create a CLoggingServerGuiAppView object, which will draw itself
        * to aRect.
        * @param aRect Rectangle this view will be drawn to.
        * @return A pointer to the created instance of CLoggingServerGuiAppView.
        */
        static CLoggingServerGuiAppView* NewLC( const TRect& aRect );

        /**
        * ~CLoggingServerGuiAppView
        * Virtual Destructor.
        */
        virtual ~CLoggingServerGuiAppView();

       /**
        * GetText
        * @return modifiable reference to the descriptor, which stores
        *   user-given text
        */
        TDes& GetText();

    public:  // Functions from base classes

        /**
        * From CCoeControl, Draw
        * Draw this CLoggingServerGuiAppView to the screen.
        * If the user has given a text, it is also printed to the center of
        * the screen.
        * @param aRect the rectangle of this view that needs updating
        */
        void Draw( const TRect& aRect ) const;
        
        /// Holds the information about the service status
        SLogManInformation iLogManServiceInfo;

    private: // Constructors

        /**
        * ConstructL
        * 2nd phase constructor.
        * Perform the second phase construction of a
        * CLoggingServerGuiAppView object.
        * @param aRect The rectangle this view will be drawn to.
        */
        void ConstructL(const TRect& aRect);

        /**
        * CLoggingServerGuiAppView.
        * C++ default constructor.
        */
        CLoggingServerGuiAppView();

    private:

       /**
        * font used, when printing user-given text to the screen
        */
        const CFont* iFont;
        const CFont* iValueFont;

        LOGMAN_MEMBER
    };

#endif // __LoggingServerGuiAPPVIEW_H__

// End of File


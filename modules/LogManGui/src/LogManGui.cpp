
// INCLUDE FILES
#include <eikstart.h>
#include "LogManGui_Application.h"

/**
 * Create application class
 */
LOCAL_C CApaApplication* NewApplication()
{
    return new CLoggingServerGuiApplication;
}

/**
 * A normal Symbian OS executable provides an E32Main() function which is
 * called by the operating system to start the program.
 */
GLDEF_C TInt E32Main()
{
    return EikStart::RunApplication( NewApplication );
}

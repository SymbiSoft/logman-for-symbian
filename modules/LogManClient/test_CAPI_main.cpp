// Includes
#include <e32base.h>
#include <e32cons.h>
#include <libc/sys/reent.h>

_LIT(KTxtExampleCode,"LogMan C-API test");
_LIT(KTxtSAMPLES,"SAMPLES");
_LIT(KFormatFailed,"Failed: leave code=%d\n");
_LIT(KTxtOK,"");
_LIT(KTxtPressAnyKey,"[Press any key]");

// Variables
LOCAL_D CConsoleBase* console;      // a new console object for our output

extern int main(void);

LOCAL_C void Go()
{
    main();
}

LOCAL_C void runApplication()       // initialize and do main task with cleanup stack
{
    console=Console::NewL(KTxtExampleCode,TSize(KConsFullScreen,KConsFullScreen));
    CleanupStack::PushL(console);   // push console

    TRAPD(error,Go());

    CleanupStack::PopAndDestroy();  // close console
}


GLDEF_C TInt E32Main()                          // main function called by E32, equivalent of C main()
{
    __UHEAP_MARK;
    CTrapCleanup* cleanup=CTrapCleanup::New();  // get clean-up stack

    TRAPD(error,runApplication());              // init stack and do main task
    
    delete cleanup;                             // destroy clean-up stack
    CloseSTDLIB();
    __UHEAP_MARKEND;

    return 0;
}

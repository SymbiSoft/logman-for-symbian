// Includes
#include <e32base.h>
#include <e32cons.h>

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

    if (error)
        console->Printf(KFormatFailed, error);
        else console->Printf(KTxtOK);
    console->Printf(KTxtPressAnyKey);
    console->Getch();               // get and ignore character
    CleanupStack::PopAndDestroy();  // close console
}


GLDEF_C TInt E32Main()                          // main function called by E32, equivalent of C main()
{
    __UHEAP_MARK;
    CTrapCleanup* cleanup=CTrapCleanup::New();  // get clean-up stack

    TRAPD(error,runApplication());              // init stack and do main task
    __ASSERT_ALWAYS(!error,User::Panic(KTxtSAMPLES,error));

    delete cleanup;                             // destroy clean-up stack
    __UHEAP_MARKEND;

    return 0;
}
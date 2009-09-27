

#define __LOGMAN_ENABLED__
#include "LogMan.h"
#include "logmanutils.h"

int main(void) {
    LogMan_Log("Hello world!\n", 0);
    LOGMAN_SENDLOG( "Hello world!" );
    LOGMAN_SENDLOGF( "Hello %s!", "formatted world");
    
    return 0;
}

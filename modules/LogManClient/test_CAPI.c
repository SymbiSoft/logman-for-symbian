

#define __LOGMAN_ENABLED__
#include "LogMan.h"
#include "logmanutils.h"

int main(void) {
    LogMan_Log("Hello world!", 0);
    LOGMAN_SENDLOG( "Hello world!" );
    LOGMAN_SENDLOGF( "Hello %s!", "World" );
    return 0;
}

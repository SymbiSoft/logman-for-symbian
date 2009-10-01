

#define __LOGMAN_ENABLED__
#include "LogMan.h"
#include "logmanutils.h"

int main(void) {
    LogMan_Log("Hello world!\n", 0);
    LOGMAN_SENDLOG( "Hello world!" );
    LOGMAN_SENDLOGF( "Hello %s!", "formatted world");
    
    LOGMAN_SENDLOG( "Testing STACK_INFO" );
    LOGMAN_SEND_STACK_INFO();
    LOGMAN_SENDLOG( "Testing HEAP_INFO" );
    LOGMAN_SEND_HEAP_INFO();
    LOGMAN_SENDLOG( "Testing MEMORY_INFO" );
    LOGMAN_SEND_MEMORY_INFO();
	
    return 0;
}

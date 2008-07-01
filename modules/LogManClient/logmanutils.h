#ifndef LOGMANUTILS_H
#define LOGMANUTILS_H

#if defined( __LOGMAN_ENABLED__ )
    #include <LogMan.h>

    #pragma message( RLogMan logging enabled )

    #define __LOGMAN_TOSTR2(a) #a
    #define __LOGMAN_TOSTR(x) __LOGMAN_TOSTR2(x)

    // Message prefix can be overridden. Default prefix contains the source file and linenumber.
    #ifndef __LOGMAN_MSG_PREFIX__
        // Before each message: '(filename):(linenumber) >'
        #define __LOGMAN_MSG_PREFIX__ __FILE__ ":" __LOGMAN_TOSTR(__LINE__) "> "
    #endif

    #define GETMSG(msg) _L8( __LOGMAN_MSG_PREFIX__ msg "\n" )

    // Use 'MACRO __LOGMAN_SEND_ASYNC__' in mmp to enable asynchronous sending
    #ifdef __LOGMAN_SEND_ASYNC__

        #undef __LOGMAN_SEND_ASYNC__
        #pragma message( RLogMan asynchronous mode used by default )
        #define __LOGMAN_SEND_ASYNC__ ETrue

    #else

        #pragma message( RLogMan synchronous mode used by default )
        #define __LOGMAN_SEND_ASYNC__ EFalse

    #endif

    // =========================================================================================
    // Using static RLogMan functions( a bit slower than using member instance )
    // =========================================================================================
    // Easy one line logging
    #define LOGMAN_SENDLOG( x )  RLogMan::Log( GETMSG(x), __LOGMAN_SEND_ASYNC__ );
    // Easy one line logging, specify async mode explicitly
    #define LOGMAN_SENDLOG2( x, async )  RLogMan::Log( GETMSG(x) ), async );
    // Easy one line logging with formatting
    #define LOGMAN_SENDLOGF( x, args... )  RLogMan::Log( GETMSG(x), __LOGMAN_SEND_ASYNC__, args );
    // Easy one line logging with formatting, specify async mode explicitly
    #define LOGMAN_SENDLOGF2( x, async, args... )  RLogMan::Log( GETMSG(x), async, args );
    // Log line of code before executing it.
    #define LOGMAN_SENDLINE( line ) LOGMAN_SENDLOG( #line ); line
    // Write descriptor directly
    #define LOGMAN_SENDDESC( desc ) RLogMan::Log( desc );
    // Easy one liner for logging information about thread's stack usage
    #define LOGMAN_SEND_STACK_INFO() { RLogMan __logman;__logman.Connect();__logman.StackInfo(); __logman.Close(); }
    // Easy one liner for logging information about thread's heap usage
    #define LOGMAN_SEND_STACK_INFO() { RLogMan __logman;__logman.Connect();__logman.HeapInfo(); __logman.Close(); }
    // Easy one liner for logging information about thread's memory usage
    #define LOGMAN_SEND_MEMORY_INFO() { RLogMan __logman;__logman.Connect();__logman.MemoryInfo(); __logman.Close(); }
        
    // =========================================================================================
    // Using RLogMan as a member
    // =========================================================================================
    // Underscore in _iLogMan to avoid name conflicts.
    #define LOGMAN_MEMBER_VARIABLE _iLogMan
    // This goest to class's header
    #define LOGMAN_MEMBER   RLogMan* LOGMAN_MEMBER_VARIABLE;
    // This goes to class's ConstructL
    #define LOGMAN_INIT     LOGMAN_MEMBER_VARIABLE = new (ELeave) RLogMan; LOGMAN_MEMBER_VARIABLE->Connect();

    // Send log
    #define LOGMAN_LOG( x )         LOGMAN_MEMBER_VARIABLE->Write( GETMSG(x), __LOGMAN_SEND_ASYNC__ );
    #define LOGMAN_LOG2( x, async ) LOGMAN_MEMBER_VARIABLE->Write( GETMSG(x), async );

    // Send log with formatting
    #define LOGMAN_LOGF( x, args... )         LOGMAN_MEMBER_VARIABLE->Writef( GETMSG(x) , __LOGMAN_SEND_ASYNC__, args );
    #define LOGMAN_LOGF2( x, async, args... ) LOGMAN_MEMBER_VARIABLE->Writef( GETMSG(x) , async, args );

    #define LOGMAN_STACK_INFO()  LOGMAN_MEMBER_VARIABLE->StackInfo();
    #define LOGMAN_HEAP_INFO()   LOGMAN_MEMBER_VARIABLE->HeapInfo();
    #define LOGMAN_MEMORY_INFO() LOGMAN_MEMBER_VARIABLE->MemoryInfo();
    
    // Log line of code before executing it.
    #define LOGMAN_LINE( line ) LOGMAN_LOG( #line ); line

    // This goes to class's destructor
    #define LOGMAN_CLOSE  if( LOGMAN_MEMBER_VARIABLE ) { LOGMAN_MEMBER_VARIABLE->Close(); delete LOGMAN_MEMBER_VARIABLE; }

#else // not __LOGMAN_ENABLED__
    #pragma message( RLogMan logging disabled )
    // Empty defines
    #define LOGMAN_SENDLOG(  x, args... )
    #define LOGMAN_SENDLOG2( x, async, args...)
    #define LOGMAN_SENDLOGF( x, args... )
    #define LOGMAN_SENDLOGF2( x, async, args... )
    #define LOGMAN_SENDLINE( line ) line
    #define LOGMAN_SENDDESC( desc )
    #define LOGMAN_SEND_STACK_INFO()
    
    #define LOGMAN_MEMBER
    #define LOGMAN_INIT
    #define LOGMAN_LOG( x )
    #define LOGMAN_LOG2( x, async )
    #define LOGMAN_LOGF( x, args... )
    #define LOGMAN_LOGF2( x, async, args... )
    #define LOGMAN_STACK_INFO() 
    #define LOGMAN_HEAP_INFO()  
    #define LOGMAN_MEMORY_INFO()
    
    #define LOGMAN_CLOSE
    #define LOGMAN_LINE( line ) line

#endif // __LOGMAN_ENABLED__

#endif

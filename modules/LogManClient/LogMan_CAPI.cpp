
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "types.h"
#include "LogMan.h"
#include "string.h"

extern "C" {

EXPORT_C TLogMan* LogMan_New(void)
{
    RLogMan *instance = new RLogMan();
    if( !instance )return NULL;
    
    TLogMan *logman = (TLogMan*)malloc(sizeof(TLogMan));
    logman->cppInstance = (void*)instance;
    return logman;
}

EXPORT_C BOOL LogMan_Connect(TLogMan *aLogMan)
{
    return static_cast<RLogMan*>(aLogMan->cppInstance)->Connect();
}

EXPORT_C void LogMan_Destroy(TLogMan *aLogMan)
{
    delete (RLogMan*)aLogMan->cppInstance;
    free( aLogMan );
}

EXPORT_C void LogMan_Write(TLogMan *aLogMan, const char *aFmt, BOOL aAsync, ...)
{
	char buf[KMaxFormatBufferSize] = {0};
	
	va_list ap;
    va_start(ap, aAsync );
    
    vsprintf( buf, aFmt, ap);
	
    static_cast<RLogMan*>(aLogMan->cppInstance)->Write(buf, aAsync );
	
    va_end(ap);	
}

// Generate simple getters and setters 

/**[[[cog

methods = [ 
    ( "int", "ShellEnabled", "" ),
    ( "int", "SetShellEnabled", ["int aEnable"] ), 
    ( "int", "StackInfo", "" ),
    ( "int", "HeapInfo", "" ),
    ( "int", "MemoryInfo", "" ),
];
template = """
EXPORT_C %(TYPE)s LogMan_%(NAME)s( TLogMan * aLogMan%(ARGS)s )
{
    return static_cast<RLogMan*>(aLogMan->cppInstance)->%(NAME)s(%(PASSEDARGS)s);
}
"""

for m in methods:
    ret, name, args = m
    passedargs = ""
    if len(args) > 0:
        passedargs = ",".join( [ x.split(" ")[-1] for x in args ] )
        args = ", " + ", ".join( args ) 
    fields = { "TYPE" : ret, "NAME" : name, "ARGS" : args, "PASSEDARGS" : passedargs }
    cog.outl( template % fields )

]]]*/

EXPORT_C int LogMan_ShellEnabled( TLogMan * aLogMan )
{
    return static_cast<RLogMan*>(aLogMan->cppInstance)->ShellEnabled();
}


EXPORT_C int LogMan_SetShellEnabled( TLogMan * aLogMan, int aEnable )
{
    return static_cast<RLogMan*>(aLogMan->cppInstance)->SetShellEnabled(aEnable);
}


EXPORT_C int LogMan_StackInfo( TLogMan * aLogMan )
{
    return static_cast<RLogMan*>(aLogMan->cppInstance)->StackInfo();
}


EXPORT_C int LogMan_HeapInfo( TLogMan * aLogMan )
{
    return static_cast<RLogMan*>(aLogMan->cppInstance)->HeapInfo();
}


EXPORT_C int LogMan_MemoryInfo( TLogMan * aLogMan )
{
    return static_cast<RLogMan*>(aLogMan->cppInstance)->MemoryInfo();
}

///[[[end]]]

EXPORT_C BOOL LogMan_Log( const char * aFmt, BOOL aDoAsync, ...)
{
	RLogMan logman;
	TLogMan wlogman;
	char buf[KMaxFormatBufferSize] = {0};
	
	va_list ap;
	va_start( ap, aDoAsync );
	
	vsprintf( buf, aFmt, ap);
	
	TInt result = logman.Connect();
	if ( result == KErrNone) {
		wlogman.cppInstance = &logman;
		LogMan_Write(&wlogman, buf, aDoAsync);
		logman.Close();
	}
	va_end(ap);
 
	return result;
}

}

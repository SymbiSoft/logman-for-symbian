
#include <stdio.h>
#include <stdlib.h>

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

EXPORT_C void LogMan_Destroy(TLogMan *aLogMan)
{
    delete (RLogMan*)aLogMan->cppInstance;
    free( aLogMan );
}

EXPORT_C BOOL LogMan_Log( const char * aString, BOOL aAsync, ...)
{
    va_list ap;
    va_start(ap, aAsync );
    RLogMan::Log( aString, aAsync, ap );
    va_end(ap);
}

}
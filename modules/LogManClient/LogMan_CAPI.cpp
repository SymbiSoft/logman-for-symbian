
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

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

EXPORT_C void LogMan_Write(TLogMan *aLogMan, const char *aString, BOOL aAsync, ...)
{
	VA_LIST ap;
    VA_START(ap, aAsync );
	static_cast<RLogMan*>(aLogMan->cppInstance)->WriteFormatList( aString, KMaxFormatBufferSize, aAsync, ap );
    VA_END(ap);	
}

/** Log information about stack usage */
EXPORT_C int LogMan_StackInfo(TLogMan * aLogMan)
{
	return static_cast<RLogMan*>(aLogMan->cppInstance)->StackInfo();
}

/** Log information about heap usage */
EXPORT_C int LogMan_HeapInfo(TLogMan * aLogMan)
{
	return static_cast<RLogMan*>(aLogMan->cppInstance)->HeapInfo();
}

/** Utility to log both stack and heap usage */
EXPORT_C int LogMan_MemoryInfo(TLogMan * aLogMan)
{
	return static_cast<RLogMan*>(aLogMan->cppInstance)->MemoryInfo();
}

EXPORT_C BOOL LogMan_Log( const char * aString, BOOL aAsync, ...)
{
    VA_LIST ap;
    VA_START(ap, aAsync );
    BOOL res = RLogMan::Log( aString, aAsync, ap );
    VA_END(ap);
    
    return res;
}

}

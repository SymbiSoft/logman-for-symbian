
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

EXPORT_C void LogMan_Write(TLogMan *aLogMan, const char *aFmt, BOOL aAsync, ...)
{
	char buf[KMaxFormatBufferSize] = {0};
	
	va_list ap;
    va_start(ap, aAsync );
    
    vsprintf( buf, aFmt, ap);
	
    static_cast<RLogMan*>(aLogMan->cppInstance)->Write(buf, aAsync );
	
    va_end(ap);	
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

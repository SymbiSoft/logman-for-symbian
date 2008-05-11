#ifndef LOGGINGDEFS_H
#define LOGGINGDEFS_H

#if defined( __DEBUG_LOGGING__ ) && defined( __WINS__ )
#include <e32debug.h>

#pragma message( RDebug logging enabled )

#define PRINT(args...) RDebug::Print( args);
#define PRINTF(args...) RDebug::Printf( args );

#else

#pragma message( No logging )

#define PRINT( args... )
#define PRINTF( args... )
#endif

#endif

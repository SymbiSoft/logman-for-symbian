/*Code generation globals
[[[cog
import cog
import loggingserver
import cpptemplates
]]]*/
//[[[end]]]

#ifndef __LoggingServerDef_h__
#define __LoggingServerDef_h__

#include <e32base.h>

#include "..\LogManClient\types.h"


/// LogMan server's panic enumerations
enum TLogServPanic
{
	EBadRequest = 1,
	EMainSchedulerError,
	ESvrCreateServer,
	ESvrStartServer,
	ECreateTrapCleanup
};

#endif


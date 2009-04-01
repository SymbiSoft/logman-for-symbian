
/*-------------------------------------------------------------------------*/
/**
  @file		ptrace.c
  @author	N. Devillard, V. Chudnovsky
  @date		March 2004
  @version	$Revision: 1.1.1.1 $
  @brief	Add tracing capability to any program compiled with gcc.

  This module is only compiled when using gcc and tracing has been
  activated. It allows the compiled program to output messages whenever
  a function is entered or exited.

  To activate this feature, your version of gcc must support
  the -finstrument-functions flag.

  When using ptrace on a dynamic library, you must set the
  PTRACE_REFERENCE_FUNCTION macro to be the name of a function in the
  library. The address of this function when loaded will be the first
  line output to the trace file and will permit the translation of the
  other entry and exit pointers to their symbolic names. You may set
  the macro PTRACE_INCLUDE with any #include directives needed for
  that function to be accesible to this source file.

  The printed messages yield function addresses, not human-readable
  names. To link both, you need to get a list of symbols from the
  program. There are many (unportable) ways of doing that, see the
  'etrace' project on freshmeat for more information about how to dig
  the information.
*/
/*--------------------------------------------------------------------------*/

/**
	This is a port of ptrace.c for Symbian GCCE.
	Uses LogMan to send the trace.
	@author Jussi Toivola

	To get the function addresses from Symbian binary you can use
	"arm-none-symbianelf-nm -o" to dumb them from the unstripped binary.
	The values this code returns are 0x8000 less than in the binary so add that value yourself.
	I don't want to hardcode it here, because I don't know if it can change.
	TODO: Find out why 0x8000

	Example of compiling your application with call trace enabled using scons-for-symbian:
		scons compiler=gcce gcce_options=-finstrument-functions extra_libs=logman_ptrace.lib,logman,estlib dosis=true

	For MMP:
		OPTION GCCE -finstrument-functions

		LIBRARY logman.lib
		LIBRARY estlib.lib
		STATICLIBRARY ptrace.lib
*/

#if (__GNUC__>2) || ((__GNUC__ == 2) && (__GNUC_MINOR__ > 95))

/*---------------------------------------------------------------------------
   								Includes
 ---------------------------------------------------------------------------*/
#include <e32std.h>
#include <stdlib.h>
#include "../LogManClient/LogMan.h"


/*---------------------------------------------------------------------------
   							    User Macros
 ---------------------------------------------------------------------------*/
#define PTRACE_PIPENAME	 "TRACE"

/* When using ptrace on a dynamic library, the following must be defined:

#include "any files needed for PTRACE_REFERENCE_FUNCTION"
#define PTRACE_REFERENCE_FUNCTION functionName

`*/


/*---------------------------------------------------------------------------
   								Defines
 ---------------------------------------------------------------------------*/

#define REFERENCE_OFFSET "REFERENCE:"
#define FUNCTION_ENTRY   "enter"
#define FUNCTION_EXIT    "exit"
#define END_TRACE        "EXIT"
#define __NON_INSTRUMENT_FUNCTION__   __attribute__((__no_instrument_function__))
#define PTRACE_OFF        __NON_INSTRUMENT_FUNCTION__
#define STR(_x)          #_x
#define DEF(_x)          _x
#define GET(_x,_y)       _x(_y)

static RLogMan* gLogMan = NULL;
static TUint32 gBaseaddr = 0;
static TModuleMemoryInfo info;

extern "C" {

	void logman_close()
	{
		gLogMan->Writef("%s\n", EFalse, END_TRACE);
		gLogMan->Close();
		delete gLogMan;
		gLogMan = NULL;
	}

	int logman_init()
	{
		// Get the base address
		RProcess process;

		process.GetMemoryInfo( info );
		gBaseaddr = info.iCodeBase;

		gLogMan = new RLogMan();
		if( !gLogMan )
			return 0;

		if( gLogMan->Connect() != KErrNone )
		{
			return 0;
		}

		atexit(logman_close);

		return 1;
	}

	void logman_trace(char * what, void * p)
	{


		//gLogMan->Writef( "stack \n", EFalse);//, info.iBase );
		//gLogMan->Writef( "chunk %d\n", EFalse, what, chunk.Base() );
		/*
		gLogMan->Writef( "%s %u\n", EFalse, "iCodeBase", info.iCodeBase );
		gLogMan->Writef( "%s %u\n", EFalse, "iCodeSize", info.iCodeSize );
		gLogMan->Writef( "%s %u\n", EFalse, "iConstDataBase", info.iConstDataBase );
		gLogMan->Writef( "%s %u\n", EFalse, "iConstDataSize", info.iConstDataSize );
		gLogMan->Writef( "%s %u\n", EFalse, "iInitialisedDataBase", info.iInitialisedDataBase );
		gLogMan->Writef( "%s %u\n", EFalse, "iInitialisedDataSize", info.iInitialisedDataSize );
		gLogMan->Writef( "%s %u\n", EFalse, "iUninitialisedDataBase", info.iUninitialisedDataBase );
		gLogMan->Writef( "%s %u\n", EFalse, "iUninitialisedDataSize", info.iUninitialisedDataSize );
		*/
		//gLogMan->Writef( "%s %u-%u=%x\n", EFalse, what, p, info.iCodeBase, reinterpret_cast<TUint32>(p) - (info.iCodeBase ) );

		// Calculate the address to match the value in the unstripped binary.
		gLogMan->Writef( "%s %u\n", EFalse, what, reinterpret_cast<TUint32>(p) - (info.iCodeBase ) );
/*
		TFindLibrary findLib;
		TFullName libName;

		while (findLib.Next(libName)==KErrNone)
		{
			TFindChunk findChunk(libName);
			TFullName chunkName;

			if (findChunk.Next(chunkName)!=KErrNone)
				continue;
			RChunk chunk;

			if (chunk.Open(findChunk)!=KErrNone)
				continue;

			TUint8* base=chunk.Base();
			TInt size=chunk.Size();
			chunk.Close();

			gLogMan->Write("joo");

		}
		*/

		/*
		TFindChunk finder;
		TFullName name;
		while( finder.Next(name) == KErrNone )
		{
			RChunk chunk;
			if( chunk.Open( finder ) == KErrNone )
			{
				gLogMan->Write("chunk");
				//gLogMan->Writef( "chunk base:%x bottom:%x size:%x\r\n", EFalse,
				//			chunk.Base(),
				//			chunk.Bottom(),
				//			chunk.Size());
				chunk.Close();
			}
		}
*/
		//gLogMan->Writef( "%s %u\n", EFalse, "base", gBaseaddr );
		//gLogMan->Writef( "%s %u\n", EFalse, "diff", reinterpret_cast<TUint32>(p) - (gBaseaddr ));
	}
}

/*---------------------------------------------------------------------------
  							Function codes
 ---------------------------------------------------------------------------*/

/** Function called by every function event */
void __NON_INSTRUMENT_FUNCTION__ gnu_ptrace(char * what, void * p);

void
gnu_ptrace(char * what, void * p)
{

	static int first=1 ;
	static int active=1 ;

	if (first) {
		first = 0;
		active = logman_init();
		//first  = 0;
	}

	if (active==0 || !gLogMan )
		return ;

	logman_trace( what, p );

	return ;
}

/** According to gcc documentation: called upon function entry */
extern "C" void
__cyg_profile_func_enter(void *this_fn, void *call_site)
{
	gnu_ptrace(FUNCTION_ENTRY, this_fn);
	(void)call_site;
}

/** According to gcc documentation: called upon function exit */
extern "C" void
__cyg_profile_func_exit(void *this_fn, void *call_site)
{
	gnu_ptrace(FUNCTION_EXIT, this_fn);
	(void)call_site;
}

#endif
/* vim: set ts=4 et sw=4 tw=75 */

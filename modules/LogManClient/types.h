#ifndef __LOGGING_SERVER_CLIENTS_TYPES_H__
#define __LOGGING_SERVER_CLIENTS_TYPES_H__

_LIT(KLoggingServer,   "LogManServ");
_LIT(KNoArgs,"");

const TInt KLogServMessageSize = 512;

typedef TBuf16<KLogServMessageSize> TMessageBuffer16;
typedef TBuf8<KLogServMessageSize> TMessageBuffer8;
typedef TMessageBuffer16 TMessageBuffer;

// Used by RawSend
enum TRawMsgType {
    EMsgComplete = 10,
    EMsgPartial
};

const TUint KDefaultMessageSlots=4;

// Generate build version
/**[[[cog
import pkg_gen
data = """
const TUint KLogServMajorVersionNumber=%d;
const TUint KLogServMinorVersionNumber=%d;
const TUint KLogServBuildVersionNumber=%d;
"""
build = pkg_gen.get_build_number()
build = [ int(x) for x in build ]
vermaj, vermin, build = build

cog.outl( data % ( vermaj, vermin, build ) )
]]]*/

const TUint KLogServMajorVersionNumber=1;
const TUint KLogServMinorVersionNumber=8;
const TUint KLogServBuildVersionNumber=623;

///[[[end]]]

enum TLogServRqst
	{
	ELogServSendMessage = 100,
	ELogServConnect,
	ELogServDisconnect,
	ELogServStart,
	ELogServStop,
    EGetPortRange,
    ELoadModule,
    EIsSerialConnected,
    EGetSentBytes,
	//Getters and setters for connection info
    /*[[[cog
    import loggingserver
    SET    = "Set"
    GET    = "Get"
    CASEID = "E"
    enums  = []
    for getset in [ SET, GET ]:
        for attr in loggingserver.SConnectionInfo.public.attributes:
            type = attr.Type
            name = attr.Name
            enums.append( CASEID + getset + name )
    cog.outl( ",\n".join( enums ) + "," )
    ]]]*/
    ESetPortName,
    ESetPort,
    EGetPortName,
    EGetPort,
    //[[[end]]]
    KLogManMessageIdCount
	};

#endif

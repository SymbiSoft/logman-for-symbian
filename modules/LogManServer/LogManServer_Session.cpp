/*Code generation globals
 [[[cog
 import cog
 import loggingserver
 import cpptemplates
 import symbiantypes
 ]]]*/
//[[[end]]]
#define __DEBUG_LOGGING__
#include "../Common/loggingdefs.h"

#include "LogManServer_Defines.h"
#include "LogManServer_Session.h"

CLogManSession::CLogManSession()
{
}

CLogManSession::~CLogManSession()
{

}

void CLogManSession::DispatchMessageL(const RMessage2& aMessage)
{
	switch (aMessage.Function())
	{
	case ELogServSendMessage:
	{
		HandleMessageSendingL(aMessage);
		return;
	}
	case ELogServConnect:
		PRINTF( "ELogServConnect" )
		TInt err = LoggingServer().ConnectSerial();
		User::LeaveIfError(err);
		return;
/*
	case EIsSerialConnected:
	{
		PRINTF( "EIsSerialConnected" )
		TBool connected = LoggingServer().IsSerialConnected();
		TPckgBuf<TBool> result(connected);
		aMessage.WriteL(0, result);
		PRINTF( "EIsSerialConnected:%d", result )

		return;
	}
	*/
	case EConnectionStatus:
		{
			TInt status = 0;			
			
			if( LoggingServer().IsSerialConnected() ){
				PRINT( _L("Serial connected") );
				status |= (TInt)EConnectionSerial;
			}
			if( LoggingServer().IsSocketConnected() ){
				PRINT( _L("Socket connected") );
				status |= (TInt)EConnectionSocket;
			}
			
			PRINTF( "EConnectionStatus:%d", (TInt)status )
			
			TPckgBuf<TInt> result((TInt)status);
			aMessage.WriteL(0, result);
					
			return;
		}
		
	case ELogServDisconnect:
		PRINTF( "ELogServDisconnect" )
		LoggingServer().DisconnectSerial();
		return;

	case ELogServStop:
	{
		PRINTF( "ELogServStop" )
		LoggingServer().iIsClosing = ETrue;
		CActiveScheduler::Stop();
		return;
	}

	case EGetSentBytes:
	{
		PRINTF( "EGetSentBytes" )

		// Get from server
		TUint32 bytes = LoggingServer().BytesSent();

		// Write to result
		TPckgBuf<TUint32> result(bytes);
		aMessage.WriteL(0, result);

		PRINTF( "EGetSentBytes:%d", result )

		return;
	}

		// It would be nice to be able to query available ports, but this is not working on device.
		//    case EGetPortRange:
		//        {
		//        PRINTF("EGetPortRange");
		//
		//        const TFullName& module = ModuleName( );
		//        TSerialInfo info;
		//        TInt err = LoggingServer().iServer.GetPortInfo( module, info );
		//        PRINT( _L("Module:%S Err:%d, PortName:%S, Low:%d High:%d" ), &module, err, &(info.iName), info.iLowUnit, info.iHighUnit);
		//        User::LeaveIfError( err );
		//
		//        PRINTF("Low:%d High:%d", info.iLowUnit, info.iHighUnit);
		//
		//        {
		//        TPckgBuf<TInt> port( info.iLowUnit );
		//        aMessage.WriteL(0,port);
		//        }
		//        {
		//        TPckgBuf<TInt> port( info.iHighUnit );
		//        aMessage.WriteL(1,port);
		//        }
		//        {
		//        TPortName tmp;
		//        tmp.Copy( info.iName );
		//        TPckgBuf<TPortName> port( tmp );
		//        aMessage.WriteL(2, port );
		//        }

		//        return;
		//        }

	case ELoadModule:
	{
		TFullName aBuffer;
		aMessage.ReadL(1, aBuffer, 0);

		PRINTF( "ELOADMODULE")
		PRINT( aBuffer )

		TInt err = LoggingServer().LoadModule(aBuffer);
		TPckgBuf<TInt> result(err);
		aMessage.WriteL(0, result);

		return;
	}

	//Getters and setters for connection info
	/*[[[cog
	SET = "Set"
	GET = "Get"
	CASEID = "case E"
	descriptors = []
	basic       = []
	I = " " * 4
	for getset in [ SET, GET ]:
		for attr in loggingserver.SConnectionInfo.public.attributes:
			type = attr.Type
			name = attr.Name

			cog.outl( CASEID + getset + name + ":")
			cog.outl( "{")

			if getset == SET:
				function = getset + name
				if attr.Name == "ModuleName": function += "L"
				function += "(aBuffer);"

				cog.outl( I + type + " aBuffer;" )
				if symbiantypes.IsNumber( type ):
					cog.outl( I + "aBuffer = (%s)" % type + "aMessage.Int0();" )
				else:
					cog.outl( I + "aMessage.ReadL(0, aBuffer, 0);" )
				cog.outl( I + function )
				cog.outl( I + "return;")

			elif getset == GET:
				function = name + "( )"

				template = """
					const %(TYPE)s%(REFTYPE)s tmp = %(FUNCTION)s;
					TPckgBuf<%(TYPE)s> p( tmp );
					aMessage.WriteL(0,p);
					return;
				""".rstrip()

				reftype = "&"
				if symbiantypes.IsBasic( type ): reftype = ""
				data = cpptemplates.getTemplateOutput( { "TYPE" : type,
														 "FUNCTION" : function,
														 "REFTYPE"  : reftype },
													   template )
				cog.outl( ("\n" + I ).join( data.split("\n") ) )

			cog.outl( "}")

	]]]*/
	case ESetPortName:
	{
	    TPortName aBuffer;
	    aMessage.ReadL(0, aBuffer, 0);
	    SetPortName(aBuffer);
	    return;
	}
	case ESetPort:
	{
	    TInt aBuffer;
	    aBuffer = (TInt)aMessage.Int0();
	    SetPort(aBuffer);
	    return;
	}
	case ESetAddr:
	{
	    TPortName aBuffer;
	    aMessage.ReadL(0, aBuffer, 0);
	    SetAddr(aBuffer);
	    return;
	}
	case EGetPortName:
	{

	    const TPortName& tmp = PortName( );
	    TPckgBuf<TPortName> p( tmp );
	    aMessage.WriteL(0,p);
	    return;
	}
	case EGetPort:
	{

	    const TInt tmp = Port( );
	    TPckgBuf<TInt> p( tmp );
	    aMessage.WriteL(0,p);
	    return;
	}
	case EGetAddr:
	{

	    const TPortName& tmp = Addr( );
	    TPckgBuf<TPortName> p( tmp );
	    aMessage.WriteL(0,p);
	    return;
	}
	//[[[end]]]
	case ESocketServerConnect:
	{
		const TInt tmp = LoggingServer().ConnectToServer( );
		TPckgBuf<TInt> p( tmp );
		aMessage.WriteL(0,p);
		return;
	}
	
	case ESocketServerListen:
	{
		const TInt tmp = LoggingServer().InitializeSocketServer( );
		TPckgBuf<TInt> p( tmp );
		aMessage.WriteL(0,p);
		return;
	}
	//  Requests that we don't understand at all are a different matter.
	//  This is considered a client programming error, so we panic the
	//  client - this also completes the message.
	default:
		PanicClient(aMessage, EBadRequest);
		return;
	}
}

void CLogManSession::ServiceL(const RMessage2& aMessage)
{
	if (aMessage.IsNull())
	{
		PRINTF( "Invalid client!" )
		return;
	}
	TRAPD(err,DispatchMessageL(aMessage));
	aMessage.Complete(err);
PRINTF( "ServiceL err:%d", err )

}

void CLogManSession::HandleMessageSendingL(const RMessage2& aMessage)
{
	PRINTF("SendMessageL");

	TMessageBuffer8 aBuffer;
	aMessage.ReadL(0, aBuffer);

	TInt aDoAsync = aMessage.Int1();
	PRINTF( "aDoAsync: %d", aDoAsync );

	if (aDoAsync != 0)
	{
		PRINTF("SendMessageL AsyncMessage");
		LoggingServer().AddAsyncMessageL(aBuffer);
	}
	else
	{
		PRINTF("SendMessageL SyncMessage");
		LoggingServer().SendMessage(aBuffer);
	}PRINTF("SendMessageL exit");
}

CLogManServer& CLogManSession::LoggingServer()
{
	return *static_cast<CLogManServer*> (const_cast<CServer2*> (Server()));
}
void CLogManSession::PanicClient(const RMessage2& aMessage, TInt aPanic) const
{
	_LIT(KTxtServer,"LogManServer");
	aMessage.Panic(KTxtServer, aPanic);
}

TPortName CLogManSession::PortName()
{
	return LoggingServer().iConnectionInfo.iPortName;
}

TInt CLogManSession::SetPortName(TPortName& aPortName)
{
	LoggingServer().iConnectionInfo.iPortName.Copy(aPortName);
	return KErrNone;
}

TInt CLogManSession::Port()
{
	return LoggingServer().iConnectionInfo.iPort;
}

TPortName CLogManSession::Addr( )
{
	return LoggingServer().iConnectionInfo.iRemoteAddr;
}

TInt CLogManSession::SetPort(TInt aPort)
{
	LoggingServer().iConnectionInfo.iPort = aPort;
	return KErrNone;
}

TInt CLogManSession::SetAddr(TPortName& aHost)
{
	PRINTF("SetAddr:%S", &aHost);
	LoggingServer().iConnectionInfo.iRemoteAddr.Copy( aHost );
	return KErrNone;
}

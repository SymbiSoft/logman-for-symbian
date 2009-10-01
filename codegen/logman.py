__docformat__ = "epytext"
__author__ = "Jussi Toivola"
__license__ = "BSD"
__copyright__ = "2008 Jussi Toivola"

from templatingtypes import Attribute, Function

PYTHON_STRING_SETTER_TEMPLATE_CODE = """
char* bytes      = NULL;
TInt length  = 0;

if (!PyArg_ParseTuple(args, "u#", &bytes, &length))
{
	return 0;
}

//Py_BEGIN_ALLOW_THREADS;

TPtrC16 buf((TUint16*)bytes, length );
%(TYPE)s tmp; tmp.Copy( buf );
TInt err = self->iLogMan->%(FNAME)s( tmp );

//Py_END_ALLOW_THREADS;

return Py_BuildValue( "i", err );
"""
def getStrSetter( fname, type, prefix = "Set" ):
    code = PYTHON_STRING_SETTER_TEMPLATE_CODE % { "FNAME" : prefix + fname,
                                                  "TYPE"  : type }
    return code

LOADMODULE_IMPLEMENTATION    = getStrSetter("LoadModule", "TFullName", "")

PYTHON_STRING_GETTER_TEMPLATE_CODE = """

%(TYPE)s result = self->iLogMan->%(FNAME)s( );
PyObject* pyresult = Py_BuildValue("u#",result.Ptr(),result.Length());
return pyresult;
"""

def getStrGetter( fname, type ):
    code = PYTHON_STRING_GETTER_TEMPLATE_CODE % { "FNAME" : fname,
                                                  "TYPE"  : type }
    return code


SIMPLE_RETURNING_TEMPLATE = """
PRINTF("%(FNAME)s");
%(RTYPE)s result = self->iLogMan->%(FNAME)s();
return Py_BuildValue( "%(PyRTYPE)s", result );
"""

def getSimpleGetter( fname, rtype, pyrtype ):
    code = SIMPLE_RETURNING_TEMPLATE % { "FNAME" : fname, "RTYPE" : rtype,
                                         "PyRTYPE" : pyrtype }
    return code

BOOL_RETURNING_TEMPLATE = """
PRINTF("%(FNAME)s");
TInt result = self->iLogMan->%(FNAME)s();
if( result == (int)ETrue ) {
    Py_INCREF(Py_True);
    return Py_True;
}
Py_INCREF(Py_False);
return Py_False;

"""

def getBoolGetter( fname ):
    return BOOL_RETURNING_TEMPLATE % {"FNAME" : fname }

def getIntGetter(fname):
    return getSimpleGetter( fname, "TInt", "i" )

ERROR_RAISING_GETTER_TEMPLATE = """
PRINTF("%(FNAME)s");
TInt result = self->iLogMan->%(FNAME)s();
if( result == KErrNone ) {
    Py_INCREF(Py_True);
    return Py_True;
}

return SPyErr_SetFromSymbianOSErr(result);

"""
def getErrorRaisingGetter( fname ):
    return ERROR_RAISING_GETTER_TEMPLATE % {"FNAME" : fname }


CONNECT_IMPLEMENTATION              = getIntGetter("Connect" )
CONNECTSERIAL_IMPLEMENTATION        = getIntGetter("ConnectSerial" )
IS_SERIAL_CONNECTED_IMPLEMENTATION  = getBoolGetter("IsSerialConnected")

PYTHON_INT_SETTER_TEMPLATE_CODE = """
TInt number = 0;

if (!PyArg_ParseTuple(args, "i", &number ) )
{
	return 0;
}

TInt err = self->iLogMan->%(FNAME)s( number );

if (err)
{
	return SPyErr_SetFromSymbianOSErr(err);
}

Py_INCREF(Py_True);
return Py_True;
"""

def getIntSetter( fname ):
    code = PYTHON_INT_SETTER_TEMPLATE_CODE % { "FNAME" : "Set" + fname }
    return code


VOID_RETURNING_TEMPLATE = """
PRINTF("%(FNAME)s");
self->iLogMan->%(FNAME)s();

Py_INCREF(Py_None);
return Py_None;
"""

CLOSE_IMPLEMENTATION = VOID_RETURNING_TEMPLATE % {"FNAME" : "Close"}
STOPSERVICE_IMPLEMENTATION = VOID_RETURNING_TEMPLATE % {"FNAME" : "StopService"}
DISCONNECT_SERIAL_IMPLEMENTATION = VOID_RETURNING_TEMPLATE % {"FNAME" : "DisconnectSerial"}
PORT_RANGE_IMPLEMENTATION = """
PRINTF("PortRange");
TInt low;
TInt high;
TPortName portname;

self->iLogMan->PortRange( low, high, portname);
return Py_BuildValue( "iiu#", low, high, &portname, portname.Length() );
"""


SEND_IMPLEMENTATION_TEMPLATE = """

PRINTF( "Write" )

char* bytes   = NULL;
TInt length   = 0;
TInt aDoAsync = 0;

static const char *const kwlist[] =
{
    "data", "async", NULL
};

if (!PyArg_ParseTupleAndKeywords(args, kwargs, "s#|i", ( char** ) kwlist,
                                 &bytes, &length, &aDoAsync) )
{
	return 0;
}

TInt err = KErrNone;
//Py_BEGIN_ALLOW_THREADS;

TPtrC8 aMsg((TUint8*)bytes, length );
//PRINTF( bytes )

self->iLogMan->%(FNAME)s;
//Py_END_ALLOW_THREADS;

if (err)
{
	return SPyErr_SetFromSymbianOSErr(err);
}

Py_INCREF(Py_True);
return Py_True;
"""

SEND_IMPLEMENTATION = SEND_IMPLEMENTATION_TEMPLATE % {"FNAME" : "Write( aMsg, aDoAsync )"}

SELF = [ Attribute( "Type_LogMan*", "self") ]


class LogMan:
    Methods = [ Function( "static PyObject*", "PyLogMan_Close",
                            SELF,
                          code = CLOSE_IMPLEMENTATION
                        ),
                Function( "static PyObject*", "PyLogMan_Connect",
                            SELF,
                          code = CONNECT_IMPLEMENTATION
                        ),
#                 Function( "static PyObject*", "PyLogMan_PortRange",
#                             SELF,
#                           code = PORT_RANGE_IMPLEMENTATION
#                         ),
                Function( "static PyObject*", "PyLogMan_LoadModule",
                            [ Attribute( "Type_LogMan*", "self"),
                              Attribute( "PyObject*", "args") ],
                          code = LOADMODULE_IMPLEMENTATION
                        ),
                Function( "static PyObject*", "PyLogMan_ConnectSerial",
                            SELF,
                          code = CONNECTSERIAL_IMPLEMENTATION
                        ),
                Function( "static PyObject*", "PyLogMan_IsSerialConnected",
                            SELF,
                          code = IS_SERIAL_CONNECTED_IMPLEMENTATION
                        ),
                Function( "static PyObject*", "PyLogMan_StopService",
                            SELF,
                          code = STOPSERVICE_IMPLEMENTATION
                        ),
                Function( "static PyObject*", "PyLogMan_DisconnectSerial",
                            SELF,
                          code = DISCONNECT_SERIAL_IMPLEMENTATION
                        ),

                Function( "static PyObject*", "PyLogMan_BytesSent",
                            SELF,
                          code = getSimpleGetter( "BytesSent", "TUint32", "l" )
                        ),
                        
                Function( "static PyObject*", "PyLogMan_MemoryInfo",
                            SELF,
                            code = getBoolGetter( "MemoryInfo" ) 
                        ),
                
                Function( "static PyObject*", "PyLogMan_Write",
                            [ Attribute( "Type_LogMan*", "self"),
                              Attribute( "PyObject*", "args"),
                              Attribute( "PyObject*", "kwargs") ],
                          code = SEND_IMPLEMENTATION
                        ),
                #Function( "static PyObject*", "PyLogMan_ModuleName",
                #           [ Attribute( "Type_LogMan*", "self" ) ],
                #          code = getStrGetter( "ModuleName", "TFullName" )
                #        ),
                #Function( "static PyObject*", "PyLogMan_SetModuleName",
                #            [ Attribute( "Type_LogMan*", "self"),
                #              Attribute( "PyObject*", "args") ],
                #          code = getStrSetter( "ModuleName", "TFullName" )
                #        ),
                Function( "static PyObject*", "PyLogMan_Port",
                            [ Attribute( "Type_LogMan*", "self") ],
                          code = getSimpleGetter( "Port", "TInt", "i" )
                        ),
                Function( "static PyObject*", "PyLogMan_SetPort",
                            [ Attribute( "Type_LogMan*", "self"),
                              Attribute( "PyObject*", "args") ],
                          code = getIntSetter( "Port" )
                        ),
                Function( "static PyObject*", "PyLogMan_PortName",
                            [ Attribute( "Type_LogMan*", "self")],
                            code = getStrGetter( "PortName", "TPortName" )
                        ),
                Function( "static PyObject*", "PyLogMan_SetPortName",
                            [ Attribute( "Type_LogMan*", "self"),
                              Attribute( "PyObject*", "args") ],
                          code = getStrSetter( "PortName", "TPortName" )
                        ),
                Function( "static PyObject*", "PyLogMan_StartSocketServer",
                            [ Attribute( "Type_LogMan*", "self") ],
                          code = getIntGetter( "StartSocketServer" )
                        ),
              ]



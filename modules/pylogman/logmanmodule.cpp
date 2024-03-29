
/*Code generation globals
[[[cog
import cog
import cpptemplates
import symbiantypes
import logman
]]]*/
//[[[end]]]

#include <Python.h>
#include <symbian_python_ext_util.h>

#include "../Common/loggingdefs.h"
#include "types.h"
#include "LogMan.h" //RLogMan


#define LogMan_TypeString "LogMan.type_LogMan"
#define LogManTypeObject (*(PyTypeObject *)SPyGetGlobalString(LogMan_TypeString))

/// RLogMan class wrapper declaration
struct Type_LogMan {
    /// Python type header
    PyObject_HEAD;
    /// RLogMan instance.
    RLogMan* iLogMan;
};

/*[[[cog
import cog
import logman
for method in logman.LogMan.Methods:
    cog.outl( method.Declaration() )

cog.outl('')

template = " " * 4 + '{"%s",    (PyCFunction)%s, %s},'
cog.outl( 'static const PyMethodDef LogMan_methods[] = {')
for method in logman.LogMan.Methods:
    pyname = method.Name.split("_")[1]
    fname  = method.Name
    args = 'METH_VARARGS'
    if len( method.Args ) <= 1:
        args = 'METH_NOARGS'
    elif len( method.Args ) == 3:
        args = 'METH_VARARGS | METH_KEYWORDS'


    if pyname == "Close":
        cog.outl( '// Different name to work like standard Python stdout' )
        pyname = "close"
    elif pyname == "Write":
        cog.outl( '// Different name to work like standard Python stdout' )
        pyname = "write"

    cog.outl( template % ( pyname, fname, args ) )

#cog.outl( '// Interfaces for supporting standard stdin/stdout' )
#cog.outl( template % ( "write", "Write", 'METH_VARARGS | METH_KEYWORDS' ) )
#cog.outl( template % ( "close", "Close",   'METH_NOARGS' ) )
cog.outl( " " * 4 + '{NULL, NULL}')
cog.outl( '};')

cog.outl( '\n' * 3)

for method in logman.LogMan.Methods:
    cog.outl( "/// Python wrapper for RLogMan.%s" % method.Name.split("_")[-1] )
    cog.outl( method.Implementation() )

]]]*/
static PyObject* PyLogMan_Close( Type_LogMan* self );
static PyObject* PyLogMan_Connect( Type_LogMan* self );
static PyObject* PyLogMan_LoadModule( Type_LogMan* self, PyObject* args );
static PyObject* PyLogMan_ConnectSerial( Type_LogMan* self );
static PyObject* PyLogMan_IsSerialConnected( Type_LogMan* self );
static PyObject* PyLogMan_StopService( Type_LogMan* self );
static PyObject* PyLogMan_DisconnectSerial( Type_LogMan* self );
static PyObject* PyLogMan_BytesSent( Type_LogMan* self );
static PyObject* PyLogMan_MemoryInfo( Type_LogMan* self );
static PyObject* PyLogMan_Write( Type_LogMan* self, PyObject* args, PyObject* kwargs );
static PyObject* PyLogMan_ShellEnabled( Type_LogMan* self );
static PyObject* PyLogMan_SetShellEnabled( Type_LogMan* self, PyObject* args );
static PyObject* PyLogMan_Port( Type_LogMan* self );
static PyObject* PyLogMan_SetPort( Type_LogMan* self, PyObject* args );
static PyObject* PyLogMan_PortName( Type_LogMan* self );
static PyObject* PyLogMan_SetPortName( Type_LogMan* self, PyObject* args );
static PyObject* PyLogMan_StartSocketServer( Type_LogMan* self );

static const PyMethodDef LogMan_methods[] = {
// Different name to work like standard Python stdout
    {"close",    (PyCFunction)PyLogMan_Close, METH_NOARGS},
    {"Connect",    (PyCFunction)PyLogMan_Connect, METH_NOARGS},
    {"LoadModule",    (PyCFunction)PyLogMan_LoadModule, METH_VARARGS},
    {"ConnectSerial",    (PyCFunction)PyLogMan_ConnectSerial, METH_NOARGS},
    {"IsSerialConnected",    (PyCFunction)PyLogMan_IsSerialConnected, METH_NOARGS},
    {"StopService",    (PyCFunction)PyLogMan_StopService, METH_NOARGS},
    {"DisconnectSerial",    (PyCFunction)PyLogMan_DisconnectSerial, METH_NOARGS},
    {"BytesSent",    (PyCFunction)PyLogMan_BytesSent, METH_NOARGS},
    {"MemoryInfo",    (PyCFunction)PyLogMan_MemoryInfo, METH_NOARGS},
// Different name to work like standard Python stdout
    {"write",    (PyCFunction)PyLogMan_Write, METH_VARARGS | METH_KEYWORDS},
    {"ShellEnabled",    (PyCFunction)PyLogMan_ShellEnabled, METH_NOARGS},
    {"SetShellEnabled",    (PyCFunction)PyLogMan_SetShellEnabled, METH_VARARGS},
    {"Port",    (PyCFunction)PyLogMan_Port, METH_NOARGS},
    {"SetPort",    (PyCFunction)PyLogMan_SetPort, METH_VARARGS},
    {"PortName",    (PyCFunction)PyLogMan_PortName, METH_NOARGS},
    {"SetPortName",    (PyCFunction)PyLogMan_SetPortName, METH_VARARGS},
    {"StartSocketServer",    (PyCFunction)PyLogMan_StartSocketServer, METH_NOARGS},
    {NULL, NULL}
};




/// Python wrapper for RLogMan.Close
static PyObject* PyLogMan_Close( Type_LogMan* self ){
        
    PRINTF("Close");
    self->iLogMan->Close();
    
    Py_INCREF(Py_None);
    return Py_None;
    
}
/// Python wrapper for RLogMan.Connect
static PyObject* PyLogMan_Connect( Type_LogMan* self ){
        
    PRINTF("Connect");
    TInt result = self->iLogMan->Connect();
    return Py_BuildValue( "i", result );
    
}
/// Python wrapper for RLogMan.LoadModule
static PyObject* PyLogMan_LoadModule( Type_LogMan* self, PyObject* args ){
        
    char* bytes      = NULL;
    TInt length  = 0;
    
    if (!PyArg_ParseTuple(args, "u#", &bytes, &length))
    {
    	return 0;
    }
    
    //Py_BEGIN_ALLOW_THREADS;
    
    TPtrC16 buf((TUint16*)bytes, length );
    TFullName tmp; tmp.Copy( buf );
    TInt err = self->iLogMan->LoadModule( tmp );
    
    //Py_END_ALLOW_THREADS;
    
    return Py_BuildValue( "i", err );
    
}
/// Python wrapper for RLogMan.ConnectSerial
static PyObject* PyLogMan_ConnectSerial( Type_LogMan* self ){
        
    PRINTF("ConnectSerial");
    TInt result = self->iLogMan->ConnectSerial();
    return Py_BuildValue( "i", result );
    
}
/// Python wrapper for RLogMan.IsSerialConnected
static PyObject* PyLogMan_IsSerialConnected( Type_LogMan* self ){
        
    PRINTF("IsSerialConnected");
    TInt result = self->iLogMan->IsSerialConnected();
    if( result == (int)ETrue ) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    Py_INCREF(Py_False);
    return Py_False;
    
    
}
/// Python wrapper for RLogMan.StopService
static PyObject* PyLogMan_StopService( Type_LogMan* self ){
        
    PRINTF("StopService");
    self->iLogMan->StopService();
    
    Py_INCREF(Py_None);
    return Py_None;
    
}
/// Python wrapper for RLogMan.DisconnectSerial
static PyObject* PyLogMan_DisconnectSerial( Type_LogMan* self ){
        
    PRINTF("DisconnectSerial");
    self->iLogMan->DisconnectSerial();
    
    Py_INCREF(Py_None);
    return Py_None;
    
}
/// Python wrapper for RLogMan.BytesSent
static PyObject* PyLogMan_BytesSent( Type_LogMan* self ){
        
    PRINTF("BytesSent");
    TUint32 result = self->iLogMan->BytesSent();
    return Py_BuildValue( "l", result );
    
}
/// Python wrapper for RLogMan.MemoryInfo
static PyObject* PyLogMan_MemoryInfo( Type_LogMan* self ){
        
    PRINTF("MemoryInfo");
    TInt result = self->iLogMan->MemoryInfo();
    if( result == (int)ETrue ) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    Py_INCREF(Py_False);
    return Py_False;
    
    
}
/// Python wrapper for RLogMan.Write
static PyObject* PyLogMan_Write( Type_LogMan* self, PyObject* args, PyObject* kwargs ){
        
    
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
    
    self->iLogMan->Write( aMsg, aDoAsync );
    //Py_END_ALLOW_THREADS;
    
    if (err)
    {
    	return SPyErr_SetFromSymbianOSErr(err);
    }
    
    Py_INCREF(Py_True);
    return Py_True;
    
}
/// Python wrapper for RLogMan.ShellEnabled
static PyObject* PyLogMan_ShellEnabled( Type_LogMan* self ){
        
    PRINTF("ShellEnabled");
    TInt result = self->iLogMan->ShellEnabled();
    return Py_BuildValue( "i", result );
    
}
/// Python wrapper for RLogMan.SetShellEnabled
static PyObject* PyLogMan_SetShellEnabled( Type_LogMan* self, PyObject* args ){
        
    TInt number = 0;
    
    if (!PyArg_ParseTuple(args, "i", &number ) )
    {
    	return 0;
    }
    
    TInt err = self->iLogMan->SetShellEnabled( number );
    
    if (err)
    {
    	return SPyErr_SetFromSymbianOSErr(err);
    }
    
    Py_INCREF(Py_True);
    return Py_True;
    
}
/// Python wrapper for RLogMan.Port
static PyObject* PyLogMan_Port( Type_LogMan* self ){
        
    PRINTF("Port");
    TInt result = self->iLogMan->Port();
    return Py_BuildValue( "i", result );
    
}
/// Python wrapper for RLogMan.SetPort
static PyObject* PyLogMan_SetPort( Type_LogMan* self, PyObject* args ){
        
    TInt number = 0;
    
    if (!PyArg_ParseTuple(args, "i", &number ) )
    {
    	return 0;
    }
    
    TInt err = self->iLogMan->SetPort( number );
    
    if (err)
    {
    	return SPyErr_SetFromSymbianOSErr(err);
    }
    
    Py_INCREF(Py_True);
    return Py_True;
    
}
/// Python wrapper for RLogMan.PortName
static PyObject* PyLogMan_PortName( Type_LogMan* self ){
        
    
    TPortName result = self->iLogMan->PortName( );
    PyObject* pyresult = Py_BuildValue("u#",result.Ptr(),result.Length());
    return pyresult;
    
}
/// Python wrapper for RLogMan.SetPortName
static PyObject* PyLogMan_SetPortName( Type_LogMan* self, PyObject* args ){
        
    char* bytes      = NULL;
    TInt length  = 0;
    
    if (!PyArg_ParseTuple(args, "u#", &bytes, &length))
    {
    	return 0;
    }
    
    //Py_BEGIN_ALLOW_THREADS;
    
    TPtrC16 buf((TUint16*)bytes, length );
    TPortName tmp; tmp.Copy( buf );
    TInt err = self->iLogMan->SetPortName( tmp );
    
    //Py_END_ALLOW_THREADS;
    
    return Py_BuildValue( "i", err );
    
}
/// Python wrapper for RLogMan.StartSocketServer
static PyObject* PyLogMan_StartSocketServer( Type_LogMan* self ){
        
    PRINTF("StartSocketServer");
    TInt result = self->iLogMan->StartSocketServer();
    return Py_BuildValue( "i", result );
    
}
//[[[end]]]

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/** Python wrapper for RLogMan::Log */

IMPORT_C BOOL LogMan_Log( const char * aFmt, BOOL aDoAsync, ...);

static PyObject* pylogman_Log(PyObject* /*dummy*/, PyObject* args)
{
	
	
	char* bytes      = NULL;
	TInt length  = 0;
    TInt aDoAsync = 0;

	if (!PyArg_ParseTuple(args, "s#|i", &bytes, &length, &aDoAsync))
	{
		return 0;
	}

    TInt err = KErrNone;   
    bytes[length] = 0;
    err = LogMan_Log(bytes, aDoAsync);

	if (err)
	{
		return SPyErr_SetFromSymbianOSErr(err);
	}

	Py_INCREF(Py_True);
	return Py_True;
}

static PyObject *getattr_LogMan(PyObject *self, char *name)
{
  return Py_FindMethod(const_cast<PyMethodDef*>(&LogMan_methods[0]), self, name);
}

/** Create RLogMan instance */
static RLogMan* CreateLogManL()
{
    return new (ELeave)RLogMan();
}
/**LogMan python constructor*/
static PyObject* Type_LogMan_Construct(PyObject* /*self*/, PyObject * /*args*/)
{
    Type_LogMan *self = PyObject_New(Type_LogMan, &LogManTypeObject);

    if (!self)
    {
        return SPyErr_SetFromSymbianOSErr(KErrNone);
    }

    self->iLogMan = NULL;
    TRAPD(err, self->iLogMan = CreateLogManL() );

    if (err)
    {
        PyObject_Del(self);
        return SPyErr_SetFromSymbianOSErr(err);
    }

  return (PyObject*)self;
}

/** Logman python destructor */
static void dealloc_LogMan(Type_LogMan* self)
{

    self->iLogMan->Close();
    delete self->iLogMan;
    self->iLogMan = NULL;
    PyObject_Del(self);

}


static const PyTypeObject type_template_LogMan = {
  /*******************************************************/
  PyObject_HEAD_INIT(0)    /* initialize to 0 to ensure Win32 portability */
  0,                 /*ob_size*/
  "_pylogman.LogMan",            /*tp_name*/
  sizeof(Type_LogMan), /*tp_basicsize*/
  0,                 /*tp_itemsize*/
  /* methods */
  (destructor)dealloc_LogMan, /*tp_dealloc*/
  0, /*tp_print*/
  (getattrfunc)getattr_LogMan, /*tp_getattr*/
  
  0,                                        /*tp_setattr*/
  0,                                        /*tp_compare*/
  0,                                        /*tp_repr*/
  0,                                        /*tp_as_number*/
  0,                                        /*tp_as_sequence*/
  0,                                        /*tp_as_mapping*/   
  0,                                        /*tp_hash */
  0,                                        /*tp_call*/
  0,                                        /*tp_str*/
  0,                                        /*tp_getattro*/
  0,                                        /*tp_setattro*/
  0,                                        /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT,                       /*tp_flags*/
  "",                                       /*tp_doc */
  0,                                        /*tp_traverse */
  0,                                        /*tp_clear */
  0,                                        /*tp_richcompare */
  0,                                        /*tp_weaklistoffset */
  0,                                        /*tp_iter */
  /* implied by ISO C: all zeros thereafter */
};

extern "C" {

  static const PyMethodDef logman_methods[] = {
    {"LogMan", (PyCFunction)Type_LogMan_Construct, METH_NOARGS},
    {"Log", (PyCFunction)pylogman_Log, METH_VARARGS, "Send log message" },    
    {NULL,              NULL}           /* sentinel */
  };

  DL_EXPORT(void) initlogman(void)
  {
    Py_InitModule("_pylogman", (PyMethodDef*)logman_methods);


    // Create LogMan class type
    PyTypeObject *logman_type = PyObject_New(PyTypeObject, &PyType_Type);
	if (!logman_type)
		return;

    *logman_type = type_template_LogMan;

    TInt err = SPyAddGlobalString(LogMan_TypeString, (PyObject *)logman_type);
    if (err != KErrNone ) // 0 is success
    {
    	PyObject_Del(logman_type);
    	PyErr_SetString(PyExc_Exception, "SPyAddGlobalString failed");
    	return;
    }

    LogManTypeObject.ob_type = &PyType_Type;

  }
} /* extern "C" */

#ifndef EKA2
GLDEF_C TInt E32Dll(TDllReason)
{
  return KErrNone;
}
#endif /*EKA2*/

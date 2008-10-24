
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

#include "..\Common\loggingdefs.h"
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
static PyObject* LogMan_Close( Type_LogMan* self );
static PyObject* LogMan_Connect( Type_LogMan* self );
static PyObject* LogMan_LoadModule( Type_LogMan* self, PyObject* args );
static PyObject* LogMan_ConnectSerial( Type_LogMan* self );
static PyObject* LogMan_IsSerialConnected( Type_LogMan* self );
static PyObject* LogMan_StopService( Type_LogMan* self );
static PyObject* LogMan_DisconnectSerial( Type_LogMan* self );
static PyObject* LogMan_BytesSent( Type_LogMan* self );
static PyObject* LogMan_MemoryInfo( Type_LogMan* self );
static PyObject* LogMan_Write( Type_LogMan* self, PyObject* args, PyObject* kwargs );
static PyObject* LogMan_Port( Type_LogMan* self );
static PyObject* LogMan_SetPort( Type_LogMan* self, PyObject* args );
static PyObject* LogMan_PortName( Type_LogMan* self );
static PyObject* LogMan_SetPortName( Type_LogMan* self, PyObject* args );

static const PyMethodDef LogMan_methods[] = {
// Different name to work like standard Python stdout
    {"close",    (PyCFunction)LogMan_Close, METH_NOARGS},
    {"Connect",    (PyCFunction)LogMan_Connect, METH_NOARGS},
    {"LoadModule",    (PyCFunction)LogMan_LoadModule, METH_VARARGS},
    {"ConnectSerial",    (PyCFunction)LogMan_ConnectSerial, METH_NOARGS},
    {"IsSerialConnected",    (PyCFunction)LogMan_IsSerialConnected, METH_NOARGS},
    {"StopService",    (PyCFunction)LogMan_StopService, METH_NOARGS},
    {"DisconnectSerial",    (PyCFunction)LogMan_DisconnectSerial, METH_NOARGS},
    {"BytesSent",    (PyCFunction)LogMan_BytesSent, METH_NOARGS},
    {"MemoryInfo",    (PyCFunction)LogMan_MemoryInfo, METH_NOARGS},
// Different name to work like standard Python stdout
    {"write",    (PyCFunction)LogMan_Write, METH_VARARGS | METH_KEYWORDS},
    {"Port",    (PyCFunction)LogMan_Port, METH_NOARGS},
    {"SetPort",    (PyCFunction)LogMan_SetPort, METH_VARARGS},
    {"PortName",    (PyCFunction)LogMan_PortName, METH_NOARGS},
    {"SetPortName",    (PyCFunction)LogMan_SetPortName, METH_VARARGS},
    {NULL, NULL}
};




/// Python wrapper for RLogMan.Close
static PyObject* LogMan_Close( Type_LogMan* self ){
        
    PRINTF("Close");
    self->iLogMan->Close();
    
    Py_INCREF(Py_None);
    return Py_None;
    
}
/// Python wrapper for RLogMan.Connect
static PyObject* LogMan_Connect( Type_LogMan* self ){
        
    PRINTF("Connect");
    TInt result = self->iLogMan->Connect();
    if( result == KErrNone ) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    
    return SPyErr_SetFromSymbianOSErr(result);
    
    
}
/// Python wrapper for RLogMan.LoadModule
static PyObject* LogMan_LoadModule( Type_LogMan* self, PyObject* args ){
        
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
    
    if (err)
    {
    	return SPyErr_SetFromSymbianOSErr(err);
    }
    
    Py_INCREF(Py_True);
    return Py_True;
    
}
/// Python wrapper for RLogMan.ConnectSerial
static PyObject* LogMan_ConnectSerial( Type_LogMan* self ){
        
    PRINTF("ConnectSerial");
    TInt result = self->iLogMan->ConnectSerial();
    if( result == KErrNone ) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    
    return SPyErr_SetFromSymbianOSErr(result);
    
    
}
/// Python wrapper for RLogMan.IsSerialConnected
static PyObject* LogMan_IsSerialConnected( Type_LogMan* self ){
        
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
static PyObject* LogMan_StopService( Type_LogMan* self ){
        
    PRINTF("StopService");
    self->iLogMan->StopService();
    
    Py_INCREF(Py_None);
    return Py_None;
    
}
/// Python wrapper for RLogMan.DisconnectSerial
static PyObject* LogMan_DisconnectSerial( Type_LogMan* self ){
        
    PRINTF("DisconnectSerial");
    self->iLogMan->DisconnectSerial();
    
    Py_INCREF(Py_None);
    return Py_None;
    
}
/// Python wrapper for RLogMan.BytesSent
static PyObject* LogMan_BytesSent( Type_LogMan* self ){
        
    PRINTF("BytesSent");
    TUint32 result = self->iLogMan->BytesSent();
    return Py_BuildValue( "l", result );
    
}
/// Python wrapper for RLogMan.MemoryInfo
static PyObject* LogMan_MemoryInfo( Type_LogMan* self ){
        
    PRINTF("MemoryInfo");
    TInt result = self->iLogMan->MemoryInfo();
    if( result == KErrNone ) {
        Py_INCREF(Py_True);
        return Py_True;
    }
    
    return SPyErr_SetFromSymbianOSErr(result);
    
    
}
/// Python wrapper for RLogMan.Write
static PyObject* LogMan_Write( Type_LogMan* self, PyObject* args, PyObject* kwargs ){
        
    
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
/// Python wrapper for RLogMan.Port
static PyObject* LogMan_Port( Type_LogMan* self ){
        
    PRINTF("Port");
    TInt result = self->iLogMan->Port();
    return Py_BuildValue( "i", result );
    
}
/// Python wrapper for RLogMan.SetPort
static PyObject* LogMan_SetPort( Type_LogMan* self, PyObject* args ){
        
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
static PyObject* LogMan_PortName( Type_LogMan* self ){
        
    
    TPortName result = self->iLogMan->PortName( );
    PyObject* pyresult = Py_BuildValue("u#",result.Ptr(),result.Length());
    return pyresult;
    
}
/// Python wrapper for RLogMan.SetPortName
static PyObject* LogMan_SetPortName( Type_LogMan* self, PyObject* args ){
        
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
    
    if (err)
    {
    	return SPyErr_SetFromSymbianOSErr(err);
    }
    
    Py_INCREF(Py_True);
    return Py_True;
    
}
//[[[end]]]

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
/** Python wrapper for RLogMan::Log */
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

    TPtrC8 buf((TUint8*)bytes, length);
    err = RLogMan::Log( buf, aDoAsync );

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
  "pylogman.LogMan",            /*tp_name*/
  sizeof(Type_LogMan), /*tp_basicsize*/
  0,                 /*tp_itemsize*/
  /* methods */
  (destructor)dealloc_LogMan, /*tp_dealloc*/
  0, /*tp_print*/
  (getattrfunc)getattr_LogMan, /*tp_getattr*/

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

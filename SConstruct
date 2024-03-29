#!/usr/env/bin python

import os
from os.path import join
import sys
import glob
import scons_symbian
from scons_symbian import *
from scons_symbian.arguments import Help

import codegen

COMMON_DEFINES = [
    #"__DEBUG_LOGGING__"
]
LOGGINGSERVER_INCLUDES = [ join("modules", x) for x in [
                                "LogManServer",
                                "LogManClient",
                                "Common" 
                            ]
                          ]

PACKAGE_DRIVE_MAP = { "C" : ".*[.](mif|rsc)" }

Help('cert - Set certificate certificate\n')
CERT  = ARGUMENTS.get("cert", None)
Help('passwd - Set certificate key\n')
KEY   = ARGUMENTS.get("key", None)
Help('passwd - Set certificate password\n')
PASSWD = ARGUMENTS.get("passwd", "" )

#: Enable test compilation
Help( 'withtests - Include tests\n')
HAVE_TESTS = ARGUMENTS.get("withtests", "False" )
HAVE_TESTS = eval( HAVE_TESTS )

LOGMAN_VERSION = ".".join(map(str, codegen.pkg_gen.get_build_number()))
LOGMAN_PACKAGE = "LogMan_%s_%s_%s.sis" % (COMPILER, RELEASE, LOGMAN_VERSION.replace(".", "_"))

#: Python library to link with.
PYTHON_LIB = ARGUMENTS.get("pythonlib", "python25")
Help("pythonlib - Python library to link pylogman with. python25 sets pymodprefix and openc settings automatically: default=python25\n")

PYMODPREFIX = ARGUMENTS.get("pymodprefix", "")
Help("pymodprefix - Prefix required at beginning of dynamic extension's filename: default=''\n")

USE_OPENC = ARGUMENTS.get("openc", "false")
Help("openc - Use OpenC: default=false\n")
  
PACKAGE_UID  = 0xe3195807
SERVER_UID   = PACKAGE_UID + 1
GUI_UID      = SERVER_UID + 1
CLIENT_UID   = GUI_UID + 1
PYCLIENT_UID = CLIENT_UID + 1
TESTER_UID = CLIENT_UID + 1

def ZModemLib():
    """Module for ZModem file transfer"""
    
    sources = glob.glob(join("modules", "ZModemLib", "*.cpp"))
        
    loggingserver_libraries = [ ]
    return SymbianProgram('ZModemStatic', TARGETTYPE_LIB,
                             sources,
                             LOGGINGSERVER_INCLUDES,
                             loggingserver_libraries,
                             defines=COMMON_DEFINES,
                             package=LOGMAN_PACKAGE,
                             package_drive_map=PACKAGE_DRIVE_MAP,
                     )

    
def LogManServer():
    "LogMan.exe (server ) defines"
        
    sources = glob.glob(join("modules", "LogManServer", "*.cpp"))
        
    loggingserver_libraries = [ 'euser', 'efsrv', 'c32', 'bafl', 'esock', 'insock', 'commdb',
                                #'ZModemStatic'
                                ]
    return SymbianProgram('LogMan', TARGETTYPE_EXE,
                             sources,
                             LOGGINGSERVER_INCLUDES,
                             loggingserver_libraries,
                             uid3=SERVER_UID,
                             defines=COMMON_DEFINES,
                             package=LOGMAN_PACKAGE,
                             package_drive_map=PACKAGE_DRIVE_MAP,
                             capabilities = CAPS_DEV_CERT,
                             mmpexport = join( "modules", "LogManServer", "LogManServer.mmp" ),
                     )

def LogManDll():
    "LogMan.dll defines ( client )"
    logManDllIncludes = LOGGINGSERVER_INCLUDES
            
    #logman_sources += glob.glob(join("modules", "Common", "ptrace_logman.cpp"))
     
    logman_libraries = [ 'euser', 'efsrv', 'estlib' ]
    logman_sources = Glob(join("modules", "LogManClient", "Log*.cpp"))        
    
    def_folder = { COMPILER_WINSCW : "BWINS", COMPILER_GCCE : "EABI"}[COMPILER]
    for name, type in [('LogMan', TARGETTYPE_DLL),('LogManStatic', TARGETTYPE_LIB)]:
          SymbianProgram(name, type,
                        logman_sources,
                        includes = logManDllIncludes,
                        sysincludes = [join(EPOCROOT, "epoc32", "include", "libc")],
                        libraries   = logman_libraries,
                        uid3 = CLIENT_UID,
                        defines=COMMON_DEFINES,
                        package=LOGMAN_PACKAGE,
                        package_drive_map=PACKAGE_DRIVE_MAP,
                        mmpexport = join( "modules", "LogManClient", name + ".mmp" ),
                        )      
    
    if HAVE_TESTS:
	    SymbianProgram("LogManTestCAPI", TARGETTYPE_EXE, 
	        Glob( join( "modules", "LogManClient", "test*.*")),
	        includes = logManDllIncludes,
	        libraries = ["euser", "efsrv", "estlib", "LogManStatic.lib"],
	        sysincludes = [join(EPOCROOT, "epoc32", "include", "libc")],
	        uid3 = TESTER_UID,
	        )
                 
def PyLogManDll():
    global USE_OPENC
    global PYMODPREFIX
    #-------------------------------------------- pylogman defines ( Python client )
    def_folder = { COMPILER_WINSCW : "BWINS", COMPILER_GCCE : "EABI"}[COMPILER]
    
    name = "_pylogman"
    
    if PYTHON_LIB == "python25":
        USE_OPENC   = True
        PYMODPREFIX = "kf_"
        
    if PYTHON_LIB != "python222":
        name = PYMODPREFIX + name
                
        sysincludes = [ 
                        join(EPOC32_INCLUDE, PYTHON_LIB),
                      ] 
    else:        
        
        sysincludes = [
                        join(EPOC32_INCLUDE, "python"),
                      ]
        
    if USE_OPENC:
        sysincludes.append( join(EPOC32_INCLUDE, "stdapis") )
    else:
        sysincludes.append( join(EPOC32_INCLUDE, "libc") )
        
    ToPackage(package=LOGMAN_PACKAGE, target=join("python", "lib"),
                source=join("modules", "pylogman", "pylogman.py"))
    
    ToPackage(package=LOGMAN_PACKAGE, target=join("python"),
                source=join("modules", "pylogman", "logman_manager.py")) 
                                                        
    return SymbianProgram(name, TARGETTYPE_PYD,
                    [ join("modules", "pylogman", "logmanmodule.cpp")],
                    includes = [join("modules", "LogManClient")],
                    sysincludes = sysincludes,
                    libraries = [PYTHON_LIB, "euser", "LogMan", "libc"],
                    uid3   = PYCLIENT_UID, 
                    defines=COMMON_DEFINES,
                    package=LOGMAN_PACKAGE,
                    package_drive_map=PACKAGE_DRIVE_MAP,
                    mmpexport = join( "modules", "pylogman", "pylogman.mmp" ),
                    winscw_options = "-w noempty",
                    )

#-------------------------------------- LogManGui defines ( tester and manager )
def LogManGui():
    
        
    src_path = join("modules", "LogManGui")
    sources = glob.glob(join(src_path, "src", "*.cpp"))
    resources = glob.glob(join(src_path, "data", "*.rss"))
    resources.sort() # Because LogManGui_reg.reg depends on LogManGui.reg
    icons = glob.glob(join(src_path, "gfx", "*.svg"))
    
    libraries = [ 
	                "eikcore", "euser", "apparc",
	                "avkon", "commonengine", "efsrv",
	                "estor", "eikcoctl", "eikdlg",
	                "gdi", "cone", "LogMan",
                    "hlplch",
                    # For testing ptrace.lib
                    #"ptrace.lib","estlib"
                ] 
                           
                             
    includes = [
                    join("modules", "Common"),
                    join("modules", "LogManClient"),
                    join("modules", "LogManGui", "inc"),
                    join("modules", "LogManGui", "data")
                ]
    
    
    # Just for testing scons-for-symbian
    #gui_only_sis  = SymbianPackage( "LogManGui.sis",
    #                                pkgfile = join( "install", "LogManGui.pkg" )
    #                )
    # With Ensymble method, the dependencies are generated automatically with the package
    # argument for SymbianProgram, but not for PKG files with different output sis.
    # So we need to do a small hack to put PKG-based sis in correct order.
    #Depends( gui_only_sis, main_sis )
    
    SymbianProgram('LogManGui', TARGETTYPE_EXE,
                    sources,
                    includes,
                    libraries,
                    uid3=GUI_UID,
                    resources=resources,
                    icons=icons,
                    defines=[ ],
                    #epocstacksize = 0x8000,
#                    help         = join( "modules", "LogManGui", "help", "LogManGui.cshlp" ), 
                    package=LOGMAN_PACKAGE,
                    package_drive_map=PACKAGE_DRIVE_MAP,
                    mmpexport = join( "modules", "LogManGui", "group", "LogManGui.mmp" ),
                    # For testing ptrace.lib
                    #gcce_options="-finstrument-functions" + GCCE_OPTIMIZATION_FLAGS
                    )
    
    SymbianPackage(LOGMAN_PACKAGE,
                   pkgargs={"uid"     : PACKAGE_UID,
                            "version" : LOGMAN_VERSION.split("."),
                            "appname" : "LogMan",
                            "cert"    : CERT,
                            "key"     : KEY,
                            "passwd"  : PASSWD, },
    )

def PTraceLib():
    # Built as a separate lib
    ptrace_sources = glob.glob(join("modules", "PTrace", "*.cpp"))
    includes = [join(EPOCROOT, "epoc32", "include", "libc")]
    
    SymbianProgram('logman_ptrace', scons_symbian.TARGETTYPE_LIB,
                    ptrace_sources,
                    sysincludes = includes,
                    mmpexport = join( "modules", "PTrace","PTrace.mmp" ), )          
    
LogManServer()
LogManDll()
PyLogManDll()
LogManGui() 
ZModemLib()
PTraceLib()


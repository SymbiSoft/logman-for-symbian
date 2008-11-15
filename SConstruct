#!/usr/env/bin python

import os
from os.path import join
import sys
import glob

import scons_symbian
from scons_symbian import *

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

opts = Options()
opts.Add('pythonlib', 'Select correct Python library', "Python222")
opts.Add('cert', 'Set signing certificate', None)
opts.Add('key', 'Set certificate key', None)

env = Environment(options=opts)
Help(opts.GenerateHelpText(env))

CERT = ARGUMENTS.get("cert", None)
KEY = ARGUMENTS.get("key", None)

#: Select your Python library
PYTHON_LIB = ARGUMENTS.get("pythonlib", "Python222")

LOGMAN_VERSION = ".".join(map(str, codegen.pkg_gen.get_build_number()))
LOGMAN_PACKAGE = "LogMan_%s_%s_%s.sis" % (COMPILER, RELEASE, LOGMAN_VERSION.replace(".", "_"))

PYTHON_LIB = ARGUMENTS.get("pythonlib", "python222")
Help("pythonlib - Python library to link pylogman with: default=Python222\n")
 
PACKAGE_UID  = 0xe3195807
SERVER_UID   = PACKAGE_UID + 1
GUI_UID      = SERVER_UID + 1
CLIENT_UID   = GUI_UID + 1
PYCLIENT_UID = CLIENT_UID + 1

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
                             capabilities=FREE_CAPS,
                             defines=COMMON_DEFINES,
                             package=LOGMAN_PACKAGE,
                             package_drive_map=PACKAGE_DRIVE_MAP,
                             mmpexport = join( "modules", "LogManServer", "LogManServer.mmp" ),
                     )

def LogManDll():
    "LogMan.dll defines ( client )"
    logManDllIncludes = LOGGINGSERVER_INCLUDES
            
    #logman_sources += glob.glob(join("modules", "Common", "ptrace_logman.cpp"))
     
    logman_libraries = [ 'euser', 'efsrv', 'estlib' ]
    logman_sources = glob.glob(join("modules", "LogManClient", "*.cpp"))    
     
    def_folder = { COMPILER_WINSCW : "BWINS", COMPILER_GCCE : "EABI"}[COMPILER]
    return SymbianProgram('LogMan', TARGETTYPE_DLL,
                    logman_sources,
                    includes = logManDllIncludes,
                    sysincludes = [join(EPOCROOT, "epoc32", "include", "libc")],
                    libraries   = logman_libraries,
                    uid3 = CLIENT_UID,
                    defines=COMMON_DEFINES,
                    package=LOGMAN_PACKAGE,
                    package_drive_map=PACKAGE_DRIVE_MAP,
                    mmpexport = join( "modules", "LogManClient", "LogMan.mmp" ),
                    )
                 
def PyLogManDll():
    #-------------------------------------------- pylogman defines ( Python client )
    def_folder = { COMPILER_WINSCW : "BWINS", COMPILER_GCCE : "EABI"}[COMPILER]
    pylogman_sysincludes = [ join(EPOCROOT, "epoc32", "include", "libc"),
                             join(EPOCROOT, "epoc32", "include", "python"),
                          #join("modules", "LogManClient")
                        ]
    
    ToPackage(package=LOGMAN_PACKAGE, target=join("python", "lib"),
                source=join("modules", "pylogman", "pylogman.py"))
    
    ToPackage(package=LOGMAN_PACKAGE, target=join("python"),
                source=join("modules", "pylogman", "logman_manager.py")) 
                                                        
    return SymbianProgram('_pylogman', TARGETTYPE_PYD,
                    [ join("modules", "pylogman", "logmanmodule.cpp")],
                    includes = [join("modules", "LogManClient")],
                    sysincludes = pylogman_sysincludes,
                    libraries = [PYTHON_LIB, "euser", "LogMan"],
                    uid3   = PYCLIENT_UID, 
                    defines=COMMON_DEFINES,
                    package=LOGMAN_PACKAGE,
                    package_drive_map=PACKAGE_DRIVE_MAP,
                    mmpexport = join( "modules", "pylogman", "pylogman.mmp" )
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
                            "key"     : KEY },
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

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
LOGGINGSERVER_INCLUDES  = [ join( "modules", x ) for x in [
                                "LogManServer", 
                                "LogManClient", 
                                "Common" 
                            ]
                          ]

PACKAGE_DRIVE_MAP = { "C" : ".*[.](mif|rsc)" }

opts = Options()
opts.Add('pythonlib', 'Select correct Python library', "Python222")
opts.Add('cert', 'Set signing certificate', None )
opts.Add('key', 'Set certificate key', None )

env = Environment(options = opts)
Help(opts.GenerateHelpText(env))

CERT = ARGUMENTS.get( "cert", None )
KEY  = ARGUMENTS.get( "key", None )

#: Select your Python library
PYTHON_LIB = ARGUMENTS.get( "pythonlib", "Python222" )

LOGMAN_VERSION = ".".join( map(str, codegen.pkg_gen.get_build_number() ) )
LOGMAN_PACKAGE = "LogMan_%s_%s_%s.sis" % ( COMPILER, RELEASE, LOGMAN_VERSION.replace(".", "_") )

def LogManServer():
    "LogMan.exe (server ) defines"
        
    sources = glob.glob( join( "modules","LogManServer","*.cpp") )
        
    loggingserver_libraries = [ 'euser', 'efsrv', 'c32', 'bafl' ]
    return SymbianProgram(   'LogMan', scons_symbian.TARGETTYPE_EXE,
                             sources,
                             LOGGINGSERVER_INCLUDES,
                             loggingserver_libraries,
                             uid3 = '0xe3195807',
                             capabilities = FREE_CAPS,
                             defines      = COMMON_DEFINES,
                             package      = LOGMAN_PACKAGE,
                             package_drive_map = PACKAGE_DRIVE_MAP,
                     )

def LogManDll():
    "LogMan.dll defines ( client )"
    
    logman_libraries = [ 'euser', 'efsrv', 'estlib' ]
    logman_sources   = glob.glob( join( "modules", "LogManClient","*.cpp" ) )
     
    def_folder = { COMPILER_WINSCW : "BWINS", COMPILER_GCCE : "EABI"}[COMPILER]
    return SymbianProgram( 'LogMan', scons_symbian.TARGETTYPE_DLL,
                    logman_sources,                     
                    LOGGINGSERVER_INCLUDES,
                    logman_libraries,    
                    defines      = COMMON_DEFINES,
                    package      = LOGMAN_PACKAGE,
                    package_drive_map = PACKAGE_DRIVE_MAP, )
                 
def PyLogManDll():
    #-------------------------------------------- pylogman defines ( Python client )
    def_folder = { COMPILER_WINSCW : "BWINS", COMPILER_GCCE : "EABI"}[COMPILER]
    pylogman_includes = [ join( EPOCROOT, "epoc32", "include", "libc"),
                          join( EPOCROOT, "epoc32", "include", "python" ),
                          join( "modules", "LogManClient" )
                        ]
    return SymbianProgram( 'pylogman', scons_symbian.TARGETTYPE_PYD,
                    [ join( "modules", "pylogman", "logmanmodule.cpp" )],                     
                    pylogman_includes,
                    [PYTHON_LIB, "euser", "LogMan"],                    
                    defines      = COMMON_DEFINES,
                    package      = LOGMAN_PACKAGE,
                    package_drive_map = PACKAGE_DRIVE_MAP, )

#-------------------------------------- LogManGui defines ( tester and manager )
def LogManGui():
    
        
    src_path  = join( "modules", "LogManGui" )
    sources   = glob.glob( join( src_path, "src", "*.cpp" ) )
    resources = glob.glob( join( src_path, "data","*.rss") )
    resources.sort() # Because LogManGui_reg.reg depends on LogManGui.reg
    icons     = glob.glob( join( src_path, "gfx", "*.svg" ) )
    
    libraries = [ 
	                "eikcore", "euser",        "apparc",  
	                "avkon",   "commonengine", "efsrv",
	                "estor",   "eikcoctl",     "eikdlg",
	                "gdi",     "cone",         "LogMan",
                    "hlplch"
                ] 
                           
                             
    includes  = [
                    join( "modules","Common"),
                    join( "modules","LogManClient"),
                    join( "modules","LogManGui","inc"),
                    join( "modules","LogManGui","data" )
                ]
    
    #: Create installer
    uid = '0xAF111111'
    main_sis = SymbianPackage(  LOGMAN_PACKAGE,
                                pkgargs = { "uid"     : uid,
                                            "version" : LOGMAN_VERSION.split("."),
                                            "appname" : "LogMan",
                                            "cert"    : CERT,
                                            "key"     : KEY },
                  )
    
    # Just for testing scons-for-symbian
    #gui_only_sis  = SymbianPackage( "LogManGui.sis",
    #                                pkgfile = join( "install", "LogManGui.pkg" )
    #                )
    # With Ensymble method, the dependencies are generated automatically with the package
    # argument for SymbianProgram, but not for PKG files with different output sis.
    # So we need to do a small hack to put PKG-based sis in correct order.
    #Depends( gui_only_sis, main_sis )
    
    return SymbianProgram( 'LogManGui', scons_symbian.TARGETTYPE_EXE,
                    sources,                     
                    includes,
                    libraries,    
                    uid3         = uid,                    
                    resources    = resources,
                    icons        = icons,
                    defines      = [ ],
#                    help         = join( "modules", "LogManGui", "help", "LogManGui.cshlp" ), 
                    package      = LOGMAN_PACKAGE,
                    package_drive_map = PACKAGE_DRIVE_MAP,
                    )

LogManServer()
LogManDll()
PyLogManDll()
LogManGui() 

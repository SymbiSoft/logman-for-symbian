#!/usr/env/bin python

import os
import sys
import glob

import scons_symbian
from scons_symbian import *

import codegen

COMMON_DEFINES = [
    #"__DEBUG_LOGGING__"
]
LOGGINGSERVER_INCLUDES  = [ r"modules\LogManServer", r"modules\LogManClient", r"modules\Common" ]

LOGMAN_PACKAGE = "LogMan.sis"
LOGMAN_VERSION = ".".join( map(str, codegen.pkg_gen.get_build_number() ) )

def LogManServer():
    "LogMan.exe (server ) defines"
        
    sources = glob.glob( r"Modules\LogManServer\*.cpp")
        
    loggingserver_libraries = [ 'euser', 'efsrv', 'c32', 'BAFL' ]
    return SymbianProgram(   'LogMan', scons_symbian.TARGETTYPE_EXE,
                             sources,
                             LOGGINGSERVER_INCLUDES,
                             loggingserver_libraries,
                             uid2 = '0x100039ce',
                             uid3 = '0xe3195807',
                             capabilities = FREE_CAPS,
                             defines      = COMMON_DEFINES,
                             package      = LOGMAN_PACKAGE
                     )

def LogManDll():
    "LogMan.dll defines ( client )"
    
    logman_libraries = [ 'euser', 'efsrv', 'estlib' ]
    logman_sources   = glob.glob( r"modules\LogManClient\*.cpp" )
    
    
    def_folder = { COMPILER_WINSCW : "BWINS", COMPILER_GCCE : "EABI"}[COMPILER]
    return SymbianProgram( 'LogMan', scons_symbian.TARGETTYPE_DLL,
                    logman_sources,                     
                    LOGGINGSERVER_INCLUDES,
                    logman_libraries,    
                    #definput = r"LoggingServer\\" + def_folder + r"\LogManU.DEF",
                    capabilities = FREE_CAPS,
                    defines = COMMON_DEFINES,
                    package      = LOGMAN_PACKAGE )
                 
def PyLogManDll():
    #-------------------------------------------- pylogman defines ( Python client )
    def_folder = { COMPILER_WINSCW : "BWINS", COMPILER_GCCE : "EABI"}[COMPILER]
    pylogman_includes = [ r"\epoc32\include\libc",
                          r"\epoc32\include\python",
                          r"modules\LogManClient"
                        ]
    return SymbianProgram( 'pylogman', scons_symbian.TARGETTYPE_PYD,
                    ["modules\\pylogman\\logmanmodule.cpp"],                     
                    pylogman_includes,
                    ["python222", "euser", "LogMan"],                    
                    #definput = r"LoggingServer\\" + def_folder + r"\pylogmanU.DEF",
                    capabilities = FREE_CAPS,
                    defines = COMMON_DEFINES,
                    package      = LOGMAN_PACKAGE )

#-------------------------------------- LogManGui defines ( tester and manager )
def LogManGui():
    
        
    src_path  = r"modules\LogManGui\\"
    sources   = glob.glob( src_path + "src\\*.cpp" )
    resources = glob.glob( src_path + "data\\*.rss")
    resources.sort() # Because LogManGui_reg.reg depends on LogManGui.reg
    icons     = glob.glob( src_path + "gfx\\*.svg" )
    
    libraries = [ 
	                "eikcore", "euser",        "apparc",  
	                "avkon",   "commonengine", "efsrv",
	                "estor",   "eikcoctl",     "eikdlg",
	                "gdi",     "cone",         "LogMan"
                ] 
                           
                             
    includes  = [
                    r"modules\Common",
                    r"modules\LogManClient",                    
                    r"modules\LogManGui\inc",
                    r"modules\LogManGui\data"                    
                ]
    
    uid = '0xAF111111'
    SymbianPackage( LOGMAN_PACKAGE,
                    uid, 
                    ensymbleargs = { "--version" : LOGMAN_VERSION }
                  )
                  
    return SymbianProgram( 'LogManGui', scons_symbian.TARGETTYPE_EXE,
                    sources,                     
                    includes,
                    libraries,    
                    uid3 = uid,                    
                    capabilities = FREE_CAPS,
                    resources = resources,
                    icons = icons,
                    defines = [ ],
                    package      = LOGMAN_PACKAGE,
                    )
                     
LogManServer()
LogManDll()
PyLogManDll()
LogManGui() 


import os
import sys
import shutil
import glob

def install():
    epocroot    = os.environ.get( "EPOCROOT", "\\")
    
    installpath = os.path.join( epocroot, "EPOC32" )
    if not os.path.exists( installpath ):
        raise Exception( "Invalid EPOCROOT")
        
    installpath = os.path.join( installpath, "include", "logman" ) 
    if not os.path.exists( installpath ):
        os.mkdir( installpath )
    
    source = ( os.path.dirname(__file__), "..", "modules", "LogManClient" )
    source = os.path.join( *source )
    source = os.path.normpath( source )
    for header in glob.glob( os.path.join( source, "*.h") ):
        s = header
        t = os.path.join( installpath, os.path.basename( header ) )
        print "Copying %s ==> %s" % ( s, t )
        shutil.copyfile( s, t )
    
def start():

    if raw_input( r"""
This script installs logman headers to '%EPOCROOT%\epoc32\include\logman'.
Press Enter to continue, Ctrl+C to cancel.
""") == "":
        install()

if __name__ == "__main__":
    try:
        start()
    except KeyboardInterrupt: pass

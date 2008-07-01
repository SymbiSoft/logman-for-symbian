
import os
import sys
import shutil

def install():
    epocroot    = os.environ["EPOCROOT"]
    installpath = epocroot + r"EPOC32\include\logman\\" 
    if not os.path.exists( installpath ):
        os.mkdir( installpath )
    
    source = "..\modules\LogManClient\\"
    for header in [ "LogMan.h", "logmanutils.h"]:
        s = source + header
        t = installpath + header
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
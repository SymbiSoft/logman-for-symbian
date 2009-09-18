"""
Code generation launcher using COG
http://nedbatchelder.com/code/cog
"""
import time
start = time.clock()

import sys
sys.path.append( "codegen" )

from cogapp import Cog
# main expects these
FILES = [ __file__, "-r"]

def add(globexpr, exclude = ""):
    import glob
    files = glob.glob(globexpr)
    
    for x in files:        
        doadd = True
        for e in exclude:
            if  x.endswith( e ):
                doadd = False 
                continue
                    
        if doadd:
            FILES.append( x )

add( r"modules/LogManGui/src/*.*" )
add( r"modules/LogManGui/inc/*.*" )
add( r"modules/LogManClient/*.*" )
add( r"modules/LogManServer/*.*" )
add( r"modules/pylogman/*.*" )
add( r"install/*.*" )

if __name__ == "__main__":
    ret = Cog().main( FILES )
    sys.exit(ret)

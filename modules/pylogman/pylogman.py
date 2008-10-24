import _pylogman
from _pylogman import *

from traceback import extract_stack

class LogManError(Exception):
    pass

ECHO_LOCATION_TEMPLATE = "%s[%s:%d]> %s"
_LOG = Log
def Log(msg, async=0, echolocation=1):
    if echolocation:
        filepath, linenumber, caller, line = extract_stack()[-2]
       #filepath = basename( filepath )
        msg = ECHO_LOCATION_TEMPLATE % ( filepath, caller, linenumber, msg)
        
    return _LOG(msg, async)


class LogMan(object):
    
    def __init__(self):
        self.this = _pylogman.LogMan()
        
        self.echoLocation = True
        self._isConnected = False
                 
        
        attrs = dir(self.this)
        attrs.remove("Connect")
        attrs.remove("close")
        attrs.remove("write")                                        
        for x in attrs:
            setattr(self, x, getattr(self.this, x ))
        
                 
    def Connect(self):        
        #Exception on error
        self.this.Connect()
        self._isConnected = True
        return True
    
    def close(self):    
        self._isConnected = False                
        return self.this.close()
            
    def write(self, msg ):
        global ECHO_LOCATION_TEMPLATE
        
        if not self._isConnected:
            raise LogManError("Not connected")
             
        if self.echoLocation:
            filepath, linenumber, caller, line = extract_stack()[-2]            
            msg = ECHO_LOCATION_TEMPLATE % ( filepath, caller, linenumber, msg)
                
        return self.this.write( msg ) 
        
        
        

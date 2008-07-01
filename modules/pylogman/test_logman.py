"""Tests for LogMan client"""

__docformat__ = "epytext"
__author__ = "Jussi Toivola"
__license__ = "BSD"
__copyright__ = "2008 Jussi Toivola"

import pylogman
logman = pylogman.LogMan()
print dir(logman)
import time
import e32
import sys


MESSAGE = "the quick brown fox jumps over the lazy dog"
MESSAGE = "".join( [ str(x) +":" + MESSAGE for x in xrange(20) ] ) + "\n"
MESSAGE_UNICODE     = MESSAGE.encode( "UTF-16LE")
HALF_MESSAGE_LENGTH = len(MESSAGE) / 2
             
__file__ = "test_logman.py"

NOT_DONE = True

def log(msg):
    
    pylogman.Log( __file__ + "> " + msg)
    #print msg

    
#pylogman.Log("test_logman.py pys60usb.USBConnection()")
def recv():    
    pylogman.Log("test_logman.py LOOPBACK receiver started")
    
    import pys60usb
    conn = pys60usb.USBConnection()
    conn.connect( 1, u"LOOPBACK", mode = pys60usb.ECommShared)

    received = tmp = ""
    while tmp != "\n":
        #sys.stdout.write( tmp )
        received += tmp
        tmp = conn.recv( 1 )
        #e32.ao_sleep(0.1)
        
    print ""
        
    log ( "Received: '%s'" % received )
    conn.close()
    
    #assert received == str(MESSAGE), "Received data is not equal to the sent data."
    global NOT_DONE
    NOT_DONE = False

def send( data, async ):
    log( data )
    logman.write( data, async )
    
pylogman.Log("test_logman.py started")

logman.Connect()
logman.LoadModule(u"LOOPBACK")
log( "logman.PortName()" )
print logman.PortName()
log( "logman.ConnectSerial()" )
print logman.ConnectSerial()
log( "logman.SendLog()" )
for cmd in ( MESSAGE[:HALF_MESSAGE_LENGTH], MESSAGE[HALF_MESSAGE_LENGTH:] ):             
    log( cmd )
    send(cmd, False )

log("Receive the data")
recv()

log( "Stopping service" )
logman.StopService()

log( "logman.Close()" )

logman.Close()

del logman

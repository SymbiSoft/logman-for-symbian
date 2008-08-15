
import sys
import os
import serial
import time
from os.path import join

def send_data( c, data ):

    c.read( 100 );
            
    length = len( data )
    c.write( "%d " % length )
    print "Sending:%d bytes" % length
    
    buffersize = 512
    sent       = 0
    while sent < length:
        print "S"
        buffer = min( buffersize, length - sent )
        print sent, sent+buffer
        #import pdb;pdb.set_trace()
        c.write( data[sent:sent+buffer] )
        
        #if buffer == 0:
        #    print "No data sent. Cancelling."
        #    return

        sent += buffer
        print length, "|", sent
        
        #t = ""
        #while t == "":
        t = c.read( 1 );
        sys.stdout.write( t )
            #time.sleep(0.01)
    
def install_binary( c, sourcepath ):
    filename   = os.path.basename( sourcepath )
    targetpath = "C:\\sys\\bin\\" + filename
    f = open( sourcepath,'rb' ); data = f.read(); f.close()

    cmd = "put %s\n" % targetpath
    print cmd
    
    c.write( cmd )
    send_data(c, data )

def install_resource( c, sourcepath ):

    targetpath = "C:\\" + sourcepath
    sourcepath = join( "GCCE", "LogMan", sourcepath )
    f = open( sourcepath,'rb' ); data = f.read(); f.close()

    cmd = "put %s\n" % targetpath
    #print cmd

    c.write( cmd )
    send_data(c, data )
    
def install( c, source, target ):

    sourcepath = join( "GCCE", "LogMan", source )
    f = open( sourcepath,'rb' ); data = f.read(); f.close()

    cmd = "put %s\n" % target
    print cmd

    c.write( cmd )
    send_data(c, data )


def start():
    # Program parameter definitions
    from optparse import OptionParser
    parser = OptionParser()

    parser.add_option("-p", "--port", dest="port",
                      help="Used COM port number",
                      default = None )

    (options, args) = parser.parse_args()

    portnumber = options.port
    if portnumber is None:
        portnumber = raw_input("Give serial port number: ")

    port = "//./COM%s" % portnumber
    print "Using port", port
    c = serial.Serial( port, 115200, timeout = 1 )

    #c.write( "cp c:\\data\\logman.exe c:\\sys\\bin\n" )
    c.write( "put c:\\sys\\bin\\LogMan.exe\n")
    install_binary( c, join( "GCCE", "LogMan", "sys", "bin", "LogMan.exe" ) )
    install( c, join( "private", "10003a3f", "import", "apps", "LogManGui.rsc" ),
                join( "C:\\", "resource", "apps", "LogManGui.rsc" ) )
    #install_resource( c, join( "private", "10003a3f", "import", "apps", "LogManGui_reg.rsc" ) )
    #f =
    #c.write( "y\n" ) # Query

    t = " "
    while t != "":
        t = c.read( 1 );
        sys.stdout.write(t)#t.decode("utf-8") )
    c.close( )
    
start()

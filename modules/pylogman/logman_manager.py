"""Manager for LogMan service"""
__docformat__ = "epytext"
__author__ = "Jussi Toivola"
__license__ = "BSD"
__copyright__ = "2008 Jussi Toivola"

import appuifw
OLD_BODY =  appuifw.app.body

import graphics
import e32
import time
import pylogman

class Gui:

    def __init__(self):
        
        print "Initializing..."
        self.script_lock = e32.Ao_lock()
        self.LogMan      = pylogman.LogMan() 
        self.__initMenu()
        self.mhConnectToService()
        self.mhGetServerInfo()
        print "LogMan Manager ready to serve"        
        
    def __initMenu(self,menu=None):
        """Initialize the main menu
        TODO: Remove Stop task if none running
        """    
        if menu is None:
            menu =[ ( u"Get server info",   self.mhGetServerInfo),
                    ( u"Connect to server", self.mhConnectToService ),
                    ( u"Connect serial",    self.mhConnectSerial ),
                    ( u"Stop service",      self.mhStopService ),
                    ( u"Disconnect serial", self.mhDisconnectSerial ),
                    ( u"Test message",      self.mhTestMessage),
                    ( u"Test async msg",    self.mhTestMessageAsync),
                    ( u"Load serial module",self.mhLoadSerialModule),
                    ( u"Set port name",     self.mhSetPortName),
                    ( u"Set port",          self.mhSetPort),
                    ( u"Exit",              self.script_lock.signal) ]

        appuifw.app.menu = menu

#--- Menu handlers -------------------------------------------------------------
    def mhGetServerInfo(self):       
        "Menu handle for Get server info" 
        print "Used Port :", self.LogMan.PortName()
        print "Port      :", self.LogMan.Port()        
        print "Bytes sent:", self.LogMan.BytesSent()
        print "Connected",         
        if self.LogMan.IsSerialConnected(): print "True"
        else: print "False" 
        
             
    def mhConnectToService(self):
        print "Connecting to server",
        if self.LogMan.Connect(): print "True"
        else: print "False"
    
    def mhConnectSerial(self):
        print "Connecting serial",
        if self.LogMan.ConnectSerial(): print "True"
        else: print "False" 
        
    def mhStopService(self):
        print "Stopping service",
        print self.LogMan.StopService()
    
    def mhDisconnectSerial(self):
        print "Closing serial connection",
        print self.LogMan.DisconnectSerial()        
    
    
    def mhTestMessage(self):
        print "Sending test log",                
        self.LogMan.write( time.ctime().strip() + ": Testing message" )
        self.LogMan.write( "\r\n" )
        print "Done"
    
    def mhTestMessageAsync(self):
        print "Sending async test log",        
        #for x in xrange( 100 ):
        msg = ""
        for x in xrange( 100 ):
            msg += "%d:Testing asynchronous message " % ( x )
            
        self.LogMan.write( time.ctime().strip() + ":" + msg, async=True )
        self.LogMan.write( "\r\n" )
        print "Done"
    
    def mhSetPort(self):
        #ports    = [ unicode(x) for x in self.LogMan.PortRange() ]
        #cur_port = self.LogMan.Port()
        #print ports, "current:", cur_port
        #
        #
        #selection = appuifw.selection_list( ports )
        selection = appuifw.query( u"Set port number", "number" )
        # User cancelled
        if selection is None: return
        
        #selection = ports[selection]
        success   = self.LogMan.SetPort( int( selection ) )
        print success
        print "Port %s set" % selection 
        
    def mhLoadSerialModule(self):

#------- Show list of predefined modules, but allow user to give custom name----    
        print "Selecting serial module"
        items = [
            u"ECACM", 
            u"LOOPBACK",            
            u"IRCOMM",
            #u"ECUART",
            u"Add Custom"
        ]
        selection = appuifw.selection_list( items )
        
        # User cancelled
        if selection is None: return
        
        selection = items[selection]
        if selection == items[-1]:
            selection = appuifw.query( u"Custom Module", "text" )
            # User cancelled
            if selection is None: return
        
        
#------- Enable the module -----------------------------------------------------        
        success = self.LogMan.LoadModule( selection )        
        if not success:
            appuifw.note( u"Failed to set serial module %s" % selection, 'error')
        else:
            appuifw.note( u"Serial module %s enabled" % selection, 'info')
        
        
    def mhSetPortName(self):
        print "Selecting serial port name",
        # TODO: Read from a file
        items = [ 
            u"ACM",
            u"LOOPBACK",            
            u"IRCOMM",
            u"Add Custom"
        ]
        selection = appuifw.selection_list( items )
         # User cancelled
        if selection is None: return
        
        selection = items[selection]
        if selection == items[-1]:
            selection = appuifw.query( u"Custom port name", "text" )
            # User cancelled
            if selection is None: return
            
        success = self.LogMan.SetPortName( selection )
        print selection
        if not success:
            appuifw.note( u"Failed to set port name %s" % selection, 'error')
        else:
            appuifw.note( u"Serial port %s enabled" % selection, 'info')
            
        
myGui = Gui()
appuifw.app.exit_key_handler=myGui.script_lock.signal
myGui.script_lock.wait()
    
appuifw.app.body = OLD_BODY    
print "LogMan manager exited"

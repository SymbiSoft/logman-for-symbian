__docformat__ = "epytext"
__author__ = "Jussi Toivola"
__license__ = "BSD"
__copyright__ = "2008 Jussi Toivola"

from templatingtypes import *
from symbiantypes    import *

class SConnectionInfo:
    Comment = "Struct containing serial connection type strings and port number."
    Type    = STRUCT
    class public:
        attributes = [ 
            #Attribute( TFullName, "ModuleName", "Name of the connection module to use." ),
            Attribute( TPortName, "PortName", "Name of the port to open." ),
            Attribute( TInt,      "Port", "Port number to open." ),
            Attribute( TPortName, "Addr", "Remote address to connect to." ),
            Attribute( TBool,     "ShellEnabled", "Shell access enable\\disable." ),
        ]

ClassName = "CLoggingServerServer"


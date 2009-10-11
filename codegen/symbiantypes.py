__docformat__ = "epytext"
__author__ = "Jussi Toivola"
__license__ = "BSD"
__copyright__ = "2008 Jussi Toivola"

# Descriptors
DESCRIPTOR_TYPES = [ "TBuf", "TFullName", "TPortName" ]
for x in DESCRIPTOR_TYPES:
    locals()[x] = x
    
NUMBER_TYPES = [ "TInt", "TUint", "TBool", ]
BASIC_TYPES = [ "TChar",  ] + NUMBER_TYPES
 
for x in BASIC_TYPES:
    locals()[x] = x

def IsBasic( vartype ):
    return ( vartype in BASIC_TYPES )
def IsNumber(vartype):    
    return ( vartype in NUMBER_TYPES )

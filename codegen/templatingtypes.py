__docformat__ = "epytext"
__author__ = "Jussi Toivola"
__license__ = "BSD"
__copyright__ = "2008 Jussi Toivola"


for x in [ "STRUCT", "CLASS" ]:
    locals()[x] = x

class Attribute:
    def __init__( self, type, name, comment = "",
                  doset = True, doget = True):
        self.Name    = name
        self.Type    = type
        self.Comment = comment
        
        self.doget   = doget
        self.doset   = doset

class Function:
    def __init__(self, rtype, name, args = None, code = ""):
        if args is None: args = []
        
        self.RType = rtype
        self.Name  = name
        self.Args  = args
        self.Code  = code
        self.Class = ""
    
    def Declaration(self):
        template = "%(RTYPE)s %(NAME)s( %(ARGS)s );"
        args = ", ".join( [x.Type + " " + x.Name for x in self.Args] )
        return template % { "RTYPE" : self.RType,
                            "NAME"  : self.Name,
                            "ARGS"  : args}
                            
    def Implementation(self):
        template = "%(RTYPE)s %(NAME)s( %(ARGS)s )" 
        args = ", ".join( [x.Type + " " + x.Name for x in self.Args] )
        name = ""
        if self.Class != "":
            name = self.Class + "::"
        name += self.Name
        
        data = template % { "RTYPE" : self.RType,
                            "NAME"  : name,
                            "ARGS"  : args}
        code = " " * 4 + "\n".join( [ " " * 4 + x for x in self.Code.split("\n") ] )
        data += "\n".join( [ "{", code, "}" ] )
        
        return data
                            
        
                    

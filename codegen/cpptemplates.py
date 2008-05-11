__docformat__ = "epytext"
__author__ = "Jussi Toivola"
__license__ = "BSD"
__copyright__ = "2008 Jussi Toivola"

import cog
import textwrap
import symbiantypes
from templatingtypes import *

TYPE      = "TYPE"
NAME      = "NAME"
CLASSNAME = "CLASSNAME"
BASEATTR  = "BASEATTR"
REFTYPE   = "REFTYPE"

GETREF_DECLARATION_TEMPLATE = "%(TYPE)s& %(NAME)s( );"
GET_DECLARATION_TEMPLATE    = "%(TYPE)s %(NAME)s( );"
GET_IMPLEMENTATION_TEMPLATE    = """
%(TYPE)s %(CLASSNAME)s::%(NAME)s( ) {
    return %(BASEATTR)si%(NAME)s;
}""".strip()

SET_DECLARATION_TEMPLATE    = "TInt Set%(NAME)s( %(TYPE)s& a%(NAME)s );"

STRUCT_DECLARATION_TEMPLATE = """
struct %(NAME)s {
%(DATA)s
};
""".strip()

SET_DESCRIPTOR_IMPLEMENTATION_TEMPLATE = """
TInt %(CLASSNAME)s::Set%(NAME)s( %(TYPE)s& a%(NAME)s ) {
    return %(BASEATTR)si%(NAME)s%(REFTYPE)sCopy( a%(NAME)s );
}""".strip()

SET_BASIC_IMPLEMENTATION_TEMPLATE = """
TInt %(CLASSNAME)s::Set%(NAME)s( %(TYPE)s& a%(NAME)s ) {
    %(BASEATTR)si%(NAME)s = a%(NAME)s;
}""".strip()

def hasfield( template, variable ):
    return ( ("%(" + variable + ")s") in template )

def getTemplateOutput( datafields, template ):

    def setdefault( variable, default ):
        if  hasfield( template, variable)\
        and variable not in datafields:
            datafields[variable] = default

    setdefault( "NAME",      "")
    setdefault( "TYPE",      "")
    setdefault( "CLASSNAME", "")
    setdefault( "VARTYPE",   "")

#     import pdb;pdb.set_trace()
    data = template % datafields
    return textwrap.dedent( data )

def Comment( data, indents = 0 ):
    indent = "    " * indents
    data = data.split("\n")
    if len( data ) > 1:
        data = [ indent + "* " + x for x in data ]

    lines = [indent + "/**"]
    lines += data
    lines += [ indent + "*/" ]
    if len( data ) > 1:
        data = "\n".join( lines )
    else:
        data = " ".join( lines )

    cog.outl( data )
    #for line in lines:
    #    cog.outl( line )

def GetRefDeclaration( attribute ):
    return getTemplateOutput( { NAME : attribute.Name, TYPE : attribute.Type }, GETREF_DECLARATION_TEMPLATE )

def GetDeclaration( attribute ):
    return getTemplateOutput( { NAME : attribute.Name, TYPE : attribute.Type }, GET_DECLARATION_TEMPLATE )

def SetDeclaration( attribute ):
    return getTemplateOutput( { NAME : attribute.Name, TYPE : attribute.Type }, SET_DECLARATION_TEMPLATE )

def GetImplementation( classname, attribute, baseattr ):
    return getTemplateOutput( { NAME         : attribute.Name,
                                 TYPE      : attribute.Type,
                                 CLASSNAME : classname,
                                 BASEATTR  : baseattr },
                               GET_IMPLEMENTATION_TEMPLATE )


def IsDescriptor( attribute):
    #print attribute.Type
    is_descriptor = ( attribute.Type in symbiantypes.DESCRIPTOR_TYPES )
    #print is_descriptor
    return is_descriptor    
    
def SetImplementation( classname, attribute, baseattr ):
    is_descriptor = IsDescriptor( attribute )
    reftype       = "."
    if "*" in attribute.Type:
        reftype = "->"

    data = { "NAME"    : attribute.Name,
             "TYPE"    : attribute.Type,
             "REFTYPE" : reftype,
             "CLASSNAME" : classname,
             "BASEATTR" : baseattr }

    template = ""
    if is_descriptor:

        template = SET_DESCRIPTOR_IMPLEMENTATION_TEMPLATE
        data = getTemplateOutput( data, template )
        return data
    else:
        template = SET_BASIC_IMPLEMENTATION_TEMPLATE
        data = getTemplateOutput( data, template )

    return data

def GenerateGetSet( attr, export = False ):
    """
    Generate attribute getter and setter declarations
    @param  attr Attribute object
    """

    #for attr in classdata.public.attributes:
        #data = "    " + attr.Type + " i" + attr.Name
        #data += "\n"
    getset = []
    if attr.doget: getset.append((GetDeclaration, "Getter"))
    if attr.doset: getset.append((SetDeclaration, "Setter"))
    
    for f, comment in getset:      
        decl = f( attr )
        Comment( comment + " for: " + attr.Comment )
        if export: cog.out("IMPORT_C ")                
        cog.outl( decl )
#     if attr.doset:
#         decl = SetDeclaration( attr )
#         Comment( "Setter for: " + attr.Comment )
#         if export: cog.out("IMPORT_C ")
#         cog.outl( decl )

    cog.outl("")



def GenerateGetSetImplementation( classdata, attr, baseattr = ""):
    """
    Generate attribute getter and setter
    @param  attr Attribute object
    """
    classname = classdata.ClassName

    if attr.doget:
        decl = GetImplementation( classname, attr, baseattr)
        Comment( "Getter for: " + attr.Comment )        
        cog.outl( decl )
    if attr.doset:
        decl = SetImplementation( classname, attr, baseattr )
        Comment( "Setter for: " + attr.Comment )
        cog.outl( decl )

    cog.outl("")

def ClassDeclaration( classdata ):
    """
    Generate attribute getter and setter
    @param  attr Attribute object
    """
    name = classdata.__name__
    #data = GenerateAttributeMethods(classdata)
    data = ""
    for attr in classdata.public.attributes:
        data += "    /**" + attr.Comment + "*/\n"
        data += "    " + attr.Type + " i" + attr.Name + ";\n"

    template = ""
    if classdata.Type == STRUCT:
        template = STRUCT_DECLARATION_TEMPLATE
    else:
        template = CLASS_DECLARATION_TEMPLATE

    Comment( classdata.Comment )

    #comment = classdata.Comment
    #cog.outl
    data = template % { "DATA" : data.rstrip(), "NAME" : name}
    cog.outl( data )


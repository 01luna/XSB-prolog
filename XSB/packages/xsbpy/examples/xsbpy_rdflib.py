
# Warning!!!  As written here, the interface translates
# rdflib.term.Literal objects as UTF-8 strings, meaning that
# attributes such as datatype and lang are not loaded into Prolog.
# 


from rdflib import *
from rdflib.plugins.serializers.n3 import *

g = Graph()                                                              
                                                                                
#def rdflib_parse(File,**Kwargs):
#    g.remove((None,None,None))
#    g.parse(File,**Kwargs)
#    return(list(g))                                                             

def rdflib_parse(File,**Kwargs):
    g.remove((None,None,None))
    g.parse(File,**Kwargs)
    strlist = {str(Elt) for elt in list(g)}
    return(strlist)

# for practical use, a more sophisticated approach is necessary.  In
# particular, the transformation of the third position into nt
# literals does not preserve languate tags such as @en, or type tags
# such as ^^<http://www.w3.org/2001/XMLSchema#double>.  Possibly the rdflib 
# N3Serializer class could be used.
def rdflib_serialize_file(inlist, File,**Kwargs):
    g.remove((None,None,None))
    for (s,p,o) in inlist:
        g.add((URIRef(s),URIRef(p),Literal(o)))
    with open(File,"w") as fp:
        print(g.serialize(**Kwargs).decode("utf-8"),file = fp)

b

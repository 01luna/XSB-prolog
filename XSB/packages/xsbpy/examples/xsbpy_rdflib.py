# Warning!!!  This is a sample interface, and should only serve as an
# illustration of how an interface might be written.  There are no known
# bugs, but it should be thoroughly tested.
#
# This is also not an example of writing good Python code.
#
# This interface works by translating the elements of an rdflib graph
# to prolog data structures.  Conceptually:#F
# First, any rdflib.Literal objects are translated to 2-tuples (URLRefs
# and Bnodes are unaffected)
# Second, each triple in the rdflib.Graph are translated to 3-tuples.
# Thus the graph is conveyed to Prolog as a list of 3-tuples, each of
# whose arguments may be a string or 2-tuple.
# When translating to Python from Prolog, literals, triples and a graph
# are created from lists of the above form.
@ 
# This has been tested out on N-triple and Turtle files.

from rdflib import *

# leaves non-literals unaffected.
def rdflib_term_to_tuple(o):
    if type(o) == Literal:
        if o.language:
            lang = o.language
        else:
            lang = ''
        if o.datatype:
            dt = o.datatype
        else:
            dt = ''
        onew = (str(o),dt,lang)
    else:
        onew = o
    return(onew)

def graph_to_tuples(gr):
    glist = []
    for (s,p,o) in gr:
        snew = rdflib_term_to_tuple(s)
        pnew = rdflib_term_to_tuple(p)
        onew = rdflib_term_to_tuple(o)
        glist.append((snew,pnew,onew))
    return(glist)

def rdflib_parse(File,**Kwargs):
    g = Graph()
    g.parse(File,**Kwargs)
    return(graph_to_tuples(g))

#----------------------------------------

def tuples_to_graph(inlist):
    g = Graph()
    for (s,p,o) in inlist:
        print(o)
        if o[2] != '':
            olit = Literal(o[0],lang=o[2])
        elif o[1] != '':
            olit = Literal(o[0],datatype=o[1])
        else:
            olit = Literal(o[0])
        g.add((URIRef(s),URIRef(p),olit))
    return(g)

def rdflib_serialize_file(inlist, File,**Kwargs):
    g = tuples_to_graph(inlist)
    with open(File,"w") as fp:
        print(g.serialize(**Kwargs).decode("utf-8"),file = fp)


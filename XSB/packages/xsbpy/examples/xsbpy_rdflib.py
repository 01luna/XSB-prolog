
import rdflib                                                                   
g = rdflib.Graph()                                                              
                                                                                
def rdflib_parse(File,**Kwargs): 
    g.parse(File,**Kwargs)
#    print("done with parse")                                                    
    return(list(g))                                                             
                                                                                
            

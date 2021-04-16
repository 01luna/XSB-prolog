
#from json import *
import json

def prolog_loads(String):
    jdict = json.loads(String)
    return(jdict)

def prolog_dumps(dict):
    print(('pd',dict))
    jstring = json.dumps(dict)
    return(jstring)

def prolog_load(File):
    with open(File) as fileptr:
        data = json.load(fileptr)
        return(data)

def prolog_dump(Dict,File):
    with open(File,"w") as fileptr:
        ret = json.dump(Dict,fileptr)
        return(ret)

#should not be needed -- transformation now done in C.    
def dict_to_list(indict):
    originally_dict = False
    orig_struct = indict
    if type(indict) not in [dict,list,tuple]:
        return indict
    elif type(indict) is dict:
        indict = list(indict.items())
        originally_dict = True
    retstruct = []
    print("  ",end = " ")
    print(indict)
    for elt in indict:
#        print("    ",end = " ")
#        print(elt)
        if type(elt) in [dict,list,tuple]: 
            newelt = dict_to_list(elt)
        else:
            newelt = elt
        retstruct.append(newelt)
    if type(indict) is tuple:
        retstruct = tuple(retstruct)
    elif originally_dict == True:
        retstruct = ("__dict",retstruct)
    return(retstruct)

            
# Output: {'name': 'Bob', 'languages': ['English', 'Fench']}

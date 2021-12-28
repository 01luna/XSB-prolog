from xsbext import *
import sys
import time
import atexit

def myexit():
    px_close()
    print("Bye!")

atexit.register(myexit)

px_init ( )
#px_cmd('curr_sym','set_prolog_flag','heap_margin',16433152)
px_cmd('curr_sym','set_prolog_flag','heap_garbage_collection','none')

def px_consult(File):
    px_cmd('consult','consult',File)

def prolog_paths():
    return px_query('px_test','prolog_paths')
    
def add_prolog_path(List):
    px_cmd('px_test','append_prolog_paths',List)
    

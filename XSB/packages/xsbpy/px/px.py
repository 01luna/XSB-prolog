from xsbext import *
import sys
import time
import atexit

# ================ Setup  ================
def myexit():
    px_close()
    print("XSB has been closed")

atexit.register(myexit)

px_init ( )

# TES: need to fix
px_cmd('curr_sym','set_prolog_flag','heap_garbage_collection','none')

px_cmd('consult','consult','px')
px_cmd('consult','consult','px_test')

# ================ Utils  ================

def ensure_loaded(File):
    px_cmd('consult','ensure_loaded',File)

def consult(File):
    px_cmd('consult','consult',File)

def prolog_paths():
    return px_query('px_test','prolog_paths')
    
def add_prolog_path(List):
    px_cmd('px_test','append_prolog_paths',List)
    
# ================ Pretty Printing  ================
# Gives a Prolog-like echo to calls and writes answers in a Prolog-like manner

def pp_px_query(Module,Pred,*args):
    try: 
        if len(args) == 0:
            print('?- '+Module+':'+Pred+'(Answer).')
        else: 
            print('?- '+Module+':'+Pred+'('+str(args)+',Answer).')
        print('')
        Tup = px_query(Module,Pred,*args)
        if Tup != 0:
            print('   Answer  = ' + str(Tup[0]))
            print('   TV = ' + printable_tv(Tup[1]))
        else:
            print('   TV = ' + printable_tv(Tup))
        print('')
    except Exception as err:
        display_xsb_error(err)
        print('')
    
def pp_px_comp(Module,Pred,*args):
    try: 
        if len(args) == 0:
            print('?- comprehension('+Module+':'+Pred+'(_),Answer.')
        else: 
            print('?- '+Module+':'+Pred+'('+str(args)+',Answer).')
        print('')
        Tup = px_comp(Module,Pred,*args)
        if Tup != 0:
            print('   Answer  = ' + str(Tup[0]))
            print('   TV = ' + printable_tv(Tup[1]))
        else:
            print('   TV = ' + printable_tv(Tup))
        print('')
    except Exception as err:
        display_xsb_error(err)
        print('')
    
def pp_px_cmd(Module,Pred,*args):
    try:
        Ret = px_cmd(Module,Pred,*args)
        print('?- '+Module+':'+Pred+'('+str(*args)+')')
        print('')
        print('   TV = ' + printable_tv(Ret))
        print('')
    except Exception as err:
        display_xsb_error(err)
    
def display_xsb_error(err):    
        print('Exception Caught from XSB: ')
#        print('   ' + str(err))
        print('      ' + px_get_error_message())

def printable_tv(TV):
    if TV == 1:
        return('True')
    elif TV == 0:
        return('False')
    else:
        return('Undefined')


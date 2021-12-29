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


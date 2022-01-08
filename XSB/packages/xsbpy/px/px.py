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

def pp_px_comp(Module,Pred,*args,**kwargs):
    if 'vars' in kwargs:
        varnum = kwargs.get('vars')
    else:
        varnum = 1
    try:
        if kwargs != {}:
            print(kwargs)
        print_comp_goal(Module,Pred,varnum,*args)
        Ret = px_comp(Module,Pred,*args,**kwargs)
        print_comp_answer(Ret)
    except Exception as err:
        display_xsb_error(err)
        print('')

def print_comp_goal(Module,Pred,varnum,*args):
    print('?- px_comp('+Module+':'+Pred+'(',end="")
    argnum = len(args)
    for i in range(0,argnum-1):
        print(str(args[i])+',',end = "")
    if argnum > 0 and varnum==0:
        print(str(args[argnum-1])+'),Answer).',end = "")
        return
    elif argnum>0:
        print(str(args[argnum-1])+',',end = "")
    if varnum > 0:
        for i in range(0,varnum-1):
            print('_,',end = "")
        print('_',end = "")
    print('),Answer).')
        
def print_comp_answer(Ret):
    print_term(Ret,5)

def tab(N):
    print(N*' ',end='')
    
def print_term(Term,Offset):
    if type(Term) is tuple:
        print_tuple(Term,Offset)
    elif type(Term) is list:
        tab(Offset)
        print('[')
        for i in range(0,len(Term)):
            print_term_tup(Term[i],(Offset+1))
            if i < len(Term)-1:
                print(',')
            else:
                print(' ')
                tab(Offset)
                print(']')
    elif type(Term) is set:
        tab(Offset)
        print('{')
        i=0
        for setelt in Term:
            print_term_tup(setelt,(Offset+1))
            if i < len(Term)-1:
                print(',')
                i = i+1
            else:
                print(' ')
                tab(Offset)
                print('}')
    else:
        tab(Offset)
        print(Term)

def print_term_tup(Term,Offset):
    if type(Term) is tuple:
        print_tuple(Term,Offset)
    elif type(Term) is list:
        print('[',end="")
        for elt in Term:
            print_term_tup(elt,0)
        print(']',end="")
    else:
        print(Term,end="")

def print_tuple(Tup,Offset):
    if Tup[0] == 'plgTerm':
        print(Tup[1],end="")
        print_tuple(Tup[2:],0)
    else:
        tab(Offset)
        print('(',end="")
        for i in range(0,len(Tup)):
            print_term_tup(Tup[i],0)
            if i != len(Tup)-1:
                print(',',end='')
        print(')',end="")          

        
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


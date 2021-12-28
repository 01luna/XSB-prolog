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
px_cmd('consult','consult','px_test')

def px_consult(File):
    px_cmd('consult','consult',File)

def prolog_paths():
    return px_query('px_test','prolog_paths')
    
def add_prolog_path(List):
    px_cmd('px_test','append_prolog_paths',List)
    
add_prolog_path(['../../../../xsbtests/attv_tests'])

def printable_tv(TV):
    if TV == 1:
        return('True')
    elif TV == 0:
        return('False')
    else:
        return('Undefined')

def print_query(Tup): 
    print('   X = ' + str(Tup[0]))
    print('   TV = ' + printable_tv(Tup[1]))
    
def tpx():
    print('------------ command: arity 1 -------------')
    print('calling consult:consult(px_test)')
    print('     TV = ' + printable_tv(px_cmd('consult','consult','px_test')))
    print('')
    print('calling px_test:win(0)')
    print('     TV = ' + printable_tv(px_cmd('px_test','win',0)))
    print('')
    print('calling px_test:one_ary_fail(p)')
    print('     TV = ' + printable_tv(px_cmd('px_test','one_ary_fail','p')))
    print('')
    print('calling px_test:instan(b) (should fail)')
    print('     TV = ' + printable_tv(px_cmd('px_test','instan','b')))
    print('')
    print('----------- command arity 0 --------------')
    print('calling px_test:zero_ary_true()')
    print('     TV = ' + printable_tv(px_cmd('px_test','zero_ary_true')))
    print('')
    print('calling px_test:zero_ary_fail() (should fail)')
    print('     TV = ' + printable_tv(px_cmd('px_test','zero_ary_fail')))
    print('')
    print('calling px_test:zero_ary_undef()')
    print('     TV = ' + printable_tv(px_cmd('px_test','zero_ary_undef')))
    print('')
    print('----------- query: arity 1 --------------')
    print('calling px_test:one_ary_undef(X)')
    print_query(px_query('px_test','one_ary_undef'))
    print('')
    print('calling px_test:instan(X)')
    print_query(px_query('px_test','instan'))
    print('')
    print('calling px_test:one_ary_fail(X)')
    print('     TV = ' + printable_tv(px_query('px_test','one_ary_fail')))
    print('')
    print('calling test_iteration_cmd(200000)')
    test_iteration_cmd(200000)
    print('')
    print('calling test_iteration_nondet(200000)')
    test_iteration_nondet(200000)
    print('')
    
    print('------------ query: arity 2 -------------')
    print('calling basics:reverse([1,2,3,{a:{b:c}},X)')
    print_query(px_query('basics','reverse',[1,2,3,{'a':{'b':'c'}}]))
    print('')
    mylist = makelist(100000)
    print('getting the length of List = makelist(100000)')
    start = time.time()
    print_query(px_query('basics','length',mylist))
    end = time.time()
    print((end-start))
    print('')
    print('calling string:concat_atom([a,b,c,d,e,f,g],X)')
    print_query(px_query('string','concat_atom',['a','b','c','d','e','f','g']))
    print('')
    print('calling test_iteration_query(200000)')
    test_iteration_query(200000)
    print('')
    print('calling prolog_makelist(1000000)')
    start = time.time()
    px_query('px_test','prolog_makelist',1000000)    
    end = time.time()
    print((end-start))
    print('')
    print('------------ query: arity 3 -------------')
    print('calling basics:append([1,2],[3,4],X)')
    print_query(px_query('basics','append',[1,2],[3,4]))

    print('----------- undef error --------------')
    try: 
        print('Try calling: px_query(nomod,nopred,1)')
        px_query('nomod','nopred',1)
    except Exception as err:
        display_xsb_error(err)
        
#    except ChildProcessError as err:
    print('----------- user file error --------------')
    try:
        print('Try : px_query(px_test,throw_an_error,here is an error thrown from Prolog)')
        px_query('px_test','throw_an_error','here is an error thrown from Prolog')
    except Exception as err:
        display_xsb_error(err)    
    print('----------- testi~ng interrupts --------------')
    test_interrupts()
    print('----------- done with test --------------')

def test_interrupts():
    px_cmd('px_test','tc_rep_max') 
    px_cmd('consult','consult','attv_test')
    px_cmd('usermod','test')
    
def display_xsb_error(err):    
        print('Exception Caught from XSB: ')
        print('   ' + str(err))
        print('      ' + px_get_error_message())

def makelist(N):
    list = []
    for i in range(1,N):
        list.append(i)
    return list
       
def list_retest():
    mylist = makelist(100000000)
    print('getting the length of List = makelist(2000000)')
    start = time.time()
    print_query(px_query('basics','length',mylist))
    end = time.time()
    print((end-start))

def test_iteration(N):
    Start = time.time()
    test_iteration_1(N)
    End = time.time()
    print((End-Start))

def test_iteration_query(N):
    px_cmd('consult','consult','px_test')
    Start = time.time()
    test_iteration_query_1(N)
    End = time.time()
    print('test_iteration_query('+str(N)+'): '+str(End-Start))
    
def test_iteration_cmd(N):
    px_cmd('consult','consult','px_test')
    Start = time.time()
    test_iteration_cmd_1(N)
    End = time.time()
    print('test_iteration_cmd('+str(N)+'): '+str(End-Start))
    
def test_iteration_nondet(N):
    px_cmd('consult','consult','px_test')
    Start = time.time()
    test_iteration_nondet_1(N)
    End = time.time()
    print('test_iteration_nondet('+str(N)+'): '+str(End-Start))
    
def test_iteration_1(N):
    for i in range(1,N):
        pass

def test_iteration_query_1(N):
    for i in range(1,N):
        px_query('px_test','simple_call',N)
    
def test_iteration_cmd_1(N):
    for i in range(1,N):
        px_cmd('px_test','simple_cmd',N)
    
def test_iteration_nondet_1(N):
    for i in range(1,N):
        px_query('px_test','nondet_query')

def px_list(call,tup):
    [mod,pred] = call.split('.')
    print('px_query('+mod+','+'pred'+','+str(tup)+')')
    

from px import *

px_cmd('consult','consult','px_test')

add_prolog_path(['../../../../xsbtests/attv_tests'])

def tpx():
    print('------------ command: arity 1 -------------')
    print('')
    pp_px_cmd('px_test','win',0)
    pp_px_cmd('px_test','one_ary_fail','p')
    pp_px_cmd('px_test','instan','b')
    print('----------- command arity 0 --------------')
    pp_px_cmd('px_test','zero_ary_true')
    pp_px_cmd('px_test','zero_ary_fail')
    pp_px_cmd('px_test','zero_ary_undef')
    print('----------- query: arity 1 --------------')
    pp_px_query('px_test','one_ary_undef')
    pp_px_query('px_test','instan')
    pp_px_query('px_test','one_ary_fail')
    print('calling test_iteration_cmd(200000)')
    test_iteration_cmd(200000)
    print('')
    print('calling test_iteration_nondet(200000)')
    test_iteration_nondet(200000)
    print('')
    print('------------ query: arity 2 -------------')
    print('calling basics:reverse([1,2,3,{a:{b:c}},X)')
    pp_px_query('basics','reverse',[1,2,3,{'a':{'b':'c'}}])
    print('')
    mylist = makelist(100000)
    print('getting the length of List = makelist(100000)')
    start = time.time()
    px_query('basics','length',mylist)
    end = time.time()
    print((end-start))
    print('')
    print('calling string:concat_atom([a,b,c,d,e,f,g],X)')
    pp_px_query('string','concat_atom',['a','b','c','d','e','f','g'])
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
    pp_px_query('basics','append',[1,2],[3,4])

    print('----------- undef error --------------')
    pp_px_query('nomod','nopred',1)
        
#    except ChildProcessError as err:
    print('----------- user file error --------------')
    pp_px_query('px_test','throw_an_error','here is an error thrown from Prolog')
    print('----------- testi~ng interrupts --------------')
    test_interrupts()
    print('----------- done with test --------------')

def test_interrupts():
    px_cmd('px_test','tc_rep_max') 
    px_cmd('consult','consult','attv_test')
    px_cmd('usermod','test')

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
    

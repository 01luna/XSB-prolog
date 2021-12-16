from xsbext import *
import sys

pyxsb_init ( )
#px_cmd('curr_sym','set_prolog_flag','heap_margin',16433152)
    
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
    
    print('------------ query: arity 2 -------------')
    print('calling basics:reverse([1,2,3,{a:{b:c}},X)')
    print_query(px_query('basics','reverse',[1,2,3,{'a':{'b':'c'}}]))
    print('')
    mylist = makelist(2000000)
    print('getting the length of List = makelist(2000000)')
    print_query(px_query('basics','length',mylist))
    print('')
    print('calling string:concat_atom([a,b,c,d,e,f,g],X)')
    print_query(px_query('string','concat_atom',['a','b','c','d','e','f','g']))

    print('------------ query: arity 3 -------------')
    print('calling basics:append([1,2],[3,4],X)')
    print_query(px_query('basics','append',[1,2],[3,4]))

    print('----------- undef error --------------')
    try: 
        print('Try calling: px_query(nomod,nopred,1)')
        px_query('nomod','nopred',1)
    except Exception as err:
        print('Python Caught: ')
        print(err)
#        print(sys.exec_info())
#        print(err.message)
        
#    except ChildProcessError as err:
    print('----------- user file error --------------')
    try:
        print('Try calling: px_query(px_test,throw_an_error,here is an error thrown from Prolog)')
        px_query('px_test','throw_an_error','here is an error thrown from Prolog')
    except Exception as err:
        print('Python Caught: ')
        print(err)
        
    print('----------- done with test --------------')

def makelist(N):
    list = []
    for i in range(1,N):
        list.append(i)
    return list
       

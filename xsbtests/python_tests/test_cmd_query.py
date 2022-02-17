import sys
sys.path.insert(0,'../../XSB/packages/xsbpy/px')
from px import *

orig_output = sys.stdout 
sys.stdout = open('./temp', 'w')

px_cmd('consult','ensure_loaded','px_test')

def test_cmd_query():
    print('------------ command: arity 1 -------------')
    pp_px_cmd('px_test','win',0)
    pp_px_cmd('px_test','one_ary_fail','p')
    pp_px_cmd('px_test','instan','b')
    print('----------- command arity 0 --------------')
    pp_px_cmd('px_test','zero_ary_true')
    pp_px_cmd('px_test','zero_ary_fail')
    pp_px_cmd('px_test','zero_ary_undef')
    print('----------- query: arity 1 --------------')
    pp_px_qdet('px_test','one_ary_undef')
    pp_px_qdet('px_test','instan')
    pp_px_qdet('px_test','one_ary_fail')
    pp_px_qdet('px_test','return_tuple')
    pp_px_qdet('px_test','return_term')
    print('------------ query: arity 2 -------------')
    pp_px_qdet('basics','reverse',[1,2,3,{'a':{'b':'c'}}])
    pp_px_qdet('string','concat_atom',['a','b','c','d','e','f','g'])
    print('------------ query: arity 3 -------------')
    pp_px_qdet('basics','append',[1,2],[3,4])
#    print('----------- testing interrupts --------------')
#    test_interrupts()
    print('----------- done with test_cmd_query --------------')

test_cmd_query()

sys.stdout = orig_output

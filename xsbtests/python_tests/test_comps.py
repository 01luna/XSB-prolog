import sys
sys.path.insert(0,'../../XSB/packages/xsbpy/px')
from px import *

orig_output = sys.stdout 
sys.stdout = open('./temp', 'w')

print('Expect slight differences between (...) and (...), due to set creation in Python')

def test_comps():
    pp_px_comp('px_test','test_comp')
    print('------------------------')
    pp_px_comp('px_test','test_comp','e')
    print('------------------------')
    pp_px_comp('px_test','test_comp',vars=2)
    print('------------------------')
    pp_px_comp('px_test','test_comp',truth_vals=PLAIN_TRUTHVALS)
    print('------------------------')

    pp_px_comp('px_test','test_comp',set_collect=True)
    print('------------------------')
    pp_px_comp('px_test','test_comp','e',set_collect=True)
    print('------------------------')
    pp_px_comp('px_test','test_comp',vars=2,set_collect=True)
    print('------------------------')

    pp_px_comp('px_test','test_comp',truth_vals=DELAY_LISTS)
    print('------------------------')
    pp_px_comp('px_test','test_comp','e',truth_vals=DELAY_LISTS)
    print('------------------------')
    pp_px_comp('px_test','test_comp',vars=2,truth_vals=DELAY_LISTS)
    print('------------------------')

    pp_px_comp('px_test','test_comp',truth_vals=NO_TRUTHVALS)
    print('------------------------')
    pp_px_comp('px_test','test_comp','e',truth_vals=NO_TRUTHVALS)
    print('------------------------')
    pp_px_comp('px_test','test_comp',vars=2,truth_vals=NO_TRUTHVALS)
    print('------------------------')

    pp_px_comp('px_test','table_comp')
    print('------------------------')
    pp_px_comp('px_test','table_comp','e')
    print('------------------------')
    pp_px_comp('px_test','table_comp',vars=2)
    print('------------------------')
    pp_px_comp('px_test','table_comp','e',truth_vals=PLAIN_TRUTHVALS)
    print('------------------------')

    pp_px_comp('px_test','table_comp',set_collect=True)
    print('------------------------')
    pp_px_comp('px_test','table_comp','e',set_collect=True)
    print('------------------------')
    pp_px_comp('px_test','table_comp',vars=2,set_collect=True)
    print('------------------------')

    pp_px_comp('px_test','table_comp',truth_vals=DELAY_LISTS)
    print('------------------------')
    pp_px_comp('px_test','table_comp','e',truth_vals=DELAY_LISTS)
    print('------------------------')
    pp_px_comp('px_test','table_comp',vars=2,truth_vals=DELAY_LISTS)
    print('------------------------')

    pp_px_comp('px_test','table_comp',truth_vals=NO_TRUTHVALS)
    print('------------------------')
    pp_px_comp('px_test','table_comp','e',truth_vals=NO_TRUTHVALS)
    print('------------------------')
    pp_px_comp('px_test','table_comp',vars=2,truth_vals=NO_TRUTHVALS)
    print('------------------------')

test_comps()

sys.stdout = orig_output

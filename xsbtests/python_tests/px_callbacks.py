import sys
sys.path.insert(0,'../../XSB/packages/xsbpy/px')
from px import *

orig_output = sys.stdout 
sys.stdout = open('./temp', 'w')

consult('xp_unicode')
consult('px_callbacks')
print('xp_unicode:unicode_upper: '
      + str(px_qdet('xp_unicode','unicode_upper','Η Google μου το μετέφρασε')))
print('px_callbacks:test_json: ' + str(px_qdet('px_callbacks','test_json')))
NewClass,TV = px_qdet('px_callbacks','test_class','joe')
print('px_callbacks','test_class: ' + str(NewClass.name))

sys.stdout = orig_output

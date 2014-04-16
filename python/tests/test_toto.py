import sys, os

print sys.path
print os.environ.get('PYTHONPATH', "")

import toto

# Element constructed on the Python side
print "** Test 1 ...**"
elt = toto.PyMyElement ("toto")
assert elt.get_name() == "toto"
print "** Test 1 OK **"

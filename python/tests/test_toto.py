import os
import utils

try:
    import toto
except ImportError as exc:
    # Add information regarding PYTHONPATH to the exception
    exc.args = exc.args + (
        'PYTHONPATH=%s' % os.environ.get('PYTHONPATH', ''),
        'CONFIG:%s' % (utils.config,)
    )
    raise


# Element constructed on the Python side
print "** Test 1 ...**"
elt = toto.PyMyElement ("toto")
# NB An AssertionError will exit the interpreter with status code 1
assert elt.get_name() == "toto"
print "** Test 1 OK **"

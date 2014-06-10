import os
import utils

TEST_OUTPUT_REDIRECTED = 'test_toto_out.log'
TEST_ERRORS_REDIRECTED = 'test_toto_err.log'
# TEST_OUTPUT_REDIRECTED = os.devnull


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


with utils.no_output(to=TEST_OUTPUT_REDIRECTED, err_to=TEST_ERRORS_REDIRECTED):
    elt = toto.PyMyElement ("toto")

# NB An AssertionError will exit the interpreter with status code 1
assert elt.get_name() == "toto"
print "** Test 1 OK **"

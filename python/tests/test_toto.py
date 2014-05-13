import sys, os, os.path as osp

_HERE = osp.realpath(osp.dirname(__file__))
_PROJECT_BASE = osp.abspath(osp.join(_HERE, '..', '..'))

_CYTHON_EXTENSION_DIR = {
    'Release' : osp.abspath(osp.join(_PROJECT_BASE, 'python', 'cython')),
    'Debug'   : osp.abspath(osp.join(_PROJECT_BASE, 'python', 'cython_d'))
}

config = len(sys.argv) > 1 and sys.argv[1]
if config in _CYTHON_EXTENSION_DIR:
        sys.path.append(_CYTHON_EXTENSION_DIR[config])
else:
    print 'WARNING: no config specification or unknown config (%s)' % config


try:
    import toto
except ImportError as exc:
    # Add information regarding PYTHONPATH to the exception
    exc.args = exc.args + (
        'PYTHONPATH=%s' % os.environ.get('PYTHONPATH', ''),
        'CONFIG:%s' % (config,)
    )
    raise


# Element constructed on the Python side
print "** Test 1 ...**"
elt = toto.PyMyElement ("toto")
# NB An AssertionError will exit the interpreter with status code 1
assert elt.get_name() == "toto"
print "** Test 1 OK **"

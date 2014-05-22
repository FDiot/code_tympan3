"""Ease paths configuration for test in Python

Allow to access the test data, solvers and other resources in a cross
platform way (the relative path to use between a test executable and
the resources is not the same on Linux and Windows) by exporting the
following constants:

PROJECT_BASE
    absolute path to the root of the Code_TYMPAN build tree

TEST_DATA_DIR
    absolute path to the directory holding the test data

TEST_SOLVERS_DIR
    absolute path to the directory holding the solvers

Handle the configuration of sys.path to find either the Debug or
Release the cython module.

THIS EXPECTS THE NAME OF THE BUILD CONFIG TO BE TESTED IS PASSED
AS FIRST ARGUMENT TO THE SCRIPT
"""

import sys
import os
import os.path as osp


_HERE = osp.realpath(osp.dirname(__file__))
PROJECT_BASE = osp.abspath(osp.join(_HERE, '..', '..'))

TEST_DATA_DIR = osp.join(PROJECT_BASE, 'tests', 'data')
assert osp.isdir(TEST_DATA_DIR), "The test data dir does not exists '%s'" % TEST_DATA_DIR

TEST_SOLVERS_DIR = osp.join(PROJECT_BASE, 'pluginsd')
if not osp.isdir(TEST_SOLVERS_DIR):
    solver_dir = osp.abspath(osp.join(_HERE, '..', '..', 'plugins'))
assert osp.isdir(TEST_SOLVERS_DIR), "The test solver plugins dir does not exists '%s'" % TEST_SOLVERS_DIR

_SOLVERS_DIR = {
    'Release' : osp.abspath(osp.join(PROJECT_BASE, 'plugins')),
    'Debug'   : osp.abspath(osp.join(PROJECT_BASE, 'pluginsd'))
}

_CYTHON_EXTENSION_DIR = {
    'Release' : osp.abspath(osp.join(PROJECT_BASE, 'python', 'cython')),
    'Debug'   : osp.abspath(osp.join(PROJECT_BASE, 'python', 'cython_d'))
}

assert frozenset(_SOLVERS_DIR) == frozenset(_CYTHON_EXTENSION_DIR)
_KNOWN_CONFIGURATIONS = frozenset(_SOLVERS_DIR)

config = len(sys.argv) > 1 and sys.argv[1]
if config in _KNOWN_CONFIGURATIONS:
    TEST_CYTHON_EXTENSION_DIR = _CYTHON_EXTENSION_DIR[config]
    assert osp.isdir(TEST_CYTHON_EXTENSION_DIR ), "The test cython dir does not exists '%s'" % TEST_CYTHON_EXTENSION_DIR
    sys.path.append(TEST_CYTHON_EXTENSION_DIR)
    TEST_SOLVERS_DIR = _SOLVERS_DIR[config]
    assert osp.isdir(TEST_SOLVERS_DIR), "The test solver plugins dir does not exists '%s'" % TEST_SOLVERS_DIR
else:
    print 'WARNING: no config specification or unknown config (%s)' % config


def main():
    import unittest
    # The build configuration (Debug or Release) to be tested is added as
    # first argument by CTest and need to be removed from sys.argv not to
    # confuse unittest
    if config:
        del sys.argv[1]
    unittest.main()


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
import unittest
from contextlib import contextmanager
import numpy as np

_HERE = osp.realpath(osp.dirname(__file__))
PROJECT_BASE = osp.abspath(osp.join(_HERE, '..', '..'))

TEST_DATA_DIR = osp.join(PROJECT_BASE, 'tests', 'data')
assert osp.isdir(TEST_DATA_DIR), "The test data dir does not exists '%s'" % TEST_DATA_DIR

TEST_SOLVERS_DIR = osp.join(PROJECT_BASE, 'pluginsd')
if not osp.isdir(TEST_SOLVERS_DIR):
    TEST_SOLVERS_DIR = osp.abspath(osp.join(_HERE, '..', '..', 'plugins'))
assert osp.isdir(TEST_SOLVERS_DIR), "The test solver plugins dir does not exists '%s'" % TEST_SOLVERS_DIR

TEST_PROBLEM_DIR = osp.join(TEST_DATA_DIR, 'projects-panel')
assert osp.isdir(TEST_PROBLEM_DIR), "The test problem dir does not exists '%s'" % TEST_PROBLEM_DIR

TEST_RESULT_DIR = osp.join(TEST_DATA_DIR, 'expected')
assert osp.isdir(TEST_RESULT_DIR), "The test result dir does not exists '%s'" % TEST_RESULT_DIR

_SOLVERS_DIR = {
    'Release' : osp.abspath(osp.join(PROJECT_BASE, 'plugins')),
    'Debug'   : osp.abspath(osp.join(PROJECT_BASE, 'pluginsd'))
}

_CYTHON_EXTENSION_DIR = {
    'Release' : osp.abspath(osp.join(PROJECT_BASE, 'cython')),
    'Debug'   : osp.abspath(osp.join(PROJECT_BASE, 'cython_d'))
}

assert frozenset(_SOLVERS_DIR) == frozenset(_CYTHON_EXTENSION_DIR)
_KNOWN_CONFIGURATIONS = frozenset(_SOLVERS_DIR)

CONFIG = len(sys.argv) > 1 and sys.argv[1] in _KNOWN_CONFIGURATIONS and sys.argv[1]

def config_cython_extensions_path():
    if CONFIG:
        TEST_CYTHON_EXTENSION_DIR = _CYTHON_EXTENSION_DIR[CONFIG]
        assert osp.isdir(TEST_CYTHON_EXTENSION_DIR ), "The test cython dir does not exists '%s'" % TEST_CYTHON_EXTENSION_DIR
        sys.path.append(TEST_CYTHON_EXTENSION_DIR)
        TEST_SOLVERS_DIR = _SOLVERS_DIR[CONFIG]
        assert osp.isdir(TEST_SOLVERS_DIR), "The test solver plugins dir does not exists '%s'" % TEST_SOLVERS_DIR
    else:
        print 'WARNING: no config specification or unknown config'

def compare_floats(x, y):
    """ Compare x and y which are float arrays:
        Go through them, considering x[i] and y[i] equal up to a 3 decimal
        precision. Then when they sufficiently differ, return -1 if x[i] < y[i]
        and 1 otherwise. 0 is returned if the arrays are equal.
    """
    from itertools import izip
    for xi, yi in izip(x, y):
        if not np.allclose(xi, yi, atol=1e-03):
            if xi < yi:
                return -1
            else:
                return 1
    return 0 # arrays are equal

def main():
    # The build configuration (Debug or Release) to be tested is added as
    # first argument by CTest and need to be removed from sys.argv not to
    # confuse unittest
    if CONFIG:
        del sys.argv[1]
    unittest.main()


# low level output redirection, see
# http://stackoverflow.com/questions/4675728/redirect-stdout-to-a-file-in-python/22434262#22434262

def fileno(file_or_fd):
    fd = getattr(file_or_fd, 'fileno', lambda: file_or_fd)()
    if not isinstance(fd, int):
        raise ValueError("Expected a file (`.fileno()`) or a file descriptor")
    return fd

@contextmanager
def stdout_redirected(to=os.devnull, stdout=None):
    if stdout is None:
       stdout = sys.stdout

    stdout_fd = fileno(stdout)
    # copy stdout_fd before it is overwritten
    with os.fdopen(os.dup(stdout_fd), 'wb') as copied:
        stdout.flush()  # flush library buffers that dup2 knows nothing about
        try:
            os.dup2(fileno(to), stdout_fd)  # $ exec >&to
        except ValueError:  # filename
            with open(to, 'ab') as to_file:
                os.dup2(to_file.fileno(), stdout_fd)  # $ exec > to
        try:
            yield stdout # allow code to be run with the redirected stdout
        finally:
            stdout.flush()
            os.dup2(copied.fileno(), stdout_fd)  # $ exec >&copied

@contextmanager
def no_output(to=os.devnull, err_to=None):
    if err_to is None :
        err_to = to
    with stdout_redirected(to=to, stdout=sys.stdout):
        with stdout_redirected(to=err_to, stdout=sys.stderr):
            yield


class TympanTC(unittest.TestCase):

    @classmethod
    @contextmanager
    def no_output(cls):
        stdout_log = '%s_stdout.log' % (cls.__name__)
        stderr_log = '%s_stderr.log' % (cls.__name__)
        with no_output(to=stdout_log, err_to=stderr_log):
            yield

    def load_project(self, *path):
        with self.no_output():
            import tympan.models.business as tybusiness
            import tympan.business2solver as bus2solv
            from tympan.altimetry.builder import Builder
            from tympan import Simulation
            from tympan.altimetry._export import _build_mesh_data
            tybusiness.init_tympan_registry()
            # read acoustic project
            project = tybusiness.Project.from_xml(osp.join(TEST_DATA_DIR, *path))
            site = project.site
            # Build a Simulation and compute altimetry.
            sml = Simulation(project)
            _, mesh, material_by_face = sml.altimetry()
            vertices, faces, materials, faces_materials = _build_mesh_data(
                mesh, material_by_face)
            site.update_altimetry(vertices, faces, materials, faces_materials)
            project.update()
            # build solver model
            comp = project.current_computation
            bus2solv_conv = bus2solv.Business2SolverConverter(comp, project.site)
            bus2solv_conv.build_solver_problem()
        return project, bus2solv_conv

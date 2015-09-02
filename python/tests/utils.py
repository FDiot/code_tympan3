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
"""

import sys
import os
import os.path as osp
import unittest
from contextlib import contextmanager
import numpy as np
from functools import cmp_to_key

# Environment variables.

_HERE = osp.realpath(osp.dirname(__file__))
PROJECT_BASE = osp.abspath(osp.join(_HERE, '..', '..'))

TEST_DATA_DIR = osp.join(PROJECT_BASE, 'tests', 'data')
assert osp.isdir(TEST_DATA_DIR), "The test data dir does not exists '%s'" % TEST_DATA_DIR

TEST_SOLVERS_DIR = os.environ.get('TYMPAN_SOLVERDIR')
if not TEST_SOLVERS_DIR:
    for d in ('pluginsd', 'plugins'):
        TEST_SOLVERS_DIR = osp.join(PROJECT_BASE, d)
        if osp.isdir(TEST_SOLVERS_DIR) and os.listdir(TEST_SOLVERS_DIR):
            break
    else:
        raise RuntimeError("The test solver plugins dir wasn't found")

TEST_PROBLEM_DIR = osp.join(TEST_DATA_DIR, 'projects-panel')
assert osp.isdir(TEST_PROBLEM_DIR), "The test problem dir does not exists '%s'" % TEST_PROBLEM_DIR

TEST_RESULT_DIR = osp.join(TEST_DATA_DIR, 'expected')
assert osp.isdir(TEST_RESULT_DIR), "The test result dir does not exists '%s'" % TEST_RESULT_DIR


# Utilities.

def compare_floats(x, y):
    """ Compare x and y which are float arrays:
        Go through them, considering x[i] and y[i] equal up to a 3 decimal
        precision. Then when they sufficiently differ, return -1 if x[i] < y[i]
        and 1 otherwise. 0 is returned if the arrays are equal.
    """
    for xi, yi in zip(x, y):
        if not np.allclose(xi, yi, atol=1e-03):
            if xi < yi:
                return -1
            else:
                return 1
    return 0 # arrays are equal

def _test_solve_with_file(test_file, testobj):
    """Run a tympan simulation on project `test_file` and compare the results with a computed
    project.
    """
    from tympan.models.project import Project
    from tympan.models.solver import Model, Solver
    # Load and solve the project
    project = testobj.load_project(osp.join(TEST_PROBLEM_DIR, test_file))
    model = Model.from_project(project)
    solver = Solver.from_project(project, TEST_SOLVERS_DIR)
    # avoid segfaults due to multithreading
    solver.nthreads = 1
    solver_result = solver.solve(model)
    project.import_result(model, solver_result)
    # Load the expected result
    result_file = osp.join(TEST_RESULT_DIR, test_file).replace('_NO_RESU', '')
    expected_result_project = Project.from_xml(result_file)
    # Compare results
    current_result = project.current_computation.result
    expected_result = expected_result_project.current_computation.result
    # Check we have the same number of receptors
    testobj.assertEqual(current_result.nreceptors, expected_result.nreceptors)
    # Check if there is a control point (TYPointControl) among the receptors
    # in the project.
    # If not, we are not interested in checking the sources since the
    # control points are the only receptors able to take into account the
    # individual contributions of the sources.
    # The sources here can be user sources, the machines and the buildings
    # (TYUserSourcePonctuelle, TYMachine, TYBatiment)
    check_nsources  = False
    for i in range (current_result.nreceptors):
        if current_result.receptor(i).is_control_point():
            testobj.assertTrue(expected_result.receptor(i).is_control_point())
            check_nsources = True
    if check_nsources:
        testobj.assertEqual(current_result.nsources, expected_result.nsources)
    current_spectra = np.array(list(current_result.spectrum(i, j).values
                                    for i in range(current_result.nreceptors)
                                    for j in range(current_result.nsources)))
    expected_spectra = np.array(list(expected_result.spectrum(i, j).values
                                    for i in range(current_result.nreceptors)
                                    for j in range(current_result.nsources)))
    if current_result.nsources + current_result.nreceptors > 1:
        # Order the two spectra lists because spectra are not always kept in the same order
        current_spectra = sorted(current_spectra, key=cmp_to_key(compare_floats))
        expected_spectra = sorted(expected_spectra, key=cmp_to_key(compare_floats))
    for i in range(len(current_spectra)):
        # All spectra must have the same number of elements
        testobj.assertEqual(current_spectra[i].size, expected_spectra[i].size)
        np.testing.assert_almost_equal(current_spectra[i],
                                       expected_spectra[i], decimal=1)



class TympanTC(unittest.TestCase):

    @classmethod
    @contextmanager
    def no_output(cls):
        stdout_log = '%s_stdout.log' % (cls.__name__)
        stderr_log = '%s_stderr.log' % (cls.__name__)
        from tympan.models import filter_output
        with filter_output(verbose=False, to=stdout_log, err_to=stderr_log):
            yield

    def load_project(self, *path):
        from tympan.models.project import Project
        # read acoustic project
        project = Project.from_xml(osp.join(TEST_DATA_DIR, *path))
        return project

    @staticmethod
    def datapath(*args):
        return osp.join(_HERE, 'data', *args)

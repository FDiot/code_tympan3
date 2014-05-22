import os, os.path as osp
import unittest

import numpy as np

from utils import no_output

with no_output():
    import pytam

_HERE = osp.realpath(osp.dirname(__file__))

_PROJECT_BASE = osp.abspath(osp.join(_HERE, '..', '..'))

_TEST_DATA_DIR = osp.join(_PROJECT_BASE, 'tests', 'data')
assert osp.isdir(_TEST_DATA_DIR), "The test data dir does not exists '%s'" % _TEST_DATA_DIR

_TEST_PROBLEM_DIR = osp.join(_TEST_DATA_DIR, 'projects-panel')
assert osp.isdir(_TEST_PROBLEM_DIR), "The test problem dir does not exists '%s'" % _TEST_PROBLEM_DIR

_TEST_RESULT_DIR = osp.join(_TEST_DATA_DIR, 'expected')
assert osp.isdir(_TEST_RESULT_DIR), "The test result dir does not exists '%s'" % _TEST_RESULT_DIR

_TEST_SOLVERS_DIR = osp.join(_PROJECT_BASE, 'pluginsd')
if not osp.isdir(_TEST_SOLVERS_DIR):
    solver_dir = osp.abspath(osp.join(_HERE, '..', '..', 'plugins'))
assert osp.isdir(_TEST_SOLVERS_DIR), "The test solver plugins dir does not exists '%s'" % _TEST_SOLVERS_DIR


class TestTympan(unittest.TestCase):
    pass


def make_test_with_file(test_file):
    """ For a TEST_xx_NO_RESU.xml file from data/project-panel, load and
    solve the corresponding project, load the expected result
    from data/expected/TEST_xx.xml and compare the computed result with
    the expected result.
    The acoustic spectrums of the results are compared one by one (identified
    by a receiver and source id), in a dB scale, with a 1 decimal definition.
    """
    def test_with_file(self):
        # Load and solve the project
        with no_output():
            project = pytam.Project.from_xml(osp.join(_TEST_PROBLEM_DIR, test_file))
            computation = project.current_computation()
            pytam.loadsolver(_TEST_SOLVERS_DIR, computation)
            result = computation.go()
        self.assertTrue(result)
        # Load the expected result
        result_file = osp.join(_TEST_RESULT_DIR, test_file).replace('_NO_RESU', '')
        with no_output():
            expected_result_project = pytam.Project.from_xml(result_file)
        # Compare results
        current_result = computation.result
        expected_result = expected_result_project.current_computation().result
        self.assertEqual(current_result.nsources, expected_result.nsources)
        self.assertEqual(current_result.nreceivers, expected_result.nreceivers)
        for i in xrange(current_result.nreceivers):
            for j in xrange(current_result.nsources):
                curr_spectrum = current_result.spectrum(i, j)
                expected_spectrum = expected_result.spectrum(i, j)
                # Both spectrums must have the same number of elements
                self.assertEqual(curr_spectrum.nvalues, expected_spectrum.nvalues)
                # Let's compare the values in dB
                curr_spectrum = curr_spectrum.to_dB()
                expected_spectrum = expected_spectrum.to_dB()
                np.testing.assert_almost_equal(curr_spectrum.values,
                                               expected_spectrum.values, decimal=1)
    return test_with_file

# Retrieve all the available "TEST_XX" xml files and make a test for each one
for test_file in os.listdir(_TEST_PROBLEM_DIR):
    if test_file.startswith('TEST_') and test_file.endswith('xml'):
        setattr(TestTympan, "test_" + test_file.split('.')[0].replace('TEST_', '').lower(),
                make_test_with_file(test_file))

if __name__ == '__main__':
    unittest.main()

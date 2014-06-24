import os, os.path as osp
import unittest

import numpy as np

from utils import TEST_DATA_DIR, TEST_SOLVERS_DIR, TEST_PROBLEM_DIR, TEST_RESULT_DIR, no_output

TEST_OUTPUT_REDIRECTED = 'test_solve_out.log'
TEST_ERRORS_REDIRECTED = 'test_solve_err.log'

with no_output(to=TEST_OUTPUT_REDIRECTED, err_to=TEST_ERRORS_REDIRECTED):
    import pytam
    pytam.init_tympan_registry()


class TestTympan(unittest.TestCase):
    pass


def make_test_with_file(test_file):
    """ For a TEST_xx_NO_RESU.xml file from data/project-panel, load and
    solve the corresponding project, load the expected result
    from data/expected/TEST_xx.xml and compare the computed result with
    the expected result.
    The acoustic spectrums of the results are compared one by one (identified
    by a receptor and source id), in a dB scale, with a 1 decimal definition.
    """
    def test_with_file(self):
        # Load and solve the project
        with no_output(to=TEST_OUTPUT_REDIRECTED, err_to=TEST_ERRORS_REDIRECTED):
            project = pytam.Project.from_xml(osp.join(TEST_PROBLEM_DIR, test_file))
            project.update_site()
            project.update_altimetry_on_receptors()
            computation = project.current_computation
            computation.set_nthread(1) # avoid segfaults due to multithreading
            pytam.loadsolver(TEST_SOLVERS_DIR, computation)
            result = computation.go()
        self.assertTrue(result)
        # Load the expected result
        result_file = osp.join(TEST_RESULT_DIR, test_file).replace('_NO_RESU', '')
        with no_output(to=TEST_OUTPUT_REDIRECTED, err_to=TEST_ERRORS_REDIRECTED):
            expected_result_project = pytam.Project.from_xml(result_file)
        # Compare results
        current_result = computation.result
        expected_result = expected_result_project.current_computation.result
        # Check we have the same number of receptors
        self.assertEqual(current_result.nreceptors, expected_result.nreceptors)
        # Check if there is a control point (TYPointControl) among the receptors
        # in the project.
        # If not, we are not interested in checking the sources since the
        # control points are the only receptors able to take into account the
        # individual contributions of the sources.
        # The sources here can be user sources, the machines and the buildings
        # (TYUserSourcePonctuelle, TYMachine, TYBatiment)
        check_nsources  = False
        for i in xrange (current_result.nreceptors):
            if current_result.receptor(i).is_control_point():
                self.assertTrue(expected_result.receptor(i).is_control_point())
                check_nsources = True
        if check_nsources:
            self.assertEqual(current_result.nsources, expected_result.nsources)
        # check spectrums
        for i in xrange(current_result.nreceptors):
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
for test_file in os.listdir(TEST_PROBLEM_DIR):
    if test_file.startswith('TEST_') and test_file.endswith('xml'):
        setattr(TestTympan, "test_" + test_file.split('.')[0].replace('TEST_', '').lower(),
                make_test_with_file(test_file))

if __name__ == '__main__':
    from utils import main
    main()

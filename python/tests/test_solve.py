import os, os.path as osp
import unittest
import numpy as np

from utils import (TEST_SOLVERS_DIR, TEST_PROBLEM_DIR, TEST_RESULT_DIR, TympanTC,
                   no_output, compare_floats)

with no_output():
    import tympan.models.business as tybusiness
    import tympan.business2solver as bus2solv
    from tympan.models.solver import Configuration

# avoid segfaults due to multithreading
Configuration.get().NbThreads = 1

class TestTympan(TympanTC):
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
        with self.no_output():
            (project, bus2solv_conv) = self.load_project(
                osp.join(TEST_PROBLEM_DIR, test_file))
            computation = project.current_computation
            solver = bus2solv.load_computation_solver(TEST_SOLVERS_DIR, computation)
            result = solver.solve_problem(bus2solv_conv.solver_problem, bus2solv_conv.solver_result)
            bus2solv_conv.postprocessing()
        self.assertTrue(result)
        # Load the expected result
        result_file = osp.join(TEST_RESULT_DIR, test_file).replace('_NO_RESU', '')
        with self.no_output():
            expected_result_project = tybusiness.Project.from_xml(result_file)
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
        current_spectra = np.array(list(current_result.spectrum(i, j).values
                                        for i in xrange(current_result.nreceptors)
                                        for j in xrange(current_result.nsources)))
        expected_spectra = np.array(list(expected_result.spectrum(i, j).values
                                        for i in xrange(current_result.nreceptors)
                                        for j in xrange(current_result.nsources)))
        if current_result.nsources + current_result.nreceptors > 1:
            # Order the two spectra lists because spectra are not always kept in the same order
            current_spectra = sorted(current_spectra, cmp=compare_floats)
            expected_spectra = sorted(expected_spectra, cmp=compare_floats)

        for i in xrange(len(current_spectra)):
            # All spectra must have the same number of elements
            self.assertEqual(current_spectra[i].size, expected_spectra[i].size)
            np.testing.assert_almost_equal(current_spectra[i],
                                           expected_spectra[i], decimal=1)
    return test_with_file


# Retrieve all the available "TEST_XX" xml files and make a test for each one
for test_file in os.listdir(TEST_PROBLEM_DIR):
    if test_file.startswith('TEST_') and test_file.endswith('xml'):
        setattr(TestTympan, "test_" + test_file.split('.')[0].replace('TEST_', '').lower(),
                make_test_with_file(test_file))

if __name__ == '__main__':
    from utils import main, config_cython_extensions_path
    config_cython_extensions_path()
    main()

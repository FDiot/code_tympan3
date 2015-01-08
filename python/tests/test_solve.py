import os, os.path as osp
import unittest
import numpy as np

from utils import (TEST_SOLVERS_DIR, TEST_PROBLEM_DIR, TEST_RESULT_DIR, TympanTC,
                   compare_floats)
from tympan.models.project import Project
from tympan.models.solver import Model, Solver

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
        project = self.load_project(osp.join(TEST_PROBLEM_DIR, test_file))
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


KNOWN_FAIL = (
    'test_cube_no_resu',
    'test_ecran_cube_no_resu',
    'test_ecran_no_resu',
    'test_effet_sol_no_resu',
    'test_face_no_resu',
    'test_impedance_sol_no_resu',
    'test_rayonnement_batiment_no_resu',
    'test_source_ponctuelle_no_resu',
)


# Retrieve all the available "TEST_XX" xml files and make a test for each one
for test_file in os.listdir(TEST_PROBLEM_DIR):
    if test_file.startswith('TEST_') and test_file.endswith('xml'):
        methodname = test_file.split('.')[0].lower()
        method = make_test_with_file(test_file)
        if methodname in KNOWN_FAIL:
            method = unittest.expectedFailure(method)
        setattr(TestTympan, methodname, method)


if __name__ == '__main__':
    unittest.main()

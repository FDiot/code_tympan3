import sys
import os, os.path as osp
import unittest
import re
import numpy as np
from numpy.testing import assert_allclose

from utils import (TEST_DATA_DIR, TEST_SOLVERS_DIR, TEST_RESULT_DIR, TympanTC,
                   compare_floats)
import tympan.business2solver as bus2solv

_TEST_PROBLEM_DIR = osp.join(TEST_DATA_DIR, 'computed-projects-panel')
assert osp.isdir(_TEST_PROBLEM_DIR), "The test problem dir does not exists '%s'" % _TEST_PROBLEM_DIR


class SourceAdditionTC(TympanTC):

    def test_add_sources_to_model(self):
        project = self.load_project('projects-panel', 'TEST_SOURCE_PONCTUELLE_NO_RESU.xml')
        model = bus2solv.build_solver_model(project).model
        self.assertEqual(model.nsources, 1)
        self.assertEqual(model.nreceptors, 6)
        freq = np.array([100.0] * 31, dtype=float)
        sources = [((1., 1., 0.), freq, 0), ((2., 2., 10.), freq*2, 0)]
        src_ids = []
        for (pos, spec, shift) in sources:
            src_ids.append(model.add_source(pos, spec, shift))
        self.assertEqual(model.nsources, 3)
        self.assertEqual(model.nreceptors, 6)
        src1 = model.source(src_ids[0])
        self.assertEqual(src1.position.x, 1.)
        self.assertEqual(src1.position.y, 1.)
        self.assertEqual(src1.position.z, 0.)
        assert_allclose(src1.spectrum.to_dB().values, freq)
        src2 = model.source(src_ids[1])
        self.assertEqual(src2.position.x, 2.)
        self.assertEqual(src2.position.y, 2.)
        self.assertEqual(src2.position.z, 10.)
        assert_allclose(src2.spectrum.to_dB().values, freq*2)

    def test_computation_with_manually_added_source(self):
        power_lvl = np.array([10.0] * 31, dtype=float)
        ref_proj = self.load_project('site_receptor_source.xml')
        ref_model = bus2solv.build_solver_model(ref_proj).model
        assert ref_model.nsources == 1
        assert ref_model.nreceptors == 1
        ref_src = ref_model.source(0)
        assert (ref_src.position.x, ref_src.position.y, ref_src.position.z) == (3., 3., 2.)
        assert_allclose(ref_src.spectrum.to_dB().values, power_lvl)
        solver = bus2solv.load_computation_solver(TEST_SOLVERS_DIR, ref_proj.current_computation)
        ref_result = solver.solve_problem(ref_model).spectrum(0, 0).values
        # do the same with a manually added source (the xml project is the same as
        # 'site_receptor_source.xml' except the source has been removed)
        proj = self.load_project('site_receptor.xml')
        model = bus2solv.build_solver_model(proj).model
        assert model.nsources == 0
        assert model.nreceptors == 1
        model.add_source((3., 3., 2.), power_lvl, 0)
        solver = bus2solv.load_computation_solver(TEST_SOLVERS_DIR, proj.current_computation)
        result = solver.solve_problem(model).spectrum(0, 0).values
        assert_allclose(ref_result, result)

    def test_value(self):
        proj = self.load_project('site_receptor.xml')
        model = bus2solv.build_solver_model(proj).model
        power_lvl = np.array([10., 10., 10., 15., 15., 15., 20., 20., 20., 20., 50.],
                             dtype=float)
        model.add_source((3., 3., 2.),  power_lvl, 0)
        assert model.nsources == 1
        source = model.source(0)
        self.assertAlmostEqual(source.value(16.0), 1e-11)
        self.assertAlmostEqual(source.value(63.0), 1e-11)
        self.assertAlmostEqual(source.value(100.0), 1e-10)
        self.assertEqual(source.value(160.0), 1e-7)


class TestTympan(TympanTC):
    """Place holder class to be filled with methods below"""

def make_sources_test_with_file(project_file, sources_file):
    """ For a N_*.xml file from data/computed-projects-panel, load
    the corresponding project and build an acoustic problem model. Then
    check that the sources have correctly been exported by looking at the
    expected result from data/expected/sources_positions_Project_N.txt and comparing:
        - Number of sources
        - Global positions of the sources
        - Spectrums of the sources (power spectrums, unit: pA2)
    """
    def test_with_file(self):
        """
            * For each project, positions (x, y, z) and spectrums (31 values) are
              recorded into 2D np.arrays (1 line = 1 source = 34 rows).
            * There is 1 np.array "expected_sources" for the expected result
              (read from "sources_file") and another one "real_sources"
              containing the actual values that are being checked.
            * These 2 arrays are ordered (first by coordinates, then if necessary
              by values -- see compare_floats())
            * Then the lines are compared 2 by 2
        """
        project = self.load_project(project_file)
        model = bus2solv.build_solver_model(project).model
        nexpected_sources = sum(1 for line in open(sources_file))
        # Check no sources are missing
        self.assertEqual(nexpected_sources, model.nsources)
        # Read expected result
        expected_sources = np.loadtxt(sources_file)
        # push real values to another nparray
        real_sources = np.array(list([source.position.x, source.position.y,
                                      source.position.z]
                                     + source.spectrum.values.tolist()
                                     for source in model.sources))
        if model.nsources > 1:
            # Order the two lists to be able to compare them two by two
            expected_sources = sorted(expected_sources, cmp=compare_floats)
            real_sources = sorted(real_sources, cmp=compare_floats)
        else:
            real_sources = real_sources[0]
        # Compare the sources
        assert_allclose(expected_sources, real_sources, atol=1e-03)
    return test_with_file

def make_receptors_test_with_file(project_file, receptors_file):
    """ For a N_*.xml file from data/computed-projects-panel, load
     the corresponding project and build an acoustic problem model. Then
     check that the receptors have correctly been exported by looking at the
     expected result from data/expected/receptors_positions_Project_N.txt and comparing:
        - Number of receptors
        - Global positions of the receptors
    """
    def test_with_file(self):
        """
            Operates as in make_sources_test_with_file (see above), except here
            we are just dealing with positions and not spetrums.
        """
        project = self.load_project(project_file)
        model = bus2solv.build_solver_model(project).model
        # Check no receptors are missing
        nexpected_receptors = sum(1 for line in open(receptors_file))
        self.assertEqual(nexpected_receptors, model.nreceptors)
        # Read expected result
        expected_receptors = np.loadtxt(receptors_file)
        real_receptors = np.array(list([receptor.position.x, receptor.position.y,
                                        receptor.position.z]
                                       for receptor in model.receptors))
        if model.nreceptors > 1:
            # Order the two lists to be able to compare the receptors' positions two by two
            expected_receptors = sorted(expected_receptors, cmp=compare_floats)
            real_receptors = sorted(real_receptors, cmp=compare_floats)
        else:
            real_receptors = real_receptors[0]
        # Compare the receptors
        assert_allclose(expected_receptors, real_receptors, atol=1e-03)
    return test_with_file


# Retrieve all the available "N_*" xml files and make a test for each one
for project_file in os.listdir(_TEST_PROBLEM_DIR):
    # Must start with 1 or more digits followed by '_', whatever, and end by '.xml'
    rgx = re.compile(r'([0-9])+_(.)*\.xml')
    if rgx.match(project_file) is None:
        continue
    project_file_path = osp.join(_TEST_PROBLEM_DIR, project_file)
    # Check the corresponding result files exist
    for result_file in os.listdir(TEST_RESULT_DIR):
        projnum = project_file.split('_')[0] # get project number
        # Sources
        if result_file == 'source_positions_Project_' + projnum + '.txt':
            source_file_path = osp.join(TEST_DATA_DIR, TEST_RESULT_DIR, result_file)
            test_name = "test_sources_" + project_file.split('_', 1)[1].split('.')[0].lower()
            setattr(TestTympan, test_name, make_sources_test_with_file(project_file_path,
                                                                       source_file_path))
        # Receptors
        if result_file == 'receptor_positions_Project_' + projnum + '.txt':
            receptor_file_path = osp.join(TEST_DATA_DIR, TEST_RESULT_DIR, result_file)
            test_name = "test_receptors_" + project_file.split('_', 1)[1].split('.')[0].lower()
            setattr(TestTympan, test_name, make_receptors_test_with_file(project_file_path,
                                                                         receptor_file_path))

if __name__ == '__main__':
    unittest.main()

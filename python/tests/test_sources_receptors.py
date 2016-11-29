import sys
import os, os.path as osp
import unittest
import re
import numpy as np
from numpy.testing import assert_allclose
from functools import cmp_to_key

from utils import (TEST_DATA_DIR, TEST_SOLVERS_DIR, TEST_RESULT_DIR, TympanTC,
                   compare_floats)
from tympan.models.solver import Model, Solver

_TEST_PROBLEM_DIR = osp.join(TEST_DATA_DIR, 'computed-projects-panel')
assert osp.isdir(_TEST_PROBLEM_DIR), "The test problem dir does not exists '%s'" % _TEST_PROBLEM_DIR


class SolverSourceTC(TympanTC):

    def test_directivity_vector(self):
        proj = self.load_project(osp.join('projects-panel', 'site_with_single_machine.xml'))
        model = Model.from_project(proj)
        vectors = {'{cfdf78a8-bc89-4583-afff-f238b6752f18}': [(-1, 0, 0), None],
                   '{70420c97-5b97-4c5b-b429-ec2aaf801f0d}': [(1, 0, 0), None]}
        for source in model.sources:
            if source.face_id in vectors:
                vectors[source.face_id][1] = (source.directivity_vector.vx,
                                              source.directivity_vector.vy,
                                              source.directivity_vector.vz)
        for key, value in vectors.items():
            if value[1] is None:
                self.fail('No directivity vector found for face %s' % key)
            self.assertEqual(value[0], value[1])


class SourceAdditionTC(TympanTC):

    def test_add_sources_to_model(self):
        project = self.load_project('projects-panel', 'TEST_SOURCE_PONCTUELLE_NO_RESU.xml')
        model = Model.from_project(project)
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
        power_lvl = 10. * np.ones(31)
        ref_proj = self.load_project('site_receptor_source.xml')
        ref_model = Model.from_project(ref_proj)
        assert ref_model.nsources == 1
        assert ref_model.nreceptors == 1
        ref_src = ref_model.source(0)
        assert (ref_src.position.x, ref_src.position.y, ref_src.position.z) == (3., 3., 2.)
        assert_allclose(ref_src.spectrum.to_dB().values, power_lvl,
                        rtol=1e-4)
        solver = Solver.from_project(ref_proj, TEST_SOLVERS_DIR)
        ref_result = solver.solve(ref_model).spectrum(0, 0).values
        # do the same with a manually added source (the xml project is the same as
        # 'site_receptor_source.xml' except the source has been removed)
        proj = self.load_project('site_receptor.xml')
        model = Model.from_project(proj)
        assert model.nsources == 0
        assert model.nreceptors == 1
        model.add_source((3., 3., 2.), power_lvl, 0)
        solver = Solver.from_project(proj, TEST_SOLVERS_DIR)
        result = solver.solve(model).spectrum(0, 0).values
        assert_allclose(ref_result, result)

    def test_value(self):
        proj = self.load_project('site_receptor.xml')
        model = Model.from_project(proj)
        power_lvl = np.array([10., 10., 10., 15., 15., 15., 20., 20., 20., 20., 50.],
                             dtype=float)
        model.add_source((3., 3., 2.),  power_lvl, 0)
        assert model.nsources == 1
        source = model.source(0)
        self.assertAlmostEqual(source.value(16.0), 1e-11)
        self.assertAlmostEqual(source.value(63.0), 1e-11)
        self.assertAlmostEqual(source.value(100.0), 1e-10)
        self.assertEqual(source.value(160.0), 1e-7)


class SourceContainerTC(TympanTC):

     def test_source_container(self):
        """Check the id of the surface and volume containing the source"""
        proj = self.load_project(osp.join('projects-panel', 'site_with_single_machine.xml'))
        model = Model.from_project(proj)
        for source in model.sources:
            self.assertEqual(source.volume_id, "{91ed858c-8434-467a-bf14-bac2447a0ee7}")
            # Face must be one of the 6 of the volume
            self.assertIn(source.face_id, ["{70420c97-5b97-4c5b-b429-ec2aaf801f0d}",
                                           "{14ce2c62-75d2-4173-b99d-b8a2d1a13da0}",
                                           "{cfdf78a8-bc89-4583-afff-f238b6752f18}",
                                           "{805043e8-7698-4c71-9516-f4ae9447c441}",
                                           "{0c4a96f8-3b43-4313-9eae-af2a2237a2f4}",
                                           "{e6f1e3a1-f45a-4d2c-bea0-1b3618343738}"])


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
        model = Model.from_project(project, set_receptors=False)
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
            expected_sources = sorted(expected_sources, key=cmp_to_key(compare_floats))
            real_sources = sorted(real_sources, key=cmp_to_key(compare_floats))
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
        model = Model.from_project(project, set_sources=False)
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
            expected_receptors = sorted(expected_receptors, key=cmp_to_key(compare_floats))
            real_receptors = sorted(real_receptors, key=cmp_to_key(compare_floats))
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

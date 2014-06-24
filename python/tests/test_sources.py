import sys
import os, os.path as osp
import unittest
import re
import numpy as np
from itertools import izip
from numpy.testing import assert_allclose

from utils import TEST_DATA_DIR, TEST_SOLVERS_DIR, TEST_RESULT_DIR, TympanTC, pytam


_TEST_PROBLEM_DIR = osp.join(TEST_DATA_DIR, 'computed-projects-panel')
assert osp.isdir(_TEST_PROBLEM_DIR), "The test problem dir does not exists '%s'" % _TEST_PROBLEM_DIR


def compare_floats(x, y):
    """ Compare x and y which are float arrays:
        Go through them, considering x[i] and y[i] equal up to a 3 decimal
        precision. Then when they sufficiently differ, return -1 if x[i] < y[i]
        and 1 otherwise. 0 is returned if the arrays are equal.
    """
    for xi, yi in izip(x, y):
        if not np.allclose(xi, yi, atol=1e-03):
            if xi < yi:
                return -1
            else:
                return 1
    return 0 # arrays are equal


class TestTympan(TympanTC):
    pass


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
        model = project.current_computation.acoustic_problem
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
        model = project.current_computation.acoustic_problem
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
    from utils import main
    main()

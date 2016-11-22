import sys
import os, os.path as osp
import unittest

import numpy as np
from numpy.testing import assert_allclose

from utils import TEST_DATA_DIR, TEST_PROBLEM_DIR, TEST_SOLVERS_DIR, TympanTC
from tympan.models.solver import Model, Solver

_HERE = osp.realpath(osp.dirname(__file__))

class TestPyTam(TympanTC):

    def _compute_project(self, test_file):
        project = self.load_project(test_file)
        model = Model.from_project(project)
        solver = Solver.from_project(project, TEST_SOLVERS_DIR)
        # avoid segfaults due to multithreading
        solver.nthreads = 1
        solver_result = solver.solve(model)
        project.import_result(model, solver_result)
        return project

    @unittest.expectedFailure
    def test_solve_check_business_result_one_source(self):
        input_proj = osp.join(TEST_PROBLEM_DIR, 'TEST_FACE_NO_RESU.xml')
        project = self._compute_project(input_proj)
        result = project.current_computation.result
        self.assertEqual(result.nsources, 1)
        self.assertEqual(result.nreceptors, 6)
        actual = [result.spectrum(result.receptors[i],result.sources[0]).dBA for i in range(6)]
        expected = [result.receptor(i).dBA for i in range(6)]
        actual.sort()
        expected.sort()
        assert_allclose(actual, expected)

    @unittest.skip('C++ assertion error, see issue #33')
    def test_solve_check_business_result_two_sources(self):
        input_proj = osp.join(TEST_DATA_DIR, 'test_restitution_results.xml')
        project = self._compute_project(input_proj)
        result = project.current_computation.result
        self.assertEqual(result.nsources, 2)
        self.assertEqual(result.nreceptors, 6)
        # because first source has a very low emission spectrum and therefore
        # doesn't contribute to the result
        actual = [result.spectrum(result.receptors[i],result.sources[1]).dBA for i in range(6)]
        expected = [result.receptor(i).dBA for i in range(6)]
        actual.sort()
        expected.sort()
        assert_allclose(actual, expected)

    def test_hierarchy(self):
        project = self.load_project(
            'projects-panel',
            "10_PROJET_SITE_emprise_non_convexe_avec_butte_et_terrains.xml")
        site = project.site
        childs = site.childs()
        for c in childs:
            self.assertRegex(c.name,'Infrastructure|Topographie')

    def test_base(self):
        # XXX This test uses expected bad values provided by the current
        # implementation
        project = self.load_project('solver_export', "base.xml")
        model = Model.from_project(project)
        self.assertEqual(model.npoints, 6) # OK
        self.assertEqual(model.ntriangles, 5) # XXX should be 4
        self.assertEqual(model.nmaterials, 5) # XXX should be 1
        # FIXME the default material is replicated once per triangle.
        # TODO : how to test the altitude of a point ? or access a triangle at
        # some place ?
        # TODO to be completed: cf. ticket #1468184

if __name__ == '__main__':
    unittest.main()

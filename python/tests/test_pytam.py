import sys
import os, os.path as osp
import unittest

import numpy as np
from numpy.testing import assert_allclose

from utils import TEST_DATA_DIR, TympanTC, no_output

with no_output():
    import tympan.business2solver as bus2solv

_HERE = osp.realpath(osp.dirname(__file__))

class TestPyTam(TympanTC):

    def test_hierarchy(self):
        project, _ = self.load_project(
            'projects-panel',
            "10_PROJET_SITE_emprise_non_convexe_avec_butte_et_terrains.xml")
        site = project.site
        childs = site.childs()
        for c in childs:
            self.assertRegexpMatches(c.name(),'Infrastructure|Topographie')

    def test_base(self):
        # XXX This test uses expected bad values provided by the current
        # implementation
        project, model = self.load_project('solver_export', "base.xml")
        self.assertEqual(model.npoints, 6) # OK
        self.assertEqual(model.ntriangles, 5) # XXX should be 4
        self.assertEqual(model.nmaterials, 5) # XXX should be 1
        # FIXME the default material is replicated once per triangle.
        # TODO : how to test the altitude of a point ? or access a triangle at
        # some place ?
        # TODO to be completed: cf. ticket #1468184

    @unittest.skip("Mesh computation implementation has changed. Must be adjusted to the new one")
    def test_mesh(self):
        """
        Check SolverModelBuilder.fill_problem (triangular mesh creation)
        """
        # load a xml project, build an acoustic problem from it and retrieve
        # its triangular mesh to make sure it contains the correct data
        project, _ = self.load_project("tiny_site.xml")
        with self.no_output():
            model = project.current_computation.acoustic_problem
            # exports in nodes_test the nodes coordinates (x,y,z) and in triangles_test
            # the triangle nodes indices (position in the nodes_test array)
            (nodes_test, triangles_test) = model.export_triangular_mesh()
            # Dump actual exported mesh
            np.savetxt(osp.join(_HERE, "test_mesh_nodes_actual.csv"),
                       nodes_test,
                       delimiter=';', fmt='%f')
            np.savetxt(osp.join(_HERE, "test_mesh_triangles_actual.csv"),
                       triangles_test,
                       delimiter=';', fmt='%d')
        nodes_ref = np.loadtxt(osp.join(TEST_DATA_DIR, "expected",
                                        "test_mesh_nodes_ref.csv"),
                               delimiter=';', dtype=np.float)
        # nodes coordinates must be almost equal (milimeter precision)
        assert_allclose(nodes_ref, nodes_test, atol=1e-03)
        triangles_ref = np.loadtxt(osp.join(TEST_DATA_DIR, "expected",
                                            "test_mesh_triangles_ref.csv"),
                                   delimiter=';', dtype=np.uint)
        # Put triangles in a set of set
        tgles_set_ref = set()
        tgles_set_test = set()
        for i in xrange(triangles_ref.shape[0]):
            tgles_set_ref.add(frozenset(
                [triangles_ref[i][0], triangles_ref[i][1], triangles_ref[i][2]]))
            tgles_set_test.add(frozenset(
                [triangles_test[i][0], triangles_test[i][1], triangles_test[i][2]]))
        # the indices must be strictly equal, but order of triangles and of
        # their vertices is not important
        self.assertEqual(set([]), tgles_set_ref.symmetric_difference(tgles_set_test))


    @unittest.skip("Implementation to be fixed")
    def test_ground_materials(self):
        project, _ = self.load_project('solver_export', "ground_materials.xml")
        model = project.current_computation.acoustic_problem
        self.assertEqual(model.nmaterials(), 3)
        # XXX FIXME: the default material is replicated once per triangle
        # TODO to be completed: cf. ticket #1468184

if __name__ == '__main__':
    unittest.main()

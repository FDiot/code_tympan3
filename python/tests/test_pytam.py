import sys
import os, os.path as osp
import unittest

import numpy as np

from utils import TEST_DATA_DIR, TEST_SOLVERS_DIR, TympanTC, bus2solv

_HERE = osp.realpath(osp.dirname(__file__))

class TestPyTam(TympanTC):

    def test_solve(self):
        project = self.load_project('projects-panel',
                               "10_PROJET_SITE_emprise_non_convexe_avec_butte_et_terrains.xml")
        computation = project.current_computation
        with self.no_output():
            bus2solv.loadsolver(TEST_SOLVERS_DIR, computation)
            self.assertTrue(computation.go())

    def test_hierarchy(self):
        project = self.load_project('projects-panel',
                               "10_PROJET_SITE_emprise_non_convexe_avec_butte_et_terrains.xml")
        site = project.site
        childs = site.childs()
        for c in childs:
            self.assertRegexpMatches(c.name(),'Infrastructure|Topographie')

    def test_base(self):
        # XXX This test uses expected bad values provided by the current
        # implementation
        project = self.load_project('solver_export', "base.xml")
        model = project.current_computation.acoustic_problem
        self.assertEqual(model.npoints, 6) # OK
        self.assertEqual(model.ntriangles, 5) # XXX should be 4
        self.assertEqual(model.nmaterials, 5) # XXX should be 1
        # FIXME the default material is replicated once per triangle.
        # TODO : how to test the altitude of a point ? or access a triangle at
        # some place ?
        # TODO to be completed: cf. ticket #1468184

    def test_mesh(self):
        """
        Check SolverModelBuilder.fill_problem (triangular mesh creation)
        """
        # load a xml project, build an acoustic problem from it and retrieve
        # its triangular mesh to make sure it contains the correct data
        project = self.load_project("tiny_site.xml")
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
        self.assertTrue(np.allclose(a=nodes_ref, b=nodes_test, atol=1e-03))
        triangles_ref = np.loadtxt(osp.join(TEST_DATA_DIR, "expected",
                                            "test_mesh_triangles_ref.csv"),
                                   delimiter=';', dtype=np.uint)
        # the indices must be strictly equal
        self.assertTrue(np.array_equal(triangles_ref, triangles_test))


    @unittest.skip("Implementation to be fixed")
    def test_ground_materials(self):
        project = self.load_project('solver_export', "ground_materials.xml")
        model = project.current_computation.acoustic_problem
        self.assertEqual(model.nmaterials(), 3)
        # XXX FIXME: the default material is replicated once per triangle
        # TODO to be completed: cf. ticket #1468184

if __name__ == '__main__':
    from utils import main
    main()

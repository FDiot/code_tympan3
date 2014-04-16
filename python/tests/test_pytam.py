import os, os.path as osp

import unittest
import numpy as np

import pytam

_HERE = osp.realpath(osp.dirname(__file__))

_PROJECT_BASE = osp.abspath(osp.join(_HERE, '..', '..'))

_TEST_DATA_DIR = osp.join(_PROJECT_BASE, 'tests', 'data')
assert osp.isdir(_TEST_DATA_DIR), "The test data dir does not exists '%s'" % _TEST_DATA_DIR

_TEST_SOLVERS_DIR = osp.join(_PROJECT_BASE, 'pluginsd')
if not osp.isdir(_TEST_SOLVERS_DIR):
    solver_dir = osp.abspath(osp.join(_HERE, '..', '..', 'plugins'))
assert osp.isdir(_TEST_SOLVERS_DIR), "The test solver plugins dir does not exists '%s'" % _TEST_SOLVERS_DIR


class TestTympan(unittest.TestCase):
    def test_solve(self):
        project = pytam.Project.from_xml(osp.join(_TEST_DATA_DIR, 'projects-panel',
                    "10_PROJET_SITE_emprise_non_convexe_avec_butte_et_terrains.xml"))
        computation = project.current_computation()
        pytam.loadsolver(_TEST_SOLVERS_DIR, computation)
        self.assertTrue(computation.go())

    def test_hierarchy(self):
        project = pytam.Project.from_xml(osp.join(_TEST_DATA_DIR, 'projects-panel',
                    "10_PROJET_SITE_emprise_non_convexe_avec_butte_et_terrains.xml"))
        site = project.site()
        childs = site.childs()
        for c in childs:
            self.assertRegexpMatches(c.name(),'Infrastructure|Topographie')

    def test_base(self):
        # XXX This test uses expected bad values provided by the current
        # implementation
        project = pytam.Project.from_xml(osp.join(_TEST_DATA_DIR, 'solver_export', "base.xml"))
        model = project.current_computation().problem()
        builder = pytam.SolverModelBuilder(model)
        builder.fill_problem(project.site())
        self.assertEqual(model.nbpoints(), 6) # OK
        self.assertEqual(model.nbtriangles(), 5) # XXX should be 4
        self.assertEqual(model.nbmaterials(), 5) # XXX should be 1
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
        project = pytam.Project.from_xml(osp.join(_TEST_DATA_DIR, "tiny_site.xml"))
        model = project.current_computation().problem()
        builder = pytam.SolverModelBuilder(model)
        builder.fill_problem(project.site())
        # exports in nodes_test the nodes coordinates (x,y,z) and in triangles_test
        # the triangle nodes indices (position in the nodes_test array)
        (nodes_test, triangles_test) = model.export_triangular_mesh()
        nodes_ref = np.loadtxt("data/test_mesh_nodes_ref.csv", delimiter=';',
                               dtype=np.float)
        # nodes coordinates must be almost equal (milimeter precision)
        self.assertTrue(np.allclose(a=nodes_ref, b=nodes_test, atol=1e-03))
        triangles_ref = np.loadtxt("data/test_mesh_triangles_ref.csv", delimiter=';',
                               dtype=np.uint)
        # the indices must be strictly equal
        self.assertTrue(np.array_equal(triangles_ref, triangles_test))


    @unittest.skip("Implementation to be fixed")
    def test_ground_materials(self):
        project = pytam.Project.from_xml(osp.join(_TEST_DATA_DIR, 'solver_export', "ground_materials.xml"))
        model = project.current_computation().problem()
        builder = pytam.SolverModelBuilder(model)
        builder.fill_problem(project.site())
        self.assertEqual(model.nbmaterials(), 3)
        # XXX FIXME: the default material is replicated once per triangle
        # TODO to be completed: cf. ticket #1468184

if __name__ == '__main__':
    unittest.main()

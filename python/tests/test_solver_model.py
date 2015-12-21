import unittest
import os.path as osp

from tympan.models.solver import Model, Solver
from tympan.models._common import Point3D
from utils import TympanTC, TEST_SOLVERS_DIR


class TriangleContainerTC(TympanTC):

    def test_triangle_container(self):
        """Check the id of the volume containing the acoustic triangles from the topography"""
        proj = self.load_project(osp.join('projects-panel', 'site_with_single_machine.xml'))
        model = Model.from_project(proj)
        found_a_topography_triangle = False
        for triangle in model.triangles:
            # triangles can come from the topography or the altimetry. The ones from the altimetry
            # don't possess any volume id.
            if triangle.volume_id:
                self.assertEqual(triangle.volume_id, "{91ed858c-8434-467a-bf14-bac2447a0ee7}")
                found_a_topography_triangle = True
        # make sure we checked at least one topography triangle volume id, otherwise the test
        # doesn't stand for anything.
        self.assertTrue(found_a_topography_triangle)


class SolverModelWithoutProjectTC(TympanTC):

    def test_solver_from_name(self):
        """Check Solver.from_name() / just ensure this does not crash"""
        params_file = self.datapath('solverparams.ini')
        solver = Solver.from_name('default', params_file, TEST_SOLVERS_DIR)


class FresnelZoneIntersectionTC(TympanTC):

    def test_volume_inside_scene(self):
        """Simple test with a planar scene and a cube on top of it"""
        # build a model with two triangles (z=0)
        model = Model()
        model.add_node(0, 0, 0)
        model.add_node(0, 1, 0)
        model.add_node(1, 1, 0)
        model.add_node(1, 0, 0)
        model.add_triangle(0, 1, 2)
        model.add_triangle(1, 2, 3)
        intersected_triangles = model.fresnel_zone_intersection(1, 1, Point3D(-0.5, 0, 0),
                                                                Point3D(0.5, 0, 0))
        self.assertEqual(intersected_triangles, [0, 1])

    def test_volume_on_scene(self):
        """Simple test with a squared planar scene and a cube on top of it"""
        model = Model()
        model.add_node(0, 0, 0)
        model.add_node(0, 1, 0)
        model.add_node(1, 1, 0)
        model.add_node(2, 1, 0)
        model.add_node(1, 2, 0)
        model.add_triangle(0, 1, 2)
        model.add_triangle(2, 3, 4)
        # It is on the first triangle but not on the second one
        intersected_triangles = model.fresnel_zone_intersection(1, 1, Point3D(-0.5, 0, 0),
                                                                Point3D(0.5, 0, 0))
        self.assertEqual(intersected_triangles, [0])

    def test_floating_volume_above_scene(self):
        model = Model()
        model.add_node(0, 0, 0)
        model.add_node(0, 1, 0)
        model.add_node(1, 1, 0)
        model.add_node(1, 0, 0)
        model.add_triangle(0, 1, 2)
        model.add_triangle(1, 2, 3)
        intersected_triangles = model.fresnel_zone_intersection(1, 1, Point3D(-0.5, 0, 2),
                                                                Point3D(0.5, 0, 1))
        self.assertEqual(intersected_triangles, [])

    def test_alti_mesh(self):
        proj = self.load_project(osp.join('projects-panel', 'fresnel.xml'))
        model = Model.from_project(proj)
        length = 75
        width = 75
        height = 75
        intersected_triangles = model.fresnel_zone_intersection(width, height,
                                                                Point3D(-length/2., 0, 0),
                                                                Point3D(length/2., 0, 0))
        all_triangles = model.triangles
        self.assertEqual(len(all_triangles), 182)
        for tri_idx, triangle in enumerate(all_triangles):
            node_inside_box = False
            for node in triangle.nodes:
                x, y, z = model.node_coords(node)
                if abs(x) <= length/2 and abs(y) <= width/2 and abs(z) <= height/2:
                    node_inside_box = True
            # If one of the node of the triangle is inside the box, make sure it is part of
            # the intersected triangles
            if node_inside_box:
                self.assertIn(tri_idx, intersected_triangles)
            # If the triangle isn't part of the intersected triangles, make sure it has no node
            # inside the box.
            if tri_idx not in intersected_triangles:
                self.assertFalse(node_inside_box)


if __name__ == '__main__':
    unittest.main()

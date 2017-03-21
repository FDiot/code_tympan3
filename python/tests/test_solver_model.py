import os.path as osp
import unittest

import numpy as np
from numpy.testing import assert_array_equal, assert_allclose

from tympan.models.solver import Model, Solver
from tympan.models._solver import Directivity
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


def add_source_with_directivity(model, directivity_type):
    directivity = Directivity(directivity_type=directivity_type, support_normal=(1, 1.5, 3),
                              size=5.)
    return model.add_source(position=(0.7, 0.7, 0),
                            spectrum_values=10. * np.ones(31),
                            directivity=directivity)


def source_directivity(model, source_id):
    source = model.source(source_id)
    directivity_vector = source.directivity_vector
    return directivity_vector.vx, directivity_vector.vy, directivity_vector.vz


class SolverModelWithoutProjectTC(TympanTC):

    def test_solver_from_name(self):
        """Check Solver.from_name() / just ensure this does not crash"""
        params_file = self.datapath('solverparams.ini')
        Solver.from_name('default', params_file, TEST_SOLVERS_DIR)

    def test_add_mesh(self):
        model = Model()
        nodes = np.array([[0, 0, 0],
                          [0, 1, 0],
                          [1, 1, 0],
                          [1, 0, 0]])
        triangles = np.array([[0, 1, 2],
                              [1, 2, 3]])
        materials = [('mat1', 0.1, 2.3, 12),
                     ('mat2', 0.2, 3.3, 23)]
        nodes_idx, triangles_idx = model.add_mesh(nodes, triangles, materials)
        assert_array_equal(np.array([model.node_coords(n) for n in range(4)]),
                           nodes)
        assert_array_equal(np.array([t.nodes for t in model.triangles]),
                           triangles)
        self.assertEqual([t.material_name for t in model.triangles],
                         ['mat1', 'mat2'])

    def test_add_receptor(self):
        model = Model()
        coords = 0.7, 0.7, 0
        model.add_receptor(*coords)
        point = model.receptor(0).position
        self.assertEqual((point.x, point.y, point.z), coords)

    def test_add_source_spherical_directivity(self):
        model = Model()
        coords = 0.7, 0.7, 0
        power_lvl = 10. * np.ones(31)
        source_id = model.add_source(coords, power_lvl, directivity=Directivity())
        source = model.source(source_id)
        source_pos = source.position.x, source.position.y, source.position.z
        self.assertEqual(source_pos, coords)
        assert_allclose(source.spectrum.to_dB().values, power_lvl, rtol=1e-4)
        # spherical sources have no directivity
        with self.assertRaises(ValueError) as cm:
            source.directivity_vector
        self.assertEqual(str(cm.exception),
                         'The directivity of this source has no support normal vector')

    def test_create_unknown_directivity(self):
        with self.assertRaises(AssertionError) as cm:
            Directivity(directivity_type='foo', support_normal=(0.7, 0.7, 0), size=5.)
        self.assertEqual(str(cm.exception),
                         'Unknown directivity type. Possible values: spherical, '
                         'surface, baffled, chimney.')

    def test_add_source_with_directivity(self):
        model = Model()
        for dtype in ('surface', 'baffled', 'chimney'):
            directivity = add_source_with_directivity(model, dtype)
            self.assertEqual(source_directivity(model, directivity),
                             (1.0, 1.5, 3.0))


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

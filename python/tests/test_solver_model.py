import unittest
import os.path as osp

from tympan.models.solver import Model
from tympan.models._common import Box
from utils import TympanTC

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
        # length, width, height centered in (0,0,0)
        box = Box(1, 1, 1)
        intersected_triangles = model.fresnel_zone_intersection(box)
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
        box = Box(1, 1, 1)
        intersected_triangles = model.fresnel_zone_intersection(box)
        self.assertEqual(intersected_triangles, [0])


if __name__ == '__main__':
    unittest.main()

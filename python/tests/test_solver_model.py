import unittest
import os.path as osp

from tympan.models.solver import Model
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


if __name__ == '__main__':
    unittest.main()

import unittest

import numpy as np

from altimetry_testutils import rect
from tympan.altimetry import (
    builder,
    datamodel,
)

road_coords = np.array([(5, 5, 1.2), (10, 5, 2), (10, 7, 3),
                        (15, 8, 2.5), (15, 10, 2), (20, 13, 0.5)])


class TestAltimetryRoads(unittest.TestCase):

    def test_add_road(self):
        asite = datamodel.SiteNode(rect(0, 0, 30, 30), id='Main site')
        road = datamodel.Road(
            road_coords[:, :2],
            road_coords[:, 2],
            width=(2, 2),
            id='road',
        )
        asite.add_child(road)
        self.assertEqual(list(asite.roads)[0], road)

    def test_create_bad_road(self):
        with self.assertRaises(ValueError) as cm:
            datamodel.Road(
                road_coords[:, :2],
                road_coords[:, 2][1:],
                width=(2, 4),
                id='road',
            )
        self.assertEqual(
            str(cm.exception),
            'coords and altitudes have different lengths for Road #road',
        )

    def test_add_road_constraint(self):
        asite = datamodel.SiteNode(rect(0, 0, 30, 30), id="Main site")
        road = datamodel.Road(
            road_coords[:, :2],
            road_coords[:, 2],
            width=(4, 2),
            id="road",
            parent_site=asite,
        )
        _, mesh, _ = builder.build_altimetry(asite)
        mesh_set = set(map(tuple, mesh.as_arrays()[0]))
        for line_coords in road.build_coordinates()[0]:
            coords_set = set([(x, y, z) for (x, y), z in
                              zip(line_coords, road.altitudes)])
            self.assertEqual(
                set.intersection(coords_set, mesh_set),
                coords_set,
            )


if __name__ == '__main__':
    unittest.main()

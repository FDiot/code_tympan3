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
            angle=(0, 10),
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
                angle=(0, -10),
                id='road',
            )
        self.assertEqual(
            str(cm.exception),
            'coords and altitudes have different lengths for Road #road',
        )

    def test_create_bad_road_angle(self):
        with self.assertRaises(ValueError) as cm:
            datamodel.Road(
                road_coords[:, :2],
                road_coords[:, 2][1:],
                width=(2, 4),
                angle=0,
                id='road',
            )
        self.assertEqual(
            str(cm.exception),
            'angle of Road #road is not a tuple: [left value, rigth value]',
        )

    def test_add_road_constraint(self):
        asite = datamodel.SiteNode(rect(0, 0, 30, 30), id="Main site")
        road = datamodel.Road(
            road_coords[:, :2],
            road_coords[:, 2],
            width=(4, 2),
            angle=(10, 10),
            id="road",
            parent_site=asite,
        )
        _, mesh, _ = builder.build_altimetry(asite)
        mesh = set(map(tuple, mesh.as_arrays()[0]))
        for line_coords in road.build_coordinates()[0]:
            coords = set([(x, y, z) for x, y, z in line_coords])
            self.assertEqual(
                set.intersection(coords, mesh),
                coords,
            )

    def test_road_intersection(self):
        asite = datamodel.SiteNode(rect(0, 0, 30, 30), id="Main site")
        curve_coord = np.array([(2, 4), (28, 8), (25, 14)])
        datamodel.LevelCurve(
            curve_coord,
            altitude=1,
            id="level_curve",
            parent_site=asite,
        )
        _, mesh, _ = builder.build_altimetry(asite)
        road = datamodel.Road(
            road_coords[:, :2],
            road_coords[:, 2],
            width=(2, 4),
            angle=(0, 10),
            id="road",
            parent_site=asite,
        )
        merged_site, mesh_with_road, _ = builder.build_altimetry(asite)
        mesh = set(map(tuple, mesh_with_road.as_arrays()[0]))
        for line_coords in road.build_coordinates()[0]:
            coords = set([(x, y, z) for x, y, z in line_coords])
            self.assertEqual(
                set.intersection(coords, mesh),
                coords,
            )
        level_curves = [lc.id for lc in merged_site.level_curves]
        self.assertNotIn("level_curve", level_curves)
        self.assertIn('LevelCurve #level_curve split left by Road #road',
                      level_curves)

    def test_road_2_intersections(self):
        """test road that intersect the level curve several times"""
        asite = datamodel.SiteNode(rect(0, 0, 30, 30), id="Main site")
        curve_coord = np.array([(2, 4), (8, 10), (25, 14), (20, 24)])
        datamodel.LevelCurve(
            curve_coord,
            altitude=1,
            id="level_curve",
            parent_site=asite,
        )
        datamodel.Road(
            road_coords[:, :2],
            road_coords[:, 2],
            width=(2, 2),
            angle=(0, 10),
            id="road",
            parent_site=asite,
        )
        with self.assertRaises(NotImplementedError) as cm:
            builder.build_altimetry(asite)
        self.assertEqual(
            str(cm.exception),
            'Too many intersection points (2) between Road #road'
            ' and LevelCurve #level_curve',
        )

    def test_2_roads_intersection(self):
        """test 2 roads that intersect the same level curve"""
        asite = datamodel.SiteNode(rect(0, 0, 30, 30), id="Main site")
        curve_coord = np.array([(15, 0), (15, 15), (15, 20), (15, 30)])
        datamodel.LevelCurve(
            curve_coord,
            altitude=1,
            id="level_curve",
            parent_site=asite,
        )
        road1 = datamodel.Road(
            [(0, 15), (5, 15), (15, 15), (20, 15), (25, 15), (30, 15)],
            road_coords[:, 2],
            width=(2, 2),
            angle=(0, 10),
            id="road1",
            parent_site=asite,
        )
        road2 = datamodel.Road(
            [(0, 20), (5, 20), (10, 20), (15, 20), (25, 20), (30, 20)],
            road_coords[:, 2],
            width=(2, 2),
            angle=(0, 10),
            id="road2",
            parent_site=asite,
        )
        merged_site, mesh_with_road, _ = builder.build_altimetry(asite)
        mesh = set(map(tuple, mesh_with_road.as_arrays()[0]))
        for line_coords in road1.build_coordinates()[0]:
            coords = set([(x, y, z) for x, y, z in line_coords])
            self.assertEqual(
                set.intersection(coords, mesh),
                coords,
            )
        for line_coords in road2.build_coordinates()[0]:
            coords = set([(x, y, z) for x, y, z in line_coords])
            self.assertEqual(
                set.intersection(coords, mesh),
                coords,
            )
        level_curves = [lc.id for lc in merged_site.level_curves]
        self.assertNotIn("level_curve", level_curves)
        self.assertEqual(3, len(level_curves))


if __name__ == '__main__':
    unittest.main()

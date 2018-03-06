import unittest

import numpy as np

from altimetry_testutils import rect
from tympan.altimetry import (
    builder,
    datamodel,
)
from utils import TympanTC

road1_description = [
    (5, 5, 1.2, 2, 2, 0, 10, 1, 1),
    (10, 5, 2, 2, 2, 0, 10, 1, 1),
    (10, 7, 3, 2, 2, 0, 10, 1, 1),
    (15, 8, 2.5, 2, 2, 0, 10, 2, 2),
    (15, 10, 2, 2, 2, 0, 10, 2, 2),
    (20, 13, 0.5, 2, 2, 0, 10, 2, 2),
]

road2_description = [
    (0, 15, 1, 2, 2, 0, 10, 0.2, 0.5),
    (5, 15, 2, 2, 2, 0, 0, 0.2, 0.5),
    (15, 15, 2.5, 2, 2, 0, 20, 0.2, 0.5),
    (20, 15, 2, 2, 2, 0, 10, 0.2, 0.5),
    (25, 15, 0.5, 2, 2, 0, 10, 0.2, 0.5),
]


class TestAltimetryRoads(TympanTC):

    def test_add_road(self):
        asite = datamodel.SiteNode(rect(0, 0, 30, 30), id='Main site')
        road_profiles = [datamodel.RoadProfile(*profile) for profile in
                         road1_description]
        road = datamodel.Road(road_profiles, id='road')
        asite.add_child(road)
        self.assertEqual(list(asite.roads)[0], road)

    def test_load_csv_road(self):
        asite = datamodel.SiteNode(rect(0, 0, 30, 30), id='Main site')
        road_file = self.datapath('road.csv')
        road_profiles = datamodel.load_csv_road(road_file)
        self.assertEqual(len(road_profiles), 6)
        road = datamodel.Road(road_profiles, id='road')
        asite.add_child(road)
        self.assertEqual(list(asite.roads)[0], road)

    def test_add_road_constraint(self):
        asite = datamodel.SiteNode(rect(0, 0, 30, 30), id="Main site")
        road_profiles = [datamodel.RoadProfile(*profile) for profile in
                         road1_description]
        road = datamodel.Road(road_profiles, id='road', parent_site=asite)
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
        road_profiles = [datamodel.RoadProfile(*profile) for profile in
                         road1_description]
        road = datamodel.Road(road_profiles, id='road', parent_site=asite)
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

    def test_road_with_variable_angles(self):
        asite = datamodel.SiteNode(rect(0, 0, 30, 30), id="Main site")
        curve_coord = np.array([(2, 4), (28, 8), (25, 14)])
        datamodel.LevelCurve(
            curve_coord,
            altitude=1,
            id="level_curve",
            parent_site=asite,
        )
        _, mesh, _ = builder.build_altimetry(asite)
        road_profiles = [datamodel.RoadProfile(*profile) for profile in
                         road1_description]
        road1 = datamodel.Road(road_profiles, id='road', parent_site=asite)
        road_profiles[1].angle = (5, 15)
        road2 = datamodel.Road(road_profiles, id='road', parent_site=asite)
        np.testing.assert_array_equal(
            np.array(road1.build_coordinates()[0][0][:])[:, :2],
            np.array(road2.build_coordinates()[0][0][:])[:, :2],
        )
        self.assertNotEqual(
            road1.build_coordinates()[0][0][:][1][2],
            road2.build_coordinates()[0][0][:][1][2],
        )

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
        road_profiles = [datamodel.RoadProfile(*profile) for profile in
                         road1_description]
        datamodel.Road(road_profiles, id='road', parent_site=asite)
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
        road1_profiles = [datamodel.RoadProfile(*profile) for profile in
                          road2_description]
        road1 = datamodel.Road(road1_profiles, id='road1', parent_site=asite)
        # make a road2 parallel to road1
        road2_profiles = []
        for profil in road1_profiles:
            profil.coords = (profil.coords[0], profil.coords[1] + 5)
            road2_profiles.append(profil)
        road2 = datamodel.Road(road2_profiles, id='road2', parent_site=asite)
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

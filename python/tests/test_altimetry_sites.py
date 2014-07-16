import sys
import os, os.path as osp
import unittest

_runVisualTests = os.environ.get('RUN_VISUAL_TESTS', False)

if _runVisualTests:
    from matplotlib import pyplot as plt


from shapely import geometry
from shapely.geometry import MultiLineString, LineString

# NB Importing altimetry configures path to find CGAL bindings
from altimetry.datamodel import (LevelCurve, MaterialArea, GroundMaterial,
                                 WaterBody, SiteNode, PolygonalTympanFeature,
                                 InconsistentGeometricModel, MATERIAL_WATER,
                                 elementary_shapes)
from altimetry.merge import (SiteNodeGeometryCleaner, build_site_shape_with_hole,
                             recursively_merge_all_subsites)
from altimetry import visu
from altimetry import mesh

def rect(x1, y1, x2, y2):
    return [(x1, y1), (x2, y1), (x2, y2), (x1, y2)]

class AltimetryDataTC(unittest.TestCase):

    big_rect_coords = rect(0, 0, 12, 10)

    level_curve_A_coords = [(0, 0), (2.5, 3.5), (4, 4.0)]
    level_curve_A =  LevelCurve(level_curve_A_coords, altitude=10.0, id="{123456}")
    level_curve_A_json = (
        '{"geometry": {"coordinates": [[[0.0, 0.0], [2.5, 3.5], [4.0, 4.0]]],'
        ' "type": "MultiLineString"}, "id": "{123456}",'
        ' "properties": {"altitude": 10.0, "type": "LevelCurve"},'
        ' "type": "Feature"}')
    material_area_A_coords = big_rect_coords
    grass = GroundMaterial("grass")
    material_area_A = MaterialArea(material_area_A_coords, material=grass, id=None)
    material_area_A_json = (
        '{"geometry": {'
        '"coordinates": [[[0.0, 0.0], [12.0, 0.0], [12.0, 10.0], [0.0, 10.0]]],'
        ' "type": "Polygon"},'
        ' "properties": {"material": "grass", "type": "MaterialArea"},'
        ' "type": "Feature"}')
    waterbody_coords = [(3, 3), (5, 4), (3, 5)]

    subsite_A_coords = [(8.0, 4.0), (8.0, 7.0), (12.0, 7.0)]
    subsite_A = SiteNode(subsite_A_coords, id='{Sub-site A ID}')
    level_curve_B_coords =[(8.0, 4.0), (8.0, 7.0), (12.0, 7.0)]


    def test_LevelCurve_as_json(self):
        self.assertEqual(self.level_curve_A.as_geojson, self.level_curve_A_json)

    def test_MaterialArea_as_json(self):
        self.assertEqual(self.material_area_A.as_geojson, self.material_area_A_json)

    def test_LevelCurve_as_shape(self):
        shape = self.level_curve_A.as_shape
        self.assertTrue(shape.equals(geometry.LineString(self.level_curve_A_coords)))

    def test_MaterialArea_as_shape(self):
        shape = geometry.shape(self.material_area_A)
        self.assertTrue(shape.equals(geometry.Polygon(self.material_area_A_coords)))

    def test_SiteNode_as_shape(self):
        shape = geometry.shape(self.subsite_A)
        self.assertTrue(shape.equals(geometry.Polygon(self.subsite_A_coords)))

    def test_site_belonging(self):
        self.assertIsNone(self.subsite_A.parent_site)
        level_curve_B =  LevelCurve(self.level_curve_B_coords, altitude=20.0,
                                    parent_site=self.subsite_A, id=None)
        self.assertIn(level_curve_B, self.subsite_A.children["LevelCurve"])
        self.assertEqual(level_curve_B.parent_site_id, "{Sub-site A ID}")

    def test_cached_shape_property(self):
        level_curve_B =  LevelCurve(self.level_curve_B_coords, altitude=20.0, id=None)
        self.assertIsNone(level_curve_B._shape)

        shape = level_curve_B.shape

        self.assertTrue(shape.equals(level_curve_B.as_shape))
        self.assertIs(shape, level_curve_B._shape)

    def test_polygon_validity(self):
        poly = PolygonalTympanFeature(self.big_rect_coords, id=None)
        try:
            poly.ensure_ok()
        except InconsistentGeometricModel:
            self.fail("poly was expected to be valid")

        coords = self.big_rect_coords[:]
        coords[1:3] = coords[2:0:-1]
        poly2 = PolygonalTympanFeature(coords, id="toto")

        with self.assertRaises(InconsistentGeometricModel) as cm:
            poly2.ensure_ok()
        self.assertEqual(cm.exception.ids, ["toto"])
        self.assertIn("Self-intersection", str(cm.exception))

    def test_polygon_exterior_orientation(self):
        coords = self.big_rect_coords[:]
        coords.reverse() # change orientation to CW
        poly = PolygonalTympanFeature(coords, id=None)

        self.assertTrue(poly.shape.exterior.is_ccw)

    def test_WaterBody_initialisation(self):
        waterbody=  WaterBody(self.waterbody_coords, altitude=20, id=None)

        self.assertEqual(waterbody.altitude, 20.0)
        self.assertEqual(waterbody.material, MATERIAL_WATER)

    def test_sitenode_attribution_and_accessors(self):
        mainsite = SiteNode(self.big_rect_coords, id="{Main site ID}")
        subsite = SiteNode(self.subsite_A_coords, id="{Subsite ID}",
                           parent_site=mainsite)
        level_curve_B =  LevelCurve(self.level_curve_B_coords, altitude=20.0,
                                    parent_site=subsite, id=None)
        level_curve_A =  LevelCurve(self.level_curve_A_coords, altitude=10.0,
                                    parent_site=mainsite, id=None)
        waterbody=  WaterBody(self.waterbody_coords, altitude=5,
                              parent_site=mainsite, id=None)
        material_area_A = MaterialArea(self.material_area_A_coords, material=self.grass,
                                       parent_site=mainsite, id=None)

        self.assertItemsEqual(mainsite.level_curves, [level_curve_A, waterbody])
        self.assertItemsEqual(mainsite.material_areas, [material_area_A, waterbody])
        self.assertItemsEqual(mainsite.subsites, [subsite])
        self.assertItemsEqual(subsite.level_curves, [level_curve_B])

    def test_elementary_shapes(self):
        mainsite = SiteNode(self.big_rect_coords, id="{Main site ID}")
        mls = MultiLineString([[(0.0, 0.0), (1.0, 0.0)], [(1.0, 1.0), (1.0, 2.0)]])
        ls1 = LineString([(0.0, 0.0), (1.0, 0.0)])
        ls2 = LineString([(1.0, 1.0), (1.0, 2.0)])
        level_curve = LevelCurve(mls, altitude=10.0,
                                 parent_site=mainsite, id=None)

        shapes = elementary_shapes(level_curve.shape)
        self.assertEqual(len(shapes), 2)
        for shape in shapes:
            self.assertTrue(ls1.equals(shape) or ls2.equals(shape))


class _TestFeatures(object):

    big_rect_coords = rect(0, 0, 12, 10)
    grass = GroundMaterial("grass")

    level_curve_A_coords = [(-1, -1), (2, 2), (4, 2)]
    cleaned_level_A_shape = MultiLineString([[(0, 0), (2, 2), (4, 2)]])

    altitude_A = 10.0
    grass_area_coords = [(1.0, 1.0), (11.0, 1.0), (1.0, 9.0), (1.0, 1.0)]
    waterbody_coords = [(3, 3), (5, 4), (3, 5)]
    altitude_water = 5.0
    subsite_A_coords = rect(6, 6, 11, 8)
    level_curve_B_coords =[(8.0, 4.0), (8.0, 7.0), (12.0, 7.0)]
    cleaned_level_B_shape = MultiLineString([[(8.0, 6.0), (8.0, 7.0), (11.0, 7.0)]])
    altitude_B = 20.0

    def build_features(self):
        self.mainsite = SiteNode(self.big_rect_coords, id="{Main site ID}")
        self.level_curve_A = LevelCurve(self.level_curve_A_coords,
                                        altitude=self.altitude_A,
                                        parent_site=self.mainsite, id="{Level curve A}")
        self.grass_area = MaterialArea(self.grass_area_coords,
                                       material=self.grass,
                                       parent_site=self.mainsite, id="{Grass area}")
        self.waterbody= WaterBody(self.waterbody_coords,
                                   altitude=self.altitude_water,
                                   parent_site=self.mainsite, id="{Water body}")
        self.subsite = SiteNode(self.subsite_A_coords, id="{Subsite ID}",
                                parent_site=self.mainsite)
        self.level_curve_B = LevelCurve(self.level_curve_B_coords,
                                        altitude=self.altitude_B,
                                        parent_site=self.subsite, id="{Level curve B}")

    def build_more_features_in_subsites(self):
        self.subsubsite = SiteNode(rect(6, 6.5, 7, 7.5), id="{SubSubsite ID}",
                                   parent_site=self.subsite)
        self.sub_level_curve = LevelCurve([(6.5, 5), (6.5, 9)], altitude=6.0,
                                          id="{Cut level curve}",
                                          parent_site=self.subsite)
        self.subsub_level_curve = LevelCurve([(5, 5.5), (8, 8.5)], altitude=7.0,
                                             id="{Subsub level curve}",
                                             parent_site=self.subsubsite)


class AltimetryMergerTC(unittest.TestCase, _TestFeatures):

    def setUp(self):
        _TestFeatures.build_features(self)

    def test_holes_in_site_shape(self):
        shape = build_site_shape_with_hole(self.mainsite)

        self.assertEqual(len(shape.interiors), 1)
        self.assertFalse(shape.contains(geometry.Point(9, 7)))
        self.assertTrue(shape.contains(geometry.Point(1, 1)))

    def test_add_and_clean_level_curves(self):
        cleaner = SiteNodeGeometryCleaner(self.mainsite)

        cleaner.process_level_curves()

        self.assertTrue(cleaner.geom['{Level curve A}'].equals(self.cleaned_level_A_shape))
        # Not directly in sitenode
        with self.assertRaises(KeyError):
            cleaner['{Level curve B}']
        # Already clean
        self.assertTrue(cleaner.geom['{Water body}'].equals(self.waterbody.shape))

    def test_add_and_clean_material_area(self):
        overlap_area = MaterialArea(rect(5, 7, 7, 9),
                                    material=self.grass,
                                    parent_site=self.mainsite, id="{Overlap area}")
        in_hole_scope_area = MaterialArea(rect(6, 6, 7, 7),
                                    material=self.grass,
                                    parent_site=self.mainsite, id="{In hole area}")
        cleaner = SiteNodeGeometryCleaner(self.mainsite)

        cleaner.process_material_areas()

        self.assertIn("{Overlap area}", cleaner.erroneous_overlap)
        self.assertIn("{In hole area}", cleaner.ignored_features)
        self.assertTrue(cleaner.geom["{Grass area}"].equals(self.grass_area.shape))

    def test_water_body_info(self):
        cleaner = SiteNodeGeometryCleaner(self.mainsite)

        cleaner.process_level_curves()
        cleaner.process_material_areas()

        water_shape, water_info = cleaner['{Water body}']
        self.assertEqual(water_info["altitude"], self.altitude_water)

    def test_export_subsite_feature(self):
        cleaner_mainsite = SiteNodeGeometryCleaner(self.mainsite)
        cleaner_subsite = SiteNodeGeometryCleaner(self.subsite)
        cleaner_subsite.process_level_curves()

        cleaner_subsite.export_cleaned_geometries_into(cleaner_mainsite)

        self.assertTrue(cleaner_mainsite.geom['{Level curve B}'].equals(
            self.cleaned_level_B_shape))

    def test_merge_subsite_OK(self):
        cleaner = SiteNodeGeometryCleaner(self.mainsite)
        cleaner.process_all_features()

        cleaner.merge_subsite(self.subsite)

        self.assertTrue(cleaner.geom['{Level curve A}'].equals(
            self.cleaned_level_A_shape))
        self.assertTrue(cleaner.geom['{Level curve B}'].equals(
            self.cleaned_level_B_shape))


    def test_merge_subsite_report_errors(self):
        overlap_area = MaterialArea(rect(5, 5, 7, 7),
                                    material=self.grass,
                                    parent_site=self.subsite, id="{Overlap area}")

        cleaner = SiteNodeGeometryCleaner(self.mainsite)

        with self.assertRaises(InconsistentGeometricModel):
            cleaner.merge_subsite(self.subsite)

    def assert_pos_and_insert(self, cleaner, area, expected_pos):
        self.assertTrue(cleaner._add_or_reject_polygonal_feature(area))
        pos = cleaner.insert_position_for_sorted_material_area(area)
        self.assertEqual(pos, expected_pos)
        cleaner._sorted_material_areas.insert(pos, area.id)

    def test_sorting_material_area_in_order(self):
        cleaner = SiteNodeGeometryCleaner(self.mainsite)

        self.assert_pos_and_insert(cleaner, self.waterbody, 0)
        self.assertEqual(cleaner.material_areas_inner_first(),
                         ['{Water body}'])

        self.assert_pos_and_insert(cleaner, self.grass_area, 1)
        self.assertEqual(cleaner.material_areas_inner_first(),
                         ['{Water body}','{Grass area}'])

    def test_sorting_material_area_reverse_order(self):
        cleaner = SiteNodeGeometryCleaner(self.mainsite)

        self.assert_pos_and_insert(cleaner, self.grass_area, 0)
        self.assertEqual(cleaner.material_areas_inner_first(),
                         ['{Grass area}'])

        self.assert_pos_and_insert(cleaner, self.waterbody, 0)
        self.assertEqual(cleaner.material_areas_inner_first(),
                         ['{Water body}', '{Grass area}'])

    def test_sorting_material_area_two_branches(self):
        site = SiteNode(rect(0, 0, 12, 12), id='{Main site}')
        cleaner = SiteNodeGeometryCleaner(self.mainsite)

        cleaner.process_material_areas()
        self.assertEqual(cleaner.check_issues_with_material_area_order(), [])

    def test_merge_subsite_material(self):
        self.subgrass = MaterialArea(rect(6, 7, 7, 8), material=self.grass,
                                     id="{Grass in subsite}",
                                     parent_site=self.subsite)
        cleaner = SiteNodeGeometryCleaner(self.mainsite)
        cleaner.process_all_features()
        cleaner.merge_subsite(self.subsite)

        self.assertIn(self.subgrass.id, cleaner.geom)
        geom, info = cleaner[self.subgrass.id]
        self.assertEqual(info['material'], self.grass.id)

    def test_recursive_merge(self):
        self.build_more_features_in_subsites()
        cleaner = recursively_merge_all_subsites(self.mainsite)

        self.assertIn("{Subsub level curve}", cleaner.geom)
        geom, info = cleaner["{Subsub level curve}"]
        expected_shape = geometry.LineString([(6, 6.5), (7, 7.5)])
        self.assertTrue(expected_shape.equals(geom))
        self.assertEqual(info['site'], self.subsubsite.id)

    def test_split_level_curve(self):
        self.build_more_features_in_subsites()
        cleaner = recursively_merge_all_subsites(self.mainsite)

        self.assertIn("{Cut level curve}", cleaner.geom)
        geom, info = cleaner["{Cut level curve}"]
        expected_shape = geometry.MultiLineString([[(6.5, 6), (6.5, 6.5)],
                                                  [(6.5, 7.5), (6.5, 8)]])
        self.assertTrue(expected_shape.equals(geom))

    def test_equivalent_site(self):
        self.build_more_features_in_subsites()
        cleaner = recursively_merge_all_subsites(self.mainsite)
        equiv = cleaner.equivalent_site

        self.assertItemsEqual(equiv.subsites, [])
        self.assertNotIn("{Out of subsite area}", equiv.features_by_id)
        self.assertIn("{Subsub level curve}", equiv.features_by_id)

@unittest.skipUnless(_runVisualTests, "Set RUN_VISUAL_TESTS env. variable to run me")
class VisualisationTC(unittest.TestCase, _TestFeatures):
    global_lims = [-1, 13, -1, 11]

    def setUp(self):
        _TestFeatures.build_features(self)
        self.fig = plt.figure()
        self.ax = self.fig.add_subplot(1, 1, 1)
        self.ax.set_title(self._testMethodName)

    def tearDown(self):
        self.show()
        plt.close(self.fig)

    def show(self):
        plt.axis(self.global_lims, normed=True)
        plt.show(self.fig)

    def test_plot_site_node_recursive(self):
        self.build_more_features_in_subsites()
        self.mainsite.plot(self.ax, recursive=True)

    def test_plot_recursive_merge(self):
        self.build_more_features_in_subsites()
        cleaner = recursively_merge_all_subsites(self.mainsite)
        self.mainsite.plot(self.ax, recursive=True, alt_geom_map=cleaner.geom)

    def test_plot_equivalent_site(self):
        self.build_more_features_in_subsites()
        cleaner = recursively_merge_all_subsites(self.mainsite)
        cleaner.equivalent_site.plot(self.ax, alt_geom_map=cleaner.geom)


if __name__ == '__main__':
    from unittest import main
    main()

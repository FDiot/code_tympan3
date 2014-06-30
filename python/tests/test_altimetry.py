import sys
import os, os.path as osp
from itertools import imap

from shapely import geometry
from shapely.geometry import MultiLineString

# NB Importing altimetry configures path to find CGAL bindings
from altimetry.datamodel import (LevelCurve, MaterialArea, GroundMaterial,
                                 WaterBody, SiteNode, PolygonalTympanFeature,
                                 InvalidGeometry, MATERIAL_WATER)
from altimetry.merge import SiteNodeGeometryCleaner,build_site_shape_with_hole
from altimetry import visu
from altimetry import mesh


class AltimetryDataTC(unittest.TestCase):

    big_rect_coords = [(0, 0), (12,0), (12, 10), (0, 10)]

    level_curve_A_coords = [(0, 0), (2.5, 3.5), (4, 4.0)]
    level_curve_A =  LevelCurve(level_curve_A_coords, altitude=10.0, id="{123456}")
    level_curve_A_json = (
        '{"geometry": {"coordinates": [[0.0, 0.0], [2.5, 3.5], [4.0, 4.0]],'
        ' "type": "LineString"}, "id": "{123456}",'
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
        except InvalidGeometry:
            self.fail("poly was expected to be valid")

        coords = self.big_rect_coords[:]
        coords[1:3] = coords[2:0:-1]
        poly2 = PolygonalTympanFeature(coords, id=None)

        with self.assertRaises(InvalidGeometry) as cm:
            poly2.ensure_ok()
        self.assertIn("Self-intersection", cm.exception.args[0])


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

class TestAltimetry(TympanTC):
    def test_environment(self):
        # Import shapely (must be in the PYTHONPATH)
        try:
            import shapely
        except ImportError:
            self.fail('Shapely must be installed and must be in your PYTHONPATH')
        # import CGAL bindings (CGAL_BINDINGS_PATH environment variable must be defined)
        CGAL_BINDINGS_PATH = os.environ.get('CGAL_BINDINGS_PATH')
        self.assertTrue(CGAL_BINDINGS_PATH is not None)
        self.assertTrue(osp.isdir(CGAL_BINDINGS_PATH))
        sys.path.append(CGAL_BINDINGS_PATH)
        try:
            import CGAL
            from CGAL import CGAL_Kernel, CGAL_Mesh_2
        except ImportError:
            self.fail('''CGAL_BINDINGS_PATH environment variable must contain
                      the path to CGAL_bindings python libraries''')

class _TestFeatures(object):

    big_rect_coords = [(0, 0), (12,0), (12, 10), (0, 10)]
    grass = GroundMaterial("grass")

    level_curve_A_coords = [(-1, -1), (2, 2), (4, 2)]
    cleaned_level_A_shape = MultiLineString([[(0, 0), (2, 2), (4, 2)]])

    altitude_A = 10.0
    grass_area_coords = [(1.0, 1.0), (11.0, 1.0), (1.0, 9.0), (1.0, 1.0)]
    waterbody_coords = [(3, 3), (5, 4), (3, 5)]
    altitude_water = 5.0
    subsite_A_coords = [(6, 8), (11, 8), (11, 6), (6, 6)]
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
                                   parent_site=self.mainsite, id="{Water body ID}")
        self.subsite = SiteNode(self.subsite_A_coords, id="{Subsite ID}",
                                parent_site=self.mainsite)
        self.level_curve_B = LevelCurve(self.level_curve_B_coords,
                                        altitude=self.altitude_B,
                                        parent_site=self.subsite, id="{Level curve B}")


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
        self.assertTrue(cleaner.geom['{Water body ID}'].equals(self.waterbody.shape))

    def test_add_and_clean_material_area(self):
        overlap_area = MaterialArea([(5, 5), (5, 7), (7, 7), (7, 5)],
                                    material=self.grass,
                                    parent_site=self.mainsite, id="{Overlap area}")
        in_hole_scope_area = MaterialArea([(6, 6), (6, 7), (7, 7), (7, 6)],
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

        water_shape, water_info = cleaner['{Water body ID}']
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
        overlap_area = MaterialArea([(5, 5), (5, 7), (7, 7), (7, 5)],
                                    material=self.grass,
                                    parent_site=self.subsite, id="{Overlap area}")

        cleaner = SiteNodeGeometryCleaner(self.mainsite)

        with self.assertRaises(InvalidGeometry):
            cleaner.merge_subsite(self.subsite)


_runVisualTests = os.environ.get('RUN_VISUAL_TESTS', False)

if _runVisualTests:
    from matplotlib import pyplot as plt

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

    def test_LevelCurve_and_grass(self):
        self.level_curve_A.plot(self.ax)
        self.grass_area.plot(self.ax, facecolor='green')

    def test_plot_site_node(self):
        self.subsite.plot(self.ax)

    def test_plot_site_node_recursive(self):
        self.mainsite.plot(self.ax, recursive=True)

    def test_plot_site_node_recursive_alt_geom(self):
        cleaner = SiteNodeGeometryCleaner(self.mainsite)
        cleaner.process_all_features()
        cleaner.merge_subsite(self.subsite)

        self.mainsite.plot(self.ax, recursive=True, alt_geom_map=cleaner.geom)


class MeshedCDTTC(unittest.TestCase):

    def setUp(self):
        self.mesher = mesh.MeshedCDTWithInfo()

    def test_insert_point(self):
        points = [mesh.Point(1, 1)]
        self.mesher.insert_polyline(imap(mesh.to_cgal_point, points))
        self.assertEqual(self.mesher.cdt.number_of_vertices(), 1)
        self.assertEqual(self.mesher.cdt.number_of_faces(), 0)

    def test_insert_point_types(self):
        for point in [geometry.Point(1, 1), (3,4)]:
            with self.assertRaises(TypeError):
                self.mesher.insert_polyline([point])
            self.mesher.insert_polyline([mesh.to_cgal_point(point)])
        point = mesh.Point(1, 2) # CGAL Point
        self.assertEqual(mesh.to_cgal_point(point), point)

    def test_insert_3_point(self):
        points = [(1, 1), (1, 2), (3, 4)]
        self.mesher.insert_polyline(imap(mesh.to_cgal_point, points),
                               connected=False)
        self.assertEqual(self.mesher.cdt.number_of_vertices(), 3)
        self.assertEqual(self.mesher.cdt.number_of_faces(), 1)
        count = 0
        for c in self.mesher.cdt.finite_edges():
            self.assertFalse(self.mesher.cdt.is_constrained(c))
            count += 1
        self.assertEqual(count, 3)

    def test_insert_triangle(self):
        points = [(1, 1), (1, 2), (3, 4)]
        self.mesher.insert_polyline(imap(mesh.to_cgal_point, points),
                               close_it = True)
        self.assertEqual(self.mesher.cdt.number_of_vertices(), 3)
        self.assertEqual(self.mesher.cdt.number_of_faces(), 1)
        count = 0
        for c in self.mesher.cdt.finite_edges():
            self.assertTrue(self.mesher.cdt.is_constrained(c))
            count += 1
        self.assertEqual(count, 3)

    def test_insert_vee(self):
        points = [(-1, 2), (0, 0), (1, 2)]
        self.mesher.insert_polyline(imap(mesh.to_cgal_point, points),
                               close_it=False) # the default by the way
        self.assertEqual(self.mesher.cdt.number_of_vertices(), 3)
        self.assertEqual(self.mesher.cdt.number_of_faces(), 1)
        count_constrained = 0
        for c in self.mesher.cdt.finite_edges():
            if self.mesher.cdt.is_constrained(c):
                count_constrained += 1
        self.assertEqual(count_constrained, 2)

    def test_info_simple_polyline(self):
        points = map(mesh.to_cgal_point, [(1, 1), (1, 2), (3,4)])
        vertices, input_constraints = self.mesher.insert_polyline_with_info(
            points, altitude=10)

        for constraint in input_constraints:
            self.assertEqual(self.mesher.constraints_infos[constraint],
                             {"altitude": 10})
        for vertex in vertices:
            self.assertEqual(self.mesher.vertices_infos[vertex],
                             {"altitude": 10})


if __name__ == '__main__':
    from utils import main
    main()

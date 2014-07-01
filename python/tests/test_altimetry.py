import sys
import os, os.path as osp

from shapely import geometry
from shapely.geometry import Point, MultiLineString
from altimetry.datamodel import * # NB also allows to find CGAL bindings


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

if __name__ == '__main__':
    from utils import main
    main()

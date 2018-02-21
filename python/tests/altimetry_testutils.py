import os

runVisualTests = os.environ.get('RUN_VISUAL_TESTS', False)

import numpy as np
from shapely.geometry import MultiLineString
from tympan.altimetry.datamodel import (LevelCurve, MaterialArea,
                                        VegetationArea, GroundMaterial,
                                        WaterBody, SiteNode, Road,
                                        InfrastructureLandtake,
                                        HIDDEN_MATERIAL)
from tympan.altimetry import mesh

def rect(x1, y1, x2, y2):
    return [(x1, y1), (x2, y1), (x2, y2), (x1, y2)]

class TestFeatures(object):

    big_rect_coords = rect(0, 0, 12, 10)
    grass = GroundMaterial("grass")
    pine = GroundMaterial("pine")

    level_curve_A_coords = [(-1, -1), (2, 2), (4, 2)]
    cleaned_level_A_shape = MultiLineString([[(0, 0), (2, 2), (4, 2)]])

    altitude_A = 10.0
    grass_area_coords = [(1.0, 1.0), (11.0, 1.0), (1.0, 9.0), (1.0, 1.0)]
    pine_area_coords = [(0, 9.5), (12, 9.5), (12, 10), (0, 10)]
    waterbody_coords = [(3, 3), (5, 4), (3, 5)]
    altitude_water = 5.0
    subsite_A_coords = rect(6, 6, 11, 8)
    level_curve_B_coords =[(8.0, 4.0), (8.0, 7.0), (12.0, 7.0)]
    cleaned_level_B_shape = MultiLineString([[(8.0, 6.0), (8.0, 7.0), (11.0, 7.0)]])
    altitude_B = 20.0
    landtake_coords = rect(2, 7, 4, 8)
    road_coords = np.array([(1.2, 1.2, 1.0), (2.0, 1.5, 1.5),
                            (2.0, 2.0, 1.5), (5.0, 4.0, 1.0)])

    def build_features(self):
        self.mainsite = SiteNode(self.big_rect_coords, id="{Main site ID}")
        self.level_curve_A = LevelCurve(self.level_curve_A_coords,
                                        altitude=self.altitude_A,
                                        parent_site=self.mainsite, id="{Level curve A}")
        self.grass_area = MaterialArea(self.grass_area_coords,
                                       material=self.grass,
                                       parent_site=self.mainsite, id="{Grass area}")
        self.veg_area = VegetationArea(self.pine_area_coords,
                                       material=self.pine, height=5,
                                       parent_site=self.mainsite, id="{Pine area}")
        self.waterbody = WaterBody(self.waterbody_coords,
                                   altitude=self.altitude_water,
                                   parent_site=self.mainsite, id="{Water body}")
        self.subsite = SiteNode(self.subsite_A_coords, id="{Subsite ID}",
                                parent_site=self.mainsite)
        self.level_curve_B = LevelCurve(self.level_curve_B_coords,
                                        altitude=self.altitude_B,
                                        parent_site=self.subsite, id="{Level curve B}")
        self.out_of_subsite = MaterialArea(rect(9, 9, 10, 10),
                                       material=self.grass,
                                       parent_site=self.subsite, id="{Out of subsite area}")
        self.building = InfrastructureLandtake(self.landtake_coords,
                                               parent_site=self.mainsite, id="{Building}")
        self.road = Road(self.road_coords[:, :2],
                         self.road_coords[:, 2],
                         width=(2, 2),
                         angle=(0, 0),
                         parent_site=self.mainsite,
                         id="{Road}")

    def build_more_features_in_subsites(self):
        self.subsubsite = SiteNode(rect(6, 6.5, 7, 7.5), id="{SubSubsite ID}",
                                   parent_site=self.subsite)
        self.sub_level_curve = LevelCurve([(6.5, 5), (6.5, 9)], altitude=6.0,
                                          id="{Cut level curve}",
                                          parent_site=self.subsite)
        self.subsub_level_curve = LevelCurve([(5, 5.5), (8, 8.5)], altitude=7.0,
                                             id="{Subsub level curve}",
                                             parent_site=self.subsubsite)


class MesherTestUtilsMixin(object):

    def assert_basic_counts(self, vertices=None, faces=None,
                            edges=None, constrained=None, mesher=None):
        mesher = mesher or self.mesher
        if vertices is not None:
            self.assertEqual(mesher.cdt.number_of_vertices(), vertices)
        if faces is not None:
            self.assertEqual(mesher.cdt.number_of_faces(), faces)
        count_edges, count_constrained = mesher.count_edges()
        if edges is not None:
            self.assertEqual(count_edges, edges)
        if constrained is not None:
            self.assertEqual(count_constrained, constrained)

    def build_two_overlapping_segments(self):
        segment1 = map(mesh.to_cgal_point, [(0, 0), (0, 2)])
        segment2 = map(mesh.to_cgal_point, [(0, 1), (0, 3)])
        (vA, vB), (cAB,) = self.mesher.insert_polyline(
            segment1, id="1", altitude=10)
        (vC, vD), (cCD,) = self.mesher.insert_polyline(
            segment2, id="2", color='blue')
        return (vA, vB, vC, vD, cAB, cCD)
        # Geometrical precondition checks
        self.assert_basic_counts(vertices=4, faces=0, edges=3, constrained=3)

    def _find_vertex_at(self, p):
        # Point comparison is NOT ROBUST : do no use in production
        p = mesh.to_cgal_point(p)
        for v in self.mesher.cdt.finite_vertices():
            if v.point() == p: # Not robust in real cases
                return v
        else:
            return None

    def build_two_crossing_segments(self, V_altitude=None):
        h_segment = map(mesh.to_cgal_point, [(-1, 0), (1, 0)])
        v_segment = map(mesh.to_cgal_point, [(0, -1), (0, 1)])
        (vA, vB), (cAB,) = self.mesher.insert_polyline(
            h_segment, id="H", altitude=10)
        if V_altitude is None:
            (vC, vD), (cCD,) =self.mesher.insert_polyline(
                v_segment, id="V", color='blue')
        else:
            (vC, vD), (cCD,) =self.mesher.insert_polyline(
                v_segment, id="V", color='blue', altitude=V_altitude)
        # Geometrical precondition checks
        self.assert_basic_counts(vertices=5, faces=4, edges=8, constrained=4)
        # Informations check
        vO = self._find_vertex_at((0, 0))
        self.assertIsNotNone(vO)
        return (vA, vB, vC, vD, cAB, cCD, vO)

    def assertEqualButNotIdentical(self, a, b):
        self.assertIsNot(a, b)
        self.assertEqual(a, b)

    def build_triangle(self):
        cdt = self.mesher.cdt
        (vA, vB), (cAB,) = self.mesher.insert_polyline(
            [(0, 0), (2, 0)], id='concrete', altitude=0.0)
        vC = self.mesher.insert_point((1, 1), altitude=10.0)
        self.assert_basic_counts(faces=1, vertices=3, edges=3, constrained=1)
        (edgeAB,) = [edge for edge in cdt.finite_edges()
                    if cdt.is_constrained(edge)]
        (faceABC,) = cdt.finite_faces()
        self.assert_basic_counts(faces=1, vertices=3, edges=3, constrained=1)
        return (vA, vB, vC, edgeAB, faceABC)

    def build_simple_scene(self):
        border = self.mesher.insert_polyline( #NB CCW
            [(0, 0), (6, 0), (6, 5), (0, 5)], close_it=True,
            material='concrete', altitude=0, id='border')
        hole = self.mesher.insert_polyline( # NB CW
            reversed([(2, 2), (5, 2), (5, 4), (2, 4)]), close_it=True,
            material=HIDDEN_MATERIAL.id, id='hole')
        line = self.mesher.insert_polyline(
            [(1, 4), (4, 1)], altitude=20, id='line')
        return (border, hole, line)

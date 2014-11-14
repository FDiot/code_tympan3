from functools import partial
from itertools import izip_longest
import unittest

import numpy as np
from numpy.testing import assert_array_equal
from shapely.geometry import LineString, Point, Polygon

from tympan.altimetry.datamodel import (InconsistentGeometricModel,
                                        GroundMaterial)
from tympan.altimetry import mesh
from altimetry_testutils import MesherTestUtilsMixin, runVisualTests, rect

if runVisualTests:
    from tympan.altimetry import visu


TestFailureException = unittest.TestCase.failureException


def assert_geometry_equal(actual, expected):
    """Assert that two shapely.geometry objects are equal"""
    if not expected.equals(actual):
        raise TestFailureException('%s != %s' % (actual, expected))


def assert_geometry_items_equal(actual, expected):
    """Assert that two sequences of shapely.geomety objects are equal"""
    marker = object()
    for idx, (a, e) in enumerate(izip_longest(actual, expected,
                                              fillvalue=marker)):
        if a is marker or e is marker:
            raise TestFailureException(
                'sequences have different number of elements')
        if not e.equals(a):
            raise TestFailureException(
                'elements %d mismatch: %s != %s' % (idx, a, e))


class MeshedCDTTC(unittest.TestCase, MesherTestUtilsMixin):

    def setUp(self):
        self.mesher = mesh.MeshedCDTWithInfo()

    def test_insert_point(self):
        points = [mesh.to_cgal_point((1, 1))]
        self.mesher.insert_polyline(points)
        self.assert_basic_counts(vertices=1, faces=0)

    def test_insert_3_point(self):
        points = [(1, 1), (1, 2), (3, 4)]
        self.mesher.insert_polyline(map(mesh.to_cgal_point, points),
                                    connected=False)
        self.assert_basic_counts(vertices=3, faces=1, edges=3, constrained=0)

    def test_insert_triangle(self):
        points = [(1, 1), (1, 2), (3, 4)]
        self.mesher.insert_polyline(map(mesh.to_cgal_point, points),
                                    close_it=True)
        self.assert_basic_counts(vertices=3, faces=1, edges=3, constrained=3)

    def test_insert_vee(self):
        points = [(-1, 2), (0, 0), (1, 2)]
        self.mesher.insert_polyline(map(mesh.to_cgal_point, points),
                                    close_it=False) # the default by the way
        self.assert_basic_counts(vertices=3, faces=1, edges=3, constrained=2)

    def test_info_simple_polyline(self):
        points = map(mesh.to_cgal_point, [(1, 1), (1, 2), (3,4)])
        vertices, input_constraints = self.mesher.insert_polyline(
            points, altitude=10)

        for constraint in input_constraints:
            self.assertEqual(self.mesher.input_constraint_infos(constraint),
                             {"altitude": 10})
        for vertex in vertices:
            self.assertEqual(self.mesher.input_vertex_infos(vertex),
                             {"altitude": 10})

    def assert_constraints_between(self, v1, v2, constraints):
        l_constraint = list(self.mesher.iter_input_constraint_overlapping((v1, v2)))
        self.assertItemsEqual(l_constraint, constraints)

    def test_overlapping_constraints(self):
        (vA, vB, vC, vD, cAB, cCD) = self.build_two_overlapping_segments()
        self.assert_constraints_between(vA, vC, [cAB])
        self.assert_constraints_between(vB, vC, [cAB, cCD])
        self.assert_constraints_between(vB, vD, [cCD])

    def test_overlapping_constraints_info(self):
        (vA, vB, vC, vD, cAB, cCD) = self.build_two_overlapping_segments()

        self.assertItemsEqual(list(self.mesher.iter_constraints_info_overlapping((vB, vC))),
                              [{"altitude":10, "id":"1"},
                               {"color": "blue", "id":"2"}])

    def test_explicit_edge_conversion(self):
        segment = map(mesh.to_cgal_point, [(0, 0), (0, 2)])
        (vA, vB), (cAB,) = self.mesher.insert_polyline(segment)
        (edge, ) = self.mesher.cdt.finite_edges()

        (fh, i) = self.mesher.half_edge_from_vertices_pair(vA, vB)
        self.assertEqual((fh, i), edge)
        (v1, v2) = self.mesher.vertices_pair_from_half_edge(fh, i)
        self.assertItemsEqual((v1, v2), (vA, vB))

        (fh, i) = self.mesher.half_edge_from_vertices_pair(vB, vA)
        self.assertEqual((fh, i), edge)
        (v1, v2) = self.mesher.vertices_pair_from_half_edge(fh, i)
        self.assertItemsEqual((v1, v2), (vB, vA))

    def test_orientation_in_edge_conversion(self):
        segment = map(mesh.to_cgal_point, [(0, 0), (2, 0)])
        (vA, vB), (cAB,) = self.mesher.insert_polyline(segment)
        cV = self.mesher.insert_point((1, 1))
        # NB ABC is CCW
        cdt = self.mesher.cdt

        (edgeAB,) = [edge for edge in cdt.finite_edges()
                    if cdt.is_constrained(edge)]
        (faceABC,) = cdt.finite_faces()

        (fh, i) = self.mesher.half_edge_from_vertices_pair(vB, vA)
        self.assertEqual(fh, faceABC)
        (fh, i) = self.mesher.half_edge_from_vertices_pair(vA, vB)
        self.assertTrue(cdt.is_infinite(fh))

    def test_ensured_edge_conversion(self):
        segment = map(mesh.to_cgal_point, [(0, 0), (0, 2)])
        (vA, vB), (cAB,) = self.mesher.insert_polyline(segment)
        (edge, ) = self.mesher.cdt.finite_edges()

        self.assertIs(edge, self.mesher.ensure_half_edge(edge))
        self.assertIs(cAB, self.mesher.ensure_vertices_pair(cAB))
        self.assertItemsEqual((vA, vB), self.mesher.ensure_vertices_pair(edge))
        self.assertEqual(edge, self.mesher.ensure_half_edge(cAB))

    def test_info_on_edges_crossing_polylines(self):
        (vA, vB, vC, vD, cAB, cCD, vO) = self.build_two_crossing_segments()

        edges_infos = self.mesher.fetch_constraint_infos_for_edges()
        self.assertEqual(len(edges_infos), 8)
        self.assertEqual(edges_infos[mesh.sorted_vertex_pair(vA, vO)],
                         [{'altitude': 10, 'id':'H'}])

    def test_input_constraints_around_polylines_crossing(self):
        (vA, vB, vC, vD, cAB, cCD, vO) = self.build_two_crossing_segments()

        input_constraints = set(self.mesher.iter_input_constraint_around(vO))
        self.assertEqual(input_constraints, set((cAB, cCD)))

    def test_constraints_info_on_intersection_of_polylines(self):
        (vA, vB, vC, vD, cAB, cCD, vO) = self.build_two_crossing_segments()

        vertex_infos = self.mesher.fetch_constraint_infos_for_vertices([vO])
        self.assertItemsEqual(vertex_infos[vO],
                              [{'id': 'H', 'altitude': 10},
                               {'id': 'V', 'color': 'blue'}])

    def test_faces_from_edge(self):
        # NB ABCD is given in counter-clock-wise orientation
        (vA, vB, vC, vD), _ = self.mesher.insert_polyline(
            [(0, 0), (6, 0), (6, 5), (0, 5)], close_it=True,
            material='concrete', altitude=0)

        (face_left, face_right) = self.mesher.faces_for_edge(vA, vB)
        self.assertTrue(self.mesher.cdt.is_infinite(face_right))
        self.assertFalse(self.mesher.cdt.is_infinite(face_left))
        (face_left, face_right) = self.mesher.faces_for_edge(vB, vA)
        self.assertFalse(self.mesher.cdt.is_infinite(face_right))
        self.assertTrue(self.mesher.cdt.is_infinite(face_left))

    def test_find_single_face(self):
        (v1, v2, v3), _ = self.mesher.insert_polyline(
            [(1, 1), (1, 2), (3, 4)], close_it=True)
        face, = self.mesher.cdt.finite_faces()

        found_face = self.mesher.face_for_vertices(v1, v2, v3)
        self.assertEqual(found_face, face)
        found_face = self.mesher.face_for_vertices(v1, v3, v2)
        self.assertEqual(found_face, face)

    def test_faces_from_input_constraint(self):
        # NB ABCD is given in counter-clock-wise orientation
        (vA, vB, vC, vD), _ = self.mesher.insert_polyline(
            [(0, 0), (2, 0), (2, 1), (0, 1)], close_it=True)
        (vM, vN), _ = self.mesher.insert_polyline([(1, 2), (1, -1)])
        # Get the two added vertices
        vP = self._find_vertex_at((1, 0))
        self.assertIsNotNone(vP)
        vQ = self._find_vertex_at((1, 1))
        self.assertIsNotNone(vQ)
        # Get the faces of interest
        f1 = self.mesher.face_for_vertices(vN, vP, vA)
        self.assertIsNotNone(f1)
        f2a = self.mesher.face_for_vertices(vP, vQ, vA)
        f2b = self.mesher.face_for_vertices(vP, vQ, vD)
        f2 = f2a or f2b
        self.assertIsNotNone(f2)
        f3 = self.mesher.face_for_vertices(vQ, vM, vD)
        self.assertIsNotNone(f3)

        faces_left, faces_right = mesh.left_and_right_faces(
            self.mesher.iter_faces_for_input_constraint(vM, vN))
        if runVisualTests:
            plotter = visu.MeshedCDTPlotter(self.mesher, title=self._testMethodName)
            plotter.plot_edges()
            points_left = [self.mesher.point_for_face(f) for f in faces_left]
            points_right = [self.mesher.point_for_face(f) for f in faces_right]
            for vertex, name in zip((vA, vB, vC, vD, vM, vN, vP, vQ),
                                    ("A", "B", "C", "D", "M", "N", "P", "Q")):
                plotter.annotate_vertex(vertex, name)
            for i, f in enumerate((f1, f2, f3), 1):
                plotter.annotate_finite_face(f, "F%d"%i)
            visu.plot_points_seq(plotter.ax, points_left, marker='<')
            visu.plot_points_seq(plotter.ax, points_right, marker='>')
            plotter.show()

        self.assertEqual(faces_right, (f1, f2, f3))

    @unittest.skipUnless(runVisualTests, "Set RUN_VISUAL_TESTS env. variable to run me")
    def test_mesh_refine_no_holes(self):
        (border, hole, line) = self.build_simple_scene()
        self.mesher.refine_mesh()
        plotter = visu.MeshedCDTPlotter(self.mesher, title=self._testMethodName)
        plotter.plot_edges()
        plotter.show()

    @unittest.skipUnless(runVisualTests, "Set RUN_VISUAL_TESTS env. variable to run me")
    def test_input_constraints_orientation(self):
        (border, hole, line) = self.build_simple_scene()
        plotter = visu.MeshedCDTPlotter(self.mesher, title=self._testMethodName)
        plotter.plot_edges()

        faces_left, faces_right = mesh.left_and_right_faces(
            self.mesher.iter_faces_for_input_polyline(hole[0], close_it=True))
        points_left = [self.mesher.point_for_face(f) for f in faces_left]
        points_right = [self.mesher.point_for_face(f) for f in faces_right]
        visu.plot_points_seq(plotter.ax, points_left, marker='<')
        visu.plot_points_seq(plotter.ax, points_right, marker='>')
        plotter.show()

    def test_copy(self):
        cdt = self.mesher.cdt
        (vA, vB, vC, _, _) = self.build_triangle()

        mesher2 = self.mesher.copy()

        vD = mesher2.insert_point((1, -1), altitude=20.0)
        self.assert_basic_counts(faces=1, vertices=3, edges=3, constrained=1)
        self.assert_basic_counts(faces=2, vertices=4, edges=5, constrained=1,
                                 mesher=mesher2)

        # Test use of faces and edges identifier across the copy
        vmap = self.mesher.vertices_map_to_other_mesh(mesher2)
        edgeAB2 = ((vmap[vA], vmap[vB]))

        self.assertEqual(mesher2.input_constraint_infos(edgeAB2),
                         mesher2.VertexInfo(id='concrete', altitude=0.0))
        self.assertIsNot(mesher2.input_constraint_infos(edgeAB2),
                         self.mesher.input_constraint_infos((vA, vB)))

    def test_mirror_half_edge(self):
        cdt = self.mesher.cdt
        (vA, vB, vC, edgeAB, faceABC) = self.build_triangle()
        mirror_half_edge = self.mesher.mirror_half_edge

        fh, i = mirror_half_edge(*edgeAB)
        self.assertTrue(cdt.is_infinite(fh))
        self.assertEqual(mirror_half_edge(fh, i), edgeAB)

    def test_locate(self):
        cdt = self.mesher.cdt
        (vA, vB, vC, edgeAB, faceABC) = self.build_triangle()
        edgeBA = self.mesher.mirror_half_edge(*edgeAB)
        locate = self.mesher.locate_point

        self.assertEqual(locate((1, 0.5)), (faceABC, None))
        self.assertIsNone(locate((1, -1))[0])
        self.assertEqual(locate((0, 0)), (faceABC, vA))
        self.assertIn(locate((1, 0)), (edgeAB, edgeBA))

        degenerate_mesher = mesh.MeshedCDTWithInfo()
        degenerate_mesher.insert_point((0, 0))
        with self.assertRaises(InconsistentGeometricModel):
            degenerate_mesher.locate_point((0, 1))

    def test_segment_intersection(self):
        points = [(1, 1), (1, 3), (3, 1)]
        self.mesher.insert_polyline(map(mesh.to_cgal_point, points),
                                    close_it=True)
        # No intersection.
        segment = (0, 0), (0, 3)
        inter = list(self.mesher.segment_intersection_points(segment))
        self.assertEqual(inter, [])
        # One intersection.
        segment = (0, 1.5), (1.1, 1.5)
        inter = list(self.mesher.segment_intersection_points(segment))
        self.assertEqual(len(inter), 1)
        assert_geometry_equal(inter[0], Point((1, 1.5)))
        # Two intersections, points only.
        segment = (0, 2), (4, 2)
        inter = list(self.mesher.segment_intersection_points(segment))
        assert_geometry_items_equal(inter, [Point((1, 2)), Point((2, 2))])

    def test_segment_intersection_2elements(self):
        points = [(1, 3), (3, 1), (3, 3)]
        self.mesher.insert_polyline(map(mesh.to_cgal_point, points),
                                    close_it=True)
        points = [(1, 1), (1, 3), (3, 1)]
        self.mesher.insert_polyline(map(mesh.to_cgal_point, points),
                                    close_it=True)
        segment = (0, 2), (4, 2)
        inter = list(self.mesher.segment_intersection_points(segment))
        assert_geometry_items_equal(
            inter, [Point((1, 2)), Point((2, 2)), Point((3, 2))])


class ElevationMeshTC(unittest.TestCase, MesherTestUtilsMixin):

    def setUp(self):
        self.mesher = mesh.ReferenceElevationMesh()

    def test_mandatory_altitude(self):
        with self.assertRaises(TypeError) as cm:
             self.mesher.insert_point((1, 0.5))

    def test_point_altitude(self):
        cdt = self.mesher.cdt
        (vA, vB, vC, edgeAB, faceABC) = self.build_triangle()
        slope = self.mesher.altitude_for_input_vertex(vC) / vC.point().y()

        self.assertEqual(self.mesher.altitude_for_input_vertex(vA), 0)
        self.assertEqual(self.mesher.point_altitude((0, 0)), 0) # vertex
        self.assertEqual(self.mesher.point_altitude((0.5, 0.5)), slope*0.5) # edge
        self.assertIs(self.mesher.point_altitude((1, -1)),
                      mesh.UNSPECIFIED_ALTITUDE) #out of bounds
        self.assertEqual(self.mesher.point_altitude((1, 0.5)), slope*0.5) # face

    def test_crossing_level_lines_same_altitude_merge_info(self):
        (vA, vB, vC, vD, cAB, cCD, vO) = self.build_two_crossing_segments(V_altitude=10)
        altitudes = self.mesher.merge_info_for_vertices(
            lambda i1, i2: i1.merge_with(i2), vertices=(vA, vO))

        self.assertItemsEqual(altitudes.keys(), (vA, vO))
        self.assertEqual(altitudes[vA].altitude, 10)
        self.assertEqual(altitudes[vO].altitude, 10)
        self.assertItemsEqual(altitudes[vO].ids, ["H", "V"])

    def test_crossing_level_lines_same_altitude_update_info(self):
        (vA, vB, vC, vD, cAB, cCD, vO) = self.build_two_crossing_segments(V_altitude=10)
        self.mesher.update_info_for_vertices(vertices=(vA, vO))

        self.assertEqual(self.mesher.vertices_info[vA].altitude, 10)
        self.assertEqual(self.mesher.vertices_info[vO].altitude, 10)
        self.assertItemsEqual(self.mesher.vertices_info[vO].ids, ["H", "V"])

    def test_crossing_level_lines_different_altitude(self):
        (vA, vB, vC, vD, cAB, cCD, vO) = self.build_two_crossing_segments(V_altitude=20)
        self.assertIsInstance(vO, mesh.Vertex_handle)
        with self.assertRaises(InconsistentGeometricModel) as cm:
            self.mesher.update_info_for_vertices((vO,))
        self.assertEqual(cm.exception.witness_point, (0.0, 0.0))
        self.assertItemsEqual(cm.exception.ids, ["H", "V"])

    def test_vertices_info_set_from_input(self):
        (vA, vB, vC, vD, cAB, cCD, vO) = self.build_two_crossing_segments(V_altitude=20)
        self.assertIsInstance(vO, mesh.Vertex_handle)
        vertices_info = self.mesher.vertices_info
        self.assertEqual(vertices_info[vA].altitude, 10)
        self.assertIs(vertices_info[vO].altitude, mesh.UNSPECIFIED_ALTITUDE)

    def test_copy_elevation_mesh(self):
        cdt = self.mesher.cdt
        (vA, vB, vC, _, _) = self.build_triangle()
        vD = self.mesher.insert_point((0, 1), altitude=0.0, id="stuff")

        mesher2 = self.mesher.copy()
        vmap = self.mesher.vertices_map_to_other_mesh(mesher2)

        self.assertEqual(self.mesher.vertices_info[vD],
                         mesher2.vertices_info[vmap[vD]])
        self.assertIsNot(self.mesher.vertices_info[vD],
                         mesher2.vertices_info[vmap[vD]])

    def test_copy_as_base_class_and_compute_altitude(self):
        cdt = self.mesher.cdt
        (vA, vB, vC, edgeAB, faceABC) = self.build_triangle()
        slope = self.mesher.altitude_for_input_vertex(vC) / vC.point().y()

        mesher2 = self.mesher.copy(class_=mesh.ElevationMesh)
        vD = mesher2.insert_point((1, 0.5)) # Altitude is missing and this should be OK
        mesher2.update_altitude_from_reference(self.mesher.point_altitude)

        self.assert_basic_counts(faces=1, vertices=3, edges=3, constrained=1)
        self.assert_basic_counts(faces=3, vertices=4, edges=6, constrained=1, mesher=mesher2)
        self.assertEqual(mesher2.vertices_info[vD].altitude, slope*0.5)


class MaterialMeshTC(unittest.TestCase, MesherTestUtilsMixin):

    def setUp(self):
        self.mesher = mesh.ElevationMesh()

    def test_flood(self):

        (border, hole, line) = self.build_simple_scene()

        flooder = self.mesher.flood_polygon(mesh.MaterialFaceFlooder,
                                            hole[0], close_it=True,
                                            flood_right=True) # hole is CW

        if runVisualTests:
            plotter = visu.MeshedCDTPlotter(self.mesher, title=self._testMethodName)
            plotter.plot_edges()
            seeds = [self.mesher.point_for_face(f) for f in flooder.visited]
            visu.plot_points_seq(plotter.ax, seeds, marker='*')
            plotter.show()

        face_in, expected_None = self.mesher.locate_point((3, 3))
        self.assertIsNone(expected_None)
        face_out, expected_None = self.mesher.locate_point((3, 4.5))
        self.assertIsNone(expected_None)

        self.assertEqual(len(flooder.visited), 4)
        self.assertIn(face_in, flooder.visited)
        self.assertNotIn(face_out, flooder.visited)

    def test_material_boundary_info(self):
        cdt = self.mesher.cdt
        (border, hole, line) = self.build_simple_scene()
        (va, vb), _ = line
        self.mesher.update_info_for_edges()

        for edge in self.mesher.iter_edges_for_input_constraint(va, vb):
            self.assertFalse(self.mesher.edges_info[edge].material_boundary)
        for edge in self.mesher.iter_edges_for_input_polyline(hole[0], close_it=True):
            edge = mesh.sorted_vertex_pair(*edge) # Important
            self.assertTrue(self.mesher.edges_info[edge].material_boundary)

    def test_landtake_flood_overwrite_material(self):
        (border, hole, line) = self.build_simple_scene()
        pond = self.mesher.insert_polyline( #NB CCW
            rect(3, 2.5, 4, 3.5), close_it=True,
            material='water', altitude=0, id='pond')
        self.mesher.update_info_for_edges()

        flooder = self.mesher.flood_polygon(mesh.LandtakeFaceFlooder,
                                            hole[0], close_it=True,
                                            flood_right=True) # hole is CW

        if runVisualTests:
            plotter = visu.MeshedCDTPlotter(self.mesher, title=self._testMethodName)
            plotter.plot_edges()
            marks = [self.mesher.point_for_face(f) for f in flooder.visited]
            visu.plot_points_seq(plotter.ax, marks, marker='*')
            plotter.show()

        face_in_both, expected_None = self.mesher.locate_point((3.25, 3))
        self.assertIsNone(expected_None)
        self.assertIn(face_in_both, flooder.visited)
        face_out, expected_None = self.mesher.locate_point((1, 1.25))
        self.assertIsNone(expected_None)
        self.assertNotIn(face_out, flooder.visited)

    def test_flood_two_materials(self):
        (border, hole, line) = self.build_simple_scene()
        pond = self.mesher.insert_polyline( #NB CCW
            rect(0.5, 0.5, 1.5, 1.5), close_it=True,
            material='water', altitude=0, id='pond')
        self.mesher.update_info_for_edges()

        flooder = self.mesher.flood_polygon(mesh.MaterialFaceFlooder,
                                            border[0], close_it=True)

        if runVisualTests:
            plotter = visu.MeshedCDTPlotter(self.mesher, title=self._testMethodName)
            plotter.plot_edges()
            marks = [self.mesher.point_for_face(f) for f in flooder.visited]
            visu.plot_points_seq(plotter.ax, marks, marker='*')
            plotter.show()

        face_in_pond, expected_None = self.mesher.locate_point((1, 1.25))
        self.assertIsNone(expected_None)
        self.assertNotIn(face_in_pond, flooder.visited)
        face_in_hole, expected_None = self.mesher.locate_point((3.25, 3))
        self.assertIsNone(expected_None)
        self.assertNotIn(face_in_hole, flooder.visited)


class ElevationProfileTC(unittest.TestCase):

    rectangles = [
        [(1, 1), (4, 1), (4, 4), (1, 4)],
        [(1.5, 1.5), (3.5, 1.5), (3.5, 3.5), (1.5, 3.5)],
        [(2, 2), (3, 2), (3, 3), (2, 3)],
    ]
    altitudes = (0, 0, 2)
    materials = (0, 1, 2)

    def setUp(self):
        self.mesh = mesh.ReferenceElevationMesh()
        # Three squares inside each others, with an outer zone at altitude 0
        # and an inner zone at altitude 2.
        for points, alt in zip(self.rectangles, self.altitudes):
            self.mesh.insert_polyline(map(mesh.to_cgal_point, points),
                                      altitude=alt, close_it=True)

    def test__point_at_distance(self):
        segment = LineString([(2, 2), (3, 2)])
        origin = Point(segment.coords[0])
        profile = mesh.ElevationProfile(self.mesh, segment)
        p = profile._point_at_distance(0)
        self.assertEqual(origin.distance(p), 0)
        self.assertEqual(p.coords[0], origin.coords[0])
        p = profile._point_at_distance(3)
        self.assertEqual(origin.distance(p), 3)
        self.assertEqual(p.coords[0], (5, 2))

    def test_direction(self):
        segment = LineString([(2, 0), (2, 4)])
        profile = mesh.ElevationProfile(self.mesh, segment)
        assert_array_equal(profile.direction, [0, 1])

    def test_point_altitude(self):
        segment = LineString([(2.5, 0), (2.5, 4)])
        profile = mesh.ElevationProfile(self.mesh, segment)
        self.assertIs(profile.point_altitude(0.5),
                      mesh.UNSPECIFIED_ALTITUDE)
        self.assertEqual(profile.point_altitude(2.5), 2.0)
        self.assertEqual(profile.point_altitude(1.5), 0.0)

    def test_interpolation(self):
        segment = LineString([(2.5, 0), (2.5, 5)])
        profile = mesh.ElevationProfile(self.mesh, segment)
        self.assertEqual(profile(0), 0)
        self.assertEqual(profile(1), 0)
        self.assertEqual(profile(2), 2)
        self.assertTrue(0 < profile(1.7) < 2)

    @staticmethod
    def _polygon_from_face(fh):
        coords = [(fh.vertex(i).point().x(), fh.vertex(i).point().y())
                  for i in range(3)]
        return Polygon(coords)

    def _material_by_face(self):
        material_by_face = {}
        # Map rectangles to a material.
        material_by_rectangle = [
            (Polygon(rect), mat)
            for mat, rect in zip(self.materials, self.rectangles)]
        for fh in self.mesh.cdt.finite_faces():
            # Some point inside the face triangle.
            point = self._polygon_from_face(fh).representative_point()
            # Loop on rectangles from inner-most one.
            for rect, mat in reversed(material_by_rectangle):
                if rect.contains(point):
                    material_by_face[fh] = GroundMaterial(
                        id_=mat, resistivity=mat)
                    break
            else:
                # Should not occur.
                raise Exception(
                    'could not find a rectangle containing %s' % point)
        return material_by_face

    def test_material_by_face(self):
        """Test _material_by_face test method"""
        material_by_face = self._material_by_face()
        for point, mat in zip([(1.2, 1.2), (1.7, 1.7), (2.2, 2.2)],
                              self.materials):
            fh, _ = self.mesh.locate_point(mesh.to_cgal_point(point))
            self.assertEqual(material_by_face[fh].id, mat)

    def test_point_data_with_material(self):
        """Test `point_data` method, using `material_by_face` dict."""
        segment = LineString([(2.5, 0), (2.5, 5)])
        profile = mesh.ElevationProfile(self.mesh, segment)
        # `point_material` returns the material at a given point.
        point_material = partial(profile.point_data,
                                 face_data=self._material_by_face())
        # Ouside domain.
        self.assertIs(point_material(0.1), mesh.UNSPECIFIED_MATERIAL)
        # Inside first rectangle.
        self.assertEqual(point_material(1.2).id, 0)
        # Inside second rectangle.
        self.assertEqual(point_material(1.7).id, 1)
        # Inside inner-most rectangle.
        self.assertEqual(point_material(2.5).id, 2)

    def test_point_data_vertex(self):
        """Test `point_data` method when the point is on a vertex."""
        # Beginning of segment is on a vertex of the mesh (first point of
        # outer rectangle).
        segment = LineString([(1, 1), (2, 3)])
        profile = mesh.ElevationProfile(self.mesh, segment)
        # Point on a vertex.
        p = self.rectangles[0][0]
        fh, vh = self.mesh.locate_point(p)
        self.assertIsInstance(vh, mesh.Vertex_handle)
        d = profile.point_data(0, face_data=self._material_by_face())
        self.assertEqual(d.id, 0)

    def assert_within(self, value, bounds):
        lb, ub = bounds
        if not lb <= value <= ub:
            self.fail('%s not within bounds %s' % (value, bounds))

    def test_sigma_interpolation(self):
        segment = LineString([(2.5, 0), (2.5, 5)])
        profile = mesh.ElevationProfile(self.mesh, segment)
        material_by_face = self._material_by_face()
        # Build an interpolator `sigma` on material resistivity.
        sigma_by_face = dict((fh, mat.resistivity)
                             for fh, mat in material_by_face.items())
        sigma = profile.face_data_interpolator(sigma_by_face)
        # Inside first rectangle.
        self.assert_within(sigma(1.2), [0, 1])
        self.assert_within(sigma(3.6), [0, 1])
        # Inside second rectangle.
        self.assert_within(sigma(1.7), [1, 2])
        # Middle of inner-most rectangle.
        self.assertGreaterEqual(sigma(2.5), 2)


if __name__ == '__main__':
    from utils import main
    main()

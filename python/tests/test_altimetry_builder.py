import unittest


from tympan.altimetry.datamodel import (InconsistentGeometricModel,
                                 LevelCurve)
from tympan.altimetry import mesh
from tympan.altimetry.builder import Builder

from altimetry_testutils import (MesherTestUtilsMixin, TestFeatures,
                                 rect, runVisualTests, left_and_right_faces)

if runVisualTests:
    from tympan.altimetry import visu


class AltimetryBuilderTC(unittest.TestCase, TestFeatures):

    def setUp(self):
        TestFeatures.build_features(self)
        TestFeatures.build_more_features_in_subsites(self)
        # NB Level curve A intersects this one : altitudes must be the same
        self.landtake_level_curve = LevelCurve(
            self.big_rect_coords, altitude=self.altitude_A, close_it=True,
            parent_site=self.mainsite, id="{Mainsite ref altitude}")
        self.builder = Builder(self.mainsite)

    @unittest.skipUnless(runVisualTests, "Set RUN_VISUAL_TESTS env. variable to run me")
    def test_plot(self):
        self.builder.merge_subsites()
        self.builder.build_altimetric_base()
        cleaned = self.builder.cleaned
        plotter = visu.MeshedCDTPlotter(self.builder.mesh, title=self._testMethodName)
        plotter.plot_edges()
        cleaned.equivalent_site.plot(plotter.ax, alt_geom_map=cleaned.geom)
        plotter.show()

    def check_vertices_props(self, mesher, points_and_expectations):
        for point, expected in points_and_expectations:
            fh, vh = mesher.locate_point(point)
            self.assertIsInstance(vh, mesh.Vertex_handle)
            info = mesher.vertices_info[vh]
            for k, v in expected.iteritems():
                if v is mesh.UNSPECIFIED_ALTITUDE:
                    self.assertIs(getattr(info, k), mesh.UNSPECIFIED_ALTITUDE)
                else:
                    self.assertEqual(getattr(info, k), v)

    def test_build_altimetric_base(self):
        self.builder.merge_subsites()
        for id_ in ["{Mainsite ref altitude}", "{Subsub level curve}"]:
            self.assertIn(id_, self.builder.equivalent_site.features_by_id)
            self.assertIn(id_, self.builder.cleaned.geom)

        self.builder.build_altimetric_base()
        self.assertIsNotNone(self.builder.mesh)

        common_expectations = [((0, 0), {'altitude': self.altitude_A})]
        self.check_vertices_props(self.builder.alti, common_expectations)
        self.check_vertices_props(self.builder.mesh, common_expectations)

    def test_build_triangulation(self):
        self.builder.merge_subsites()
        self.builder.build_altimetric_base()
        self.builder.build_triangulation()

        self.check_vertices_props( self.builder.mesh, [
            ((1, 1), {'altitude': self.altitude_A,
                      'ids': set(['{Grass area}', '{Level curve A}'])}),
            ((1, 9), {'altitude': mesh.UNSPECIFIED_ALTITUDE,
                      'ids': set(['{Grass area}'])}),
        ])

    def test_compute_elevations(self):
        self.builder.merge_subsites()
        self.builder.build_altimetric_base()
        self.builder.build_triangulation()
        pM = (8.5, 6.5) # in the corner of Level curve B
        vM = self.builder.mesh.insert_point(pM)
        self.builder.compute_elevations()

        self.check_vertices_props( self.builder.mesh, [
            ((1, 1), {'altitude': self.altitude_A,
                      'ids': set(['{Grass area}', '{Level curve A}'])}),
            (pM, {'altitude': self.level_curve_B.altitude,}),
        ])

if __name__ == '__main__':
    from utils import main
    main()

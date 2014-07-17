import unittest


from altimetry.datamodel import (InconsistentGeometricModel,
                                 LevelCurve)
from altimetry import visu
from altimetry import mesh
from altimetry.builder import Builder

from altimetry_testutils import (MesherTestUtilsMixin, TestFeatures,
                                 rect, runVisualTests, left_and_right_faces)


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

    def test_altimetric_base(self):
        self.builder.merge_subsites()
        self.builder.build_altimetric_base()
        cleaned = self.builder.cleaned

        self.assertIsNotNone(self.builder.mesh)

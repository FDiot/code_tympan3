import tempfile
import unittest
import os

from numpy.testing.utils import assert_allclose

from tympan.altimetry.datamodel import (InconsistentGeometricModel, HIDDEN_MATERIAL,
                                        LevelCurve, InfrastructureLandtake)
from tympan.altimetry import mesh, export_to_ply
from tympan.altimetry.builder import Builder

from altimetry_testutils import (MesherTestUtilsMixin, TestFeatures,
                                 rect, runVisualTests)

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
        self.builder.build_triangulation()
        self.builder.fill_material_and_landtakes()
        cleaned = self.builder.cleaned
        plotter = visu.MeshedCDTPlotter(self.builder.mesh, title=self._testMethodName)
        cleaned.equivalent_site.plot(plotter.ax, alt_geom_map=cleaned.geom)
        plotter.plot_edges()

        fh, expected_None = self.builder.mesh.locate_point((4, 4))
        self.assertIsNone(expected_None)
        plotter.plot_face(fh, material_id='concrete')
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
        self.builder.compute_informations()

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
        self.builder.compute_informations()
        pM = (8.5, 6.5) # in the corner of Level curve B
        vM = self.builder.mesh.insert_point(pM)
        self.builder.compute_elevations()

        self.check_vertices_props( self.builder.mesh, [
            ((1, 1), {'altitude': self.altitude_A,
                      'ids': set(['{Grass area}', '{Level curve A}'])}),
            (pM, {'altitude': self.level_curve_B.altitude,}),
        ])

    def test_vertices_by_feature(self):
        coords = rect(9, 9, 10, 10)
        self.building = InfrastructureLandtake(coords,
                                               parent_site=self.mainsite,
                                               id="{some building}")
        self.builder.merge_subsites()
        self.builder.build_altimetric_base()
        self.builder.build_triangulation()
        self.builder.compute_informations()

        vertices = self.builder._vertices_for_feature[self.building.id]

        for i, v in enumerate(vertices):
            self.assertEquals(v.point(), mesh.to_cgal_point(coords[i % len(coords)]))

    @unittest.skipUnless(runVisualTests, "Set RUN_VISUAL_TESTS env. variable to run me")
    def test_plot_complete_processing(self):
        self.builder.complete_processing()
        cleaned = self.builder.cleaned
        plotter = visu.MeshedCDTPlotter(self.builder.mesh, title=self._testMethodName)
        cleaned.equivalent_site.plot(plotter.ax, alt_geom_map=cleaned.geom)
        plotter.plot_edges()

        for fh in self.builder.mesh.cdt.finite_faces():
            material = self.builder.material_by_face.get(fh)
            if material is None : continue
            plotter.plot_face(fh, material_id=material.id)
        plotter.show()

    @unittest.skipUnless(runVisualTests, "Set RUN_VISUAL_TESTS env. variable to run me")
    def test_plot_landtake_flooding(self):
        self.builder.merge_subsites()
        self.builder.build_altimetric_base()
        self.builder.build_triangulation()
        self.builder.compute_informations()
        self.builder.compute_elevations()

        flood_seeds = self.builder.fill_polygonal_feature(self.building, mesh.LandtakeFaceFlooder)

        cleaned = self.builder.cleaned
        plotter = visu.MeshedCDTPlotter(self.builder.mesh, title=self._testMethodName)
        cleaned.equivalent_site.plot(plotter.ax, alt_geom_map=cleaned.geom)
        plotter.plot_edges()

        for fh in flood_seeds:
            marks = [self.builder.mesh.point_for_face(f) for f in flood_seeds]
            visu.plot_points_seq(plotter.ax, marks, marker='*')
        for fh in self.builder.mesh.cdt.finite_faces():
            material = self.builder.material_by_face.get(fh)
            if material is None : continue
            plotter.plot_face(fh, material_id=material.id)
        plotter.show()

    def test_join_with_landtakes(self):
        self.builder.complete_processing()
        # fetch infrastructure landtakes faces assuming they have
        # HIDDEN_MATERIAL.
        landtake_faces = (fh for fh, mat in
                            self.builder.material_by_face.iteritems()
                          if mat == HIDDEN_MATERIAL)
        altitudes = [self.builder.mesh.vertices_info[fh.vertex(i)].altitude
                     for fh in landtake_faces for i in range(3)]
        assert_allclose(altitudes, altitudes[0])

    def test_ply_export(self):
        from plyfile import PlyData
        self.builder.complete_processing()
        mesh, material_by_face = self.builder.mesh, self.builder.material_by_face
        try:
            # delete=False and manual removal to avoid pb on windows platform
            # (though proper fix would imply stream based api)
            with tempfile.NamedTemporaryFile(delete=False) as f:
                export_to_ply(mesh, material_by_face, f.name)
                data = PlyData.read(f.name)
                vertices = data['vertex']
                faces = data['face']
                materials = data['material']
                self.assertEqual(vertices.count, 119)
                self.assertEqual(faces.count, 198)
                self.assertEqual(materials.count, 5)
                materials_id = [''.join(map(chr, data)) for data, in materials.data]
                self.assertItemsEqual(materials_id, ['__default__', '__hidden__',
                                                     'grass', 'pine', 'Water'])
        finally:
            os.remove(f.name)

if __name__ == '__main__':
    from utils import main
    main()

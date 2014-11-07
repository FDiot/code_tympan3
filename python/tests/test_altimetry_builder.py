import tempfile
import unittest
import os

from numpy.testing.utils import assert_allclose

from tympan.altimetry.datamodel import (InconsistentGeometricModel, HIDDEN_MATERIAL,
                                        LevelCurve, InfrastructureLandtake)
from tympan.altimetry import mesh, export_to_ply, builder

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

    @unittest.skipUnless(runVisualTests, "Set RUN_VISUAL_TESTS env. variable to run me")
    def test_plot(self):
        equivalent_site, mesh, _ = builder.build_altimetry(self.mainsite)
        plotter = visu.MeshedCDTPlotter(mesh, title=self._testMethodName)
        equivalent_site.plot(plotter.ax, alt_geom_map=equivalent_site._cleaner.geom)
        plotter.plot_edges()

        fh, expected_None = mesh.locate_point((4, 4))
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
        cleaner = builder.recursively_merge_all_subsites(self.mainsite)
        for id_ in ["{Mainsite ref altitude}", "{Subsub level curve}"]:
            self.assertIn(id_, cleaner.equivalent_site.features_by_id)
            self.assertIn(id_, cleaner.geom)
        msite = cleaner.merged_site()
        alti = builder.MeshBuilder(msite)._build_altimetric_base()
        common_expectations = [((0, 0), {'altitude': self.altitude_A})]
        self.check_vertices_props(alti, common_expectations)

    def test_build_triangulation(self):
        cleaner = builder.recursively_merge_all_subsites(self.mainsite)
        msite = cleaner.merged_site()
        mbuilder = builder.MeshBuilder(msite)
        alti = mbuilder._build_altimetric_base()
        bmesh = mbuilder._build_triangulation(alti)
        mbuilder._compute_informations(bmesh)
        self.check_vertices_props(bmesh, [
            ((1, 1), {'altitude': self.altitude_A,
                      'ids': set(['{Grass area}', '{Level curve A}'])}),
            ((1, 9), {'altitude': mesh.UNSPECIFIED_ALTITUDE,
                      'ids': set(['{Grass area}'])}),
        ])

    def test_compute_elevations(self):
        cleaner = builder.recursively_merge_all_subsites(self.mainsite)
        msite = cleaner.merged_site()
        mbuilder = builder.MeshBuilder(msite)
        alti = mbuilder._build_altimetric_base()
        bmesh = mbuilder._build_triangulation(alti)
        mbuilder._compute_informations(bmesh)
        pM = (8.5, 6.5) # in the corner of Level curve B
        vM = bmesh.insert_point(pM)
        mbuilder._compute_elevations(bmesh, alti)

        self.check_vertices_props(bmesh, [
            ((1, 1), {'altitude': self.altitude_A,
                      'ids': set(['{Grass area}', '{Level curve A}'])}),
            (pM, {'altitude': self.level_curve_B.altitude,}),
        ])

    def test_vertices_by_feature(self):
        coords = rect(9, 9, 10, 10)
        self.building = InfrastructureLandtake(coords,
                                               parent_site=self.mainsite,
                                               id="{some building}")
        cleaner = builder.recursively_merge_all_subsites(self.mainsite)
        msite = cleaner.merged_site()
        mbuilder = builder.MeshBuilder(msite)
        alti = mbuilder._build_altimetric_base()
        bmesh = mbuilder._build_triangulation(alti)
        mbuilder._compute_informations(bmesh)

        vertices = mbuilder.vertices_for_feature[self.building.id]

        for i, v in enumerate(vertices):
            self.assertEquals(v.point(), mesh.to_cgal_point(coords[i % len(coords)]))

    def test_materials(self):
        equivalent_site, bmesh, material_by_face = builder.build_altimetry(
            self.mainsite)
        materials_id = set(mat.id for mat in material_by_face.values())
        self.assertItemsEqual(materials_id, ['__default__', '__hidden__',
                                             'grass', 'pine', 'Water'])

    @unittest.skipUnless(runVisualTests, "Set RUN_VISUAL_TESTS env. variable to run me")
    def test_plot_complete_processing(self):
        equivalent_site, mesh, material_by_face = builder.build_altimetry(
            self.mainsite)
        plotter = visu.MeshedCDTPlotter(mesh, title=self._testMethodName)
        equivalent_site.plot(
            plotter.ax, alt_geom_map=equivalent_site._cleaner.geom)
        plotter.plot_edges()

        for fh in mesh.cdt.finite_faces():
            material = material_by_face.get(fh)
            if material is None : continue
            plotter.plot_face(fh, material_id=material.id)
        plotter.show()

    @unittest.skipUnless(runVisualTests, "Set RUN_VISUAL_TESTS env. variable to run me")
    def test_plot_landtake_flooding(self):
        cleaner = builder.recursively_merge_all_subsites(self.mainsite)
        site = cleaner.merged_site()
        mbuilder = builder.MeshBuilder(msite)
        bmesh = mbuilder.build_mesh(refine=False)

        mfiller = builder.MeshFiller(bmesh, mbuilder.vertices_for_feature)
        flood_seeds = mfiller._fill_polygonal_feature(
            self.building, mesh.LandtakeFaceFlooder)

        plotter = visu.MeshedCDTPlotter(bmesh, title=self._testMethodName)
        cleaner.equivalent_site.plot(plotter.ax, alt_geom_map=cleaner.geom)
        plotter.plot_edges()

        for fh in flood_seeds:
            marks = [bmesh.point_for_face(f) for f in flood_seeds]
            visu.plot_points_seq(plotter.ax, marks, marker='*')
        material_by_faces = mfiller.fill_material_and_landtakes(
            self.mainsite, cleaner)
        for fh in bmesh.cdt.finite_faces():
            material = material_by_face.get(fh)
            if material is None : continue
            plotter.plot_face(fh, material_id=material.id)
        plotter.show()

    def test_join_with_landtakes(self):
        equivalent_site, mesh, material_by_face = builder.build_altimetry(
            self.mainsite)
        # fetch infrastructure landtakes faces assuming they have
        # HIDDEN_MATERIAL.
        landtake_faces = (fh for fh, mat in material_by_face.iteritems()
                          if mat == HIDDEN_MATERIAL)
        altitudes = [mesh.vertices_info[fh.vertex(i)].altitude
                     for fh in landtake_faces for i in range(3)]
        assert_allclose(altitudes, altitudes[0])

    def test_ply_export(self):
        from plyfile import PlyData
        _, mesh, material_by_face = builder.build_altimetry(self.mainsite)
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
                materials_id = [''.join(map(chr, data)) for data, in materials.data]
                self.assertItemsEqual(materials_id, ['__default__', '__hidden__',
                                                     'grass', 'pine', 'Water'])
                self.assertEqual(materials.count, 5)
        finally:
            os.remove(f.name)

if __name__ == '__main__':
    from utils import main
    main()

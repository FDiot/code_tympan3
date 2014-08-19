import sys
import os, os.path as osp
import unittest

import numpy as np
from numpy.testing import assert_allclose

from utils import TympanTC, TEST_PROBLEM_DIR

import tympan.altimetry.process_altimetry as tyalti

class TestProcessAltimetry(TympanTC):

    def test_process_single_landtake(self):
        with self.no_output():
            asite = tyalti.process_site_altimetry(
                osp.join(TEST_PROBLEM_DIR, '1_PROJET_Site_emprise_seule.xml'))
        landtake = asite.build_coordinates()[0]
        self.assertEqual(asite.id, "{f1a57b76-c918-4f63-a74b-7c67b7179df9}")
        self.assertEqual(landtake, [(-200.0,200.0), (200.0, 200.0),
                                    (200.0, -200.0), (-200.0, -200.0)])
        level_curves = asite.level_curves
        # No level curves are explicitely defined in the XML project, but
        # useEmpriseAsCrbNiv option is set to 1 so a level curve will have been
        # defined with the coordinates of the site and an altitude of 0 meters
        # (altiEmprise = 0)
        self.assertEqual(len(level_curves), 1)
        lcurve = level_curves[0]
        self.assertEqual(lcurve.altitude, 0)
        self.assertEqual(lcurve.build_coordinates()[0], landtake)

    def test_process_subsite(self):
        """ Open a project with a site and a subsite and check the altimetry
        created accordingly
        """
        with self.no_output():
            asite = tyalti.process_site_altimetry(
                osp.join(TEST_PROBLEM_DIR, 'site_with_subsite.xml'))
        # Main site:
        self.assertEqual(asite.id, "{5634c66e-2ad9-4d6f-a569-e0449337cea2}")
        landtake = asite.build_coordinates()[0]
        self.assertEqual(landtake, [(-94.0,117.0), (41.0, 114.0),
                                    (37.0, -16.0), (-153.0, -27.0)])
        # No level curves are explicitely defined in the XML project, and
        # useEmpriseAsCrbNiv option is set to 0, but as no level curves are
        # defined for this project, one will have been created
        # with the coordinates of the site and an altitude of 0 meters
        # (altiEmprise = 0)
        self.assertEqual(len(asite.level_curves), 1)
        lcurve = asite.level_curves[0]
        self.assertEqual(lcurve.altitude, 0)
        self.assertEqual(lcurve.build_coordinates()[0], landtake)
        # Subsite
        subsites = asite.subsites
        self.assertEqual(len(subsites), 1) # Check there is just 1 subsite
        subsite = subsites[0]
        self.assertEqual(subsite.id, "{f69851a7-fa13-4cff-b024-3ef44f4e7a40}")
        sbsite_landtake = subsite.build_coordinates()[0]
        self.assertEqual(sbsite_landtake, [(-83.0,98.0), (-40.0, 73.0),
                                           (-98.0, 28.0), (-105.0, 82.0)])
        sbsite_level_curves = subsite.level_curves
        # same as above
        self.assertEqual(len(sbsite_level_curves), 1)
        sbsite_lcurve = sbsite_level_curves[0]
        self.assertEqual(sbsite_lcurve.altitude, 0)
        self.assertEqual(sbsite_lcurve.build_coordinates()[0], sbsite_landtake)

    def test_process_level_curve(self):
        with self.no_output():
            asite = tyalti.process_site_altimetry(
                osp.join(TEST_PROBLEM_DIR, '2_PROJET_Site_une_courbe_seule.xml'))
        level_curves = asite.level_curves
        self.assertEqual(len(level_curves), 1)
        lcurve = level_curves[0]
        self.assertEqual(lcurve.id, "{393923e3-359c-4ce4-811e-3b4662415312}")
        self.assertEqual(lcurve.altitude, 0)
        self.assertEqual(lcurve.build_coordinates()[0],
                         [(-197.0, 197.0), (197.0, 197.0), (197.0, -195.0),
                          (-196.0, -197.0), (-197.0, 197.0)])

    def test_process_water_body(self):
        with self.no_output():
            asite = tyalti.process_site_altimetry(
                osp.join(TEST_PROBLEM_DIR, '7_PROJET_Site_emprise_seule_avec_plan_eau.xml'))
        mat_areas = asite.material_areas # water bodies are treated as material areas
        # There is 1 water body + 1 default material area (there is always one)
        # (here just check the water body)
        self.assertEqual(len(mat_areas), 2)
        lake = mat_areas[1]
        self.assertEqual(lake.build_coordinates()[0],
                         [(-83.0, -104.0), (-137.0, -80.0), (-141.0, -45.0),
                          (-117.0, 7.0), (-90.0, 22.0), (-49.0, 21.0),
                          (-19.0, 24.0), (-2.0, 64.0), (54.0, 95.0),
                          (109.0, 66.0), (122.0, -4.0), (75.0, -48.0),
                          (26.0, -62.0), (3.0, -115.0), (-33.0, -153.0),
                          (-75.0, -136.0)])
        self.assertEqual(lake.id, "{1d99f26d-83df-4404-94a4-7e3c5eccce13}")

    def test_process_material_area(self):
        with self.no_output():
            asite = tyalti.process_site_altimetry(
                osp.join(TEST_PROBLEM_DIR,
                         '10_PROJET_SITE_emprise_non_convexe_avec_butte_et_terrains.xml'))
        areas = asite.material_areas
        # There are 2 material areas + 1 water body
        self.assertEqual(len(areas), 3)
        # Check material area 1
        self.assertEqual(areas[0].id, "{4f084d7d-fc0b-4f1e-aaae-939cee5fa200}")
        self.assertEqual(len(areas[0].build_coordinates()[0]), 0) # no points
        self.assertEqual(areas[0].material_id, "{715dbf77-3d40-423d-9cae-2bd2dfdd5af9}")
        # Check material area 2
        self.assertEqual(areas[1].id, "{28bf9ef3-eb38-4de4-9921-8b76bf164f09}")
        self.assertEqual(areas[1].build_coordinates()[0],
                         [(-13.0, 176.0), (-60.0, 161.0), (-87.0, 140.0),
                          (-91.0, 103.0), (-75.0, 83.0), (-44.0, 76.0),
                          (29.0, 93.0), (112.0, 123.0), (125.0, 173.0),
                          (77.0, 185.0), (17.0, 185.0), (-4.0, 178.0)])
        self.assertEqual(areas[1].material_id, "{6fb2a2c5-e329-4c8a-9ec9-311072bddc73}")
        # Check water body
        self.assertEqual(areas[2].id, "{1d99f26d-83df-4404-94a4-7e3c5eccce13}")
        self.assertEqual(areas[2].build_coordinates()[0],
                         [(-83.0, -104.0), (-137.0, -80.0), (-141.0, -45.0),
                          (-117.0, 7.0), (-90.0, 22.0), (-49.0, 21.0),
                          (-19.0, 24.0), (-2.0, 64.0), (54.0, 95.0),
                          (109.0, 66.0), (122.0, -4.0), (75.0, -48.0),
                          (26.0, -62.0), (3.0, -115.0), (-33.0, -153.0),
                          (-75.0, -136.0)])


if __name__ == '__main__':
    from utils import main, config_cython_extensions_path
    config_cython_extensions_path()
    main()

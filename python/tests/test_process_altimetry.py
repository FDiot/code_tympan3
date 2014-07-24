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
            asite = tyalti.process_site_altimetry(osp.join(TEST_PROBLEM_DIR,
                                                           '1_PROJET_Site_emprise_seule.xml'))
        landtake = asite.build_coordinates()[0]
        self.assertEqual(asite.id, "{f1a57b76-c918-4f63-a74b-7c67b7179df9}")
        self.assertEqual(landtake, [(-200.0,200.0), (200.0, 200.0),
                                  (200.0, -200.0), (-200.0, -200.0)])
        level_curves = asite.level_curves
        # No level curves are explicitely defined in the XML project, but
        # useEmpriseAsCrbNiv option is set to 1 so a level curve will have been
        # defined with the coordinates of the site and an altitude of 0 meters
        # (altiEmprise = 0 in the XML project)
        self.assertEqual(len(level_curves), 1)
        lcurve = level_curves[0]
        self.assertEqual(lcurve.altitude, 0)
        self.assertEqual(lcurve.build_coordinates()[0], landtake)

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

    def test_process_water_bodies(self):
        with self.no_output():
            asite = tyalti.process_site_altimetry(
                osp.join(TEST_PROBLEM_DIR, '7_PROJET_Site_emprise_seule_avec_plan_eau.xml'))
        lakes = asite.material_areas # water bodies are treated as material areas
        self.assertEqual(len(lakes), 1) # just 1 water body, no other material areas
        lake = lakes[0]
        self.assertEqual(lake.build_coordinates()[0],
                         [(-83.0, -104.0), (-137.0, -80.0), (-141.0, -45.0),
                          (-117.0, 7.0), (-90.0, 22.0), (-49.0, 21.0),
                          (-19.0, 24.0), (-2.0, 64.0), (54.0, 95.0),
                          (109.0, 66.0), (122.0, -4.0), (75.0, -48.0),
                          (26.0, -62.0), (3.0, -115.0), (-33.0, -153.0),
                          (-75.0, -136.0)])
        self.assertEqual(lake.id, "{1d99f26d-83df-4404-94a4-7e3c5eccce13}")

if __name__ == '__main__':
    from utils import main, config_cython_extensions_path
    config_cython_extensions_path()
    main()

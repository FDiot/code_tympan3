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

if __name__ == '__main__':
    from utils import main, config_cython_extensions_path
    config_cython_extensions_path()
    main()

import sys
import os, os.path as osp
import unittest

import numpy as np
from numpy.testing import assert_allclose

from utils import TEST_DATA_DIR, TympanTC, no_output

with no_output():
    import tympan._business2solver as bus2solv

_HERE = osp.realpath(osp.dirname(__file__))

class TestPyTam(TympanTC):

    def test_hierarchy(self):
        project = self.load_project(
            'projects-panel',
            "10_PROJET_SITE_emprise_non_convexe_avec_butte_et_terrains.xml")
        site = project.site
        childs = site.childs()
        for c in childs:
            self.assertRegexpMatches(c.name(),'Infrastructure|Topographie')

    def test_base(self):
        # XXX This test uses expected bad values provided by the current
        # implementation
        project = self.load_project('solver_export', "base.xml")
        model = bus2solv.build_solver_model(project).model
        self.assertEqual(model.npoints, 6) # OK
        self.assertEqual(model.ntriangles, 5) # XXX should be 4
        self.assertEqual(model.nmaterials, 5) # XXX should be 1
        # FIXME the default material is replicated once per triangle.
        # TODO : how to test the altitude of a point ? or access a triangle at
        # some place ?
        # TODO to be completed: cf. ticket #1468184

if __name__ == '__main__':
    unittest.main()

import unittest

import os
from utils import TympanTC
from _util import TOOLBOX_DATA_DIR
from compare_tympan_project import App


class Test(TympanTC):

    def test_compare_same_projects(self):
        """ Run the comparison on the same project (reference) and check there is no difference """
        app = App(None, False)
        xml_file = "Reference_project.xml"
        app.load_reference_xml_file(xml_file)
        app.load_compared_xml_file(xml_file)
        app.output_file = "same.xlsx"  # Output
        self.assertEqual(app.launch_compare(), True)

    def test_compare_very_different_projects(self):
        """ Run the comparison on different projects about elements/topography/model """
        app = App(None, False)
        app.load_reference_xml_file("Reference_project.xml")
        app.load_compared_xml_file("Different_project.xml")
        app.output_file = "different.xlsx"  # Output
        self.assertEqual(app.launch_compare(), False)

    def test_compare_different_projects(self):
        """ Run the comparison on project with different results cause the calculation hasn't be relaunched """
        app = App(None, False)
        app.load_reference_xml_file("Reference_project.xml")
        app.load_compared_xml_file("Compared_project.xml")
        app.output_file = "different_results.xlsx"  # Output
        self.assertEqual(app.launch_compare(), False)

    def test_tolerance(self):
        """ Check the results are the same with a very high tolerance """
        app = App(None, False)
        app.load_reference_xml_file("Reference_project.xml")
        app.load_compared_xml_file("Compared_project.xml")
        app.output_file = "same_with_high_tolerance.xlsx"  # Output
        app.tolerance = 1e6
        self.assertEqual(app.launch_compare(), True)

    def test_relaunch(self):
        """ Check the results are the same after relaunching the calculation """
        app = App(None, False)
        app.load_reference_xml_file("Reference_project.xml")
        app.load_compared_xml_file("Compared_project.xml")
        app.output_file = "same_after_relaunch.xlsx"  # Output
        app.relaunch = 1
        self.assertEqual(app.launch_compare(), True)

    def test_global(self):
        """ Global test """
        reference_project = "modele_test.xml"  # Complex model
        app = App(None, False)
        app.load_reference_xml_file(reference_project)
        app.load_compared_xml_file(reference_project)
        self.assertEqual(app.launch_compare(), True)


if __name__ == '__main__':
    os.chdir(os.path.join(TOOLBOX_DATA_DIR, 'Compare_Tympan_Project'))
    unittest.main()

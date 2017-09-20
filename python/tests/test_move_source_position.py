from __future__ import print_function

import unittest
import sys, os
import numpy as np

from utils import TympanTC
from tympan.models.project import Project
from _util import import_infra, moyenne_mesh, compare_xml_results, line_count, TYMPAN_DATA_DIR
from move_source_position import main


class Test(TympanTC):

    def test_moyenne_mesh(self):
        """ Test the average (E/Lot1-010) """
        ndarray = np.array([-0.2, 1.1, 2.1])
        np.testing.assert_equal(moyenne_mesh(ndarray), 1.0)

    def test_import_infra(self):
        """ Test loading an infrastructure (E/Lot1-021) """
        project = Project.create()
        # Read an engine and check
        infra = import_infra("A320.xml", "engine")
        self.assertEqual(len(infra.engines), 1)
        engine = infra.engines[0]
        self.assertEqual(engine.name, "Machine 0")
        self.assertEqual(engine.hauteur, 0.0)
        # Local frame:
        self.assertEqual(engine.position.x, 0.0)
        self.assertEqual(engine.position.y, 0.0)
        self.assertEqual(engine.position.z, 0.0)

        # Read a building and check
        infra = import_infra("Building.xml", "building")
        self.assertEqual(len(infra.buildings), 1)
        building = infra.buildings[0]
        self.assertEqual(building.name, "MyBuilding")
        self.assertEqual(building.hauteur, 0.0)
        # Local frame:
        self.assertEqual(building.position.x, 0.0)
        self.assertEqual(building.position.y, 0.0)
        self.assertEqual(building.position.z, 0.0)

        # Check a no radiant building is refused:
        try:
            infra = import_infra("Building_no_radiant.xml", "building")
        except:
            print("Ok, non radiant building is refused as expected.")
        else:
            print("Non radiant building should be refused.")
            sys.exit(-1)

    def test_non_regression(self):
        """ Check the results compared to expected ones """
        return # ToDo provisoire
        main("Source_mobile.xml", [["engine", "A320.xml", "A320.csv"]], "Resultat.xml", gui=False)
        compare_xml_results("Resultat.xml", "Reference.xml", self)

    def test_global(self):
        """  Global test (E/Lot1-022) """
        return # ToDo provisoire
        objects = []
        # Engine moving plane in circle:
        objects.append(["engine", "A320.xml", "Cercle.csv"])
        main("Test_global.xml", objects, "Cercle.xml", gui=False)
        # Check in the results that the mean is the same than each contribution
        project = Project.from_xml("Cercle.xml")
        # Check the number of computations is the number of lines + initial computation + average computation
        nb_calc = line_count("Cercle.csv") + 2
        self.assertEqual(len(project.computations), nb_calc)
        # Compare the the contribution:
        reference = None
        for num_calc in range(1, nb_calc - 1):
            calc = project.computations[num_calc]
            rec = calc.result.receptors[0]
            src = calc.result.sources[0]
            contribution = calc.result.spectrum(rec, src).values
            if num_calc == 1:
                reference = contribution
            # Check each calculation is the same:
            np.testing.assert_array_equal(reference, contribution)

if __name__ == '__main__':
    os.chdir(os.path.join(TYMPAN_DATA_DIR, 'Source_Mobile'))
    unittest.main()

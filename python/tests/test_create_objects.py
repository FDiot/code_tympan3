import unittest

import os
from utils import TympanTC
from tympan.models.project import Project
from _util import import_infra, line_count, TOOLBOX_DATA_DIR
from create_objects import main

class Test(TympanTC):

    def test_global(self):
        """ Global test (E/Lot1-032) """
        # Building a project by adding sources, receptors, engines, buildings
        project_xml = "Final_project.xml"

        # Create a new project with sources located on positions from Spirale_log.csv:
        main("", "Spirale_log.csv", "source", "MySource", "", project_xml)

        # Update the project with receptors located on positions from Recepteur.csv:
        main("", "Recepteur.csv", "receptor", "MyReceptor", project_xml, project_xml)

        # Update the project with engine Machine.xml located on positions from Machine.csv:
        main("Machine.xml", "Machine.csv", "engine", "MyEngine", project_xml, project_xml)

        # Update the project with building Building.xml located on positions from Building.csv:
        main("Building.xml", "Building.csv", "building", "MyBuilding", project_xml, project_xml)

        # Check all objects are here in the final project
        # based on the number of lines in CSV file minus 1, the header
        project = Project.from_xml("Final_project.xml")
        nsources = line_count('Spirale_log.csv') - 1
        self.assertEqual(len(project.site.user_sources), nsources)
        nengines = line_count('Machine.csv') - 1
        self.assertEqual(len(project.site.engines), nengines)
        nbuildings = line_count('Building.csv') - 1
        self.assertEqual(len(project.site.buildings), nbuildings)
        nreceptors = line_count('Recepteur.csv') - 1
        self.assertEqual(len(project.user_receptors), nreceptors)

if __name__ == '__main__':
    os.chdir(os.path.join(TOOLBOX_DATA_DIR, 'Crea_Objets'))
    unittest.main()

import unittest

import os
from utils import TympanTC
from tympan.models.project import Project
from _util import line_count, TOOLBOX_DATA_DIR
from create_objects import main


class Test(TympanTC):

    def test_global(self):
        """ Global test """
        # Building a project by adding sources, receptors, engines, buildings
        project_xml = "Final_project.xml"

        # Create a new project with sources located on positions from Spirale_log.csv:
        main([""], ["Spirale_log.csv"], ["source"], ["MySource"], "", project_xml)

        # Update the project by adding several objects:
        main(["", "Machine.xml", "Building.xml"],
             ["Recepteur.csv",  "Machine.csv", "Building.csv"],
             ["receptor", "engine", "building"],
             ["MyReceptor", "MyEngine", "MyBuilding"],
             project_xml,
             project_xml)

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

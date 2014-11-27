import os.path as osp
import os
import tempfile
import ConfigParser

from utils import TympanTC, no_output, TEST_DATA_DIR, TEST_SOLVERS_DIR
from tympan.models.business import Project
import tympan.solve_project as tysolve

class TestSolveProject(TympanTC):

    def setUp(self):
        self.output_proj = None
        self.output_mesh = None

    def tearDown(self):
        if self.output_proj and osp.exists(self.output_proj):
            self.remove_file(output_proj)
        if self.output_mesh and osp.exists(self.output_mesh):
            self.remove_file(output_mesh)

    def remove_file(self, f):
        f.close()
        os.unlink(f.name)

    def run_solve(self, input_project):
        output_proj = tempfile.NamedTemporaryFile(suffix='.xml', delete=False)
        output_mesh = tempfile.NamedTemporaryFile(suffix='.ply', delete=False)
        with no_output():
            tysolve.solve(input_project, output_proj, output_mesh.name, TEST_SOLVERS_DIR)

    def test_solver_project_no_sources(self):
        input_proj = osp.join(
            TEST_DATA_DIR, 'projects-panel',
            '11_PROJET_GRAND_SITE_VIDE_AVEC_SOUS_SITE_AU_CENTRE.xml')
        with self.assertRaises(RuntimeError) as cm:
            self.run_solve(input_proj)
        self.assertIn(
            'You must have at least one source to run a simulation.',
            str(cm.exception))

    def test_solver_config_errors(self):
        input_proj = osp.join(TEST_DATA_DIR, 'empty_site_config_ko.xml')
        with self.assertRaises(ConfigParser.Error) as cm:
            self.run_solve(input_proj)
        self.assertEqual(str(cm.exception),
                         os.linesep.join(
                             ["""bad option value for AtmosPressure: \'"ooops"\'""",
                              """bad option value for Discretization: \'2.5\'"""]))

    def test_solve_project_without_receptor_and_source(self):
        # This site has no source and no receptor
        input_proj = osp.join(TEST_DATA_DIR, 'empty_site.xml')
        with self.assertRaises(RuntimeError) as cm:
            self.run_solve(input_proj)
        self.assertEqual(str(cm.exception),
                         os.linesep.join(
                             ['You must have at least one source to run a simulation.',
                              'You must have at least one receptor to run a simulation.']))

    def test_solve_project_without_source(self):
        # This site has no source
        input_proj = osp.join(TEST_DATA_DIR, 'empty_site_receptor.xml')
        with self.assertRaises(RuntimeError) as cm:
            self.run_solve(input_proj)
        self.assertEqual(str(cm.exception),
                         "You must have at least one source to run a simulation.")

    def test_solve_project_without_receptor(self):
        # This site has no receptor
        input_proj = osp.join(TEST_DATA_DIR, 'empty_site_source.xml')
        with self.assertRaises(RuntimeError) as cm:
            self.run_solve(input_proj)
        self.assertEqual(str(cm.exception),
                         "You must have at least one receptor to run a simulation.")



if __name__ == '__main__':
    import unittest
    unittest.main()

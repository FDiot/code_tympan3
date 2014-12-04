import os.path as osp
import os
import subprocess
import sys
import tempfile
import ConfigParser

from utils import (TympanTC, no_output, TEST_DATA_DIR, TEST_SOLVERS_DIR,
                   PROJECT_BASE)
from tympan.models._business import Project
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

    def build_tempfiles(self):
        output_proj = tempfile.NamedTemporaryFile(suffix='.xml', delete=False)
        output_mesh = tempfile.NamedTemporaryFile(suffix='.ply', delete=False)
        return output_proj, output_mesh

    def run_solve(self, input_project):
        output_proj, output_mesh = self.build_tempfiles()
        with no_output():
            tysolve.solve(input_project, output_proj.name, output_mesh.name,
                          TEST_SOLVERS_DIR)

    def run_solve_subprocess(self, input_project):
        output_proj, output_mesh = self.build_tempfiles()
        script = osp.join(PROJECT_BASE, 'bin', 'solve_tympan_project.py')
        args = [sys.executable, script, input_project, output_proj.name,
                output_mesh.name, TEST_SOLVERS_DIR]
        with tempfile.TemporaryFile() as stdout:
            with tempfile.TemporaryFile() as stderr:
                proc = subprocess.Popen(args, stdout=stdout, stderr=stderr)
                r = proc.wait()
        return r

    def test_solver_project_no_sources(self):
        input_proj = osp.join(
            TEST_DATA_DIR, 'projects-panel',
            '11_PROJET_GRAND_SITE_VIDE_AVEC_SOUS_SITE_AU_CENTRE.xml')
        with self.assertRaises(RuntimeError) as cm:
            self.run_solve(input_proj)
        self.assertIn(
            'You must have at least one source to run a simulation.',
            str(cm.exception))

    def test_solver_project_subprocess(self):
        input_proj = osp.join(TEST_DATA_DIR, 'projects-panel',
                              'TEST_SOURCE_PONCTUELLE_NO_RESU.xml')
        self.assertFalse(self.run_solve_subprocess(input_proj))

    def test_solver_project_ok(self):
        input_proj = osp.join(TEST_DATA_DIR, 'projects-panel',
                              'TEST_SOURCE_PONCTUELLE_NO_RESU.xml')
        self.run_solve(input_proj)

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

import os.path as osp
import os
import subprocess
import sys
import tempfile
import unittest
import configparser

import numpy as np

from utils import TympanTC, TEST_DATA_DIR, TEST_SOLVERS_DIR, PROJECT_BASE
import tympan.solve_project as tysolve
from tympan.models.solver import Solver, Model


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
        with self.assertRaises(configparser.Error) as cm:
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
   
    def test_read_solve_older_noisemap_project(self):
        # Test reading an old noise map project
        input_proj = osp.join(
            TEST_DATA_DIR, 'projects-panel',
            'TEST_CARTO_ANCIEN_MODELE.xml')
        self.run_solve(input_proj)

class ProjectResultsTC(TympanTC):

    def test_combined_spectra(self):
        project = self.load_project(osp.join('projects-panel', 'TEST_CUBE_NO_RESU.xml'))
        model = Model.from_project(project, set_sources=False)
        model.add_source((-20, -30, 2), np.array([100.0] * 31, dtype=float))
        model.add_source((10, 50, 2), np.array([150.0] * 31, dtype=float))
        solver = Solver.from_project(project, solverdir=TEST_SOLVERS_DIR)
        result = solver.solve(model)
        combined_spectra = result.combined_spectra()
        expected_spectra = np.array([2.0602e-03, 1.9806e-03, 1.8942e-03, 1.8005e-03, 1.6987e-03,
                                     1.5867e-03, 1.4610e-03, 1.3166e-03, 1.1486e-03, 9.5532e-04,
                                     7.4293e-04, 5.2950e-04, 3.4102e-04, 1.9865e-04, 1.0676e-04,
                                     5.4570e-05, 2.7016e-05, 1.2721e-05, 5.3064e-06, 1.7143e-06,
                                     3.4595e-07, 3.1354e-08, 7.6832e-10, 2.3192e-12, 2.5400e-16,
                                     1.6487e-22, 1.3216e-23, 1.3216e-23, 1.3216e-23, 1.3216e-23,
                                     1.3216e-23])
        for rec in range(result.nreceptors):
            np.testing.assert_almost_equal(combined_spectra[rec, :], expected_spectra, decimal=6)


if __name__ == '__main__':
    import unittest
    unittest.main()

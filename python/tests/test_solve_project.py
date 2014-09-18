import os.path as osp
import os
import tempfile
import ConfigParser

from utils import TympanTC, no_output, TEST_DATA_DIR, TEST_SOLVERS_DIR
from tympan.models.business import Project
import tympan.solve_project as tysolve

class TestSolveProject(TympanTC):

    def remove_file(self, f):
        f.close()
        os.unlink(f.name)

    def run_solve(self, input_project):
        output_proj = tempfile.NamedTemporaryFile(suffix='.xml', delete=False)
        output_mesh = tempfile.NamedTemporaryFile(suffix='.ply', delete=False)
        try:
            with no_output():
                tysolve.solve(input_project, output_proj, output_mesh.name, TEST_SOLVERS_DIR)
        except:
            self.remove_file(output_proj)
            self.remove_file(output_mesh)
            raise
        self.remove_file(output_proj)
        self.remove_file(output_mesh)

    def test_solver_config_errors(self):
        input_proj = osp.join(TEST_DATA_DIR, 'empty_site_config_ko.xml')
        with self.assertRaises(ConfigParser.Error) as cm:
            self.run_solve(input_proj)
        self.assertEqual(str(cm.exception),
                         """bad option value for AtmosPressure: \'"ooops"\'\nbad option value for Discretization: \'2.5\'""")


if __name__ == '__main__':
    from utils import main
    main()

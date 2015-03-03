import os
import tempfile

from utils import TympanTC
from tympan.models.project import Project
from tympan.models.solver import _set_solver_config
from tympan.models import _solver as cysolver

class TestSolverConfig(TympanTC):

    def test_parameters_serialization(self):
        config = (
            '''
            [tutu]
                titi=15549.15
                toto=hi there!
                tata=0
            ''')
        # Open a basic project that doesn't have solver parameters
        project = self.load_project('', 'empty_site.xml')
        # Set a custom solver configuration to it
        project.current_computation.solver_parameters = config
        # Export to XML and reimport it
        with tempfile.NamedTemporaryFile(suffix='.xml', delete=False) as f:
            project.to_xml(f.name)
            configured_project = Project.from_xml(f.name)
            # Check configuration didn't disappear
            self.assertEqual(configured_project.current_computation.solver_parameters,
                             config)
        f.close()
        os.unlink(f.name)


    def test_set_solver_config(self):
        # Open a basic project with a custom solver configuration
        project = self.load_project('', 'test_solver_params.xml')
        _set_solver_config(project.current_computation)
        solver_config = cysolver.Configuration.get()
        # 10. by default, 20. in the XML file
        self.assertEqual(solver_config.H1parameter, 20.)


if __name__ == '__main__':
    import unittest
    unittest.main()

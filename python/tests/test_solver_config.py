import os
import tempfile

from utils import TympanTC, no_output
from tympan.models.business import Project

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
        (project, _) = self.load_project('', 'empty_site.xml')
        # Set a custom solver configuration to it
        project.current_computation.solver_parameters = config
        # Export to XML and reimport it
        with tempfile.NamedTemporaryFile(suffix='.xml', delete=False) as f:
            project.to_xml(f.name)
            with no_output():
                configured_project = Project.from_xml(f.name)
            # Check configuration didn't disappear
            self.assertEqual(configured_project.current_computation.solver_parameters,
                             config)
        f.close()
        os.unlink(f.name)


if __name__ == '__main__':
    import unittest
    unittest.main()

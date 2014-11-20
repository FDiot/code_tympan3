import sys
import os, os.path as osp
import unittest

class TestAltimetry(unittest.TestCase):
    def test_environment(self):
        # Import shapely (must be in the PYTHONPATH)
        try:
            import shapely
        except ImportError:
            self.fail('Shapely must be installed and must be in your PYTHONPATH')
        # import CGAL bindings (CGAL_BINDINGS_PATH environment variable must be defined)
        CGAL_BINDINGS_PATH = os.environ.get('CGAL_BINDINGS_PATH')
        self.assertTrue(CGAL_BINDINGS_PATH is not None)
        self.assertTrue(osp.isdir(CGAL_BINDINGS_PATH))
        sys.path.append(CGAL_BINDINGS_PATH)
        try:
            from CGAL import CGAL_Kernel, CGAL_Mesh_2
        except ImportError as exc:
             self.fail(
                'Could not load CGAL bindings : note the '
                'CGAL_BINDINGS_PATH environment variable must hold '
                'the path to CGAL_bindings python libraries in case they '
                'are not already in the PYTHONPATH  (' + CGAL_BINDINGS_PATH + 
                ') the error was: ' + exc.message)

if __name__ == '__main__':
    unittest.main()

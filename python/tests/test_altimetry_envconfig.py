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
            import CGAL
            from CGAL import CGAL_Kernel, CGAL_Mesh_2
        except ImportError:
            self.fail('''CGAL_BINDINGS_PATH environment variable must contain
                      the path to CGAL_bindings python libraries''')

if __name__ == '__main__':
    from utils import main
    main()

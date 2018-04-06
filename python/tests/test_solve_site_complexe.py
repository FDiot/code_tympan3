import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveSITE_COMPLEXE(TympanTC):
    @unittest.expectedFailure # Regression compared to Code_TYMPAN 3.10
    def test_site_complexe(self):
        _test_solve_with_file('TEST_SITE_COMPLEXE_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
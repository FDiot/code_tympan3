import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveRECEPTEURS_SOURCE(TympanTC):
    def test_recepteurs_source(self):
        _test_solve_with_file('TEST_RECEPTEURS_SOURCE_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveSANS_EFFET_SOL_SOURCE(TympanTC):
    def test_sans_effet_sol_source(self):
        _test_solve_with_file('TEST_SANS_EFFET_SOL_SOURCE_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
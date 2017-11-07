import unittest

from utils import TympanTC, _test_solve_with_file

class TestSolveEffetSol(TympanTC):
    def test_effet_sol(self):
        _test_solve_with_file('TEST_EFFET_SOL_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()

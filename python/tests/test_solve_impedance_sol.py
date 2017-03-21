import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveImpedanceSol(TympanTC):
    @unittest.expectedFailure
    def test_impedance_sol(self):
        _test_solve_with_file('TEST_IMPEDANCE_SOL_NO_RESU.xml', self)

if __name__ == '__main__':
    unittest.main()

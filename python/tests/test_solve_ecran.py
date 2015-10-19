import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveEcran(TympanTC):
    @unittest.expectedFailure
    def test_ecran(self):
        _test_solve_with_file('TEST_ECRAN_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()

import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveEcranCube(TympanTC):
    def test_ecran_cube(self):
        _test_solve_with_file('TEST_ECRAN_CUBE_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()

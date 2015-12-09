import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveBoiteCarto(TympanTC):
    def test_boite_carto(self):
        _test_solve_with_file('TEST_BOITE_CARTO_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()

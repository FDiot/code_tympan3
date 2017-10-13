import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveDiffractionButteCarto(TympanTC):
    def test_diffraction_butte_carto(self):
        _test_solve_with_file('TEST_DIFFRACTION_BUTTE_CARTO_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()

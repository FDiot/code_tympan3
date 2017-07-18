import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveDiffractionConstructionCarto(TympanTC):
    def test_diffraction_construction_carto(self):
        _test_solve_with_file('TEST_DIFFRACTION_CONSTRUCTION_CARTO_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()

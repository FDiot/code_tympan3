import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveRECEPTEURS_MACHINE_DANS_BATIMENT_CUBIQUE(TympanTC):
    def test_recepteurs_machine_dans_batiment_cubique(self):
        _test_solve_with_file('TEST_RECEPTEURS_MACHINE_DANS_BATIMENT_CUBIQUE_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
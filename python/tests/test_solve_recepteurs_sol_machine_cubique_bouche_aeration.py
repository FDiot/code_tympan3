import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveRECEPTEURS_SOL_MACHINE_CUBIQUE_BOUCHE_AERATION(TympanTC):
    def test_recepteurs_sol_machine_cubique_bouche_aeration(self):
        _test_solve_with_file('TEST_RECEPTEURS_SOL_MACHINE_CUBIQUE_BOUCHE_AERATION_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
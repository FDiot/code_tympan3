import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveRECEPTEURS_SOL_SOURCE_DANS_BATIMENT_CUBIQUE_AVEC_ECRAN(TympanTC):
    @unittest.expectedFailure # Regression compared to Code_TYMPAN 3.10
    def test_recepteurs_sol_source_dans_batiment_cubique_avec_ecran(self):
        _test_solve_with_file('TEST_RECEPTEURS_SOL_SOURCE_DANS_BATIMENT_CUBIQUE_AVEC_ECRAN_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
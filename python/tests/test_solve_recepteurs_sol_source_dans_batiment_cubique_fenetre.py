import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveRECEPTEURS_SOL_SOURCE_DANS_BATIMENT_CUBIQUE_FENETRE(TympanTC):
    @unittest.expectedFailure # Regression compared to Code_TYMPAN 3.10
    def test_recepteurs_sol_source_dans_batiment_cubique_fenetre(self):
        _test_solve_with_file('TEST_RECEPTEURS_SOL_SOURCE_DANS_BATIMENT_CUBIQUE_FENETRE_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveRECEPTEURS_SOL_MACHINE_DANS_BATIMENT_CYLINDRIQUE(TympanTC):
    @unittest.expectedFailure # Regression compared to Code_TYMPAN 3.10
    def test_recepteurs_sol_machine_dans_batiment_cylindrique(self):
        _test_solve_with_file('TEST_RECEPTEURS_SOL_MACHINE_DANS_BATIMENT_CYLINDRIQUE_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
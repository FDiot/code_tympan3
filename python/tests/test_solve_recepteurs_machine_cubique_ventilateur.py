import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveRECEPTEURS_MACHINE_CUBIQUE_VENTILATEUR(TympanTC):
    def test_recepteurs_machine_cubique_ventilateur(self):
        _test_solve_with_file('TEST_RECEPTEURS_MACHINE_CUBIQUE_VENTILATEUR_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveRECEPTEURS_MACHINE_CYLINDRIQUE(TympanTC):
    def test_recepteurs_machine_cylindrique(self):
        _test_solve_with_file('TEST_RECEPTEURS_MACHINE_CYLINDRIQUE_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
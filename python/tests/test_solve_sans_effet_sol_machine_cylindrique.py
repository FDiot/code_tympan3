import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveSANS_EFFET_SOL_MACHINE_CYLINDRIQUE(TympanTC):
    def test_sans_effet_sol_machine_cylindrique(self):
        _test_solve_with_file('TEST_SANS_EFFET_SOL_MACHINE_CYLINDRIQUE_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
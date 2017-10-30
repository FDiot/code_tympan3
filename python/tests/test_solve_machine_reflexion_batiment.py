import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveMACHINE_REFLEXION_BATIMENT(TympanTC):
    def test_machine_reflexion_batiment(self):
        _test_solve_with_file('TEST_MACHINE_REFLEXION_BATIMENT_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
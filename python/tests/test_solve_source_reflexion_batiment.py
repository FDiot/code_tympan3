import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveSOURCE_REFLEXION_BATIMENT(TympanTC):
    @unittest.expectedFailure # Regression compared to Code_TYMPAN 3.10
    def test_source_reflexion_batiment(self):
        _test_solve_with_file('TEST_SOURCE_REFLEXION_BATIMENT_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()
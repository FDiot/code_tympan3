import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveRayonnementBatiment(TympanTC):
    @unittest.expectedFailure
    def test_rayonnement_batiment(self):
        _test_solve_with_file('TEST_RAYONNEMENT_BATIMENT_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()

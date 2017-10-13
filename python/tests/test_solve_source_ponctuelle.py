import unittest

from utils import TympanTC, _test_solve_with_file


class TestSolveSourcePonctuelle(TympanTC):
    def test_source_ponctuelle(self):
        _test_solve_with_file('TEST_SOURCE_PONCTUELLE_NO_RESU.xml', self)


if __name__ == '__main__':
    unittest.main()

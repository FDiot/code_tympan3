import unittest

from utils import TympanTC, _test_solve_with_file

class TestSolveFace(TympanTC):
    @unittest.expectedFailure
    def test_face(self):
        _test_solve_with_file('TEST_FACE_NO_RESU.xml', self)

if __name__ == '__main__':
    unittest.main()

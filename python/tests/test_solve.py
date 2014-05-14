import os, os.path as osp
import unittest
import pytam

_HERE = osp.realpath(osp.dirname(__file__))

_PROJECT_BASE = osp.abspath(osp.join(_HERE, '..', '..'))

_TEST_DATA_DIR = osp.join(_PROJECT_BASE, 'tests', 'data')
assert osp.isdir(_TEST_DATA_DIR), "The test data dir does not exists '%s'" % _TEST_DATA_DIR

_TEST_PROBLEM_DIR = osp.join(_TEST_DATA_DIR, 'projects-panel')
assert osp.isdir(_TEST_PROBLEM_DIR), "The test problem dir does not exists '%s'" % _TEST_PROBLEM_DIR

_TEST_RESULT_DIR = osp.join(_TEST_DATA_DIR, 'expected')
assert osp.isdir(_TEST_RESULT_DIR), "The test result dir does not exists '%s'" % _TEST_RESULT_DIR

_TEST_SOLVERS_DIR = osp.join(_PROJECT_BASE, 'pluginsd')
if not osp.isdir(_TEST_SOLVERS_DIR):
    solver_dir = osp.abspath(osp.join(_HERE, '..', '..', 'plugins'))
assert osp.isdir(_TEST_SOLVERS_DIR), "The test solver plugins dir does not exists '%s'" % _TEST_SOLVERS_DIR


class TestTympan(unittest.TestCase):
    pass


def make_test_with_file(test_file):
    """ For a TEST_xx_NO_RESU.xml file from data/project-panel, load and
    solve the corresponding project, load the expected result
    from data/expected/TEST_xx.xml and compare the computed result with
    the expected result
    """
    def test_with_file(self):
        # Load and solve the project
        project = pytam.Project.from_xml(osp.join(_TEST_PROBLEM_DIR, test_file))
        computation = project.current_computation()
        pytam.loadsolver(_TEST_SOLVERS_DIR, computation)
        self.assertTrue(computation.go())
        # Load the expected result
        result_file = osp.join(_TEST_RESULT_DIR, test_file).replace('_NO_RESU', '')
        expected_result_project = pytam.Project.from_xml(result_file)
        # Compare results
        self.assertTrue(computation.same_result(expected_result_project.current_computation()))
    return test_with_file

# Retrieve all the available "TEST_XX" xml files and make a test for each one
for test_file in os.listdir(_TEST_PROBLEM_DIR):
    if test_file.startswith('TEST_') and test_file.endswith('xml'):
        setattr(TestTympan, "test_" + test_file.split('.')[0].replace('TEST_', '').lower(),
                make_test_with_file(test_file))

if __name__ == '__main__':
    unittest.main()

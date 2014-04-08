import pytam
import unittest

class TestTympan(unittest.TestCase):
    def test_solve(self):
        project = pytam.Project.from_xml("data/projects/10_PROJET_SITE_emprise_non_convexe_avec_butte_et_terrains.xml")
        computation = project.current_computation()
        pytam.loadsolver("data/solver", computation)
        self.assertTrue(computation.go())

    def test_hierarchy(self):
        project = pytam.Project.from_xml("data/projects/10_PROJET_SITE_emprise_non_convexe_avec_butte_et_terrains.xml")
        site = project.site()
        childs = site.childs()
        for c in childs:
            self.assertRegexpMatches(c.name(),'Infrastructure|Topographie')

    def test_base(self):
        # XXX This test uses expected bad values provided by the current
        # implementation
        project = pytam.Project.from_xml("data/projects/base.xml")
        model = project.current_computation().problem()
        builder = pytam.SolverModelBuilder(model)
        builder.fill_problem(project.site())
        self.assertEqual(model.nbpoints(), 6) # OK
        self.assertEqual(model.nbtriangles(), 5) # XXX should be 4
        self.assertEqual(model.nbmaterials(), 5) # XXX should be 1
        # FIXME the default material is replicated once per triangle.
        # TODO : how to test the altitude of a point ? or access a triangle at
        # some place ?
        # TODO to be completed: cf. ticket #1468184


    def test_triangles(self):
        project = pytam.Project.from_xml("data/projects/tiny_site.xml")
        model = project.current_computation().problem()
        builder = pytam.SolverModelBuilder(model)
        builder.fill_problem(project.site())
        model.export_triangles("export/export_check_triangles")

    @unittest.skip("Implementation to be fixed")
    def test_ground_materials(self):
        project = pytam.Project.from_xml("data/projects/ground_materials.xml")
        model = project.current_computation().problem()
        builder = pytam.SolverModelBuilder(model)
        builder.fill_problem(project.site())
        self.assertEqual(model.nbmaterials(), 3)
        # XXX FIXME: the default material is replicated once per triangle
        # TODO to be completed: cf. ticket #1468184 

if __name__ == '__main__':
    unittest.main()

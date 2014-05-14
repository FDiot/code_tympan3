import pytam
import sys

def solve_acoustic_problem(input_project, output_project, solverdir):
    """ Solve an acoustic problem with Code_TYMPAN from

        Keywords arguments:
        input_project -- XML file containing the serialized project with the
            "calcul" to solve
        output_project -- XML file where to put the project updated with the
            results of the computation
        solvedir -- directory containing the solver plugin
    """
    ret = False
    # Load an existing project and retrieve its calcul to solve it
    project = pytam.Project.from_xml(input_project)
    comp = project.current_computation()
    # Build an acoustic problem from the site of the computation
    problem = comp.acoustic_problem()
    builder = pytam.SolverModelBuilder(problem)
    builder.fill_problem(project.site())

    # Load solver plugin
    pytam.loadsolver(solverdir, comp)
    # Solve the problem and fill the acoustic result
    ret = comp.go()
    if ret is True:
        # Reserialize project
        project.to_xml(output_project)
    return ret;

if __name__ == '__main__':
    # read command-line arguments
    if len(sys.argv) == 4:
        input_proj = sys.argv[1]
        output_proj = sys.argv[2]
        solverdir = sys.argv[3]
        # solve problem
        solve_acoustic_problem(input_project=input_proj,
                                      output_project=output_proj,
                                      solverdir=solverdir)
    else:
        solve_acoustic_problem("pourcython.xml", "out.xml", "Tympan/pluginsd")

import sys
import logging
import os.path as osp

# open file in unbuffered mode so it get written asap, in case of later crash
# due to underlying C code
stream = open('tympan.log', 'a', 0)
logging.basicConfig(stream=stream, level=logging.DEBUG,
                    format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')


try:
    import pytam
except ImportError:
    err = "tympan.py module couldn't find pytam library."
    logging.critical("%s Check PYTHONPATH and path to Tympan libraries.", err)
    raise ImportError(err)

def solve_acoustic_problem(input_project, output_project, solverdir):
    """ Solve an acoustic problem with Code_TYMPAN from

        Keywords arguments:
        input_project -- XML file containing the serialized project with the
            "calcul" to solve
        output_project -- XML file where to put the project updated with the
            results of the computation
        solvedir -- directory containing the solver plugin

        The execution is logged into 'tympan.log', created in the directory of
        the input XML project (the one opened from the Code_TYMPAN GUI)
    """
    ret = False
    pytam.init_tympan_registry()
    # Load an existing project and retrieve its calcul to solve it
    try:
        project = pytam.Project.from_xml(input_project)
    except RuntimeError:
        logging.exception("Couldn't load the acoustic project from %s file", input_project)
        raise
    comp = project.current_computation
    # Build an acoustic problem from the site of the computation
    problem = comp.acoustic_problem
    builder = pytam.SolverModelBuilder(problem)
    # Update site before building the acoustic problem
    site = project.site
    site.update()
    project.update_altimetry_on_receptors()
    builder.fill_problem(site, comp)
    # Load solver plugin
    pytam.loadsolver(solverdir, comp)
    # Solve the problem and fill the acoustic result
    logging.debug("Calling C++ go method")
    ret = comp.go()
    if ret is False:
        err = "Computation failed (C++ go method returned false)"
        logging.error(err)
        raise RuntimeError(err)
    # Reserialize project
    try:
        project.to_xml(output_project)
    except ValueError:
        logging.exception("Couldn't export the acoustic results to %s file", output_project)
        raise

if __name__ == '__main__':
    if len(sys.argv) != 4:
        err = "tympan.py module called with bad arguments"
        logging.error("%s Couldn't solve acoustic problem.", err)
        sys.exit(-1) # XXX to be improved
    # read command-line arguments
    input_proj = sys.argv[1]
    output_proj = sys.argv[2]
    solverdir = sys.argv[3]
    # solve problem
    try:
        solve_acoustic_problem(input_project=input_proj,
                               output_project=output_proj,
                               solverdir=solverdir)
    except:
        logging.exception("tympan.py module couldn't solve the acoustic problem")
        sys.exit(-1)
    sys.exit(0)

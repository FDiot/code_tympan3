import os
import logging

logging.basicConfig(filename='tympan.log', level=logging.DEBUG,
                    format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')

from tympan.models.project import Project
from tympan.models.solver import Model, Solver


def solve(input_project, output_project, output_mesh, solverdir,
          multithreading_on=True, interactive=False, verbose=False):
    """ Solve an acoustic problem with Code_TYMPAN from

        Keywords arguments:
        input_project -- XML file containing the serialized project with the
            "calcul" to solve
        output_project -- XML file where to put the project updated with the
            results of the computation
        output_mesh -- a file in which to put the altimetry mesh once computed (ply format)
        solverdir -- directory containing the solver plugin
        -------
        optional (debug):
        multithreading_on -- set it to False to solve the acoustic problem with only
            one thread
        interactive -- if True, pdb debugger will be invoked before running solving
            the acoustic problem, so that the program can be executed in interactive
            mode.

        The execution is logged into 'tympan.log', created in the directory of
        the input XML project (the one opened from the Code_TYMPAN GUI)
    """
    if interactive:
        import pdb
        pdb.set_trace()

    # Load an existing project and retrieve its calcul to solve it
    try:
        logging.info("Trying to load project ...")
        project = Project.from_xml(input_project, verbose=verbose)
    except RuntimeError:
        logging.exception("Couldn't load the acoustic project from %s file", input_project)
        raise
    logging.info("Project loaded !")
    # Export altimetry
    project.export_altimetry(output_mesh)
    # Solver model
    model = Model.from_project(project)
    logging.info("Solver model built.\nNumber of sources: %d\nNumber of receptors: %d",
                 model.nsources, model.nreceptors)
    # Load solver plugin and run it on the current computation
    logging.info("Trying to load solver ...")
    solver = Solver.from_project(project, solverdir, verbose)
    if not multithreading_on:
        solver.nthread = 1
    logging.info("Checking solver model ...")
    _check_solver_model(model, project.site)
    logging.debug("Calling C++ SolverInterface::solve() method")
    try:
        solver_result = solver.solve(model)
    except RuntimeError as exc:
        logging.error(str(exc))
        logging.info("It doesn't work", str(exc))
        raise
    logging.info("Solver computation done !")
    # Export solver results to the business model
    logging.info("Loading results from solver ...")
    project.import_result(model, solver_result)
    # Reserialize project
    try:
        logging.info("Trying to export result project to xml ...")
        project.to_xml(output_project)
    except ValueError:
        logging.exception("Couldn't export the acoustic results to %s file", output_project)
        raise

def _check_solver_model(model, site):
    """Various checks for a solver model, to be performed before computation.

    Raises a RuntimeError in case of incomplete modelling.
    """
    errors = []
    if model.nsources == 0:
        errors.append(
            'You must have at least one source to run a simulation.')
        for id_, name in site.outdated_elements:
            errors.append('Update failed on element %s (id %s)' % (name, id_))
    if model.nreceptors == 0:
        errors.append(
            'You must have at least one receptor to run a simulation.')
    if errors:
        raise RuntimeError(os.linesep.join(errors))

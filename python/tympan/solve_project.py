import sys
import os
import io
import logging
import ConfigParser
from StringIO import StringIO

# open file in unbuffered mode so it get written asap, in case of later crash
# due to underlying C code
stream = open('tympan.log', 'a', 0)
logging.basicConfig(stream=stream, level=logging.DEBUG,
                    format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')


try:
    import tympan.models.business as tybusiness
    import tympan.models.solver as tysolver
except ImportError:
    err = "solve_project.py module couldn't find tympan.models.business cython library."
    logging.critical("%s Check PYTHONPATH and path to Tympan libraries.", err)
    raise ImportError(err)

try:
    import tympan.business2solver as bus2solv
except ImportError:
    err = "solve_project.py module couldn't find tympan.business2solver cython library."
    logging.critical("%s Check PYTHONPATH and path to Tympan libraries.", err)
    raise ImportError(err)

from tympan import SOLVER_CONFIG_ATTRIBUTES
from tympan.altimetry import export_to_ply, builder
from tympan.models.solver import Configuration

CONVERTERS = {
    'bool': bool,
    'int': int,
    'float': float,
    'double': float,
}

CONFIG_MAP = dict((optname, CONVERTERS[opttype]) for opttype, optname in SOLVER_CONFIG_ATTRIBUTES)


def solve(input_project, output_project, output_mesh, solverdir,
          multithreading_on=True,
          interactive=False):
    """ Solve an acoustic problem with Code_TYMPAN from

        Keywords arguments:
        input_project -- XML file containing the serialized project with the
            "calcul" to solve
        output_project -- XML file where to put the project updated with the
            results of the computation
        output_mesh -- a file in which to put the altimetry mesh once computed (ply format)
        solvedir -- directory containing the solver plugin
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
    ret = False
    # Load an existing project and retrieve its calcul to solve it
    try:
        project = tybusiness.Project.from_xml(input_project)
    except RuntimeError:
        logging.exception("Couldn't load the acoustic project from %s file", input_project)
        raise
    # Business model
    site = project.site
    comp = project.current_computation
    # Setup solver configuration
    parser = ConfigParser.RawConfigParser()
    parser.optionxform = str # keep param names case
    parser.readfp(StringIO(comp.solver_parameters))
    solver_config = Configuration.get()
    errors = []
    for section in parser.sections():
        for optname, value in parser.items(section):#solver_config.items(section):
            try:
                value = CONFIG_MAP[optname](value)
            except ValueError:
                errors.append('bad option value for %s: %r' % (optname, value))
                continue
            getattr(solver_config, optname, value)
    if errors:
        raise ConfigParser.Error(os.linesep.join(errors))
    if not multithreading_on:
        solver_config.NbThreads = 1
    # Recompute altimetry
    asite = builder.build_sitenode(project.site)
    _, mesh, feature_by_face = builder.build_altimetry(asite)
    material_by_face = builder.material_by_face(feature_by_face)
    export_to_ply(mesh, material_by_face, output_mesh)
    # Update site and the project before building the solver model
    project.update_site_altimetry(mesh, material_by_face)
    # Solver model
    solver_problem = project.build_model()
    logging.info("Solver model built.\nNumber of sources: %d\nNumber of receptors: %d",
                 solver_problem.nsources, solver_problem.nreceptors)
    _check_solver_model(solver_problem, site)
    # Load solver plugin and run it on the current computation
    solver = bus2solv.load_computation_solver(solverdir, comp)
    logging.debug("Calling C++ SolverInterface::solve() method")
    solver_result = tysolver.ResultModel()
    ret = solver.solve_problem(solver_problem, solver_result)
    if ret is False:
        err = "Computation failed (C++ SolverInterface::solve() method returned false)"
        logging.error(err)
        raise RuntimeError(err)
    # Export solver results to the business model
    bus2solv_conv = Business2SolverConverter(comp, site)
    bus2solv_conv.postprocessing(model, solver_result)
    # Reserialize project
    try:
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

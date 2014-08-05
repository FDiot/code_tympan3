import sys
import logging

# open file in unbuffered mode so it get written asap, in case of later crash
# due to underlying C code
stream = open('tympan.log', 'a', 0)
logging.basicConfig(stream=stream, level=logging.DEBUG,
                    format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')


try:
    import tympan.models.business as tybusiness
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

from tympan.altimetry.builder import Builder
from tympan.altimetry import process_altimetry

def solve(input_project, output_project, solverdir, multithreading_on=True,
          interactive=False):
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
    if interactive:
        import pdb
        pdb.set_trace()
    ret = False
    tybusiness.init_tympan_registry()
    # Load an existing project and retrieve its calcul to solve it
    try:
        project = tybusiness.Project.from_xml(input_project)
    except RuntimeError:
        logging.exception("Couldn't load the acoustic project from %s file", input_project)
        raise
    # Business model
    site = project.site
    comp = project.current_computation
    if not multithreading_on:
        comp.set_nthread(1)
    # Solver model
    solver_problem = comp.acoustic_problem
    solver_result = comp.acoustic_result
    # Recompute altimetry
    # Rebuild topography with altimetry data model
    alti_site = process_altimetry.export_site_topo(site)
    # Compute altimetry and retrieve the resulting mesh
    builder =  Builder(alti_site)
    builder.complete_processing()
    vertices, faces, materials, faces_materials = builder.build_mesh_data()
    # Update site and the project before building the solver model
    site.update_altimetry(vertices, faces, materials, faces_materials)
    project.update()
    # Build an acoustic problem from the site of the computation
    bus2solv_conv = bus2solv.Business2SolverConverter(comp, project.site)
    bus2solv_conv.build_solver_problem()
    # Load solver plugin
    solver = bus2solv.load_computation_solver(solverdir, comp)
    # Solve the problem and fill the acoustic result
    logging.debug("Calling C++ go method")
    ret = comp.go(solver)
    if ret is False:
        err = "Computation failed (C++ go method returned false)"
        logging.error(err)
        raise RuntimeError(err)
    # Export solver results to the business model
    bus2solv_conv.postprocessing()
    # Reserialize project
    try:
        project.to_xml(output_project)
    except ValueError:
        logging.exception("Couldn't export the acoustic results to %s file", output_project)
        raise

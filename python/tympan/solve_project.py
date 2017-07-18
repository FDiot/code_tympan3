import os
import logging

logging.basicConfig(filename='tympan.log', level=logging.DEBUG,
                    format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')

from tympan.models.project import Project
from tympan.models.solver import Model, Solver


def solve(input_project, output_project, output_mesh, solverdir, parameters = {},
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
        optional :
        parameters -- Dictionary containing the solver parameters the user wants to change from their
        default value in the form {'parameter_name':value,...}.
        The possible parameters are: atmos_pressure, atmos_temperature, atmos_hygrometry, wind_direction,
        analytic_gradC, analytic_gradV, ray_tracing_order, discretization, nb_rays_per_source, max_length,
        size_receiver, accelerator, max_tree_depth, angle_diff_min, cylindre_thick, max_profondeur, use_sol,
        max_reflexion, max_diffraction, diffraction_use_random_sampler, nb_ray_with_diffraction, diffraction_drop_down_nb_rays,
        diffraction_filter_ray_at_creation, use_path_dif_validation, max_path_difference, diffraction_use_distance_as_filter,
        keep_debug_ray, use_post_filters, curve_ray_sampler, initial_angle_theta, final_angle_theta, initial_angle_phi, final_angle_phi,
        analytic_nb_ray, analytic_tMax, analyticH, analytic_dMax, analytic_type_transfo, mesh_element_size_max, show_scene, minSR_distance,
        nb_threads, use_real_ground, use_screen, use_lateral_diffraction, use_reflection, propa_conditions, h1parameter, mod_summation, use_meteo,
        use_fresnel_area, anime3D_sigma, anime3D_forceC, anime3D_keep_rays, debug_use_close_event_selector, debug_use_diffraction_angle_selector,
        debug_use_diffraction_path_selector, debug_use_fermat_selector, debug_use_face_selector
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
    # Setting the parameters chosen by the user
    for parameter in parameters:
        setattr(solver,parameter,parameters[parameter]) 
    if not multithreading_on:
        solver.nb_threads = 1
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

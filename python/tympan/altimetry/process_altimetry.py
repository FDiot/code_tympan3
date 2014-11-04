import logging

# open file in unbuffered mode so it get written asap, in case of later crash
# due to underlying C code
stream = open('tympan.log', 'a', 0)
logging.basicConfig(stream=stream, level=logging.DEBUG,
                    format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')

from tympan import Simulation
from tympan.altimetry import export_to_ply



def process_site_altimetry(input_project, result_file):
    """ Build an altimetry from the topography of the site described in the
        'input_project' XML file

        Keywords argument:
            input_project -- XML file containing the serialized project
                containing the site whose altimetry must be processed.
            result_file -- ply file that will be filled with the altimetry mesh

        The execution is logged into 'tympan.log', created in the directory of
        the input XML project (the one opened from the Code_TYMPAN GUI)
    """
    try:
        sml = Simulation.from_xml(input_project)
    except RuntimeError:
        logging.exception("Couldn't load the acoustic project from %s file", input_project)
        raise
    _, mesh, material_by_face = sml.altimetry(allow_features_outside_mainsite=True)
    export_to_ply(mesh, material_by_face, result_file)

def export_site_topo(*args, **kwargs):
    from tympan._simulation import altimetry_site_from_topography
    from warnings import warn
    warn('Use Simulation API', DeprecationWarning)
    return altimetry_site_from_topography(*args, **kwargs)

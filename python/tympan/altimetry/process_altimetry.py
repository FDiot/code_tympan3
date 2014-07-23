import sys
import logging

# open file in unbuffered mode so it get written asap, in case of later crash
# due to underlying C code
stream = open('tympan.log', 'a', 0)
logging.basicConfig(stream=stream, level=logging.DEBUG,
                    format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')

import tympan.models.business as tybusiness
import tympan.altimetry.datamodel as altimetry


def process_site_altimetry(input_project):
    """ Build an altimetry from the topography of the site described in the
        'input_project' XML file

        Keywords argument:
            input_project -- XML file containing the serialized project
                containing the site whose altimetry must be processed.


        The execution is logged into 'tympan.log', created in the directory of
        the input XML project (the one opened from the Code_TYMPAN GUI)
    """
    tybusiness.init_tympan_registry()
    # Load an existing project and retrieve its calcul to solve it
    try:
        project = tybusiness.Project.from_xml(input_project)
    except RuntimeError:
        logging.exception("Couldn't load the acoustic project from %s file", input_project)
        raise
    # Business model
    cysite = project.site
    asite = export_site_topo(cysite)
    return asite


def export_site_topo(cysite):
    """ Extract the topography of tympan site and build an altimetry from it

        Keyword argument:
            cy_site -- cython site to process in order to build an altimetry site
    """
    # build current SiteNode
    (points, cylcurve) = cysite.process_landtake()
    asite = altimetry.SiteNode(coords=cypoints2acoords(points), id=cysite.elem_id)
    if cylcurve is not None:
        alcurve = altimetry.LevelCurve(
            coords=cypoints2acoords(cylcurve.points),
            altitude=cylcurve.altitude,
            id=cylcurve.elem_id)
        asite.add_child(alcurve)
    return asite


def cypoints2acoords(points):
    """ Take a list of tympan.models.common.Point3D objects and return a list
        of 2D coords in the format [(x1,y1),(x2,y2)]
    """
    coords = []
    for pt in points:
        coords.append((pt.x, pt.y))
    return coords



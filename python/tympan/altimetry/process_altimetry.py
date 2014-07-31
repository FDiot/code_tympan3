import logging

# open file in unbuffered mode so it get written asap, in case of later crash
# due to underlying C code
stream = open('tympan.log', 'a', 0)
logging.basicConfig(stream=stream, level=logging.DEBUG,
                    format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')

import tympan.models.business as tybusiness
from tympan.altimetry.datamodel import (SiteNode, LevelCurve, WaterBody,
                                        GroundMaterial, MaterialArea,
                                        InfrastructureLandtake)


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
    tybusiness.init_tympan_registry()
    # Load an existing project
    try:
        project = tybusiness.Project.from_xml(input_project)
    except RuntimeError:
        logging.exception("Couldn't load the acoustic project from %s file", input_project)
        raise
    # Business model
    cysite = project.site
    asite = export_site_topo(cysite)
    # XXX TODO: build altimetry from this site and fill result file with the
    # resulting mesh (ply format)
    return asite


def export_site_topo(cysite):
    """ Extract the topography of tympan site and build an altimetry from it

        Keyword argument:
            cy_site -- cython site to process in order to build an altimetry site
    """
    # Site landtake
    (points, cylcurve) = cysite.process_landtake()
    asite = SiteNode(coords=cypoints2acoords(points), id=cysite.elem_id)
    if cylcurve is not None:
        alcurve = LevelCurve(
            coords=cypoints2acoords(cylcurve.points),
            altitude=cylcurve.altitude,
            id=cylcurve.elem_id)
        asite.add_child(alcurve)
    # Water bodies
    for cylake in cysite.lakes:
        allake = WaterBody(
            coords=cypoints2acoords(cylake.level_curve.points),
            altitude=cylake.level_curve.altitude,
            id=cylake.elem_id)
        asite.add_child(allake)
    # Other material areas
    for cymarea in cysite.material_areas:
        # Build a ground material
        cymaterial = cymarea.ground_material
        almaterial = GroundMaterial(cymaterial.elem_id)
        # Build a material area made of the above defined ground material
        almatarea = MaterialArea(
            coords=cypoints2acoords(cymarea.points),
            material=almaterial,
            id=cymarea.elem_id)
        asite.add_child(almatarea)
    # Level curves
    for cylcurve in cysite.level_curves:
        alcurve = LevelCurve(
            coords=cypoints2acoords(cylcurve.points),
            altitude=cylcurve.altitude,
            id=cylcurve.elem_id)
        asite.add_child(alcurve)
    # Ground contour (infrastructure landtake)
    for (cy_id, cy_volume_contour) in cysite.ground_contour.items():
        infra_landtake = InfrastructureLandtake(
            coords=cypoints2acoords(cy_volume_contour),
            id=cy_id
            )
        asite.add_child(infra_landtake)
    # Recurse
    cysubsites = cysite.subsites
    for cysbsite in cysubsites:
        asbsite = export_site_topo(cysbsite)
        asite.add_child(asbsite)
    return asite


def cypoints2acoords(points):
    """ Take a list of tympan.models.common.Point3D objects and return a list
        of 2D coords in the format [(x1,y1),(x2,y2)]
    """
    coords = []
    for pt in points:
        coords.append((pt.x, pt.y))
    return coords



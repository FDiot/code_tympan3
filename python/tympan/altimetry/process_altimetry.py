import logging
import sys
from warnings import warn

# open file in unbuffered mode so it get written asap, in case of later crash
# due to underlying C code
stream = open('tympan.log', 'a', 0)
logging.basicConfig(stream=stream, level=logging.DEBUG,
                    format='%(levelname)s:%(asctime)s - %(name)s - %(message)s')

import tympan.models.business as tybusiness
from tympan.altimetry import datamodel
from tympan.altimetry.datamodel import (SiteNode, LevelCurve, WaterBody,
                                        GroundMaterial, MaterialArea,
                                        InfrastructureLandtake, SiteLandtake)
from tympan.altimetry.builder import Builder


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
    asite = export_site_topo(cysite, mainsite=True)
    # Build altimetry
    builder = Builder(asite, allow_features_outside_mainsite=False)
    builder.complete_processing()
    builder.export_to_ply(result_file)
    return asite


def export_site_topo(cysite, mainsite=False):
    """ Extract the topography of tympan site `cysite`

    If `mainsite` is True the site is assumed to be the parent possibly
    containing sub-sites.
    """
    # Site landtake
    (points, cylcurve) = cysite.process_landtake()
    asite = SiteNode(coords=cypoints2acoords(points), id=cysite.elem_id)
    if cylcurve is not None:
        lctype = SiteLandtake if mainsite else LevelCurve
        if cylcurve.points[0] != cylcurve.points[-1]:
            warn('main site landtake (or surrounding level curve) does not '
                 'appear to be closed; closing it for altimetry processing',
                 RuntimeWarning)
            close_it = True
        else:
            close_it = False
        alcurve = lctype(
            coords=cypoints2acoords(cylcurve.points),
            altitude=cylcurve.altitude,
            close_it=close_it,
            id=cylcurve.elem_id)
        asite.add_child(alcurve)
    # Water bodies
    water_material = None
    for cylake in cysite.lakes:
        # Build water material
        cywater = cylake.ground_material
        alwater = GroundMaterial(cywater.elem_id)
        if not water_material:
            water_material = cywater.elem_id
            datamodel.MATERIAL_WATER = alwater
        allake = WaterBody(
            coords=cypoints2acoords(cylake.level_curve.points),
            altitude=cylake.level_curve.altitude,
            id=cylake.elem_id)
        asite.add_child(allake)
    # Other material areas
    default_material = None
    for cymarea in cysite.material_areas:
        # Build a ground material
        cymaterial = cymarea.ground_material
        almaterial = GroundMaterial(cymaterial.elem_id)
        coords = cypoints2acoords(cymarea.points)
        # Build a material area made of the above defined ground material
        coords = cypoints2acoords(cymarea.points)
        if not coords:
            assert not default_material, "Found several default materials"
            default_material = cymaterial.elem_id
            datamodel.DEFAULT_MATERIAL = almaterial
            continue
        almatarea = MaterialArea(
            coords=coords,
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



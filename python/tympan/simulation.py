"""Simulation for Code_TYMPAN."""
from warnings import warn

from tympan.models.business import Project
from tympan.altimetry import datamodel
from tympan.altimetry.datamodel import (SiteNode, LevelCurve, WaterBody,
                                        GroundMaterial, MaterialArea,
                                        VegetationArea,
                                        InfrastructureLandtake, SiteLandtake)
from tympan.altimetry.builder import build_altimetry


def points_to_coords(points):
    """Return a list of coordinates from given `points` objects (Point3D)"""
    return [(p.x, p.y) for p in points]


def ground_material_from_business(material):
    """Return a GroundMaterial from a business model material"""
    return GroundMaterial(material.elem_id, material.resistivity)


def build_material_area(ty_materialarea, altimetry_groundmaterial):
    """Build a MaterialArea in altimetry data model from a Tympan material
    area and an altimetry GroundMaterial.

    This may be a plain MaterialArea or a VegetationArea.
    """
    kwargs = {}
    if ty_materialarea.has_vegetation():
        kwargs['height'] = ty_materialarea.vegetation.height
        cls = VegetationArea
    else:
        cls = MaterialArea
    return cls(coords=points_to_coords(ty_materialarea.points),
               material=altimetry_groundmaterial,
               id=ty_materialarea.elem_id, **kwargs)


def altimetry_site_from_topography(ty_site, mainsite=False):
    """Extract the topography of tympan `ty_site` and build another site
    following altimetry data model.

    If `mainsite` is True the site is assumed to be the parent possibly
    containing sub-sites.
    """
    # Site landtake
    (points, cylcurve) = ty_site.process_landtake()
    altimetry_site = SiteNode(coords=points_to_coords(points),
                              id=ty_site.elem_id)
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
            coords=points_to_coords(cylcurve.points),
            altitude=cylcurve.altitude,
            close_it=close_it,
            id=cylcurve.elem_id)
        altimetry_site.add_child(alcurve)
    # Water bodies
    water_material = None
    for cylake in ty_site.lakes:
        # Build water material
        cywater = cylake.ground_material
        alwater = ground_material_from_business(cywater)
        if not water_material:
            water_material = cywater.elem_id
            datamodel.MATERIAL_WATER = alwater
        allake = WaterBody(
            coords=points_to_coords(cylake.level_curve.points),
            altitude=cylake.level_curve.altitude,
            id=cylake.elem_id)
        altimetry_site.add_child(allake)
    # Other material areas
    default_material = None
    for cymarea in ty_site.material_areas:
        # Build a ground material
        cymaterial = cymarea.ground_material
        almaterial = ground_material_from_business(cymaterial)
        # Build a material area made of the above defined ground material
        if not cymarea.points:
            assert not default_material, "Found several default materials"
            default_material = cymaterial.elem_id
            datamodel.DEFAULT_MATERIAL = almaterial
            continue
        almatarea = build_material_area(cymarea, almaterial)
        altimetry_site.add_child(almatarea)
    # Level curves
    for cylcurve in ty_site.level_curves:
        if len(cylcurve.points) == 1:
            warn('Level curve %s is made of a single point: ignoring it.' %
                 cylcurve.elem_id, RuntimeWarning)
            continue
        alcurve = LevelCurve(
            coords=points_to_coords(cylcurve.points),
            altitude=cylcurve.altitude,
            id=cylcurve.elem_id)
        altimetry_site.add_child(alcurve)
    # Ground contour (infrastructure landtake)
    for (cy_id, cy_volume_contour) in ty_site.ground_contour.items():
        infra_landtake = InfrastructureLandtake(
            coords=points_to_coords(cy_volume_contour),
            id=cy_id
            )
        altimetry_site.add_child(infra_landtake)
    # Recurse
    cysubsites = ty_site.subsites
    for cysbsite in cysubsites:
        asbsite = altimetry_site_from_topography(cysbsite)
        altimetry_site.add_child(asbsite)
    return altimetry_site


class Simulation(object):
    """Modelling and simulation with Code_TYMPAN.

    The main entry point is the `from_xml` method which loads a Tympan project
    from a XML file.
    """

    @classmethod
    def from_xml(cls, fpath):
        """Build a Simulation by loading a project from a XML file."""
        project = Project.from_xml(fpath)
        return cls(project)

    def __init__(self, project):
        self._project = project

    def altimetry(self, **kwargs):
        """Return the altimetry site, mesh and associated material to mesh
        faces mapping.

        The altimetry site is the result from first converting Tympan site
        topography into an altimetry site (using an internal data model) and
        then cleaning the latter by merging features.
        """
        altimetry_site = self._build_altimetry_site()
        return build_altimetry(altimetry_site, **kwargs)

    def _build_altimetry_site(self):
        """Build a Site model from altimetry data model using a TYSite"""
        return altimetry_site_from_topography(self._project.site,
                                              mainsite=True)

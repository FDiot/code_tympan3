"""
Provide chaining of the step required to build the altimetry of a compound site.
"""

from itertools import chain
from warnings import warn
import numpy as np
from shapely import geometry

from .datamodel import (InconsistentGeometricModel, elementary_shapes,
                        SiteNode, LevelCurve, WaterBody, GroundMaterial,
                        MaterialArea, VegetationArea, InfrastructureLandtake,
                        SiteLandtake)
from . import datamodel
from .merge import recursively_merge_all_subsites
from .mesh import (ElevationMesh, ReferenceElevationMesh,
                   LandtakeFaceFlooder)


# Altimetry side building utilities.

def points_to_coords(points):
    """Return a list of coordinates from given `points` objects (Point3D)"""
    return [(p.x, p.y) for p in points]


def ground_material_from_business(material):
    """Return a GroundMaterial from a business model material"""
    return GroundMaterial(material.elem_id, material.resistivity)


def build_material_area(ty_materialarea, altimetry_groundmaterial, landtake_points_for_subsites=[]):
    """Build a MaterialArea in altimetry data model from a Tympan material
    area, an altimetry GroundMaterial and a set of points representing the landtake if it is a subsite.

    This may be a plain MaterialArea, a VegetationArea or a subsite.
    """
    kwargs = {}
    if ty_materialarea.has_vegetation():
        kwargs['height'] = ty_materialarea.vegetation.height
        kwargs['variety'] = ty_materialarea.vegetation.name()
        kwargs['foliage'] = ty_materialarea.vegetation.foliage
        cls = VegetationArea
    else:
        cls = MaterialArea
    if ty_materialarea.points:
        return cls(coords=points_to_coords(ty_materialarea.points),
                   material=altimetry_groundmaterial,
                   id=ty_materialarea.elem_id, **kwargs)
    else:
        return cls(coords=points_to_coords(landtake_points_for_subsites),
                   material=altimetry_groundmaterial,
                   id=ty_materialarea.elem_id, **kwargs)


def build_sitenode(ty_site, mainsite=True):
    """Build an altimetry SiteNode from a Tympan topography site `ty_site`.

    If `mainsite` is True the site is assumed to be the parent possibly
    containing sub-sites.
    """
    # Site landtake
    (points, cylcurve) = ty_site.process_landtake()
    altimetry_site = SiteNode(coords=points_to_coords(points),
                              id=ty_site.elem_id)
    if cylcurve is not None:
        close_it = False
        if cylcurve.points[0] != cylcurve.points[-1]:
            warn('main site landtake (or surrounding level curve) does not '
                 'appear to be closed; closing it for altimetry processing',
                 RuntimeWarning)
            close_it = True
        lctype = SiteLandtake if mainsite else LevelCurve
        alcurve = lctype(
            coords=points_to_coords(cylcurve.points),
            altitude=cylcurve.altitude,
            close_it=close_it,
            id=cylcurve.elem_id)
        altimetry_site.add_child(alcurve)

    # Water bodies
    add_lake(ty_site, altimetry_site)

    # Other material areas
    add_material(ty_site, altimetry_site, points, mainsite)

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
    for id_, volume_contours in ty_site.ground_contour.items():
        contours_coords = map(points_to_coords, volume_contours)
        altimetry_site.add_child(
            InfrastructureLandtake(*contours_coords, id=id_))
    # Recurse
    cysubsites = ty_site.subsites
    for cysbsite in cysubsites:
        asbsite = build_sitenode(cysbsite, mainsite=False)
        altimetry_site.add_child(asbsite)
    return altimetry_site


def add_lake(ty_site, altimetry_site):
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
            coords=points_to_coords(cylake.points),
            altitude=cylake.altitude,
            id=cylake.elem_id)
        altimetry_site.add_child(allake)


def add_material(ty_site, altimetry_site, material_border_points, is_mainsite):
    # Material
    default_material = None
    for cymarea in ty_site.material_areas:
        # Build a ground material
        cymaterial = cymarea.ground_material
        almaterial = ground_material_from_business(cymaterial)
        # Build a material area made of the above defined ground material
        if not cymarea.points:
            if is_mainsite:
                assert not default_material, "Found several default materials"
                default_material = cymaterial.elem_id
                datamodel.DEFAULT_MATERIAL = almaterial
                continue
            else:
                almatarea = build_material_area(cymarea, almaterial, material_border_points)
        else:
            almatarea = build_material_area(cymarea, almaterial)
        altimetry_site.add_child(almatarea)


# Altimetry mesh building utilities.
def build_altimetry(mainsite, allow_features_outside_mainsite=True,
                    size_criterion=0.0):
    """Return the results of altimetry building from a site tree model."""
    cleaner = recursively_merge_all_subsites(
        mainsite, allow_outside=allow_features_outside_mainsite)
    merged_site = cleaner.merged_site()
    builder = MeshBuilder(merged_site, size_criterion=size_criterion)
    mesh = builder.build_mesh()
    filler = MeshFiller(mesh, builder.vertices_for_feature)
    feature_by_face = filler.fill_material_and_landtakes(merged_site, cleaner)
    builder.join_with_landtakes(mesh)
    return merged_site, mesh, feature_by_face


def material_by_face(feature_by_face):
    """Return a material_by_face mapping given a feature_by_face mapping"""
    m2f = {}
    for fh, feature in feature_by_face.items():
        material = feature.material if feature else datamodel.DEFAULT_MATERIAL
        m2f[fh] = material
    return m2f


class MeshBuilder(object):
    """Build an elevation mesh from a site cleaner.

    Parameters
    ----------
    site: SiteNode
        The site to build altimetry mesh for.
    size_criterion: float, optional
        Size criterion for mesh refinement, default to 0.0 (meaning no size
        criterion) (see tympan.altimetry.mesh.Mesh.refine_mesh for details).
    shape_criterion: float, optional
        Shape criterion for mesh refinement, default to 0.125 (see
        tympan.altimetry.mesh.Mesh.refine_mesh for details).
    """

    def __init__(self, site, size_criterion=0.0, shape_criterion=0.125):
        self._site = site
        self.size_criterion = size_criterion
        self.shape_criterion = shape_criterion
        self.vertices_for_feature = {}

    def build_mesh(self, refine=True):
        """Build the mesh and fill the feature to vertices mapping."""
        # First get an ElevationMesh (possibly with vertices missing altitude,
        # in order to be able to add non-altimetric features).
        alti = self._build_altimetric_base()
        mesh = self._build_triangulation(alti)
        if refine:
            # Refine the mesh.
            # TODO (optional) flood landtake in order to mark them as not to be refined
            mesh.refine_mesh(size_criterion=self.size_criterion,
                             shape_criterion=self.shape_criterion)
        self._compute_informations(mesh)
        self._compute_elevations(mesh, alti)
        return mesh

    @staticmethod
    def _compute_informations(mesh):
        mesh.update_info_for_vertices()
        mesh.update_info_for_edges()

    @staticmethod
    def _compute_elevations(mesh, altimesh):
        # From this point, all vertices in the mesh have an altitude.
        mesh.update_altitude_from_reference(altimesh.point_altitude)

    def _insert_feature(self, feature, mesher, **properties):
        """Insert a `feature` into a mesh."""
        try:
            shape = self._site.features_by_id[feature.id].shape
        except KeyError:
            # The element was filtered out (e.g. it was outside of its sub-site)
            return None
        vertices_groups = []
        for polyline in elementary_shapes(shape):
            if isinstance(polyline, geometry.LineString):
                points = polyline.coords[:]
            elif isinstance(polyline, geometry.Polygon):
                if list(polyline.interiors):
                    raise ValueError("Polygons with holes are not (yet) supported")
                points = polyline.exterior.coords[:]
                # NB: polygons' coordinates sequences are automatically closed
            elif isinstance(polyline, geometry.Point):
                warn('Found an isolated point in the altimetry features', RuntimeWarning)
                continue
            else:
                raise TypeError("Only level curves or waterbodies are expected, "
                                "not %s found in %s" % (polyline, feature))
            vertices, _ = mesher.insert_polyline(
                points, id=feature.id, **properties)
            vertices_groups.append(vertices)
        return vertices_groups

    def _build_altimetric_base(self):
        """Return an elevation mesh along with a feature to vertices mapping.

        The mesh is built by walking the equivalent site for level curves and
        roads *only*.
        """
        alti = ReferenceElevationMesh() # Altimetric base
        for polyline in chain(self._site.level_curves, self._site.roads):
            props = polyline.build_properties()
            assert 'altitude' in props  or 'altitudes' in props
            vertices = self._insert_feature(polyline, alti, **props)
            self.vertices_for_feature[polyline.id] = vertices
        alti.update_info_for_vertices()
        return alti

    def _copy_mesh(self, altimesh):
        """Return a copy of reference altimetry mesh and update
        vertices_for_feature map.
        """
        vmap = {}
        mesh = altimesh.copy(class_=ElevationMesh, vmap=vmap)
        for fid, vertices_groups in list(self.vertices_for_feature.items()):
            self.vertices_for_feature[fid] = [[vmap[vh] for vh in vertices]
                                              for vertices in vertices_groups]
        return mesh

    def _build_triangulation(self, altimesh):
        """Return an elevation mesh, copied from the base mesh with non
        altimetric features inserted.
        """
        mesh = self._copy_mesh(altimesh)
        for feature in self._site.non_altimetric_features:
            vertices = self._insert_feature(feature, mesh,
                                            **feature.build_properties())
            self.vertices_for_feature[feature.id] = vertices
        return mesh

    def join_with_landtakes(self, mesh):
        """Join the altimetry to the landtakes.

        For each infrastructure land-take, compute the mean altitude of the
        vertices of the corresponding input constraint (e.g. 4 points defining
        the contour): this altitude is about to become THE altitude of the
        whole piece of infrastructure.

        Then update the altitude of all vertices (those of the meshed contour
        and those of inside faces) to the mean altitude: this is akin to an
        earth work and will ensure the machine or building is flat on the
        ground.

        NB: This has the restriction that building in a strong slopes
        are not well supported: they produce artifact in the altimetry.
        """
        for landtake in self._site.landtakes:
            for polyline in self.vertices_for_feature[landtake.id]:
                mean_alt = np.mean([mesh.vertices_info[vh].altitude
                                    for vh in polyline])
                close_it = polyline[0] != polyline[-1]
                contour_vertices = mesh.iter_vertices_for_input_polyline(
                    polyline, close_it=close_it)
                flooder = mesh.flood_polygon(LandtakeFaceFlooder, polyline,
                                             close_it=close_it)
                inside_vertices = set((fh.vertex(i) for fh in flooder.visited
                                       for i in range(3)))
                for vh in chain(contour_vertices, inside_vertices):
                    mesh.vertices_info[vh].altitude = mean_alt


class MeshFiller(object):
    """Fill a mesh with material information"""

    def __init__(self, mesh, vertices_for_feature):
        self._mesh = mesh
        self._vertices_for_feature = vertices_for_feature

    def fill_material_and_landtakes(self, mainsite, cleaner):
        """Build the face to geometrical feature mapping."""
        feature_by_face = {}
        self.fill_lantakes(mainsite, feature_by_face)
        self.fill_material(mainsite, feature_by_face, cleaner)
        return feature_by_face

    def fill_lantakes(self, mainsite, feature_by_face):
        # Fill landtakes using the flooding algorithm.
        for feature in mainsite.landtakes:
            _check_feature_inserted(feature, mainsite)
            for faces in self._faces_for_polygonal_feature(
                    feature, flooder_class=LandtakeFaceFlooder):
                for fh in faces:
                    if fh not in feature_by_face:
                        feature_by_face[fh] = feature

    def fill_material(self, mainsite, feature_by_face, cleaner):
        # Fill material areas by finding the underlying feature based on the
        # position of face "middle point".
        features = _material_area_features(mainsite, cleaner)
        for fh in self._mesh.cdt.finite_faces():
            if fh in feature_by_face:
                # Already inserted (landtake).
                continue
            point = self._mesh.point_for_face(fh)
            assert point is not None, \
                    'could not find a point inside face handle {}'.format(fh)
            point = geometry.Point((point.x(), point.y()))
            for feature in features:
                if feature.shape.contains(point):
                    feature_by_face[fh] = feature
                    break
            else:
                feature_by_face[fh] = None

    def _faces_for_polygonal_feature(self, feature, flooder_class):
        """Return the list of faces within a polygonal feature"""
        for vertices in self._vertices_for_feature[feature.id]:
            close_it = vertices[0] != vertices[-1]
            flooder = self._mesh.flood_polygon(flooder_class, vertices,
                                               close_it=close_it)
            yield flooder.visited


def _check_feature_inserted(feature, site):
    if feature.id not in datamodel.SiteNode.recursive_features_ids(site):
        raise ValueError("Only features already inserted can be filled ID:%s"
                         % feature.id)


def _material_area_features(mainsite, cleaner):
    """Return the list geometric features corresponding to material areas
    found in `cleaner`.
    """
    checked = set()
    features = []
    for material_area_id in cleaner.material_areas_inner_first():
        feature = cleaner.equivalent_site.features_by_id[material_area_id]
        if feature not in checked:
            _check_feature_inserted(feature, mainsite)
            checked.add(feature)
        features.append(feature)
    return features

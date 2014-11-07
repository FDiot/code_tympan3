"""
Provide chaining of the step required to build the altimetry of a compound site.
"""

from itertools import chain
from warnings import warn
import numpy as np
from shapely import geometry

from .datamodel import (InconsistentGeometricModel,
                        elementary_shapes)
from . import datamodel
from .merge import recursively_merge_all_subsites
from .mesh import (ElevationMesh, ReferenceElevationMesh,
                   LandtakeFaceFlooder, MaterialFaceFlooder)


def build_altimetry(mainsite, allow_features_outside_mainsite=True):
    """Return the results of altimetry building from a site tree model."""
    cleaner = recursively_merge_all_subsites(
        mainsite, allow_outside=allow_features_outside_mainsite)
    merged_site = cleaner.merged_site()
    builder = MeshBuilder(merged_site)
    mesh = builder.build_mesh()
    filler = MeshFiller(mesh, builder.vertices_for_feature)
    feature_by_face = filler.fill_material_and_landtakes(mainsite, cleaner)
    builder.join_with_landtakes(mesh)
    return merged_site, mesh, feature_by_face


def material_by_face(feature_by_face):
    """Return a material_by_face mapping given a feature_by_face mapping"""
    m2f = {}
    for fh, feature in feature_by_face.iteritems():
        material = feature.material if feature else datamodel.DEFAULT_MATERIAL
        m2f[fh] = material
    return m2f


class MeshBuilder(object):
    """Build an elevation mesh from a site cleaner."""

    def __init__(self, site):
        self._site = site
        self.size_criterion = 0.0 # zero means no size criterion
        self.shape_criterion = 0.125
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
        vertices = []
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
        return vertices

    def _build_altimetric_base(self):
        """Return an elevation mesh along with a feature to vertices mapping.

        The mesh is built by walking the equivalent site for level curves *only*.
        """
        alti = ReferenceElevationMesh() # Altimetric base
        for level_curve in self._site.level_curves:
            props = level_curve.build_properties()
            assert 'altitude' in props
            vertices = self._insert_feature(level_curve, alti, **props)
            self.vertices_for_feature[level_curve.id] = vertices
        alti.update_info_for_vertices()
        return alti

    def _copy_mesh(self, altimesh):
        """Return a copy of reference altimetry mesh and update
        vertices_for_feature map.
        """
        vmap = {}
        mesh = altimesh.copy(class_=ElevationMesh, vmap=vmap)
        for fid, vertices in self.vertices_for_feature.items():
            self.vertices_for_feature[fid] = [vmap[vh] for vh in vertices]
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
            polyline = self.vertices_for_feature[landtake.id]
            mean_alt = np.mean([mesh.vertices_info[vh].altitude
                                for vh in polyline])
            close_it = polyline[0] != polyline[-1]
            contour_vertices = mesh.iter_vertices_for_input_polyline(
                polyline, close_it=close_it)
            flooder = mesh.flood_polygon(LandtakeFaceFlooder, polyline,
                                         close_it=close_it)
            inside_vertices = set((fh.vertex(i) for fh in flooder.visited
                                   for i in xrange(3)))
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

        def fill_feature(feature, floodercls):
            """Update feature_by_face with feature's faces using specified
            flooder.
            """
            self._check_feature_inserted(feature, mainsite)
            faces = self._faces_for_polygonal_feature(
                feature, flooder_class=floodercls)
            for fh in faces:
                if fh not in feature_by_face:
                    feature_by_face[fh] = feature

        for landtake in mainsite.landtakes:
            fill_feature(landtake, LandtakeFaceFlooder)
        for material_area_id in cleaner.material_areas_inner_first():
            material_area = cleaner.equivalent_site.features_by_id[material_area_id]
            fill_feature(material_area, MaterialFaceFlooder)
        for fh in self._mesh.cdt.finite_faces():
            if fh not in feature_by_face:
                feature_by_face[fh] = None
        return feature_by_face

    @staticmethod
    def _check_feature_inserted(feature, site):
        if feature.id not in datamodel.SiteNode.recursive_features_ids(site):
            raise ValueError("Only features already inserted can be filled ID:%s"
                             % feature.id)

    def _faces_for_polygonal_feature(self, feature, flooder_class):
        """Return the list of faces within a polygonal feature"""
        vertices = self._vertices_for_feature[feature.id]
        close_it = vertices[0] != vertices[-1]
        flooder = self._mesh.flood_polygon(flooder_class, vertices,
                                           close_it=close_it)
        return flooder.visited

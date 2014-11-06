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
                   Vertex_handle,
                   LandtakeFaceFlooder, MaterialFaceFlooder)


def build_altimetry(mainsite, allow_features_outside_mainsite=True):
    """Return the results of altimetry building from a site tree model."""
    builder = Builder(mainsite)
    builder.cleaned = recursively_merge_all_subsites(
        mainsite, allow_outside=allow_features_outside_mainsite)
    # First get an ElevationMesh (possibly with vertices missing altitude,
    # in order to be able to add non-altimetric features).
    builder.build_altimetric_base()
    builder.build_triangulation()
    builder.refine_triangulation()
    builder.compute_informations()
    # From this point, all vertices in the mesh have an altitude.
    builder.compute_elevations()
    builder.fill_material_and_landtakes()
    # Finally update the altitude of infrastructure landtakes by
    # averaging using their contour altitude. From this point, the mesh
    # altitudes is completely determined and no change are supposed to
    # occur.
    builder.join_with_landtakes()
    return builder.equivalent_site, builder.mesh, builder.material_by_face


class Builder(object):

    def __init__(self, mainsite):
        self.mainsite = mainsite
        self.cleaned = None # The cleaned and merged site
        self.alti = ReferenceElevationMesh() # Altimetric base
        self.mesh = None
        self._vertices_for_feature = {} # List of vertex handle for a given feature
        self.material_by_face = {}
        self.size_criterion = 0.0 # zero means no size criterion
        self.shape_criterion = 0.125

    @property
    def equivalent_site(self):
        return self.cleaned.equivalent_site

    def insert_feature(self, feature, mesher, **properties):
        try:
            shape = self.cleaned.geom[feature.id]
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

    def build_altimetric_base(self):
        assert self.cleaned is not None
        assert self.mesh is None
        for level_curve in self.equivalent_site.level_curves:
            props = level_curve.build_properties()
            assert 'altitude' in props
            vertices = self.insert_feature(level_curve, self.alti, **props)
            self._vertices_for_feature[level_curve.id] = vertices
        self.alti.update_info_for_vertices()
        self.mesh = self.alti.copy_as_ElevationMesh()

    def build_triangulation(self):
        assert self.mesh is not None
        for feature in self.equivalent_site.non_altimetric_features:
            vertices = self.insert_feature(feature, self.mesh,
                                           **feature.build_properties())
            self._vertices_for_feature[feature.id] = vertices

    def refine_triangulation(self):
        # TODO (optional) flood landtake in order to mark them as not to be refined
        self.mesh.refine_mesh(size_criterion=self.size_criterion,
                              shape_criterion=self.shape_criterion)

    def compute_informations(self):
        self.mesh.update_info_for_vertices()
        self.mesh.update_info_for_edges()

    def compute_elevations(self):
        self.mesh.update_altitude_from_reference(self.alti.point_altitude)

    def fill_polygonal_feature(self, feature, flooder_class):
        assert self.mesh is not None
        if feature.id not in datamodel.SiteNode.recursive_features_ids(self.mainsite):
            raise ValueError("Only features already inserted can be filled ID:%s"
                             % feature.id)
        vertices = self._vertices_for_feature[feature.id]
        close_it = vertices[0] != vertices[-1]
        flooder = self.mesh.flood_polygon(flooder_class, vertices,
                                          close_it=close_it)
        affected_faces =[fh for fh in flooder.visited
                         if fh not in self.material_by_face]
        for fh in affected_faces:
            self.material_by_face[fh] = feature.material
        return affected_faces

    def fill_material_and_landtakes(self):
        assert self.mesh is not None
        assert len(self.material_by_face)==0
        for landtake in self.mainsite.landtakes:
            self.fill_polygonal_feature(landtake,
                                        flooder_class=LandtakeFaceFlooder)
        for material_area_id in self.cleaned.material_areas_inner_first():
            material_area = self.equivalent_site.features_by_id[material_area_id]
            self.fill_polygonal_feature(material_area,
                                        flooder_class=MaterialFaceFlooder)
        for fh in self.mesh.cdt.finite_faces():
            if fh not in self.material_by_face:
                self.material_by_face[fh] = datamodel.DEFAULT_MATERIAL

    def join_with_landtakes(self):
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
        for landtake in self.equivalent_site.landtakes:
            polyline = self._vertices_for_feature[landtake.id]
            mean_alt = np.mean([self.mesh.vertices_info[vh].altitude
                                for vh in polyline])
            close_it = polyline[0] != polyline[-1]
            contour_vertices = self.mesh.iter_vertices_for_input_polyline(
                polyline, close_it=close_it)
            flooder = self.mesh.flood_polygon(LandtakeFaceFlooder, polyline,
                                              close_it=close_it)
            inside_vertices = set((fh.vertex(i) for fh in flooder.visited
                                   for i in xrange(3)))
            for vh in chain(contour_vertices, inside_vertices):
                self.mesh.vertices_info[vh].altitude = mean_alt


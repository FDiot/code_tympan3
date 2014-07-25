"""
Provide chaining of the step required to build the altimetry of a compound site.
"""

from shapely import geometry

from .datamodel import (InconsistentGeometricModel,
                        DEFAULT_MATERIAL, HIDDEN_MATERIAL,
                        elementary_shapes)
from .merge import recursively_merge_all_subsites
from .mesh import ElevationMesh, ReferenceElevationMesh,Vertex_handle

class Builder(object):

    def __init__(self, mainsite):
        self.mainsite = mainsite
        self.cleaned = None # The cleaned and merged site
        self.alti = ReferenceElevationMesh() # Altimetric base
        self.mesh = None
        self.vertices_for_feature = {} # List of vertex handle for a given feature

    @property
    def equivalent_site(self):
        return self.cleaned.equivalent_site

    def merge_subsites(self):
        assert self.cleaned is None
        self.cleaned = recursively_merge_all_subsites(self.mainsite)

    def insert_feature(self, feature, mesher=None, **properties):
        try:
            shape = self.cleaned.geom[feature.id]
        except KeyError:
            # The element was filtered out (e.g. it was outside of its sub-site)
            return None
        mesher = mesher or self.mesh
        for polyline in elementary_shapes(shape):
            if isinstance(polyline, geometry.LineString):
                points = polyline.coords[:]
            elif isinstance(polyline, geometry.Polygon):
                if list(polyline.interiors):
                    raise ValueError("Polygons with holes are not (yet) supported")
                points = polyline.exterior.coords[:]
                # NB: polygons' coordinates sequences are automatically closed
            else:
                raise TypeError("Only level curves or waterbodies are expected")
            vertices, _ = mesher.insert_polyline(
                points, id=feature.id, **properties)
        return vertices

    def build_altimetric_base(self):
        assert self.cleaned is not None
        assert self.mesh is None
        for level_curve in self.equivalent_site.level_curves:
            props = level_curve.build_properties()
            assert 'altitude' in props
            vertices = self.insert_feature(level_curve, mesher=self.alti, **props)
        self.alti.update_info_for_vertices()
        self.mesh = self.alti.copy_as_ElevationMesh()

    def build_triangulation(self):
        assert self.mesh is not None
        for feature in self.equivalent_site.non_altimetric_features:
            vertices = self.insert_feature(feature, **feature.build_properties())
            self.vertices_for_feature[feature.id] = vertices
        self.mesh.update_info_for_vertices()

    def compute_elevations(self):
        self.mesh.update_altitude_from_reference(self.alti.point_altitude)

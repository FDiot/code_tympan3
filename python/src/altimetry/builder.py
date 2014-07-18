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

    @property
    def equivalent_site(self):
        return self.cleaned.equivalent_site

    def merge_subsites(self):
        self.cleaned = recursively_merge_all_subsites(self.mainsite)

    def build_altimetric_base(self):
        assert self.cleaned is not None
        for level_curve in self.equivalent_site.level_curves:
            altitude = level_curve.altitude
            try:
                shape = self.cleaned.geom[level_curve.id]
            except KeyError:
                # The element was filtered out (water body out of the site)
                continue
            for polyline in elementary_shapes(shape):
                if isinstance(polyline, geometry.LineString):
                    points = polyline.coords[:]
                elif isinstance(polyline, geometry.Polygon): # Waterbodies
                    points = polyline.exterior.coords[:]
                else:
                    raise TypeError("Only level curves or waterbodies are expected")
                vertices = self.alti.insert_polyline(points, altitude=altitude,
                                                     id=level_curve.id)
        self.mesh = self.alti.copy_as_ElevationMesh()

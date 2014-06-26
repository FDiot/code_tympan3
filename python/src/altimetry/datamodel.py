"""
Data model for the computation of the altimetry.

The input classes implement the `geo_interface`_  protocol
which enable them to be used directly with Shapely_ and allow
to easily load test data from a GeoJSON_ file.

.. _geo_interface: https://gist.github.com/sgillies/2217756
.. _Shapely: http://toblerity.org/shapely/manual.html#python-geo-interface
.. _GeoJSON: http://geojson.org/geojson-spec.html
"""

import json

from shapely import geometry
from shapely.validation import explain_validity


GEOMETRY_TYPES = (
    "Point", "MultiPoint",
    "LineString", "MultiLineString",
    "Polygon", "MultiPolygon",
)


def _preproc_one_coord(coord):
    if len(coord) not in (2, 3):
        raise ValueError("Coordinates are expected to be seq of 2 or 3 numbers")
    return tuple(float(c) for c in coord)

def _preproc_point_seq(coordinates):
    return [_preproc_one_coord(c) for c in coordinates]



class InvalidGeometry(Exception):
    pass

class GroundMaterial(object):

    def __init__(self, id_):
        self.id = id_


MATERIAL_WATER = GroundMaterial("Water")


class GeometricFeature(object):
    geometric_type = None # To be overridden by derived classes

    def __init__(self, coords, id):
        self._coords = _preproc_point_seq(coords)
        self.id = id
        self._shape = None # to cache the Shapely shape

    def build_coordinates(self):
        raise NotImplementedError

    def build_geometry(self):
        if self.geometric_type not in GEOMETRY_TYPES:
            return None
        else:
            d = {"type": self.geometric_type,
                 "coordinates": self.build_coordinates()}
            return d

    def build_properties(self):
        return {}

    def build_feature(self):
        d = {"type": "Feature",
             "geometry" : self.build_geometry(),
             "properties": self.build_properties()}
        if self.id is not None:
            d["id"] = self.id
        return d

    def build_shape(self):
        return geometry.shape(self)

    @property
    def as_geojson(self):
        return json.dumps(self.build_feature(), sort_keys=True)

    def __repr__(self):
        return self.as_geojson

    @property
    def __geo_interface__(self):
        return self.build_geometry()

    @property
    def as_feature(self):
        return self.build_feature()

    @property
    def as_shape(self):
        return geometry.asShape(self)

    @property
    def shape(self):
        if self._shape is None:
            self._shape = self.build_shape()
        return self._shape

    def ensure_ok(self):
        shape = self.shape # CAUTION Cache the shape
        if shape.is_valid:
            return
        else :
            raise InvalidGeometry(explain_validity(shape))


class TympanFeature(GeometricFeature):

    def __init__(self, coords, parent_site=None, **kwargs):
        super(TympanFeature, self).__init__(coords, **kwargs)
        self.parent_site = parent_site
        if self.parent_site is not None:
            assert isinstance(self.parent_site, (SiteNode,))
            self.parent_site.add_child(self)

    @property
    def tympan_type(self):
        return type(self).__name__

    @property
    def parent_site_id(self):
        return  self.parent_site and self.parent_site.id

    def build_properties(self):
        p = super(TympanFeature, self).build_properties()
        assert "type" not in p
        p["type"] = self.tympan_type
        if self.parent_site:
            assert "site" not in p
            p["site"] = self.parent_site_id
        return p


class LevelCurve(TympanFeature):
    geometric_type = "LineString"

    def __init__(self, coords, altitude, **kwargs):
        try:
            self.altitude = float(altitude)
        except KeyError:
            raise ValueError('LevelCurve(...) expect an `altitude` named argument')
        super(LevelCurve, self).__init__(coords, **kwargs)

    def build_coordinates(self):
        return self._coords

    def build_properties(self):
        d = super(LevelCurve, self).build_properties()
        d.update(altitude=self.altitude)
        return d


class PolygonalTympanFeature(TympanFeature):
    geometric_type = "Polygon"

    def __init__(self, coords, **kwargs):
        self.holes = [_preproc_point_seq(hole) for hole in kwargs.pop("holes", [])]
        super(PolygonalTympanFeature, self).__init__(coords, **kwargs)

    def build_coordinates(self):
        return [self._coords] + self.holes

    def build_shape(self):
        shape = super(PolygonalTympanFeature, self).build_shape()
        if not shape.exterior.is_ccw:
            shape = geometry.polygon.orient(shape)
        return shape

    def ensure_ok(self):
        super(PolygonalTympanFeature, self).ensure_ok()
        if not self.shape.exterior.is_simple:
            raise InvalidGeometry("Polygon is expected to be simple")


class MaterialArea(PolygonalTympanFeature):

    def __init__(self, coords, material, **kwargs):
        try:
            self.material = material
        except KeyError:
            raise ValueError('Material(...) expect a `material` named argument')
        super(MaterialArea, self).__init__(coords, **kwargs)

    @property
    def material_id(self):
        return self.material.id

    def build_properties(self):
        d = super(MaterialArea, self).build_properties()
        d.update(material=self.material.id)
        return d



class WaterBody(MaterialArea, LevelCurve):
    geometric_type = "Polygon"

    def __init__(self, coords, **kwargs):
        super(WaterBody, self).__init__(coords, material=MATERIAL_WATER, **kwargs)


class SiteNode(PolygonalTympanFeature):

    CHILDREN_TYPES = ("LevelCurve", "MaterialArea", "WaterBody",
                      "InfrastructureLandtake", "SiteNode")

    def __init__(self, coords, **kwargs):
        super(SiteNode, self).__init__(coords, **kwargs)

        self.children = {}
        for k in self.CHILDREN_TYPES:
            self.children[k] = []

    def add_child(self, child):
        self.children[child.tympan_type].append(child)

    @property
    def level_curves(self):
        return self.children["LevelCurve"] + self.children["WaterBody"]

    @property
    def material_areas(self):
        return self.children["MaterialArea"] + self.children["WaterBody"]

    @property
    def subsites(self):
        return self.children["SiteNode"][:]

    @property
    def landtakes(self):
        return self.children["InfrastructureLandtake"][:]

    @property
    def all_features(self):
        # TODO Use itertools.chain
        return ( self.children["LevelCurve"] +
                 self.children["MaterialArea"] +
                 self.children["WaterBody"] +
                 self.children["InfrastructureLandtake"] )

class InfrastructureLandtake(PolygonalTympanFeature):

    def __init__(self, coords, **kwargs):
        super(InfrastructureLandtake, self).__init__(coords, **kwargs)

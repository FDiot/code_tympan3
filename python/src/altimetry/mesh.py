"""
Provide triangulation and meshing of a clean, single site, geometry.
"""

from collections import defaultdict

from shapely import geometry as sh_geom

from CGAL.CGAL_Kernel import (
    Point_2 as Point,
    Vector_2 as Vector,
    Triangle_2 as Triangle,
    centroid)
from CGAL.CGAL_Mesh_2 import (
    Mesh_2_Constrained_Delaunay_triangulation_plus_2 as CDT,
    Mesh_2_Constrained_Delaunay_triangulation_plus_2_Face_handle as Face_handle,
    Delaunay_mesh_plus_size_criteria_2 as Mesh_criteria,
    refine_Delaunay_mesh_2 as CGAL_refine_Delaunay_mesh)


from datamodel import *


UNSPECIFIED_ALTITUDE = float('nan')
UNSPECIFIED_MATERIAL = None

def to_cgal_point(pt):
    if isinstance(pt, Point):
        return pt
    elif isinstance(pt, sh_geom.Point):
        return Point(pt.x, pt.y)
    elif isinstance(pt, (tuple, list)):
        assert len(pt) == 2
        return Point(*pt)
    else:
        raise TypeError("Don't know how to make a CGAL Point_2 from", pt)


def _sorted_vertex_pair(v1, v2):
    if v1.__hash__() < v2.__hash__():
        return (v1, v2)
    else:
        return (v2, v1)


class MeshedCDTWithInfo(object):
    """
    This call provide the meshing of a geometry with arbitrary informations attached
    """

    def __init__(self):
        self.cdt = CDT()

    def insert_polyline(self, polyline, close_it=False, connected=True):
        """Insert a sequence of points as a polyline.

        If ``close_it=True`` is specified, then a segment joining the
        last point to the first one is added. No attempt is done at
        ensure this is a valid  (ccw oriented and simple) polygon.

        If ``connected=False`` is specified only the points are added,
        but not the segments connecting them.

        Returns the list of vertices handles and of constraints added.
        """
        assert not close_it or connected, \
            "It's meaningless to close an unconnected points sequence"
        points_it = iter(polyline)
        first_point = next(points_it)
        if not isinstance(first_point, Point):
            raise TypeError("`polyline` is expected to be an iterable over CGAL Point_2")
        vertices_handles = [self.cdt.insert(first_point)]
        constraints = []
        # Insert points, thus making CDT vertices and build the list of edges
        for point in points_it:
            vertex = self.cdt.insert(point)
            vertices_handles.append(vertex)
            if connected:
                # Constraint connecting the last vertex and the one just inserted
                # represented as a pair of arbitrarily vertices handles
                constraint = _sorted_vertex_pair(vertices_handles[-2], vertices_handles[-1])
                constraints.append(constraint)
        if close_it and len(vertices_handles) > 2:
            constraints.append(_sorted_vertex_pair(vertices_handles[-1], vertices_handles[0]))
        # Now actually insert the edges as constraints into the triangulation
        for constraint in constraints:
            self.cdt.insert_constraint(*constraint)
        return vertices_handles, constraints

"""
Provide triangulation and meshing of a clean, single site, geometry.
"""

from collections import defaultdict

from shapely import geometry as sh_geom

from CGAL.CGAL_Kernel import (
    Point_2 as Point,
    Vector_2 as Vector,
    Triangle_2 as Triangle,
    Ref_int,
    centroid)
from CGAL.CGAL_Mesh_2 import (
    Mesh_2_Constrained_Delaunay_triangulation_plus_2 as CDT,
    Mesh_2_Constrained_Delaunay_triangulation_plus_2_Face_handle as Face_handle,
    Ref_Mesh_2_Constrained_Delaunay_triangulation_plus_2_Face_handle as Ref_Face_handle,
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

def sorted_vertex_pair(v1, v2):
    if v1.__hash__() < v2.__hash__():
        return (v1, v2)
    else:
        return (v2, v1)

def first_and_last(iterable):
    it = iter(iterable)
    first = next(it)
    for e in it:
        last = e
    return (first, last)


def ilinks(it, close_it=False):
    """Given an input iterable `it` iter over the successive pairs (v0,
    v1), (v1, v2)... if close_it=True add an nth pair (vn-1, v0)
    """
    first = next(it)
    prev = first
    for el in it:
        yield (prev, el)
        prev = el
    if close_it and prev != first:
        yield (prev, first)

class MeshedCDTWithInfo(object):
    """
    This call provide the meshing of a geometry with arbitrary informations attached
    """
    FaceInfo = dict
    EdgeInfo = dict
    VertexInfo = dict

    def __init__(self):
        self.cdt = CDT()
        self.faces_infos = defaultdict(self.FaceInfo)
        self.vertices_infos = {}
        self._edges_infos = defaultdict(list)
        self._constraints_infos = {}

    def input_constraint_infos(self, (va, vb)):
        """Get the constraint informations associated to the given pair of vertices

        NB: Input constraints are represented as a pair of vertices,
        sorted in some arbitrary order and used as keys in
        _constraints_infos. This method ensure this normalisation is
        performed.

        """
        return self._constraints_infos[sorted_vertex_pair(va, vb)]

    def insert_constraint(self, va, vb, **kwargs):
        constraint = sorted_vertex_pair(va, vb)
        self.cdt.insert_constraint(va, vb)
        self._constraints_infos[constraint] = self.EdgeInfo(**kwargs)
        return constraint

    def insert_point(self, point, **kwargs):
        point = to_cgal_point(point)
        vertex = self.cdt.insert(point)
        self.vertices_infos[vertex] = self.VertexInfo(**kwargs)
        return vertex

    def insert_polyline(self, polyline, close_it=False, connected=True, **kwargs):
        """Insert a sequence of points as a polyline.

        If ``close_it=True`` is specified, then a segment joining the
        last point to the first one is added. No attempt is done at
        ensure this is a valid  (ccw oriented and simple) polygon.

        If ``connected=False`` is specified only the points are added,
        but not the segments connecting them.

        The key-word arguments are used to buildthe self.VertexInfo
        and self.EdgeInfo instances associated with new vertices and
        constraints.

        Returns the list of vertices handles and of constraints added.
        """
        assert not close_it or connected, \
            "It's meaningless to close an unconnected points sequence"
        # Insert points, thus making CDT vertices and build the list of edges
        vertices_handles = [self.insert_point(point, **kwargs)
                            for point in polyline]
        if connected:
            constraints = [self.insert_constraint(va, vb, **kwargs)
                           for va, vb in ilinks(iter(vertices_handles), close_it=close_it)]
        else:
            constraints = []
        return vertices_handles, constraints

    def vertice_pair_from_edge(self, (face, index)):
        """ Make a normalized pair of vertices handles from an edge
        """
        va = face.vertex(self.cdt.cw(index))
        vb = face.vertex(self.cdt.ccw(index))
        return sorted_vertex_pair(va, vb)

    def edge_from_vertice_pair(self, va, vb):
        """ Make a CGAL edge (Face_handle, index) from a pair of pair of vertices handles
        """
        # fh and i are output arguments a la C++
        fh = Ref_Face_handle()
        i = Ref_int()
        is_edge = self.cdt.is_edge(va, vb, fh, i)
        if is_edge:
            return (fh.object(), i.object())
        else:
            raise ValueError("This is not an edge (%s, %s)" % (va.point(), vb.point()))

    def iter_input_constraint_overlapping(self, edge):
        """Return an iterator over the input constraint overlapping the given edge.

        The edge can be given either as a pair a vertices handles or a
        face handle and an index.
        """
        if isinstance(edge[0], Face_handle):
            edge_v_pair = self.vertice_pair_from_edge(edge)
        else:
            edge_v_pair = edge
            edge = self.edge_from_vertice_pair(*edge_v_pair)
        if not self.cdt.is_constrained(edge):
            return
        for context in self.cdt.contexts(*edge_v_pair):
            context_boundaries = first_and_last(context.vertices())
            constraint_v_pair = sorted_vertex_pair(*context_boundaries)
            yield constraint_v_pair

    def iter_constraints_info_overlapping(self, edge):
        for constraint in self.iter_input_constraint_overlapping(edge):
            yield self._constraints_infos[constraint]

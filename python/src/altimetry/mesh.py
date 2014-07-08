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

def _sorted_vertex_pair(v1, v2):
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
        self.vertices_infos = defaultdict(self.VertexInfo)
        self._edges_infos = defaultdict(list)
        self._constraints_infos = defaultdict(self.EdgeInfo)

    def input_constraint_infos(self, (va, vb)):
        """Get the constraint informations associated to the given pair of vertices

        NB: Input constraints are represented as a pair of vertices,
        sorted in some arbitrary order and used as keys in
        _constraints_infos. This method ensure this normalisation is
        performed.

        """
        return self._constraints_infos[_sorted_vertex_pair(va, vb)]

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

    def insert_polyline_with_info(self, polyline,
                                  close_it=False, connected=True,
                                  **kwargs):
        """Just as ``insert_polyline``, but the remaining key-word arguments
        are used to build self.VertexInfo and self.EdgeInfo to build the
        additional information associated with new vertices and constraints.
        """
        vertices_handles, constraints = self.insert_polyline(
            polyline, close_it=False, connected=True)
        for vertex in vertices_handles:
            self.vertices_infos[vertex] = self.VertexInfo(**kwargs)
        for constraint in constraints:
            self._constraints_infos[constraint] = self.EdgeInfo(**kwargs)
        return vertices_handles, constraints

    def vertice_pair_from_edge(self, (face, index)):
        """ Make a normalized pair of vertices handles from an edge
        """
        va = face.vertex(self.cdt.cw(index))
        vb = face.vertex(self.cdt.ccw(index))
        return _sorted_vertex_pair(va, vb)

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
            constraint_v_pair = _sorted_vertex_pair(*context_boundaries)
            yield constraint_v_pair

    def iter_constraints_info_overlapping(self, edge):
        for constraint in self.iter_input_constraint_overlapping(edge):
            yield self._constraints_infos[constraint]


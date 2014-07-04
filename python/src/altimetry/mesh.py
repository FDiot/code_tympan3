"""
Provide triangulation and meshing of a clean, single site, geometry.
"""

from collections import defaultdict

from shapely import geometry as sh_geom

from CGAL.CGAL_Kernel import (
    Point_2 as Point,
    Vector_2 as Vector,
    Triangle_2 as Triangle,
    Segment_2 as Segment,
    Ref_int,
    centroid)
from CGAL.CGAL_Mesh_2 import (
    Mesh_2_Constrained_Delaunay_triangulation_plus_2 as CDT,
    Mesh_2_Constrained_Delaunay_triangulation_plus_2_Face_handle as Face_handle,
    Ref_Mesh_2_Constrained_Delaunay_triangulation_plus_2_Face_handle as Ref_Face_handle,
    Mesh_2_Constrained_Delaunay_triangulation_plus_2_Vertex_handle as Vertex_handle,
    Mesh_2_Constrained_Delaunay_triangulation_plus_2_Edge as Edge,
    Delaunay_mesh_plus_size_criteria_2 as Mesh_criteria,
    refine_Delaunay_mesh_2 as CGAL_refine_Delaunay_mesh)


# Monkey patch Edge to work around issue
# http://code.google.com/p/cgal-bindings/issues/detail?id=48
Edge.__iter__= lambda this: iter((this[0], this[1]))

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
        self.cdt.insert_constraint(va, vb)
        self._constraints_infos[sorted_vertex_pair(va, vb)] = self.EdgeInfo(**kwargs)
        return (va, vb) # Important to return the contrain in the input order

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

    def vertices_pair_from_half_edge(self, face, index):
        """Make a directed pair of vertices handles from an half edge so the
        pair is ccw oriented with respect to the input face (i.e. the
        fae is on the left of a->b)

        """
        va = face.vertex(self.cdt.ccw(index))
        vb = face.vertex(self.cdt.cw(index))
        return (va, vb)

    def ensure_vertices_pair(self, edge):
        if isinstance(edge[0], Face_handle):
            fh, i = edge
            return self.vertices_pair_from_half_edge(fh, i)
        else:
            return edge

    def half_edge_from_vertices_pair(self, va, vb):
        """ Make a CGAL half edge (Face_handle, index) from a pair of pair of vertices handles
        """
        # fh and i are output arguments a la C++
        fh = Ref_Face_handle()
        i = Ref_int()
        is_edge = self.cdt.is_edge(va, vb, fh, i)
        if is_edge:
            return (fh.object(), i.object())
        else:
            raise ValueError("This is not an edge (%s, %s)" % (va.point(), vb.point()))

    def ensure_half_edge(self, edge):
        if isinstance(edge[0], Vertex_handle):
            va, vb = edge
            return self.half_edge_from_vertices_pair(va, vb)
        else:
            return edge

    def iter_input_constraint_overlapping(self, edge):
        """Return an iterator over the input constraint overlapping the given edge.

        The edge can be given either as a pair of vertices handles or a
        half edge (face handle and an index).
        """
        edge_v_pair = self.ensure_vertices_pair(edge)
        half_edge = self.ensure_half_edge(edge)
        if not self.cdt.is_constrained(half_edge):
            return
        for context in self.cdt.contexts(*edge_v_pair):
            context_boundaries = first_and_last(context.vertices())
            constraint_v_pair = sorted_vertex_pair(*context_boundaries)
            yield constraint_v_pair

    def iter_constraints_info_overlapping(self, edge):
        for constraint in self.iter_input_constraint_overlapping(edge):
            yield self._constraints_infos[constraint]


    def fetch_constraint_infos_for_edges(self, edges=None):
        """Return a dict mapping each given edges (or all edges if none is
         given), to the list of input constraints information
         overlapping this edge.

        """
        edges_infos = {}
        if edges is None:
            edges_it = self.cdt.finite_edges() # Iter over half-edges !!!
        else:
            edges_it = iter(edges)
        for edge in edges_it:
            v_pair = self.ensure_vertices_pair(edge)
            infos = edges_infos.setdefault(v_pair, [])
            for info in self.iter_constraints_info_overlapping(edge):
                infos.append(info)
        return edges_infos

    def iter_input_constraint_around(self, vertex):
        incident_constrained_edges = [] # Output argument a la C++
        self.cdt.incident_constraints(vertex, incident_constrained_edges)
        for edge in incident_constrained_edges:
            for input_contraint in self.iter_input_constraint_overlapping(edge):
                 yield input_contraint

    def fetch_constraint_infos_for_vertices(self, vertices=None):
        """Return a dict mapping each given vertices (or all vertices if none
        is given) to the list of input constraint information incident
        to the vertex.

        NB 1: if the same input constraint overlap two incident edges
        the associated information will be present only once.

        NB 2: This only fetch information associated with input
        constraints, not with the the vertices themselves.
        """
        vertices_info = {}
        if vertices is None:
            vertices_it = self.cdt.finite_vertices()
        else:
            vertices_it = iter(vertices)
        for vertex in vertices_it:
            infos = vertices_info.setdefault(vertex, [])
            for input_constraint in self.iter_input_constraint_around(vertex):
                info = self.input_constraint_infos(input_constraint)
                if info not in infos:
                    infos.append(info)
        return vertices_info

    def segment_for_edge(self, edge):
        if isinstance(edge[0], Face_handle):
            return self.cdt.segment(edge)
        else: # Vertices pair assumed
            return Segment(edge[0].point(), edge[1].point())

    def faces_for_edge(self, va, vb):
        """Return the pair (face_left, face_right) of face handle respectively
        on the left and on the right of the given edge.
        """
        face1, i = self.half_edge_from_vertices_pair(va, vb)
        face2 = face1.neighbor(i)
        vaa, vbb = self.vertices_pair_from_half_edge(face1, i)
        if (vaa, vbb)==(va, vb): # f1 is the face on the left of a->b
            return (face1, face2)
        else:
            return (face2, face1)

    def point_for_face(self, fh):
        "Return a point in the interior of the face, or None if face is infinite"
        if self.cdt.is_infinite(fh):
            return None
        else:
            return centroid(*(fh.vertex(i).point() for i in xrange(3)))

    def iter_faces_for_input_constraint(self, va, vb):
        # CAUTION the CGAL method vertices_in_constraint does NOT
        # always return the vertices in the order from va to vb
        constraint_direction = Segment(va.point(), vb.point()).direction()
        def same_direction_as_constraint(v0, v1):
            s = Segment(v0.point(), v1.point())
            if s.direction() == constraint_direction:
                return (v0, v1)
            else:
                return (v1, v0)
        for v0, v1 in ilinks(self.cdt.vertices_in_constraint(va, vb)):
            v0, v1 = same_direction_as_constraint(v0, v1)
            yield self.faces_for_edge(v0, v1)

    def py_vertex(self, vh):
        p = vh.point()
        return ((p.x(), p.y()))

    def py_face(self, face):
        return ["FACE"] + [self.py_vertex(face.vertex(i)) for i in xrange(3)]

    def py_edge(self, edge):
        """Return a python representation of the edge either as a pair of
        points or a triple of point and an index
        """
        if isinstance(edge[0], Face_handle):
            return ["EDGE", edge[1]] + self.py_face(edge[0])[1:]
        else:
            return ["EDGE"] + [self.py_vertex(v) for v in edge]

    def face_for_vertices(self, v1, v2, v3):
        """Return the face handle for the face having the arguments as
        vertices or None if no such face exists.
        """
        fh = Ref_Face_handle()
        if self.cdt.is_face(v1, v2, v3, fh):
            return fh.object()
        else:
            return None

    def iter_faces_for_input_polyline(self, poly, close_it=False):
        for constraint in ilinks(iter(poly), close_it=close_it):
            for faces_left_right in self.iter_faces_for_input_constraint(*constraint):
                yield faces_left_right

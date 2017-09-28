"""
Provide triangulation and meshing of a clean, single site, geometry.
"""
from collections import defaultdict
import copy
from warnings import warn

import numpy as np
from scipy.interpolate import InterpolatedUnivariateSpline
from shapely.geometry import Point, LineString

from CGAL.CGAL_Kernel import (
    Point_2,
    Segment_2 as Segment,
    Point_3,
    Vector_3,
    Triangle_3,
    Line_3,
    Ref_int,
    centroid,
    intersection)
from CGAL.CGAL_Mesh_2 import (
    Mesh_2_Constrained_Delaunay_triangulation_plus_2 as CDT,
    Mesh_2_Constrained_Delaunay_triangulation_plus_2_Face_handle as Face_handle,
    Ref_Mesh_2_Constrained_Delaunay_triangulation_plus_2_Face_handle as Ref_Face_handle,
    Mesh_2_Constrained_Delaunay_triangulation_plus_2_Vertex_handle as Vertex_handle,
    Mesh_2_Constrained_Delaunay_triangulation_plus_2_Edge as Edge,
    Delaunay_mesh_plus_size_criteria_2 as Mesh_criteria,
    refine_Delaunay_mesh_2 as CGAL_refine_Delaunay_mesh)
from CGAL.CGAL_Triangulation_2 import  (
    Ref_Locate_type_2 as Ref_locate_type,
    VERTEX, EDGE, FACE, OUTSIDE_CONVEX_HULL, OUTSIDE_AFFINE_HULL
)
import collections
from functools import reduce

Z_VECTOR = Vector_3(0, 0, 1)
_PROXIMITY_THRESHOLD = 0.01

# Monkey patch Edge to work around issue
# http://code.google.com/p/cgal-bindings/issues/detail?id=48
Edge.__iter__= lambda this: iter((this[0], this[1]))


from . datamodel import InconsistentGeometricModel, DEFAULT_MATERIAL, HIDDEN_MATERIAL


UNSPECIFIED_ALTITUDE = float('nan')
UNSPECIFIED_MATERIAL = None

def to_cgal_point(pt):
    if isinstance(pt, Point_2):
        return pt
    elif isinstance(pt, Point):
        return Point_2(pt.x, pt.y)
    elif isinstance(pt, (tuple, list)):
        assert len(pt) == 2
        return Point_2(*pt)
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
    """ This class provides the meshing of a geometry with arbitrary
    information attached.
    """
    edge_info = dict
    vertex_info = dict

    def __init__(self):
        self.cdt = CDT()
        self._input_vertices_infos = {}
        self._input_constraints_infos = {}

    def vertices_map_to_other_mesh(self, other_mesh):
        vmap = {}
        for orig_vh in self.cdt.finite_vertices():
            _, dest_vh = other_mesh.locate_point(orig_vh.point())
            if not isinstance(dest_vh, Vertex_handle):
                # other_mesh as not vertex at the same position as orig_vh
                continue
            assert orig_vh.point() == dest_vh.point()
            vmap[orig_vh] = dest_vh
        return vmap

    def copy(self, class_=None, deep=False, vmap=None):
        """Return a copy of self as a new mesher.

        The copy wraps a copy of the underlying CGAL CDT and a copy of the
        input information stored by self. If deep==True the information is
        copied using the standard copy.deepcopy function, otherwise (the
        default) a shallow copy is made with copy.copy.

        In order to port information from self to the copy, a vertex handle
        mapping from self's vertices to the copy's vertices is built. If a
        dictionnary like object is provided as vmap optional argument, it is
        expected to be empty and will be updated with the computed map (useful
        for overrides of copy).

        And if a copy of self as an other type is desired, the class_ argument
        provides for specifying the desired type. USE WITH CAUTION, this
        option is mainly aimed at internal use for with ElevationMesh.
        """
        vmap = {} if vmap is None else vmap
        if len(vmap) != 0:
            raise ValueError("The vertice map output argument is expected to be empty")
        class_ =  type(self) if class_ is None else class_
        # Copying a CDT is tricky
        # See http://code.google.com/p/cgal-bindings/issues/detail?id=49
        newone = class_()
        newone.cdt = self.cdt.deepcopy()
        vmap.update(self.vertices_map_to_other_mesh(newone))
        for orig_vh, orig_info in self._input_vertices_infos.items():
            dest_vh = vmap[orig_vh]
            dest_info = copy.deepcopy(orig_info) if deep else copy.copy(orig_info)
            newone._input_vertices_infos[dest_vh] = dest_info
        for (orig_va, orig_vb), orig_info in self._input_constraints_infos.items():
            dest_va, dest_vb = vmap[orig_va], vmap[orig_vb]
            dest_info = copy.deepcopy(orig_info) if deep else copy.copy(orig_info)
            if sorted_vertex_pair(dest_va, dest_vb) != (dest_va, dest_vb):
                warn("Vertex pair unexpectedly not sorted during copy", RuntimeWarning)
            newone._input_constraints_infos[sorted_vertex_pair(dest_va, dest_vb)] = dest_info
        return newone

    def count_edges(self):
        count_edges, count_constrained = 0, 0
        for c in self.cdt.finite_edges():
            if self.cdt.is_constrained(c):
                count_constrained += 1
            count_edges += 1
        return count_edges, count_constrained

    def input_constraint_infos(self, vertices):
        """Get the constraint informations associated to the given pair of vertices

        NB: Input constraints are represented as a pair of vertices,
        sorted in some arbitrary order and used as keys in
        _input_constraints_infos. This method ensure this normalisation is
        performed.

        """
        (va, vb) = vertices
        return self._input_constraints_infos[sorted_vertex_pair(va, vb)]

    def input_vertex_infos(self, v):
        return self._input_vertices_infos[v]

    def insert_constraint(self, va, vb, **kwargs):
        self.cdt.insert_constraint(va, vb)
        self._input_constraints_infos[sorted_vertex_pair(va, vb)] = self.edge_info(**kwargs)
        return (va, vb) # Important to return the contrain in the input order

    def insert_point(self, point, **kwargs):
        point = to_cgal_point(point)
        vertex = self.cdt.insert(point)
        self._input_vertices_infos[vertex] = self.vertex_info(**kwargs)
        return vertex

    def insert_polyline(self, polyline, close_it=False, connected=True, **kwargs):
        """Insert a sequence of points as a polyline.

        If ``close_it=True`` is specified, then a segment joining the
        last point to the first one is added. No attempt is done at
        ensure this is a valid  (ccw oriented and simple) polygon.

        If ``connected=False`` is specified only the points are added,
        but not the segments connecting them.

        The key-word arguments are used to buildthe self.vertex_info
        and self.edge_info instances associated with new vertices and
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
            yield self.input_constraint_infos(constraint)

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

    def merge_info_for_vertices(self, merge_function, vertices=None, init_map=None):
        """Reduce the list of information available on each vertex to a single information.

        This method works by calling repeatedly merge_function to
        merge two infos into a single one, starting with informatiosn
        drawn from init_map (or default constructed if init_map is not
        given or does not contains the vertex).

        If vertices is None all vertices are considered.

        """
        init_map = init_map or {}
        d = {}
        for v, info_list in self.fetch_constraint_infos_for_vertices(vertices=vertices).items():
            info = self._input_vertices_infos.get(v, init_map.get(v, self.vertex_info()))
            try:
                d[v] = reduce(merge_function, info_list, info)
            except InconsistentGeometricModel as exc:
                exc.witness_point = self.py_vertex(v)
                raise
        return d

    def merge_info_for_edges(self, merge_function, edges=None, init_map=None):
        """Reduce the list of information available on each edges to a single information.

        This method works by calling repeatedly merge_function to
        merge two infos into a single one, starting with informatiosn
        drawn from init_map (or default constructed if init_map is not
        given or does not contains the vertex).

        If edges is None all edges are considered.
        """
        init_map = init_map or {}
        d = {}
        for v_pair, info_list in self.fetch_constraint_infos_for_edges(edges=edges).items():
            v_pair = sorted_vertex_pair(*v_pair)
            info = init_map.get(v_pair, self.edge_info())
            try:
                d[v_pair] = reduce(merge_function, info_list, info)
            except InconsistentGeometricModel as exc:
                pa, pb = (v.point() for v in v_pair)
                exc.witness_point = ((pa.x() + pb.x())/2.0, (pa.y() + pb.y())/2.0)
                raise
        return d

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

    def triangle_for_face(self, fh):
        if self.cdt.is_infinite(fh):
            return None
        else:
            return [fh.vertex(i).point() for i in range(3)]

    def point_for_face(self, fh):
        "Return a point in the interior of the face, or None if face is infinite"
        if self.cdt.is_infinite(fh):
            return None
        else:
            return centroid(*self.triangle_for_face(fh))

    def iter_edges_for_input_constraint(self, va, vb):
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
            yield same_direction_as_constraint(v0, v1)

    def iter_vertices_for_input_polyline(self, poly, close_it=False):
        for va, vb in ilinks(iter(poly), close_it=close_it):
            for v in self.cdt.vertices_in_constraint(va, vb):
                yield v

    def iter_faces_for_input_constraint(self, va, vb):
        for v0, v1 in self.iter_edges_for_input_constraint(va, vb):
            yield self.faces_for_edge(v0, v1)

    def iter_edges_for_input_polyline(self, poly, close_it=False):
        for constraint in ilinks(iter(poly), close_it=close_it):
            for v0, v1 in self.iter_edges_for_input_constraint(*constraint):
                yield (v0, v1)

    def iter_faces_for_input_polyline(self, poly, close_it=False):
        for v0, v1 in self.iter_edges_for_input_polyline(poly, close_it=close_it):
            yield self.faces_for_edge(v0, v1)

    def py_vertex(self, vh):
        """ Return a pure python representation of the vertex, intended for debugging"""
        p = vh.point()
        return (p.x(), p.y())

    def py_face(self, face):
        """ Return a pure python representation of the face, intended for debugging"""
        return ["FACE"] + [self.py_vertex(face.vertex(i)) for i in range(3)]

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

    def refine_mesh(self, hole_seeds=None, size_criterion=0, shape_criterion=0.125):
        """Refine the triangulation into a regular mesh.

        * size_criteria is an upper bound on the size of triangles
          (the default, 0, means no bound)
        * shape_criteria is explained in CGAL documentation.
          Beware! The default value is the best bound which warrant
          termination of the algorithms, Changing its value can lead
          to application freezing.

        cf. http://doc.cgal.org/latest/Mesh_2/index.html#secMesh_2_criteria
        """
        if shape_criterion > 0.125:
            raise ValueError("0.125 is the best shape criterion still providing"
                             " warranted termination of the refinement.")
        criteria = Mesh_criteria(shape_criterion, size_criterion)
        hole_seeds = hole_seeds or []
        CGAL_refine_Delaunay_mesh(self.cdt, hole_seeds, criteria)

    def mirror_half_edge(self, fh, i):
        fh2 = fh.neighbor(i)
        i2 = self.cdt.mirror_index(fh, i)
        return (fh2, i2)

    def locate_point(self, p, face_hint=None):
        """Locate a point in the triangulation and return a pair whose first
        element is the finite face handle of a face on which the point
        lies and the second element a complementary information :

        * if the point is out of the convex hull of the triangulation,
          the first face handle is None and the second if the witness
          infinite face handle return by CGAL locate method.
        * if the point lies within the face, the second element is None

        * if the point is on an edge, then the second element is the
          index of the edge and thus the returned (face _andle, index)
          forms an half-edge representation
        * if the point is a vertex the second element is its vertex handle

        An optional Face_handle can be passed as the face_hint
        argument. It can be used to make the search in the
        triangulation start from the given face (cf CGAL doc for
        CDT::locate(...)).
        """

        face_hint = face_hint or Face_handle()
        p = to_cgal_point(p)
        locate_type = Ref_locate_type()
        index = Ref_int()
        face_handle = self.cdt.locate(p, locate_type, index, face_hint)
        locate_type = locate_type.object()
        if locate_type == OUTSIDE_CONVEX_HULL:
            return None, face_handle
        elif locate_type == FACE:
            return face_handle, None
        elif locate_type == EDGE:
            return (face_handle, index.object())
        elif locate_type == VERTEX:
            return face_handle, face_handle.vertex(index.object())
        else:
            assert locate_type == OUTSIDE_AFFINE_HULL
            raise InconsistentGeometricModel("Degenerate triangulation (0D or 1D)")

    # Export utilities.
    def as_arrays(self):
        """Return vertices and faces as Numpy arrays."""
        vertices, vertices_id = [], {}
        for idx, vh in enumerate(self.cdt.finite_vertices()):
            vertices_id[vh] = idx
            point = self.point3d_for_vertex(vh)
            vertices.append((point.x(), point.y(), point.z()))
        faces = [[vertices_id[fh.vertex(i)] for i in range(3)]
                 for fh in self.cdt.finite_faces()]
        return np.array(vertices), np.array(faces)

    def faces_material(self, material_by_face):
        """Return the list of faces' material given a `material_by_face`
        mapping.
        """
        return [material_by_face[fh] for fh in self.cdt.finite_faces()]

    def segment_intersection_points(self, segment):
        """Return the list of intersection points between the mesh CDT and a
        segment. This is list sorted according to the distance of points to
        the origin (first point) of the segment and only contains unique
        points.
        """
        if not isinstance(segment, LineString):
            segment = LineString(segment)
        points = list(self._segment_intersection_points(segment))
        # Sort points by distance to the segment origine.
        points.sort(key=lambda x: Point(segment.coords[0]).distance(x))
        # Eliminate identical adjacent points.
        if len(points) < 2:
            return points
        points =  [p for i, p in enumerate(points)
                   if not p.equals(points[i-1])]
        return points

    def _segment_intersection_points(self, segment):
        """Yield intersection points between the mesh CDT and a segment."""
        for fh in self.cdt.finite_faces():
            triangle = self._face_triangle(fh)
            inter = triangle.intersection(segment)
            if not inter:
                # No intersection.
                continue
            elif isinstance(inter, Point):
                # Got a single Point
                yield inter
            elif isinstance(inter, LineString):
                # Got a LineString
                for coord in inter.coords:
                    yield Point(coord)
            else:
                try:
                    inter = iter(inter)
                    # Got a collection of points.
                    for i in inter:
                        assert isinstance(i, Point), i
                        yield i
                except TypeError:
                    raise Exception('unhandled intersection object %r' % inter)

    def _face_triangle(self, fh):
        """Return a shapely geometry LineString for given face."""
        return LineString([self.py_vertex(fh.vertex(i % 3))
                           for i in range(4)])


class InfoWithIDsAndAltitude(object):
    ALTITUDE_TOLERANCE = 0.1

    def  __init__(self, altitude=UNSPECIFIED_ALTITUDE, id=None, **kwargs):
        self.ids = kwargs.pop('ids', set((id and [id]) or []))
        self.altitude = float(altitude)

    def merge_with(self, other_info):
        """ Merges all information provided by ``other`` into self.

        Returns self to enable chaining or merge via reduce.
        """
        self.merge_ids(other_info)
        self.merge_altitude(other_info)
        return self # so as to enable using reduce

    def merge_ids(self, other_info):
        ids = getattr(other_info, "ids", None)
        if ids is None: return
        self.ids.update(ids)

    def merge_altitude(self, other_info):
        alti = getattr(other_info, "altitude", UNSPECIFIED_ALTITUDE)
        if alti is not UNSPECIFIED_ALTITUDE:
            if self.altitude is UNSPECIFIED_ALTITUDE:
                self.altitude = alti
            else:
                delta = abs(alti - self.altitude)
                if delta > self.ALTITUDE_TOLERANCE:
                    raise InconsistentGeometricModel(
                        "Intersecting constraints with different altitudes: {ids}",
                        ids=sorted(self.ids))

    def __repr__(self):
        args = ", ".join(["%s=%r" % kv for kv in self.__dict__.items()])
        return "%s(%s)" % (self.__class__.__name__, args)

    def __eq__(self, other):
        if isinstance(other, self.__class__): # XXX type(other) is type(self)
            return self.__dict__ == other.__dict__
        else:
            return False

    def __ne__(self, other):
        return not self.__eq__(other)


class EdgeInfoWithMaterial(InfoWithIDsAndAltitude):
    """ Add a flag if the edge is a boundary for materials """

    def  __init__(self, altitude=UNSPECIFIED_ALTITUDE,
                  material=None, id=None, **kwargs):
        super(EdgeInfoWithMaterial, self).__init__(altitude, id=id, **kwargs)
        self.material_boundary = material is not None
        assert material is None or isinstance(material, str) #XXX
        self.landtake_boundary = material == HIDDEN_MATERIAL.id

    def merge_material_boundary(self, other):
        self.material_boundary |= other.material_boundary
        self.landtake_boundary |= other.landtake_boundary

    def merge_with(self, other_info):
        super(EdgeInfoWithMaterial, self).merge_with(other_info)
        self.merge_material_boundary(other_info)
        return self # so as to enable using reduce


class ElevationMesh(MeshedCDTWithInfo):
    """ An elevation mesh associates an altitude to its vertices.

    This altitude can be unspecified (yet) and represented as UNSPECIFIED_ALTITUDE
    """

    vertex_info = InfoWithIDsAndAltitude

    edge_info = EdgeInfoWithMaterial

    def __init__(self):
        super(ElevationMesh, self).__init__()
        self.vertices_info = defaultdict(self.vertex_info)
        self.edges_info = defaultdict(self.edge_info)

    def copy(self, class_=None, deep=False, vmap=None):
        vmap = {} if vmap is None else vmap
        newone = super(ElevationMesh, self).copy(class_=class_, deep=deep, vmap=vmap)
        for orig_vh, orig_info in self.vertices_info.items():
            dest_vh = vmap[orig_vh]
            dest_info = copy.deepcopy(orig_info) if deep else copy.copy(orig_info)
            newone.vertices_info[dest_vh] = dest_info
        for (orig_va, orig_vb), orig_info in self.edges_info.items():
            dest_va, dest_vb = vmap[orig_va], vmap[orig_vb]
            dest_info = copy.deepcopy(orig_info) if deep else copy.copy(orig_info)
            newone.edges_info[(dest_va, dest_vb)] = dest_info
        return newone

    def insert_point(self, point, **kwargs):
        vh = super(ElevationMesh, self).insert_point(point, **kwargs)
        self.vertices_info[vh] = self.input_vertex_infos(vh) # TODO Consider copying ?
        return vh

    def point3d_for_vertex(self, vh):
        alti = self.vertices_info[vh].altitude
        x, y = vh.point().x(), vh.point().y()
        if alti is UNSPECIFIED_ALTITUDE:
            raise InconsistentGeometricModel("No altitude defined for vertex",
                                             witness_point=(x, y))
        else:
            return Point_3(x, y, alti)

    def triangle3d_for_face(self, fh):
        if self.cdt.is_infinite(fh):
            return None
        else:
            t = Triangle_3(*(self.point3d_for_vertex(fh.vertex(i))
                             for i in range(3)))
            return t

    def update_info_for_vertices(self, vertices=None, init_map=None):
        merge_function = lambda i1, i2: i1.merge_with(i2)
        d = self.merge_info_for_vertices(
            merge_function, vertices=vertices, init_map=self.vertices_info)
        self.vertices_info.update(d)

    def update_info_for_edges(self, edges=None, init_map=None):
        merge_function = lambda i1, i2: i1.merge_with(i2)
        d = self.merge_info_for_edges(
            merge_function, edges=edges, init_map=self.edges_info)
        self.edges_info.update(d)

    def altitude_for_input_vertex(self, vh):
        alti = self.input_vertex_infos(vh).altitude
        assert alti is not UNSPECIFIED_ALTITUDE, \
            'vertex {} has no altitude specified'.format(self.py_vertex(vh))
        return alti

    def point_altitude(self, p, face_hint=None):
        p = to_cgal_point(p)
        fh, vh_or_i = self.locate_point(p, face_hint=face_hint)
        if fh is None: # point p is out of the convex hull of the triangulation
            return UNSPECIFIED_ALTITUDE
        if (isinstance(vh_or_i, Vertex_handle)
                and vh_or_i in self._input_vertices_infos):
            # Point is on a vertex, which is part of an input constraint.
            return self.altitude_for_input_vertex(vh_or_i)
        # point is on an edge and get a finite face if needed
        if isinstance(vh_or_i, int) and self.cdt.is_infinite(fh):
            fh, _ = self.mirror_half_edge(fh, vh_or_i)
            assert not self.cdt.is_infinite(fh)
        triangle = self.triangle3d_for_face(fh)
        p2 = Point_3(p.x(), p.y(), 0)
        vline = Line_3(p2, Z_VECTOR)
        inter = intersection(triangle, vline)
        if not inter.is_Point_3():
            raise InconsistentGeometricModel("Can not compute elevation",
                                             witness_point=(p.x(), p.y()))
        p3 = inter.get_Point_3()
        alti = p3.z()
        dist = abs((p3-p2).squared_length()-alti**2)
        assert dist <= _PROXIMITY_THRESHOLD * 1e-3  + _PROXIMITY_THRESHOLD * (alti**2), (
            "unexpected distance between point and its projection : %f (threshold = %f, altitude = %f)"
            % (dist, _PROXIMITY_THRESHOLD, alti))
        return alti

    def update_altitude_from_reference(self, reference):
        for vh in self.cdt.finite_vertices():
            try:
                input_altitude = self._input_vertices_infos[vh].altitude
            except KeyError:
                input_altitude = UNSPECIFIED_ALTITUDE
            if input_altitude is UNSPECIFIED_ALTITUDE:
                info = self.vertices_info[vh]
                info.altitude = reference(vh.point())
                if info.altitude is UNSPECIFIED_ALTITUDE:
                    warn("Found a vertex with an unspecified altitude in the mesh (%f, %f)"
                         % (vh.point().x(), vh.point().y()), RuntimeWarning)
                    # XXX this happens when the point is outside the convex hull
                    # of the triangulation (see ReferenceElevationMesh.point_altitude)
                    # In that case, don't keep unspecified altitudes because it
                    # leads to an exception (raised in ElevationMesh.point3d_for_vertex)
                    # XXX Would it be better to remove the point somewhere
                    # before entering this method ?
                    info.altitude = 0

    def flood_polygon(self, flooder_class, vertices, flood_right=False, close_it=False):
        """Flood the inside of a polygon given by its vertices list using the
        specified class of Flooder.

        By default flood on the left side of the boudary, which is the
        inner of the polygon if it is conter-clock-wise oriented. You
        can specify flood_right=True if you fill the outside of if the
        polygon is clock-wise-oriented.

        """
        faces_left, faces_right = left_and_right_faces(
            self.iter_faces_for_input_polyline(vertices, close_it=True))
        seed_faces = faces_right if flood_right else faces_left
        flooder = flooder_class(self)
        flooder.flood_from(seed_faces)
        return flooder


class ReferenceElevationMesh(ElevationMesh):
    """A reference elevation mesh is an elevation mesh whose all
    constraint must have an altitude specified.

    This enables computing the altitude of all vertices created as
    constraints intersection, and from this on to compute an
    interpolated altitude for all points within the convex hull of the
    constraints.
    """

    def insert_point(self, point, **kwargs):
        if 'altitude' not in kwargs:
            raise TypeError('altitude is mandatory for *reference* elevation meshes')
        return super(ReferenceElevationMesh, self).insert_point(point, **kwargs)

    def insert_polyline(self, polyline, **kwargs):
        if 'altitude' not in kwargs:
            raise TypeError('altitude is mandatory for *reference* elevation meshes')
        return super(ReferenceElevationMesh, self).insert_polyline(polyline, **kwargs)


class ElevationProfile(object):
    """2D profile built from the intersection of a segment with a mesh CDT.

    mesh: a ReferenceElevationMesh
    segment: a LineString (shapely.geometry)

    Example:

    >>> profile = ElevationProfile(mesh, ((0, 0), (1, 1)))
    >>> # Evaluate the profile a distance 0.5 from segment origin.
    >>> profile(0.5)
    >>> # Evaluate the profile for an array of distances.
    >>> h = profile(np.arange(0, 1, 0.1))
    >>> # Compute the first and second derivative of the profile.
    >>> dh = h.gradient()
    >>> d2h = dh.gradient()
    """
    def __init__(self, mesh, segment):
        self._mesh = mesh
        if not isinstance(segment, LineString):
            segment = LineString(segment)
        if len(segment.coords) != 2:
            raise ValueError('expecting a segment with two points')
        self._segment = segment
        self.points = mesh.segment_intersection_points(segment)
        # Cached spline.
        self._spline = None

    def face_data_interpolator(self, face_data):
        """Return an interpolator on profile points for data associated with
        mesh faces through the `face_data` dict.
        """
        distances = self._point_distances()
        point_data = [self.point_data(d, face_data) for d in distances]
        return self.point_data_interpolator(point_data, distances)

    def point_data_interpolator(self, point_data, _distances=None):
        """Return an interpolator on profile points for data associated with
        profile points through `point_data`.

        `point_data` may be a sequence of same lenght that distances or a
        callable giving data as a function of the distance to the segment
        origin.
        """
        distances = _distances or self._point_distances()
        if isinstance(point_data, collections.Callable):
            point_data = list(map(point_data, distances))
        elif len(distances) != len(point_data):
            raise ValueError('incompatible number of data points')
        return InterpolatedUnivariateSpline(distances, point_data, k=1)

    @property
    def spline(self):
        """The underlying interpolating spline.

        May be None if the segment does not intersect the mesh.
        """
        if not self.points:
            # no points in segment, cannot build a spline.
            return None
        if not self._spline:
            self._spline = self.point_data_interpolator(self.point_altitude)
        return self._spline

    @property
    def direction(self):
        """Direction vector (in horizontal plane) of the profile as a numpy
        array.
        """
        start, end = self._segment.coords
        v = np.array(end) - np.array(start)
        return v / np.linalg.norm(v)

    @property
    def _segment_origin(self):
        return Point(self._segment.coords[0])

    def _point_at_distance(self, dist):
        """Return a Point object located at `dist` from segment origin"""
        return Point(np.array(self._segment_origin) + dist * self.direction)

    def _point_distances(self):
        """Return the list of point distances"""
        origin = self._segment_origin
        return [origin.distance(p) for p in self.points]

    def point_altitude(self, dist):
        """Return the altitude of a point located at `dist` from segment
        origin. This is the altitude of the mesh triangle the point belongs
        to.
        """
        p = self._point_at_distance(dist)
        return self._mesh.point_altitude(p)

    def point_data(self, dist, face_data, default=None):
        """Return some "data" at a point located at `dist` from segment origin
        based on the mesh face the point belongs to and the `face_data`
        mapping which relates mesh face handles to these "data". `default` is
        used if the face is not found in the `face_data` map or the the point
        if outside the mesh convex hull. In case the point is located on a
        mesh vertex, the data of any of the mesh faces which the vertex
        belongs to will be used.

        For instance, given a `material_by_face` map::

            material = profile.point_data(dist, material_by_face)

        will return the material of a point located at `dist` from segment
        origin in the profile.
        """
        p = self._point_at_distance(dist)
        # See ReferenceElevationMesh.point_altitude for logic.
        fh, vh_or_i = self._mesh.locate_point(to_cgal_point(p))
        if fh is None:
            # Point outside the convex hull.
            return default
        if isinstance(vh_or_i, Vertex_handle):
            # Point is a vertex.
            for fh in self._mesh.cdt.finite_faces():
                if fh.has_vertex(vh_or_i):
                    break
            else:
                return default
        # Point is on an edge and get a finite face if needed:
        if isinstance(vh_or_i, int) and self._mesh.cdt.is_infinite(fh):
            fh, _ = self._mesh.mirror_half_edge(fh, vh_or_i)
            assert not self._mesh.cdt.is_infinite(fh)
        return face_data.get(fh, default)

    def __call__(self, dist):
        """Return the altitude at distance `dist` from segment origin.

        Either it is interpolated if the segment intersects some mesh faces or
        it is computed by mesh projection.
        """
        if self.spline is not None:
            return self.spline(dist)
        else:
            return np.vectorize(self.point_altitude)(dist)


class FaceFlooder(object):
    """This is a base class for implementing the family of flood algorithms

    Flood algorithms are a way of walking through a mesh to mark or
    process faces, edges and vertices. It consists of starting at some
    seed point and processing adjacents faces (potentially under
    condition on the edge connecting them).
    """

    def __init__(self, mesher):
        self.visited = set()
        self.frontier = set()
        self.border = set()
        self.mesher = mesher

    def add_to_frontier(self, element):
        if element not in self.visited:
            self.frontier.add(element)

    def links_for(self, face_handle):
        """ Return an iterable on edges with adjacent faces
        """
        for i in range(3):
            edge = (face_handle, i)
            yield edge

    def follow_link(self, edge):
        """ Follow the edge to the adjacent face if the face
        is finite and self.should_follow(edge) is True.
        """
        cdt = self.mesher.cdt
        (fh, i) = edge
        neighbor = fh.neighbor(i)
        if not cdt.is_infinite(neighbor) and self.should_follow(fh, edge, neighbor):
            return neighbor
        else:
            return None

    def should_follow(self, from_face, edge, to_face):
        """Override this method to define which edges stop the flood"""
        raise NotImplementedError

    def visit_epilog(self, element):
        """Override this method if some action needs to be taken on an element
        after all potential neighbor have been considered and
        scheduled for visit in case they should.
        """
        return element

    def visit_one(self):
        """Process the next element in the frontier: consider all potential
        links to neighbors (as returned by ``links_for``) and, for
        the links which should be followed to a neighbor (as
        indicated by ``follow_link``) mark the neighbor for visit if
        is has no already been visited.

        Returns the value returned by ``visit_epilog`` called at the end.

        Raises IndexError if the frontier is empty

        """
        next_one = self.frontier.pop()
        for link in self.links_for(next_one):
            neighbor = self.follow_link(link)
            if not neighbor:
                self.border.add(link)
                continue
            if neighbor in self.visited:
                continue
            self.frontier.add(neighbor)
        self.visited.add(next_one)
        return self.visit_epilog(next_one)

    def flood_from(self, seeds):
        """Walk faces reachable from the given seeds

        ``seeds`` must be an iterable over elements.  The first seed
        is added to the frontier and the graph is flooded from this
        point until the frontier is empty.  Then the next seed is
        added to the frontier if it has not beed visited yet and the process
        starts again until seeds are all considered.
        """
        for seed in seeds:
            if seed in self.visited:
                continue
            self.add_to_frontier(seed)
            while(self.frontier):
                self.visit_one()


def left_and_right_faces(faces_it):
    """Takes an iterable on pair (left_face, right_face) and return the
    pair of the list of all left faces and the list of all right faces
    """
    return zip(*list(faces_it))


class LandtakeFaceFlooder(FaceFlooder):

    def is_landtake_border(self, edge):
        return any(info.landtake_boundary
                   for info in self.mesher.iter_constraints_info_overlapping(edge))

    def should_follow(self, from_face, edge, to_face):
        return not self.is_landtake_border(edge)

"""
Provide triangulation and meshing of a clean, single site, geometry.
"""
from collections import defaultdict
import copy

from shapely import geometry as sh_geom

from CGAL.CGAL_Kernel import (
    Point_2 as Point,
    Vector_2 as Vector,
    Triangle_2 as Triangle,
    Segment_2 as Segment,
    Point_3,
    Vector_3,
    Triangle_3,
    Line_3,
    Ref_int,
    Object as CGAL_Object,
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

Z_VECTOR = Vector_3(0, 0, 1)
_PROXIMITY_THRESHOLD = 0.01

# Monkey patch Edge to work around issue
# http://code.google.com/p/cgal-bindings/issues/detail?id=48
Edge.__iter__= lambda this: iter((this[0], this[1]))


from . datamodel import InconsistentGeometricModel


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
    EdgeInfo = dict
    VertexInfo = dict

    def __init__(self):
        self.cdt = CDT()
        self._input_vertices_infos = {}
        self._input_constraints_infos = {}

    def clear_caches(self):
        pass

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

    def copy(self, class_=None, deep=False):
        class_ =  type(self) if class_ is None else class_
        # Copying a CDT is tricky
        # See http://code.google.com/p/cgal-bindings/issues/detail?id=49
        newone = class_()
        newone.cdt = self.cdt.deepcopy()
        vmap = self.vertices_map_to_other_mesh(newone)
        for orig_vh, orig_info in self._input_vertices_infos.iteritems():
            dest_vh = vmap[orig_vh]
            dest_info = copy.deepcopy(orig_info) if deep else copy.copy(orig_info)
            newone._input_vertices_infos[dest_vh] = dest_info
        for (orig_va, orig_vb), orig_info in self._input_constraints_infos.iteritems():
            dest_va, dest_vb = vmap[orig_va], vmap[orig_vb]
            dest_info = copy.deepcopy(orig_info) if deep else copy.copy(orig_info)
            newone._input_constraints_infos[(dest_va, dest_vb)] = dest_info
        return newone

    def count_edges(self):
        count_edges, count_constrained = 0, 0
        for c in self.cdt.finite_edges():
            if self.cdt.is_constrained(c):
                count_constrained += 1
            count_edges += 1
        return count_edges, count_constrained

    def input_constraint_infos(self, (va, vb)):
        """Get the constraint informations associated to the given pair of vertices

        NB: Input constraints are represented as a pair of vertices,
        sorted in some arbitrary order and used as keys in
        _input_constraints_infos. This method ensure this normalisation is
        performed.

        """
        return self._input_constraints_infos[sorted_vertex_pair(va, vb)]

    def input_vertex_infos(self, v):
        return self._input_vertices_infos[v]

    def insert_constraint(self, va, vb, **kwargs):
        self.clear_caches()
        self.cdt.insert_constraint(va, vb)
        self._input_constraints_infos[sorted_vertex_pair(va, vb)] = self.EdgeInfo(**kwargs)
        return (va, vb) # Important to return the contrain in the input order

    def insert_point(self, point, **kwargs):
        self.clear_caches()
        point = to_cgal_point(point)
        vertex = self.cdt.insert(point)
        self._input_vertices_infos[vertex] = self.VertexInfo(**kwargs)
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
        """Reduce the list of information available on a vertices to a single one.

        This method works by calling repeatedly merge_function to
        merge two infos into a single one, starting with informatiosn
        drawn from init_map (or default constructed if init_map is not
        given or does not contains the vertex).

        """
        init_map = init_map or {}
        d = {}
        for v, info_list in self.fetch_constraint_infos_for_vertices(vertices=vertices).iteritems():
            info = self._input_vertices_infos.get(v, init_map.get(v, self.VertexInfo()))
            try:
                d[v] = reduce(merge_function, info_list, info)
            except InconsistentGeometricModel as exc:
                exc.witness_point = self.py_vertex(v)
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


class InfoWithIDsAndAltitude(object):
    ALTITUDE_TOLERANCE = 0.1

    def  __init__(self, altitude=UNSPECIFIED_ALTITUDE, id=None, **kwargs):
        self.ids = kwargs.pop('ids', set((id and [id]) or []))
        self.altitude = float(altitude)

    def merge_ids(self, other_info):
        ids = getattr(other_info, "ids", None)
        if ids is None: return
        self.ids.update(ids)

    def merge_with(self, other_info):
        self.merge_ids(other_info)
        self.merge_altitude(other_info)
        return self # so as to enable using reduce

    def __repr__(self):
        args = ", ".join(["%s=%r" % kv for kv in self.__dict__.iteritems()])
        return "%s(%s)" % (self.__class__.__name__, args)

    def __eq__(self, other):
        if isinstance(other, self.__class__): # XXX type(other) is type(self)
            return self.__dict__ == other.__dict__
        else:
            return False

    def __ne__(self, other):
        return not self.__eq__(other)

    def merge_altitude(self, other_info):
        alti = getattr(other_info, "altitude", UNSPECIFIED_ALTITUDE)
        if alti is not UNSPECIFIED_ALTITUDE:
            if self.altitude is UNSPECIFIED_ALTITUDE:
                self.altitude = alti
            else:
                delta = abs(alti - self.altitude)
                if delta > self.ALTITUDE_TOLERANCE:
                    raise InconsistentGeometricModel(
                        "Intersecting constraints with different altitudes",
                        ids=self.ids)


class ElevationMesh(MeshedCDTWithInfo):
    """ An elevation mesh associates an altitude to its vertices.

    This altitude can be unspecified (yet) and represented as UNSPECIFIED_ALTITUDE
    """

    VertexInfo = InfoWithIDsAndAltitude

    EdgeInfo = InfoWithIDsAndAltitude

    def __init__(self):
        super(ElevationMesh, self).__init__()
        self.vertices_info = defaultdict(self.VertexInfo)
        self._init_vertices_info_from_input()

    def _init_vertices_info_from_input(self):
        for vh, info in self._input_vertices_infos.iteritems():
            self.vertices_info[vh] = info # XXX or copy ?

    def copy(self, class_=None, deep=False):
        newone = super(ElevationMesh, self).copy(class_=class_, deep=deep)
        vmap = self.vertices_map_to_other_mesh(newone)
        for orig_vh, dest_vh in vmap.iteritems():
            orig_info = self.vertices_info[orig_vh]
            dest_info = copy.deepcopy(orig_info) if deep else copy.copy(orig_info)
            newone.vertices_info[dest_vh] = dest_info
        return newone

    def clear_caches(self):
        self.vertices_info.clear()
        self._init_vertices_info_from_input()

    def insert_point(self, point, **kwargs):
        vh = super(ElevationMesh, self).insert_point(point, **kwargs)
        self.vertices_info[vh] = self.input_vertex_infos(vh) # XXX or copy ?
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
                             for i in xrange(3)))
            return t

    def update_info_for_vertices(self, vertices=None, init_map=None):
        merge_function = lambda i1, i2: i1.merge_with(i2)
        d = self.merge_info_for_vertices(
            merge_function, vertices=vertices, init_map=self.vertices_info)
        self.vertices_info.update(d)

    def update_altitude_from_reference(self, reference):
        for vh in self.cdt.finite_vertices():
            try:
                input_altitude = self._input_vertices_infos[vh].altitude
            except KeyError:
                input_altitude = UNSPECIFIED_ALTITUDE
            if input_altitude is UNSPECIFIED_ALTITUDE:
                info = self.vertices_info[vh]
                info.altitude = reference(vh.point())


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

    def altitude_for_input_vertex(self, vh):
        alti = self.input_vertex_infos(vh).altitude
        assert alti is not UNSPECIFIED_ALTITUDE
        return alti

    def point_altitude(self, p, face_hint=None):
        p = to_cgal_point(p)
        fh, vh_or_i = self.locate_point(p, face_hint=face_hint)
        if fh is None: # point p is out of the convex hull of the triangulation
            return UNSPECIFIED_ALTITUDE
        if isinstance(vh_or_i, Vertex_handle): # point p is a vertex
            return self.altitude_for_input_vertex(vh_or_i)
        if isinstance(vh_or_i, int): # point is on an edge
            if self.cdt.is_infinite(fh): # get a finite face if needed
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
        assert abs((p3-p2).squared_length()-alti**2) < _PROXIMITY_THRESHOLD*alti
        return alti

    def copy_as_ElevationMesh(self):
        return self.copy(class_=ElevationMesh)
        for dest_vh in newone.cdt.finite_vertices():
            _, orig_vh = self.locate_point(dest_vh.point())
            assert isinstance(orig_vh, Vertex_handle)
            assert orig_vh.point() == dest_vh.point()
            orig_info = self.vertices_info[orig_vh]
            dest_info = newone.vertices_info[dest_vh]
            dest_info.merge_with(orig_info)


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
        for i in xrange(3):
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
        point until the frontier is emppty.  Then the next seed is
        added to the frontier if it was not yet visted and the process
        starts again until seeds are all considered.
        """
        for seed in seeds:
            if seed in self.visited:
                continue
            self.add_to_frontier(seed)
            while(self.frontier):
                self.visit_one()

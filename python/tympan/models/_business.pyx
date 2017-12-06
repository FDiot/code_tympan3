"""business models for Code_TYMPAN
"""

import cython as cy
from cython.operator cimport dereference as deref, preincrement as inc

from tympan.models cimport _common as tycommon
from tympan._business2solver cimport business2microsource


@cy.locals(elm=Element)
def elemen2receptor(elm):
    """ Convert an Element to a Receptor if possible """
    ptcalc = cy.declare(cy.pointer(TYPointCalcul),
                        downcast_point_calcul(elm.thisptr.getRealPointer()))
    assert ptcalc != NULL
    rec = Receptor()
    rec.thisptr = SmartPtr[TYPointCalcul](ptcalc)
    return rec

cdef pointcalcul2receptor(SmartPtr[TYPointCalcul] ptcalc):
    """Receptor cython object wrapping a SmartPtr[TYPointCalcul] (c++)"""
    rec = Receptor()
    rec.thisptr = ptcalc
    return rec

cdef typrojet2project(TYProjet* proj):
    """Project cython object wrapping a TYProjet (c++)"""
    project = Project()
    project.thisptr = SmartPtr[TYProjet](proj)
    return project

cdef make_computation():
    """ Create a new computation"""
    init_tympan_registry()
    comp = Computation()
    comp.thisptr = SmartPtr[TYCalcul](new TYCalcul())
    return comp

cdef make_typrojet():
    """ Attempt to build a typrojet from nothing """
    init_tympan_registry()
    project = Project()
    project.thisptr = SmartPtr[TYProjet](new TYProjet())
    return project

cdef tymateriauconstruction2material(SmartPtr[TYMateriauConstruction] mat):
    """Material cython object wrapping a SmartPtr[TYMateriauConstruction] (c++)
    """
    material = Material()
    material.thisptr = mat
    return material

cdef tysol2ground(SmartPtr[TYSol] grnd):
    """Ground python object wrapping a SmartPtr[TYSol] (c++)"""
    ground = Ground()
    ground.thisptr = grnd
    return ground

cdef tyelement_id(TYElement* elem):
    """The id of the element contained in the TYGeometryNode as a string"""
    return elem.getID().toString().toStdString().decode()

cdef tyelement_name(TYElement* elem):
    """The name of the element contained in the TYGeometryNode as a string"""
    str = elem.getName().toStdString()
    try:
        return str.decode()
    except UnicodeDecodeError:
        return str.decode('cp1252')

cdef cpp2cypoints(vector[TYPoint] cpp_points):
    """Build a list of 'Point3D' objects from the c++ 'TYPoint' objects
    """
    points = []
    for i in xrange(cpp_points.size()):
            cy_point = cy.declare(tycommon.Point3D, tycommon.opoint3d2point3d(cpp_points[i]))
            points.append(cy_point)
    return points

cdef cpp2cypoints_global(vector[TYPoint] cpp_points, tycommon.OMatrix matrix):
    """Build a list of 'Point3D' objects from the c++ 'TYPoint' objects

    Convert them to a global scale with the transform matrix 'matrix'
    """
    points = []
    for i in xrange(cpp_points.size()):
        cpp_point = cy.declare(tycommon.OPoint3D, tycommon.dot(matrix, cpp_points[i]))
        cy_point = cy.declare(tycommon.Point3D, tycommon.opoint3d2point3d(cpp_point))
        points.append(cy_point)
    return points

cdef TYPoint cypoint2cpp(cy_point):
    """ Build a TYPoint from a Point3D object """
    cpp_point = cy.declare(TYPoint)
    cpp_point._x = cy_point.x
    cpp_point._y = cy_point.y
    cpp_point._z = cy_point.z
    return cpp_point


cdef vector[TYPoint] cypoints2cpp(cy_points):
    """ Build a list of TYPoint from Point3D objects """
    cpp_points = cy.declare(vector[TYPoint])
    cpp_point = cy.declare(TYPoint)
    for point in cy_points :
        cpp_point._x = point.x
        cpp_point._y = point.y
        cpp_point._z = point.z
        cpp_points.push_back(cpp_point)
    return cpp_points


ctypedef TYElement* (*downcast_func_type)(TYElement *)

cdef TYElement* get_volume(TYElement * elem):
    """Try and downcast `elem` into a TYAcousticVolume. Return an TYAcousticVolume if it succeeds,
    else NULL.
    """
    volume = cy.declare(cy.pointer(TYAcousticVolume), downcast_acoustic_volume(elem))
    return volume

cdef TYElement* get_surface_node(TYElement * elem):
    """Try and downcast `elem` into a TYAcousticSurfaceNode. Return an TYAcousticSurfaceNode if it
    succeeds, else NULL.
    """
    surface_node = cy.declare(cy.pointer(TYAcousticSurfaceNode),
                              downcast_acoustic_surface_node(elem))
    return surface_node

cdef find_parent_id(TYElement * elem, downcast_func_type func):
    """Find the identifier of the parent of the element `elem` corresponding to the downcast function
    `func`
    """
    dest = cy.declare(cy.pointer(TYElement), NULL)
    while dest == NULL:
        elem = elem.getParent()
        if elem == NULL:
            return
        dest = func(elem)
    return tyelement_id(dest)

cdef find_parent_name(TYElement * elem, downcast_func_type func):
    """Find the name of the parent of the element `elem` corresponding to the downcast function
    `func`
    """
    dest = cy.declare(cy.pointer(TYElement), NULL)
    while dest == NULL:
        elem = elem.getParent()
        if elem == NULL:
            return
        dest = func(elem)
    return tyelement_name(dest)

cdef find_volume_id(TYElement * elem):
    """Find the identifier of the volume containing the element
    """
    acoustic_volume_id = find_parent_id(elem, get_volume)
    return acoustic_volume_id

cdef find_volume_name(TYElement * elem):
    """Find the name of the volume containing the element
    """
    acoustic_volume_name = find_parent_name(elem, get_volume)
    return acoustic_volume_name

cdef find_surface_node_id(TYElement * elem):
    """Find the identifier of the surface node containing the element
    """
    acoustic_surface_node_id = find_parent_id(elem, get_surface_node)
    return acoustic_surface_node_id

cdef find_surface_node_name(TYElement * elem):
    """Find the name of the surface node containing the element
    """
    acoustic_surface_node_name = find_parent_name(elem, get_surface_node)
    return acoustic_surface_node_name

def init_tympan_registry():
    """Trigger the registration of Tympan business classes (TY* classes).

    It is necessary to do it before playing with Tympan library (just after
    cython libraries import)
    """
    init_registry()


cdef class Element:
    thisptr = cy.declare(SmartPtr[TYElement])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYElement]()

    @property
    def name(self):
        """The name of the element"""
        assert self.thisptr.getRealPointer() != NULL
        str = self.thisptr.getRealPointer().getName().toStdString()
        try:
            return str.decode()
        except UnicodeDecodeError:
            return str.decode('cp1252')

    def setName(self, name):
        """Set the name of the element"""
        assert self.thisptr.getRealPointer() != NULL
        self.thisptr.getRealPointer().setName(name.encode('utf-8'))

    def getParent(self):
        """Return element parent if any """
        assert self.thisptr.getRealPointer() != NULL
        cpp_elem = self.thisptr.getRealPointer().getParent()
        elem = Element()
        elem.thisptr = SmartPtr[TYElement](cpp_elem)
        return elem

cdef class Element_array:
    thisptr = cy.declare(vector[SmartPtr[TYElement]])

    def __cinit__(self):
        self.thisptr = vector[SmartPtr[TYElement]]()

    def from_xml(self, file_name):
        # To avoid Cython error message "Obtaining 'char const *' from temporary Python value"
        # See http://cython.readthedocs.io/en/latest/src/userguide/language_basics.html#caveats-when-using-a-python-string-in-a-c-context
        pystring = file_name.encode('utf-8')
        self.thisptr = load_elements(pystring)

    @property
    def user_sources(self):
        ret = []
        cpp_src = cy.declare(cy.pointer(TYUserSourcePonctuelle))
        for elm in self.thisptr:
            cpp_src = downcast_user_source_ponctuelle(elm.getRealPointer())
            if cpp_src != NULL :
                cy_source = User_source()
                cy_source.thisptr = SmartPtr[TYUserSourcePonctuelle](cpp_src)
                ret.append(cy_source)
        return ret

    @property
    def engines(self):
        ret = []
        cpp_engine = cy.declare(cy.pointer(TYMachine))
        for elm in self.thisptr:
            cpp_engine = downcast_machine(elm.getRealPointer())
            if cpp_engine != NULL :
                cy_engine = Engine()
                cy_engine.thisptr = SmartPtr[TYMachine](cpp_engine)
                ret.append(cy_engine)
        return ret

    @property
    def buildings(self):
        ret = []
        cpp_building = cy.declare(cy.pointer(TYBatiment))
        for elm in self.thisptr:
            cpp_building = downcast_batiment(elm.getRealPointer())
            if cpp_building != NULL :
                cy_building = Building()
                cy_building.thisptr = SmartPtr[TYBatiment](cpp_building)
                ret.append(cy_building)
        return ret

    @property
    def sites(self):
        ret = []
        cpp_site = cy.declare(cy.pointer(TYSiteNode))
        for elm in self.thisptr:
            cpp_site = downcast_sitenode(elm.getRealPointer())
            if cpp_site != NULL :
                cy_site = Site()
                cy_site.thisptr = SmartPtr[TYSiteNode](cpp_site)
                ret.append(cy_site)
        return ret

cdef class AcousticSurface:
    thisptr = cy.declare(cy.pointer(TYAcousticSurface))
    thisgeonodeptr = cy.declare(SmartPtr[TYGeometryNode])

    def __cinit__(self):
        self.thisptr = NULL

    def export_mesh(self):
        """The mesh covering the acoustic surface

        Retrieve the points and triangles constituting the acoustic surface
        and return them as list of 'Point3D' and a list of 'Triangle' cython
        objects
        """
        assert self.thisptr != NULL
        pts = cy.declare(deque[tycommon.OPoint3D])
        tgles = cy.declare(deque[tycommon.OTriangle])
        # Use the triangulating interface of TYSurfaceInterface to get triangles
        # and convert them to Nodes and AcousticTriangles (beware of mapping
        # TYPoints to Node in the correct way.)
        # !! Here we must not give exportMesh the geonode face_list[i] but
        # build a new one to avoid constructing the triangles in a local
        # scale
        self.thisptr.exportMesh(pts, tgles, TYGeometryNode(self.thisptr))
        points = []
        itp = cy.declare(deque[tycommon.OPoint3D].iterator, pts.begin())
        while itp != pts.end():
            points.append(tycommon.opoint3d2point3d(deref(itp)))
            inc(itp)
        triangles = []
        itt = cy.declare(deque[tycommon.OTriangle].iterator, tgles.begin())
        while itt != tgles.end():
            # Assert consistency of the tycommon.OPoint3D given in the mesh
            check = deref(itt).checkConsistencyWrtPointsTab(pts)
            if not check:
                raise RuntimeError(deref(itt).reportInconsistencyWrtPointsTab(pts))
            tri = cy.declare(cy.pointer(tycommon.OTriangle),
                             new tycommon.OTriangle(deref(itt)._p1, deref(itt)._p2, deref(itt)._p3))
            triangles.append(tycommon.otriangle2triangle(tri))
            inc(itt)
        return (points, triangles)

    @property
    def material(self):
        """The material the surface is made of, as a 'Material' cython object
        """
        return tymateriauconstruction2material(self.thisptr.getMateriau())

    def volume_id(self):
        """Find the identifier of the volume containing the acoustic surface
        """
        return find_volume_id(self.thisptr)

    def volume_name(self):
        """Find the name of the volume containing the acoustic surface
        """
        return find_volume_name(self.thisptr)

    def surface_node_id(self):
        """Find the identifier of the surface node
        """
        return find_surface_node_id(self.thisptr)

    def surface_node_name(self):
        """Find the name of the surface node
        """
        return find_surface_node_name(self.thisptr)

    def element_name(self):
        """Return the infrastructure element name"""
        elem = Element()
        elem = self.getParent().getParent()
        if self.surface_node_name() == None:
            return elem.name
        else:
            return elem.getParent().name

    @property
    def getIsRayonnant(self):
        return self.thisptr.getIsRayonnant()

    def getParent(self):
        """Return element parent if any """
        cpp_elem = self.thisptr.getParent()
        elem = Element()
        elem.thisptr = SmartPtr[TYElement](cpp_elem)
        return elem

cdef class Material:

    @property
    def spectrum(self):
        """The spectrum of the material as a 'Spectrum' cython object
        """
        assert self.thisptr.getRealPointer() != NULL
        return tycommon.ospectre2spectrum(self.thisptr.getRealPointer().getSpectreAbso())

    @property
    def name(self):
        """The name of the element"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getName().toStdString()


cdef class Ground:

    @property
    def resistivity(self):
        """Ground resistivity (floating point value)"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getResistivite()

    @property
    def width(self):
        """Ground width (floating point value)"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getEpaisseur()

    @property
    def deviation(self):
        """Ground deviation (floating point value)"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getEcartType()

    @property
    def length(self):
        """Ground autocorrelation length (floating point value)"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getLongueur()

    def name(self):
        """The name of the element"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getName().toStdString().decode()

    @property
    def elem_id(self):
        """Ground id"""
        return tyelement_id(self.thisptr.getRealPointer())


cdef class Vegetation:

    @property
    def height(self):
        """The vegetation height"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getHauteur()

    def name(self):
        """The name of the element"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getName().toStdString().decode()

    @property
    def foliage(self):
        """Does the vegetation has foliage?"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getFoliageStatus()

    @property
    def elem_id(self):
        """Vegetation id"""
        return tyelement_id(self.thisptr.getRealPointer())


cdef class User_source:

    def __cinit__(self):
        self.thisgeonodeptr = SmartPtr[TYGeometryNode] (new TYGeometryNode())

    @property
    def getID(self):
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getID().toString().toStdString().decode()

    @property
    def name(self):
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getName().toStdString().decode()

    @cy.locals(pos=tycommon.Point3D)
    def set_position(self, pos):
        assert self.thisptr.getRealPointer() != NULL
        self.thisptr.getRealPointer().setPos(SmartPtr[TYPoint](new TYPoint(cypoint2cpp(pos))))

    @property
    def position(self):
        assert self.thisptr.getRealPointer() != NULL
        smartptr_typoint = cy.declare(SmartPtr[TYPoint],
                             self.thisptr.getRealPointer().getPos())
        ptr_opoint3D = cy.declare(cy.pointer(tycommon.OPoint3D),
                                  smartptr_typoint.getRealPointer().downcast_opoint())
        cpp_pos = cy.declare(tycommon.OPoint3D, deref(ptr_opoint3D))
        return tycommon.opoint3d2point3d(cpp_pos)

    def set_hauteur(self, hauteur):
        assert self.thisptr.getRealPointer() != NULL
        self.thisptr.getRealPointer().setHauteur(hauteur)

    @property
    def hauteur(self):
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getHauteur()


cdef class Engine:

    def __cinit__(self):
        self.thisgeonodeptr = SmartPtr[TYGeometryNode] (new TYGeometryNode())

    @property
    def getID(self):
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getID().toString().toStdString()

    @property
    def name(self):
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getName().toStdString().decode()

    def setName(self, name):
        assert self.thisptr.getRealPointer() != NULL
        self.thisptr.getRealPointer().setName(name.encode('utf-8'))

    @cy.locals(pos=tycommon.Point3D)
    def set_position(self, pos):
        assert self.thisgeonodeptr.getRealPointer() != NULL
        self.thisgeonodeptr.getRealPointer().setPosition(cypoint2cpp(pos))

    @property
    def position(self):
        assert self.thisgeonodeptr.getRealPointer() != NULL
        cpp_pos = cy.declare(tycommon.OPoint3D,
                             self.thisgeonodeptr.getRealPointer().position())
        return tycommon.opoint3d2point3d(cpp_pos)

    @cy.locals(rot=tycommon.Point3D)
    def set_rotation(self, rot):
        assert self.thisgeonodeptr.getRealPointer() != NULL
        self.thisgeonodeptr.getRealPointer().setPosition(cypoint2cpp(rot))

    @property
    def rotation(self):
        assert self.thisgeonodeptr.getRealPointer() != NULL
        cpp_rot = cy.declare(tycommon.OPoint3D,
                             self.thisgeonodeptr.getRealPointer().rotation())
        return tycommon.opoint3d2point3d(cpp_rot)

    def set_hauteur(self, hauteur):
        assert self.thisgeonodeptr.getRealPointer() != NULL
        self.thisgeonodeptr.getRealPointer().setHauteur(hauteur)

    @property
    def hauteur(self):
        assert self.thisgeonodeptr.getRealPointer() != NULL
        return self.thisgeonodeptr.getRealPointer().getHauteur()

    @property
    def sommets(self):
        """The sequence of vertexes forming the engine ('Point3D' objects)
        """
        cpp_points = cy.declare(vector[TYPoint], self.thisptr.getRealPointer().sommets())
        return cpp2cypoints(cpp_points)


cdef class Building:

    def __cinit__(self):
        self.thisgeonodeptr = SmartPtr[TYGeometryNode] (new TYGeometryNode())

    @property
    def name(self):
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getName().toStdString().decode()

    def setName(self, name):
        assert self.thisptr.getRealPointer() != NULL
        self.thisptr.getRealPointer().setName(name.encode('utf-8'))

    @cy.locals(pos=tycommon.Point3D)
    def set_position(self, pos):
        assert self.thisgeonodeptr.getRealPointer() != NULL
        self.thisgeonodeptr.getRealPointer().setPosition(cypoint2cpp(pos))

    @property
    def position(self):
        assert self.thisgeonodeptr.getRealPointer() != NULL
        cpp_pos = cy.declare(tycommon.OPoint3D,
                             self.thisgeonodeptr.getRealPointer().position())
        return tycommon.opoint3d2point3d(cpp_pos)

    @cy.locals(rot=tycommon.Point3D)
    def set_rotation(self, rot):
        assert self.thisgeonodeptr.getRealPointer() != NULL
        self.thisgeonodeptr.getRealPointer().setPosition(cypoint2cpp(rot))

    @property
    def rotation(self):
        assert self.thisgeonodeptr.getRealPointer() != NULL
        cpp_rot = cy.declare(tycommon.OPoint3D,
                             self.thisgeonodeptr.getRealPointer().rotation())
        return tycommon.opoint3d2point3d(cpp_rot)

    def set_hauteur(self, hauteur):
        assert self.thisgeonodeptr.getRealPointer() != NULL
        self.thisgeonodeptr.getRealPointer().setHauteur(hauteur)

    @property
    def hauteur(self):
        assert self.thisgeonodeptr.getRealPointer() != NULL
        return self.thisgeonodeptr.getRealPointer().getHauteur()

    @property
    def getIsRayonnant(self):
        return self.thisptr.getRealPointer().getIsRayonnant()

    @property
    def sommets(self):
        """The sequence of vertexes forming the building ('Point3D' objects)
        """
        cpp_points = cy.declare(vector[TYPoint], self.thisptr.getRealPointer().sommets())
        return cpp2cypoints(cpp_points)

cdef class Site:

    @property
    def elem_id(self):
        """SiteNode id"""
        return tyelement_id(self.thisptr.getRealPointer())

    @property
    def name(self):
        """SiteNode name"""
        return tyelement_name(self.thisptr.getRealPointer())

    @property
    def subsites(self):
        """List of the site subsites as 'Site' cython objects"""
        subsite_geonodes = cy.declare(vector[SmartPtr[TYGeometryNode]],
                                      self.thisptr.getRealPointer().getListSiteNode())
        subsites = []
        for i in xrange(subsite_geonodes.size()):
            cpp_site = cy.declare(
                cy.pointer(TYSiteNode),
                downcast_sitenode(subsite_geonodes[i].getRealPointer().getElement()))
            site = Site()
            site.matrix = self.matrix.dot(subsite_geonodes[i].getRealPointer().getMatrix())
            site.thisptr = SmartPtr[TYSiteNode](cpp_site)
            subsites.append(site)
        return subsites

    @property
    def outdated_elements(self):
        """The list of the infrastructure elements that weren't updated

        Return a list of tuples (elt_id, elt_name)
        """
        infra = cy.declare(cy.pointer(TYInfrastructure),
                           self.thisptr.getRealPointer().getInfrastructure().getRealPointer())
        outdated = cy.declare(vector[SmartPtr[TYElement]], infra.getTabElemNOk())
        nb_outdated = outdated.size()
        outdated_info = []
        for i in xrange(nb_outdated):
            elt = cy.declare(cy.pointer(TYElement), outdated[i].getRealPointer())
            outdated_info.append((elt.getID().toString().toStdString(),
                                  elt.getName().toStdString()))
        for subsite in self.subsites:
            outdated_info.extend(subsite.outdated_elements)
        return outdated_info

    @property
    def project(self):
        """Parent project of the site as a 'Project' cython object
        """
        return typrojet2project(self.thisptr.getRealPointer().getProjet())

    @property
    def ground_contour(self):
        """The ground contour of the infrastructure in a global scale

        As a dict {volume node id: list of 'Point3D' cython object}
        """
        cpp_contours = cy.declare(cppmap[OGenID, deque[deque[tycommon.OPoint3D]]])
        cpp_contours_iter = cy.declare(cppmap[OGenID, deque[deque[tycommon.OPoint3D]]].iterator)
        self.thisptr.getRealPointer().getFacesOnGround(cpp_contours)
        cpp_contours_iter = cpp_contours.begin()
        contours = {}
        for it in cpp_contours:
            volume_id = it.first.toString().toStdString().decode()
            contours[volume_id] = [
                [tycommon.opoint3d2point3d(tycommon.dot(self.matrix, contour[i]))
                 for i in xrange(contour.size())]
                for contour in it.second
            ]
        return contours

    @property
    def acoustic_surfaces(self):
        """The list of the acoustic surfaces of the site"""
        assert self.thisptr.getRealPointer() != NULL
        is_screen_face_idx = cy.declare(vector[bool])
        face_list = cy.declare(vector[SmartPtr[TYGeometryNode]])
        nb_building_faces = cy.declare(cy.uint, 0)
        self.thisptr.getRealPointer().getListFaces(face_list, nb_building_faces,
                                                   is_screen_face_idx)
        surfaces = []
        for i in xrange(nb_building_faces):
            surf = AcousticSurface()
            pelt = cy.declare(cy.pointer(TYElement), face_list[i].getRealPointer().getElement())
            surf.thisptr = downcast_acoustic_surface(pelt)
            surf.thisgeonodeptr = face_list[i]
            # 'face_list' can contain topography elements. Not relevant here.
            if surf.thisptr == NULL:
                continue
            surfaces.append(surf)
        return surfaces

    def export_topo_mesh(self):
        """Retrieve the mesh of the site topography

        3 lists are returned:
            - 'points' contains 'Point3D' cython objects
            - 'triangles' contains 'Triangle' cython objects
            - 'ground' contains 'Ground' cython objects
        """
        assert self.thisptr.getRealPointer() != NULL
        pts = cy.declare(deque[tycommon.OPoint3D])
        tgles = cy.declare(deque[tycommon.OTriangle])
        mats = cy.declare(deque[SmartPtr[TYSol]])
        ptopo = cy.declare(cy.pointer(TYTopographie),
                           self.thisptr.getRealPointer().getTopographie().getRealPointer())
        # Retrieve topography mesh as c++ objects
        ptopo.exportMesh(pts, tgles, mats)
        # Retrieve points
        points = []
        itp = cy.declare(deque[tycommon.OPoint3D].iterator)
        itp = pts.begin()
        while itp != pts.end():
            points.append(tycommon.opoint3d2point3d(deref(itp)))
            inc(itp)
        # Retrieve triangles
        triangles = []
        itt = cy.declare(deque[tycommon.OTriangle].iterator, tgles.begin())
        while itt != tgles.end():
            # Assert consistency of the tycommon.OPoint3D given in the mesh
            check = deref(itt).checkConsistencyWrtPointsTab(pts)
            if not check:
                raise RuntimeError(deref(itt).reportInconsistencyWrtPointsTab(pts))
            tri = cy.declare(cy.pointer(tycommon.OTriangle),
                             new tycommon.OTriangle(deref(itt)._p1, deref(itt)._p2, deref(itt)._p3))
            triangles.append(tycommon.otriangle2triangle(tri))
            inc(itt)
        # Retrieve ground materials
        grounds = []
        itg = cy.declare(deque[SmartPtr[TYSol]].iterator, mats.begin())
        while itg != mats.end():
            grounds.append(tysol2ground(deref(itg)))
            inc(itg)
        return (points, triangles, grounds)

    @cy.locals(comp=Computation)
    def fetch_sources(self, comp):
        """Retrieve acoustic sources (macro and micro) from business infrastructure

        Return them as a 'Business2MicroSource' containing a map linking macro
        sources (ie business sources like buildings or machines) to the
        corresponding lists of micro sources.
        """
        assert self.thisptr.getRealPointer() != NULL
        map_elt_srcs = cy.declare(cppmap[TYElem_ptr, vector[SmartPtr[TYGeometryNode]]])
        infra = cy.declare(cy.pointer(TYInfrastructure),
                           self.thisptr.getRealPointer().getInfrastructure().getRealPointer())
        infra.getAllSrcs(comp.thisptr.getRealPointer(), map_elt_srcs)
        return business2microsource(map_elt_srcs)

    def childs(self):
        """The list of the direct childs of the Site (not recursive)
        """
        assert self.thisptr.getRealPointer() != NULL
        childs = cy.declare(vector[SmartPtr[TYElement]])
        # Retrieves the childs of the site (LPTYElements) into the "childs" vector.
        self.thisptr.getRealPointer().getChilds(childs, False)
        # Constructs a python list and appends to it the LPTYElement objects
        # previously wrapped into Elements objects
        pylist = []
        for i in xrange(childs.size()):
            child = Element()
            child.thisptr = childs[i]
            pylist.append(child)
        return pylist

    def update_altimetry(self, altimetry_mesh, material_by_face):
        """Set altimetry mesh back to TYAltimetry, update infrastructure and receptors
        """
        # This method actually leads to the dynamic allocation of an array of
        # TYGeometryNode that will be accessed by some other method(s) later on.
        # Not calling this method leads to a segmentation fault since the array
        # doesn't exist then.
        self.thisptr.getRealPointer().getTopographie().getRealPointer().sortTerrainsBySurface()
        pts = cy.declare(deque[tycommon.OPoint3D])
        tgles = cy.declare(deque[tycommon.OTriangle])
        mat_ids = cy.declare(deque[string])
        vertices, faces = altimetry_mesh.as_arrays()
        for x, y, z in vertices:
            pts.push_back(tycommon.OPoint3D(x, y, z))
        for e0, e1, e2 in faces:
            tgles.push_back(tycommon.OTriangle(e0, e1, e2))
            tgles.back()._A = pts[e0]
            tgles.back()._B = pts[e1]
            tgles.back()._C = pts[e2]
        for mat in altimetry_mesh.faces_material(material_by_face):
            mat_ids.push_back(mat.id.encode('utf-8'))
        cppmats = cy.declare(deque[SmartPtr[TYSol]])
        self.thisptr.getRealPointer().uuid2tysol(mat_ids, cppmats)
        alti = cy.declare(
            SmartPtr[TYAltimetrie],
            self.thisptr.getRealPointer().getTopographie().getRealPointer().getAltimetrie())
        alti.getRealPointer().plugBackTriangulation(pts, tgles, cppmats)
        # Recursively update the acoustic, the altimetry of the infrastructure
        # elements and the altimetry of the acoustic receptors
        self.thisptr.getRealPointer().update(True)

    def process_landtake(self):
        """Sequence of points ('Point3D' objects) delimiting the site landtake

        If 'use_landtake_as_level_curve' is true or if no level
        curves are defined for the site, the landtake will be used as a level
        curve and a 'LevelCurve' cython object will be returned as well
        """
        topo = cy.declare(cy.pointer(TYTopographie),
                          self.thisptr.getRealPointer().getTopographie().getRealPointer())
        cpp_points = cy.declare(vector[TYPoint], topo.getEmprise())
        points = cpp2cypoints_global(cpp_points, self.matrix)
        cpp_lcurves = cy.declare(vector[SmartPtr[TYGeometryNode]], topo.getListCrbNiv())
        make_level_curve = self.thisptr.getRealPointer().getUseEmpriseAsCrbNiv()
        if make_level_curve or cpp_lcurves.empty():
            alti = self.thisptr.getRealPointer().getAltiEmprise()
            level_curve = LevelCurve()
            level_curve.thisptr = new TYCourbeNiveau(cpp_points, alti)
            level_curve.matrix = self.matrix
            level_curve.altitude_set = True
            level_curve._altitude = alti
            return (points, level_curve)
        return (points, None)

    def add_levelcurve(self, points, alti):
        """ Allow to add a level curve in a site from PYTHON """
        cpp_points = cy.declare(vector[TYPoint])
        cpp_points = cypoints2cpp(points)
        topo = cy.declare(cy.pointer(TYTopographie))
        topo = self.thisptr.getRealPointer().getTopographie().getRealPointer()
        cpp_lcurve = cy.declare(SmartPtr[TYCourbeNiveau])
        cpp_lcurve = SmartPtr[TYCourbeNiveau]( new TYCourbeNiveau(cpp_points, alti) )
        topo.addCrbNiv(cpp_lcurve)

    @cy.locals(curve=LevelCurve)
    def delete_levelcurve(self, curve):
        cpp_lcurve = cy.declare(SmartPtr[TYCourbeNiveau],
                                SmartPtr[TYCourbeNiveau](curve.thisptr))
        topo = cy.declare(cy.pointer(TYTopographie))
        topo = self.thisptr.getRealPointer().getTopographie().getRealPointer()
        topo.remCrbNiv(cpp_lcurve)


    @cy.locals(source=User_source, position=tycommon.Point3D)
    def add_user_source(self, source, position, height):
        cpp_point = cy.declare(SmartPtr[TYPoint])
        cpp_point = SmartPtr[TYPoint](new TYPoint(cypoint2cpp(position)))
        source_copy = cy.declare(cy.pointer(TYUserSourcePonctuelle), new TYUserSourcePonctuelle())
        source_copy.deepCopy(source.thisptr.getRealPointer(), False)
        source_copy.setPos(cpp_point)
        source_copy.setHauteur(height)
        geonode = cy.declare(SmartPtr[TYGeometryNode],
                             SmartPtr[TYGeometryNode](new TYGeometryNode()))
        geonode.getRealPointer().setElement(source_copy)
        infra = cy.declare(cy.pointer(TYInfrastructure),
                           self.thisptr.getRealPointer().getInfrastructure().getRealPointer())
        infra.addSrc(geonode)

    @property
    def user_sources(self):
        infra = cy.declare(cy.pointer(TYInfrastructure),
                           self.thisptr.getRealPointer().getInfrastructure().getRealPointer())
        cpp_tab = cy.declare(vector[SmartPtr[TYGeometryNode]],
                             infra.getSrcs())
        cy_sources = []
        cpp_source_node = cy.declare(SmartPtr[TYGeometryNode])
        for cpp_source_node in cpp_tab:
            src_ptr = cy.declare(cy.pointer(TYUserSourcePonctuelle))
            src_ptr = downcast_user_source_ponctuelle(
                                cpp_source_node.getRealPointer().getElement())
            cy_source = User_source()
            cy_source.thisptr = SmartPtr[TYUserSourcePonctuelle](src_ptr)
            cy_source.thisgeonodeptr = cpp_source_node
            cy_sources.append(cy_source)
        return cy_sources

    @property
    def engines(self):
        infra = cy.declare(cy.pointer(TYInfrastructure),
                           self.thisptr.getRealPointer().getInfrastructure().getRealPointer())
        cpp_tab = cy.declare(vector[SmartPtr[TYGeometryNode]],
                             infra.getListMachine())
        cy_sources = []
        cpp_source_node = cy.declare(SmartPtr[TYGeometryNode])
        for cpp_source_node in cpp_tab:
            src_ptr = cy.declare(cy.pointer(TYMachine))
            src_ptr = downcast_machine(
                                cpp_source_node.getRealPointer().getElement())
            cy_source = Engine()
            cy_source.thisptr = SmartPtr[TYMachine](src_ptr)
            cy_source.thisgeonodeptr = cpp_source_node
            cy_sources.append(cy_source)
        return cy_sources

    @property
    def buildings(self):
        infra = cy.declare(cy.pointer(TYInfrastructure),
                           self.thisptr.getRealPointer().getInfrastructure().getRealPointer())
        cpp_tab = cy.declare(vector[SmartPtr[TYGeometryNode]],
                             infra.getListBatiment())
        cy_sources = []
        cpp_source_node = cy.declare(SmartPtr[TYGeometryNode])
        for cpp_source_node in cpp_tab:
            src_ptr = cy.declare(cy.pointer(TYBatiment))
            src_ptr = downcast_batiment(
                                cpp_source_node.getRealPointer().getElement())
            cy_source = Building()
            cy_source.thisptr = SmartPtr[TYBatiment](src_ptr)
            cy_source.thisgeonodeptr = cpp_source_node
            cy_sources.append(cy_source)
        return cy_sources

    @cy.locals(engine=Engine, position=tycommon.Point3D, rotation=tycommon.Point3D)
    def add_engine(self, engine, position, rotation, height):
        engine_copy = cy.declare(cy.pointer(TYMachine), new TYMachine())
        engine_copy.deepCopy(engine.thisptr.getRealPointer(), False)
        geonode = cy.declare(SmartPtr[TYGeometryNode],
                             SmartPtr[TYGeometryNode](new TYGeometryNode()))
        geonode.getRealPointer().setElement(engine_copy)
        geonode.getRealPointer().setPosition(position.thisobj)
        geonode.getRealPointer().setRotation(rotation.thisobj)
        geonode.getRealPointer().setHauteur(height)
        infra = cy.declare(cy.pointer(TYInfrastructure),
                           self.thisptr.getRealPointer().getInfrastructure().getRealPointer())
        infra.addMachine(geonode)

    @cy.locals(building=Building, position=tycommon.Point3D, rotation=tycommon.Point3D)
    def add_building(self, building, position, rotation, height):
        building_copy = cy.declare(cy.pointer(TYBatiment), new TYBatiment())
        building_copy.deepCopy(building.thisptr.getRealPointer(), False)
        geonode = cy.declare(SmartPtr[TYGeometryNode],
                             SmartPtr[TYGeometryNode](new TYGeometryNode()))
        geonode.getRealPointer().setElement(building_copy)
        geonode.getRealPointer().setPosition(position.thisobj)
        geonode.getRealPointer().setRotation(rotation.thisobj)
        geonode.getRealPointer().setHauteur(height)
        infra = cy.declare(cy.pointer(TYInfrastructure),
                           self.thisptr.getRealPointer().getInfrastructure().getRealPointer())
        infra.addBatiment(geonode)

    @cy.locals(site=Site, position=tycommon.Point3D, rotation=tycommon.Point3D)
    def add_sub_site(self, site, position, rotation):
        site_copy = cy.declare(cy.pointer(TYSiteNode), new TYSiteNode())
        site_copy.deepCopy(site.thisptr.getRealPointer(), False)
        geonode = cy.declare(SmartPtr[TYGeometryNode],
                             SmartPtr[TYGeometryNode](new TYGeometryNode()))
        geonode.getRealPointer().setElement(site_copy)
        geonode.getRealPointer().setPosition(position.thisobj)
        geonode.getRealPointer().setRotation(rotation.thisobj)
        self.thisptr.getRealPointer().addSiteNode(geonode)

    def set_landtake(self, points, useAsLevelCurve=True, alti=0.):
        cpp_points = cy.declare(vector[TYPoint])
        cpp_points = cypoints2cpp(points)
        topo = cy.declare(cy.pointer(TYTopographie))
        topo = self.thisptr.getRealPointer().getTopographie().getRealPointer()
        topo.setEmprise(cpp_points, True)
        self.thisptr.getRealPointer().setUseEmpriseAsCrbNiv(useAsLevelCurve)
        self.thisptr.getRealPointer().setAltiEmprise(alti)

    @property
    def lakes(self):
        """The lakes of the topography as a list of 'Lake' cython objects"""
        lakes = []
        material_areas = []
        topo = cy.declare(cy.pointer(TYTopographie),
                          self.thisptr.getRealPointer().getTopographie().getRealPointer())
        cpp_lakes = cy.declare(vector[SmartPtr[TYGeometryNode]], topo.getListPlanEau())
        for i in xrange(cpp_lakes.size()):
            cpp_lake = cy.declare(cy.pointer(TYPlanEau),
                                  downcast_plan_eau(cpp_lakes[i].getRealPointer().getElement()))
            lake = Lake()
            lake.thisptr = cpp_lake
            lake.matrix = self.matrix.dot(cpp_lakes[i].getRealPointer().getMatrix())
            lake.thisgeonodeptr = cpp_lakes[i]
            lakes.append(lake)
        return lakes

    @property
    def material_areas(self):
        """The material areas of the topography as 'MaterialArea' cython objects"""
        mareas = []
        cpp_mat_areas = cy.declare(vector[SmartPtr[TYGeometryNode]])
        topo = cy.declare(cy.pointer(TYTopographie),
                          self.thisptr.getRealPointer().getTopographie().getRealPointer())
        cpp_mat_areas = topo.getListTerrain()
        for i in xrange (cpp_mat_areas.size()):
            cpp_mat_area = cy.declare(cy.pointer(TYTerrain), downcast_terrain(
                cpp_mat_areas[i].getRealPointer().getElement()))
            area = MaterialArea()
            area.thisptr = cpp_mat_area
            area.matrix = self.matrix.dot(cpp_mat_areas[i].getRealPointer().getMatrix())
            area.thisgeonodeptr = cpp_mat_areas[i]
            mareas.append(area)
        return mareas

    @property
    def level_curves(self):
        """The level curves of the topography (list of 'LevelCurve' objects)
        """
        lcurves = []
        cpp_lcurves = cy.declare(vector[SmartPtr[TYGeometryNode]])
        topo = cy.declare(cy.pointer(TYTopographie),
                          self.thisptr.getRealPointer().getTopographie().getRealPointer())
        cpp_lcurves = topo.getListCrbNiv()
        for i in xrange (cpp_lcurves.size()):
            cpp_lcurve = cy.declare(cy.pointer(TYCourbeNiveau), downcast_courbe_niveau(
                cpp_lcurves[i].getRealPointer().getElement()))
            lcurve = LevelCurve()
            lcurve.thisptr = cpp_lcurve
            lcurve.matrix = self.matrix.dot(cpp_lcurves[i].getRealPointer().getMatrix())
            lcurve.thisgeonodeptr = cpp_lcurves[i]
            lcurves.append(lcurve)
        return lcurves

    @cy.locals(pos=tycommon.Point3D)
    def set_position(self, pos):
        mat = cy.declare(tycommon.OMatrix, self.matrix)
        cpp_geonode = cy.declare(TYGeometryNode)
        cpp_geonode.setMatrix(mat)
        cpp_pos = cy.declare(tycommon.OPoint3D, tycommon.cypoint2cpp(pos))
        cpp_geonode.setPosition(cpp_pos)
        self.matrix = cpp_geonode.getMatrix()

    @property
    def position(self):
        cpp_geonode = cy.declare(TYGeometryNode)
        cpp_geonode.setMatrix(self.matrix)
        cpp_pos = cy.declare(tycommon.OPoint3D,
                             cpp_geonode.position())
        return tycommon.opoint3d2point3d(cpp_pos)

    @cy.locals(rot=tycommon.Point3D)
    def set_rotation(self, rot):
        mat = cy.declare(tycommon.OMatrix, self.matrix)
        cpp_geonode = cy.declare(TYGeometryNode)
        cpp_geonode.setMatrix(mat)
        cpp_rot = cy.declare(tycommon.OPoint3D, tycommon.cypoint2cpp(rot))
        cpp_geonode.setPosition(cpp_rot)
        self.matrix = cpp_geonode.getMatrix()

    @property
    def rotation(self):
        cpp_geonode = cy.declare(TYGeometryNode)
        cpp_geonode.setMatrix(self.matrix)
        cpp_rot = cy.declare(tycommon.OPoint3D,
                             cpp_geonode.rotation())
        return tycommon.opoint3d2point3d(cpp_rot)

    def set_sig_type(self, type_sig):
        self.thisptr.getRealPointer().setSIGType(type_sig)

    def set_sig_x(self, x):
        self.thisptr.getRealPointer().setSIG_X(x)

    def set_sig_y(self, y):
        self.thisptr.getRealPointer().setSIG_Y(y)

    def set_sig_offset(self, offset):
        self.thisptr.getRealPointer().setSIG_OFFSET(offset)

    @property
    def sig_type(self):
        return self.thisptr.getRealPointer().getSIGType()

    @property
    def sig_x(self):
        return self.thisptr.getRealPointer().getSIG_X()

    @property
    def sig_y(self):
        return self.thisptr.getRealPointer().getSIG_Y()

    @property
    def sig_offset(self):
        return self.thisptr.getRealPointer().getSIG_OFFSET()

    def getEmprise(self):
        topo = cy.declare(cy.pointer(TYTopographie),
                          self.thisptr.getRealPointer().getTopographie().getRealPointer())
        cpp_points = cy.declare(vector[TYPoint], topo.getEmprise())
        xmin = 1e8
        xmax = -xmin
        ymin = xmin
        ymax = -ymin
        for cpp_point in cpp_points:
            x = cpp_point._x
            y = cpp_point._y
            xmax = x if x > xmax else xmax
            ymax = y if y > ymax else ymax
            xmin = x if x < xmin else xmin
            ymin = y if y < ymin else ymin

        return xmin, xmax, ymin, ymax

cdef class MaterialArea:
    thisgeonodeptr = cy.declare(SmartPtr[TYGeometryNode])
    thisptr = cy.declare(cy.pointer(TYTerrain))
    matrix = cy.declare(tycommon.OMatrix)

    @property
    def ground_material(self):
        """The ground material the material area is made of ('Ground' object)
        """
        ground = Ground()
        ground.thisptr = self.thisptr.getSol()
        return ground

    @property
    def points(self):
        """The list of points delimiting the material area in a global scale"""
        # retrieve material area points
        cpp_points = cy.declare(vector[TYPoint], self.thisptr.getListPoints())
        return cpp2cypoints_global(cpp_points, self.matrix)

    @property
    def elem_id(self):
        """The MaterialArea id"""
        return tyelement_id(self.thisptr)

    @property
    def name(self):
        """The MaterialArea name"""
        return tyelement_name(self.thisptr)

    def has_vegetation(self):
        """True if the material area has vegetation"""
        return self.thisptr.isVegetActive()

    @property
    def vegetation(self):
        """The vegetation associated with material area"""
        vegetation = Vegetation()
        vegetation.thisptr = self.thisptr.getVegetation()
        return vegetation


cdef class LevelCurve:
    thisgeonodeptr = cy.declare(SmartPtr[TYGeometryNode])
    thisptr = cy.declare(cy.pointer(TYCourbeNiveau))
    matrix = cy.declare(tycommon.OMatrix)
    _altitude = cy.declare(double)
    altitude_set = cy.declare(bool)

    def __cinit__(self):
        self.altitude_set = False

    @property
    def points(self):
        """The sequence of points forming the level curve ('Point3D' objects)
        """
        # retrieve level curve points
        cpp_points = cy.declare(vector[TYPoint], self.thisptr.getListPoints())
        return cpp2cypoints_global(cpp_points, self.matrix)

    @property
    def altitude(self):
        """The altitude of the level curve (float value)"""
        if not self.altitude_set:
            return self.thisptr.getAltitude()
        return self._altitude

    @property
    def elem_id(self):
        """The LevelCurve id"""
        return tyelement_id(self.thisptr)

    @property
    def name(self):
        """The LevelCurve name"""
        return tyelement_name(self.thisptr)


cdef class Lake:
    thisgeonodeptr = cy.declare(SmartPtr[TYGeometryNode])
    thisptr = cy.declare(cy.pointer(TYPlanEau))
    matrix = cy.declare(tycommon.OMatrix) # to get Lake pos in a global scale

    @property
    def ground_material(self):
        """The water material the material area is made of ('Ground' object)
        """
        ground = Ground()
        ground.thisptr = self.thisptr.getSol()
        return ground

    @property
    def elem_id(self):
        """The Lake id"""
        return tyelement_id(self.thisptr)

    @property
    def altitude(self):
        """The lake's level curve as a 'LevelCurve' cython object"""
        lev_curve = LevelCurve()
        lev_curve.thisptr = self.thisptr.getCrbNiv().getRealPointer()
        return lev_curve.altitude

    @property
    def points(self):
        """The sequence of points forming the level curve of the lake"""
        lev_curve = LevelCurve()
        lev_curve.thisptr = self.thisptr.getCrbNiv().getRealPointer()
        cpp_points = cy.declare(
            vector[TYPoint], lev_curve.thisptr.getListPoints())
        return cpp2cypoints_global(cpp_points, self.matrix)

    @property
    def name(self):
        """The Lake name"""
        return tyelement_name(self.thisptr)

    @property
    def level_curve(self):
        """The lake's level curve as a 'LevelCurve' cython object"""
        lev_curve = LevelCurve()
        lev_curve.thisptr = self.thisptr.getCrbNiv().getRealPointer()
        return lev_curve


cdef class Result:

    def __cinit__(self):
        self.thisptr = SmartPtr[TYResultat]()

    @property
    def nsources(self):
        """The number of acoustic sources"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getNbOfSources()

    @property
    def nreceptors(self):
        """The number of acoustic receptors"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getNbOfRecepteurs()

    @property
    def sources(self):
        """ get the list of sources known by the result """
        tab_srcs = cy.declare(vector[SmartPtr[TYElement]],
                              self.thisptr.getRealPointer().getSources())
        sources = []
        nsources = tab_srcs.size()
        for i in xrange(nsources):
            source = Element()
            source.thisptr = tab_srcs[i]
            sources.append(source)
        return sources

    @property
    def receptors(self):
        """ get the list of receptors known by the result """
        tab_rcpts = cy.declare(vector[SmartPtr[TYElement]],
                               self.thisptr.getRealPointer().getReceptors())
        receptors = []
        nreceptors = tab_rcpts.size()
        for i in xrange(nreceptors):
            receptor = Element()
            receptor.thisptr = tab_rcpts[i]
            receptors.append(receptor)
        return receptors

    def receptor(self, index):
        """The receptor of index 'index'"""
        assert self.thisptr.getRealPointer() != NULL
        return pointcalcul2receptor(self.thisptr.getRealPointer().getRecepteur(index))

    def build_matrix(self):
        """ Build the matrix knowing sources and receptors """
        self.thisptr.getRealPointer().buildMatrix()

    def not_use_LW(self, abool=True):
        """ set use of LW """
        self.thisptr.getRealPointer().setHideLW(abool)

    @cy.locals(receptor=Element, source=Element)
    def spectrum(self, receptor, source):
        """The computed acoustic spectrum"""
        assert self.thisptr.getRealPointer() != NULL
        return tycommon.ospectre2spectrum(
                        self.thisptr.getRealPointer().getSpectre(
                                        receptor.thisptr.getRealPointer(),
                                        source.thisptr.getRealPointer()  )   )

    @cy.locals(receptor=Element)
    def add_receptor(self, receptor):
        """ add a business receptor in result matrix """
        self.thisptr.getRealPointer().addRecepteur(receptor.thisptr.getRealPointer())

    @cy.locals(source=Element)
    def add_source(self, source):
        """ Add a new business source in result matrix """
        self.thisptr.getRealPointer().addSource(source.thisptr.getRealPointer())

    @cy.locals(receptor=Element, source=Element, spectrum=tycommon.Spectrum)
    def set_spectrum(self, receptor, source, spectrum):
        self.thisptr.getRealPointer().setSpectre(
                                        receptor.thisptr.getRealPointer(),
                                        source.thisptr.getRealPointer(),
                                        spectrum.thisobj                    )


cdef class Mesh:
    thisptr = cy.declare(cy.pointer(TYMaillage))
    thisgeonodeptr =  cy.declare(SmartPtr[TYGeometryNode])
    matrix = cy.declare(tycommon.OMatrix) # local to global transformation

    def export_csv(self, file_name):
        self.thisptr.exportCSV(file_name)

    @property
    def is_active(self):
        """True if the mesh is in an active state, false otherwise"""
        # enum value from MaillageState (class TYMaillage)
        return self.thisptr.etat()

    @property
    def receptors(self):
        """The receptors contained in the mesh as a list of 'Receptor' cython objects
        """
        assert self.thisptr != NULL
        ptscalc = cy.declare(vector[SmartPtr[TYPointCalcul]], self.thisptr.getPtsCalcul())
        nptscalc = ptscalc.size()
        receptors = []
        for i in xrange(nptscalc):
            rec = Receptor()
            rec.thisptr = ptscalc[i]
            rec.parent_mesh = self
            receptors.append(rec)
        return receptors

    @property
    def getDataType(self):
        """Return the type to set the unit"""
        return self.thisptr.getDataType()


cdef class Receptor:
    thisptr = cy.declare(SmartPtr[TYPointCalcul])
    parent_mesh = cy.declare(Mesh)

    def __cinit__(self):
        self.thisptr = SmartPtr[TYPointCalcul]()

    @property
    def position(self):
        """The receptor position in a local scale (as a 'Point3D' object)"""
        assert(self.thisptr.getRealPointer() != NULL)
        cdef tycommon.OPoint3D pos
        pos._x = self.thisptr.getRealPointer()._x
        pos._y = self.thisptr.getRealPointer()._y
        pos._z = self.thisptr.getRealPointer()._z
        return tycommon.opoint3d2point3d(pos)

    @property
    def global_position(self):
        """The receptor position in a global scale (as a 'Point3D' object)"""
        assert self.thisptr.getRealPointer() != NULL
        return tycommon.opoint3d2point3d(tycommon.dot(self.parent_mesh.matrix,
                                                      self.thisptr.getRealPointer()[0]))

    def is_control_point(self):
        """True if the receptor is a control point, false otherwise"""
        assert self.thisptr.getRealPointer() != NULL
        control_point = cy.declare(cy.pointer(TYPointControl),
                                   downcast_point_control(self.thisptr.getRealPointer()))
        return (control_point != NULL)

    @cy.locals(comp=Computation)
    def is_active(self, comp):
        """ Return true if this receptor is active in computation 'comp', false
            otherwise
        """
        if self.thisptr.getRealPointer().etat(comp.thisptr.getRealPointer()):
            return True
        return False

#    @cy.locals(spectrum=tycommon.Spectrum, comp=Computation)
#    def set_spectrum(self, spectrum, comp):
#        """ Set the spectrum for the given computation """
#        cpp_calc = cy.declare(cy.pointer(TYCalcul))
#        cpp_calc = downcast_calcul(comp.thisptr.getRealPointer())
#
#        self.thisptr.getRealPointer().setSpectre(TYSpectre(spectrum.thisobj))

    @property
    def dBA(self):
        """Balanced spectrum value in DBA (for audible frequencies)"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getValA()


cdef class UserReceptor:
    thisptr = cy.declare(SmartPtr[TYPointControl])


cdef class Computation:

    def __cinit__(self):
        self.thisptr = SmartPtr[TYCalcul]()

    def get_solver_parameters(self):
        """Retrieve solver parameters in a string"""
        return self.thisptr.getRealPointer().solverParams.toStdString()

    def set_solver_parameters(self, params):
        # self.thisptr.getRealPointer().solverParams = QString(params.encode('utf-8'))
        # To avoid Cython error message "Obtaining 'char const *' from temporary Python value"
        # See http://cython.readthedocs.io/en/latest/src/userguide/language_basics.html#caveats-when-using-a-python-string-in-a-c-context
        pystring = params.encode('utf-8')
        self.thisptr.getRealPointer().solverParams = QString(pystring)

    solver_parameters = property(get_solver_parameters, set_solver_parameters)

    @cy.locals(receptor=UserReceptor)
    def addReceptor(self, receptor):
        self.thisptr.getRealPointer().addPtCtrlToResult(receptor.thisptr)

    @cy.locals(receptor=Receptor, spectrum=tycommon.Spectrum)
    def set_spectrum(self, receptor, spectrum):
        tyspectre = cy.declare(cy.pointer(TYSpectre))
        tyspectre = new TYSpectre(spectrum.thisobj)
        self.thisptr.getRealPointer().setSpectre(receptor.thisptr.getRealPointer(), tyspectre)

    @cy.locals(mesh=Mesh)
    def add_noise_map(self, mesh):
        self.thisptr.getRealPointer().addMaillage(mesh.thisptr)

    @cy.locals(mesh=Mesh)
    def get_noise_map_spectrums(self, mesh):
        cpp_spectrums = cy.declare(vector[SmartPtr[TYSpectre]])
        cpp_spectrums = deref(self.thisptr.getRealPointer().getSpectrumDatas(mesh.thisptr))
        spectrums = []
        cpp_spectre = cy.declare(SmartPtr[TYSpectre])
        for cpp_spectre in cpp_spectrums:
            tyspectre = cy.declare(cy.pointer(TYSpectre),
                                   cpp_spectre.getRealPointer())
            ospectre_ptr = cy.declare(cy.pointer(tycommon.OSpectre),
                                      tyspectre.downcast_ospectre())
            ospectre = cy.declare(tycommon.OSpectre,
                                  deref(ospectre_ptr))
            cyspectre = tycommon.Spectrum()
            cyspectre = tycommon.ospectre2spectrum(ospectre)
            spectrums.append(cyspectre)
        return spectrums

    @cy.locals(mesh=Mesh)
    def set_noise_map_spectrums(self, mesh, spectrums):
        cpp_spectrums = cy.declare(cy.pointer(vector[SmartPtr[TYSpectre]]))
        cpp_spectrums = self.thisptr.getRealPointer().getSpectrumDatas(mesh.thisptr)
        assert(cpp_spectrums.size() == len(spectrums))
        cpp_spectrums.clear()
        cy_spectrum = cy.declare(tycommon.Spectrum)
        for cy_spectrum in spectrums:
            ospectre = cy.declare(tycommon.OSpectre, cy_spectrum.thisobj)
            spectrum_ptr = cy.declare(SmartPtr[TYSpectre],
                                      SmartPtr[TYSpectre](new TYSpectre(ospectre)))
            cpp_spectrums.push_back(spectrum_ptr)

    @property
    def result(self):
        """Return an acoustic result (business representation)"""
        assert self.thisptr.getRealPointer() != NULL
        res = Result()
        res.thisptr = self.thisptr.getRealPointer().getResultat()
        return res

    @property
    def name(self):
        """The name of the element"""
        assert self.thisptr.getRealPointer() != NULL
        cpp_elem = cy.declare(cy.pointer(TYElement),
                              downcast_Element(self.thisptr.getRealPointer()))
        return cpp_elem.getName().toStdString().decode()

    def set_name(self, name):
        """ Set the name of the computation """
        assert self.thisptr.getRealPointer() != NULL
        cpp_elem = cy.declare(cy.pointer(TYElement),
                              downcast_Element(self.thisptr.getRealPointer()))
        cpp_elem.setName(name.encode('utf-8'))

    def set_solver(self, solverdir, name):
        """`solver_name` will be used to solve this computation"""
        assert self.thisptr.getRealPointer() != NULL
        # load_solvers(solverdir.encode('utf-8'))
        # To avoid Cython error message "Obtaining 'char const *' from temporary Python value"
        # See http://cython.readthedocs.io/en/latest/src/userguide/language_basics.html#caveats-when-using-a-python-string-in-a-c-context
        pystring = solverdir.encode('utf-8')
        load_solvers(pystring)
        solverid = cy.declare(OGenID, solver_id(name.encode('utf-8')))
        self.thisptr.getRealPointer().setSolverId(solverid)

    @property
    def solver_id(self):
        """The identifier of the solver that will be used to solve the computation"""
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getSolverId().toString().toStdString().decode()


cdef class Project:
    thisptr = cy.declare(SmartPtr[TYProjet])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYProjet]()

    def update(self):
        """Update the project (inactive mesh points detection)"""
        # detect and disable the mesh points that are inside machines or buildings
        self.thisptr.getRealPointer().selectActivePoint(self.thisptr.getRealPointer().getSite())

    def _update_site_altimetry(self, *args):
        """Update site altimetry and project"""
        self.site.update_altimetry(*args)
        self.update()

    def add_user_receptor(self, point, height, name):
        cpp_point = cy.declare(TYPoint, cypoint2cpp(point))
        cpp_receptor = cy.declare(SmartPtr[TYPointControl],
                                  SmartPtr[TYPointControl](new TYPointControl(cpp_point)))
        cpp_receptor.getRealPointer().setHauteur(height)
        cpp_receptor.getRealPointer().setName(name.encode("utf-8"))
        self.thisptr.getRealPointer().addPointControl(cpp_receptor)

    @cy.locals(comp=Computation)
    def add_computation(self):
        """Add a new computation to the project and return it"""
        comp = make_computation()
        self.thisptr.getRealPointer().addCalcul(comp.thisptr)
        return comp

    @cy.locals(comp=Computation)
    def select_computation(self, comp):
        """ set the current computation """
        self.thisptr.getRealPointer().setCurrentCalcul(comp.thisptr)

    @property
    def current_computation(self):
        """The project current computation"""
        assert self.thisptr.getRealPointer() != NULL
        comp = Computation()
        comp.thisptr = self.thisptr.getRealPointer().getCurrentCalcul()
        return comp

    @property
    def computations(self):
        tab_comp = cy.declare(vector[SmartPtr[TYCalcul]],
                              self.thisptr.getRealPointer().getListCalcul())
        computations = []
        ncomp = tab_comp.size()
        for i in xrange(ncomp):
            comp = Computation()
            comp.thisptr = tab_comp[i]
            computations.append(comp)
        return computations


    @property
    def site(self):
        """The site considered in the project"""
        assert self.thisptr.getRealPointer() != NULL
        site = Site()
        site.thisptr = self.thisptr.getRealPointer().getSite()
        return site

    @property
    def user_receptors(self):
        """User-defined receptors (control points) as 'UserReceptor' cython objects
        """
        assert self.thisptr.getRealPointer() != NULL
        ctrl_pts = cy.declare(vector[SmartPtr[TYPointControl]],
                              self.thisptr.getRealPointer().getPointsControl())
        n_ctrl_pts = ctrl_pts.size()
        control_points = []
        for i in xrange(n_ctrl_pts):
            urec = UserReceptor()
            urec.thisptr = ctrl_pts[i]
            control_points.append(urec)
        return control_points

    @property
    def meshes(self):
        """The meshes of the computation (a list of 'Mesh' cython objects)"""
        assert self.thisptr.getRealPointer() != NULL
        geomaill = cy.declare(vector[SmartPtr[TYGeometryNode]])
        geomaill = self.thisptr.getRealPointer().getMaillages()
        nmaill = geomaill.size()
        meshes = []
        for i in xrange(nmaill):
            mesh = Mesh()
            mesh.matrix = geomaill[i].getRealPointer().getMatrix()
            mesh.thisptr = downcast_maillage(geomaill[i].getRealPointer().getElement())
            meshes.append(mesh)
            mesh.thisgeonodeptr = geomaill[i]
        return meshes

    @staticmethod
    def from_xml(filepath):
        """Build a project (TYProject) from a xml file"""
        init_tympan_registry()
        project = Project()
        # if an exception is raised from the C++ code, it will be converted to
        # RuntimeError python exception. what() message should be preserved.
        # see http://docs.cython.org/src/userguide/wrapping_CPlusPlus.html#exceptions
        if isinstance(filepath, str):
            filepath = filepath.encode('utf-8')
        project.thisptr = load_project(filepath)
        return project

    @staticmethod
    def create():
        return make_typrojet()

    def to_xml(self, filepath):
        """Export an acoustic project to a XML file"""
        assert self.thisptr.getRealPointer() != NULL
        # same thing as for load_project about the exception
        # save_project(filepath.encode('utf-8'), self.thisptr)
        # To avoid Cython error message "Obtaining 'char const *' from temporary Python value"
        # See http://cython.readthedocs.io/en/latest/src/userguide/language_basics.html#caveats-when-using-a-python-string-in-a-c-context
        pystring = filepath.encode('utf-8')
        save_project(pystring, self.thisptr)

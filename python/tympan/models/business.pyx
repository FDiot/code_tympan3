"""business models for Code_TYMPAN
"""

import cython as cy
from cython.operator cimport dereference as deref, preincrement as inc

from tympan.models cimport common as tycommon
from tympan.models cimport solver as tysolver
from tympan.business2solver cimport business2microsource


cdef pointcalcul2receptor(SmartPtr[TYPointCalcul] ptcalc):
    """factory function: return a Receptor (python object) from a
    SmartPtr[TYPointCalcul] (cpp lib)
    """
    rec = Receptor()
    rec.thisptr = ptcalc
    return rec

cdef typrojet2project(TYProjet* proj):
    """factory function: return a Project (python object) from a TYProjet (cpp
    lib)
    """
    project = Project()
    project.thisptr._pObj = proj
    return project

cdef tymateriauconstruction2material(SmartPtr[TYMateriauConstruction] mat):
    """factory function: return a Material (python object) from a
    SmartPtr[TYMateriauConstruction] (cpp lib)
    """
    material = Material()
    material.thisptr = mat
    return material

cdef tysol2ground(SmartPtr[TYSol] grnd):
    """factory function: return a Ground (python object) from a SmartPtr[TYSol] (cpp lib)
    """
    ground = Ground()
    ground.thisptr = grnd
    return ground

cdef tyelement_id(TYElement* elem):
    """ Return the id of the element contained in the TYGeometryNode as a string
    """
    return elem.getID().toString().toStdString()

cdef cpp2cypoints(vector[TYPoint] cpp_points, tycommon.OMatrix matrix):
    """ Return a list of 'Point3D' cython objects from the c++ 'TYPoint' contained
    in the vector 'cpp_points'.
    Convert them to a global scale with the transform matrix 'matrix'
    """
    points = []
    for i in xrange(cpp_points.size()):
        cpp_point = cy.declare(tycommon.OPoint3D)
        cpp_point = tycommon.dot(matrix, cpp_points[i])
        cy_point = cy.declare(tycommon.Point3D)
        cy_point = tycommon.opoint3d2point3d(cpp_point)
        points.append(cy_point)
    return points

def init_tympan_registry():
    """ Trigger the registration of Tympan business classes (TY* classes).
        It is necessary to do it before playing with Tympan library (just after
        cython libraries import)
    """
    init_registry()


cdef class Element:
    thisptr = cy.declare(SmartPtr[TYElement])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYElement]()

    def name(self):
        """ Return the name of the element
        """
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getName().toStdString()


cdef class AcousticSurface:
    thisptr = cy.declare(cy.pointer(TYAcousticSurface))

    def __cinit__(self):
        self.thisptr = NULL

    def export_mesh(self):
        """ Retrieve the points and triangles constituting the acoustic surface
            and return them as list of 'Point3D' and a list of 'Triangle'
            cy objects
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
        itp = cy.declare(deque[tycommon.OPoint3D].iterator)
        itp = pts.begin()
        while itp != pts.end():
            points.append(tycommon.opoint3d2point3d(deref(itp)))
            inc(itp)
        triangles = []
        itt = cy.declare(deque[tycommon.OTriangle].iterator)
        itt = tgles.begin()
        while itt != tgles.end():
            # Assert consistency of the tycommon.OPoint3D given in the mesh
            assert (deref(itt).checkConsistencyWrtPointsTab(pts),
                    deref(itt).reportInconsistencyWrtPointsTab(pts))
            tri = cy.declare(cy.pointer(tycommon.OTriangle))
            tri = new tycommon.OTriangle(deref(itt)._p1, deref(itt)._p2, deref(itt)._p3)
            triangles.append(tycommon.otriangle2triangle(tri))
            inc(itt)
        return (points, triangles)

    @property
    def material(self):
        """ Return the material the acoustic surface is made of, as a 'Material'
            cython object
        """
        return tymateriauconstruction2material(self.thisptr.getMateriau())


cdef class Material:

    @property
    def spectrum(self):
        """ Return the spectrum of the material as a 'Spectrum' cython object
        """
        assert self.thisptr.getRealPointer() != NULL
        return tycommon.ospectre2spectrum(self.thisptr.getRealPointer().getSpectreAbso())

    @property
    def name(self):
        """ Return a string representing the name of the element
        """
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getName().toStdString()


cdef class Ground:

    @property
    def resistivity(self):
        """ Return ground resistivity (double value)
        """
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getResistivite()

    def name(self):
        """ Return a string representing the name of the element
        """
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getName().toStdString()

    @property
    def elem_id(self):
        """ Return Ground id as a string """
        return tyelement_id(self.thisptr.getRealPointer())


cdef class Site:

    @property
    def project(self):
        """ Return the parent project of the site as a 'Project' cython object
        """
        return typrojet2project(self.thisptr.getRealPointer().getProjet())

    @property
    def acoustic_surfaces(self):
        """ Retrieve the acoustic surfaces of the site and return them as a list
            of 'AcousticSurface' cython objects.
        """
        assert self.thisptr.getRealPointer() != NULL
        is_screen_face_idx = cy.declare(vector[bool])
        face_list = cy.declare(vector[SmartPtr[TYGeometryNode]])
        nb_building_faces = cy.declare(cy.uint)
        nb_building_faces = 0
        self.thisptr.getRealPointer().getListFaces(face_list, nb_building_faces,
                                                   is_screen_face_idx)
        surfaces = []
        for i in xrange(nb_building_faces):
            surf = AcousticSurface()
            pelt = cy.declare(cy.pointer(TYElement))
            pElt = face_list[i].getRealPointer().getElement()
            surf.thisptr = downcast_acoustic_surface(pElt)
            # 'face_list' can contain topography elements. Not relevant here.
            if surf.thisptr == NULL:
                continue
            surfaces.append(surf)
        return surfaces

    def export_topo_mesh(self):
        """ Retrieve the mesh of the site topography and return it as 3 lists:
            - 'points' contains 'Point3D' cython objects
            - 'triangles' contains 'Triangle' cython objects
            - 'ground' contains 'Ground' cython objects
        """
        assert self.thisptr.getRealPointer() != NULL
        pts = cy.declare(deque[tycommon.OPoint3D])
        tgles = cy.declare(deque[tycommon.OTriangle])
        mats = cy.declare(deque[SmartPtr[TYSol]])
        ptopo = cy.declare(cy.pointer(TYTopographie))
        ptopo = self.thisptr.getRealPointer().getTopographie().getRealPointer()
        # Retrieve topography mesh as c++ objects
        ptopo.exportMesh(pts, tgles, cy.address(mats))
        # Retrieve points
        points = []
        itp = cy.declare(deque[tycommon.OPoint3D].iterator)
        itp = pts.begin()
        while itp != pts.end():
            points.append(tycommon.opoint3d2point3d(deref(itp)))
            inc(itp)
        # Retrieve triangles
        triangles = []
        itt = cy.declare(deque[tycommon.OTriangle].iterator)
        itt = tgles.begin()
        while itt != tgles.end():
            # Assert consistency of the tycommon.OPoint3D given in the mesh
            assert (deref(itt).checkConsistencyWrtPointsTab(pts),
                    deref(itt).reportInconsistencyWrtPointsTab(pts))
            tri = cy.declare(cy.pointer(tycommon.OTriangle))
            tri = new tycommon.OTriangle(deref(itt)._p1, deref(itt)._p2, deref(itt)._p3)
            triangles.append(tycommon.otriangle2triangle(tri))
            inc(itt)
        # Retrieve ground materials
        grounds = []
        itg = cy.declare(deque[SmartPtr[TYSol]].iterator)
        itg = mats.begin()
        while itg != mats.end():
            grounds.append(tysol2ground(deref(itg)))
            inc(itg)
        return (points, triangles, grounds)

    @cy.locals(comp=Computation)
    def fetch_sources(self, comp):
        """ Explore business infrastructure to retrieve its acoustic sources
            (macro and micro)
            Return them as a 'Business2MicroSource' containing a map linking macro
            sources (ie business sources like buildings or machines) to the
            corresponding lists of micro sources.
        """
        assert self.thisptr.getRealPointer() != NULL
        map_elt_srcs = cy.declare(map[TYElem_ptr, vector[SmartPtr[TYGeometryNode]]])
        infra = cy.declare(cy.pointer(TYInfrastructure))
        infra = self.thisptr.getRealPointer().getInfrastructure().getRealPointer()
        infra.getAllSrcs(comp.thisptr.getRealPointer(), map_elt_srcs)
        return business2microsource(map_elt_srcs)

    def childs(self):
        """ Return the direct childs of the Site (ie the elements it contains)
            as a python list. Not recursive.
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

    def update(self):
        """ Updates a site:
            - "Sort" terrains contained in a topography
            - Updates the altimetry of the infrastructure elements
            - Updates the acoustic of the infrastructure elements
        """
        # This method actually leads to the dynamic allocation of an array of
        # TYGeometryNode that will be accessed by some other method(s) later on.
        # Not calling this method leads to a segmentation fault since the array
        # doesn't exist then.
        self.thisptr.getRealPointer().getTopographie().getRealPointer().sortTerrainsBySurface()
        self.thisptr.getRealPointer().updateAltiInfra(True)
        self.thisptr.getRealPointer().updateAcoustique(True)


    @property
    def lakes(self):
        """ Return the lakes of the topography as a list of 'Lake' cython objects """
        lakes = []
        material_areas = []
        cpp_lakes = cy.declare(vector[SmartPtr[TYGeometryNode]])
        topo = cy.declare(cy.pointer(TYTopographie))
        topo = self.thisptr.getRealPointer().getTopographie().getRealPointer()
        cpp_lakes = topo.getListPlanEau()
        for i in xrange(cpp_lakes.size()):
            cpp_lake = cy.declare(cy.pointer(TYPlanEau))
            cpp_lake = downcast_plan_eau(cpp_lakes[i].getRealPointer().getElement())
            lake = Lake()
            lake.thisptr = cpp_lake
            lake.matrix = cpp_lakes[i].getRealPointer().getMatrix()
            lakes.append(lake)
        return lakes

    @property
    def material_areas(self):
        """ Return the material areas of the topography as a list of
        'MaterialArea' cython objects """
        mareas = []
        cpp_mat_areas = cy.declare(vector[SmartPtr[TYGeometryNode]])
        topo = cy.declare(cy.pointer(TYTopographie))
        topo = self.thisptr.getRealPointer().getTopographie().getRealPointer()
        cpp_mat_areas = topo.getListTerrain()
        for i in xrange (cpp_mat_areas.size()):
            cpp_mat_area = cy.declare(cy.pointer(TYTerrain))
            cpp_mat_area = downcast_terrain(
                cpp_mat_areas[i].getRealPointer().getElement())
            area = MaterialArea()
            area.thisptr = cpp_mat_area
            area.matrix = cpp_mat_areas[i].getRealPointer().getMatrix()
            mareas.append(area)
        return mareas

    @property
    def level_curves(self):
        """ Return the level curves of the topography as a list of 'LevelCurve'
        cython objects """
        lcurves = []
        cpp_lcurves = cy.declare(vector[SmartPtr[TYGeometryNode]])
        topo = cy.declare(cy.pointer(TYTopographie))
        topo = self.thisptr.getRealPointer().getTopographie().getRealPointer()
        cpp_lcurves = topo.getListCrbNiv()
        for i in xrange (cpp_lcurves.size()):
            cpp_lcurve = cy.declare(cy.pointer(TYCourbeNiveau))
            cpp_lcurve = downcast_courbe_niveau(
                cpp_lcurves[i].getRealPointer().getElement())
            lcurve = LevelCurve()
            lcurve.thisptr = cpp_lcurve
            lcurve.matrix = cpp_lcurves[i].getRealPointer().getMatrix()
            lcurve.altitude = cpp_lcurve.getAltitude()
            lcurves.append(lcurve)
        return lcurves


cdef class MaterialArea:
    thisptr = cy.declare(cy.pointer(TYTerrain))
    matrix = cy.declare(tycommon.OMatrix)

    @property
    def ground_material(self):
        """ Return the ground material the material area is made of as a 'Ground'
        cython object """
        ground = Ground()
        ground.thisptr = self.thisptr.getSol()
        return ground

    @property
    def points(self):
        # retrieve material area points
        cpp_points = cy.declare(vector[TYPoint])
        cpp_points = self.thisptr.getListPoints()
        return cpp2cypoints(cpp_points, self.matrix)

    @property
    def elem_id(self):
        """ Return MaterialArea id as a string """
        return tyelement_id(self.thisptr)


cdef class LevelCurve:
    thisptr = cy.declare(cy.pointer(TYCourbeNiveau))
    matrix = cy.declare(tycommon.OMatrix)
    altitude

    @property
    def points(self):
        """ Returns the successive points of the level curve as a list of
        'Point3D' cython objects
        """
        # retrieve level curve points
        cpp_points = cy.declare(vector[TYPoint])
        cpp_points = self.thisptr.getListPoints()
        return cpp2cypoints(cpp_points, self.matrix)

    @property
    def altitude(self):
        """ Return the altitude of the level curve (float value) """
        return self.altitude

    @property
    def elem_id(self):
        """ Return LevelCurve id as a string """
        return tyelement_id(self.thisptr)


cdef class Lake:
    thisptr = cy.declare(cy.pointer(TYPlanEau))
    matrix = cy.declare(tycommon.OMatrix) # to get Lake pos in a global scale

    @property
    def elem_id(self):
        """ Return Lake id as a string """
        return tyelement_id(self.thisptr)

    @property
    def level_curve(self):
        """ Return the lake's level curve as a 'LevelCurve' cython object
        """
        lev_curve = LevelCurve()
        lev_curve.thisptr = self.thisptr.getCrbNiv().getRealPointer()
        return lev_curve


cdef class Result:
    thisptr = cy.declare(SmartPtr[TYResultat])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYResultat]()

    @property
    def nsources(self):
        """ Return the number of acoustic sources
        """
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getNbOfSources()

    @property
    def nreceptors(self):
        """ Return the number of acoustic receptors
        """
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getNbOfRecepteurs()

    def receptor(self, index):
        """ Return the receptor of index 'index'
        """
        assert self.thisptr.getRealPointer() != NULL
        return pointcalcul2receptor(self.thisptr.getRealPointer().getRecepteur(index))

    def spectrum(self, receptor, source):
        """ Return the computed acoustic spectrum
        """
        assert self.thisptr.getRealPointer() != NULL
        return tycommon.ospectre2spectrum(self.thisptr.getRealPointer().getSpectre(receptor, source))


cdef class Mesh:
    thisptr = cy.declare(cy.pointer(TYMaillage))
    matrix = cy.declare(tycommon.OMatrix) # local to global transformation

    @property
    def is_active(self):
        """ Return true if the mesh is in an active state, false otherwise
        """
        # enum value from MaillageState (class TYMaillage)
        return self.thisptr.getState() == Actif

    @property
    def receptors(self):
        """ Return the receptors contained in the mesh as a list of 'Receptor'
            cython objects
        """
        assert self.thisptr != NULL
        ptscalc = cy.declare(vector[SmartPtr[TYPointCalcul]])
        ptscalc = self.thisptr.getPtsCalcul()
        nptscalc = ptscalc.size()
        receptors = []
        for i in xrange(nptscalc):
            rec = Receptor()
            rec.thisptr = ptscalc[i]
            rec.parent_mesh = self
            receptors.append(rec)
        return receptors


cdef class Receptor:
    thisptr = cy.declare(SmartPtr[TYPointCalcul])
    parent_mesh = cy.declare(Mesh)

    def __cinit__(self):
        self.thisptr = SmartPtr[TYPointCalcul]()

    @property
    def position(self):
        """ Return the receptor position in a local scale (as a 'Point3D' object)
        """
        assert(self.thisptr.getRealPointer() != NULL)
        cdef tycommon.OPoint3D pos
        pos._x = self.thisptr.getRealPointer()._x
        pos._y = self.thisptr.getRealPointer()._y
        pos._z = self.thisptr.getRealPointer()._z
        return tycommon.opoint3d2point3d(pos)

    @property
    def global_position(self):
        """ Return the receptor position in a global scale (as a 'Point3D' object)
        """
        assert self.thisptr.getRealPointer() != NULL
        return tycommon.opoint3d2point3d(tycommon.dot(self.parent_mesh.matrix,
                                                      self.thisptr.getRealPointer()[0]))

    def is_control_point(self):
        """ Return true if the receptor is a control point (that is, a kind of
            "smart" receptor), false otherwise
        """
        assert self.thisptr.getRealPointer() != NULL
        control_point = cy.declare(cy.pointer(TYPointControl))
        control_point = downcast_point_control(self.thisptr.getRealPointer())
        return (control_point != NULL)

    @cy.locals(comp=Computation)
    def is_active(self, comp):
        """ Return true if this receptor is active in computation 'comp', false
            otherwise
        """
        if self.thisptr.getRealPointer().getEtat(comp.thisptr.getRealPointer()):
            return True
        return False


cdef class UserReceptor:
    thisptr = cy.declare(SmartPtr[TYPointControl])


cdef class Computation:

    def __cinit__(self):
        self.thisptr = SmartPtr[TYCalcul]()

    @property
    def meshes(self):
        """ Return the meshes of the computation (a list containing Mesh cython
            objects)
        """
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
        return meshes

    @property
    def result(self):
        """ Return an acoustic result (business representation)
        """
        assert self.thisptr.getRealPointer() != NULL
        res = Result()
        res.thisptr = self.thisptr.getRealPointer().getResultat()
        return res

    def go(self):
        """ Solve the current acoustic problem. A solver must be loaded.
        """
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().go()

    def set_nthread(self, nthread):
        """ Set the number of threads used by the default solver to compute the
            acoustic problem
        """
        assert self.thisptr.getRealPointer() != NULL
        self.thisptr.getRealPointer().setNbThread(nthread)

    @property
    def acoustic_problem(self):
        """ Return an acoustic problem model (geometric representation as
            used by the solvers)
        """
        assert self.thisptr.getRealPointer() != NULL
        return tysolver.acousticproblemmodel2problemmodel(
            self.thisptr.getRealPointer()._acousticProblem.get())

    @property
    def acoustic_result(self):
        """ Return an acoustic result model (geometric representation as used
        by the solvers)
        """
        assert self.thisptr.getRealPointer() != NULL
        return tysolver.acousticresultmodel2resultmodel(
            self.thisptr.getRealPointer()._acousticResult.get())


cdef class Project:
    thisptr = cy.declare(SmartPtr[TYProjet])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYProjet]()

    def update_site(self):
        """ Update the project site (altimetry, atmosphere, inactive mesh points
            detection).
        """
        self.site.update()
        computation = cy.declare(cy.pointer(TYCalcul))
        computation = self.thisptr.getRealPointer().getCurrentCalcul().getRealPointer()
        atmosphere = cy.declare(SmartPtr[TYAtmosphere])
        atmosphere = computation.getAtmosphere()
        self.thisptr.getRealPointer().getSite().getRealPointer().setAtmosphere(atmosphere)
        # detect and deactivate the mesh points that are inside machines or buildings
        computation.selectActivePoint(self.thisptr.getRealPointer().getSite())

    def update_altimetry_on_receptors(self):
        """ Call Tympan method to update the acoustic receptors
            (TYPointControl objects) with regard to the current altimetry
        """
        site = cy.declare(cy.pointer(TYSiteNode))
        site = self.thisptr.getRealPointer().getSite().getRealPointer()
        alti = cy.declare(cy.pointer(TYAltimetrie))
        # Retrieve current altimetry from the site topography
        alti = site.getTopographie().getRealPointer().getAltimetrie().getRealPointer()
        self.thisptr.getRealPointer().updateAltiRecepteurs(alti)

    @property
    def current_computation(self):
        """ Return the project current computation
        """
        assert self.thisptr.getRealPointer() != NULL
        comp = Computation()
        comp.thisptr = self.thisptr.getRealPointer().getCurrentCalcul()
        return comp

    @property
    def site(self):
        """ Return the site considered in the project
        """
        assert self.thisptr.getRealPointer() != NULL
        site = Site()
        site.thisptr = self.thisptr.getRealPointer().getSite()
        return site

    @property
    def user_receptors(self):
        """ Return user-defined receptors (i.e. control points) as a list of
            'UserReceptor' cython objects
        """
        assert self.thisptr.getRealPointer() != NULL
        ctrl_pts = cy.declare(vector[SmartPtr[TYPointControl]])
        ctrl_pts = self.thisptr.getRealPointer().getPointsControl()
        n_ctrl_pts = ctrl_pts.size()
        control_points = []
        for i in xrange(n_ctrl_pts):
            urec = UserReceptor()
            urec.thisptr = ctrl_pts[i]
            control_points.append(urec)
        return control_points

    @staticmethod
    def from_xml(filepath):
        """ Build a project (TYProject) from a xml file
        """
        project = Project()
        # if an exception is raised from the C++ code, it will be converted to
        # RuntimeError python exception. what() message should be preserved.
        # see http://docs.cython.org/src/userguide/wrapping_CPlusPlus.html#exceptions
        project.thisptr = load_project(filepath)
        return project

    def to_xml(self, filepath):
        """ Export an acoustic project to a XML file
        """
        assert self.thisptr.getRealPointer() != NULL
        # same thing as for load_project about the exception
        save_project(filepath, self.thisptr)

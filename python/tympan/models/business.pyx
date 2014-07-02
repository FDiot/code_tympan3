"""business models for Code_TYMPAN
"""

import cython as cy
from cython.operator cimport dereference as deref, preincrement as inc

from tympan.models cimport common as tycommon
from tympan.models cimport solver as tysolver
from tympan.pytam cimport load_project, save_project, business2microsource # XXX


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
    thisptr = cy.declare(SmartPtr[TYMateriauConstruction])

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


cdef class Result:
    thisptr = cy.declare(SmartPtr[TYResultat])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYResultat]()

    @property
    def nsources(self):
        """ Returns the number of acoustic sources
        """
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getNbOfSources()

    @property
    def nreceptors(self):
        """ Returns the number of acoustic receptors
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
        # enum value from MaillageState (class TYMaillage)
        return self.thisptr.getState() == Actif

    @property
    def receptors(self):
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
        """ Returns an acoustic result model (geometric representation as used
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

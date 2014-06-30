"""business models for Code_TYMPAN, headers
"""

import cython as cy

from tympan.models cimport common as tycommon
from tympan.pytam cimport (load_project, save_project,
                           acousticproblemmodel2problemmodel,
                           acousticresultmodel2resultmodel,) # XXX


cdef class Element:
    thisptr = cy.declare(SmartPtr[TYElement])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYElement]()

    def name(self):
        """ Return the name of the element
        """
        assert self.thisptr.getRealPointer() != NULL
        return self.thisptr.getRealPointer().getName().toStdString()


cdef class Site:
    thisptr = cy.declare(SmartPtr[TYSiteNode])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYSiteNode]()

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
        receptor = Receptor()
        receptor.thisptr = self.thisptr.getRealPointer().getRecepteur(index)
        return receptor

    def spectrum(self, receptor, source):
        """ Return the computed acoustic spectrum
        """
        assert self.thisptr.getRealPointer() != NULL
        return tycommon.ospectre2spectrum(self.thisptr.getRealPointer().getSpectre(receptor, source))


cdef class Receptor:
    thisptr = cy.declare(SmartPtr[TYPointCalcul])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYPointCalcul]()

    def is_control_point(self):
        """ Return true if the receptor is a control point (that is, a kind of
            "smart" receptor), false otherwise
        """
        assert self.thisptr.getRealPointer() != NULL
        control_point = cy.declare(cy.pointer(TYPointControl))
        control_point = downcast_point_control(self.thisptr.getRealPointer())
        return (control_point != NULL)


cdef class Computation:
    thisptr = cy.declare(SmartPtr[TYCalcul])

    def __cinit__(self):
        self.thisptr = SmartPtr[TYCalcul]()

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
        return acousticproblemmodel2problemmodel(
            self.thisptr.getRealPointer()._acousticProblem.get())

    @property
    def acoustic_result(self):
        """ Returns an acoustic result model (geometric representation as used
        by the solvers)
        """
        assert self.thisptr.getRealPointer() != NULL
        return acousticresultmodel2resultmodel(
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

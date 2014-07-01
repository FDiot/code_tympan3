"""solver models for Code_TYMPAN
"""

import cython as cy

import numpy as np

from tympan.models cimport common as tycommon


cdef acousticproblemmodel2problemmodel(AcousticProblemModel* apm):
    """factory function: return a ProblemModel (python object) from an
    AcousticProblemModel (cpp lib)
    """
    pbmodel = ProblemModel()
    pbmodel.thisptr = apm
    return pbmodel

cdef acousticresultmodel2resultmodel(AcousticResultModel* arm):
    """factory function: return a ResultModel (python object) from an
    AcousticResultModel (cpp lib)
    """
    resmodel = ResultModel()
    resmodel.thisptr = arm
    return resmodel


cdef class ProblemModel:

    @property
    def npoints(self):
        """ Return the number of mesh nodes contained in the acoustic problem
            model
        """
        assert self.thisptr != NULL
        return self.thisptr.npoints()

    @property
    def ntriangles(self):
        """ Return the number of mesh triangles contained in the acoustic problem
            model
        """
        assert self.thisptr != NULL
        return self.thisptr.ntriangles()

    @property
    def nmaterials(self):
        """ Return the number of acoustic materials contained in the acoustic
            problem model
        """
        assert self.thisptr != NULL
        return self.thisptr.nmaterials()

    @property
    def nsources(self):
        """ Return the number of acoustic sources involved in the acoustic
            problem model
        """
        assert self.thisptr != NULL
        return self.thisptr.nsources()

    @property
    def nreceptors(self):
        """ Return the number of acoustic receptors involved in the acoustic
            problem model
        """
        assert self.thisptr != NULL
        return self.thisptr.nreceptors()

    def source(self, idx):
        """ Return the acoustic source (SolverSource object) of index 'idx'
        """
        assert self.thisptr != NULL
        source = SolverSource()
        source.thisptr = cy.address(self.thisptr.source(idx))
        return source

    @property
    def sources(self):
        """ Return all the acoustic sources of the model
        """
        assert self.thisptr != NULL
        sources = []
        for i in xrange(self.nsources):
            source = SolverSource()
            source.thisptr = cy.address(self.thisptr.source(i))
            sources.append(source)
        return sources

    def receptor(self, idx):
        """ Return the acoustic receptor (SolverReceptor object) of index 'idx'
        """
        assert self.thisptr != NULL
        receptor = SolverReceptor()
        receptor.thisptr = cy.address(self.thisptr.receptor(idx))
        return receptor

    @property
    def receptors(self):
        """ Return all the acoustic receptors of the model
        """
        assert self.thisptr != NULL
        receptors = []
        for i in xrange(self.nreceptors):
            receptor = SolverReceptor()
            receptor.thisptr = cy.address(self.thisptr.receptor(i))
            receptors.append(receptor)
        return receptors

    def export_triangular_mesh(self):
        """ Build a triangular mesh from the acoustic problem model.
            Return two nparrays:
                * 'nodes': an array of nodes (of dimension 'npoints'X3), where
                each line stands for a node and contains 3 coordinates)
                * 'triangles': an array of triangles (of dimension 'ntriangles'X3),
                where each line stands for a triangle and contains the indices of
                its 3 vertices in the 'nodes' array.
        """
        assert self.thisptr != NULL
        nb_elts = cy.declare(cy.uint)
        actri = cy.declare(cy.pointer(AcousticTriangle))
        nb_elts = self.thisptr.ntriangles()
        triangles = np.empty([nb_elts, 3])
        for i in xrange(nb_elts):
            actri = cy.address(self.thisptr.triangle(i))
            triangles[i] = [actri.n[0], actri.n[1], actri.n[2]]
        point = cy.declare(cy.pointer(tycommon.OPoint3D))
        nb_elts = self.thisptr.npoints()
        nodes = np.empty([nb_elts, 3])
        for i in xrange(nb_elts):
            point = cy.address(self.thisptr.node(i))
            nodes[i] = [point._x, point._y, point._z]
        return (nodes, triangles)


cdef class ResultModel:
    thisptr = cy.declare(cy.pointer(AcousticResultModel))

    def __cinit__(self):
        self.thisptr = NULL


cdef class SolverSource:
    thisptr = cy.declare(cy.pointer(AcousticSource))

    def __cinit__(self):
        self.thisptr = NULL

    @property
    def position(self):
        """ Return the acoustic source position (as a 'Point3D' object)
        """
        assert self.thisptr != NULL
        return tycommon.opoint3d2point3d(self.thisptr.position)

    @property
    def spectrum(self):
        """ Return the acoustic spectrum of the Source (dB scale, power spectrum)
        """
        assert self.thisptr != NULL
        return tycommon.ospectre2spectrum(self.thisptr.spectrum)


cdef class SolverReceptor:
    thisptr = cy.declare(cy.pointer(AcousticReceptor))

    def __cinit__(self):
        self.thisptr = NULL

    @property
    def position(self):
        """ Return the acoustic source position (as a 'Point3D' object)
        """
        assert self.thisptr != NULL
        return tycommon.opoint3d2point3d(self.thisptr.position)



from libcpp.map cimport map
from libcpp.vector cimport vector

from tympan.core cimport SmartPtr, OGenID, SolverInterface
from tympan.models cimport business as tybusiness
from tympan.models cimport common as tycommon
from tympan.models cimport solver as tysolver

cdef class Business2MicroSource:
    cdef map[tybusiness.TYElem_ptr, vector[SmartPtr[tybusiness.TYGeometryNode]]] map_sources

cdef business2microsource(map[tybusiness.TYElem_ptr, vector[SmartPtr[tybusiness.TYGeometryNode]]] map_elt_srcs)
cdef build_mesh(tysolver.ProblemModel model, tybusiness.Site site, tybusiness.Computation comp)
cdef build_sources(tysolver.ProblemModel model, tybusiness.Site site,
                   tybusiness.Computation comp)
cdef build_receptors(tysolver.ProblemModel model, tybusiness.Site site,
                     tybusiness.Computation comp)

cdef extern from "Tympan/models/business/TYPluginManager.h" namespace "tympan":
    SolverInterface* load_solver(const char *path, OGenID uuid)

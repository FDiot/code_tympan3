"""core objects for Code_TYMPAN, headers
"""

from libcpp.string cimport string
from libcpp cimport bool
from tympan.models._solver cimport AcousticProblemModel, AcousticResultModel, SolverConfiguration

# STL ##########################################################################

cdef extern from "boost/shared_ptr.hpp" namespace "boost":

    cdef cppclass shared_ptr[T]:
        shared_ptr(T*)
        shared_ptr()
        T *get()

cdef extern from "memory" namespace "std":

    cdef cppclass unique_ptr[T]:
        T *get()


# Qt ###########################################################################

cdef extern from "Qt/qstring.h":
    cdef cppclass QString:
        string toStdString() const
        QString(const char*)

# Tympan #######################################################################

cdef extern from "Tympan/core/smartptr.h":
    cdef cppclass SmartPtr[T]:
        SmartPtr()
        SmartPtr(T*)
        T* getRealPointer()
        T* _pObj

cdef extern from "Tympan/core/idgen.h":
    cdef cppclass OGenID:
        OGenID(const OGenID& other)
        OGenID()
        OGenID(const QString& string_id)
        const QString toString() const

cdef extern from "Tympan/core/interfaces.h":
    cdef cppclass SolverInterface:
        SolverInterface()
        void purge()
        bool solve(const AcousticProblemModel& aproblem,
                   const AcousticResultModel& aresult,
                   SmartPtr[SolverConfiguration])

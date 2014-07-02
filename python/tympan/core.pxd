"""core objects for Code_TYMPAN, headers
"""

from libcpp.string cimport string


# STL ##########################################################################

cdef extern from "boost/shared_ptr.hpp" namespace "boost":

    cdef cppclass shared_ptr[T]:
        T *get()

cdef extern from "memory" namespace "std":

    cdef cppclass unique_ptr[T]:
        T *get()


# Qt ###########################################################################

cdef extern from "Qt/qstring.h":
    cdef cppclass QString:
        string toStdString()


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

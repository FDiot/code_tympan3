from libcpp.string cimport string

# Element Wrapping
cdef extern from "totolib.hpp":
    cdef cppclass MyElement:
        MyElement()
        MyElement(string)
        string Name()

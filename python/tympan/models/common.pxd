"""shared models for Code_TYMPAN, headers
"""

from libcpp cimport bool
from libcpp.string cimport string
from libcpp.deque cimport deque

from tympan cimport core as tycore

cdef extern from "Tympan/models/common/3d.h":

    cdef cppclass OMatrix:
        pass
    OPoint3D dot "operator*"(const OMatrix& mat, const OPoint3D& point)
    # because "import operator*" triggers a syntax error

    cdef cppclass OCoord3D:
        double _x
        double _y
        double _z
        double _value[3]
        bool operator==(const OCoord3D& coord)

    cdef cppclass OPoint3D (OCoord3D):
        OPoint3D(double x, double y, double z)
        OPoint3D()

    cdef cppclass OVector3D (OCoord3D):
        pass


cdef extern from "Tympan/models/common/triangle.h":

    cdef cppclass OTriangle:
        OTriangle(int p1, int p2, int p3)
        OPoint3D _A
        OPoint3D _B
        OPoint3D _C
        int _p1
        int _p2
        int _p3
        bool checkConsistencyWrtPointsTab(const deque[OPoint3D]& points) const
        string reportInconsistencyWrtPointsTab(const deque[OPoint3D]& points)


cdef extern from "Tympan/models/common/spectre.h":

    const unsigned int TY_SPECTRE_DEFAULT_NB_ELMT

    cdef enum TYSpectreType:
        SPECTRE_TYPE_ATT
        SPECTRE_TYPE_ABSO
        SPECTRE_TYPE_LW
        SPECTRE_TYPE_LP
        SPECTRE_TYPE_AUTRE

    cdef cppclass OSpectre:
        OSpectre()
        bool operator==(const OSpectre& other) const
        bool operator!=(const OSpectre& other) const
        OSpectre toDB()
        void setType(TYSpectreType type)
        double * getTabValReel()
        unsigned int getNbValues() const
        void setDefaultValue(const double& valeur)
        OSpectre sum(const OSpectre& spectre) const
        bool isValid() const
        void setValid(const bool& valid)

    cdef cppclass OSpectreComplex (OSpectre):
        OSpectreComplex()
        OSpectreComplex(const OSpectre& other)


cdef class Spectrum:
    cdef OSpectre thisobj

cdef class Point3D:
    cdef OPoint3D thisobj

cdef class Triangle:
    # because OTriangle has no default constructor so we can't have an OTriangle
    # allocated object as data member
    cdef OTriangle* thisptr


cdef ospectre2spectrum(OSpectre os)
cdef opoint3d2point3d(OPoint3D pt)
cdef otriangle2triangle(OTriangle* tri)

cdef class SolverInterface:
    cdef tycore.SolverInterface* thisptr

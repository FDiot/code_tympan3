"""shared models for Code_TYMPAN, headers
"""

from libcpp cimport bool
from libcpp.string cimport string
from libcpp.deque cimport deque
from libcpp.vector cimport vector

from tympan cimport _core as tycore


cdef extern from "Tympan/models/common/spectrum_matrix.h" namespace "tympan":
    cdef cppclass SpectrumMatrix:
        SpectrumMatrix()
        SpectrumMatrix(const SpectrumMatrix& matrix)
        const vector[OSpectre]& by_receptor(size_t receptor_idx) const
        OSpectre& element "operator()"(size_t receptor_idx, size_t sources_idx)
        void setSpectre(size_t receptor_idx, size_t sources_idx, OSpectre spectrum)
        void resize(size_t nb_receptors, size_t nb_sources)
        size_t nb_sources() const
        size_t nb_receptors() const
        void clearReceptor(size_t receptor_idx)

cdef extern from "Tympan/models/common/acoustic_path.h":
    cdef cppclass acoustic_path:
        acoustic_path()

cdef extern from "Tympan/models/common/3d.h":

    cdef cppclass OMatrix:
        OMatrix dot "operator*"(const OMatrix& mat)
    OPoint3D dot "operator*"(const OMatrix& mat, const OPoint3D& point)
    OVector3D dot "operator*"(const OMatrix& mat, const OVector3D& point)
    # because "import operator*" triggers a syntax error

    cdef cppclass OCoord3D:
        OCoord3D()
        OCoord3D(double x, double y, double z)
        double _x
        double _y
        double _z
        double _value[3]
        bool operator==(const OCoord3D& coord)

    cdef cppclass OPoint3D (OCoord3D):
        OPoint3D()
        OPoint3D(double x, double y, double z)

    cdef cppclass OVector3D (OCoord3D):
        OVector3D(double x, double y, double z)
        OVector3D()
        OVector3D(const OVector3D &)


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

    cdef enum TYSpectreEtat:
        SPECTRE_ETAT_DB
        SPECTRE_ETAT_LIN

    cdef enum TYSpectreType:
        SPECTRE_TYPE_ATT
        SPECTRE_TYPE_ABSO
        SPECTRE_TYPE_LW
        SPECTRE_TYPE_LP
        SPECTRE_TYPE_AUTRE

    cdef cppclass OSpectre:
        OSpectre()
        OSpectre(double defaultValue)
        OSpectre(const double* valeurs, unsigned nbVal, unsigned decalage)
        bool operator==(const OSpectre& other) const
        bool operator!=(const OSpectre& other) const
        OSpectre toDB()
        OSpectre toGPhy()
        TYSpectreType getType()
        void setType(TYSpectreType type)
        TYSpectreEtat getEtat()
        void setEtat(TYSpectreEtat etat)
        double * getTabValReel()
        unsigned int getNbValues() const
        double getValueReal(double freq)
        void setDefaultValue(const double& valeur)
        OSpectre sum(const OSpectre& spectre) const
        bool isValid() const
        void setValid(const bool& valid)
        void printme() const
        double valGlobDBA()

    cdef cppclass OSpectreComplex (OSpectre):
        OSpectreComplex()
        OSpectreComplex(const OSpectre& other)

cdef class Spectrum:
    cdef OSpectre thisobj

cdef class Point3D:
    cdef OPoint3D thisobj

cdef class Vector3D:
    cdef OVector3D thisobj

cdef class Triangle:
    # because OTriangle has no default constructor so we can't have an OTriangle
    # allocated object as data member
    cdef OTriangle* thisptr


cdef ospectre2spectrum(OSpectre os)
cdef opoint3d2point3d(OPoint3D pt)
cdef ovector3d2vector3d(OVector3D pt)
cdef otriangle2triangle(OTriangle* tri)

cdef class SolverInterface:
    cdef tycore.SolverInterface* thisptr

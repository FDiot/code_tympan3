"""Shared models for Code_TYMPAN"""

import cython as cy
from cython.view cimport array as cyarray

import numpy as np
cimport numpy as np


cdef ospectre2spectrum(OSpectre os):
    """Spectrum (cython object) wrapping an OSpectre (c++)"""
    spectrum = Spectrum()
    spectrum.thisobj = os
    return spectrum

cdef opoint3d2point3d(OPoint3D pt):
    """Point3D (cython object) wrapping a Point3D (c++)"""
    point = Point3D()
    point.thisobj = pt
    return point

cdef ovector3d2vector3d(OVector3D vect):
    """Vector3D (cython object) wrapping a OVector3D (c++)"""
    vector = Vector3D()
    vector.thisobj = vect
    return vector

cdef otriangle2triangle(OTriangle * tri):
    """Triangle (cython object) wrapping an OTriangle(c++)"""
    assert tri != NULL
    triangle = cy.declare(Triangle, Triangle())
    triangle.thisptr = tri
    return triangle

cdef OPoint3D cypoint2cpp(cy_point):
    """ Build a TYPoint from a Point3D object """
    cpp_point = cy.declare(OPoint3D)
    cpp_point._x = cy_point.x
    cpp_point._y = cy_point.y
    cpp_point._z = cy_point.z
    return cpp_point

cdef class Spectrum:

    def __cinit__(self, np.ndarray[double, ndim=1] values=None):
        """Build a Spectrum, possibly out of ndarray `values`"""
        if values is None:
            return
        self.thisobj = OSpectre( < double * > values.data, len(values), 0)
        self.thisobj.setEtat(SPECTRE_ETAT_DB)
        self.thisobj.setType(SPECTRE_TYPE_LW)

    @classmethod
    def constant(cls, value):
        """Build a Spectrum instance from a constant `value`."""
        return cls(np.ones(31) * value)

    def __len__(self):
        return len(self.values)

    def __mul__(self, other):
        newvalues = self.values * other
        if newvalues is NotImplemented:
            return NotImplemented
        return self.__class__(newvalues)

    @property
    def nvalues(self):
        """Number of values contained in the spectrum"""
        return self.thisobj.getNbValues()

    @property
    def values(self):
        """Values of the spectrum"""
        cdef cyarray cy_array = <double[:self.nvalues] > self.thisobj.getTabValReel()
        spec_val = np.array(cy_array, dtype=np.double)
        return spec_val

    def to_dB(self):
        """Convert the spectrum to a dB scale (by default: linear)"""
        return ospectre2spectrum(self.thisobj.toDB())

    def to_GPhy(self):
        return ospectre2spectrum(self.thisobj.toGPhy())

    @property
    def dBA(self):
        """Balanced spectrum value in DBA (for audible frequencies)"""
        return self.thisobj.valGlobDBA()

    @property
    def dBLin(self):
        return self.thisobj.valGlobDBLin()



cdef class Triangle:

    def __cinit__(self):
        self.thisptr = NULL

    @property
    def p1(self):
        return self.thisptr._p1

    @property
    def p2(self):
        return self.thisptr._p2

    @property
    def p3(self):
        return self.thisptr._p3


cdef class Point3D:

    def __init__(self, x=0, y=0, z=0):
        self.thisobj._x = x
        self.thisobj._y = y
        self.thisobj._z = z

    def __repr__(self):
        return '({x}, {y}, {z})'.format(x=self.x, y=self.y, z=self.z)

    def set_x(self, x):
        self.thisobj._x = x

    def set_y(self, y):
        self.thisobj._y = y

    def set_z(self, z):
        self.thisobj._z = z

    @property
    def x(self):
        return self.thisobj._x

    @property
    def y(self):
        return self.thisobj._y

    @property
    def z(self):
        return self.thisobj._z


cdef class Vector3D:

    @property
    def vx(self):
        return self.thisobj._x

    @property
    def vy(self):
        return self.thisobj._y

    @property
    def vz(self):
        return self.thisobj._z

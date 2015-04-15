"""Shared models for Code_TYMPAN"""

import cython as cy
from cython.view cimport array as cyarray

import numpy as np


def make_spectrum(np_array):
    """ Make a Spectrum from an array of double values """
    assert len(np_array) == 31
    spectre = Spectrum()
    spectre.thisobj = OSpectre()
    cdef cyarray cy_array = <double[:len(np_array)]> spectre.thisobj.getTabValReel()
    for i in range(len(np_array)):
        cy_array[i] = np_array[i]

    return spectre

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

cdef otriangle2triangle(OTriangle* tri):
    """Triangle (cython object) wrapping an OTriangle(c++)"""
    assert tri != NULL
    triangle = cy.declare(Triangle)
    triangle = Triangle()
    triangle.thisptr = tri
    return triangle



cdef class Spectrum:

    @property
    def nvalues(self):
        """Number of values contained in the spectrum"""
        return self.thisobj.getNbValues()

    @property
    def values(self):
        """Values of the spectrum"""
        cdef cyarray cy_array = <double[:self.nvalues]> self.thisobj.getTabValReel()
        spec_val = np.array(cy_array, dtype=np.double)
        return spec_val

    def to_dB(self):
        """Convert the spectrum to a dB scale (by default: linear)"""
        return ospectre2spectrum(self.thisobj.toDB())

    @property
    def dBA(self):
        """Balanced spectrum value in DBA (for audible frequencies)"""
        return self.thisobj.valGlobDBA()


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

    @property
    def x(self):
        return self.thisobj._x

    @property
    def y(self):
        return self.thisobj._y

    @property
    def z(self):
        return self.thisobj._z

"""Shared models for Code_TYMPAN"""

import cython as cy
from cython.view cimport array as cyarray

import numpy as np


cdef ospectre2spectrum(OSpectre os):
    """factory function: return a Spectrum (python object) from an OSpectre (cpp
    lib)
    """
    spectrum = Spectrum()
    spectrum.thisobj = os
    return spectrum


cdef class Spectrum:
    thisobj = cy.declare(OSpectre)

    @property
    def nvalues(self):
        """ Return the number of values contained in the spectrum
        """
        return self.thisobj.getNbValues()

    @property
    def values(self):
        """ Return the values of the spectrum
        """
        cdef cyarray cy_array = <double[:self.nvalues]> self.thisobj.getTabValReel()
        spec_val = np.array(cy_array, dtype=np.double)
        return spec_val

    def to_dB(self):
        """ Convert the spectrum to a dB scale
        """
        return ospectre2spectrum(self.thisobj.toDB())


cdef class Point3D:
    thisobj = cy.declare(OPoint3D)

    @property
    def x(self):
        return self.thisobj._x

    @property
    def y(self):
        return self.thisobj._y

    @property
    def z(self):
        return self.thisobj._z


import unittest
import numpy as np
import os
import sys

try:
    # Python 2
    from StringIO import StringIO
except ImportError:
    # Python 3
    from io import StringIO
from math import log10
from utils import TympanTC
from _util import operation_array, mesh_spect_to_ndarray, ndarray_to_mesh_spect
from _util import (TOOLBOX_DATA_DIR)
from tympan.models._common import Spectrum
from ope_rec_surf import main, load_tympan_xml


''' From page 22 of http://www.mayenne.gouv.fr/content/download/11981/74880/file/12)%20Annexe%2008%20Mesure%20Initiale%20Bruit%20M%C3%A9thanisation%20Craon%20(201.pdf '''


def difference(Lres, Lbrut):
   return Lbrut-Lres


def emergence(Lres, Lbrut):
    Lamb = bruit_ambiant(Lres, Lbrut)
    return Lamb-Lres    


def bruit_ambiant(Lres, Lbrut):
    return 10*log10(10**(Lres/10)+10**(Lbrut/10))  # Division entiere en Python2, division reelle en Python3 !
    # return 10*log10(10**(0.1*Lres)+10**(0.1*Lbrut))


class Test(TympanTC):
       
    def test_difference_operation(self):
        """ Test array operations (E/Lot1-008) """
        ndarray_ref  = np.array([0, 1, 2])
        ndarray_calc = np.array([2, 3, 4])
        # Difference
        np.testing.assert_array_equal(operation_array(ndarray_ref, ndarray_calc, 0), list(map(difference, ndarray_ref, ndarray_calc)))

    def test_emergence_operation(self):
        """ Test array operations (E/Lot1-008) """
        ndarray_ref  = np.array([0, 1, 2])
        ndarray_calc = np.array([2, 3, 4])
        # Emergence
        np.testing.assert_array_equal(operation_array(ndarray_ref, ndarray_calc, 1), list(map(emergence, ndarray_ref, ndarray_calc)))

    def test_ambiant_noise_operation(self):
        """ Test array operations (E/Lot1-008) """
        ndarray_ref  = np.array([0, 1, 2])
        ndarray_calc = np.array([2, 3, 4])
        # Ambiant noise
        np.testing.assert_array_equal(operation_array(ndarray_ref, ndarray_calc, 2), list(map(bruit_ambiant, ndarray_ref, ndarray_calc)))

    def test_mesh_spect_to_ndarray_ndarray_to_mesh_spect(self):
        """ Test conversion between array and spectrum (E/Lot1-009) """
        mesh_spectrums = list()
        mesh_spectrums.append(Spectrum(np.array([100.0] * 31)))
        mesh_spectrums.append(Spectrum(np.array([200.0] * 31)))
        mesh_spectrums.append(Spectrum(np.array([300.0] * 31)))
        ndarray = mesh_spect_to_ndarray(mesh_spectrums)
        returned_mesh_spectrums = ndarray_to_mesh_spect(ndarray)
        # Check the conversion to ndarray then back to mesh_spect
        for i in range(len(mesh_spectrums)):
            np.testing.assert_array_equal(mesh_spectrums[i].values, returned_mesh_spectrums[i].values)

    def test_import(self):
        """ Global test (E/Lot1-011) """
        project = load_tympan_xml(os.path.join(TOOLBOX_DATA_DIR,"Calculs_Recepteurs_Surfaciques","Recepteur_surfacique.xml"))
        # Mesh
        mesh = project.meshes[0]
        assert(mesh.is_active)
        # Receptor
        receptor = mesh.receptors[0]
        assert(receptor.is_control_point)
        # Calculation
        calc = project.computations[0]
        assert(receptor.is_active(calc))

    def test_global(self):
        """ Global test (E/Lot1-012) """
        fichier_xml = os.path.join(TOOLBOX_DATA_DIR, "Calculs_Recepteurs_Surfaciques", "Recepteur_surfacique.xml")
        output_xml = "Result_all_operations.xml"
        # Tip to provide anwser to raw_input
        stdin_saved = sys.stdin
        input = ""
        input += "0\n"  # Selected mesh
        input += "1\n"  # Selected first computation
        input += "2\n"  # Selected second computation
        input += "0\n"  # Selected first operation
        input += "1\n"  # Selected second operation
        input += "2\n"  # Selected third operation
        input += "0\n"  # Select again first operation (no new calculation should be added)
        input += "y\n"  # Confirm deletion of the calculation
        input += "3\n"  # Exit
        f = StringIO(input)
        sys.stdin = f
        main(fichier_xml, output_xml)
        f.close()
        sys.stdin = stdin_saved
        # Load the result
        project = load_tympan_xml(output_xml)
        # Number of computations
        assert len(project.computations) == 5
        # Take last one
        last_calc = project.computations[len(project.computations)-1]
        project.select_computation(last_calc)
        mesh = project.meshes[0]
        mesh_spectrums = last_calc.get_noise_map_spectrums(mesh)
        assert len(mesh_spectrums) == 56
        # Take last spectrum
        last_spectrum = mesh_spectrums[len(mesh_spectrums)-1]
        expected =[50.4,50.3,50.3,50.4,50.4,50.4,50.4,50.3,50.2,50.2,50.1,50.1,50.0,49.9,49.8,49.7,49.6,49.4,49.2,49.0,48.7,48.2,47.8,47.1,46.1,44.8,43.0,40.4,36.8,31.6,24.1]
        # Check equal on 5 decimals:
        np.testing.assert_almost_equal(expected, last_spectrum.values, decimal=5)


if __name__ == '__main__':
    os.chdir(os.path.join(TOOLBOX_DATA_DIR, 'Calculs_Recepteurs_Surfaciques'))
    unittest.main()

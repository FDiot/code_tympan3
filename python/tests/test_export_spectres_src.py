import unittest
import numpy as np
import os
from utils import TympanTC

from tympan.models.solver import Model
from _util import compare_txt_results, run_calculations, TOOLBOX_DATA_DIR
from export_spectres_src import main, calc_surf_spectrum, calc_vol_spectrum

class Test(TympanTC):

    def test_non_regression(self):
        """ Check the results compared to expected ones (E/Lot1-037) """
        main("Projet_Machine_multiBoites.xml", "Spectres_sources")
        compare_txt_results("Spectres_sources_parSurface.txt", "ref_Spectres_sources_parSurface.txt")
        compare_txt_results("Spectres_sources_parVolume.txt", "ref_Spectres_sources_parVolume.txt")

    def test_global(self):
        """ Global test (E/Lot1-038) """
        project = run_calculations("Projet_Machine_multiBoites.xml")
        model = Model.from_project(project)
        list_src = model.sources
        # Compute the spectrum per face:
        (src_spectrums, src_ids) = calc_surf_spectrum(list_src)
        # Check the spectrum are the sames:
        i = 0
        for (val, id) in zip(src_spectrums, src_ids):
            np.testing.assert_array_equal(val, list_src[i].spectrum.values)
            i += 1

if __name__ == '__main__':
     os.chdir(os.path.join(TOOLBOX_DATA_DIR, 'Export_Spectres_Sources'))
     unittest.main()


import unittest

import numpy as np

from tympan.models import Spectrum


class SpectrumTC(unittest.TestCase):

    def test_constant(self):
        s = Spectrum.constant(1.2)
        self.assertEqual(s.values, np.ones(31) * 1.2)

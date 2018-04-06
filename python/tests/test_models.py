import unittest

import numpy as np

from tympan.models import Spectrum


class SpectrumTC(unittest.TestCase):

    def test_constant(self):
        s = Spectrum.constant(1.2)
        self.assertEqual(s.values, np.ones(31) * 1.2)

    def test_multiply(self):
        s1 = Spectrum.constant(1.2)
        s2 = s1 * 2
        self.assertEqual(s2.values, np.ones(31) * 2.4)

.. _api-part:

============================
C++ documentation by Doxygen
============================

This section provides the Doxygen_ documentation of `Tympan C++ source files <../doxygen/html/index.html>`_.

.. _Doxygen: http://www.stack.nl/~dimitri/doxygen/

===================================
C++ documentation by Breathe (ToDo)
===================================

This section contains the documentation extracted from the Doxygen
comments embedded in the code thanks to breathe_ (cf. :doc:`tools`).

.. _breathe: http://breathe.readthedocs.io/en/latest/
       
Example with a class (commented cause Breathe 1.2.0 doesn't support Python3):

... doxygenclass:: BruteForceAccelerator
...   :members:
...   :no-link:
   
... doxygenfunction:: BruteForceAccelerator

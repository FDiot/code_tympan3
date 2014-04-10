.. _build-system:

==============================
  Build system configuration
==============================

The build system used in Code_TYMPAN is CMake (see
:ref:`devel-tools-cmake`). This section does not explain how CMake
works but how it is used in Code_TYMPAN. In order to benefit from it,
it is recommended you ensure you have basic working knowledge of CMake.


The main entry point of the build system is the file :file:`CMakeLists.txt`
which is to be kept as synthetic as possible ; most build options are
documented and preset in the file :file:`build-options.cmake`.

.. note:: Do not edit :file:`build-options.cmake` !

  In case you need to tune Code_TYMPAN build to your own set-up, you
  might be tempted to edit :file:`build-options.cmake`. This is not
  the CMake way to customize a build. In this case you want to edit
  the CMake cache by either using ``ccmake`` or ``cmake-gui``. See
  `Running CMake <http://www.cmake.org/cmake/help/runningcmake.html>`_
  regarding this matter.

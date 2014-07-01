.. _build-system:

==============================
  Build system configuration
==============================

The build system used in Code_TYMPAN is CMake (see
:ref:`devel-tools-cmake`). This section does not explain how CMake
works but how it is used in Code_TYMPAN. In order to benefit from it,
it is recommended you ensure you have basic working knowledge of CMake.

Main entry point
================

The main entry point of the build system is the file :file:`CMakeLists.txt`
which is to be kept as synthetic as possible ; most build options are
documented and preset in the file :file:`tympan-build-options.cmake`.

.. note:: Do not edit :file:`tympan-build-options.cmake` !

  In case you need to tune Code_TYMPAN build to your own set-up, you
  might be tempted to edit :file:`cmake/tympan-build-options.cmake`. This is
  not the CMake way to customize a build. In this case you want to
  edit the CMake cache by either using ``ccmake`` or
  ``cmake-gui``. See `Running CMake`__ regarding this matter.

__   <http://www.cmake.org/cmake/help/runningcmake.html>`_


Platform handling
=================

The :file:`tympan-platform*.cmake` files are responsible for handling the
platform. The rationale is for :file:`tympan-platform.cmake` to detect
the platform, diagnose the unsupported ones and then call
:file:`tympan-platform-common.cmake` then
:file:`tympan-platform-XXX.cmake` for platform specific settings.

.. note:: The ``TYMPAN_COMPONENT_TYPE`` option

   Code_TYMPAN is split into components, main application and solver
   plugins. The components (DataManagerMetier and cie) must be
   compiled either as shared library or static ones depending on the
   platform. The option is thus declared in ``build-options`` and then set
   separately in each platform specific file.

The :file:`tympan-platform-common.cmake` file expects each
:file:`tympan-platform-XXX.cmake` file to define some hooks :
functions which will be called at later points requiring system
dependent actions. It should check that those hooks are actually
provided as is done for ``platform_install_hook``.

Third party dependencies
========================

As explained in :ref:`devel-installation` Code_TYMPAN has many
dependencies, some of which are shipped as a companions source package.
Those lightweight dependencies are built as `CMake external projects`__

__ `CMake external projects`: http://www.kitware.com/media/html/BuildingExternalProjectsWithCMake2.8.html

Some dependencies (CGAL and NMPB2008 notably) require the system to be
able to find some shared libraries / DLL *at run time*.  The
``TYMPAN_3RDPARTY_DLL_DIRS`` CMake variable is a list of paths which
gets built while configuring dependencies. It can then be used by
installation scripts or test scripts to setup the execution path.


Installation
============

The :file:`tympan-utils.cmake` file provides some macros related to
installation which are to be called by components or plugins to
perform their installation. Those macro just end up calling CMake's
``install`` command with the right paths and options.

The :file:`tympan-install.cmake` configures the overall installation
process, call the platform specific installation hook and add an
``uninstall`` target.

Tests
=====

Most of the tests are currently implemented in C++ using GTest_, only
the GUI-related ones are implemented using QTest. In both cases the
test driver used is CTest_ which integrates very well with CMake.

The :file:`tympan-utils.cmake` notably provides the CMake function
``configure_gtest_target`` which is called on the test executable to
setup the right dependencies for GTest and register the test with
CTest_.

An analogous function is provided for QTest. Contrary to
``configure_gtest_target`` which configures an *existing* executable
target, ``add_qtest_executable`` creates itself a *new* executable
target from the sources: this allows it to launch MOC and add the
generated sources to the executable.


Pre-compiled headers
====================

Pre-compiled headers is a controversial technology which speeds up the
compilation process in some use cases. But it increases significantly
the build complexity, is highly platform specific and can even slow
the build down in some use cases. To know more about pre-compiled
header please see :

* Introduction : http://gamesfromwithin.com/the-care-and-feeding-of-pre-compiled-headers
* For GCC : http://gcc.gnu.org/onlinedocs/gcc/Precompiled-Headers.html
* For Visual Studio : http://msdn.microsoft.com/en-us/library/szfdksca.aspx

An **experimental** support for pre-compiled headers has been
developed [1] in Code_TYMPAN so that it is possible to benefit from
the hypothetical speed-up of builds. But because the benefit (and
complexity) they bring are so controversial, this support can be
enabled or disabled with the ``LOGILAB_USE_PRECOMPILED_HEADER`` build
option.

When ``LOGILAB_USE_PRECOMPILED_HEADER`` is set, the
:file:`cmake/PrecompiledHeader.cmake` file will be loaded to provide
the ``set_precompiled_header`` and ``use_precompiled_header`` CMake
functions. Otherwise it will define dummy function as a replacement to
allow their unconditional use in the main CMakeLists.

Most Code_TYMPAN components (``DataManagerMetier``...) provide an
*include everything* header (like :file:`TYPHMetier.h`) intended to
be precompiled, hence the ``PH``. Such headers' inclusion is typically
guarded by the preprocessor symbol ``TYMPAN_USE_PRECOMPILED_HEADER``.

But this header has been misused: most source files rely on it being
included instead of explicitely including the actually needed
headers. Thus making the application compile without
``TYMPAN_USE_PRECOMPILED_HEADER`` is not possible at the time of
this writing (April 2014) and will require significant future work.

The desired setting would be for``TYMPAN_USE_PRECOMPILED_HEADER`` to
be defined **only when** the build option
``LOGILAB_USE_PRECOMPILED_HEADER`` is defined.  Because this is not
yet possible the :file:`Tympan/CMakeLists.txt)` file forcibly defines
``TYMPAN_USE_PRECOMPILED_HEADER`` and emits a warning.

Avoid debug python library on Windows
=====================================

A specific additional step was require to solve the following problem
on Windows when building in *Debug*: ``python27_d.lib`` is not found
and actually does not exists and is not really needed. This is a known
issue related to Python header ``pyconfig.h`` using the evil MSVC
autolink functionality.

The problem is described and solved by boost::Python :
http://www.boost.org/doc/libs/1_53_0/libs/python/doc/building.html#id19

Other lighter solutions found on the Internet are not relevant for the
build system of Code_TYMPAN, especially desactivating the ``_DEBUG``
pre-processor symbol for building ``pytam``. Indeed ``pytam`` relies
on CGAL which does *not* support being included both with and without
``_DEBUG`` (and we definitely need debugging mode of CGAL).

The solution is to insert an ad-hoc patch step (implemented by the
``cython_patch.py`` script) in between the generation of ``pytam.cxx``
from ``pytam.pyx`` by Cython and the actual compilation of the file.
This patch step replaces the direct inclusion of ``pyconfig.h`` and
``Python.h`` by inclusion of custom wrappers (``tympan_wrap_*.h``)
which are adapted from those used by boost to solve the very same
problem (``boost/python/detail/wrap_python.hpp``).


.. References

.. _GTest: https://code.google.com/p/googletest/wiki/Documentation
.. _CTest: http://www.cmake.org/Wiki/CMake_Testing_With_CTest

.. [1] Although the best option would be for CMake to provide cross
       platform pre-compiled headers handling

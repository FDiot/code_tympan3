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
documented and preset in the file :file:`build-options.cmake`.

.. note:: Do not edit :file:`build-options.cmake` !

  In case you need to tune Code_TYMPAN build to your own set-up, you
  might be tempted to edit :file:`cmake/build-options.cmake`. This is
  not the CMake way to customize a build. In this case you want to
  edit the CMake cache by either using ``ccmake`` or
  ``cmake-gui``. See `Running CMake`__ regarding this matter.

__   <http://www.cmake.org/cmake/help/runningcmake.html>`_


Platform handling
=================

The :file:`tympan-platform*.cmake` are responsible for handling the
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

The :file:`tympan-platform-common.cmake` expects each
:file:`tympan-platform-XXX.cmake` to define some hooks : functions
which will be called at later points requiring system dependent
actions. It should check that those hooks are actually provided as is
done for ``platform_install_hook``.

Third party dependencies
========================

As explained in :ref:`devel-installation` Code_TYMPAN has many
dependencies, some of which are shipped as a companions source package.
Those lightweight dependencies are built as `CMake external projects`__

__ `CMake external projects`: http://www.kitware.com/media/html/BuildingExternalProjectsWithCMake2.8.html

Some dependencies (CGAL and NMPB2008 notably) require the system to be
able to find some shared libraries / DLL *at run time*. A path must be
built while configuring dependencies so that it can be used when
launching the application : the ``TYMPAN_3RDPARTY_DLL_DIRS`` CMake
variable is used for this purpose.

Installation
============

The :file:`tympan-utils.cmake` provides some macros related to
installation which are to called by conponents or plugins to perform
their installation. Those macro just end up calling CMake's
``install`` command with the right paths and options.

The :file:`tympan-install.cmake` configures the overall installation
process, call the platform specific installation hook and add an
``uninstall`` target.

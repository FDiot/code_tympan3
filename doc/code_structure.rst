.. _code-structure:

Structure of the code base
==========================

First level hierarchy
---------------------

* ``Tympan``, C++ files hierarchy
* ``Python``, Python files hierarchy
* ``tests``, test files hierarchy
* ``doc``, project's developper documentation
* ``tools``, various developping utilities
* ``3rdparty``, third party code, to be filed with the dependencies archive's content
* ``resources``, GUI assets (images, fonts, etc.)
* ``cmake``, cmake scripts
* ``debian``, debian packaging


C++ code hierarchy (i.e. the `Tympan` directory)
------------------------------------------------

DISCLAIMER: this is a proposal of the *targeted* hierarchy. We should reach this
situation from the legacy one step by step.

The strong point considered here is that the layout must reflect the actual
dependencies between binary components of the application.

Also notice sub-directories will probably have to be inserted, but quoting Tim
Peters, remember that *flat is better than nested* (import this).

::

  Tympan
   \_ core
    _ models
     \_ site - application model (former `DataManagerMetier`)
      _ solver - problem and result models for the solver
      _ common - common representations (former `CommonTools`)
    _ solvers
     \_ default
      _ anime3d
    _ gui
     \_ widgets (former `DataManagerIHM`)
      _ gl (former `DataManagerGraphics`)
      _ app (former `TympanApp`)

* ``core`` may not include from other Tympan directories
* ``models`` may only include from ``core`` (and common representation ???)
* ``gui`` may only include from ``core`` and ``model/site``
* ``solvers`` may only include from ``core`, ``model/solver`` and ``model/results``


Python code hierarchy (i.e. the `Python` directory)
---------------------------------------------------

File hierarchy
~~~~~~~~~~~~~~

::

  Python
   \_ include
    _ tympan
     \
      _ __init__
      _ models
       \_ site
	_ solver
	_ results
      _ altimetry
      _ site2solver

This deliberately not specify yet which are module from which are
package. To-be-compiled Cython modules (.pyx / .pxd) should be named and located
according to the resulting python module.

Python namespaces
~~~~~~~~~~~~~~~~~

* ``tympan.models.site``
* ``tympan.models.solver``
* ``tympan.altimetry``
* ``tympan.site2solver``
* ``tympan.main.solve_project``
* ``tympan.main.compute_altimetry``


Test code hierarchy
-------------------

XXX to be completed

Mirror the code hiearchy - functional tests at the root.

Prefer writing tests in Python even for C++ code !


Executables
===========

XXX to be completed

* starts the GUI (binary)

* used by the GUI to start computation for a site XML file (python)

* used by the GUI to computate altimetry for a site XML file (python)

* used by the python library to start computation for a solver model (python
  thin wrapper around solvers code)

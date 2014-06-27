.. _debugging-sec:

====================================
 Debugging and diagnosis strategies
====================================

This section brings together resources of interest to debug and
diagnose problems be it tools, tricks, or strategies.

Some useful links
-----------------

* Edit CMake code from within Visual Studio:
  http://cmaketools.codeplex.com/

* Python and Hybrid Python/C++ integration in Visual Studio:
  http://pytools.codeplex.com/

* Hybrid debugging of Cython code on Linux with cygdb:
  http://docs.cython.org/src/userguide/debugging.html

* Attach to Running Processes with the Visual Studio Debugger:
  http://msdn.microsoft.com/en-us/library/3s68z0b3.aspx?ppud=4


Legacy code
-----------

Code_TYMPAN can still be run with the legacy code (i.e. without Python 
subprocess and without the solver data model) with the following option: *--legacy-computation*

.. note:: This is a temporary feature provided in order to ease the
    debugging and it should soon be removed.


Diagnose a computation failure
------------------------------

Look at the logs
~~~~~~~~~~~~~~~~
Three logs are available:

* A log is produced by ``OMessageManager``: before the GUI starts it is displayed
  in the console (if it exists) and then it is displayed in the ``Messages`` frame
  of the GUI (on the left)

* While solving the acoustic problem, the Python subprocess also generates a log.
  It is written in the file "*tympan.log*", using *logging*, and created in the current
  directory (which is the directory containing the XML project in the case of
  the computation of an existing project)

* Still while running the subprocess, the log emitted by the C++ libraries
  (produced by ``OMessageManager``) is kept but redirected to the console.


Is the environment correctly configured ?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The following environment variables are needed by Code_TYMPAN, and some of them
are directly set in the code (see ``Tympan/Appli/TympanApp/EnvironmentUtils.h``):

* ``PYTHONPATH``: should contain the directory of cython library ``pytam.so``/
  ``pytam.pyd`` (i.e. ``cython`` in release mode, ``cython_d`` in debug mode).
  *It is set by TYMPAN executable when running it from the install directory,
  and by CMake  when running the tests with ctest from the build directory*

* Path to libraries:

  * Under Windows, as TYMPAN libraries are statically linked to the executable,
    there is nothing to do regarding this point. However, this is not the case
    of the 3rd party libraries. When running Code_TYMPAN legacy code, as these
    libraries are in the same directory as ``Tympan.exe``, they are found. But
    when trying to access them through cython library which is located in another
    directory it is not the case. That is why the ``PATH`` environment variable
    must contain this directory. *It is set by TYMPAN executable when running 
    it from the install directory, and by CMake when running the tests with ctest
    from the build directory*

  * Under Linux, TYMPAN libraries are dynamically linked to the executable and
    therefore the ``LD_LIBRARY_PATH`` environment variable must indicate where
    to find them. *This is not done in Code_TYMPAN and must be set by the user.*

* Path to Python interpreter (Windows specific):
  to run the Python script, the complete path to the Python interpreter is
  needed (``python solve_project.py`` is not interpreted). Yet, there is no standard
  under Windows regarding where to install python, so this path cannot be known
  from Code_TYMPAN. That is why ``TYMPAN_PYTHON_INTERP`` environment variable
  must be set by the user (for example: "C:\\Python27\\python.exe").
  If it is not set, this error will appear in the TYMPAN log: 
  "*Can't access python interpreter. TYMPAN_PYTHON_INTERP environment variable is not set.*"
  If the environment variable exists but does not lead to the python interpreter, 
  the following message will be logged:
  "*Can't access python interpreter. TYMPAN_PYTHON_INTERP environment variable is not correctly set.*"


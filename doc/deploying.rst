.. _devel-deploying:

=================
Windows installer
=================

Overview
========

The windows installer is generated with `Inno Setup`_. The generation script can
be found in ``tools`` (``tympan_installer.iss``).

.. _`Inno Setup`: http://www.jrsoftware.org/isinfo.php

There are 4 main steps during the installation:

* Copy of Code_TYMPAN install directory

* Copy of a custom Python27 directory

* Copy of Microsoft Visual C++ 2010 Redistributable Package libraries (vcredist)

* Setting the needed environment variables

To generate the installer, just open ``tympan_installer.iss`` with `Inno Setup`_
and click on "Build". The installer will be created in an ``Output`` directory
in the same directory as the script. The script must be placed in the parent directory of
Tympan install directory before running it.

**Don't forget to modify the script in order to put the right path to the Python27
and vcredist directories if needed.**


Copy of Code_TYMPAN install files
---------------------------------

This step is simple: a ``Release`` installation of Code_TYMPAN has to be made
in a previously empty install directory.
All the files of the install directory will be copied into the installer, except
for generated files like *.pyc* files.


Building of the custom Python27 directory
-----------------------------------------

A custom python is made, embedding the correct version of the needed libraries.
It is based on the official Python 2.7 Windows installer retrieved from the python
website `download page`_ (choose ``Python 2.7.8 Windows Installer`` for a 32
bits installation).

Once the installation is done, the ``python27.dll`` library has to be copied from
``C:\Windows\System32`` to the newly created python directory (should be ``C:\Python27``).

Uppon this base are added (mostly using Windows installers):

Shapely (version 1.3)
            `Pypi installer`_ is convenient since it already embeds ``GEOS``,
            which is needed by shapely

Matplotlib (version 1.4)
            An unofficial windows binary can be found at `Gohlke website`_
            (choose ``matplotlib‑1.4.0.win32‑py2.7.exe``)

Numpy (version 1.8)
            An unofficial windows binary can be found at `Gohlke website`_
            (choose ``numpy‑MKL‑1.8.2.win32‑py2.7.exe``)

Cython (version 0.20.2)
            An unofficial windows binary can be found at `Gohlke website`_
            (choose ``Cython‑0.20.2.win32‑py2.7.exe``)

Setuptools
            It is needed for ``descartes`` installation (choose
            ``setuptools‑5.4.2.win32‑py2.7.exe``
            on `Gohlke website`_)

Descartes
            Must be installed after shapely, numpy and matplotlib since it depends on these
            libraries. It can be retrieved from the `Python Package Index`_ and
            installed with this command: ``python setup.py install`` where
            ``python`` must be the intepreter we've just installed (should be ``C:\Python27\python.exe``)

Scipy (version 0.14)
            An unofficial windows binary can be found at `Gohlke website`_


The resulting ``Python27`` directory won't be copied directly in C:\ in order to
avoid potential conflicts with another python installation. Therefore, it is
copied inside ``Code_TYMPAN`` install directory.

.. _`Download page`: https://www.python.org/download
.. _`Pypi installer`: http://pypi.python.org/pypi/Shapely
.. _`Gohlke website`: http://www.lfd.uci.edu/~gohlke/pythonlibs/
.. _`Python Package Index`:  https://pypi.python.org/pypi/descartes


Copy of Microsoft Visual C++ 2010 Redistributable Package libraries
-------------------------------------------------------------------

``Code_TYMPAN`` depends on Visual C++ packages to work correctly. These packages
are installed with Visual Studio and some other applications. But they might not
already be present in the target system. That is why they are copied in ``Code_TYMPAN``
directory (at the same level of the other dll, this way it is not necessary to modify the
path). They must be put in a ``vcredist`` directory at the same level as the script to be
copied.

A VC Redistributable Package installer can be found at this `Microsoft website download page`_. Don't forget
to choose the version corresponding to the current ``Visual Studio`` version
(for know, 2010).
If Visual Studio is installed, the dlls can also be found in
``C:\Program Files (x86)\Microsoft Visual Studio version\VC\Redist\platform\library\``.


.. _`Microsoft website download page`: http://www.microsoft.com/en-us/download/details.aspx?id=26999


Environment management
----------------------

For ``Code_TYMPAN`` to work on Windows, 2 environment variables must be set in the
user registry:

* *CGAL_BINDINGS_PATH* (path to CGAL bindings libraries)

* *TYMPAN_PYTHON_INTERP* (path to Python interpreter -- the custom one)

They are set by the installer at the end of the installation.


If it doesn't work
------------------

Some libraries could be missing. You can use the `dependency walker`_ to identify
the dependencies of a library.

.. _`dependency walker`: http://www.dependencywalker.com/


Uninstall step
--------------

By default, the entire ``Code_TYMPAN`` directory will be removed, including its embedded ``Python27``
directory.

*TYMPAN_PYTHON_INTERP* and *CGAL_BINDINGS_PATH* environment variables will also
removed from the registry.

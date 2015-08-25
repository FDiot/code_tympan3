.. _devel-installation:

==============
 Installation
==============

Overview
========

.. _devel-downloading:

Downloading
-----------

A Mercurial_ repository is available on Bitbucket_ : the stable branch
of development is at https://bitbucket.org/TYMPAN/code_tympan.

.. _Bitbucket: https://bitbucket.org/
.. _Mercurial: http://mercurial.selenic.com/

The project also requires some third party dependencies (see below for
more details).

Those dependencies are divided into heavy, potentially system-wide,
dependencies and lighter dependencies which can easily be
bundled in a source package along with Code_TYMPAN.

Lightweight dependencies
------------------------

Those dependencies are to be downloaded as a companion source package
and unzipped into the ``3rdparty`` directory. Please refer to the
``3rdparty/README`` file which provides the exact URL for your current
version of Code_TYMPAN.

Major dependencies
------------------

The following dependencies are a bigger deal as their are likely to be
already installed in your development environment in a system-wide
way. If you are running a Linux box, the best way to get them is
through your distribution package manager. If you are running a
Windows box, you should run the dedicated installers.

More details are given in further sections, on an OS and variant basis.

Boost
        Boost_ is a set of C++ libraries which provide a higher level
        of expressiveness and convenience for C++. A version 1.46 or
        newer is recommended.

Qt 4
        Qt_ is a toolkit for developing Graphical User Interface and
        providing portable and commonly used facilities such as
        multi-threading or reading XML files... Version 4.6 or newer
        is recommended.

OpenGL
        is a common 3D rendering API. It is mainly accessed through
        the QtOpenGL components.

CMake
        CMake_ is a multi platform built tool used to build Code_TYMPAN
        (and CGAL and many other OpenSource projects by the way). A
        version 2.8 or newer is required.

CGAL
        CGAL_ is a Computational Geometry library and itself depends
        upon both Boost and Qt4 and as such needs to be installed in a
        compatible way with your Qt and Boost installation.
        CGAL is used in the Python code for altimetry mesh triangulation and refinement. It is also used
        in the C++ code (business part) for infrastructure polygons triangulation.

        .. todo:: Specify version **required** ( 4.3.1 suggested)

                  - 4.0.5 is the version packaged on debian wheezy but
                    has broken CMake configuration scripts on Windows
                  - 4.1 has broken tuple support on Windows
                  - 4.2 and 4.3 bring improvement regarding triangulation
                  - 4.4 is quite young at the time of this writing (may 2014)

Python
        A Python_ interpreter is required, version 2.7

Cython
        Cython_ is a programming language inspired from Python, allowing to
        write Python code with C/C++ declarations. The cython code is then
        translated into C/C++ and compiled as a Python library. It is used in
        Code_TYMPAN to interface Python code with TYMPAN C++ libraries. A version
        0.20 or above is required.

Shapely
        Shapely_ is a Python package for manipulation and analysis of planar
        geometric objects. Version 1.3 is suggested.

SWIG
        Swig_ is a software development tool that connects programs written in
        C and C++ with a variety of high-level programming languages.
        It is needed by the 3rd party dependency *CGAL-bingings*, to connect
        C++ with Python code.


Documentation dependencies
--------------------------

The C++ source code is annotated and API documentation can be
extracted with Doxygen_ whereas this documentation  is generated from
reStructuredText_ sources thanks to Sphinx_. The breathe_ extension
(shipped within the 3rd party archive) provides a bridge between the
two and MathJax_ is used to nicely display equations.

 Doxygen_ and Sphinx_ need to be installed either from your package
 manager or from the official Windows installer. As there are no specific
 difficulties, please refer to their official documentations.

.. _reStructuredText: http://docutils.sourceforge.net/rst.html
.. _Sphinx: http://sphinx-doc.org/
.. _Doxygen: http://www.stack.nl/~dimitri/doxygen/
.. _breathe: http://michaeljones.github.com/breathe/
.. _MathJax: http://www.mathjax.org/


Ubuntu / Debian standard install
================================

This section introduces the installation from the package manager of a
Debian based distribution. It assumes a pretty recent version of the
system on the date of writing, so that most packages are available
from standard package repositories in a compatible version. Here we
use an *Ubuntu 12.04.1 LTS*.

**Installation of Boost** ::

  $ sudo aptitude install libboost1.48-all-dev libboost1.48-doc

In case an older version of boost is already installed you might have
to either complete [#]_ its installation or completely remove it to install
the most recent packaged version. If really none of these options is
practicable, a custom install of Boost from source is the solution
(see `Compiling Boost`_).

**Installation of Qt 4** ::

  $ sudo aptitude install libqt4-dev libqt4-opengl

**Installation of CMake** ::

  $ sudo aptitude install cmake cmake-curses-gui

**Installation of CGAL 4 :**

In *Ubuntu 12.04.1 LTS* as well as in *Debian squeeze* the version of
CGAL available in the package system is too old (it is a 3.x and a 4.x
is required).  It will thus be required for now to install CGAL 4 from the
sources (see `Compiling CGAL`_) but CGAL 4 should be available
soon in main distributions (beware then to install the nominal boost
version, against which CGAL would have been built, i.e.
libboost-all-dev, without any more recent version number).

Please note that CGAL depends upon some packages which can (and
should) be easily installed through the package manager, such as boost
and Qt which are already installed, but also the following one::

  $ sudo aptitude install libgmp-dev libmpfr-dev zlib1g-dev

*NB* : if the `aptitude` utility is not installed `apt-get` can be
used instead. And you will need to be a privileged user to execute
this command.

.. [#] A *complete* installation of Boost is not strictly required but
       is much easier than picking all the required components by hand.

**Installation of python and cython** ::

    $ sudo aptitude install python cython

**Installation of shapely** ::

    $ sudo aptitude install python-shapely

**Installation of swig** ::

    $ sudo aptitude install swig


Windows Seven
=============

*NB* : This section deals with the installation on Windows
Seven using the native Microsoft Visual Studio development
environment, which is assumed to be already installed.
Installation using MinGW_ / MSYS has not been attempted
and is not supported for now (note that this toolchain is supported by
CMake but that Boost is **officially unsupported** on MinGW / MSYS) !

The simplest way to install the development environment for Windows is
through the various installers. The point is some of the installers
require an Internet connection to download the appropriate components
depending on your system and option choices.

Boost

        BoostPro provides an installer for windows at :
        http://www.boostpro.com/download/

Qt 4

        Download the installer for the latest Qt 4 version and for
        your version of the toolchain (Visual 2008 or Visual 2010)
        from : http://qt-project.org/downloads

CMake
        http://www.cmake.org/cmake/resources/software.html

CGAL
        See the `Compiling CGAL`_ (no real binary installer available on Windows) :
        https://gforge.inria.fr/frs/download.php/32993/CGAL-4.3-Setup.exe

Cython
        Cython can be installed through *Python(x,y)*  and found at:
        http://code.google.com/p/pythonxy/wiki/StandardPlugins (follow the
        installation instructions)

Shapely

        Shapely can be installed through the dedicated executable
        installer, which contains the required GEOS DLL. It can be
        downloaded as a python(x,y) additional plugin from:
        http://code.google.com/p/pythonxy/wiki/AdditionalPlugins

        Currently you can get directly version 1.3.2-2 from :
        https://sourceforge.net/projects/python-xy/files/plugins/shapely-1.3.2-2_py27.exe

SWIG
        A prebuilt executable of SWIG can be retrieved from
        http://www.swig.org/download.html (download *swigwin-3.0.2*)

If you have no direct Internet connection (typically you are behind an
authenticating proxy) you will have to build Boost from source :
please refer to `Compiling Boost`_.

And you will have to manually download the CGAL sources then build
them according to `Compiling CGAL`_, **without forgetting** to manually
install GMP and MPFR.

The Tel Aviv University	provides some help for installing CGAL on Windows :
http://acg.cs.tau.ac.il/cgal-at-tau/installing-cgal-and-related-programs-on-windows

http://xinyustudio.wordpress.com/2012/07/18/setup-cgal-in-windows-7-step-by-step-tutorial/

http://xinyustudio.wordpress.com/2012/07/18/setup-cgal-in-windows-7-using-cmake-command-line/


**Beware** : under Windows, when setup is completed, **make sure that ``Pythonx,y - mingw``
doesn't appear in the path** (it could be something like ``C:\MinGW32-xy\bin``).


Compiling CGAL
==============

It is likely that CGAL 4 will need to be installed from source either
on Linux or on Windows. The steps are not that different are are thus
documented together in this section.

CGAL 4 depends upon some libraries amongst which Boost and Qt4 or
build tool such as CMake which are supposed to have already been
installed. It also depends on GMP and MPFR which are supposed to have
been installed through the package manager on Linux or through CGAL
installer on Windows.

The first step is to download the source archive from `CGAL
forge`_. Please download the windows installer ``CGAL-4.x-Setup.exe``
for Windows or any suitably compressed archive for Linux like
``CGAL-4.x.tar.bz2``. Then unzip it to some directory (e.g
``~/dists/CGAL-4.x``) and follow the build instruction, summarized
thereafter.

**NB:** The windows installer will download the GMP and MPFR dependencies and
install them for you. It will also donwload the CGAL sources and
suggest you to set your environment variables. You can follow this
guidance but in any case you still need to compile CGAL your-self :
the installer **does not** build and install CGAL !

Make a *build directory* **distinct** from the *source directory* in
which you unzipped the archive (typically a sibling directory) and
invoke CMake from within it :

  ~/dist$ mkdir build-CGAL-4.3
  ~/dist$ cd build-CGAL-4.3
  ~/dist/build-CGAL-4.3$ ccmake ../CGAL-4.3

You should have read some basic CMake documentation before going
further... Now suppose you have, by pressing ``c`` you launch the
configuration, then ``e`` to exit the messages screen, then you can
(or need to) edit some configuration parameters such as the base paths
to Boost, Qt4,...

Typically you want to tell CMake where you would like to *install*
CGAL by setting the ``CMAKE_INSTALL_PREFIX`` variable, and if you are
not a privileged user to **have to** change it to somewhere you can
write to. Then ``c`` and ``e`` again which should make the ``g``
option appear to generate the build system. Finally press ``g``.

**NB:** You can get the message ``Boost_DIR-NOTFOUND`` but no error in
case Boost is installed in default system path through the package
manager. Otherwise you can specify where to find Boost by setting the
*environment variable* ``BOOST_DIR`` before launching ``ccmake``.

Now you should be able to run ``make`` as usual. Please note that you
don't *have to* install CGAL as long as you tell CMake where to find
it when building Code_TYMPAN.

Compiling Boost
===============

Compiling Boost can be a complex process because there are many ways
to do it, depending on various platforms and needs... And the
documentation is quite confusing for the casual user. We try here to
summarise one simple way to do it which has been successfully used for
use with Code_TYMPAN. For more details you will have to refer to the
official `Boost Installation Documentation`_.

The most practical entry points for installing Boost from source are:

* on Linux (from `usual boost install on linux`_)::

    $ cd path/to/boost_1_xx_y
    boost_1_xx_y$ ./bootstrap.sh --prefix=path/to/installation/prefix
                                 --show-libraries
    $ ./b2 install

* on Windows (from `usual boost install on windows`_)::

    C:\<...>\boost_1_xx_y>bootstrap.bat
    C:\<...>\boost_1_xx_y>.\b2

  *NB* : Depending on Boost version you might have to use the older
  ``bjam`` **instead of** ``b2``.

Be it on Windows or on Linux you don't *have to* install Boost, you
can just build it and use it from its staging directory. For this to
work you **have to set the ``BOOST_ROOT`` environment variable** to
the directory where you unpacked the sources and ran ``bootstrap`` and
``b2``.

.. _`usual boost install on linux`: http://www.boost.org/doc/libs/1_53_0/more/getting_started/unix-variants.html#easy-build-and-install
.. _`usual boost install on windows` : http://www.boost.org/doc/libs/1_53_0/more/getting_started/windows.html#or-simplified-build-from-source

Building Code_TYMPAN
====================

This section assumes you have already checked-out from your repository
or unzipped from an archive the source code for Code_TYMPAN into a
directory, e.g. ``~/projets/Code_TYMPAN``. And downloaded the
third-party package and unzipped it into the
``~/projets/Code_TYMPAN/3rdparty`` directory.

Then create a build directory (typically a sibling of the source
directory) and run CMake in it, telling it where you have **built**
(or installed) CGAL and possibly Boost ::

  ~/projets$ mkdir build-Code_TYMPAN
  ~/projets$ cd build-Code_TYMPAN
  ~/projets/build-Code_TYMPAN$  ccmake ../Code_TYMPAN

CMake is likely not to find CGAL 4, and possibly Boost. You have to
set some CMake variables (or alternatively environment variables) in
order to help it find them, e.g.  ``CGAL_DIR=~/dist/build-CGAL-4.3``.
You also have to tell CMake where it will install Code_TYMPAN thanks
to the ``CMAKE_INSTALL_PREFIX`` variable
(e.g. ``~/projets/install-Code_TYMPAN``).

Then configure again (``c``) and generate the Makefile or Visual
Studio solution (``g``). You can now build it with ``make`` or by
opening the solution.

.. note:: More about the CMake build system

  If you want to add components or otherwise change the build system
  for Code_TYMPAN you might want to have a look at :ref:`Build System
  Documentation <build-system>` first.



.. _Boost: http://www.boost.org/
.. _Qt: http://qt-project.org/
.. _CMake: http://www.cmake.org/
.. _CGAL: http://www.cgal.org/
.. _MinGW: http://www.mingw.org/
.. _Python: http://www.python.org/
.. _Cython: http://www.cython.org/
.. _Shapely: http://pypi.python.org/pypi/Shapely
.. _Swig: http://www.swig.org/

.. _`CGAL forge` : https://gforge.inria.fr/frs/?group_id=52

.. _`Boost Installation Documentation`: http://www.boost.org/doc/libs/1_53_0/more/getting_started/index.html

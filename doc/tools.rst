.. _devel-tools:

=======
 Tools
=======

.. :authors: Anthony Truchet <anthony.truchet@logilab.fr>
          Damien Garaud <damien.garaud@logilab.fr>

This section provides some introduction to using third-party tools or
libraries on which Code_TYMPAN relies. Those dependencies are
introduced, with installation instructions in the section
:ref:`devel-installation`. In this section we give some entry-points
regarding use of the tools and libraries.

We also explain some *not so common* C++ idoms used in some portion of
the code.


Accessing the source-code
=========================

The source code of the Code_TYMPAN project is versionned using the
Mercurial_ Distributed Version Control System. On GNU/Linux the simplest
way to install is through your distribution package manager. On
Windows the best alternative is to use the TortoiseHg_ application
which integrates well with the Explorer (also available on GNU/Linux).
A most useful tool is hgview_ which enables navigating through the
history both in a Qt GUI and in text mode.

The following *named branches* are currently used:

.. todo:: Keep the branches name and policies up-to-date.

default
    Is the current branch used to keep in sync the feature branches
    (i.e. tympan_RD and logilab for now). Indeed, it is critical to
    avoid any significant divergence regarding the build system for
    example. Moreover some changes (many bug fixes, documentation
    improvement) can be put in common without interfering.

v3.8
    is the branch for the production version 3.8

tympan_RD
    is the development branch for the new solvers and features,
    especially ANIME3D and support of the meteo. Only EDF R&D people are
    supposed to commit in this branch for now.

logilab
    is the development branch for most of the refactoring work done at
    Logilab and only Logilab's developers are supposed to commit to
    it, except for specifically coordinated tasks.

    Thus this branch is intended to cherry-pick build system evolutions,
    bug-fixes for the common parts, architectural changes,
    documentation... The typical work-flow is as follows:

        * Alice (works at Logilab) improves the build system
          (CMake). She validates the change and intially commit it the
          logilab branch.
        * She then cherry-pick this commit into the common branch.
        * She **must validate it on the common branch**, which should
          never be broken (for long).
        * She tells Bob, working with the AMA team at EDF R&D, that
          this change should be integrated as soon as possible.
        * Bob look at the changes done by the commit.
        * If everything looks fine he cherry-picks the commit into
          tympan_RD and validates it still works as expected.
        * In case there would be some incompatibility or unforeseen
          problem, Bod calls Alice and they solve it together.

v4.x.y(-zzz)
    would be the release and maintenance branch for the release of
    Code_TYMPAN version 4.x.y (variant zzz) for future public and
    maintained releases. Simple milestones releases, which marks the
    end of an agile iteration but not a significant version are not
    given a dedicated branch. Instead they are simply tagged.

Of course other feature branches can be added by need.

.. _Mercurial: http://mercurial.selenic.com/
.. _TortoiseHg: http://tortoisehg.bitbucket.org/
.. _hgview: http://www.logilab.org/project/hgview


.. _devel-tools-cmake:

CMake
=====

`CMake`_ is an open-source build system. It is used to control the software
compilation process using a simple platform and compiler independent configuration
files. CMake generates native makefiles and workspaces that can be used in the
compiler environment of your choice. On GNU/Linux, CMake generates Makefiles and
on Windows, it can generate Visual Studio solutions.

Using CMake
-----------

First, create a directory where you'll put the building of Code_TYMPAN.

**Command line**

Do::

   cmake path/to/Code_TYMPAN

or::

   ccmake path/to/Code_TYMPAN
   cmake-gui path/to/Code_TYMPAN

to launch the *ncurses* or *Qt* graphical interfaces respectively. The command
line is very helpful to get some info about a command, a variable, etc.::

   # List of CMake commands
   cmake --help-command-list
   # List of CMake variables
   cmake --help-variable-list
   # Get doc for 'add_library' command
   cmake --help-command add_library
   # Get doc for 'CMAKE_BUILD_TYPE' variable
   cmake --help-variable CMAKE_BUILD_TYPE

.. note::

   A CMake 'command' is like a function. Documentation about CMake commands
   gives you indication about their different arguments etc.

**GUI**

You can use either ``ccmake`` or ``cmake-gui``. Note that ``ccmake`` is only
available on GNU/Linux. On Windows, you should use ``cmake-gui`` (see the page
`Running CMake on Windows
<http://www.cmake.org/cmake/help/runningcmake.html>`_).

**Documentation**

Getting help on CMake thanks to ``--help`` command lines as above. Moreover,
there are other resources:

  - The reference book `Mastering CMake
    <http://www.cmake.org/cmake/help/book.html>`_

  - The `online documentation <http://www.cmake.org/cmake/help/documentation.html>`_ with the
    `cmake tutorial <http://www.cmake.org/cmake/help/cmake_tutorial.html>`_,
    a `Wiki <http://www.cmake.org/Wiki/CMake>`_ and a
    `FAQ <http://www.cmake.org/Wiki/CMake_FAQ>`_.

.. _CMake: http://www.cmake.org/

Add a library to the project
----------------------------

Suppose you create a new directory with C++ headers and compilation units. The
best way is to create a ``CMakeLists.txt`` file in this current directory and write:

.. code-block:: cmake

   # The name of your new module (i.e. library)
   set(MODULE_NAME new_tympan_module_name)
   # Group all source files in a CMake list variable.
   file(GLOB ${MODULE_NAME}_SRCS *.cpp *.h)
   # Create the library as ('library_name' 'list of source files')
   add_library(${MODULE_NAME} ${${MODULE_NAME}_SRCS})

You can further refer to your module with the variable
``MODULE_NAME``.

You can also use (if needed):

.. code-block:: cmake

   # If your module depends on other Tympan modules (i.e. other Tympan libraries).
   add_dependencies(${MODULE_NAME} tympan_common tympan_lib)

   # If you have to link your library with one or a few libraries such as Boost,
   # Qt or OpenGL.
   target_link_libraries(${MODULE_NAME} ${QT_LIBRARIES})

   # If you would like to add a specific directory to the include path. A
   # 'include_dir' is missing when you have a compilation error such as 'header.h
   # no such file or directory'.
   include_directory(<path/to/dir>)


Best Practices
--------------

* Do not build a project in the source directory.

* Do not add a manual path to look for a dependency library or to find a source
  file : instead edit the said path in the GUI (or code *reliable*
  auto-detection in case you are a seasoned CMakeLists' developer).

* CMake language is not case-sensitive for identifiers, but is for
  some optional arguments names within commands. ``VARIABLE_NAME`` or
  ``variable_name`` are the same variable but ``FILE(EXISTS ...)``
  will be ok whereas ``FILE(exists ...)`` will **not**. Please try to
  follow the existed convention in reading some ``CmakeLists.txt``.

* Create a ``CMakeLists.txt`` file for each directory. Use ``add_subdirectory``
  command to execute the CMake file in this directory.

* How to clean the CMake cache: remove the ``CMakeCache.txt`` file.

* Display the value of a variable --- useful for debugging::

     message (STATUS "Your message: " ${VARIABLE})

* ``ccmake .`` or ``cmake-gui .`` in the building directory to display
  and edit all CMake variables of the current building project.

* ``cmake --build <building_dir>`` to launch compilation, whatever the
  environment. Very useful when you would like to compile on Windows without
  launching Visual Studio.

Documentation
=============

The C++ source code is annotated and API documentation can be
extracted with Doxygen_. The main documentation is generated from
reStructuredText_ sources in the ``doc`` directory thanks to
Sphinx_.

Build the documentation
-----------------------

#. First generate the XML output of Code_TYMPAN code source with `Doxygen`_::

      cd doc/doxygen
      doxygen

   .. note::

      Later, we might modify the generation of the HTML documentation
      to discard this step.

#. Go to the root source documentation directory and::

      make html

#. See the results in the ``_build/html/`` directory and opening the file
   ``index.html`` with your favorite Web browser.

.. note::

   The tool `breathe`_ provides a bridge between the `Sphinx`_ and `Doxygen`_
   documentation systems. Thus, it is possible to display the Doxygen
   documentation of a Code_TYMPAN class inside the Sphinx documentation.

.. note::

   The tools used to display maths equations properly is `MathJax`_, a
   JavaScript library which prettily renders equations written in LaTeX.


Useful directives
-----------------

Take a look at the `Sphinx`_ and `reStructuredText`_ websites.

* ``.. note::`` write a note.

* ``.. code-block:: c++`` piece of code with the C++ syntax color.

* ``:ref:`section_name``` cross reference with a different location such as a
  section of an other file.

* ``.. todo::`` add a todo task

* ``.. doxygenclass:: <class_name>`` show the Doxygen documentation of the class
  ``<class_name>``.

* ``.. doxygenfunction:: <function_name>`` show the Doxygen documentation of the
  class ``<function_name>``.

  .. note:: See other Doxygen directives in the official `breathe`_ website.

* ``:math:`x^2 -2x +1=0``` or ``.. math::`` to write LaTeX equations.

  .. warning:: A Web connection is needed to be able to see nice equations.

.. _reStructuredText: http://docutils.sourceforge.net/rst.html
.. _Sphinx: http://sphinx-doc.org/
.. _Doxygen: http://www.stack.nl/~dimitri/doxygen/
.. _breathe: http://michaeljones.github.com/breathe/
.. _MathJax: http://www.mathjax.org/


C++ idioms
==========

C++ is a very complex language, full of subtleties and pitfalls... but
some good practices, nice (template) libraries and *well identified
idioms* can improve development efficiency and code base
maintainability.

In this section, we highlight some of such idioms and tools which are
used (or to be used) in Code_TYMPAN. Anyhow we strongly recommend
C++ developers to have a look at some the following books:

Practical and motivated idioms
  Scott Meyers, Addison-Wesley,
  *Effective C++* and *Effective STL*

Complete introduction of the language
  Bjarne Stroustrup, Addison-Wesley, *C++, the language*

Some powerful techniques are exposed to the developer through
*relatively* simple interfaces by Boost_ or the STL_, such as the
standard smart pointers or the ranges concept and the *foreach loops*
for example.

.. note:: Further resources

   Here are some useful inline resources regarding C++

   - `Introductory online C++ course <OpenClassRoomsCpp>`_ with
     corresponding printed book, in french
   - cplusplus.com `tutorial <_cplusplus_tutorial>`_ or `reference
     <cplusplus_reference>`_
   - Google hands-on, practical online course  https://developers.google.com/edu/c++/

.. _Boost: http://www.boost.org/
.. _STL: http://en.cppreference.com/w/

.. _OpenClassRoomsCpp: http://ocw.mit.edu/courses/electrical-engineering-and-computer-science/6-096-introduction-to-c-january-iap-2011/
.. _cplusplus_tutorial: http://www.cplusplus.com/doc/tutorial/
.. _cplusplus_reference: http://www.cplusplus.com/reference/

Smart Pointers
--------------

http://www.dreamincode.net/forums/topic/86846-tr1-intro-smart-pointers/
    A very good, nice and accessible, introduction to smart pointers
    and modern C++ memory management.

http://www.boost.org/doc/libs/1_53_0/libs/smart_ptr/smart_ptr.htm
    Official documentation for Boost smart pointers, among which
    ``shared_ptr`` and ``weak_ptr`` have been included in the TR1 and
    C++'11.


BOOST_FOREACH
-------------

Before C++'11 the language lacked a *foreach loop* construct to
iterate other data structure in a convenient and readable way. Boost_
tries to workaround this limitation with the BOOST_FOREACH macro
(which is some kind of black magic we should get rid of
as soon as C++'11 is widespread and fully supported).

For example, say `c` is some standard STL_ container of type ``C<T>``
(i.e. with a ``begin()`` and ``end()`` method providing iterators to
elements of type ``T``) then you can write:

.. code-block:: c++

    BOOST_FOREACH( T& e, c )
    {
        // do something with the element e
    }

instead of the usual and horribly verbose when ``C`` and/or ``T`` are
complex:

.. code-block:: c++

    for( C<T>::iterator it = c.begin() ;
         it != c.end() ; ++it )
    {
        T& e = *it;
        // do something with the element e
    }


http://www.boost.org/doc/libs/1_53_0/doc/html/foreach.html
    Official documentation for Boost *foreach loops*.


The range concept
-----------------

To be right to ``BOOST_FOREACH``, this construct is far more powerful
than *just* iterating in a simple way through a container. It relies
on the *Range* concept.

The *Range* concept is a generalisation of the *input sequences*
introduced by Stroustrup in *C++, the language* 18.3.1 : basically a
*range* is something that can be seen as a pair of begin/end
iterators.

The whole point of ranges resides in the fact that they do not have to
be STL container, or any data-structure, they can pretty-well
represent a simple counter as well as an on-fly processing over an
other range ! For example :

.. code-block:: c++

    /* ... */
    struct double_int
    {
	typedef int result_type;
	int operator()(int x) const { return x * 2; }
    };

    int main(int argc, const char* argv[])
    {
	std::vector<int> input; // An STL container is a range
	/* Fill `input` with 1, 2, 3, ... */

	BOOST_FOREACH( int i, input | transformed(double_int()) )
	{
	    cout << i << ", "; // Will display 2, 4, 6, ...
	}

	return 0;
    }

http://www.boost.org/doc/libs/1_53_0/libs/range/doc/html/index.html
    Official documentation for Boost  *range concept* which goes along
    so well with *foreach loops*.

**Beware** : *Ranges* are a very powerful but quite advanced - and
thus pitfall prone - C++ technique. Please resort to it only if you
know what you are doing, otherwise just transforming the data by-hand
and storing the result in some container is likely to be a far easier
and cleaner way...

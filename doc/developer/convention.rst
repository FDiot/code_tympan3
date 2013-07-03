.. _dev-conventions:

Conventions
===========

.. note::

   This is a coding conventions proposals for Code_TYMPAN.


C++ indent style
----------------

  BSD Style. See http://en.wikipedia.org/wiki/Indent_style#BSD_KNF_style

  For Emacs user, see the file ``astyle-hoosk.el`` at
  http://astyle.sourceforge.net/scripts.html and set:

  .. code-block:: common-lisp

     (setq c-default-style "bsd")

  An `AStyle`_ ressource file is provided in ``tools/astylerc`` ; it
  can be loaded from the ASpell GUI on Windows or used to launch
  AStyle_ from the command-line (at the root of the sources)::

    $ astyle --options=tools/astylerc "Tympan/**.cpp" "Tympan/**.hpp" "Tympan/**.h"

  It can also be put in your ``$HOME/.astylerc`` or
  ``%USERPROFILE%\astylerc`` to be loaded automatically.

Comments
--------

  Use Doxygen_ to comment functions, classes, methods & attributes in header
  files.

Include guards
--------------

  Depend on the Tympan module and the filename.

  .. code-block:: c++

     #ifndef __TY_MODULE_NAME_FILENAME__
     #define __TY_MODULE_NAME_FILENAME__

     // Source code here

     #endif // __TY_MODULE_NAME_FILENAME__

Misc
----

  - UTF-8 coding
  - No trailing whitespaces


.. _Doxygen: http://www.stack.nl/~dimitri/doxygen/
.. _AStyle: http://astyle.sourceforge.net/

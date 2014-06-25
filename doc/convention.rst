.. _dev-conventions:

Conventions
===========

.. note::

   This are coding conventions for Code_TYMPAN.


C++ indent style
----------------

BSD Style. See http://en.wikipedia.org/wiki/Indent_style#BSD_KNF_style

For Emacs user, see the file ``astyle-hoosk.el`` at
http://astyle.sourceforge.net/scripts.html and set:

.. code-block:: common-lisp

   (setq c-default-style "bsd")

An `AStyle`_ ressource file is provided in ``tools/astylerc`` ; it can be loaded
from the ASpell GUI on Windows or used to launch AStyle_ from the command-line
(at the root of the sources)::

  $ astyle --options=tools/astylerc "Tympan/**.cpp" "Tympan/**.hpp" "Tympan/**.h"

It can also be put in your ``$HOME/.astylerc`` or ``%USERPROFILE%\astylerc`` to
be loaded automatically.


Comments
--------

Use Doxygen_ to comment functions, classes, methods & attributes in header
files.


Include guards
--------------

Depend on the Tympan module and the filename.

.. code-block:: c++

   #ifndef TY_<MODULE_NAME>
   #define TY_<MODULE_NAME>

   // Source code here

   #endif // TY_<MODULE_NAME>

Former convention with wrapping double underscores is abandonned as this is the
standard library convention. "Regular" projects, even such as Boost and Eigen,
don't do that.


Naming
------

* underscore+lowercase files and directories (avoid linux/windows compatibility
  problems). Exception may be accepted when a file only contains a single class
  (java style), then the file name may be the same (case included) as the class
  name. Includes MUST match the case of the files.

* uppercase constants

* camelcase function, class, attribute and method names. Class name starts with
  an uppercase letter, other with lowercase.

* private/protected attribute and method's name starts with an underscore '_'
  (public doesn't)

* underscored variable names

For Python code, only class names are CamelCased, as well as other things
directly bound from the C++ code. Function, attribute and method names are
underscore_cased.

No more need for the `TY` prefix, use the `tympan` namespace.

`LP` prefix should still be used for Tympan's smart pointers (though its meaning
has been lost in byte paradise).



Misc
----

- UTF-8 coding
- No trailing whitespaces


.. _Doxygen: http://www.stack.nl/~dimitri/doxygen/
.. _AStyle: http://astyle.sourceforge.net/

.. _dev-testing:

Testing
=======

We use the testing framework `GTest`_ aka Google Test.

.. _GTest: https://code.google.com/p/googletest/

.. _dev-testing-launch:


Launching automatic tests
-------------------------

.. todo::

   - how launch all tests
   - how launch a single test file
   - how launch a specifc test case in a file

   cf. `CTest documentation`_ : CTest is a companion program to CMake
   which acts as a tests drivers and reporter.

.. _`CTest documentation`: http://www.cmake.org/Wiki/CMake_Testing_With_CTest

Writing automated tests
-----------------------

.. todo::

   Best practices. How can you do.


.. _dev-testing-data:

Data for Testing
----------------

The directory :file:`tests/data` stores various pieces of data used by
some of the automated tests.

For example the :file:`tests/data/projects-panel` provides a set of
XML files dedicated to test some features. These files deal with very
simple Code_TYMPAN project: single site, one building, one pair of
source/receptor, etc. which are very useful for testing. Moreover, for
each of them, a typical screenshot is provided in
:file:`doc/_static/images/tests/`

.. toctree::
   :maxdepth: 1

   data_test/geometry/index


By hand validation
------------------

Because the automated tests coverage is yet unsufficient, and because
some high level, global validation procedure is always useful to
define what *it works* really means, a *by hand* validation procedure
is proposed.

For now it this procedure is documented in this :download:`external
PDF document <./methodologie_qualification-v2013-06-11.pdf>` and the
resources it refers to are stored under
:file:`tests/data/manual_validation/`.

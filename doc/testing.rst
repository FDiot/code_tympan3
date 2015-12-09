.. _dev-testing:

Testing
=======

Why set-up a testing policy ?
-----------------------------

Code_TYMPAN is quite a big application and one can not expect that
some fix or new feature will *just work* : a fix somewhere can - and
will - trigger a bug or a mere unexpected behaviour later and
elsewhere... In order to circumvent this major source of complexity it
is crucial to cover new bug and features with *automated tests*.

*Automated tests* are testing procedures :

- which can be run automatically, without requiring for a user to
  perform a series of actions in the GUI typically
- which are implemented a piece of code running them
- which will either succeed in case what they are intended to check is
  asserted or fail which does *not* mean crashing on an error but
  instead reporting *this* was expected but it got *that* instead.

Moreover, it is much easier to debug a small automated test than a
complete application like Code_TYMPAN. So coding a test should be
done at the same time - or even before - coding the actual feature
so that the developer can draw the most benefit from it while devising
the feature.

What is a testing framework ?
-----------------------------

Now, writing a stand alone program to test each new feature or bug
newly reported can be a tedious task. That's why automated testing
framework exists : to make this most important task lighter. They allow
to concentrate on asserting the feature or bug, not the plumbing of
the reporting (see :file:`tests/test_examples.cpp`).

In Code_TYMPAN we use the testing framework `GTest`_ aka Google Test :
it was chosen for it simplicity and the quality of its
documentation. You are strongly encouraged to read `the primer
<GTestPrimer>`_ *now*.

GTest (like most unit testing frameworks) builds upon the notion of
*test case* : a test case represent one simple setting in which you
want to assert some behaviour of your feature, or lack of some bug.
You create a test case (which actually is a class under the hood) with
the ``TEST`` macro:

.. code-block:: c++

   TEST(TestSuite, testname)
   {
     // Set up the objects and their state
     ...
     EXPECT_EQUAL(expected, value_tested);
     ...
   }

When you run the resulting executable, the body of each test case in
your ``.cpp`` file will be run separately, just like the ``main``
function of as many small programs, and reported as *pass*, *fail* or
*error*. In case of failure each failing ``EXPECT`` or ``ASSERT`` will
be detailed in the report.

It is a good practice to organise automated tests in a directory tree
by the components or interactions they test. Ideally each class and
major interaction should be tested in its own test file. For a complex
application like Code_TYMPAN there will thus be a whole tree a tests
to run, that's why a test driver is most useful.

.. _GTest: https://code.google.com/p/googletest/
.. _GTestPrimer: http://code.google.com/p/googletest/wiki/Primer


.. _dev-testing-launch:

How to launch automated tests ?
-------------------------------

Launching by hand all the automated tests of the :file:`tests` tree
and aggregating their reports would be a daunting task. Here the
`CTest`_ test driver comes into play. CTest_ is a companion to CMake
which uses information already available in CMake to automate the
execution and reporting of a whole set of tests.

The simplest way to use `CTest` is launching all the tests in some
(sub)tree through the ``RUN_TESTS`` project on Visual Studio or the
command line::

  $ .../Code_TYMPAN/tests/business_logic$ ctest -V

The single ``-V`` option make CTest_ a bit more verbose and a double
``-VV`` makes it a lot more verbose. You can also filter (out) by test
executable name (with reg-exp if needed) and many advanced features not
covered here. You can also launch directly a given test executable, or
select a test case within an executable::

  $ .../Code_TYMPAN/tests/business_logic$ ./tools/test_OBox
  $ .../Code_TYMPAN/tests/business_logic$ ./tools/test_OBox --gtest_filter='Enlarge*'


.. _`CTest`: http://www.cmake.org/Wiki/CMake_Testing_With_CTest

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
some high level, global validation procedure is always useful a *by
hand* validation procedure is proposed.

For now it this procedure is documented in this :download:`external
PDF document <_static/documents/methodologie_qualification-v2013-06-11.pdf>` and the
resources it refers to are stored under
:file:`tests/data/manual_validation/`.

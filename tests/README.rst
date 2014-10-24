Code_TYMPAN Tests
#################

This file deals with Code_TYMPAN tests description and folder structures.

Code_TYMPAN uses the GoogleTest framework https://code.google.com/p/googletest/
to write C++ unit tests.

Tests are divided into different folders:

  - `data`: no C++ unit tests here. Just XML or other files used in some test
    cases.

  - `business_logic`: dedicated to test objects and features related to "real
    life" models: buildings, acoustic sources, topography, etc. Itself divided
    into different folders according to Code_TYMPAN modules dependency (tools,
    common, etc.)

  - `gui`: all objects related to the Qt Graphical User Interface.

  - `solver_app_interface`: some tools and utilities to test the part which
    makes the bridge between the data model solver and the business logic part
    (i.e. site, buildings, etc.).

  - `SolverDataModel`: test entities and relations which occur in the data model
    related to the solver part.

  - `testutils`: Classes and functions used in some test cases (load an XML
    project file for instance). Useful when you would like to factorize some
    code which occurres in several places. Contain a folder `tests` which tests
    just these classes & functions.

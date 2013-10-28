================================
    Code_TYMPAN testing data
================================

:author: Anthony Truchet <anthony.truchet@logilab.fr>

:abstract:

    This document introduces the testing data in the
    ``tests/data`` directory. It aims at summarizing the rationale
    of the files and the checks intended to be based on them.


Directory ``projects-panel``
============================

This directory contains a panel of Code_TYMPAN project's files. Those
project increasingly demonstrate new features. They are, for now,
intended for a simple visual check by comparing with provided
screen-shots:

* load the ``tests/data/projects-panel/nn_description.xml`` in Code_TYMPAN
* open the image in ``doc/_static/images/tests/Img_nn.jpeg``
* and compare visually

.. note:: TODO

     This panel of projects could be used (as a base) for more
     specific checks, which still have to be specified.


Directory ``dxf``
=================

This directory holds DXF files used to check the DXF import features
by the tests in ``tests/io/test_bdtopo_import.cpp``.

Directory ``solver_export``
===========================

The serie of ``solver_export/*.xml`` files is intended to validate
the correctness of data export to the solver data model.

``base.xml``

    A site with a concave land take : it is a square from (-100, -100)
    to (100, 100) but for a top left rectangle from (-100, 0) to
    (-50, 100). The land take is not to be used at a level curve.

    The north border has a level curve (actually a segment) at 15m and
    the south border has a level curve (actually a segment too) at
    5m. The point (0, 0) should thus have altitude 10m and default
    material (*Sol 0* whose resistivity is 20000 kRay).

    .. note:: todo

              Implement this test.


Misc files
==========

``tiny_site.xml``

    This is a simple site mainly used for basic smoke tests, or providing
    a simple site for technical tests.

``*palette.xml``

    Those file are use by the unit tests for the TYPalette(Widget)

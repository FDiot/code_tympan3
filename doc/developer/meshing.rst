.. mesh-construction:

=======================================
 Building the mesh seen by the solvers
=======================================

:summary: This section states the geometrical problem of building a
          mesh suitable for the solvers from higher level site
          elements. It then give on overview of the retained process
          to do so and the API involved.

The geometrical problem
=======================

Business description for a single site
--------------------------------------

A Code_TYMPAN site is mainly described, from a end-user point of view
as a tree of elements having geometrical and acoustic
properties. Those elements are grouped under two containers,
*topography* and *infrastructures*. The *infrastructures* lean on the
*altimetry* which is built from the *topography* (and some constraint
extracted from the the *infrastructures*).

Topography
~~~~~~~~~~

The *topography* includes the elements which define the *altimetry* :

- *level curves* : defined by a 2D poly-line and an altitude
- *ground material areas* : defined by a 2D simple polygon and a material

Infrastructures
~~~~~~~~~~~~~~~

The *infrastructure* includes everything which is built on the
ground. This means the z-coordinates of those elements is defined as
an *height above the ground* as represented by the *altimetry*.  The
elements are most notably:

- *buildings* and *machines* : have a (potentially complex)
  geometrical (polyhedrons) and acoustic definition. But the property
  of interest is their *land-take* on the ground which is the simple
  2D polygon of the ground covered by the building.
- *roads* : are a simple set of sources for now but are expected to be
  considered as ground material area and local shift of the
  altitude. **The roads impact can be ignored in a first approach**.
- sources and receptors : have no geometry by their own

Altimetry
~~~~~~~~~

The *altimetry* has to be computed from the *topography* but **also
some elements of the infrastructure** as a set of **3D** triangles
such that:

1. each triangle is made of a single material
2. it is possible to make (efficient) point query to retrieve the ground
   triangle at a specified 2D location and thus

   - compute the z-coordinate of infrastructure points given their
     height above the ground
   - get the ground material at the 2D location if any (i.e. if
     the ground location is not beneath a building or machine)

3. the ground and infrastructure triangles must join properly
4. the ground triangles are neither too big or too pointy. NB:
   formal criteria are not specified, but the need is for the
   solver to be able to curve the ground by simply changing the z
   coordinate of existing points in a relevant way.

   .. todo:: double check this with the product owner

Noteworthy points
~~~~~~~~~~~~~~~~~

* The ground material polygons form a tree for the inclusion : there
  can be no partial overlap between two material areas because in this
  case it would be impossible to attribute **the** *most specific
  material* to the triangles in the intersection.

  If this case arises, this is a modelling error which has to be fed
  back to the user and which stop the process of building the altimetry.

* Two level curves can not intersect.

  If this case arise, this is a modelling error which has to be fed
  back to the user and which stop the process of building the altimetry.

* The land-take of buildings and machine should likely be preserved as
  *constraints* when building the altimetry so that :

  - they properly mark a hole in the ground
  - it is easy to join the triangles of the building or machine and
    the triangles of the ground in a later stage of processing

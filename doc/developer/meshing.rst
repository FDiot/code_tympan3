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

Sub-sites and land-take
-----------------------

Introduction
~~~~~~~~~~~~

For reasons linked to the business process of the end-user of the
product, a notion of *sub-site* has been introduced. In addition to
its *topography* and its *infratructure* a site consists of zero, one
or several *sub-sites*, each with their own *topography*,
*infrastructure* and, potentially, *sub-sites*.

A typical usage for this feature is layered modelling :

1. A first user of the application builds a model of the pre-existing
   site, before implanting a plant
2. An other user builds a model of the plant itself, which can include
   levelling works, as a sub-site to be integrated into the main site.
3. Some years later a plant extension is built and is modelled as a
   sub-sub-site to be integrated into the previous one.

When computing the altimetry for a project, the topography and
infrastructure of the main site and all its (active) sub-sites must be
taken into account. Because a sub-site must be able to change the
topography of its parent site *where and when relevant* some rules have been
established to build the altimetry for a tree of (sub-)sites. This is
the role of a site's *land-take*.

Land-take and merging sub-sites topography
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The *land-take* (*emprise* in French in the code) is a simple polygon
associated to a site which delimit the *authoritative area* for the
site. When site B is included as a sub-site of main site A and the
altimetry for the project is built :

* all features of A which lie entirely within the land-take of B are
  ignored and must be reported as such to the user. This is not an error.
* all features of B which lie entirely outside the land-take of B are
  ignored and must be reported as such to the user. This is not an error.
* all features **but for the level curves** of either A or B which
  overlap the land-take of B are to be considered as *modelling
  errors*. They must be reported as such and stop the process of
  building the altimetry.

  NB: This rule is mainly intended as a simplification measure, and is
  likely to be quite restrictive both for the end user and the
  implementation. It should be possible to amend it if need be.
* the level curves of B which overlap the land-take of B must be
  truncated to the interior of it
* the level curves of A which overlap the land-take of B must be
  truncated to the exterior of it

Noteworthy points
~~~~~~~~~~~~~~~~~

* For a given project the land-takes of the sub-sites form a tree of
  simple polygon for the inclusion, akin to the ground material
  polygons in the case of a single site.
* Thanks to the *no overlap but for level curves* rule, the trees
  formed by sub-site land-takes and material polygons are compatible
  and could possibly be represented as a single tree during the
  computation.
* In the case of a single site project the land-take's role is not yet
  clear : it could be reasonable to expect that the land-take is
  included in the convex hull of the level curves (enabling altitude
  computation at all point within the landtake) and that elements
  outside the land-take are filtered out.

* There is no *a priori* requirement for the solution to be incremental.


Solutions
=========

Algorithm for a single site
---------------------------

The processing flow implemented up to Code_TYMPAN version 4.2.3
(June 2014) was targeted at single sites and was asserting there are
no sub-site to merge. It is yet (June 2014) unclear how this approach can
be adapted and how it would scale to the whole problem.

1. All the level curves (including ponds) are first added to a
   Constrained Delaunay Triangulation (CDT) **in 2D**. The 2D vertices
   bear their altitude as additional information.

2. This triangulation is then copied and used to answer altitude
   query: when the altitude for a 2D point is needed, first the
   triangulation is searched for the triangle containing this
   point. If the point is outside the convex hull of the triangulation
   the ``undefined_altitude`` is returned.

3. Material ground polygons are then added to the triangulation as
   constraints (their vertices are given an altitude at this
   point). Then all vertices are iterated over and those without a
   valid altitude (i.e. the vertices created as intersection of
   constraints) are given an altitude.

4. A double mapping between material polygons and faces of the
   triangulation is then built by iterating over the faces of the
   triangulation. One stores, on one hand, for each face all polygons
   containing this face and, on the other hand, for each polygon all
   the faces it contains.

5. Last, this double mapping is used to associate to each face its
   material, by searching the minimal polygon (for inclusion)
   containing this face. In case there is not such minimal polygon it
   means there is a partial overlap between two material polygons and
   that this face is a witness of this overlap which is reported.

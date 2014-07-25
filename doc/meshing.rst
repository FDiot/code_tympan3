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

  NB: The current implementation does not reject intersecting level
  curves with the same altitude as those are actually meaningful.

* The land-take of buildings and machine (and roads or stream of water
  or electricity transportation network... in potential future works)
  should be preserved as constraints* when building the altimetry so
  that :

  - they properly mark a hole in the ground with a special *hidden*
    material, so that the faces with a hidden material are not
    exported to the solvers
  - it is easier to join the faces of the building or machine and
    the faces of the ground (cf. dedicated section).
  - the display artefacts for the road and other linear
    infrastructures are reduced.


Sub-sites and land-take
-----------------------

Introduction
~~~~~~~~~~~~

Because of the business process for the end-user of the product, a
notion of *sub-site* has been introduced. In addition to its
*topography* and its *infratructure* a site consists of zero, one or
several *sub-sites*, each with their own *topography*,
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
  implementation. Given the advanced polygon intersection features of
  the library used it would likely not be very costly to release this
  constraint by simply cutting off the material areas overlapping
  sub-sites land-takes.

* the level curves of B which overlap the land-take of B must be
  truncated to the interior of it
* the level curves of A which overlap the land-take of B must be
  truncated to the exterior of it


Legacy algorithm for a single site
==================================

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


New process for compound site
=============================

Starting from Code_TYMPAN version 4.2.7 a new processing of the
topography is implemented. This section document the overall algorithm
while the next main section will provide implementation notes.

Merging compound sites into single-node sites
---------------------------------------------

The first step consist in merging the main site and all its (indirect)
sub-sites into a single site. This is done by recursively merging a
sub-site in its parent site until all the sub-sites have been merged in
the parent site.

The merging of a sub-site into its parent proceeds as follow:

1. The sub-site land-take is computed as a polygon. This polygon can
   have holes in case the sub-site, itself have sub-sites.
2. Each feature of the sub-site is *cleaned*: all the rules above are
   applied to filter the feature out if required and compute its new
   geometry by taking the intersection with the site land-take.
3. Material areas are inserted in a list so that a given material area
   appears in the list before any other material area which would
   contain it. The order in this of material areas not related by
   inclusion is unspecified. This step is critical for the later
   processing of filling the mesh with material information.
4. All those cleaned elements are then included into the parent site,
   taking care to merge the material areas list in a way which
   preserves the partial order for inclusion.

Triangulating the feature set
-----------------------------

The input data for this step is a single-node site (i.e. without any
sub-site) whose elements' geometry abide to the rules stated above (no
overlapping of material areas...). The output of this step is a
triangulation of all the points and segments making the site geometry,
plus some information regarding the site features those points and
segments originate from and an altitude for each point.

It is beyond the scope of this document to explain what a Constrained
Delaunay Triangulation (CDT) is. For more information about what
please refer to:

* http://en.wikipedia.org/wiki/Delaunay_triangulation
* http://doc.cgal.org/latest/Triangulation_2/index.html#Section_2D_Triangulations_Representation
* http://doc.cgal.org/latest/Triangulation_2/index.html#Section_2D_Triangulations_Constrained_Delaunay

A first CDT is build from the level curves only: all the points and
segments which make the level curves are inserted into a CDT.  They
respectively become *vertices* and *input constraints*. Some new
vertices can appear at the intersection of input constraints : those
are accepted if the two level curves are at the same altitude.  The
result of this step is a CDT whose all vertices have a well defined
altitude. It is thus possible to interpolate the altitude of any point
specified by its 2D coordinates as long as it lies within the convex
hull of the level curves. This CDT is kept unchanged for this purpose
and a copy of it is used for the next step.

Then all other (i.e. non altimetric) features are inserted into the
CDT: this introduces many new *vertices* at intersections and
subdivide most *input constraints* into several *edges*. The
relationship between each edge and the input constraint(s) it
originates from is maintained. This allows fetching information
(material, building ID, ...) about the original features in the site
for all vertices and edges.

.. todo:: Double check when the mesh refine operation appears

Lastly the altitude of all newly created vertices is computed from the
altimetric reference CDT from the first step of triangulation.


Mesh refinement
---------------

.. todo:: document me once written

NB: The portions of the ground under buildings' land-take does not
need to be refined and it is actually preferable not to refine them to
avoid having build too much triangles to be filtered out later. But
this is a performance consideration, not a requirement.

Material attribution
--------------------

Each face must be given a material as attribute (or the special *hidden*
material in case it lies under a building land-take). A flooding
algorithm is use for this purpose.

A flooding algorithm starts from some seeds faces, marking them with a
some information and iterates on their adjacent faces. Some properties
of the edge of the adjacent face prevent the flooding to touch the
adjacent face.

For material attribution the flooding starts on all faces on the left
hand-side of the polygon delimiting the area (the polygon is expected
to be counter-clock-wise oriented) and stops on edges which originates
in a material area or a building land-take.

NB: For the purpose of marking material the building, land-takes are
considered as material areas with the special *hidden* material,
except that they can overlap other material areas, in which case they
override them. They are thus flooded first, and then the real material
area, starting with the most specific first.


Implementation notes
====================

Resources
=========

CGAL packages
-------------

The packages used for the version 4.2.3 (June 2014) are
CGAL_Polygons_ and CGAL_Triangulation_. The use of the package
CGAL_Mesh_ is planned to handle the refinement of the altimetry.
Those packages will be accessed from Python tanks to the `CGAL Bindings`_.

.. _`CGAL_Polygons`: http://doc.cgal.org/latest/Polygon/index.html#Chapter_2D_Polygon
.. _`CGAL_Triangulation`: http://doc.cgal.org/latest/Triangulation_2/index.html
.. _`CGAL_Mesh`: http://doc.cgal.org/latest/Mesh_2/index.html#Chapter_2D_Conforming_Triangulations_and_Meshes
.. _`CGAL Bindings`: http://code.google.com/p/cgal-bindings/

Shapely
-------

Shapely_ is a Python wrapping of the geogaphical processing library
GEOS_.  It will be used to pre-process the sub-sites geometry before
triangulation and meshing.

.. _Shapely: http://toblerity.org/shapely/index.html
.. _GEOS: http://trac.osgeo.org/geos/

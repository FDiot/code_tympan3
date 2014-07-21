"""
This package provides altimetry computation from Topography and Infrastructures.

It relies on :

* The GeoJSON specification as format for its input data
  (http://geojson.org/geojson-spec.html)
* The Shapely library for preprocessing
  (http://toblerity.org/shapely/index.html)
* The CGAL library and its bindings for triangulation and meshing:

  - http://code.google.com/p/cgal-bindings/
  - http://doc.cgal.org/latest/Triangulation_2/index.html#Chapter_2D_Triangulations
  - http://doc.cgal.org/latest/Mesh_2/index.html
"""

from __future__ import absolute_import

import sys, os
import os.path as osp


_HERE = osp.realpath(osp.dirname(__file__))
_PROJECT_BASE = osp.abspath(osp.join(_HERE, '..', '..', '..'))


try:
    from CGAL import CGAL_Kernel
except ImportError:
    # TODO For now we assume CGAL_BINDINGS_PATH is set as an environment variable
    #      It would be better to compute it from _PROJECT_BASE once cgal-bindings
    #      are integrated into the project as 3rd party dependency
    CGAL_BINDINGS_PATH = os.environ.get('CGAL_BINDINGS_PATH')

    if CGAL_BINDINGS_PATH is None or not osp.isdir(CGAL_BINDINGS_PATH):
        raise ImportError("A `CGAL_BINDINGS_PATH` environment variable is expected"
                           " to point to the directory holding cgal-bindings binary modules")
    try:
        # Adjusting sys.path in order to find modules shipped with Code_TYMPAN
        sys.path.append(CGAL_BINDINGS_PATH)
        from CGAL import CGAL_Kernel
    except ImportError:
        raise ImportError("The `CGAL_BINDINGS_PATH` does NOT point to the directory "
                          "holding cgal-bindings binary modules but to %s" % CGAL_BINDINGS_PATH)

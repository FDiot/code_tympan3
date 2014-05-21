#!/usr/bin/env python

import numpy as np
from mayavi import mlab


def plot_mesh(filebasename):
    """
        Plot a triangular mesh from a pair of csv files representing nodes
        (1 line = 1 node = "x; y; z") and triangles (1 line = 1 triangle =
        "node1_idx, node2_idx, node3_idx").
        The format is defined and the files are produced in the class
        tympan::AcousticProblemModel (see method export_triangles_soup)
    """
    nodes_filename = filebasename + "_nodes.csv"
    faces_filename = filebasename + "_faces.csv"
    nodes = np.loadtxt(nodes_filename, delimiter=';', dtype=np.float)
    tri_indices = np.loadtxt(faces_filename, delimiter=';', dtype=np.uint)
    return mlab.triangular_mesh(nodes[:,0], nodes[:,1], nodes[:,2], tri_indices)

if __name__ == "__main__":
    import sys
    filebasename = sys.argv[1]
    s = plot_mesh(filebasename)
    mlab.show()


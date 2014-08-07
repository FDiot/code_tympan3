#!/usr/bin/env python
# -*- encoding: utf-8 -*-

"""
Utilities for testing and prototyping file IO.
"""

import sys
import numpy as np

# libmed is a python wrapper aroung the C libmed
# https://www.logilab.org/project/python-libmed
from libmed import MEDFile

def triangle_soup_to_med(filebasename):
    """
    Convert a pair of csv file representing a triangle soup into a MED file
    """
    nodes_filename = filebasename+"_nodes.csv"
    faces_filename = filebasename+"_faces.csv"
    out_filename   = filebasename+".med"

    nodes = np.loadtxt(nodes_filename,
                       delimiter=';',
                       dtype=np.float)
    tri_indices = np.loadtxt(faces_filename,
                             delimiter=';',
                             dtype=np.uint)
    tri_indices+=1 # In MED the nodes' indices start at 1

    with MEDFile(out_filename, 'w') as outf:
        m = outf.add_mesh(name='TEST1', spacedim=3, meshdim=3)
        m.set_nodes(nodes)
        m.add_elements('Tria3',tri_indices)

    with MEDFile(out_filename, 'r') as inf:
        m = inf.get_mesh('TEST1')
        print "DEBUG : nodes / faces ", nodes.shape, tri_indices.shape
        print "In the file ", m.nodes()

if __name__=="__main__":
    filebasename = sys.argv[1]
    print "Importing {0}_*.csv and exporting to {0}.med".format(filebasename)
    triangle_soup_to_med(filebasename)

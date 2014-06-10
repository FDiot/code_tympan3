#!/usr/bin/env python

import numpy as np
from mayavi import mlab

try:
    import pytam
except:
    print "Pytam library not found"

def plot_mesh(nodes, tri_indices):
    return mlab.triangular_mesh(nodes[:,0], nodes[:,1], nodes[:,2], tri_indices)

def make_mesh(project_name):
    project = pytam.Project.from_xml(project_name)
    model = project.current_computation().acoustic_problem()
    builder = pytam.SolverModelBuilder(model)
    builder.fill_problem(project.site()) # , project.current_computation()
    (nodes, triangles) = model.export_triangular_mesh()
    return (nodes, triangles)

if __name__ == "__main__":
    """
        Plot a triangular mesh representing the site contained in a XML
        project (path to XML project must be given as an argument)
    """
    import sys
    if len(sys.argv) != 2:
        sys.exit(-1)
    project_name = sys.argv[1]
    (nodes, triangles) = make_mesh(project_name)
    s = plot_mesh(nodes, triangles)
    mlab.show()

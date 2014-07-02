"""Visualisation utilities for the geometries manipulated


This module provides some simple functions to plot shapely shapes and
monkey-patch the datamodel classes to add thema ``plot`` method.

This ``plot`` method always take a Matplotlib axes object as first
argument and plot the object on it. By default key word arguments are
passed to the matplotlib ploting function and allow setting the
stylistic properties of the plot object.

If the optional argument ``alt_geom_map`` is provided then it must be
a mapping and the shape plot will be the value this mapping maps the
feature id to. This is intended to show clean geometries using the
original site nodes tree.


Notably relies upon:

* http://matplotlib.org/users/index.html
* https://bitbucket.org/sgillies/descartes/

"""

from matplotlib import pyplot as plt
from shapely import geometry as geom
from descartes import PolygonPatch

from  datamodel import *

MATERIAL_COLORS = {
    "grass": "green",
    "Water": "blue"
}

def plot_linear_shape(ax, shape, **opts):
    x, y = shape.xy
    return ax.plot(x, y, **opts)

def plot_polygonal_shape(ax, shape, **opts):
    if shape.type == 'Polygon':
        p_list = [shape]
    else:
        assert shape.type =='MultiPolygon'
        p_list = shape.geoms
    patches = [PolygonPatch(shape, **opts) for p in p_list]
    for p in patches:
        ax.add_patch(p)
    return patches

def plot_LevelCurve(this, ax,  alt_geom_map=None, **kwargs):
    opts = {'color':'red',
            'linewidth':3 }
    opts.update(kwargs)
    shape = (alt_geom_map and alt_geom_map[this.id]) or this.shape
    plot_linear_shape(ax, shape, **opts)
LevelCurve.plot = plot_LevelCurve

def plot_PolygonalFeature(this, ax, alt_geom_map=None, **kwargs):
    opts = {'linewidth': 3,
            'alpha': 0.4,
            'facecolor':'none'}
    shape = (alt_geom_map and alt_geom_map[this.id]) or this.shape
    opts.update(kwargs)
    plot_polygonal_shape(ax, shape, **opts)
    return ax
PolygonalTympanFeature.plot = plot_PolygonalFeature

def plot_MaterialArea(this, ax, **kwargs):
    color = MATERIAL_COLORS.get(this.material_id, 'none')
    opts = {'facecolor' : color}
    opts.update(kwargs)
    super(MaterialArea, this).plot(ax, **opts)
    return ax
MaterialArea.plot = plot_MaterialArea

def plot_SiteNode(this, ax, recursive=False, **kwargs):
    super(SiteNode, this).plot(ax, alpha=1, edgecolor='black')
    for f in this.all_features:
        f.plot(ax, **kwargs)
    if recursive:
        for f in this.subsites:
            f.plot(ax, recursive=True, **kwargs)
SiteNode.plot=plot_SiteNode



def rescale_plot(ax, scale = 1.0):
    ax.axis('tight')
    ax.axis('equal')

def plot_points_seq(ax, points, **kwargs):
    plot_opts = {'linestyle':'',
                 'marker':'o'}
    plot_opts.update(kwargs)
    xs, ys = zip(*[(p.x(), p.y()) for p in points])
    return ax.plot(xs, ys, **plot_opts)

def plot_segment(ax, s, **kwargs):
    plot_opts = {'linestyle':'-',
                 'marker':''}
    plot_opts.update(kwargs)
    return plot_points_seq(ax, [s.source(), s.target()], **plot_opts)


class MeshedCDTPlotter(object):

    style_edge = {'linewidth': 1, 'color':'black'}
    style_constrained_edge = {'linewidth': 3}
    style_annotations = {'textcoords': 'offset points',
                         'xytext': (5, 5)}

    def __init__(self, mesher, title="Mesh"):
        self.mesher = mesher
        self.fig = plt.figure()
        self.ax = self.fig.add_subplot(1, 1, 1)
        self.ax.set_title(title)

    def show(self):
        rescale_plot(self.ax)
        plt.show()

    @property
    def cdt(self):
        return self.mesher.cdt

    def plot_edge(self, edge, **kwargs):
        plot_opts = self.style_edge.copy()
        if self.cdt.is_constrained(edge):
            plot_opts.update(self.style_constrained_edge)
        plotted = plot_segment(self.ax, self.mesher.segment_for_edge(edge), **plot_opts)
        return plotted

    def plot_edges(self):
        for edge in self.cdt.finite_edges():
            self.plot_edge(edge)

    def annotate_at(self, x, y, text, **kwargs):
        plot_opts = self.style_annotations.copy()
        plot_opts.update(kwargs)
        return self.ax.annotate(text, (x, y), **plot_opts)

    def annotate_vertex(self, vertex, text, **kwargs):
        p = vertex.point()
        return self.annotate_at(p.x(), p.y(), text, **kwargs)

    def annotate_finite_face(self, face, text, **kwargs):
        p = self.mesher.point_for_face(face)
        return self.annotate_at(p.x(), p.y(), text, **kwargs)

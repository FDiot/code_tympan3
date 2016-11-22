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
from warnings import warn

from matplotlib import pyplot as plt
from matplotlib import patches as mpatches
from shapely import geometry as geom
from descartes import PolygonPatch

from  .datamodel import *

MATERIAL_COLORS = {
    "grass": "green",
    "Water": "blue",
    "concrete": "yellow",
    HIDDEN_MATERIAL.id: 'magenta',
    DEFAULT_MATERIAL.id: '0.30'
}

def plot_linear_shape(ax, shape, **opts):
    if isinstance(shape, geom.LineString):
        shape = [shape]
    plots = []
    for ls in shape:
        try:
            x, y = ls.xy
        except NotImplementedError:
            warn("Not able to plot the shape %s, part of %s" % ls.wkt, shape.wkt)
            continue
        plots.append(ax.plot(x, y, **opts))
    return plots

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

def get_shape_update_opt(this, alt_geom_map, opts):
    if alt_geom_map is not None:
        shape = alt_geom_map.get(this.id)
        if shape is None: # The this feature was not deleted from the alt_geom
            shape = this.shape
            opts.update({'linestyle': 'dashed',
                         'alpha': opts['alpha']**1.5})
    else:
        shape = this.shape
    return shape

def plot_LevelCurve(this, ax,  alt_geom_map=None, **kwargs):
    opts = {'color':'red',
            'linewidth':3 }
    shape = get_shape_update_opt(this, alt_geom_map, opts)
    opts.update(kwargs)
    plot_linear_shape(ax, shape, **opts)
LevelCurve.plot = plot_LevelCurve

def plot_PolygonalFeature(this, ax, alt_geom_map=None, **kwargs):
    opts = {'linewidth': 3,
            'alpha': 0.4,
            'facecolor':'none'}
    shape = get_shape_update_opt(this, alt_geom_map, opts)
    opts.update(kwargs)
    plot_polygonal_shape(ax, shape, **opts)
    return ax
PolygonalTympanFeature.plot = plot_PolygonalFeature

def plot_MaterialArea(this, ax, **kwargs):
    color = MATERIAL_COLORS.get(this.material_id, 'gray')
    opts = {'edgecolor' : color,
            'hatch':'/',
            'alpha':.80}
    opts.update(kwargs)
    super(MaterialArea, this).plot(ax, **opts)
    return ax
MaterialArea.plot = plot_MaterialArea

def plot_InfrastructureLandtake(this, ax, **kwargs):
    opts = {'edgecolor' : 'black',
            'hatch':'\\\\',
            'alpha':1.0}
    opts.update(kwargs)
    super(InfrastructureLandtake, this).plot(ax, **opts)
    return ax
InfrastructureLandtake.plot = plot_InfrastructureLandtake

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
    xs, ys = list(zip(*[(p.x(), p.y()) for p in points]))
    return ax.plot(xs, ys, **plot_opts)

def plot_segment(ax, s, **kwargs):
    plot_opts = {'linestyle':'-',
                 'marker':''}
    plot_opts.update(kwargs)
    return plot_points_seq(ax, [s.source(), s.target()], **plot_opts)


class MeshedCDTPlotter(object):

    style_edge = {'linewidth': 1, 'color':'black'}
    style_constrained_edge = {'linewidth': 3, 'linestyle':'dashed'}
    style_annotations = {'textcoords': 'offset points',
                         'xytext': (5, 5)}

    def __init__(self, mesher, title="Mesh", ax=None):
        self.mesher = mesher
        if ax is None:
            self.fig = plt.figure()
            self.ax = self.fig.add_subplot(1, 1, 1)
        else:
            self.ax = ax
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

    def plot_face(self, fh, material_id=None, **kwargs):
        triangle = [(p.x(), p.y()) for p in self.mesher.triangle_for_face(fh)]
        if material_id is None:
            color = 'none'
        else:
            color = MATERIAL_COLORS.get(material_id, 'gray')
        plot_opts = {'edgecolor':'none',
                     'facecolor':color,
                     'alpha':0.4,
                     'zorder':-1}
        plot_opts.update(kwargs)
        poly = mpatches.Polygon(triangle, fill=True, **plot_opts)
        self.ax.add_patch(poly)

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

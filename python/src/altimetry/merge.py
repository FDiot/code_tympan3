"""
Merging sub-sites for the computation of the altimetry.

"""


from collections import defaultdict

from shapely import geometry
from altimetry.datamodel import *


def build_site_shape_with_hole(site):
    site.ensure_ok()
    exterior = site.shape.exterior
    if site.shape.interiors:
        raise ValueError("The site is not expected to already have holes")
    holes = [hole.shape.exterior
             for hole in site.subsites]
    return geometry.Polygon(exterior, holes)


class SiteNodeGeometryCleaner(object):
    """Clean a sitenode geometry, subsites excluded and build reports.

    More precisely this class is build from a root SiteNode. Then the
    ``process_*`` method walk through the geometrical elements (level
    curves, material areas, landtakes, ...) and associate them with a
    geometry warranted to be within the site landtake and outisite the
    landtake of any subsite.

    The level curves are cut and the polygonal features are filtered
    according to the following policy:

    * the IDs of feature overlapping the boundary of the site node are
      reported in the ``erroneous_overlap`` list attributes

    * the IDs of feature entirely out of the site node are reported in
      the ``ignored_feature`` list attributes

    The new geometry computed for each feature is available through
    the ``geom`` dictionary attribute, which is indexed by the
    feature IDs and the properties of the original features (if any)
    are available through the ``info`` dictionary attribute. The
    class also implement a __get_item__ method so that both geometry
    and properties can be accessed with ``geom, info = cleaner[feature_id]``.
    """

    def __init__(self, sitenode):
        self.sitenode = sitenode
        self.siteshape = build_site_shape_with_hole(self.sitenode)
        self.geom = {}
        self.info = defaultdict(dict)
        self.ignored_features = []
        self.erroneous_overlap = []

    def _add_new_shape(self, id_, shape, props):
        existing_shape = self.geom.pop(id_, None)
        if existing_shape and not existing_shape.equals(shape):
            raise ValueError("ID %s is already associated to a different shape %s" %
                             (id_, existing_shape.wkt))
        self.geom[id_] = shape
        self.info[id_].update(props)

    def __getitem__(self, feature_id):
        return self.geom[feature_id], self.info[feature_id]

    def process_level_curves(self):
        for level_curve in self.sitenode.level_curves:
            shape = level_curve.shape.intersection(self.siteshape)
            self._add_new_shape(level_curve.id, shape, level_curve.build_properties())

    def _add_or_reject_polygonal_feature(self, feature):
        """
        Implements the no overlap policy for material area and land-takes
        """
        # NB : given the possibilities of shapely, allowing to cut the
        # polygonal features so that the associated geometries lie within
        # the site land-take would be a simple matter of changing this method.

        if self.siteshape.overlaps(feature.shape):
            self.erroneous_overlap.append(feature.id)
            return False
        if not self.siteshape.contains(feature.shape):
            self.ignored_features.append(feature.id)
            return False
        self._add_new_shape(feature.id, feature.shape, feature.build_properties())
        return True

    def process_material_areas(self):
        for area in self.sitenode.material_areas:
            self._add_or_reject_polygonal_feature(area)

    def process_infrastructure_landtakes(self):
        for landtake in self.sitenode.landtakes:
            self._add_or_reject_polygonal_feature(landtake)

    def process_all_features(self):
        self.process_level_curves()
        self.process_material_areas()
        self.process_infrastructure_landtakes()

    def export_cleaned_geometries_into(self, hostcleaner):
        """Create new geometry and info into the hostside representing the
        cleaned geometry for each feature of this cleaner

        Info are shared between the self and the hostcleaner
        """
        for feature_id, shape in self.geom.iteritems():
            hostcleaner._add_new_shape(feature_id, shape, self.info[feature_id])


    def merge_subsite(self, subsite):
        "Merge the cleaned geometries for subsite into the self cleaner"
        subcleaner = SiteNodeGeometryCleaner(subsite)
        subcleaner.process_all_features()
        if subcleaner.erroneous_overlap:
            msg = ("Can not merge subsite {subsite} because of features "
                   "overlapping its boundaries.")
            raise InconsistentGeometricModel(msg, subsite=subsite.id,
                                             ids=subcleaner.erroneous_overlap)
        subcleaner.export_cleaned_geometries_into(self)

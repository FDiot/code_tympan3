"""Merging sub-sites for the computation of the altimetry.


The main entry point is the function recursively_merge_all_subsites.
It uses the class SiteNodeGeometryCleaner to perform a recursive merge
of all the site note of a compound site to build a single site.
"""


from collections import defaultdict

from shapely import geometry
from . datamodel import SiteNode, InconsistentGeometricModel


def recursively_merge_all_subsites(rootsite):
    """Merges all subsites and their subsites and so on into this merger."""
    cleaned = SiteNodeGeometryCleaner(rootsite)
    cleaned.process_all_features()
    subsites_to_be_processed = list(rootsite.subsites)
    while subsites_to_be_processed:
        current_site = subsites_to_be_processed.pop()
        cleaned.merge_subsite(current_site)
        subsites_to_be_processed.extend(current_site.subsites)
    return cleaned

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
    geometry we are sure to be within the site landtake and outside the
    landtake of any subsite.

    The level curves are cut and the polygonal features are filtered
    according to the following policy:

    * the IDs of feature overlapping the boundary of the site node are
      reported in the ``erroneous_overlap`` list attributes

    * the IDs of feature entirely out of the site node are reported in
      the ``ignored_feature`` list attributes

    The new geometry computed for each feature is available through
    the ``geom`` dictionary attribute, which is indexed by the
    feature IDs.

    An equivalent site is maintained with the original features.  The
    class also implement a __getitem__ method so that both the new
    geometry and the original properties can be accessed with ``geom,
    info = cleaner[feature_id]``.
    """

    def __init__(self, sitenode):
        if not isinstance(sitenode, SiteNode):
            raise TypeError("A altimetry.datamodel.SiteNode was expected")
        self.sitenode = sitenode
        self.siteshape = build_site_shape_with_hole(self.sitenode)
        self.geom = {}
        self.ignored_features = []
        self.erroneous_overlap = []
        self._sorted_material_areas = []
        self.equivalent_site = SiteNode(sitenode.build_coordinates()[0], id=None)

    def _add_feature_with_new_shape(self, feature, shape):
        assert not isinstance(feature, SiteNode)
        id_ = feature.id
        existing_shape = self.geom.pop(id_, None)
        if existing_shape and not existing_shape.equals(shape):
            raise ValueError("ID %s is already associated to a different shape %s" %
                             (id_, existing_shape.wkt))
        self.equivalent_site.add_child(feature)
        self.geom[id_] = shape

    def feature_from_id(self, id):
        return self.equivalent_site.features_by_id[id]

    def __getitem__(self, feature_id):
        return self.geom[feature_id], self.feature_from_id(feature_id).build_properties()

    def process_level_curves(self):
        for level_curve in self.sitenode.level_curves:
            shape = level_curve.shape.intersection(self.siteshape)
            self._add_feature_with_new_shape(level_curve, shape)

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
        self._add_feature_with_new_shape(feature, feature.shape)
        return True

    def process_material_areas(self):
        for area in self.sitenode.material_areas:
            if self._add_or_reject_polygonal_feature(area):
                pos = self.insert_position_for_sorted_material_area(area)
                self._sorted_material_areas.insert(pos, area.id)

    def process_infrastructure_landtakes(self):
        for landtake in self.sitenode.landtakes:
            self._add_or_reject_polygonal_feature(landtake)

    def process_all_features(self):
        self.process_level_curves()
        self.process_material_areas()
        self.process_infrastructure_landtakes()

    def import_cleaned_geometries_from(self, othercleaner):
        """Create new geometry and info into the site of this cleaner
        representing the cleaned geometry for each feature of the other
        cleaner.

        Info are shared between the self and the other cleaner.
        """
        for feature_id, shape in othercleaner.geom.iteritems():
            self._add_feature_with_new_shape(
                othercleaner.feature_from_id(feature_id), shape)

    def merge_subsite(self, subsite):
        """Merge the cleaned geometries for subsite into the self cleaner"""
        subcleaner = SiteNodeGeometryCleaner(subsite)
        subcleaner.process_all_features()
        if subcleaner.erroneous_overlap:
            msg = ("Can not merge subsite {subsite} because of features "
                   "overlapping its boundaries.")
            raise InconsistentGeometricModel(msg, subsite=subsite.id,
                                             ids=subcleaner.erroneous_overlap)
        self.import_cleaned_geometries_from(subcleaner)
        self._merge_subsite_materials(subcleaner)

    def insert_position_for_sorted_material_area(self, inserted_area):
        """Insert the ID of inserted_area into _sorted_material_areas so that
        area appear most inner first.

        Return its position in _sorted_material_areas.

        The invariant is that for each pair of areas in the list at
        position i and j with i<j, either A(i) and A(J) are disjoint
        or A(i) is included in A(j).

        """
        for i, area_id in enumerate(self._sorted_material_areas):
            area_geom, area_info = self[area_id]
            if inserted_area.shape.overlaps(area_geom):
                msg = ("Partially overlapping material area in {subsite}")
                raise InconsistentGeometricModel(msg, subsite=self.sitenode.id,
                                                 ids=[area_id, inserted_area.id])
            if inserted_area.shape.within(area_geom):
                return i
        else:
            # The inserted_area is disjoint with all others, insert it at the end
            return len(self._sorted_material_areas)

    def _merge_subsite_materials(self, subcleaner):
        """Expect subcleaner is a cleaner for a subsite of self.sitenode and
        import into self the ordered list of the subsite material
        areas
        """
        # A site node is not a material area but has a geometry which
        # is enough to call insert_position_for_sorted_material_area to get the
        # place where to insert its own material
        pos = self.insert_position_for_sorted_material_area(subcleaner.sitenode)
        self._sorted_material_areas[pos:pos] = subcleaner.material_areas_inner_first()


    def check_issues_with_material_area_order(self):
        """ Diagnostic helper: returns violation of the ordering for material area"""
        problems = []
        for i, area_i_id in enumerate(self._sorted_material_areas):
            for j in xrange(i+1, len(self._sorted_material_areas)):
                area_j_id = self._sorted_material_areas[j]
                area_i_geom = self.geom[area_i_id]
                area_j_geom = self.geom[area_j_id]
                if not (area_i_geom.within(area_j_geom) or
                        area_i_geom.disjoint(area_j_geom)):
                    problems.append((area_id, ))
        return problems

    def material_areas_inner_first(self):
        return list(self._sorted_material_areas)

"""
Provide chaining of the step required to build the altimetry of a compound site.
"""

import numpy as np
from shapely import geometry

from .datamodel import (InconsistentGeometricModel,
                        DEFAULT_MATERIAL,
                        elementary_shapes)
from .merge import recursively_merge_all_subsites
from .mesh import (ElevationMesh, ReferenceElevationMesh,
                   Vertex_handle,
                   LandtakeFaceFlooder, MaterialFaceFlooder)


class Builder(object):

    def __init__(self, mainsite):
        self.mainsite = mainsite
        self.cleaned = None # The cleaned and merged site
        self.alti = ReferenceElevationMesh() # Altimetric base
        self.mesh = None
        self.vertices_for_feature = {} # List of vertex handle for a given feature
        self.material_by_face = {}
        self.size_criterion = 0.0 # zero means no size criterion
        self.shape_criterion = 0.125

    @property
    def equivalent_site(self):
        return self.cleaned.equivalent_site

    def complete_processing(self):
        self.merge_subsites()
        self.build_altimetric_base()
        self.build_triangulation()
        self.refine_triangulation()
        self.compute_informations()
        self.compute_elevations()
        self.fill_material_and_landtakes()

    def merge_subsites(self):
        assert self.cleaned is None
        self.cleaned = recursively_merge_all_subsites(self.mainsite)

    def insert_feature(self, feature, mesher=None, **properties):
        try:
            shape = self.cleaned.geom[feature.id]
        except KeyError:
            # The element was filtered out (e.g. it was outside of its sub-site)
            return None
        mesher = mesher or self.mesh
        for polyline in elementary_shapes(shape):
            if isinstance(polyline, geometry.LineString):
                points = polyline.coords[:]
            elif isinstance(polyline, geometry.Polygon):
                if list(polyline.interiors):
                    raise ValueError("Polygons with holes are not (yet) supported")
                points = polyline.exterior.coords[:]
                # NB: polygons' coordinates sequences are automatically closed
            else:
                raise TypeError("Only level curves or waterbodies are expected")
            vertices, _ = mesher.insert_polyline(
                points, id=feature.id, **properties)
        return vertices

    def build_altimetric_base(self):
        assert self.cleaned is not None
        assert self.mesh is None
        for level_curve in self.equivalent_site.level_curves:
            props = level_curve.build_properties()
            assert 'altitude' in props
            vertices = self.insert_feature(level_curve, mesher=self.alti, **props)
            self.vertices_for_feature[level_curve.id] = vertices
        self.alti.update_info_for_vertices()
        self.mesh = self.alti.copy_as_ElevationMesh()

    def build_triangulation(self):
        assert self.mesh is not None
        for feature in self.equivalent_site.non_altimetric_features:
            vertices = self.insert_feature(feature, **feature.build_properties())
            self.vertices_for_feature[feature.id] = vertices

    def refine_triangulation(self):
        # TODO (optional) flood landtake in order to mark them as not to be refined
        self.mesh.refine_mesh(size_criterion=self.size_criterion,
                              shape_criterion=self.shape_criterion)

    def compute_informations(self):
        self.mesh.update_info_for_vertices()
        self.mesh.update_info_for_edges()

    def compute_elevations(self):
        self.mesh.update_altitude_from_reference(self.alti.point_altitude)

    def fill_polygonal_feature(self, feature, flooder_class):
        assert self.mesh is not None
        if feature.id not in self.mainsite.features_by_id:
            raise ValueError("Only features already inserted can be filled ID:%s"
                             % feature.id)
        vertices = self.vertices_for_feature[feature.id]
        close_it = vertices[0] != vertices[-1]
        flooder = self.mesh.flood_polygon(flooder_class, vertices,
                                            close_it=close_it)
        affected_faces =[fh for fh in flooder.visited
                         if fh not in self.material_by_face]
        for fh in affected_faces:
            self.material_by_face[fh] = feature.material
        return affected_faces

    def fill_material_and_landtakes(self):
        assert self.mesh is not None
        assert len(self.material_by_face)==0
        for landtake in self.mainsite.landtakes:
            self.fill_polygonal_feature(landtake,
                                        flooder_class=LandtakeFaceFlooder)
        for material_area_id in self.cleaned.material_areas_inner_first():
            material_area = self.equivalent_site.features_by_id[material_area_id]
            self.fill_polygonal_feature(material_area,
                                        flooder_class=MaterialFaceFlooder)
        for fh in self.mesh.cdt.finite_faces():
            if fh not in self.material_by_face:
                self.material_by_face[fh] = DEFAULT_MATERIAL

    def _build_mesh_data(self):
        """Process mesh from the CDT and materials data and return numpy
        arrays suitable for export to a .ply file.
        """
        vertices, vertices_id = [], {}
        for idx, vh in enumerate(self.mesh.cdt.finite_vertices()):
            vertices_id[vh] = idx
            point = self.mesh.point3d_for_vertex(vh)
            vertices.append((point.x(), point.y(), point.z()))
        vertices = np.array(vertices)
        materials, materials_id = [], {}
        for fh, mat in self.material_by_face.iteritems():
            matid = map(ord, mat.id)
            assert len(matid) <= 36
            matid = matid + [ord('\0')] * (36 - len(matid))
            if matid not in materials:
                materials.append(matid)
            idx = materials.index(matid)
            materials_id[fh] = idx
        materials = np.array(materials)
        faces, faces_materials = [], []
        for fh in self.mesh.cdt.finite_faces():
            faces.append([vertices_id[fh.vertex(i)] for i in range(3)])
            faces_materials.append(materials_id[fh])
        faces = np.array(faces)
        faces_materials = np.array(faces_materials)
        return vertices, faces, materials, faces_materials

    def export_to_ply(self, fname):
        """Export mesh data to a PLY file"""
        vertices, faces, materials, faces_materials = self._build_mesh_data()
        header = '\r\n'.join(['ply',
                              'format ascii 1.0',
                              'element vertex {nvertices}',
                              'property float x',
                              'property float y',
                              'property float z',
                              'element face {nfaces}',
                              'property list uchar int vertex_indices',
                              'property int material_index',
                              'element material {nmaterials}',
                              'property list uchar uchar id',
                              'end_header\r\n'])
        with open(fname, 'w') as f:
            f.write(header.format(nvertices=vertices.shape[0],
                                  nfaces=faces.shape[0],
                                  nmaterials=materials.shape[0]))
            np.savetxt(f, vertices, fmt='%.18g', newline='\r\n')
            # Insert a column with the number of face vertices.
            pfaces = np.concatenate(
                [np.ones((faces.shape[0], 1)) * faces.shape[1],
                 faces, faces_materials[:, np.newaxis]], axis=1)
            np.savetxt(f, pfaces, fmt='%d', newline='\r\n')
            # Insert a column with the lenght of the list of material id.
            pmaterials = np.concatenate(
                [np.ones((materials.shape[0], 1)) * materials.shape[1],
                 materials], axis=1)
            np.savetxt(f, pmaterials, fmt='%d', newline='\r\n')

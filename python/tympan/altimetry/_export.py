"""Export utilities for Tympan altimetry"""

import numpy as np


def export_to_ply(mesh, material_by_face, fname, color_faces=True):
    """Export mesh data to a PLY file.

    `color_faces` option adds colors to mesh faces, mostly for visual
    debug.
    """
    vertices, faces, materials, faces_materials = _build_mesh_data(
        mesh, material_by_face)
    header = _ply_headers(color_faces=color_faces)
    with open(fname, 'w') as f:
        f.write(header.format(nvertices=vertices.shape[0],
                              nfaces=faces.shape[0],
                              nmaterials=len(materials)))
        np.savetxt(f, vertices, fmt='%.18g', newline='\r\n')
        # Insert a leading column with the number of face vertices and a
        # trailing one with face material.
        fcols = [np.ones((faces.shape[0], 1)) * faces.shape[1],
                 faces, faces_materials[:, np.newaxis]]
        if color_faces:
            # Then add faces colors.
            fcols.append(_color_faces(faces_materials))
        np.savetxt(f, np.concatenate(fcols, axis=1), fmt='%d',
                   newline='\r\n')
        # Write materials.
        for matid in materials:
            n = len(matid)
            f.write(('{}' + (' {}' * n) + '\r\n').format(n, *matid))


def _build_mesh_data(mesh, material_by_face):
    """Process mesh from the CDT and materials data and return numpy
    arrays (except for `materials` which is a list of list) suitable for
    export to a .ply file.
    """
    vertices, vertices_id = [], {}
    for idx, vh in enumerate(mesh.cdt.finite_vertices()):
        vertices_id[vh] = idx
        point = mesh.point3d_for_vertex(vh)
        vertices.append((point.x(), point.y(), point.z()))
    vertices = np.array(vertices)
    materials, materials_id = [], {}
    for fh, mat in material_by_face.iteritems():
        matid = map(ord, mat.id)
        if matid not in materials:
            materials.append(matid)
        idx = materials.index(matid)
        materials_id[fh] = idx
    faces, faces_materials = [], []
    for fh in mesh.cdt.finite_faces():
        faces.append([vertices_id[fh.vertex(i)] for i in range(3)])
        faces_materials.append(materials_id[fh])
    faces = np.array(faces)
    faces_materials = np.array(faces_materials)
    return vertices, faces, materials, faces_materials


def _ply_headers(color_faces=True):
    """Generate PLY file header for mesh export"""
    faces_headers = ['element face {nfaces}',
                     'property list uchar int vertex_indices',
                     'property int material_index']
    if color_faces:
        faces_headers += ['property uchar red',
                          'property uchar green',
                          'property uchar blue']
    headers = (['ply',
                'format ascii 1.0',
                'element vertex {nvertices}',
                'property float x',
                'property float y',
                'property float z'] +
               faces_headers +
               ['element material {nmaterials}',
                'property list uchar uchar id',
                'end_header'])
    return '\r\n'.join(headers) + '\r\n'


def _color_faces(faces_materials):
    """Return an array of shape (nfaces, 3) with colors for mesh faces
    according to their material.
    """
    faces_colors = np.zeros((faces_materials.shape[0], 3))
    try:
        from matplotlib import cm
    except ImportError:
        return faces_colors
    cmap = cm.get_cmap('jet')
    materials, indices = np.unique(faces_materials, return_inverse=True)
    nmats = len(materials)
    colors = cmap(np.arange(nmats) / float(nmats - 1), bytes=True)[:, :-1]
    return colors[indices, :]

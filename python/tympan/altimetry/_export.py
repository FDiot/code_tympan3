"""Export utilities for Tympan altimetry"""

import numpy as np


def export_to_ply(mesh, material_by_face, fname, color_faces=True):
    """Export mesh data to a PLY file.

    `color_faces` option adds colors to mesh faces, mostly for visual
    debug.
    """
    # Fetch vertices and faces arrays.
    vertices, faces = mesh.as_arrays()
    # Build a list of materials and a list faces' material with identical
    # indexing.
    materials = []
    faces_materials = mesh.faces_material(material_by_face)
    for idx, mat in enumerate(faces_materials):
        matid = list(map(ord, mat.id))
        if matid not in materials:
            materials.append(matid)
        faces_materials[idx] = materials.index(matid)
    # Need a column array for further concatenation.
    faces_materials = np.array(faces_materials).reshape(-1, 1)
    # Set PLY file header.
    header = _ply_headers(color_faces=color_faces)
    # Write PLY file.
    with open(fname, 'wb') as f:
        f.write(header.format(nvertices=vertices.shape[0],
                              nfaces=faces.shape[0],
                              nmaterials=len(materials)).encode('ascii'))
        np.savetxt(f, vertices, fmt='%.18g', newline='\r\n')
        # Insert a leading column with the number of face vertices and a
        # trailing one with face material.
        fcols = [np.ones((faces.shape[0], 1)) * faces.shape[1],
                 faces, faces_materials]
        if color_faces:
            # Then add faces colors.
            fcols.append(_color_faces(faces_materials))
        np.savetxt(f, np.concatenate(fcols, axis=1), fmt='%d',
                   newline='\r\n')
        # Write materials.
        for matid in materials:
            n = len(matid)
            f.write(('{}' + (' {}' * n) + '\r\n').format(n, *matid).encode('ascii'))


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
    from matplotlib import cm
    materials, indices = np.unique(faces_materials, return_inverse=True)
    nmats = len(materials)
    if nmats == 1:
        return np.zeros((faces_materials.shape[0], 3))
    cmap = cm.get_cmap('jet')
    colors = cmap(np.arange(nmats) / float(nmats - 1), bytes=True)[:, :-1]
    return colors[indices, :]

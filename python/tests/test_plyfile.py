import os
import numpy  as np
import unittest
import tempfile


DEBUG_TMP_FILES = os.environ.get('DEBUG_TMP_FILES', False)

class PLYFileTC(unittest.TestCase):
    def test_can_import(self):
        try:
            import plyfile
        except ImportError:
            self.fail("Can not import plyfile module : check its installation in the build tree")

    def build_ply_single_triangle(self):
        from plyfile import PlyElement, PlyData
        points_dt = np.dtype([('x', float), ('y', float), ('z', float)])
        face_dt = np.dtype([('vertex_indices', np.uint32, (3,))])
        # The numpy array data.
        points = np.array([(-1,0,0), (0,1,0), (1,0,0)], dtype=points_dt)
        triangles = np.array([((1, 0, 2),),], dtype=face_dt) #The Tuple with a tuple inside is required Why !?!
        elements = [PlyElement.describe(points, "vertex"),
                    PlyElement.describe(triangles, "face",
                                        len_types={'vertex_indices': 'u1'},
                                        val_types={'vertex_indices': 'u4'}),]
        ply_data = PlyData(elements, text=True)
        return ply_data

    def test_write_trivial_file(self):
        ply_data = self.build_ply_single_triangle()

        with tempfile.NamedTemporaryFile(suffix='.ply', delete=(not DEBUG_TMP_FILES)) as f:
            ply_data.write(f)
            if DEBUG_TMP_FILES:
                print("Temp. file for test_write_trivial_file: ", f.name)


if __name__ == '__main__':
    unittest.main()

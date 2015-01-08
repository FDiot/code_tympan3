from tympan.models import filter_output
from tympan.models._business import Project as cyProject


class Project(object):
    """Acoustic project describing an industrial site"""

    cyclass = cyProject

    def __init__(self, project):
        """Build a project from a project cython object)"""
        self._project = project

    def __getattr__(self, name):
        return getattr(self._project, name)

    def update_site_altimetry(self, mesh, material_by_face, verbose=False):
        """Update the altitude of the site infrastructure items

        Params:

            - altimetry mesh: tympan.altimetry.mesh.ElevationMesh
            - material_by_face: face id to material id mapping

        Site infrastructure items whose altitude can be updated are machines,
        buildings, sources, receptors, etc.
        """
        with filter_output(verbose):
            self._project._update_site_altimetry(mesh, material_by_face)

    @classmethod
    def from_xml(cls, fpath, verbose=False):
        """Create a project from a XML file path"""
        with filter_output(verbose):
            return cls(cls.cyclass.from_xml(fpath))

    def import_result(self, model, solver_result):
        """Update project's site acoustic according to solver result"""
        model._converter.postprocessing(model._model, solver_result)

from tympan.altimetry import AltimetryMesh
from tympan.models import filter_output
from tympan.models._business import Project as cyProject
from tympan.models._common import Spectrum


class Project(object):
    """Acoustic project describing an industrial site"""

    cyclass = cyProject

    def __init__(self, project):
        """Build a project from a project cython object)"""
        self._project = project
        self._altimetry_mesh = None

    def __getattr__(self, name):
        return getattr(self._project, name)

    def update_site_altimetry(self, verbose=False):
        """Update the altitude of the site infrastructure items

        Site infrastructure items whose altitude can be updated are machines, buildings, sources,
        receptors, etc.
        """
        with filter_output(verbose):
            self._build_altimetry_mesh()
            self._project._update_site_altimetry(self._altimetry_mesh.mesh,
                                                 self._altimetry_mesh.material_by_face)

    def _build_altimetry_mesh(self):
        """Build a mesh out of the project' site altimetry"""
        if self._altimetry_mesh is None:
            self._altimetry_mesh = AltimetryMesh.from_site(self._project.site)

    def export_altimetry(self, output_fpath):
        """Write site altimetry mesh to `output_fpath` (ply format)"""
        self._build_altimetry_mesh()
        self._altimetry_mesh.to_ply(output_fpath)

    @property
    def computations(self):
        """Project computations"""
        return self._project.computations

    @property
    def current_computation(self):
        """Project current computation"""
        return self._project.current_computation

    @property
    def site(self):
        """Project site"""
        return self._project.site

    def to_xml(self, filepath):
        """Export the project to a `filepath` XML file"""
        self._project.to_xml(filepath)

    @classmethod
    def from_xml(cls, fpath, verbose=False, update_altimetry=True):
        """Create a project from `fpath` XML file path, on the way update project site
        infrastructure altimetry if `update_altimetry` is True
        """
        with filter_output(verbose):
            project = cls(cls.cyclass.from_xml(fpath))
        if update_altimetry:
            project.update_site_altimetry(verbose)
        return project

    @classmethod
    def create(cls):
        return cls(cls.cyclass.create())

    def import_result(self, model, solver_result):
        """Update project's site acoustic according to solver result"""
        model._converter.postprocessing(model._model, solver_result)


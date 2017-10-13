from tympan.altimetry import AltimetryMesh
from tympan.models import filter_output
from tympan.models._business import Project as cyProject
from tympan.models._common import Spectrum
from tympan.models.solver import fetch_solverdir


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

    def add_computation(self, current=True):
        """Add a new empty computation to the project, set it as the default computation if
        `current` is True and return it
        """
        comp = self._project.add_computation()
        if current:
            self._project.select_computation(comp)
            #self._project.set_current_computation(comp)
        return comp

    def select_computation(self, computation):
        """Set project current computation to `computation` (`computation` MUST be part of project'
        computations)
        """
        self._project.select_computation(computation)

    @property
    def computations(self):
        """Project computations"""
        return self._project.computations

    @property
    def current_computation(self):
        """Project current computation"""
        return self._project.current_computation

    def set_solver(self, solvername, computation=None, solverdir=None):
        """Set solver `solvername` to computation `computation`

        `solverdir` is the directory where one can find the solver library. If None, it will
         be retrieved from "TYMPAN_SOLVERDIR" environment variable, which must be defined.
         if `computation` is None, the solver will be set to the current computation of the project
        """
        solverdir = solverdir or fetch_solverdir()
        computation = computation or self._project.current_computation
        computation.set_solver(solverdir, solvername)

    @property
    def site(self):
        """Project site"""
        return self._project.site

    def to_xml(self, filepath):
        """Export the project to a `filepath` XML file"""
        self._project.to_xml(filepath)

    def add_user_receptor(self, position, height, name):
        self._project.add_user_receptor(position, height, name)

    def set_sig_type(self, type_sig):
        self._project.set_sig_type(type_sig)

    def set_sig_x(self, x):
        self._project.set_sig_x(x)

    def set_sig_y(self, y):
        self._project.set_sig_y(y)

    def set_sig_offset(self, offset):
        self._project.set_sig_offset(offset)

    @property
    def sig_type(self):
        return self._project.sig_type()

    @property
    def sig_x(self):
        return self._project.sig_x()

    @property
    def sig_y(self):
        return self._project.sig_y()

    @property
    def sig_offset(self):
        return self._project.sig_offset()

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


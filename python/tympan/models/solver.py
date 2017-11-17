import configparser
import json
from io import StringIO
import os

from tympan.models import filter_output, Spectrum
from tympan.models import _solver as cysolver
from tympan import _business2solver

_CONVERTERS = {
    'bool': lambda x: x.lower() == 'true',
    'int': int,
    'float': float,
    'double': float,
}

_CONFIG_MODEL_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), os.pardir, os.pardir,
                                  os.pardir, 'resources', 'solver_config_datamodel.json')
with open(_CONFIG_MODEL_FILE) as stream:
    _CONFIG_MODEL = json.load(stream)
_SOLVER_CONFIG_ATTRS = []
for category, options in _CONFIG_MODEL.items():
    for option in options:
        _SOLVER_CONFIG_ATTRS.append((options[option]['type'], option))
_CONFIG_MAP = dict((optname, _CONVERTERS[opttype]) for opttype, optname in _SOLVER_CONFIG_ATTRS)


class Source(object):
    """An acoustic source.

    Parameters
    ----------

    position : tuple
        (x, y, z) coordinates
    spectrum : Spectrum
        acoustic power spectrum
    shift : int, optional
        the number of the first frequence for which a DB value is given
        (frequences start at 16 Hz and end at 16000Hz, 31 values in total)
    directivity: Directivity, optional
        source directivity
    """

    def __init__(self, position, spectrum, directivity=None):
        self.position = position
        self.spectrum= spectrum
        if directivity is None:
            directivity = cysolver.Directivity()
        self.directivity = directivity


class Receptor(object):
    """An acoustic receptor.

    Parameters
    ----------

    position : tuple
        (x, y, z) coordinates
    """

    def __init__(self, position):
        self.position = position
        self._spectrum = Spectrum()

    @property
    def spectrum(self):
        """Acoustic power spectrum of this Receptor."""
        return self._spectrum


class Model(object):
    """Model describing a site made of a mesh, sources, and receptors

    Used for acoustic simulation.
    """

    def __init__(self):
        """Create an empty model"""
        self._model = cysolver.ProblemModel()

    @classmethod
    def from_project(cls, project, set_sources=True, set_receptors=True):
        """Create a solver model from a project"""
        model = cls()
        model._converter = _business2solver.Business2SolverConverter(
            project.current_computation, project.site)
        model._converter.build_mesh(model._model)
        if set_sources:
            model._converter.build_sources(model._model)
        if set_receptors:
            model._converter.build_receptors(model._model)
        return model

    def add_source(self, source):
        """Add an acoustic source to the model."""
        return self._model._add_source(
            source.position, source.spectrum, source.directivity)

    def add_receptor(self, receptor):
        return self._model.add_receptor(*receptor.position)

    def export_triangular_mesh(self):
        """Build a triangular mesh from the acoustic problem model

        Return two nparrays:

            - 'nodes': an array of nodes (of dimension 'npoints'X3), where
              each line stands for a node and contains 3 coordinates)
            - 'triangles': an array of triangles (of dimension 'ntriangles'X3),
              where each line stands for a triangle and contains the indices of
              its 3 vertices in the 'nodes' array.
        """
        return self._model._export_triangular_mesh()

    @property
    def sources(self):
        """Acoustic sources of the model"""
        return self._model.sources

    @property
    def receptors(self):
        """Acoustic receptors of the model"""
        return self._model.receptors

    @property
    def nsources(self):
        """Return the number of acoustic sources involved in the model"""
        return self._model.nsources

    @property
    def nreceptors(self):
        """Return the number of acoustic receptors involved in the model"""
        return self._model.nreceptors

    @property
    def triangles(self):
        """Acoustic triangles of the model"""
        return self._model.triangles

    def node_coords(self, idx):
        """Return a tuple with the 3D coordinates for the node of id 'idx'"""
        return self._model.node_coords(idx)

    def __getattr__(self, name):
        return getattr(self._model, name)


class Solver(object):
    """Acoustic solver"""

    _solvers = {
        'default': 'DefaultSolver',
        'anime3d': 'ANIME3DSolver',
    }

    def __init__(self, solver):
        """Create a solver from a solver cython object"""
        super(Solver,self).__setattr__('_solver',solver)

    @classmethod
    def from_name(cls, name, parameters_file, solverdir=None, verbose=False):
        """Return an instance of solver with `name`.

        `name` can be "default" or "Anime3D" for instance.
        """
        solverdir = solverdir or fetch_solverdir()
        with open(parameters_file) as fp:
            _set_solver_config(fp)
        solver_name = cls._solvers.get(name.lower(), name)
        with filter_output(verbose):
            solver = _business2solver.acoustic_solver_by_name(
                solver_name, solverdir)
        return cls(solver)

    @classmethod
    def from_project(cls, project, solverdir=None, verbose=False):
        """Load and configure solver

        'solverdir' is the directory where one can find the solver library. If None, it will
        be retrieved from "TYMPAN_SOLVERDIR" environment variable, which must be defined. The
        configuration is read from the project.
        """
        solverdir = solverdir or fetch_solverdir()
        parameters_fp = StringIO(project.current_computation.solver_parameters.decode('utf-8'))
        _set_solver_config(parameters_fp)
        with filter_output(verbose):
            solver = _business2solver.acoustic_solver_from_computation(
                project.current_computation, solverdir)
        return cls(solver)
    
    def __getattr__(self,name):
        """Retrieve the parameters in cysolver.Configuration"""
        capital_name = 'get' + ''.join(s[0].upper() + s[1:] for s in name.split('_'))
        return getattr(cysolver.Configuration.get(),capital_name)()
        
    def __setattr__(self,name,value):
        """Setting the parameters in cysolver.Configuration"""
        capital_name = 'set' + ''.join(s[0].upper() + s[1:] for s in name.split('_'))
        return getattr(cysolver.Configuration.get(),capital_name)(value)

    def solve(self, model):
        """Solve the acoustic problem described in the model (run a computation)"""
        return self._solver.solve_problem(model._model)


def fetch_solverdir():
    """Try to retrieve solver plugins directory from 'TYMPAN_SOLVERDIR' environment variable

    If the environment variable is not defines, raise a RuntimeError.
    """
    try:
        return os.environ['TYMPAN_SOLVERDIR']
    except KeyError:
        raise RuntimeError('"TYMPAN_SOLVERDIR" environment variable must be set to path to the '
                           'solver libraries directory')


def _set_solver_config(parameters_fp):
    """Setup solver configuration"""
    parser = configparser.RawConfigParser()
    parser.optionxform = str  # keep param names case
    parser.readfp(parameters_fp)
    solver_config = cysolver.Configuration.get()
    errors = []
    for section in parser.sections():
        for optname, value in parser.items(section):
            try:
                value = _CONFIG_MAP[optname](value)
            except ValueError:
                errors.append('bad option value for %s: %r' % (optname, value))
                continue
            except KeyError:
                continue
            setattr(solver_config, optname, value)
    if errors:
        raise configparser.Error(os.linesep.join(errors))

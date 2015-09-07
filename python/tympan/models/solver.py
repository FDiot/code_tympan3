import ConfigParser
import json
from StringIO import StringIO
import os

from tympan.models import filter_output
from tympan.models import _solver as cysolver
from tympan._business2solver import Business2SolverConverter, load_computation_solver

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
for category, options in _CONFIG_MODEL.iteritems():
    for option in options:
        _SOLVER_CONFIG_ATTRS.append((options[option]['type'], option))
_CONFIG_MAP = dict((optname, _CONVERTERS[opttype]) for opttype, optname in _SOLVER_CONFIG_ATTRS)


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
        model._converter = Business2SolverConverter(project.current_computation,
                                                    project.site)
        model._converter.build_mesh(model._model)
        if set_sources:
            model._converter.build_sources(model._model)
        if set_receptors:
            model._converter.build_receptors(model._model)
        return model

    def add_source(self, position, spectrum_values, shift):
        """Add an acoustic source to the model

        Params:

            - position is a (x, y, z) tuple
            - spectrum_values is a np array containing the DB values of a power
              spectrum
            - shift is the number of the first frequence for which a DB value
              is given (frequences start at 16 Hz and end at 16000Hz,
              31 values in total).

        Add the corresponding acoustic source to the solver model (the acoustic
        source is created with a SphericalFaceDirectivity)
        """
        return self._model._add_source(position, spectrum_values, shift)

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

    def __init__(self, solver):
        """Create a solver from a solver cython object"""
        self._solver = solver

    @property
    def nthread(self):
        """Number of threads used to run the computation"""
        return self._solver.Configuration.get().NbThreads

    @nthread.setter
    def nthread(self, nthr):
        self._solver.Configuration.get().NbThreads = nthr

    def solve(self, model):
        """Solve the acoustic problem described in the model (run a computation)"""
        return self._solver.solve_problem(model._model)

    @classmethod
    def from_project(cls, project, solverdir=None, verbose=False):
        """Load and configure solver

        'solverdir' is the directory where one can find the solver library. If None, it will
        be retrieved from "TYMPAN_SOLVERDIR" environment variable, which must be defined. The
        configuration is read from the project.
        """
        try:
            solverdir = solverdir or os.environ['TYMPAN_SOLVERDIR']
        except KeyError:
            raise RuntimeError('"TYMPAN_SOLVERDIR" environment variable must be set to path to the '
                               'solver libraries directory')
        _set_solver_config(project.current_computation)
        with filter_output(verbose):
            solver = load_computation_solver(solverdir, project.current_computation)
        return cls(solver)


def _set_solver_config(comp):
    """Setup solver configuration"""
    parser = ConfigParser.RawConfigParser()
    parser.optionxform = str # keep param names case
    parser.readfp(StringIO(comp.solver_parameters))
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
        raise ConfigParser.Error(os.linesep.join(errors))


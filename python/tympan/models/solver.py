import ConfigParser
from StringIO import StringIO
import os

from tympan import SOLVER_CONFIG_ATTRIBUTES
from tympan.models import _solver as cysolver
from tympan._business2solver import Business2SolverConverter, load_computation_solver

_CONVERTERS = {
    'bool': bool,
    'int': int,
    'float': float,
    'double': float,
}

_CONFIG_MAP = dict((optname, _CONVERTERS[opttype]) for opttype, optname in SOLVER_CONFIG_ATTRIBUTES)


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
    def from_project(cls, project, solverdir):
        """Load and configure solver

        'solverdir' is the directory where one can find the solver library. The
        configuration is read from the project.
        """
        _set_solver_config(project.current_computation)
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
            getattr(solver_config, optname, value)
    if errors:
        raise ConfigParser.Error(os.linesep.join(errors))


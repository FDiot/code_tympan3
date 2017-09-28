"""Acoustic Ray Tracer module"""
from tympan.models import _acousticraytracer as cyAcousticRayTracer

class Simulation(object):
    """Main object Simulation for Ray Tracer
    """
    def __init__(self):
        """Create an empty simulation"""
        self._simulation = cyAcousticRayTracer.cySimulation()

    def add_source(self, source):
        """Add a source (with default sampler)"""
        self._simulation.addSource(source.cysource)

    def add_recepteur(self, recepteur):
        """Add a receptor"""
        self._simulation.addRecepteur(recepteur.cyrecepteur)

    def set_solver(self, solver):
        """Add the solver"""
        self._simulation.setSolver(solver.cysolver)

    def set_accelerator(self):
        """Set the accelerator (by default, the ToDo one)"""
        self._simulation.setAccelerator()

    def set_engine(self):
        """Set the engine (by default, the DefaultEngine one)"""
        self._simulation.setEngine()

    def launch_simulation(self):
        """Launch the ray tracer process"""
        return self._simulation.launchSimulation()

    def clean(self):
        """Clean the process"""
        self._simulation.clean()

    def configuration(self):
        """Return the ray tracer configuration"""
        return self._simulation.getConfiguration()

    def export_scene(self, filename):
        """Export a Scene to a ply file"""
        self._simulation.export_to_ply(filename)

    def import_scene(self, filename):
        """Import a Scene from a ply file"""
        self._simulation.import_from_ply(filename)

    def get_scene(self):
        """Get the Scene"""
        self._simulation.getScene()


class Source(object):
    """Source"""
    def __init__(self, x=0, y=0, z=0):
        """Create an empty source"""
        self.cysource = cyAcousticRayTracer.cySource(x, y, z)

class Recepteur(object):
    """Receptor"""
    def __init__(self, x=0, y=0, z=0, r=0):
        """Create an empty receptor"""
        self.cyrecepteur = cyAcousticRayTracer.cyRecepteur(x, y, z, r)


class Solver(object):
    """Solver"""
    def __init__(self):
        """Create a solver"""
        self.cysolver = cyAcousticRayTracer.cySolver()

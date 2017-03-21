from tympan.models import _acousticraytracer as cyAcousticRayTracer


class Simulation(object):
    """Acoustic Ray Tracing.
    """
    def __init__(self):
        """Create an empty simulation"""
        self._simulation = cyAcousticRayTracer.cySimulation()

    def add_source(self, s):
        """Add a source (with default sampler)"""
        self._simulation.addSource(s._source)

    def add_recepteur(self, r):
        """Add a receptor"""
        self._simulation.addRecepteur(r._recepteur)

    def set_solver(self, s):
        """Add the solver"""
        self._simulation.setSolver(s._solver)

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
    

class Source(object):
    """Source"""
    def __init__(self, x=0, y=0, z=0):
        """Create an empty source"""
        self._source = cyAcousticRayTracer.cySource(x,y,z)
        

class Recepteur(object):
    """Receptor"""
    def __init__(self, x=0, y=0, z=0, r=0):
        """Create an empty receptor"""
        self._recepteur = cyAcousticRayTracer.cyRecepteur(x,y,z,r)


class Solver(object):
    """Solver"""
    def __init__(self):
        """Create a solver"""
        self._solver = cyAcousticRayTracer.cySolver()      

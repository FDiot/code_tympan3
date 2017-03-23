import unittest
import tempfile
from utils import TympanTC
from tympan.models.acousticraytracer import Simulation, Source, Recepteur, Solver


class TestAcousticRayTracer(TympanTC):
    """Class for AcousticRayTracer tests"""
    def test_basic_changing_parameter(self):
        """Test: changing some parameters"""
        simulation = Simulation()
        simulation.configuration().setNbRaysPerSource(51)
        self.assertEqual(simulation.configuration().getNbRaysPerSource(), 51)

    def test_basic_checking_scene(self):
        """Test: checking the creation of a Scene"""
        simulation = Simulation()
        self.assertEqual(simulation._simulation.getSceneName(), "MyScene")

    def test_basic_change_source_sampler(self):
        """Test: adding a source"""
        simulation = Simulation()
        # Set UniformSphericSampler2 sampler
        simulation.configuration().Discretization = 2
        source = Source()
        simulation.add_source(source)

    def test_basic_read_write_ply_files(self):
        """Test: import/export a Scene"""
        simulation = Simulation()
        ply_file = self.datapath('Scene.ply')
        simulation.import_scene(ply_file)
        with tempfile.NamedTemporaryFile(delete=False) as file:
            simulation.export_scene(file.name)
            # print(f.name)

    def test_complete_running_simulation(self):
        """Test: complete running of a simulation"""
        print("\nTrying to create a simulation ...")
        # Create the RayTracer
        simulation = Simulation()
        # Read a Scene from a PLY file
        ply_file = self.datapath('Scene.ply')
        simulation.import_scene(self.datapath(ply_file))
        # Change some parameters
        simulation.configuration().Discretization = 3
        simulation.configuration().NbRaysPerSource = 11
        # Add somes sources
        source = Source(1, 1, 1)
        simulation.add_source(source)
        source = Source(2, 2, 2)
        simulation.add_source(source)
        # Add a receptor
        receptor = Recepteur(10, 10, 10, 0.4)
        simulation.add_recepteur(receptor)
        # Set a solver
        solver = Solver()
        simulation.set_solver(solver)
        # Set accelerators
        simulation.configuration().Accelerator = 2
        simulation.set_accelerator()
        # Set the engine
        simulation.set_engine()
        print("\nTrying to launch a simulation ...")
        simulation.launch_simulation()
        # Clean the process
        simulation.clean()

if __name__ == '__main__':
    unittest.main()

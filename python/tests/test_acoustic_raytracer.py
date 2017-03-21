import unittest
import tempfile
from utils import TympanTC                    
from tympan.models.acousticraytracer import Simulation, Source, Recepteur, Solver


class TestAcousticRayTracer(TympanTC):
                
    def test_basic_changing_parameter(self):
        simulation = Simulation()
        simulation.configuration().setNbRaysPerSource(51)
        self.assertEqual(simulation.configuration().getNbRaysPerSource(),51)

    def test_basic_checking_scene(self):
        simulation = Simulation()
        self.assertEqual(simulation._simulation.getSceneName(),"MyScene")
    
    def test_basic_change_source_sampler(self):
        simulation = Simulation()
        '''Set UniformSphericSampler2 sampler'''
        simulation.configuration().Discretization=2
        s = Source()
        simulation.add_source(s)
        
    def test_basic_read_write_ply_files(self):
        simulation = Simulation()
        ply_file = self.datapath('Scene.ply')
        simulation.import_scene(ply_file)
        with tempfile.NamedTemporaryFile(delete=False) as f:
            simulation.export_scene(f.name)
            '''print(f.name)'''
                            
    def test_complete_running_simulation(self):
        print("\nTrying to create a simulation ...")
        '''Create the RayTracer'''
        simulation = Simulation()
        '''Read a Scene from a PLY file'''
        ply_file = self.datapath('Scene.ply')
        simulation.import_scene(self.datapath(ply_file))
        '''Change some parameters'''
        simulation.configuration().Discretization=3
        simulation.configuration().NbRaysPerSource=11
        '''Add somes sources'''
        s = Source(1,1,1)
        simulation.add_source(s)
        s = Source(2,2,2)
        simulation.add_source(s)
        '''Add a receptor'''
        r = Recepteur(10,10,10,0.4)
        simulation.add_recepteur(r)
        '''Set a solver'''
        solver = Solver()
        simulation.set_solver(solver)
        '''Set accelerators'''
        simulation.configuration().Accelerator=2
        simulation.set_accelerator()
        ''' Set the engine'''
        simulation.set_engine()
        print("\nTrying to launch a simulation ...")
        simulation.launch_simulation()
        '''Clean the process'''
        simulation.clean()
        
if __name__ == '__main__':
    unittest.main()

/**
*
* @brief Ray tracing tests ensuring the process is not broken

*  Created on: march 14, 2017
*  Author: Fabien Diot <fabien-externe.diot@edf.fr>
*
*/


#include <cstdlib>
#include <time.h> 

#include "gtest/gtest.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Engine/Simulation.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Source.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Latitude2DSampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/PostTreatment.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/UniformSphericSampler2.h"
#include "Tympan/solvers/ANIME3DSolver/TYANIME3DRayTracerSolverAdapter.h"
#include "Tympan/models/solver/config.h"

using std::cout;
using std::cerr;
using std::endl;

int maxSources=10;
int maxReceptors=10;
int maxPos=1000;
int maxRays=100;

// Setup simulation with one source and one receptor
void setup_1source_1recepteur(Simulation* simu,
							  int nbRaysPerSource=0, 
							  int accelerator=2,
							  vec3 source_pos=vec3(5,0,0),
							  vec3 recepteur_pos=vec3(15,0,0)){

	// The configuration of the ray tracer:
	tympan::LPSolverConfiguration config =tympan::SolverConfiguration::get();

	// Init relevant config params:
	config->NbRaysPerSource = nbRaysPerSource;
	config->Accelerator = accelerator;
	config->KeepDebugRay=true;
	
	// Add a source:
	Source source;
	source.setPosition(source_pos);
	source.setSampler(new UniformSphericSampler2(nbRaysPerSource)); 
	source.setInitialRayCount(config->NbRaysPerSource);
	source.setName("src1");
	simu->addSource(source);

	// Add a receptor:
	Recepteur r(recepteur_pos,3);
	r.setName("rcpt1");
	simu->addRecepteur(r);

	// Create a solver
	TYANIME3DRayTracerSolverAdapter* solver=new TYANIME3DRayTracerSolverAdapter();

	// Add diffraction edges and selectors filtering rays  
	solver->postTreatmentScene(simu->getScene(),simu->getSources(),simu->getRecepteurs());
	//PostTreatment::constructEdge(simu->getScene());
	// Add the solver to drive the ray tracing:
	simu->setSolver(solver);

	// Accelerator configuration
	simu->getScene()->finish(config->Accelerator);   // use the configuration's accelerator
	simu->get_receptors_landscape()->finish(1);      // use grid accelerator for receptors

	// Engine setup (DefaultEngine):
	simu->setEngine();
}


// Setup simulation with a random number of sources/receptors with random position and throwing a random number of rays
void setup_random(Simulation* simu,
	std::vector<Source>& sources,
	std::vector<Recepteur>& recepteurs,
	std::vector<vec3>& src_pos,
	std::vector<vec3>& rcpt_pos,
	int nbRaysPerSource=-1,
	unsigned int nbSources=-1,
	unsigned int nbRecepteurs=-1,
	int accelerator=2){

	unsigned int seed=(unsigned int)time(NULL);
	srand(seed);
	cout<<"Random number generator initialized with seed "<<seed<<endl;

	// The configuration of the ray tracer:
	tympan::LPSolverConfiguration config =tympan::SolverConfiguration::get();

	// Init relevant config params:
	config->NbRaysPerSource = nbRaysPerSource ==-1 ? rand() % maxRays + 1 : nbRaysPerSource;
	config->Accelerator = accelerator ==-1 ? rand() % 3 + 1 : accelerator;
	config->KeepDebugRay=true;

	// random number of sources and receptors
	nbSources = nbSources == -1 ? rand() % maxSources + 1 : nbSources;
	nbRecepteurs = nbRecepteurs == -1 ? rand() % maxReceptors + 1 : nbRecepteurs;

	// Create sources
	while(sources.size()<nbSources){

		Source source;
		vec3 pos=vec3((decimal)(rand() % 1000 + 1),(decimal)(rand() % maxPos + 1),(decimal)(rand() % maxPos + 1));
		source.setPosition(pos);
		source.setId(sources.size());
		source.setSampler(new UniformSphericSampler2(nbRaysPerSource)); 
		source.setInitialRayCount(config->NbRaysPerSource);
		source.setName("src"+sources.size());

		//check that the new source is not too close from other sources
		bool keepSource=true;
		for(unsigned int i=0;i<sources.size();i++){
			if(sources.at(i).getPosition().distance(pos)<10){
				keepSource=false;
			}
		}

		if(keepSource){
			simu->addSource(source);
			sources.push_back(source);
			src_pos.push_back(pos);
	    }

	}

	// Create receptors
	while(recepteurs.size()<nbRecepteurs){

		vec3 pos=vec3((decimal)(rand() % 1000 + 1),(decimal)(rand() % maxPos + 1),(decimal)(rand() % maxPos + 1));

		Recepteur recepteur(pos,3);
		recepteur.setName("rcpt"+recepteurs.size());
		recepteur.setId(recepteurs.size());

		//check that the new receptor is not too close from other receptors
		bool keepReceptor=true;
		for(unsigned int i=0;i<recepteurs.size();i++){
			if(recepteurs.at(i).getPosition().distance(pos)<10){
				keepReceptor=false;
			}
		}

		if(keepReceptor){
			simu->addRecepteur(recepteur);
			recepteurs.push_back(recepteur);
			rcpt_pos.push_back(pos);
	    }


	}

	// Create a solver
	TYANIME3DRayTracerSolverAdapter* solver=new TYANIME3DRayTracerSolverAdapter();

	// Add diffraction edges and selectors filtering rays  
	solver->postTreatmentScene(simu->getScene(),simu->getSources(),simu->getRecepteurs());
	//PostTreatment::constructEdge(simu->getScene());
	// Add the solver to drive the ray tracing:
	simu->setSolver(solver);

	// Accelerator configuration
	simu->getScene()->finish(config->Accelerator);   // use the configuration's accelerator
	simu->get_receptors_landscape()->finish(1);      // use grid accelerator for receptors

	// Engine setup (DefaultEngine):
	simu->setEngine();

}

// Test source in simple scene with one source and opne receptor
TEST(test_simulation_1source_1recepteur, test_source)
{

	// Create a ray tracer
	Simulation simu;

	// Setup
	setup_1source_1recepteur(&simu,10);

	// Throw rays
	simu.launchSimulation();

	std::vector<Source>& sources=simu.getSources();

	// Test number of sources
	EXPECT_EQ(1,sources.size());

	Source source=sources.at(0);

	
	EXPECT_EQ("src1",source.getName());						//Test name
	EXPECT_EQ(0,source.getId());							//Test id
	EXPECT_TRUE(source.getPosition().compare(vec3(5,0,0))); //Test position
	EXPECT_EQ(10,source.getInitialRayCount());				//Test initial number of rays
	EXPECT_EQ(0,source.getNbRayLeft());						//Test nb rays left

	// Clean simulation
	simu.clean();
}

// Test receptor in simple scene with one source and opne receptor
TEST(test_simulation_1source_1recepteur, test_recepteur)
{

	// Create a ray tracer
	Simulation simu;

	// Setup
	setup_1source_1recepteur(&simu);

	// Throw rays
	simu.launchSimulation();

	std::vector<Recepteur>& recepteurs=simu.getRecepteurs();

	// Test number of receptors
	EXPECT_EQ(1,recepteurs.size());

	Recepteur recepteur=recepteurs.at(0);

	//Test name, id and pos
	EXPECT_EQ("rcpt1",recepteur.getName());						//Test name
	EXPECT_EQ(recepteur.getId(),0);								//Test id
	EXPECT_TRUE(recepteur.getPosition().compare(vec3(15,0,0)));	//Test position

	// Clean simulation
	simu.clean();
}

// Test valid ray in simple scene with one source and opne receptor
TEST(test_simulation_1source_1recepteur, test_valid_ray)
{

	// Create a ray tracer
	Simulation simu;

	// Setup
	setup_1source_1recepteur(&simu);

	// Throw rays
	simu.launchSimulation();

	std::vector<Source>& sources=simu.getSources();
	std::vector<Recepteur>& recepteurs=simu.getRecepteurs();

	Source source=sources.at(0);
	Recepteur recepteur=recepteurs.at(0);

	std::deque<Ray*>* valid_rays=simu.getSolver()->getValidRays();

	
	EXPECT_EQ(1,valid_rays->size());		// Test number of valid rays

	Ray* valid_ray=valid_rays->at(0);
	Source* ray_source=valid_ray->getSource();
	Recepteur* ray_recepteur=(static_cast<Recepteur*>(valid_ray->getRecepteur()));

	// Test original source versus valid_ray source 
	EXPECT_EQ(source.getName(),ray_source->getName());							//Test name
	EXPECT_TRUE(source.getPosition().compare(ray_source->getPosition()));		//Test position
	EXPECT_EQ(source.getId(),ray_source->getId());								//Test id

	// Test original receptor versus valid_ray receptor
	EXPECT_EQ(recepteur.getName(),ray_recepteur->getName());					//Test name
	EXPECT_TRUE(recepteur.getPosition().compare(ray_recepteur->getPosition()));	//Test position
	//EXPECT_EQ(recepteur.getId(),ray_recepteur->getId());						//ray_recepteur id is invalid

	// Test ray 
	EXPECT_EQ(0,valid_ray->getNbEvents());	//Test number of events
	EXPECT_EQ(0,valid_ray->getDiff());		//Test number of diffractions
	EXPECT_EQ(0,valid_ray->getReflex());	//Test number of reflexions
	EXPECT_EQ(10,valid_ray->getLongueur());	//Test length

	// Clean simulation
	simu.clean();

}

// Test with one obstacle between the source and the receptor
TEST(test_simulation_1source_1recepteur, test_obstacle)
{
	// Create a ray tracer
	Simulation simu;

	//Create dummy material
	Material* mat=new Material();

	// Add an obstacle between the source and the receptor
	unsigned int p1,p2,p3;
	simu.getScene()->addVertex(vec3(10,0,5), p1);
	simu.getScene()->addVertex(vec3(10,5,-5), p2);
	simu.getScene()->addVertex(vec3(10,-5,-5), p3);
	simu.getScene()->addTriangle(p1,p2,p3,mat);

	// Setup
	setup_1source_1recepteur(&simu);
	
	// Throw rays
	simu.launchSimulation();

	std::deque<Ray*>* valid_rays=simu.getSolver()->getValidRays();

	
	EXPECT_EQ(0,valid_rays->size());	// Test number of valid rays

	// Clean simulation
	simu.clean();
}


// Test the reflexion of rays with a source traped between two triangles and a maximum of 4 reflexions
TEST(test_simulation_1source_1recepteur, test_reflexion1)
{
	// The configuration of the ray tracer:
	tympan::LPSolverConfiguration config =tympan::SolverConfiguration::get();
	config->MaxReflexion=4;

	// Create a ray tracer
	Simulation simu;

	//Create dummy material
	Material* mat=new Material();

	// Add triangle obstacles 5 units to right and 5 units to the left of the source (wich is at x=5)
	unsigned int p1,p2,p3;
	simu.getScene()->addVertex(vec3(10,0,5), p1);
	simu.getScene()->addVertex(vec3(10,5,0), p2);
	simu.getScene()->addVertex(vec3(10,-5,0), p3);
	Material m;
	simu.getScene()->addTriangle(p1,p2,p3,&m);

	unsigned int p4,p5,p6;
	simu.getScene()->addVertex(vec3(0,0,5), p4);
	simu.getScene()->addVertex(vec3(0,5,0), p6);
	simu.getScene()->addVertex(vec3(0,-5,0), p5);
	simu.getScene()->addTriangle(p4,p5,p6,mat);

	// Setup
	setup_1source_1recepteur(&simu);
	
	// Throw rays
	simu.launchSimulation();

	std::deque<Ray*>* valid_rays=simu.getSolver()->getValidRays();
	std::deque<Ray*>* debug_rays=simu.getSolver()->getDebugRays();

	EXPECT_EQ(0,valid_rays->size());	 // Test number of valid rays
	EXPECT_EQ(1,debug_rays->size());	 // Test number of debug rays

	Ray* ray=debug_rays->at(0);
	ray->computeLongueur();

	std::vector<Source> sources=simu.getSources();
	std::vector<Recepteur> recepteurs=simu.getRecepteurs();

	EXPECT_EQ(1,sources.size());		 //Test number of sources
	EXPECT_EQ(1,recepteurs.size());		 //Test number of receptors

	Source source=sources.at(0);
	Recepteur recepteur=recepteurs.at(0);

	//Possible directions of the ray
	vec3 dir_right=vec3(1,0,0);
	vec3 dir_left=vec3(-1,0,0);

	// Test ray 
	EXPECT_EQ(0,ray->getDiff());									//Test number of diffractions
	EXPECT_EQ(config->MaxReflexion,ray->getReflex());				//Test number of reflexions
	EXPECT_TRUE(ray->getDirection().compare(dir_right));					//Test ray final direction									
	EXPECT_EQ(35,ray->getLongueur());								//Test ray length

	std::vector<std::shared_ptr<Event> >* events=ray->getEvents();

	//Test number of events
	EXPECT_EQ(config->MaxReflexion,ray->getNbEvents());						
	EXPECT_EQ(config->MaxReflexion,events->size());		 

	//Test events
	std::shared_ptr<Event> e=events->at(0);
	EXPECT_EQ(SPECULARREFLEXION,e.get()->getType());					//Test type
	EXPECT_TRUE(vec3(10,0,0).compare(e.get()->getPosition()));			//Test position
	EXPECT_TRUE(e.get()->getIncomingDirection().compare(dir_right));	//Test incoming direction

	e=events->at(1);
	EXPECT_EQ(SPECULARREFLEXION,e.get()->getType());					//Test type
	EXPECT_TRUE(vec3(0,0,0).compare(e.get()->getPosition()));			//Test position
	EXPECT_TRUE(e.get()->getIncomingDirection().compare(dir_left));		//Test incoming direction
	
	e=events->at(2);
	EXPECT_EQ(SPECULARREFLEXION,e.get()->getType());					//Test type
	EXPECT_TRUE(vec3(10,0,0).compare(e.get()->getPosition()));			//Test position
	EXPECT_TRUE(e.get()->getIncomingDirection().compare(dir_right));	//Test incoming direction

	e=events->at(3);
	EXPECT_EQ(SPECULARREFLEXION,e.get()->getType());					//Test type
	EXPECT_TRUE(vec3(0,0,0).compare(e.get()->getPosition()));			//Test position
	EXPECT_TRUE(e.get()->getIncomingDirection().compare(dir_left));		//Test incoming direction
	
	
	// Clean simulation
	simu.clean();
}

// Test the reflexion of a ray reflected multiple times before hiting the receptor
TEST(test_simulation_1source_1recepteur, test_reflexion2)
{
	// The configuration of the ray tracer:
	tympan::LPSolverConfiguration config =tympan::SolverConfiguration::get();
	
	// Create a ray tracer
	Simulation simu;

	//Create dummy material
	Material* mat=new Material();

	// Add 3 triangles, one reflecting the ray upward, one reflecting rightward and one downward to the receptor
	unsigned int p1,p2,p3;
	simu.getScene()->addVertex(vec3(-1,-1,0), p1);
	simu.getScene()->addVertex(vec3(1,1,1), p2);
	simu.getScene()->addVertex(vec3(1,1,-1), p3);
	simu.getScene()->addTriangle(p1,p2,p3,mat);

	unsigned int p4,p5,p6;
	simu.getScene()->addVertex(vec3(-1,9,0), p4);
	simu.getScene()->addVertex(vec3(1,11,-1), p5);
	simu.getScene()->addVertex(vec3(1,11,1), p6);
	simu.getScene()->addTriangle(p4,p5,p6,mat);

	unsigned int p7,p8,p9;
	simu.getScene()->addVertex(vec3(9,11,0), p7);
	simu.getScene()->addVertex(vec3(11,9,-1), p8);
	simu.getScene()->addVertex(vec3(11,9,1), p9);
	simu.getScene()->addTriangle(p7,p8,p9,mat);

	// Setup
	setup_1source_1recepteur(&simu,0,2,vec3(-10,0,0),vec3(10,0,0));
	
	// Throw rays
	simu.launchSimulation();

	std::deque<Ray*>* valid_rays=simu.getSolver()->getValidRays();
	std::deque<Ray*>* debug_rays=simu.getSolver()->getDebugRays();

	EXPECT_EQ(1,valid_rays->size());	 // Test number of valid rays
	EXPECT_EQ(1,debug_rays->size());	 // Test number of debug rays

	Ray* ray=valid_rays->at(0);
	ray->computeLongueur();

	std::vector<Source> sources=simu.getSources();
	std::vector<Recepteur> recepteurs=simu.getRecepteurs();

	EXPECT_EQ(1,sources.size());		 //Test number of sources
	EXPECT_EQ(1,recepteurs.size());		 //Test number of receptors

	Source source=sources.at(0);
	Recepteur recepteur=recepteurs.at(0);

	//Possible directions of the ray
	vec3 dir_right=vec3(1,0,0);
	vec3 dir_top=vec3(0,1,0);
	vec3 dir_down=vec3(0,-1,0);

	// Test ray 
	EXPECT_EQ(0,ray->getDiff());					//Test number of diffractions
	EXPECT_EQ(3,ray->getReflex());					//Test number of reflexions
	EXPECT_TRUE(ray->getDirection().compare(dir_down));	//Test ray final direction									
	EXPECT_EQ(40,ray->getLongueur());				//Test ray length

	std::vector<std::shared_ptr<Event> >* events=ray->getEvents();

	//Test number of events
	EXPECT_EQ(3,ray->getNbEvents());						
	EXPECT_EQ(3,events->size());		 

	//Test events
	std::shared_ptr<Event> e=events->at(0);
	EXPECT_EQ(SPECULARREFLEXION,e.get()->getType());					//Test type
	EXPECT_TRUE(vec3(0,0,0).compare(e.get()->getPosition()));			//Test position
	EXPECT_TRUE(e.get()->getIncomingDirection().compare(dir_right));	//Test incoming direction

	e=events->at(1);
	EXPECT_EQ(SPECULARREFLEXION,e.get()->getType());					//Test type
	EXPECT_TRUE(vec3(0,10,0).compare(e.get()->getPosition()));			//Test position
	EXPECT_TRUE(e.get()->getIncomingDirection().compare(dir_top));		//Test incoming direction

	e=events->at(2);
	EXPECT_EQ(SPECULARREFLEXION,e.get()->getType());								//Test type
	//EXPECT_TRUE(vec3(10,10,0).compare(e.value->getPosition()));					//Test position  - Does not pass because of an accumulation of small errors in the directions of reflected rays
	EXPECT_TRUE(vec3(10,10,0).compare(e.get()->getPosition(),(decimal).00001));		//Test distance with expected position
	EXPECT_TRUE(e.get()->getIncomingDirection().compare(dir_right));				//Test incoming direction
	
	// Clean simulation
	simu.clean();
}
/*
// Test the diffraction of a ray reflected multiple times before hiting the receptor
TEST(test_simulation_1source_1recepteur, test_diffraction)
{
	// The configuration of the ray tracer:
	tympan::LPSolverConfiguration config =tympan::SolverConfiguration::get();
	//config->NbRayWithDiffraction=500;
	config->UsePostFilters=false;
	// Create a ray tracer
	Simulation simu;

	// Add 3 triangles, one reflecting the ray upward, one reflecting rightward and one downward to the receptor
	unsigned int p1,p2,p3,p4,p5,p6,p7,p8;
	
	simu.getScene()->addVertex(vec3(0,0,0), p1);
	simu.getScene()->addVertex(vec3(50,0,0), p2);
	simu.getScene()->addVertex(vec3(0,0,50), p3);
	simu.getScene()->addVertex(vec3(50,0,50), p4);

	simu.getScene()->addVertex(vec3(0,50,0), p5);
	simu.getScene()->addVertex(vec3(50,50,0), p6);
	simu.getScene()->addVertex(vec3(0,50,50), p7);
	simu.getScene()->addVertex(vec3(50,50,50), p8);

	simu.getScene()->addTriangle(p2,p1,p5,&Material());
	simu.getScene()->addTriangle(p2,p5,p6,&Material());
	simu.getScene()->addTriangle(p4,p6,p8,&Material());
	simu.getScene()->addTriangle(p4,p2,p6,&Material());
	simu.getScene()->addTriangle(p3,p4,p8,&Material());
	simu.getScene()->addTriangle(p3,p8,p7,&Material());
	simu.getScene()->addTriangle(p1,p7,p5,&Material());
	simu.getScene()->addTriangle(p1,p3,p7,&Material());
	simu.getScene()->addTriangle(p7,p8,p5,&Material());
	simu.getScene()->addTriangle(p5,p8,p6,&Material());
	
	// Setup
	setup_1source_1recepteur(&simu,0,2,vec3(-25,50.1,25),vec3(75,50.1,25));
	
	// Throw rays
	simu.launchSimulation();

	std::deque<Ray*>* valid_rays=simu.getSolver()->getValidRays();
	std::deque<Ray*>* debug_rays=simu.getSolver()->getDebugRays();

	EXPECT_EQ(1,valid_rays->size());	 // Test number of valid rays
	EXPECT_EQ(1,debug_rays->size());	 // Test number of debug rays

	Ray* ray=debug_rays->at(0);
	ray->computeLongueur();

	std::vector<Source> sources=simu.getSources();
	std::vector<Recepteur> recepteurs=simu.getRecepteurs();

	EXPECT_EQ(1,sources.size());		 //Test number of sources
	EXPECT_EQ(1,recepteurs.size());		 //Test number of receptors

	Source source=sources.at(0);
	Recepteur recepteur=recepteurs.at(0);

	int rays[]={0,0,0};


	ofstream out;
	out.open ("C:/Users/FD3C591N/Desktop/output.txt");
	

	std::deque<Ray*>* output_rays=debug_rays;
	for(int i=0;i<output_rays->size();i++){
		
		out<<output_rays->at(i)->position[0]<<","<<output_rays->at(i)->position[1]<<","<<output_rays->at(i)->position[2]<<","<<output_rays->at(i)->direction[0]<<","<<output_rays->at(i)->direction[1]<<","<<output_rays->at(i)->direction[2]<<endl;
		if(output_rays->at(i)->getDiff()>1){
			rays[2]++;
		}else if(output_rays->at(i)->getDiff()>0){
			rays[1]++;
		}else rays[0]++;

	}
	out.close();
	cout<<"no diffract: "<<rays[0]<<endl;
	cout<<"1 diffract: "<<rays[1]<<endl;
	cout<<"2 diffract: "<<rays[2]<<endl;
	//Possible directions of the ray
	vec3 dir_right=vec3(1,0,0);
	vec3 dir_top=vec3(0,1,0);
	vec3 dir_down=vec3(0,-1,0);

	// Test ray 
	EXPECT_EQ(0,ray->getDiff());					//Test number of diffractions
	EXPECT_EQ(0,ray->getReflex());					//Test number of reflexions
	EXPECT_TRUE(ray->direction==dir_down);			//Test ray final direction									
	EXPECT_EQ(40,ray->getLongueur());				//Test ray length

	std::vector<std::shared_ptr<Event> >* events=ray->getEvents();

	//Test number of events
	EXPECT_EQ(3,ray->getNbEvents());						
	EXPECT_EQ(3,events->size());		 

	//Test events
	std::shared_ptr<Event> e=events->at(0);
	EXPECT_EQ(SPECULARREFLEXION,e.value->getType());			//Test type
	EXPECT_TRUE(vec3(0,0,0)==e.value->getPosition());			//Test position
	EXPECT_TRUE(e.value->getIncomingDirection()==dir_right);	//Test incoming direction

	e=events->at(1);
	EXPECT_EQ(SPECULARREFLEXION,e.value->getType());			//Test type
	EXPECT_TRUE(vec3(0,10,0)==e.value->getPosition());			//Test position
	EXPECT_TRUE(e.value->getIncomingDirection()==dir_top);		//Test incoming direction

	e=events->at(2);
	EXPECT_EQ(SPECULARREFLEXION,e.value->getType());						//Test type
	//EXPECT_TRUE(vec3(10,10,0)==e.value->getPosition());					//Test position  - Does not pass because of an accumulation of small errors in the directions of reflected rays
	EXPECT_TRUE(e.value->getPosition().distance(vec3(10,10,0))<0.00001);	//Test distance with expected position
	EXPECT_TRUE(e.value->getIncomingDirection()==dir_right);				//Test incoming direction
	
	// Clean simulation
	simu.clean();
}
*/
// Test simulation with 0 source and 0 receptor
TEST(test_simulation_0source_0receptor, test_0source_0receptor)
{

	// Create a ray tracer
	Simulation simu;

	std::vector<Source> sources;
	std::vector<Recepteur> recepteurs;

	std::vector<vec3> src_pos;
	std::vector<vec3> rcpt_pos;

	// Random setup
	setup_random(&simu,sources,recepteurs,src_pos,rcpt_pos,0,0,0);

	// Throw rays
	simu.launchSimulation();

	std::vector<Source> sources_after_simulation=simu.getSources();
	std::vector<Recepteur> recepteurs_after_simulation=simu.getRecepteurs();

	// Check that there is no source or receptor
	EXPECT_EQ(0,sources_after_simulation.size());
	EXPECT_EQ(0,recepteurs_after_simulation.size());

	// get valid and debug rays
	std::deque<Ray*>* valid_rays=simu.getSolver()->getValidRays();
	std::deque<Ray*>* debug_rays=simu.getSolver()->getDebugRays();

	// Check that no ray was thrown
	EXPECT_EQ(0,valid_rays->size());
	EXPECT_EQ(0,debug_rays->size());

	// Clean simulation
	simu.clean();
}

// Test simulation with 0 source and 1 receptor
TEST(test_simulation_0source_1receptor, test_0source_1receptor)
{

	// Create a ray tracer
	Simulation simu;

	std::vector<Source> sources;
	std::vector<Recepteur> recepteurs;

	std::vector<vec3> src_pos;
	std::vector<vec3> rcpt_pos;

	// Random setup
	setup_random(&simu,sources,recepteurs,src_pos,rcpt_pos,0,0,1);

	// Throw rays
	simu.launchSimulation();

	std::vector<Source> sources_after_simulation=simu.getSources();
	std::vector<Recepteur> recepteurs_after_simulation=simu.getRecepteurs();

	// Check that there is no source or receptor
	EXPECT_EQ(0,sources_after_simulation.size());
	EXPECT_EQ(1,recepteurs_after_simulation.size());

	// get valid and debug rays
	std::deque<Ray*>* valid_rays=simu.getSolver()->getValidRays();
	std::deque<Ray*>* debug_rays=simu.getSolver()->getDebugRays();

	// Check that no ray was thrown
	EXPECT_EQ(0,valid_rays->size());
	EXPECT_EQ(0,debug_rays->size());

	// Clean simulation
	simu.clean();
}

// Test simulation with 1 source and 0 receptor
TEST(test_simulation_1source_0receptor, test_1source_0receptor)
{

	// Create a ray tracer
	Simulation simu;

	std::vector<Source> sources;
	std::vector<Recepteur> recepteurs;

	std::vector<vec3> src_pos;
	std::vector<vec3> rcpt_pos;

	// Random setup
	setup_random(&simu,sources,recepteurs,src_pos,rcpt_pos,0,1,0);

	// Throw rays
	simu.launchSimulation();

	std::vector<Source> sources_after_simulation=simu.getSources();
	std::vector<Recepteur> recepteurs_after_simulation=simu.getRecepteurs();

	int nbRaysPerSource=tympan::SolverConfiguration::get()->NbRaysPerSource;
	
	// Check that there is no source or receptor
	EXPECT_EQ(1,sources_after_simulation.size());
	EXPECT_EQ(0,recepteurs_after_simulation.size());

	// get valid and debug rays
	std::deque<Ray*>* valid_rays=simu.getSolver()->getValidRays();
	std::deque<Ray*>* debug_rays=simu.getSolver()->getDebugRays();

	// Check that no ray was thrown
	EXPECT_EQ(0,valid_rays->size());
	EXPECT_EQ(nbRaysPerSource,debug_rays->size());

	// Clean simulation
	simu.clean();
}

// Test sources in randomly generated scene
TEST(test_simulation_random_init, test_sources)
{

	// Create a ray tracer
	Simulation simu;

	std::vector<Source> sources;
	std::vector<Recepteur> recepteurs;

	std::vector<vec3> src_pos;
	std::vector<vec3> rcpt_pos;

	// Random setup
	setup_random(&simu,sources,recepteurs,src_pos,rcpt_pos);

	// Throw rays
	simu.launchSimulation();

	std::vector<Source> sources_after_simulation=simu.getSources();

	int nbRaysPerSource=tympan::SolverConfiguration::get()->NbRaysPerSource;

	EXPECT_EQ(sources.size(),sources_after_simulation.size()); // Test number of sources

	for(unsigned int i=0;i<sources.size();i++){
		Source src_original=sources.at(i);
		Source src_after_simulation=sources_after_simulation.at(i);

		//  Test source
		EXPECT_EQ(src_original.getName(),src_after_simulation.getName());					// Test name
		EXPECT_EQ(src_original.getId(),src_after_simulation.getId());						// Test id
		EXPECT_TRUE(src_original.getPosition().compare(src_after_simulation.getPosition()));	// Test position
		EXPECT_EQ(nbRaysPerSource,src_after_simulation.getInitialRayCount());				// Test number of rays per source
		EXPECT_EQ(0,src_after_simulation.getNbRayLeft());									// Test number of rays left

	}

	// Clean simulation
	simu.clean();
}

// Test receptors with in randomly generated scene
TEST(test_simulation_random_init, test_recepteurs)
{

	// Create a ray tracer
	Simulation simu;

	std::vector<Source> sources;
	std::vector<Recepteur> recepteurs;

	std::vector<vec3> src_pos;
	std::vector<vec3> rcpt_pos;

	// Random setup
	setup_random(&simu,sources,recepteurs,src_pos,rcpt_pos);

	// Throw rays
	simu.launchSimulation();

	std::vector<Recepteur> recepteurs_after_simulation=simu.getRecepteurs();

	EXPECT_EQ(recepteurs.size(),recepteurs_after_simulation.size());	// Test number of receptors

	for(unsigned int i=0;i<recepteurs.size();i++){
		Recepteur rcpt_original=recepteurs.at(i);
		Recepteur rcpt_after_simulation=recepteurs_after_simulation.at(i);

		//  Test recepteur
		EXPECT_EQ(rcpt_original.getName(),rcpt_after_simulation.getName());						// Test name
		EXPECT_EQ(rcpt_original.getId(),rcpt_after_simulation.getId());							// Test id
		EXPECT_TRUE(rcpt_original.getPosition().compare(rcpt_after_simulation.getPosition()));	// Test position

	}
	
	// Clean simulation
	simu.clean();
}


// Test number of rays per source equals zero
TEST(test_simulation_random_init, test_valid_rays_nbRaysPerSource_equals_0)
{

	// Create a ray tracer
	Simulation simu;

	std::vector<Source> sources;
	std::vector<Recepteur> recepteurs;

	std::vector<vec3> src_pos;
	std::vector<vec3> rcpt_pos;

	// Random setup
	setup_random(&simu,sources,recepteurs,src_pos,rcpt_pos,0);

	// Throw rays
	simu.launchSimulation();

	std::deque<Ray*>* valid_rays=simu.getSolver()->getValidRays();
	std::deque<Ray*>* debug_rays=simu.getSolver()->getDebugRays();
	
	int nbRaysPerSource=tympan::SolverConfiguration::get()->NbRaysPerSource;

	
	EXPECT_EQ(sources.size()*recepteurs.size(),valid_rays->size());						// Test number of valid rays (1 ray per source for each receptor) 
	EXPECT_EQ(sources.size()*(nbRaysPerSource+recepteurs.size()),debug_rays->size());	// Test number of debug rays 

	for(unsigned int i =0;i<valid_rays->size();i++){
		Ray* valid_ray=valid_rays->at(i);

		Source* ray_source=valid_ray->getSource();
		Recepteur* ray_recepteur=(static_cast<Recepteur*>(valid_ray->getRecepteur()));
		
		vec3 src_pos=sources.at(ray_source->getId()).getPosition();
		vec3 rcpt_pos=recepteurs.at(ray_recepteur->getId()).getPosition();

		// Test original source versus valid_ray source 
		EXPECT_EQ(sources.at(ray_source->getId()).getName(),ray_source->getName());		// Test name
		EXPECT_TRUE(src_pos.compare(ray_source->getPosition()));						// Test position

		// Test original receptor versus valid_ray receptor
		EXPECT_EQ(recepteurs.at(ray_recepteur->getId()).getName(),ray_recepteur->getName());	// Test name
		EXPECT_TRUE(rcpt_pos.compare(ray_recepteur->getPosition()));							// Test position

		// Test ray 
		EXPECT_EQ(0,valid_ray->getNbEvents());								// Test number of events
		EXPECT_EQ(0,valid_ray->getDiff());									// Test number of diffractions
		EXPECT_EQ(0,valid_ray->getReflex());								// Test number of reflexions
		EXPECT_EQ(src_pos.distance(rcpt_pos),valid_ray->getLongueur());		// Test length
	}

	// Clean simulation
	simu.clean();

}


// Test valid rays in randomly generated scene
TEST(test_simulation_random_init, test_valid_rays_nbRaysPerSource_random)
{

	// Create a ray tracer
	Simulation simu;

	std::vector<Source> sources;
	std::vector<Recepteur> recepteurs;

	std::vector<vec3> src_pos;
	std::vector<vec3> rcpt_pos;

	// Random setup
	setup_random(&simu,sources,recepteurs,src_pos,rcpt_pos);

	// Throw rays
	simu.launchSimulation();

	std::deque<Ray*>* valid_rays=simu.getSolver()->getValidRays();
	std::deque<Ray*>* debug_rays=simu.getSolver()->getDebugRays();
	int nbRaysPerSource=tympan::SolverConfiguration::get()->NbRaysPerSource;
	
	// Test number of valid rays
	EXPECT_EQ(sources.size()*recepteurs.size(),valid_rays->size());
	EXPECT_EQ(sources.size()*(nbRaysPerSource+recepteurs.size()),debug_rays->size());

	for(unsigned int i =0;i<valid_rays->size();i++){
		Ray* valid_ray=valid_rays->at(i);

		Source* ray_source=valid_ray->getSource();
		Recepteur* ray_recepteur=(static_cast<Recepteur*>(valid_ray->getRecepteur()));
		
		vec3 src_pos=sources.at(ray_source->getId()).getPosition();
		vec3 rcpt_pos=recepteurs.at(ray_recepteur->getId()).getPosition();

		// Test original source versus valid_ray source 
		EXPECT_EQ(sources.at(ray_source->getId()).getName(),ray_source->getName());		// Test name
		EXPECT_TRUE(src_pos.compare(ray_source->getPosition()));						// Test position

		// Test original receptor versus valid_ray receptor
		EXPECT_EQ(recepteurs.at(ray_recepteur->getId()).getName(),ray_recepteur->getName());	// Test name
		EXPECT_TRUE(rcpt_pos.compare(ray_recepteur->getPosition()));							// Test position

		// Test ray 
		EXPECT_EQ(0,valid_ray->getNbEvents());								// Test number of events
		EXPECT_EQ(0,valid_ray->getDiff());									// Test number of diffractions
		EXPECT_EQ(0,valid_ray->getReflex());								// Test number of reflexions
		EXPECT_EQ(src_pos.distance(rcpt_pos),valid_ray->getLongueur());		// Test length
	}

	// Clean simulation
	simu.clean();

}

// Test debug rays in randomly generated scene
TEST(test_simulation_random_init, test_debug_rays_nbRaysPerSource_random)
{

	// Create a ray tracer
	Simulation simu;

	std::vector<Source> sources;
	std::vector<Recepteur> recepteurs;

	std::vector<vec3> src_pos;
	std::vector<vec3> rcpt_pos;

	// Random setup
	setup_random(&simu,sources,recepteurs,src_pos,rcpt_pos);

	// Throw rays
	simu.launchSimulation();

	std::deque<Ray*>* valid_rays=simu.getSolver()->getValidRays();
	std::deque<Ray*>* debug_rays=simu.getSolver()->getDebugRays();

	int nbRaysPerSource=tympan::SolverConfiguration::get()->NbRaysPerSource;

	// Test number of valid rays
	EXPECT_EQ(sources.size()*recepteurs.size(),valid_rays->size());
	EXPECT_EQ(sources.size()*(nbRaysPerSource+recepteurs.size()),debug_rays->size());

	std::vector<int> raysThrownFromSource(sources.size(),0);

	// Test each ray and count how many rays are thrown from each source
	for(unsigned int i=0;i<debug_rays->size();i++){
		Ray* debug_ray=debug_rays->at(i);
		Source* ray_source=debug_ray->getSource();
		raysThrownFromSource.at(ray_source->getId())++;

		vec3 src_pos=sources.at(ray_source->getId()).getPosition();

		// Test original source versus valid_ray source 
		EXPECT_EQ(sources.at(ray_source->getId()).getName(),ray_source->getName());		// Test name
		EXPECT_TRUE(src_pos.compare(ray_source->getPosition()));						// Test position

		// Test ray 
		EXPECT_EQ(0,debug_ray->getNbEvents());									// Test number of events
		EXPECT_EQ(0,debug_ray->getDiff());										// Test number of diffractions
		EXPECT_EQ(0,debug_ray->getReflex());									// Test number of reflexions
	}

	// Test if the number of rays thrown from each source equals the number of rays per source + the number of receptors
	for(unsigned int i=0;i<sources.size();i++){
		EXPECT_EQ(nbRaysPerSource+recepteurs.size(),raysThrownFromSource.at(i));
	}

	// Clean simulation
	simu.clean();

}

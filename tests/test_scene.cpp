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
#include "Tympan/solvers/ANIME3DSolver/TYANIME3DRayTracerSolverAdapter.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/PostTreatment.h"
#include "Tympan/models/solver/config.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Triangle.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Cylindre.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;


// Test the addition of a single point to a scene
TEST(test_scene, test_single_point)
{

	Simulation simu;

	unsigned int p1;
	simu.getScene()->addVertex(vec3(10,0,5), p1); //Add vertex to scene


	std::vector<vec3>* vertices=simu.getScene()->getVertices(); //Recover vertices from scene

	EXPECT_EQ(1,vertices->size());	//Test number of vertices
	EXPECT_EQ(0,p1);				//Test vertex id	

	vec3 vertex=vertices->at(0);

	EXPECT_TRUE(vec3(10,0,5)==vertex); //Test vertex coordinates
	
}


// Test the addition of a random number of points to a scene
TEST(test_scene, test_random_points)
{

	// Create a ray tracer
	Simulation simu;


	unsigned int seed=time(NULL); //Generate seed from time
	srand(seed);				  //Init random number generator with seed
	cout<<"Random number generator initialized with seed "<<seed<<endl;

	const int nb_vertices=rand()%100; //Random nuymber of points

	std::vector<unsigned int> vertex_ids(nb_vertices,0); //vector of vertex ids
	std::vector<vec3> vertices_init;					 //vector recording the vertices added to the scene
	
	for(int i=0;i<vertex_ids.size();i++){

		//Random coordinates
		int x=rand()%1000;
		int y=rand()%1000;
		int z=rand()%1000;

		vec3 vertex=vec3(x,y,z);

		vertices_init.push_back(vertex);						//Add vertex record the newly generated vertex
		simu.getScene()->addVertex(vertex, vertex_ids.at(i));	//Add vertex to the scene
	}


	std::vector<vec3>* vertices=simu.getScene()->getVertices();
	
	EXPECT_EQ(nb_vertices,vertices->size());  //Test number of vertices ins cene
	

	for(int i=0;i<vertices->size();i++){

		unsigned int id=vertex_ids.at(i);	
		EXPECT_EQ(i,id);										//Test vertex id
		EXPECT_TRUE(vertices->at(id)==vertices_init.at(i));		//Test if the vertices in the scene correspond to the recorded ones 
	}
}


// Test the addition of triangles to a scene and that their BBox and normal are correct
TEST(test_scene, test_triangles)
{

	Simulation simu;

	unsigned int p1,p2,p3,p4,p5,p6;
	simu.getScene()->addVertex(vec3(5,0,5), p1);
	simu.getScene()->addVertex(vec3(5,5,-5), p2);
	simu.getScene()->addVertex(vec3(5,-5,-5), p3);

	simu.getScene()->addVertex(vec3(0,0,0), p4);
	simu.getScene()->addVertex(vec3(-3,3,-3), p5);
	simu.getScene()->addVertex(vec3(-6,-3,-3), p6);

	simu.getScene()->addTriangle(p1,p2,p3,&Material()); 
	simu.getScene()->addTriangle(p1,p3,p2,&Material()); 

	simu.getScene()->addTriangle(p4,p5,p6,&Material()); 
	simu.getScene()->addTriangle(p4,p6,p5,&Material()); 

	std::vector<vec3>* vertices=simu.getScene()->getVertices(); //Recover vertices from scene
	std::vector<Shape*>* shapes=simu.getScene()->getShapes();   //Recover shapes from scene

	EXPECT_EQ(6,vertices->size());	//Test number of vertices
	EXPECT_EQ(4,shapes->size());	//Test number of shapes
	

	//Test the class of each shape
	for(uint i=0;i<shapes->size();i++){
		Triangle* shape=dynamic_cast<Triangle*>(shapes->at(i));
		EXPECT_TRUE(shape);
	}



	//Test normals
	vec3 n1(-1,0,0);			
	vec3 n2=n1*-1;					
	vec3 n3(-18,9,27);		
	n3.normalize();
	vec3 n4=n3*-1;

	EXPECT_TRUE(shapes->at(0)->getNormal()==n1); 
	EXPECT_TRUE(shapes->at(1)->getNormal()==n2); 
	EXPECT_TRUE(shapes->at(2)->getNormal()==n3); 
	EXPECT_TRUE(shapes->at(3)->getNormal()==n4); 



	//Test BBoxes
	BBox bbox1=shapes->at(0)->getBBox();
	BBox bbox2=shapes->at(1)->getBBox();
	BBox bbox3=shapes->at(2)->getBBox();
	BBox bbox4=shapes->at(3)->getBBox();

	vec3 one_hundredth(0.01,0.01,0.01);

	EXPECT_TRUE(bbox1.getBBMin()==(vec3(5,-5,-5)-one_hundredth)); 
	EXPECT_TRUE(bbox1.getBBMax()==(vec3(5,5,5)+one_hundredth)); 
	EXPECT_TRUE(bbox1.getCentroid()==vec3(5,0,0)); 
	
	EXPECT_TRUE(bbox2.getBBMin()==(vec3(5,-5,-5)-one_hundredth));
	EXPECT_TRUE(bbox2.getBBMax()==(vec3(5,5,5)+one_hundredth)); 
	EXPECT_TRUE(bbox2.getCentroid()==vec3(5,0,0)); 

	EXPECT_TRUE(bbox3.getBBMin()==(vec3(-6,-3,-3)-one_hundredth)); 
	EXPECT_TRUE(bbox3.getBBMax()==(vec3(0,3,0)+one_hundredth)); 
	EXPECT_TRUE(bbox3.getCentroid()==vec3(-3,0,-1.5)); 

	EXPECT_TRUE(bbox4.getBBMin()==(vec3(-6,-3,-3)-one_hundredth));
	EXPECT_TRUE(bbox4.getBBMax()==(vec3(0,3,0)+one_hundredth)); 
	EXPECT_TRUE(bbox4.getCentroid()==vec3(-3,0,-1.5)); 



	//Test scene boundaries
	BBox global_bbox=simu.getScene()->getGlobalBox();

	EXPECT_TRUE(global_bbox.getBBMin()==(vec3(-6,-5,-5)-one_hundredth)); 
	EXPECT_TRUE(global_bbox.getBBMax()==(vec3(5,5,5)+one_hundredth)); 
	EXPECT_TRUE(global_bbox.getCentroid()==vec3(-0.5,0,0)); 
	

}


// Test the addition a building to a scene
TEST(test_scene, test_building)
{

	Simulation simu;

	std::vector<unsigned int> vertex_ids(4,0);

	vec3 origin(7,-15,3);
	vec3 dimensions(12,5,8);
	simu.getScene()->addBuilding(origin,dimensions,&Material());

	std::vector<vec3>* vertices=simu.getScene()->getVertices(); //Recover vertices from scene
	std::vector<Shape*>* shapes=simu.getScene()->getShapes();   //Recover shapes from scene

	EXPECT_EQ(8,vertices->size());	//Test number of vertices
	EXPECT_EQ(10,shapes->size());	//Test number of shapes


	//Test that all expected points are present and have correct coordinates
	auto it=find(vertices->begin(),vertices->end(),origin);
	EXPECT_NE(vertices->end(),it);

	it=find(vertices->begin(),vertices->end(),origin+vec3(12,0,0));
	EXPECT_NE(vertices->end(),it);

	it=find(vertices->begin(),vertices->end(),origin+vec3(0,5,0));
	EXPECT_NE(vertices->end(),it);

	it=find(vertices->begin(),vertices->end(),origin+vec3(0,0,8));
	EXPECT_NE(vertices->end(),it);

	it=find(vertices->begin(),vertices->end(),origin+vec3(12,5,0));
	EXPECT_NE(vertices->end(),it);
	
	it=find(vertices->begin(),vertices->end(),origin+vec3(12,0,8));
	EXPECT_NE(vertices->end(),it);

	it=find(vertices->begin(),vertices->end(),origin+vec3(0,5,8));
	EXPECT_NE(vertices->end(),it);
	
	it=find(vertices->begin(),vertices->end(),origin+dimensions);
	EXPECT_NE(vertices->end(),it);

	//Test the class of each shape
	for(uint i=0;i<shapes->size();i++){
		Triangle* shape=dynamic_cast<Triangle*>(shapes->at(i));
		EXPECT_TRUE(shape);
	}

	//Test scene boundaries
	BBox bbox=simu.getScene()->getGlobalBox();
	vec3 one_hundredth(0.01,0.01,0.01);

	EXPECT_TRUE(bbox.getBBMin()==(origin-one_hundredth)); 
	EXPECT_TRUE(bbox.getBBMax()==(origin+dimensions+one_hundredth)); 
	EXPECT_TRUE(bbox.getCentroid()==origin+dimensions/2); 
}

// Test the presence of a diffraction edge when the angle between two faces is low enough
TEST(test_scene, test_diffraction_edge1)
{

	Simulation simu;

	unsigned int p1,p2,p3,p4;
	double angle=175;
	simu.getScene()->addVertex(vec3(0,0,0), p1);
	simu.getScene()->addVertex(vec3(0,1,0), p2);
	simu.getScene()->addVertex(vec3(1,1,-tan(angle/2*M_PI/180)), p3);
	simu.getScene()->addVertex(vec3(1,1,tan(angle/2*M_PI/180)), p4);

	simu.getScene()->addTriangle(p1,p2,p3,&Material());
	simu.getScene()->addTriangle(p1,p4,p2,&Material());

	// Create a solver
	TYANIME3DRayTracerSolverAdapter* solver=new TYANIME3DRayTracerSolverAdapter();

	// Add diffraction edges and selectors filtering rays  
	solver->postTreatmentScene(simu.getScene(),simu.getSources(),simu.getRecepteurs());

	std::vector<vec3>* vertices=simu.getScene()->getVertices(); //Recover vertices from scene
	std::vector<Shape*>* shapes=simu.getScene()->getShapes();   //Recover shapes from scene

	EXPECT_EQ(4,vertices->size());	//Test number of vertices
	EXPECT_EQ(3,shapes->size());	//Test number of shapes

	//Test the class of each shape
	Triangle* shape1=dynamic_cast<Triangle*>(shapes->at(0));
	Triangle* shape2=dynamic_cast<Triangle*>(shapes->at(1));
	Cylindre* shape3=dynamic_cast<Cylindre*>(shapes->at(2));

	EXPECT_TRUE(shape1);
	EXPECT_TRUE(shape2);
	EXPECT_TRUE(shape3);
	
}
// Test the presence of a diffraction edge when the angle between two faces is low enough
TEST(test_scene, test_diffraction_edge2)
{

	Simulation simu;
	 
	tympan::LPSolverConfiguration config =tympan::SolverConfiguration::get();
	config->AngleDiffMin=20;

	unsigned int p1,p2,p3,p4;
	double angle=160;
	simu.getScene()->addVertex(vec3(0,0,0), p1);
	simu.getScene()->addVertex(vec3(0,1,0), p2);
	simu.getScene()->addVertex(vec3(1,1,-tan(angle/2*M_PI/180)), p3);
	simu.getScene()->addVertex(vec3(1,1,tan(angle/2*M_PI/180)), p4);

	simu.getScene()->addTriangle(p1,p2,p3,&Material());
	simu.getScene()->addTriangle(p1,p4,p2,&Material());

	// Create a solver
	TYANIME3DRayTracerSolverAdapter* solver=new TYANIME3DRayTracerSolverAdapter();

	// Add diffraction edges and selectors filtering rays  
	solver->postTreatmentScene(simu.getScene(),simu.getSources(),simu.getRecepteurs());

	std::vector<vec3>* vertices=simu.getScene()->getVertices(); //Recover vertices from scene
	std::vector<Shape*>* shapes=simu.getScene()->getShapes();   //Recover shapes from scene

	EXPECT_EQ(4,vertices->size());	//Test number of vertices
	EXPECT_EQ(3,shapes->size());	//Test number of shapes

	//Test the class of each shape
	Triangle* shape1=dynamic_cast<Triangle*>(shapes->at(0));
	Triangle* shape2=dynamic_cast<Triangle*>(shapes->at(1));
	Cylindre* shape3=dynamic_cast<Cylindre*>(shapes->at(2));

	EXPECT_TRUE(shape1);
	EXPECT_TRUE(shape2);
	EXPECT_TRUE(shape3);
	
}


// Test the absence of diffraction edge when the angle between two faces is too high
TEST(test_scene, test_diffraction_edge3)
{

	Simulation simu;

	unsigned int p1,p2,p3,p4;
	double angle=175.1;
	simu.getScene()->addVertex(vec3(0,0,0), p1);
	simu.getScene()->addVertex(vec3(0,1,0), p2);
	simu.getScene()->addVertex(vec3(1,1,-tan(angle/2*M_PI/180)), p3);
	simu.getScene()->addVertex(vec3(1,1,tan(angle/2*M_PI/180)), p4);

	simu.getScene()->addTriangle(p1,p2,p3,&Material());
	simu.getScene()->addTriangle(p1,p4,p2,&Material());

	// Create a solver
	TYANIME3DRayTracerSolverAdapter* solver=new TYANIME3DRayTracerSolverAdapter();

	// Add diffraction edges and selectors filtering rays  
	solver->postTreatmentScene(simu.getScene(),simu.getSources(),simu.getRecepteurs());

	std::vector<vec3>* vertices=simu.getScene()->getVertices(); //Recover vertices from scene
	std::vector<Shape*>* shapes=simu.getScene()->getShapes();   //Recover shapes from scene

	EXPECT_EQ(4,vertices->size());	//Test number of vertices
	EXPECT_EQ(2,shapes->size());	//Test number of shapes

	//Test the class of each shape
	Triangle* shape1=dynamic_cast<Triangle*>(shapes->at(0));
	Triangle* shape2=dynamic_cast<Triangle*>(shapes->at(1));

	EXPECT_TRUE(shape1);
	EXPECT_TRUE(shape2);
}

// Test the absence of diffraction edge when the angle between two faces is too high
TEST(test_scene, test_diffraction_edge4)
{

	Simulation simu;
	 
	tympan::LPSolverConfiguration config =tympan::SolverConfiguration::get();
	config->AngleDiffMin=20;

	unsigned int p1,p2,p3,p4;
	double angle=160.1;
	simu.getScene()->addVertex(vec3(0,0,0), p1);
	simu.getScene()->addVertex(vec3(0,1,0), p2);
	simu.getScene()->addVertex(vec3(1,1,-tan(angle/2*M_PI/180)), p3);
	simu.getScene()->addVertex(vec3(1,1,tan(angle/2*M_PI/180)), p4);

	simu.getScene()->addTriangle(p1,p2,p3,&Material());
	simu.getScene()->addTriangle(p1,p4,p2,&Material());

	// Create a solver
	TYANIME3DRayTracerSolverAdapter* solver=new TYANIME3DRayTracerSolverAdapter();

	// Add diffraction edges and selectors filtering rays  
	solver->postTreatmentScene(simu.getScene(),simu.getSources(),simu.getRecepteurs());

	std::vector<vec3>* vertices=simu.getScene()->getVertices(); //Recover vertices from scene
	std::vector<Shape*>* shapes=simu.getScene()->getShapes();   //Recover shapes from scene

	EXPECT_EQ(4,vertices->size());	//Test number of vertices
	EXPECT_EQ(2,shapes->size());	//Test number of shapes

	//Test the class of each shape
	Triangle* shape1=dynamic_cast<Triangle*>(shapes->at(0));
	Triangle* shape2=dynamic_cast<Triangle*>(shapes->at(1));

	EXPECT_TRUE(shape1);
	EXPECT_TRUE(shape2);
}

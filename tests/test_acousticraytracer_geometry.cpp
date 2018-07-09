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
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Triangle.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Cylindre.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Latitude2DSampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Longitude2DSampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/RandomSphericSampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/UniformSphericSampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/UniformSphericSampler2.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/UniformBeamSampler.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;


unsigned int N_TESTS=100;

/***********************************************************************
						        Scene
************************************************************************/


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

	EXPECT_TRUE(vertex.compare(vec3(10,0,5))); //Test vertex coordinates
	
}


// Test the addition of a random number of points to a scene
TEST(test_scene, test_random_points)
{

	// Create a ray tracer
	Simulation simu;

	unsigned int seed=(unsigned int)time(NULL); //Generate seed from time
	srand(seed);				  //Init random number generator with seed
	cout<<"[          ] Random number generator initialized with seed "<<seed<<endl;

	const int nb_vertices=rand()%100; //Random nuymber of points

	std::vector<unsigned int> vertex_ids(nb_vertices,0); //vector of vertex ids
	std::vector<vec3> vertices_init;					 //vector recording the vertices added to the scene
	
	for(unsigned int i=0;i<vertex_ids.size();i++){

		//Random coordinates
		decimal x=(decimal)(rand()%1000);
		decimal y=(decimal)(rand()%1000);
		decimal z=(decimal)(rand()%1000);

		vec3 vertex=vec3(x,y,z);

		vertices_init.push_back(vertex);						//Add vertex record the newly generated vertex
		simu.getScene()->addVertex(vertex, vertex_ids.at(i));	//Add vertex to the scene
	}


	std::vector<vec3>* vertices=simu.getScene()->getVertices();
	
	EXPECT_EQ(nb_vertices,vertices->size());  //Test number of vertices ins cene
	

	for(unsigned int i=0;i<vertices->size();i++){

		unsigned int id=vertex_ids.at(i);	
		EXPECT_EQ(i,id);										    //Test vertex id
		EXPECT_TRUE(vertices->at(id).compare(vertices_init.at(i)));	//Test if the vertices in the scene correspond to the recorded ones 
	}
}


// Test the addition of triangles to a scene and that their BBox and normal are correct
TEST(test_scene, test_triangles)
{

	Simulation simu;

	unsigned int p1,p2,p3,p4,p5,p6;
	Material* mat=new Material();

	simu.getScene()->addVertex(vec3(5,0,5), p1);
	simu.getScene()->addVertex(vec3(5,5,-5), p2);
	simu.getScene()->addVertex(vec3(5,-5,-5), p3);

	simu.getScene()->addVertex(vec3(0,0,0), p4);
	simu.getScene()->addVertex(vec3(-3,3,-3), p5);
	simu.getScene()->addVertex(vec3(-6,-3,-3), p6);

	simu.getScene()->addTriangle(p1,p2,p3,mat); 
	simu.getScene()->addTriangle(p1,p3,p2,mat); 

	simu.getScene()->addTriangle(p4,p5,p6,mat); 
	simu.getScene()->addTriangle(p4,p6,p5,mat); 

	std::vector<vec3>* vertices=simu.getScene()->getVertices(); //Recover vertices from scene
	std::vector<Shape*>* shapes=simu.getScene()->getShapes();   //Recover shapes from scene

	EXPECT_EQ(6,vertices->size());	//Test number of vertices
	EXPECT_EQ(4,shapes->size());	//Test number of shapes
	

	//Test the class of each shape
	for(unsigned int i=0;i<shapes->size();i++){
		Triangle* shape=dynamic_cast<Triangle*>(shapes->at(i));
		EXPECT_TRUE(shape>0);
	}



	//Test normals
	vec3 n1(-1,0,0);			
	vec3 n2=n1*-1;					
	vec3 n3(-18,9,27);		
	n3.normalize();
	vec3 n4=n3*-1;

	EXPECT_TRUE(shapes->at(0)->getNormal().compare(n1)); 
	EXPECT_TRUE(shapes->at(1)->getNormal().compare(n2)); 
	EXPECT_TRUE(shapes->at(2)->getNormal().compare(n3)); 
	EXPECT_TRUE(shapes->at(3)->getNormal().compare(n4)); 


	//Test BBoxes
	BBox bbox1=shapes->at(0)->getBBox();
	BBox bbox2=shapes->at(1)->getBBox();
	BBox bbox3=shapes->at(2)->getBBox();
	BBox bbox4=shapes->at(3)->getBBox();

	vec3 one_hundredth((decimal)0.01,(decimal)0.01,(decimal)0.01);

	EXPECT_TRUE(bbox1.getBBMin().compare(vec3(5,-5,-5)-one_hundredth)); 
	EXPECT_TRUE(bbox1.getBBMax().compare(vec3(5,5,5)+one_hundredth)); 
	EXPECT_TRUE(bbox1.getCentroid().compare(vec3(5,0,0))); 
	
	EXPECT_TRUE(bbox2.getBBMin().compare(vec3(5,-5,-5)-one_hundredth));
	EXPECT_TRUE(bbox2.getBBMax().compare(vec3(5,5,5)+one_hundredth)); 
	EXPECT_TRUE(bbox2.getCentroid().compare(vec3(5,0,0))); 

	EXPECT_TRUE(bbox3.getBBMin().compare(vec3(-6,-3,-3)-one_hundredth)); 
	EXPECT_TRUE(bbox3.getBBMax().compare(vec3(0,3,0)+one_hundredth)); 
	EXPECT_TRUE(bbox3.getCentroid().compare(vec3(-3,0,-1.5))); 

	EXPECT_TRUE(bbox4.getBBMin().compare(vec3(-6,-3,-3)-one_hundredth));
	EXPECT_TRUE(bbox4.getBBMax().compare(vec3(0,3,0)+one_hundredth)); 
	EXPECT_TRUE(bbox4.getCentroid().compare(vec3(-3,0,-1.5))); 



	//Test scene boundaries
	BBox global_bbox=simu.getScene()->getGlobalBox();

	EXPECT_TRUE(global_bbox.getBBMin().compare(vec3(-6,-5,-5)-one_hundredth)); 
	EXPECT_TRUE(global_bbox.getBBMax().compare(vec3(5,5,5)+one_hundredth)); 
	EXPECT_TRUE(global_bbox.getCentroid().compare(vec3(-0.5,0,0))); 
	

}


// Test the addition a building to a scene
TEST(test_scene, test_building)
{

	Simulation simu;

	std::vector<unsigned int> vertex_ids(4,0);
	Material* mat=new Material();

	vec3 origin(7,-15,3);
	vec3 dimensions(12,5,8);
	simu.getScene()->addBuilding(origin,dimensions,mat);

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
	for(unsigned int i=0;i<shapes->size();i++){
		Triangle* shape=dynamic_cast<Triangle*>(shapes->at(i));
		EXPECT_TRUE(shape>0);
	}

	//Test scene boundaries
	BBox bbox=simu.getScene()->getGlobalBox();
	vec3 one_hundredth((decimal)0.01,(decimal)0.01,(decimal)0.01);

	EXPECT_TRUE(bbox.getBBMin().compare(origin-one_hundredth)); 
	EXPECT_TRUE(bbox.getBBMax().compare(origin+dimensions+one_hundredth)); 
	EXPECT_TRUE(bbox.getCentroid().compare(origin+dimensions/2)); 
}

// Test the presence of a diffraction edge when the angle between two faces is low enough
TEST(test_scene, test_diffraction_edge1)
{

	Simulation simu;

	unsigned int p1,p2,p3,p4;
	Material* mat=new Material();
	double angle=175;
	simu.getScene()->addVertex(vec3(0,0,0), p1);
	simu.getScene()->addVertex(vec3(0,1,0), p2);
	simu.getScene()->addVertex(vec3(1,1,(decimal)(-tan(angle/2*M_PI/180))), p3);
	simu.getScene()->addVertex(vec3(1,1,(decimal)tan(angle/2*M_PI/180)), p4);

	simu.getScene()->addTriangle(p1,p2,p3,mat);
	simu.getScene()->addTriangle(p1,p4,p2,mat);

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

	Cylindre* shape4=dynamic_cast<Cylindre*>(shapes->at(1));
	Triangle* shape5=dynamic_cast<Triangle*>(shapes->at(2));

	EXPECT_TRUE(shape1>0);
	EXPECT_TRUE(shape2>0);
	EXPECT_TRUE(shape3>0);

	EXPECT_TRUE(shape4==0);
	EXPECT_TRUE(shape5==0);
	
}
// Test the presence of a diffraction edge when the angle between two faces is low enough
TEST(test_scene, test_diffraction_edge2)
{

	Simulation simu;
	 
	AcousticRaytracerConfiguration* config =AcousticRaytracerConfiguration::get();
	config->AngleDiffMin=20;

	unsigned int p1,p2,p3,p4;
	Material* mat=new Material();
	double angle=160;
	simu.getScene()->addVertex(vec3(0,0,0), p1);
	simu.getScene()->addVertex(vec3(0,1,0), p2);
	simu.getScene()->addVertex(vec3(1,1,(decimal)-tan(angle/2*M_PI/180)), p3);
	simu.getScene()->addVertex(vec3(1,1,(decimal)tan(angle/2*M_PI/180)), p4);

	simu.getScene()->addTriangle(p1,p2,p3,mat);
	simu.getScene()->addTriangle(p1,p4,p2,mat);

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

	Cylindre* shape4=dynamic_cast<Cylindre*>(shapes->at(1));
	Triangle* shape5=dynamic_cast<Triangle*>(shapes->at(2));

	EXPECT_TRUE(shape1>0);
	EXPECT_TRUE(shape2>0);
	EXPECT_TRUE(shape3>0);

	EXPECT_TRUE(shape4==0);
	EXPECT_TRUE(shape5==0);
	
}


// Test the absence of diffraction edge when the angle between two faces is too high
TEST(test_scene, test_diffraction_edge3)
{

	Simulation simu;

	unsigned int p1,p2,p3,p4;
	Material* mat=new Material();
	double angle=175.1;
	simu.getScene()->addVertex(vec3(0,0,0), p1);
	simu.getScene()->addVertex(vec3(0,1,0), p2);
	simu.getScene()->addVertex(vec3(1,1,(decimal)-tan(angle/2*M_PI/180)), p3);
	simu.getScene()->addVertex(vec3(1,1,(decimal)tan(angle/2*M_PI/180)), p4);

	simu.getScene()->addTriangle(p1,p2,p3,mat);
	simu.getScene()->addTriangle(p1,p4,p2,mat);

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
	Cylindre* shape3=dynamic_cast<Cylindre*>(shapes->at(1));

	EXPECT_TRUE(shape1>0);
	EXPECT_TRUE(shape2>0);

	EXPECT_TRUE(shape3==0);
}

// Test the absence of diffraction edge when the angle between two faces is too high
TEST(test_scene, test_diffraction_edge4)
{

	Simulation simu;
	 
	AcousticRaytracerConfiguration* config =AcousticRaytracerConfiguration::get();
	config->AngleDiffMin=20;

	unsigned int p1,p2,p3,p4;
	Material* mat=new Material();
	double angle=160.1;
	simu.getScene()->addVertex(vec3(0,0,0), p1);
	simu.getScene()->addVertex(vec3(0,1,0), p2);
	simu.getScene()->addVertex(vec3(1,1,(decimal)-tan(angle/2*M_PI/180)), p3);
	simu.getScene()->addVertex(vec3(1,1,(decimal)tan(angle/2*M_PI/180)), p4);

	simu.getScene()->addTriangle(p1,p2,p3,mat);
	simu.getScene()->addTriangle(p1,p4,p2,mat);

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
	Cylindre* shape3=dynamic_cast<Cylindre*>(shapes->at(1));

	EXPECT_TRUE(shape1>0);
	EXPECT_TRUE(shape2>0);

	EXPECT_TRUE(shape3==0);
}



/***********************************************************************
						        Cylindre
************************************************************************/


// Test the getIntersection() function (trivial case)
TEST(test_cylindre, get_intersection1){

	//Create the two shapes of the cylinder
	Triangle* s1=new Triangle();
	Triangle* s2=new Triangle();

	s1->setNormal(vec3((decimal)1.4142155624,(decimal)1.4142155624,(decimal)0));
	s2->setNormal(vec3((decimal)-1.4142155624,(decimal)1.4142155624,(decimal)0));

	vector<vec3> vertices;
	vertices.push_back(vec3(0,0,-5));
	vertices.push_back(vec3(0,0,5));

	Cylindre cylindre(s1,s2,&vertices,0,1,(decimal)0.2);

	vec3 ray_pos(-10,0,0);
	Ray ray=new Ray(ray_pos,vec3(1,0,0)); //points in direction of the cylinder

	Intersection inter;

	bool res=cylindre.getIntersection(ray,inter);

	EXPECT_TRUE(res); //Intersection found
	EXPECT_FLOAT_EQ((decimal)9.8,inter.t); // the intersection is 10 units minus the cylinder's thickness (0.2) away from position of the ray.
	
	ray=new Ray(ray_pos,vec3(-1,0,0)); //points away from the cylinder
	

	res=cylindre.getIntersection(ray,inter);
	EXPECT_FALSE(res); //No Intersection found
}

// Test the getIntersection() function (general case)
TEST(test_cylindre, get_intersection2){

	//Create the two shapes of the cylinder
	Triangle* s1=new Triangle();
	Triangle* s2=new Triangle();

	vec3 n1=vec3(1,-2,3);
	vec3 n2=vec3(-2,-4,-1);

	n1.normalize();
	n2.normalize();

	s1->setNormal(n1);
	s2->setNormal(n2);

	vec3 axis=vec3(-7/4,5/8,1);

	vector<vec3> vertices;
	vec3 p1=vec3(10,7,-5);
	vec3 p2=p1+axis;

	vertices.push_back(p1);
	vertices.push_back(p2);

	Cylindre cylindre(s1,s2,&vertices,0,1,(decimal)0.2);
	
	
	vec3 ray_pos(6,-7,2);
	vec3 axis_middle=p1+axis/2;
	vec3 dir=axis_middle-ray_pos;
	dir.normalize();
	Ray ray=new Ray(ray_pos,dir); //points in direction of the cylinder

	Intersection inter;

	bool res=cylindre.getIntersection(ray,inter);
	EXPECT_TRUE(res); //Intersection found
	EXPECT_FLOAT_EQ((decimal)15.617231,inter.t); 
	
	ray=new Ray(ray_pos,dir+vec3(-1,7,5)); //points away from the cylinder
	
	res=cylindre.getIntersection(ray,inter);
	EXPECT_FALSE(res); //No Intersection found
	
}

/***********************************************************************
						        Sphere
************************************************************************/


// Test the getIntersection() function (trivial case)
TEST(test_sphere, get_intersection1){

	Sphere s(vec3(0,0,0),1);
	Ray r=new Ray(vec3(-10,0,0),vec3(1,0,0)); //ray pointing in direction of the sphere
	Intersection inter;
	bool res=s.getIntersection(r,inter);

	EXPECT_TRUE(res); //should find an intersection
	EXPECT_FLOAT_EQ((decimal)9,inter.t); //the distance between the ray position and the intersection should be 9


	r=new Ray(vec3(-10,0,0),vec3(-1,0,0)); //ray pointing away of the sphere
	res=s.getIntersection(r,inter);

	EXPECT_FALSE(res); //no intersection
}


// Test the getIntersection() function (general case)
TEST(test_sphere, get_intersection2){

	vec3 sphere_pos(-8,11,7);
	decimal radius=3.5;
	Sphere s(sphere_pos,radius);
	vec3 ray_pos(4,-2,-1);
	vec3 ray_dir=sphere_pos-ray_pos;
	ray_dir.normalize();

	Ray r=new Ray(ray_pos,ray_dir); //ray pointing in direction of the sphere
	Intersection inter;
	bool res=s.getIntersection(r,inter);

	EXPECT_TRUE(res); //should find an intersection
	EXPECT_NEAR((decimal)15.91649,inter.t,0.00001); //Windows and Linux do not agree on the exact value of this, hence the EXPECT_NEAR

	ray_dir=vec3(-8,9,14);
	ray_dir.normalize();
	r=new Ray(ray_pos,ray_dir); //ray pointing away of the sphere
	res=s.getIntersection(r,inter);

	EXPECT_FALSE(res); //no intersection
}


/***********************************************************************
						        Triangle
************************************************************************/

// Test the getIntersection() function (trivial case)
TEST(test_triangle, get_intersection1){

	std::vector<vec3> vertices;
	vertices.push_back(vec3(0,5,5));
	vertices.push_back(vec3(0,5,-5));
	vertices.push_back(vec3(0,-5,0));

	Triangle t1(0,1,2,&vertices,new Material());
	Triangle t2(1,0,2,&vertices,new Material()); //flip the triangle's normal


	Ray r=new Ray(vec3(-10,0,0),vec3(1,0,0)); //ray pointing in direction of the triangle
	Intersection inter;

	bool res=t1.getIntersection(r,inter);
	EXPECT_TRUE(res); //should find an intersection
	EXPECT_FLOAT_EQ((decimal)10,inter.t); //the distance between the ray position and the intersection should be 10

	res=t2.getIntersection(r,inter);
	EXPECT_FALSE(res); //normal points in the same direction has the ray -> no intersection

	r=new Ray(vec3(-10,0,0),vec3(-1,0,0)); //ray pointing away of the triangle
	res=t1.getIntersection(r,inter);

	EXPECT_FALSE(res); //should not find an intersection
}

// Test the getIntersection() function (general case)
TEST(test_triangle, get_intersection2){


	vec3 p1(-1,10,7);
	vec3 p2(3,-4,-1);
	vec3 p3(17,0,2);

	vec3 barycenter=(p1+p2+p3)/3;
	vec3 ray_pos(8,-7,9);
	vec3 ray_dir=barycenter-ray_pos;
	ray_dir.normalize();

	std::vector<vec3> vertices;
	vertices.push_back(p1);
	vertices.push_back(p2);
	vertices.push_back(p3);

	Triangle t1(0,1,2,&vertices,new Material());
	Triangle t2(1,0,2,&vertices,new Material()); //flip the normal

	Ray r=new Ray(ray_pos,ray_dir); //ray pointing in direction of the triangle
	Intersection inter;
	bool res=t1.getIntersection(r,inter);

	EXPECT_TRUE(res); //should find an intersection
	EXPECT_FLOAT_EQ((decimal)11.130539,inter.t);

	res=t2.getIntersection(r,inter);
	EXPECT_FALSE(res); //normal points in the same direction has the ray -> no intersection

	ray_dir=vec3(-14,21,7);
	ray_dir.normalize();
	r=new Ray(ray_pos,ray_dir); //ray pointing away of the triangle

	res=t1.getIntersection(r,inter);
	EXPECT_FALSE(res); //should not find an intersection
}



/***********************************************************************
						       Repere
************************************************************************/


// Test the frame changing functions
TEST(test_repere, change_frame){

	vec3 v1(1,0,0);
	vec3 v2(0,1,0);
	vec3 v3(0,0,1);

	Repere r(v2,v3,v1,vec3(0,0,0));


	EXPECT_TRUE(r.vectorFromGlobalToLocal(v1)==v3);
	EXPECT_TRUE(r.vectorFromGlobalToLocal(v2)==v1);
	EXPECT_TRUE(r.vectorFromGlobalToLocal(v3)==v2);

	EXPECT_TRUE(r.vectorFromLocalToGlobal(v1)==v2);
	EXPECT_TRUE(r.vectorFromLocalToGlobal(v2)==v3);
	EXPECT_TRUE(r.vectorFromLocalToGlobal(v3)==v1);

	EXPECT_TRUE(r.positionFromGlobalToLocal(v1)==v3);
	EXPECT_TRUE(r.positionFromGlobalToLocal(v2)==v1);
	EXPECT_TRUE(r.positionFromGlobalToLocal(v3)==v2);

	EXPECT_TRUE(r.positionFromLocalToGlobal(v1)==v2);
	EXPECT_TRUE(r.positionFromLocalToGlobal(v2)==v3);
	EXPECT_TRUE(r.positionFromLocalToGlobal(v3)==v1);


	v1=vec3(-3,5,1);
	v2=vec3(2,1,1);
	v3=vec3(4,5,-13);

	v1.normalize();
	v3.normalize();
	v2.normalize();

	r=Repere(v1,v2,v3,vec3(8,-7,12));


	EXPECT_TRUE(r.vectorFromGlobalToLocal(r.vectorFromLocalToGlobal(vec3(17,-8,-9))).barelyEqual(vec3(17,-8,-9)));
	EXPECT_TRUE(r.vectorFromGlobalToLocal(r.vectorFromLocalToGlobal(vec3(-1,0,2))).barelyEqual(vec3(-1,0,2)));
	EXPECT_TRUE(r.vectorFromGlobalToLocal(r.vectorFromLocalToGlobal(vec3(23,95,-74))).barelyEqual(vec3(23,95,-74)));

	EXPECT_TRUE(r.positionFromLocalToGlobal(r.positionFromGlobalToLocal(vec3(7,-3,4))).barelyEqual(vec3(7,-3,4)));
	EXPECT_TRUE(r.positionFromLocalToGlobal(r.positionFromGlobalToLocal(vec3(-4,-3,-12))).barelyEqual(vec3(-4,-3,-12)));
	EXPECT_TRUE(r.positionFromLocalToGlobal(r.positionFromGlobalToLocal(vec3(8,-7,-6))).barelyEqual(vec3(8,-7,-6)));
}


/***********************************************************************
						       Latitude2DSampler
************************************************************************/

// Test the Latitude2DSampler getSample function
TEST(test_latitude2dsampler, get_sample){

	Latitude2DSampler sampler=new Latitude2DSampler(4,0,0);
	sampler.setStartPhi(0);
	sampler.setEndPhi(180);


	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3(1,0,0)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)0.5,(decimal)1.7320508075/2,(decimal)0)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)-0.5,(decimal)1.7320508075/2,(decimal)0)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3(-1,0,0)));


	sampler=new Latitude2DSampler(4,0,0);
	sampler.setStartTheta(45);
	sampler.setStartPhi(90);
	sampler.setEndPhi(270);


	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)0,(decimal)0.707107,(decimal)0.707107)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)-0.612372,(decimal)0.353553,(decimal)0.707107)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)-0.612372,(decimal)-0.353553,(decimal)0.707107)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)0,(decimal)-0.707107,(decimal)0.707107)));


	sampler=new Latitude2DSampler(8,0,0);
	sampler.setStartTheta(45);
	sampler.setStartPhi(0);
	sampler.setEndPhi(360);


	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)0.707107,(decimal)0,(decimal)0.707107)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)0.5,(decimal)0.5,(decimal)0.707107)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)0,(decimal)0.707107,(decimal)0.707107)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)-0.5,(decimal)0.5,(decimal)0.707107)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)-0.707107,(decimal)0,(decimal)0.707107)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)-0.5,(decimal)-0.5,(decimal)0.707107)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)0,(decimal)-0.707107,(decimal)0.707107)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)0.5,(decimal)-0.5,(decimal)0.707107)));



}


/***********************************************************************
						       Longitude2DSampler
************************************************************************/

// Test the Longitude2DSampler getSample function
TEST(test_longitude2dsampler, get_sample){

	Longitude2DSampler sampler=new Longitude2DSampler(4,0,0);
	sampler.setStartTheta(0);
	sampler.setEndTheta(180);

	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3(1,0,0)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)0.5,(decimal)0,(decimal)1.7320508075/2)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)-0.5,(decimal)0,(decimal)1.7320508075/2)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3(-1,0,0)));


	sampler=new Longitude2DSampler(4,0,0);
	sampler.setStartPhi(45);
	sampler.setStartTheta(90);
	sampler.setEndTheta(270);
	sampler.init();

	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3(0,0,1)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)-0.612372,(decimal)-0.612372,(decimal)0.5)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3((decimal)-0.612372,(decimal)-0.612372,(decimal)-0.5)));
	EXPECT_TRUE(sampler.getSample().barelyEqual(vec3(0,0,-1)));


}


/***********************************************************************
						       UniformSphericSampler
************************************************************************/


// Check that a UniformSphericSampler with 0 nbRays returns 0 realNbRays
TEST(test_uniformsphericsampler, no_sample){

	UniformSphericSampler sampler=new UniformSphericSampler((unsigned int)0);
	EXPECT_EQ(sampler.getRealNbRays(),0); // No samples
}

// Check that a UniformBeamSampler with 1 nbRays returns a single ray equal to the directivity
TEST(test_uniformsphericsampler, one_sample){

	UniformSphericSampler sampler=new UniformSphericSampler((unsigned int)1);
	EXPECT_EQ(sampler.getRealNbRays(),1);			 // one single sample
	
}


// Non regression test of UniformSphericSampler
TEST(test_uniformsphericsampler, get_sample){

	//some of the samples obtained from a previous execution
	vec3 expected_samples[]={vec3((decimal)-0.0654031,(decimal)-5.71772e-009,(decimal)0.997859),
							 vec3((decimal)-0.0691799,(decimal)-0.182413,(decimal)0.980785),
							 vec3((decimal)-0.289607,(decimal)0.139467,(decimal)0.94693),
							 vec3((decimal)0.304332,(decimal)0.687492,(decimal)0.659346),
							 vec3((decimal)-0.212852,(decimal)0.871249,(decimal)0.442289),
						   	 vec3((decimal)-0.903503,(decimal)-0.283476,(decimal)0.321439),
							 vec3((decimal)0.308356,(decimal)-0.94902,(decimal)0.0654031),
							 vec3((decimal)-0.442242,(decimal)-0.894508,(decimal)-0.0654031),
							 vec3((decimal)0.417023,(decimal)-0.850158,(decimal)-0.321439),
							 vec3((decimal)0.896873,(decimal)1.56814e-007,(decimal)-0.442289),
							};

	//indexes of the expected samples
	int expected_samples_index[]={1,12,25,154,265,341,485,542,678,748};

	int const n=1000;
	vec3 samples[n];
	decimal theta=(decimal)M_PIDIV2;
	decimal phi=(decimal)M_2PI;
	UniformSphericSampler sampler=new UniformSphericSampler(n,theta,phi);

	//get the samples
	for(unsigned int i=0;i<sampler.getRealNbRays();i++)
			samples[i]=sampler.getSample();		

	

	//check that the samples at indexes expected_samples_index are equal to the expected_samples
	for(unsigned int i=0;i<10;i++){
		int index=expected_samples_index[i];
		EXPECT_TRUE(expected_samples[i]==samples[index]);
	}
}

/***********************************************************************
						       UniformSphericSampler2
************************************************************************/


// Check that a UniformSphericSampler2 with 0 nbRays returns 0 realNbRays
TEST(test_uniformsphericsampler2, no_sample){

	UniformSphericSampler2 sampler=new UniformSphericSampler2((unsigned int)0);
	EXPECT_EQ(sampler.getRealNbRays(),0); // No samples
}

// Check that a UniformSphericSampler2 with 1 nbRays returns a single ray equal to the directivity
TEST(test_uniformsphericsampler2, one_sample){

	UniformSphericSampler2 sampler=new UniformSphericSampler2((unsigned int)1);
	EXPECT_EQ(sampler.getRealNbRays(),1);			 // one single sample
	vec3 sample=sampler.getSample();
}

// Non regression test of UniformSphericSampler2
TEST(test_uniformsphericsampler2, get_sample){

	//some of the samples obtained from a previous execution
	vec3 expected_samples[]={vec3((decimal)-0.0880964,(decimal)0.0640058,(decimal)0.994053),
							 vec3((decimal)0.175145,(decimal)-0.12725,(decimal)0.976285),
							 vec3((decimal)0.0668467,(decimal)-0.314489,(decimal)0.946905),
							 vec3((decimal)-0.704608,(decimal)0.300932,(decimal)0.642626),
							 vec3((decimal)0.759836,(decimal)0.539893,(decimal)0.362168),
							 vec3((decimal)-0.0952962,(decimal)-0.961296,(decimal)0.258513),
							 vec3((decimal)-0.403906,(decimal)-0.912431,(decimal)-0.0657981),
							 vec3((decimal)0.648291,(decimal)-0.741229,(decimal)-0.174064),
							 vec3((decimal)0.606294,(decimal)-0.632957,(decimal)-0.481428),
							 vec3((decimal)-0.39807,(decimal)-0.63738,(decimal)-0.659762),
							};
	
	//indexes of the expected samples
	int expected_samples_index[]={1,12,25,154,265,341,485,542,678,748};

	int const n=1000;
	vec3 samples[n];
	decimal theta=(decimal)M_PIDIV2;
	decimal phi=(decimal)M_2PI;
	UniformSphericSampler2 sampler=new UniformSphericSampler2(n,theta,phi);

	//get the samples
	for(unsigned int i=0;i<sampler.getRealNbRays();i++)
			samples[i]=sampler.getSample();		


	//check that the samples at indexes expected_samples_index are equal to the expected_samples
	for(unsigned int i=0;i<10;i++){
		int index=expected_samples_index[i];
		EXPECT_TRUE(expected_samples[i]==samples[index]);
	}

}




/***********************************************************************
						       RandomSphericSampler
************************************************************************/ 

// Test the RandomSphericSampler getSample function
TEST(test_randomsphericsampler, get_sample){

	RandomSphericSampler sampler=new RandomSphericSampler();

	int const n=1000000;
	
	//idea: pick some parts of the surface of the sphere and check that they contain a number of samples proportional to their area

	//counters for each of the areas sampled on the sphere 
	//areas bounded by two circles
	int positive_sample1=0;
	int positive_sample2=0;
	int positive_sample3=0;
	//polar caps
	int positive_sample4=0;
	int positive_sample5=0;
	int positive_sample6=0;

	//each area is chosen by picking two slices along the same axis (h1 is also used to sample polar caps)
	double x_h1=0.32;
	double x_h2=0.58;
	
	double y_h1=0.63;
	double y_h2=1.25;
	
	double z_h1=1.12;
	double z_h2=1.73;

	//count the number of samples in each of the six sampled areas
	for(int i=0;i<n;i++){
		vec3 sample=sampler.getSample();

		//areas bounded by two circles
		if(sample.x>(1-x_h2) &&sample.x<(1-x_h1)) positive_sample1++;
		if(sample.y>(1-y_h2) &&sample.y<(1-y_h1)) positive_sample2++;
		if(sample.z>(1-z_h2) &&sample.z<(1-z_h1)) positive_sample3++;

		//polar caps
		if(sample.x>(1-x_h1)) positive_sample4++;
		if(sample.y>(1-y_h1)) positive_sample5++;
		if(sample.z>(1-z_h1)) positive_sample6++;
		
	}

	//theoretical probability of a sample beeing in each one of the six areas (area/(4*M_PI))
	double p1=(x_h2-x_h1)/2;
	double p2=(y_h2-y_h1)/2;
	double p3=(z_h2-z_h1)/2;
	double p4=x_h1/2;
	double p5=y_h1/2;
	double p6=z_h1/2;

	//estimates of the probabilities
	double p_hat1=positive_sample1/(n*1.0);
	double p_hat2=positive_sample2/(n*1.0);
	double p_hat3=positive_sample3/(n*1.0);
	double p_hat4=positive_sample4/(n*1.0);
	double p_hat5=positive_sample5/(n*1.0);
	double p_hat6=positive_sample6/(n*1.0);

	double z=3.890592; //confidence level of 99.99%

	//estimates of the standard deviation for each part 
	double sigma1=sqrt(p1*(1-p1)/n);
	double sigma2=sqrt(p2*(1-p2)/n);
	double sigma3=sqrt(p3*(1-p3)/n);
	double sigma4=sqrt(p4*(1-p4)/n);
	double sigma5=sqrt(p5*(1-p5)/n);
	double sigma6=sqrt(p6*(1-p6)/n);

	//bounds of the confidence interval for each sampled area
	double p_min1=p_hat1-z*sigma1;
	double p_max1=p_hat1+z*sigma1;

	double p_min2=p_hat2-z*sigma2;
	double p_max2=p_hat2+z*sigma2;

	double p_min3=p_hat3-z*sigma3;
	double p_max3=p_hat3+z*sigma3;

	double p_min4=p_hat4-z*sigma4;
	double p_max4=p_hat4+z*sigma4;

	double p_min5=p_hat5-z*sigma5;
	double p_max5=p_hat5+z*sigma5;

	double p_min6=p_hat6-z*sigma6;
	double p_max6=p_hat6+z*sigma6;

	//test if the true probabilities are within the bounds of their corresponding confidence interval
	EXPECT_TRUE(p_min1<=p1);
	EXPECT_TRUE(p_max1>=p1);

	EXPECT_TRUE(p_min2<=p2);
	EXPECT_TRUE(p_max2>=p2);	

	EXPECT_TRUE(p_min3<=p3);
	EXPECT_TRUE(p_max3>=p3);

	EXPECT_TRUE(p_min4<=p4);
	EXPECT_TRUE(p_max4>=p4);

	EXPECT_TRUE(p_min5<=p5);
	EXPECT_TRUE(p_max5>=p5);	

	EXPECT_TRUE(p_min6<=p6);
	EXPECT_TRUE(p_max6>=p6);	

}



/***********************************************************************
						       UniformBeamSampler
************************************************************************/

// Check that a UniformBeamSampler with 0 nbRays returns 0 realNbRays
TEST(test_beamsampler, no_sample){

	UniformBeamSampler sampler=new UniformBeamSampler((unsigned int)0);
	EXPECT_EQ(sampler.getRealNbRays(),0); // No samples
}

// Check that a UniformBeamSampler with 1 nbRays returns a single ray equal to the directivity
TEST(test_beamsampler, one_sample){

	unsigned int seed=(unsigned int)time(NULL); //Generate seed from time
	srand(seed);				  //Init random number generator with seed
	cout<<"[          ] Random number generator initialized with seed "<<seed<<endl;

	for(unsigned int i =0 ; i< N_TESTS ;i++){ // perform N_TESTS random tests
		// random directivity
		decimal x=decimal((rand()%200-100)/100.); 
		decimal y=decimal((rand()%200-100)/100.); 
		decimal z=decimal((rand()%200-100)/100.); 
		vec3 directivity(x,y,z);
		directivity.normalize();

		UniformBeamSampler sampler=new UniformBeamSampler((unsigned int)1,decimal(M_PI),directivity);
		EXPECT_EQ(sampler.getRealNbRays(),1);			 // one single sample
		EXPECT_TRUE(sampler.getSample()==directivity);   // equal to the directivity
	}
}


// Check that all samples lie in the opening angle of the cone
TEST(test_beamsampler,get_sample ){

	unsigned int seed=(unsigned int)time(NULL); //Generate seed from time
	srand(seed);				  //Init random number generator with seed
	cout<<"[          ] Random number generator initialized with seed "<<seed<<endl;

	for(unsigned int i =0 ; i< N_TESTS ;i++){ // perform N_TESTS random tests
		// random directivity
		decimal alpha=decimal(M_PI*(rand()%1000)/1000.+0.1); //add some small value to avoid null opening angles
		unsigned int n=rand()%1000+1; 

		decimal x=decimal((rand()%200-100)/100.); 
		decimal y=decimal((rand()%200-100)/100.); 
		decimal z=decimal((rand()%200-100)/100.); 

		vec3 directivity(x,y,z);
		directivity.normalize();

		UniformBeamSampler sampler=new UniformBeamSampler((unsigned int)n,alpha,directivity);
		EXPECT_TRUE(sampler.getRealNbRays()>=n);							 // RealNbRays must be >= n
		EXPECT_TRUE(sampler.getSample()==directivity);						 // The first sample should be equal to the directivity
		for(unsigned int j=1;j<sampler.getRealNbRays();j++)
			EXPECT_TRUE(sampler.isAcceptableSample(sampler.getSample()));	// Sample lies within the beam's cone
		

		//Requesting more samples than realNbRays should return (0,0,0)
		EXPECT_TRUE(sampler.getSample()==vec3(0,0,0));
	}

	
}

// Check that the opening angle and directivity are correct when set with the setter method
TEST(test_beamsampler, directivity_opening_angle){

	unsigned int seed=(unsigned int)time(NULL); //Generate seed from time
	srand(seed);								//Init random number generator with seed
	cout<<"[          ] Random number generator initialized with seed "<<seed<<endl;

	for(unsigned int i =0 ; i< N_TESTS ;i++){ // perform N_TESTS random tests
		// random directivity
		decimal alpha=decimal(M_PI*(rand()%1000)/1000.);
		unsigned int n=rand()%100; 

		decimal x=decimal((rand()%200-100)/100.); 
		decimal y=decimal((rand()%200-100)/100.); 
		decimal z=decimal((rand()%200-100)/100.); 

		vec3 directivity(x,y,z);
		directivity.normalize();

		UniformBeamSampler sampler1=new UniformBeamSampler((unsigned int)n,alpha,directivity);
		UniformBeamSampler sampler2=new UniformBeamSampler((unsigned int)n);
		sampler2.setOpeningAngle(alpha*M_180DIVPI);
		sampler2.setDirectivity(directivity);

		// The opening angle and directivity must be the same weather they are set by the constructor or the setter method
		EXPECT_NEAR(sampler1.getOpeningAngle(),sampler2.getOpeningAngle(),EPSILON_5);	
		EXPECT_TRUE(sampler1.getDirectivity()==sampler2.getDirectivity());
	}

}


// Test that only ray's tha tlie in the beams'cone are considered acceptable
TEST(test_beamsampler, is_acceptable_sample){

	unsigned int seed=(unsigned int)time(NULL); //Generate seed from time
	srand(seed);								//Init random number generator with seed
	cout<<"[          ] Random number generator initialized with seed "<<seed<<endl;

	for(unsigned int i =0 ; i< N_TESTS ;i++){ // perform N_TESTS random tests
		// random directivity
		decimal alpha=decimal(M_PI*(rand()%1000)/1000.);
		unsigned int n=rand()%100; 

		decimal x=decimal((rand()%200-100)/100.); 
		decimal y=decimal((rand()%200-100)/100.); 
		decimal z=decimal((rand()%200-100)/100.); 

		vec3 directivity(x,y,z);
		directivity.normalize();
		
		UniformBeamSampler sampler=new UniformBeamSampler((unsigned int)n,alpha,directivity);

		for(unsigned int j =0 ; j< N_TESTS ;j++){ // Try N_TESTS random samples
				decimal x=decimal((rand()%200-100)/100.); 
				decimal y=decimal((rand()%200-100)/100.); 
				decimal z=decimal((rand()%200-100)/100.); 

				vec3 random_sample(x,y,z);
				random_sample.normalize();
				decimal dot_product= random_sample*sampler.getDirectivity();
				if(dot_product>0 && acos(dot_product)<=(alpha/2+EPSILON_5))
					EXPECT_TRUE(sampler.isAcceptableSample(random_sample));	// Sample lies within the beam's cone
				else
					EXPECT_FALSE(sampler.isAcceptableSample(random_sample));
		}

	}

}

/***********************************************************************
						        BBox
************************************************************************/


TEST(test_3d_bbox,test_constructors)
{

	//Default constructor
    BBox bbox;
	EXPECT_TRUE(bbox.isNull);
	EXPECT_DOUBLE_EQ(0.,bbox.SurfaceArea());
	EXPECT_FLOAT_EQ(0.,bbox.diag());

	//Min-Max constructor
	bbox=BBox(vec3(-2.,-5.,2.),vec3(1.,3.,4.));
	EXPECT_FALSE(bbox.isNull);
	EXPECT_TRUE(vec3(-2.,-5.,2.)==bbox.getBBMin());
	EXPECT_TRUE(vec3(1.,3.,4.)==bbox.getBBMax());
	EXPECT_TRUE(vec3(-0.5,-1.,3.)==bbox.getCentroid());
	EXPECT_DOUBLE_EQ(92.,bbox.SurfaceArea());
	EXPECT_FLOAT_EQ(sqrtf(9.+64.+4),bbox.diag());
	
}

//Test the union with a point
TEST(test_3d_bbox,test_union_with_point)
{
	BBox bbox=BBox(vec3(-2.,-5.,2.),vec3(1.,3.,4.));

	//point inside the bbox (should not change the bbox)
	BBox res=bbox.Union(vec3(0.,2.,3.));
	EXPECT_TRUE(bbox.getBBMin()==res.getBBMin());
	EXPECT_TRUE(bbox.getBBMax()==res.getBBMax());
	EXPECT_TRUE(bbox.getCentroid()==res.getCentroid());

	res=res.Union(bbox,vec3(0.,2.,3.));
	EXPECT_TRUE(bbox.getBBMin()==res.getBBMin());
	EXPECT_TRUE(bbox.getBBMax()==res.getBBMax());
	EXPECT_TRUE(bbox.getCentroid()==res.getCentroid());


	//point outside the bbox (should enlarge the bbox)
	res=bbox.Union(vec3(-5.,4.,1.));
	EXPECT_TRUE(vec3(-5.,-5.,1.)==res.getBBMin());
	EXPECT_TRUE(vec3(1.,4.,4.)==res.getBBMax());
	EXPECT_TRUE(vec3(-2.,-0.5,2.5)==res.getCentroid());

	res=res.Union(bbox,vec3(-5.,4.,1.));
	EXPECT_TRUE(vec3(-5.,-5.,1.)==res.getBBMin());
	EXPECT_TRUE(vec3(1.,4.,4.)==res.getBBMax());
	EXPECT_TRUE(vec3(-2.,-0.5,2.5)==res.getCentroid());
}

//Test the union with another bbox
TEST(test_3d_bbox,test_union_with_bbox)
{

	BBox bbox1=BBox(vec3(-2.,-5.,2.),vec3(1.,3.,4.));

	//bbox2 included in bbox1 (res should be equal to bbox1)
	BBox bbox2=BBox(vec3(-1.,0.,2.),vec3(0.,1.,3.));
	BBox res=bbox1.Union(bbox2);
	EXPECT_TRUE(bbox1.getBBMin()==res.getBBMin());
	EXPECT_TRUE(bbox1.getBBMax()==res.getBBMax());
	EXPECT_TRUE(bbox1.getCentroid()==res.getCentroid());

	res=bbox2.Union(bbox1,bbox2);
	EXPECT_TRUE(bbox1.getBBMin()==res.getBBMin());
	EXPECT_TRUE(bbox1.getBBMax()==res.getBBMax());
	EXPECT_TRUE(bbox1.getCentroid()==res.getCentroid());

	//bbox 2 should not have been modified by previsous unions
	EXPECT_TRUE(vec3(-1.,0.,2.)==bbox2.getBBMin());
	EXPECT_TRUE(vec3(0.,1.,3.)==bbox2.getBBMax());
	EXPECT_TRUE(vec3(-0.5,0.5,2.5)==bbox2.getCentroid());


	//bbox1 included in bbox2 (res should be equal to bbox2)
	bbox2=BBox(vec3(-3.,-10.,0.),vec3(4.,5.,7.));
	res=bbox1.Union(bbox2);
	EXPECT_TRUE(bbox2.getBBMin()==res.getBBMin());
	EXPECT_TRUE(bbox2.getBBMax()==res.getBBMax());
	EXPECT_TRUE(bbox2.getCentroid()==res.getCentroid());

	res=bbox2.Union(bbox1,bbox2);
	EXPECT_TRUE(bbox2.getBBMin()==res.getBBMin());
	EXPECT_TRUE(bbox2.getBBMax()==res.getBBMax());
	EXPECT_TRUE(bbox2.getCentroid()==res.getCentroid());

	//bbox 1 should not have been modified by previsous unions
	EXPECT_TRUE(vec3(-2.,-5.,2.)==bbox1.getBBMin());
	EXPECT_TRUE(vec3(1.,3.,4.)==bbox1.getBBMax());
	EXPECT_TRUE(vec3(-0.5,-1.,3.)==bbox1.getCentroid());

	//bboxs not included in each other
	bbox2=BBox(vec3(-3.,-3.,1.),vec3(2.,2.,3.));

	res=bbox1.Union(bbox2);
	EXPECT_TRUE(vec3(-3.,-5.,1.)==res.getBBMin());
	EXPECT_TRUE(vec3(2.,3.,4.)==res.getBBMax());
	EXPECT_TRUE(vec3(-0.5,-1.,2.5)==res.getCentroid());

	res=bbox2.Union(bbox1,bbox2);
	EXPECT_TRUE(vec3(-3.,-5.,1.)==res.getBBMin());
	EXPECT_TRUE(vec3(2.,3.,4.)==res.getBBMax());
	EXPECT_TRUE(vec3(-0.5,-1.,2.5)==res.getCentroid());

	//bbox 1 and 2 should not have been modified by previsous unions
	EXPECT_TRUE(vec3(-2.,-5.,2.)==bbox1.getBBMin());
	EXPECT_TRUE(vec3(1.,3.,4.)==bbox1.getBBMax());
	EXPECT_TRUE(vec3(-0.5,-1.,3.)==bbox1.getCentroid());

	EXPECT_TRUE(vec3(-3.,-3.,1.)==bbox2.getBBMin());
	EXPECT_TRUE(vec3(2.,2.,3.)==bbox2.getBBMax());
	EXPECT_TRUE(vec3(-0.5,-0.5,2.)==bbox2.getCentroid());


}


//Test the isInBox method 
TEST(test_3d_bbox,test_is_in_box)
{

	BBox bbox(vec3(-5,1,-2),vec3(1,5,7));

	//Inside
	EXPECT_TRUE(bbox.isInBox(vec3(0,1,0)));
	EXPECT_TRUE(bbox.isInBox(vec3(-5,1,-2)));
	EXPECT_TRUE(bbox.isInBox(vec3(1,5,7)));
	EXPECT_TRUE(bbox.isInBox(vec3(0,1,-2)));
	EXPECT_TRUE(bbox.isInBox(vec3(-4,3,-1)));

	//Outside
	EXPECT_FALSE(bbox.isInBox(vec3(-5,1,(decimal)-2.00001)));
	EXPECT_FALSE(bbox.isInBox(vec3(-5,(decimal)0.99999,-2.)));
	EXPECT_FALSE(bbox.isInBox(vec3((decimal)-5.00001,1,-2.)));
	EXPECT_FALSE(bbox.isInBox(vec3((decimal)1.00001,5,7)));
	EXPECT_FALSE(bbox.isInBox(vec3(1,(decimal)5.00001,7)));
	EXPECT_FALSE(bbox.isInBox(vec3(1,5,(decimal)7.00001)));

}


//Test the Inside method 
TEST(test_3d_bbox,test_inside)
{

	BBox bbox(vec3(-5,1,-2),vec3(1,5,7));

	//Inside
	EXPECT_TRUE(bbox.Inside(vec3(0,1,0)));
	EXPECT_TRUE(bbox.Inside(vec3(-5,1,-2)));
	EXPECT_TRUE(bbox.Inside(vec3(1,5,7)));
	EXPECT_TRUE(bbox.Inside(vec3(0,1,-2)));
	EXPECT_TRUE(bbox.Inside(vec3(-4,3,-1)));

	//Outside
	EXPECT_FALSE(bbox.Inside(vec3(-5,1,(decimal)-2.00001)));
	EXPECT_FALSE(bbox.Inside(vec3(-5,(decimal)0.99999,-2.)));
	EXPECT_FALSE(bbox.Inside(vec3((decimal)-5.00001,1,-2.)));
	EXPECT_FALSE(bbox.Inside(vec3((decimal)1.00001,5,7)));
	EXPECT_FALSE(bbox.Inside(vec3(1,(decimal)5.00001,7)));
	EXPECT_FALSE(bbox.Inside(vec3(1,5,(decimal)7.00001)));

}

//Test the intersectBox method 
TEST(test_3d_bbox,test_intersect_box)
{
	BBox bbox(vec3(-4,2,-5),vec3(-1,4,-2));

	EXPECT_TRUE(bbox.intersectBox(BBox(vec3(-3,2,-4),vec3(2,4,1))));
	EXPECT_TRUE(bbox.intersectBox(BBox(vec3(-3,2,-4),vec3(-2,4,-2))));
	EXPECT_TRUE(bbox.intersectBox(BBox(vec3(-3,1,-2),vec3(-1,4,-2))));
	EXPECT_TRUE(bbox.intersectBox(BBox(vec3(-6,3,-4),vec3(2,4,-3))));

	EXPECT_FALSE(bbox.intersectBox(BBox(vec3(-10,-8,-9),vec3(-4,-6,-8))));
	EXPECT_FALSE(bbox.intersectBox(BBox(vec3(-10,-8,-9),vec3(-5,-2,-8))));
	EXPECT_FALSE(bbox.intersectBox(BBox(vec3(-10,-8,-9),vec3(-5,-6,-3))));
}


//Test the IntersectP method 
TEST(test_3d_bbox,test_intersect_p)
{

	BBox bbox(vec3(1,1,1),vec3(3,3,3));

	decimal hit1=-1;
	decimal hit2=-1;
	//Ray passes under the bbox
	EXPECT_FALSE(bbox.IntersectP(vec3(0,0,0),vec3(1,0,0),&hit1,&hit2));
	EXPECT_FLOAT_EQ(-1.,hit1);
	EXPECT_FLOAT_EQ(-1.,hit2);


	hit1=-1;
	hit2=-1;
	//Ray hits the bbox
	EXPECT_TRUE(bbox.IntersectP(vec3(0,3,3),vec3(1,0,0),&hit1,&hit2));
	EXPECT_FLOAT_EQ(1.,hit1);
	EXPECT_FLOAT_EQ(3.,hit2);

	hit1=-1;
	hit2=-1;
	//Ray starts inside the bbox
	EXPECT_TRUE(bbox.IntersectP(vec3(2,2,2),vec3(1,0,0),&hit1,&hit2));
	EXPECT_FLOAT_EQ(0.,hit1);
	EXPECT_FLOAT_EQ(1.,hit2);


}

//Test the maximumExtend method 
TEST(test_3d_bbox,test_maximum_extend)
{

	BBox bbox(vec3(-5,-1,-1),vec3(-2,1,1)); 
	EXPECT_EQ(0,bbox.MaximumExtend()); //max-min = (6,2,2) => x is the dominant direction

	bbox=BBox(vec3(1,-3,1),vec3(2,2,2));
	EXPECT_EQ(1,bbox.MaximumExtend()); //max-min = (1,2,1) => y is the dominant direction

	bbox=BBox(vec3(-5,-1,1),vec3(-2,1,10));
	EXPECT_EQ(2,bbox.MaximumExtend()); //max-min = (3,2,9) => z is the dominant direction


}
/**
 * \file test_OBox.cpp
 * \test Testing of OBox libs used by lot of TYMPAN features
 *
 *  Created on: april 21, 2013
 *      Author: Denis THOMASSON <denis.thomasson@edf.fr>
 */

#include <cstdlib>
#include <vector>
#include <map>
#include "gtest/gtest.h"

#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Diffraction.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/SpecularReflexion.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/DoNothing.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/PostTreatment.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/ValidRay.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Cylindre.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Triangle.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Scene.h"

using std::cout;
using std::cerr;
using std::endl;

/***********************************************************************
						        Diffraction
************************************************************************/

// Test the default constructor
TEST(test_diffraction,default_constructor){

	Diffraction diffraction;

	EXPECT_TRUE(vec3(0.,0.,0.)==diffraction.getPosition()); 
	EXPECT_TRUE(vec3(0.,0.,0.)==diffraction.getIncomingDirection()); 
	EXPECT_EQ(NULL,diffraction.getShape()); 
	EXPECT_EQ(200,diffraction.getInitialNbResponseLeft()); 
	EXPECT_EQ(200,diffraction.getNbResponseLeft()); 
	EXPECT_EQ(DIFFRACTION,diffraction.getType()); 

}

// Test the responseAngleLimiter() method
TEST(test_diffraction, response_angle_limiter)
{
	Diffraction diffraction;

	vec3 N1;
	vec3 N2;

	vec3 from=vec3(5,-4,9);
	vec3 T=from+vec3((decimal)0.01,0,0);
	from.normalize();
	T.normalize();
	EXPECT_TRUE(diffraction.responseValidator(from,N1,N2,T)); // from*t ~= 1. => T and from are almost colinear => true

	T=-from-vec3((decimal)0.01,0,0);
	T.normalize();
	EXPECT_FALSE(diffraction.responseValidator(from,N1,N2,T)); // from*t ~= -1. => T and from point in opposite directions => false

	from=vec3(1,-1,0);
	T=vec3(1,1,0);
	N1=vec3(-1,0,0);
	N2=vec3(0,1,0);
	EXPECT_FALSE(diffraction.responseValidator(from,N1,N2,T)); // from*N1 < 0 && from*N2 <0 => false

	from=vec3(-1,1,0);
	T=vec3(-1,-1,0);
	EXPECT_FALSE(diffraction.responseValidator(from,N1,N2,T)); // from*N1 > 0 && from*N2 > 0 => false

	from=vec3(5,1,0);
	T=vec3(1,1,0);
	from.normalize();
	T.normalize();
	EXPECT_FALSE(diffraction.responseValidator(from,N1,N2,T)); // F1 < 0 && ABS(T*N2)>ABS(F*N2) => false

	from=vec3(0,-1,0);
	T=vec3(-1,-1,0);
	T.normalize();
	EXPECT_FALSE(diffraction.responseValidator(from,N1,N2,T)); // F1 < 0 &&T*N1 > 0 => false

	from=vec3(-1,-5,0);
	T=vec3(-1,-1,0);
	from.normalize();
	T.normalize();
	EXPECT_FALSE(diffraction.responseValidator(from,N1,N2,T)); // F2 < 0 && ABS(T*N1)>ABS(F*N1) => false

	from=vec3(-1,0,0);
	T=vec3(-1,1,0);
	from.normalize();
	T.normalize();
	EXPECT_FALSE(diffraction.responseValidator(from,N1,N2,T)); // F2 < 0 && T*N2 > 0 => false

	from=vec3(-5,-1,0);
	T=vec3(-1,-1,0);
	from.normalize();
	T.normalize();
	EXPECT_TRUE(diffraction.responseValidator(from,N1,N2,T)); // Passed all tests => true

}


// Test the getTheta() method
TEST(test_diffraction, get_theta)
{
	Diffraction diffraction;
	decimal theta;

	diffraction.getTheta((decimal)M_PI/4,(decimal)M_PI/40, 10,theta);
	EXPECT_EQ((decimal)M_PI/8,theta);

	diffraction.getTheta((decimal)M_PI/4,(decimal)M_PI/40, 8,theta);
	EXPECT_EQ((decimal)(3*M_PI/40),theta);

	diffraction.getTheta((decimal)M_PI/2,(decimal)M_PI/30, 15,theta);
	EXPECT_EQ((decimal)M_PI/4,theta);

	diffraction.getTheta((decimal)M_PI/2,(decimal)M_PI/30, 7,theta);
	EXPECT_EQ((decimal)(-2*M_PI/120),theta);
}

// Test the getResponse() method
TEST(test_diffraction,get_response){

	//Create the cylinder used for the diffraction

	//Create the two shapes of the cylinder
	Triangle* s1=new Triangle();
	Triangle* s2=new Triangle();

	s1->setNormal(vec3(-1,0,0));
	s2->setNormal(vec3(0,1,0));

	vector<vec3> vertices;
	vertices.push_back(vec3(0,0,-5));
	vertices.push_back(vec3(0,0,5));

	Cylindre* cylindre=new Cylindre(s1,s2,&vertices,0,1,(decimal)0.2);

	// add a diffraction event
	vec3 from((decimal)cos(3*M_PI/8),(decimal)sin(3*M_PI/8),0);

    Diffraction diffraction(vec3(0,0,0),from,cylindre);
	diffraction.setNbResponseLeft(15);
	diffraction.setAngleOuverture((decimal)(3*M_PI/2));
	
	EXPECT_EQ((decimal)M_PI/2,diffraction.getAngle());
	EXPECT_EQ(14,diffraction.getNbResponseLeft());
	EXPECT_EQ((decimal)(3*M_PI/28),diffraction.getDeltaTheta());

	vec3 response;
	
	//We should get 4 valid responses
	EXPECT_TRUE(diffraction.getResponse(response));
	EXPECT_EQ(13,diffraction.getNbResponseLeft());
	EXPECT_TRUE(response==vec3((decimal)1.0000000 ,(decimal)2.9502795e-007,(decimal)4.3711388e-008));

	EXPECT_TRUE(diffraction.getResponse(response));
	EXPECT_EQ(12,diffraction.getNbResponseLeft());
	EXPECT_TRUE(response==vec3((decimal)0.94388330 ,(decimal)0.33027923,(decimal)4.3711388e-008));

	EXPECT_TRUE(diffraction.getResponse(response));
	EXPECT_EQ(11,diffraction.getNbResponseLeft());
	EXPECT_TRUE(response==vec3((decimal)0.78183144 ,(decimal)0.62348998,(decimal)4.3711388e-008));

	EXPECT_TRUE(diffraction.getResponse(response));
	EXPECT_EQ(10,diffraction.getNbResponseLeft());
	EXPECT_TRUE(response==vec3((decimal)0.53203201 ,(decimal)0.84672433,(decimal)4.3711388e-008));

	//5th response is not in the shadow zone and therefore should be filtered (aswell as all the subsequent reponses)
	EXPECT_FALSE(diffraction.getResponse(response));
	EXPECT_EQ(-1,diffraction.getNbResponseLeft());
}


/***********************************************************************
						        SpecularReflexion
************************************************************************/

// Test the default constructor
TEST(test_specular_reflexion,default_constructor){

	SpecularReflexion reflexion;

	EXPECT_TRUE(vec3(0.,0.,0.)==reflexion.getPosition()); 
	EXPECT_TRUE(vec3(0.,0.,0.)==reflexion.getIncomingDirection()); 
	EXPECT_EQ(NULL,reflexion.getShape()); 
	EXPECT_EQ(1,reflexion.getInitialNbResponseLeft()); 
	EXPECT_EQ(1,reflexion.getNbResponseLeft()); 
	EXPECT_EQ(SPECULARREFLEXION,reflexion.getType()); 


}

// Test the getAngle() method
TEST(test_specular_reflexion,get_angle){

	//Create the shape of the reflexion even
	Triangle* shape=new Triangle();

	//Set normal
	shape->setNormal(vec3(0,1,0));

	vec3 pos(0,0,0);
	vec3 from((decimal)1,(decimal)-1,0);
	from.normalize();

	SpecularReflexion reflexion(pos,from,shape);

	EXPECT_TRUE(abs((M_PI/4)-reflexion.getAngle())<EPSILON_6);

}

// Test the getResponse() method
TEST(test_specular_reflexion,get_response){

	//Create the two shapes of the cylinder
	Triangle* shape=new Triangle();

	shape->setNormal(vec3(0,1,0));

	vec3 pos(0,0,0);
	vec3 from((decimal)1,(decimal)-1,0);
	from.normalize();

	SpecularReflexion reflexion(pos,from,shape);

	vec3 response;
	reflexion.getResponse(response);
	vec3 expected_response((decimal)1,(decimal)1,0);
	expected_response.normalize();

	EXPECT_TRUE(expected_response==response);
}

/***********************************************************************
						        DoNothing
************************************************************************/

// Test the default constructor
TEST(test_do_nothing,default_constructor){

	DoNothing doNothing;

	EXPECT_TRUE(vec3(0.,0.,0.)==doNothing.getPosition()); 
	EXPECT_TRUE(vec3(0.,0.,0.)==doNothing.getIncomingDirection()); 
	EXPECT_EQ(NULL,doNothing.getShape()); 
	EXPECT_EQ(1,doNothing.getInitialNbResponseLeft()); 
	EXPECT_EQ(1,doNothing.getNbResponseLeft()); 
	EXPECT_EQ(NOTHING,doNothing.getType()); 

	vec3 response;

	EXPECT_TRUE(doNothing.getResponse(response)); 
	EXPECT_TRUE(vec3(0.,0.,0.)==response); 

	//get an 2nd response when only 1 responses is expectd => false
	EXPECT_FALSE(doNothing.getResponse(response)); 

}

// Test the getResponse() method
TEST(test_do_nothing,get_response){

	DoNothing doNothing;
	vec3 from((decimal)-1.4,(decimal)8.45,(decimal)-8.59);

	doNothing.setIncomingDirection(from);
	doNothing.setNbResponseLeft(10);

	vec3 response;

	int i=0;
	while(i<10){
		EXPECT_TRUE(doNothing.getResponse(response)); 
		EXPECT_TRUE(from==response); 
		i++;
	}

	//get an 11th response when only 10 responses are expectd => false
	EXPECT_FALSE(doNothing.getResponse(response)); 

}


/***********************************************************************
						        Material
************************************************************************/

// Test the Material object
TEST(test_material,material){

	MaterialManager manager;
	
	//Create materials and check their name
	Material* m1=new Material("Mat0");
	EXPECT_EQ("Mat0",m1->getName());

	std::string name_m2;
	Material* m2=new Material(name_m2);
	EXPECT_EQ("",m2->getName());

	Material* m3=new Material();
	EXPECT_EQ("unknown material",m3->getName());

	//Register materials and check the returned key
	EXPECT_EQ(0,manager.registerNewMaterial(m1));
	EXPECT_EQ(1,manager.registerNewMaterial(m2));
	EXPECT_EQ(2,manager.registerNewMaterial(m3));


	//Request materials by their key and check they are correct
	Material* r1=manager.requestMaterial(0);
	EXPECT_EQ(m1,r1);
	EXPECT_EQ("Mat0",r1->getName());

	Material* r2=manager.requestMaterial(1);
	EXPECT_EQ(m2,r2);
	EXPECT_EQ("Material 1",r2->getName());

	Material* r3=manager.requestMaterial(2);
	EXPECT_EQ(m3,r3);
	EXPECT_EQ("unknown material",r3->getName());
}


/***********************************************************************
						        Source
************************************************************************/

// Test the Source object
TEST(test_source,source){

	Source s1;
	s1.setInitialRayCount(100);

	EXPECT_EQ("unknown source",s1.getName());
	EXPECT_EQ(100,s1.getInitialRayCount());
	EXPECT_EQ(100,s1.getNbRayLeft());

	s1.setNbRayLeft(150);
	EXPECT_EQ(150,s1.getInitialRayCount()); // initialRayCount < 150  => initialRayCount = 150
	EXPECT_EQ(150,s1.getNbRayLeft());

	s1.setNbRayLeft(50);
	EXPECT_EQ(150,s1.getInitialRayCount()); // initialRayCount > 50 => initialRayCount does not change
	EXPECT_EQ(50,s1.getNbRayLeft());
}


/***********************************************************************
						        Spectre
************************************************************************/

// Test the Spectre object 
TEST(test_spectre,spectre){

	Spectre s;
	//test spectre's size
	EXPECT_EQ(0,s.getSizeSpectre());
	//add Frequencies
	s.addFrequencie(50,5.0);
	s.addFrequencie(100,10.0);
	s.addFrequencie(150,15.0);

	//test spectre's size
	EXPECT_EQ(3,s.getSizeSpectre());

	//test getFrequencie
	EXPECT_EQ(-1,s.getFrequencie(-1));
	EXPECT_EQ(50,s.getFrequencie(0));
	EXPECT_EQ(100,s.getFrequencie(1));
	EXPECT_EQ(150,s.getFrequencie(2));
	EXPECT_EQ(-1,s.getFrequencie(3));	

	//test getPower
	EXPECT_EQ(-200,s.getPower(-1));
	EXPECT_EQ(5.0,s.getPower(0));
	EXPECT_EQ(10.0,s.getPower(1));
	EXPECT_EQ(15.0,s.getPower(2));
	EXPECT_EQ(-200,s.getPower(3));	

}


/***********************************************************************
						        ValidRay
************************************************************************/

// Test the computeCumulDistance method 
TEST(test_valid_ray,compute_cumul_distance){


	Source* src=new Source();
	src->setPosition(vec3((decimal)1.7,(decimal)-4.15,(decimal)3.64));

	Ray* ray=new Ray();
	ray->source=src;

	vec3 impact((decimal)12.3,(decimal)-2.4,(decimal)-4.65);
	ValidRay::computeCumulDistance(ray,impact);
	EXPECT_EQ(impact.distance(src->getPosition()),ray->cumulDistance);

	//Create several events, some with the type NOTHING
	Event* e=new Event();
	e->setType(SPECULARREFLEXION);
	e->setPosition(vec3(-7.5,8.6,-12.41));
	std::shared_ptr<Event> SPE(e);
	ray->getEvents()->push_back(SPE);

	ValidRay::computeCumulDistance(ray,impact);
	EXPECT_EQ(impact.distance(src->getPosition())+impact.distance(e->getPosition()),ray->cumulDistance);
}

// Test the computeRealImpact method
TEST(test_valid_ray,compute_real_impact){

	//Create the cylinder used for the diffraction

	//Create the two shapes of the cylinder
	Triangle* s1=new Triangle();
	Triangle* s2=new Triangle();

	s1->setNormal(vec3(-1,0,0));
	s2->setNormal(vec3(0,1,0));

	vector<vec3> vertices;
	vertices.push_back(vec3(0,0,-5));
	vertices.push_back(vec3(0,0,5));

	//Create Cylinder
	Cylindre cylindre(s1,s2,&vertices,0,1,(decimal)0.2);

	//Create Ray
	vec3 pos(-1,1,0);
	vec3 from((decimal)1,(decimal)-1,0);
	from.normalize();
	Ray ray(pos,from);

	//Create intersection
	Intersection inter;
	
	vec3 impact;

	inter.t=1.0;
	EXPECT_TRUE(ValidRay::computeRealImpact(&ray,&inter,&cylindre,impact));
	EXPECT_TRUE(vec3(0,0,0)==impact); //ray should impact the middle of the cylinder's axis which is (0,0,0)


	vec3 from2((decimal)1,(decimal)-0.8,0.5);
	from2.normalize();
	ray.direction=from2;
	EXPECT_TRUE(ValidRay::computeRealImpact(&ray,&inter,&cylindre,impact));
	EXPECT_TRUE(vec3(0,0,0.54878056)==impact); //ray should impact the middle of the cylinder's axis which is (0,0,0)
		

	//cases that should return false
	inter.t=0; 
	EXPECT_FALSE(ValidRay::computeRealImpact(&ray,&inter,&cylindre,impact)); 

	//cylinder with 0 height
	vertices.clear();
	vertices.push_back(vec3(0,0,5));
	vertices.push_back(vec3(0,0,5));
	cylindre.setVertices(&vertices); 
	
	inter.t=1.0; 
	EXPECT_FALSE(ValidRay::computeRealImpact(&ray,&inter,&cylindre,impact));  

}


// Test the pathDiffValidationForReflection method 
TEST(test_valid_ray,path_diff_validation_for_reflection){

}

// Test the validRayWithDoNothingEvent method 
TEST(test_valid_ray,valid_ray_with_do_nothing_event){

	vec3 pos(-1,1,0);
	vec3 from((decimal)1,(decimal)-1,0);
	from.normalize();

	//Create ray
	Ray* ray=new Ray(pos,from);

	Intersection* inter=new Intersection();

	//Create the shape of the event
	Triangle* shape=new Triangle();
	shape->setNormal(vec3(0,1,0));

	inter->t=(decimal)1.4142135624;
	inter->p=shape;

	vec3 expected_impact=(ray->position+ray->direction*inter->t);
	EXPECT_TRUE(ValidRay::validRayWithDoNothingEvent(ray,inter));
	EXPECT_TRUE(expected_impact==vec3(0,0,0));
	EXPECT_TRUE(ray->direction==from);
}


// Test the validTriangleWithSpecularReflexion method 
TEST(test_valid_ray,valid_ray_with_specular_reflexion){

	vec3 pos(-1,1,0);
	vec3 from((decimal)1,(decimal)-1,0);
	from.normalize();

	//Create ray
	Ray* ray=new Ray(pos,from);

	Intersection* inter=new Intersection();

	//Create the shape of the event
	Triangle* shape=new Triangle();
	shape->setNormal(vec3(0,1,0));

	inter->t=(decimal)1.4142135624;
	inter->p=shape;

	vec3 expected_impact=(ray->position+ray->direction*inter->t);
	EXPECT_TRUE(ValidRay::validRayWithDoNothingEvent(ray,inter));
	EXPECT_TRUE(expected_impact==vec3(0,0,0));
	EXPECT_TRUE(ray->direction==from);
}
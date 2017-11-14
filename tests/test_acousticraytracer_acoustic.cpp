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
#include "Tympan/geometric_methods/AcousticRaytracer/Engine/AcousticRaytracerConfiguration.h"

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

	SpecularReflexion reflection;

	EXPECT_TRUE(vec3(0.,0.,0.)==reflection.getPosition()); 
	EXPECT_TRUE(vec3(0.,0.,0.)==reflection.getIncomingDirection()); 
	EXPECT_EQ(NULL,reflection.getShape()); 
	EXPECT_EQ(1,reflection.getInitialNbResponseLeft()); 
	EXPECT_EQ(1,reflection.getNbResponseLeft()); 
	EXPECT_EQ(SPECULARREFLEXION,reflection.getType()); 


}

// Test the getAngle() method
TEST(test_specular_reflexion,get_angle){

	//Create the shape of the reflection even
	Triangle* shape=new Triangle();

	//Set normal
	shape->setNormal(vec3(0,1,0));

	vec3 pos(0,0,0);
	vec3 from((decimal)1,(decimal)-1,0);
	from.normalize();

	SpecularReflexion reflection(pos,from,shape);

	EXPECT_TRUE(abs((M_PI/4)-reflection.getAngle())<EPSILON_6);

}

// Test the getResponse() method
TEST(test_specular_reflexion,get_response){

	//Create the two shapes of the cylinder
	Triangle* shape=new Triangle();

	shape->setNormal(vec3(0,1,0));

	vec3 pos(0,0,0);
	vec3 from((decimal)1,(decimal)-1,0);
	from.normalize();

	SpecularReflexion reflection(pos,from,shape);

	vec3 response;
	reflection.getResponse(response);
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
	EXPECT_EQ((decimal)13.570063,ray->cumulDistance);

	
	Event* e=new Event();
	e->setType(SPECULARREFLEXION);
	e->setPosition(vec3((decimal)-7.5,(decimal)8.6,(decimal)-12.41));
	std::shared_ptr<Event> SPE(e);
	ray->getEvents()->push_back(SPE);

	ValidRay::computeCumulDistance(ray,impact);
	EXPECT_EQ((decimal)37.512863,ray->cumulDistance);
}

// Test the computeRealImpact method (trivial case)
TEST(test_valid_ray,compute_real_impact_trivial){


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
	vec3 direction((decimal)1,(decimal)-1,0);
	direction.normalize();
	Ray ray(pos,direction);

	//Create intersection
	Intersection inter;
	
	vec3 impact;

	inter.t=1.0;
	EXPECT_TRUE(ValidRay::computeRealImpact(&ray,&inter,&cylindre,impact));
	EXPECT_TRUE(vec3(0,0,0)==impact); //ray should impact the middle of the cylinder's axis which is (0,0,0)


	direction=vec3(1,-1,(decimal)0.8);
	direction.normalize();
	ray.direction=direction;
	EXPECT_TRUE(ValidRay::computeRealImpact(&ray,&inter,&cylindre,impact));
	EXPECT_TRUE(vec3(0,0,(decimal)0.79999983)==impact); //ray hits the the cylinder 

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

// Test the computeRealImpact method (general case)
TEST(test_valid_ray,compute_real_impact_general){

	//Create the two shapes of the cylinder
	Triangle* s1=new Triangle();
	Triangle* s2=new Triangle();

	s1->setNormal(vec3(-1,0,0));
	s2->setNormal(vec3(0,1,0));

	vec3();

	vector<vec3> vertices;
	vertices.push_back(vec3((decimal)-0.099667764743042,(decimal)-2.427139455191618,(decimal)0.248449229997744));
	vertices.push_back(vec3((decimal)-4.464091294766189,(decimal)-4.546406845194255,(decimal)1.456966081453157));

	//Create Cylinder
	Cylindre cylindre(s1,s2,&vertices,0,1,(decimal)0.2);

	//Create Ray
	vec3 direction((decimal)-0.711853043902896,(decimal)0.926304592065416,(decimal)4.861607249316291);
	vec3 pos((decimal)-0.744131766911425,(decimal)-3.262495453200911,(decimal)0.494264174913020);
	direction.normalize();
	Ray ray(pos,direction);

	//Create intersection
	Intersection inter;
	
	vec3 impact;

	inter.t=1.0;
	EXPECT_TRUE(ValidRay::computeRealImpact(&ray,&inter,&cylindre,impact));
	EXPECT_TRUE(vec3((decimal)-0.97770512,(decimal)-2.8534949,(decimal)0.49157938 )==impact); //ray should impact the middle of the cylinder's axis which is (0,0,0)

}

// Test the isRayPassesNearRidge method 
TEST(test_valid_ray,is_ray_passes_near_ridge){

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

	//Create Source
	Source src;
	src.setPosition(vec3(0,0,0));

	//Create Ray
	vec3 pos(-1,1,0);
	vec3 direction((decimal)1,(decimal)-1,0);
	direction.normalize();
	Ray ray(pos,direction);
	ray.source=&src;
	
	// Create and add a diffraction 
	vec3 from=vec3(10,-5,7);
	from.normalize();
	Diffraction* diff=new Diffraction(vec3(10,-5,7),&from,&cylindre);
	std::shared_ptr<Event> SPE(diff);
	ray.getEvents()->push_back(SPE);

	vec3 impact(vec3(20,-10,14));

	//real impact within the thickness of the ray
	EXPECT_TRUE(ValidRay::isRayPassesNearRidge(&ray,&impact,vec3(20,-9.5,14)));
	EXPECT_TRUE(ValidRay::isRayPassesNearRidge(&ray,&impact,vec3(19.5,-10,14)));
	EXPECT_TRUE(ValidRay::isRayPassesNearRidge(&ray,&impact,vec3(20,-10,14.5)));

	//real impact outside the thickness of the ray
	EXPECT_FALSE(ValidRay::isRayPassesNearRidge(&ray,&impact,vec3(20,-9,14)));
	EXPECT_FALSE(ValidRay::isRayPassesNearRidge(&ray,&impact,vec3(18,-10,14)));
	EXPECT_FALSE(ValidRay::isRayPassesNearRidge(&ray,&impact,vec3(20,-10,13)));
}


// Test the pathDiffValidationForReflection method 
TEST(test_valid_ray,path_diff_validation_for_reflection){

	//Create Ray
	Ray* ray=new Ray();

	 // Add several events with different types
	SpecularReflexion* reflection = new SpecularReflexion();
	reflection->setPosition(vec3(-8,12,-4));
    std::shared_ptr<Event> SPE(reflection);
    ray->getEvents()->push_back(SPE);

	//init cumulDistance
	ray->cumulDistance=(decimal)20.;
	
	// set maxPathDiffrence 
	AcousticRaytracerConfiguration::get()->MaxPathDifference=(decimal)20.;

	//distance between impact and last reflection D=0
	EXPECT_TRUE(ValidRay::pathDiffValidationForReflection(ray,vec3(-8,12,-4)));
	EXPECT_FLOAT_EQ(20.,ray->cumulDelta); // D=0 => cumulDelta should not change
	EXPECT_FLOAT_EQ(0.,ray->cumulDistance); //cumulDistance should be reseted to 0


	// set maxPathDiffrence lower than the current cumulDelta
	AcousticRaytracerConfiguration::get()->MaxPathDifference=(decimal)19.;

	//distance between impact and last reflection D=0
	EXPECT_FALSE(ValidRay::pathDiffValidationForReflection(ray,vec3(-8,12,-4)));
	EXPECT_FLOAT_EQ(20.,ray->cumulDelta); //D=0 => cumulDelta should not change.
	EXPECT_FLOAT_EQ(0.,ray->cumulDistance); //cumulDistance should be reseted to 0
	

	//increase maxPathDiffrence
	AcousticRaytracerConfiguration::get()->MaxPathDifference=(decimal)25.;

	
	ray->cumulDistance=(decimal)5.;

	//distance between impact and last reflection D=5
	EXPECT_TRUE(ValidRay::pathDiffValidationForReflection(ray,vec3(-3,12,-4)));
	EXPECT_FLOAT_EQ(25.,ray->cumulDelta); //cumulDelta should increase by value of cumulDistance before the call to cumulDistance (5.0).
	EXPECT_FLOAT_EQ(0.,ray->cumulDistance); //cumulDistance should be reseted to 0


	ray->cumulDistance=(decimal)5.;

	//distance between impact and last reflection D=5
	EXPECT_FALSE(ValidRay::pathDiffValidationForReflection(ray,vec3(-3,12,-4)));
	EXPECT_FLOAT_EQ(30.,ray->cumulDelta);  //cumulDelta should increase by value of cumulDistance before the call to cumulDistance (5.0).
	EXPECT_FLOAT_EQ(0.,ray->cumulDistance); //cumulDistance should be reseted to 0

}


// Test the pathDiffValidationForDiffraction method 
TEST(test_valid_ray,path_diff_validation_for_diffraction){

	//Create Ray
	Ray* ray=new Ray();

	//Add several events with different types
	SpecularReflexion* reflection = new SpecularReflexion();
	reflection->setPosition(vec3(-8,12,-4));
    std::shared_ptr<Event> SPE(reflection);
    ray->getEvents()->push_back(SPE);

	//Set maxPathDiffrence 
	AcousticRaytracerConfiguration::get()->MaxPathDifference=(decimal)20.;


	ray->cumulDelta=10.;
	ray->cumulDistance=10;

	//distance between impact and last reflection D=10.0
	EXPECT_TRUE(ValidRay::pathDiffValidationForDiffraction(ray,vec3(-8,2,-4))); //cumulDistance-D+cumulDelta = 20-10+10
	EXPECT_FLOAT_EQ(10.,ray->cumulDelta); //cumulDelta should no be modified
	EXPECT_FLOAT_EQ(20,ray->cumulDistance); //cumulDistance should increase by D

	//distance between impact and last reflection D=5.0
	EXPECT_FALSE(ValidRay::pathDiffValidationForDiffraction(ray,vec3(-3,12,-4))); //cumulDistance-D+cumulDelta = 25-5+10 > 20
	EXPECT_FLOAT_EQ(10.,ray->cumulDelta); //cumulDelta should no be modified
	EXPECT_FLOAT_EQ(25.,ray->cumulDistance); //cumulDistance should increase by D

	//distance between impact and last reflection D=10.0
	EXPECT_FALSE(ValidRay::pathDiffValidationForDiffraction(ray,vec3(-8,2,-4))); //cumulDistance-D+cumulDelta = 35-10+10 > 20 
	EXPECT_FLOAT_EQ(10.,ray->cumulDelta); //cumulDelta should no be modified
	EXPECT_FLOAT_EQ(35.,ray->cumulDistance); //cumulDistance should increase by D

}

// Test the pathDiffValidationForReflection and pathDiffValidationForDiffraction together 
TEST(test_valid_ray,path_diff_validation){

	//Create Source
	Source* src=new Source();
	src->setPosition(vec3(-3,12,-4));

	//Create Ray
	Ray* ray=new Ray();
	ray->source=src;

	//Create two events
	SpecularReflexion* reflection1 = new SpecularReflexion();
	reflection1->setPosition(vec3(-8,12,-4));
    std::shared_ptr<Event> SPE1(reflection1);
  
	Diffraction* diffraction= new Diffraction();
	diffraction->setPosition(vec3(-8,2,-4));
    std::shared_ptr<Event> SPE2(diffraction);
 
  
	//Set maxPathDiffrence 
	AcousticRaytracerConfiguration::get()->MaxPathDifference=(decimal)5.;

	//Initially cumulDelta and cumulDistance should equal 0
	EXPECT_FLOAT_EQ(0.,ray->cumulDelta); 
	EXPECT_FLOAT_EQ(0.,ray->cumulDistance); 

	//Test if a reflection event with position (-8,12,-4) can be added. distance between impact and source D=5.0
	EXPECT_TRUE(ValidRay::pathDiffValidationForReflection(ray,vec3(-8,12,-4)));
	EXPECT_FLOAT_EQ(0.,ray->cumulDelta); //cumulDelta should not change because cumulDistance == 0.
	EXPECT_FLOAT_EQ(0.,ray->cumulDistance); //cumulDistance should be reseted to 0

	//add first event
	ray->getEvents()->push_back(SPE1);

	//Test if a diffraction event with position (-8,2,-4) can be added. distance between impact and last reflection D=10.0
	EXPECT_TRUE(ValidRay::pathDiffValidationForDiffraction(ray,vec3(-8,2,-4)));
	EXPECT_FLOAT_EQ(0.,ray->cumulDelta); //cumulDelta should not change for diffraction validation.
	EXPECT_FLOAT_EQ(10.,ray->cumulDistance); //cumulDistance should increase by D
	
	//add second event
	ray->getEvents()->push_back(SPE2);

	//Test if a reflection event with position (-8,7,-4) can be added. distance between impact and last reflection D=5.0
	EXPECT_FALSE(ValidRay::pathDiffValidationForReflection(ray,vec3(-8,7,-4))); //cumulDelta = cumulDistance -D = 10 > MaxPathDifference => return false
	EXPECT_FLOAT_EQ(10.,ray->cumulDelta); //cumulDelta increase by D => 10..
	EXPECT_FLOAT_EQ(0.,ray->cumulDistance); //cumulDistance should be reset to 0


}


// Test the validTriangleWithSpecularReflexion method 
TEST(test_valid_ray,valid_triangle_with_specular_reflexion){

	//Create Source
	Source* src=new Source();
	src->setPosition(vec3(-3,12,-4));

	//Create Ray
	Ray* ray=new Ray();
	ray->source=src;
	ray->position=src->getPosition();
	vec3 direction=vec3(-5,-10,0);
	direction.normalize();
	ray->direction=direction;

	//Create a shape for the intersection
	Triangle* shape=new Triangle();
	shape->setNormal(direction);

	//Create Intersection
	Intersection* inter=new Intersection();
	inter->t=1.;
	inter->p=shape;

  
	EXPECT_FALSE(ValidRay::validTriangleWithSpecularReflexion(ray,inter)); //ray goes in same direction as the normal of the shape => return false
	EXPECT_TRUE(ray->getEvents()->empty()); //ray's event list should remain empty

	//change shape's normal
	shape->setNormal(vec3(5,10,0));

	EXPECT_TRUE(ValidRay::validTriangleWithSpecularReflexion(ray,inter)); 
	EXPECT_EQ(1,ray->getEvents()->size()); //ray's event list should contain one event
	EXPECT_EQ(SPECULARREFLEXION,ray->getEvents()->at(0)->getType()); //the event should be a SPECULARREFLEXION
	EXPECT_EQ(1,ray->nbReflexion); //nbReflexion should have been incremented
	EXPECT_EQ(0,ray->nbDiffraction); //nbDiffraction should remain the same
}


// Test the validCylindreWithDiffraction method 
TEST(test_valid_ray,valid_cylindre_with_diffraction){

	//Create Source
	Source* src=new Source();
	src->setPosition(vec3(-2,1,0));
	Sampler sampler;
	src->setSampler(&sampler);

	//Create Ray
	Ray* ray=new Ray();
	ray->source=src;
	ray->position=src->getPosition();
	vec3 direction=vec3(0,0,0);
	direction.normalize();
	ray->direction=direction;

	//Create the two shapes of the cylinder
	Triangle* s1=new Triangle();
	Triangle* s2=new Triangle();
	s1->setNormal(vec3(-1,0,0));
	s2->setNormal(vec3(0,1,0));

	//Create Cylinder
	vector<vec3> vertices;
	vertices.push_back(vec3(0,0,-5));
	vertices.push_back(vec3(0,0,5));
	Cylindre cylindre(s1,s2,&vertices,0,1,(decimal)0.2);

	//Create Intersection
	Intersection* inter=new Intersection();
	inter->t=(decimal)1;
	inter->p=&cylindre;
 


	EXPECT_FALSE(ValidRay::validCylindreWithDiffraction(ray,inter)); //ray's direction has norm 0, hence no possible line intersection with the cylinder's axis => return false
	EXPECT_TRUE(ray->getEvents()->empty()); //ray's event list should remain empty
	EXPECT_TRUE(vec3(-2,1,0)==ray->position); //ray's position should not change
    
	
	//change ray's direction 
	direction=vec3(1,0,0);
	direction.normalize();
	ray->direction=direction;
	
	EXPECT_TRUE(ValidRay::validCylindreWithDiffraction(ray,inter)); //ray does not pass near enough the diffraction edge => call validRayWithDoNothingEvent
	EXPECT_EQ(1,ray->getEvents()->size()); //ray's event list should contain one event
	EXPECT_EQ(NOTHING,ray->getEvents()->at(0)->getType()); //the last event should be a NOTHING
	EXPECT_EQ(0,ray->nbReflexion); //nbReflexion should remain the same
	EXPECT_EQ(0,ray->nbDiffraction); //nbDiffraction should remain the same
	EXPECT_TRUE(vec3(-1,1,0)==ray->position); //expected position of the ray after validCylindreWithDiffraction


	//change ray's direction and intersection distance
	direction=vec3(1,-1,0);
	direction.normalize();
	ray->direction=direction;
	inter->t=(decimal)1.4142135624;

	EXPECT_TRUE(ValidRay::validCylindreWithDiffraction(ray,inter)); 
	EXPECT_EQ(2,ray->getEvents()->size()); //ray's event list should contain two events
	EXPECT_EQ(DIFFRACTION,ray->getEvents()->at(1)->getType()); //the last event should be a DIFFRACTION
	EXPECT_EQ(0,ray->nbReflexion); //nbReflexion should remain the same
	EXPECT_EQ(1,ray->nbDiffraction); //nbDiffraction should have been incremented
	EXPECT_TRUE(vec3(0,0,0)==ray->position); //expected position of the ray after validCylindreWithDiffraction
	
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
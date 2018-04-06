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
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Cylindre.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Triangle.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Source.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Recepteur.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Event.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Diffraction.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/SpecularReflexion.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Ray/Ray.h"

using std::cout;
using std::cerr;
using std::endl;


// Test the computeEventSequenceLength() method
TEST(test_ray, compute_event_sequence_length)
{
    Ray* ray = new Ray();

    Source* src = new Source();
    ray->source = src;

    Recepteur* rcpt = new Recepteur();
    ray->recepteur = (void*) rcpt;


	//no event => length = 0 
	EXPECT_EQ(0,ray->computeEventsSequenceLength()); 


    // add one event
    Event* ev0 = new Event();
    ev0->setPosition(vec3(10, 0, 0));
    std::shared_ptr<Event> SPE0(ev0);
    ray->getEvents()->push_back(SPE0);

	//1 event => length = 0
	EXPECT_EQ(0,ray->computeEventsSequenceLength());  


    // add a second event
    Event* ev1 = new Event();
    ev1->setPosition(vec3(20, 0, 10));
    std::shared_ptr<Event> SPE1(ev1);
    ray->getEvents()->push_back(SPE1);

	//2 events => length = dist(ev0,ev1)
	decimal dist=ev0->getPosition().distance(ev1->getPosition());
	EXPECT_FLOAT_EQ(dist,ray->computeEventsSequenceLength()); 


    // add a third event
    Event* ev2 = new Event();
    ev2->setPosition(vec3(25, 0, 7));
    std::shared_ptr<Event> SPE2(ev2);
    ray->getEvents()->push_back(SPE2);

	//3 events => length = dist(ev0,ev1) + dist(ev1,ev2)
	dist+=ev1->getPosition().distance(ev2->getPosition());
	EXPECT_FLOAT_EQ(dist,ray->computeEventsSequenceLength()); 


    // add a fourth event
    Event* ev3 = new Event();
    ev3->setPosition(vec3(50, 0, 3));
    std::shared_ptr<Event> SPE3(ev3);
    ray->getEvents()->push_back(SPE3);

	//4 events => length = dist(ev0,ev1) + dist(ev1,ev2) + dist(ev2,ev3)
	dist+=ev2->getPosition().distance(ev3->getPosition());
	EXPECT_FLOAT_EQ(dist,ray->computeEventsSequenceLength()); 

}

// Test the length of the ray
TEST(test_ray, compute_length)
{
    Ray* ray = new Ray();

    Source* src = new Source();
	src->setPosition(vec3(0,0,0));
    ray->source = src;

    Recepteur* rcpt = new Recepteur();
	rcpt->setPosition(vec3(30,0,0));

    ray->computeLongueur();

	//No event and no receptor => length=0
	EXPECT_EQ(0,ray->getLongueur()); 

	ray->recepteur = (void*) rcpt; //add receptor
	ray->computeLongueur();

	//No event => length = dist(src,rcpt)
	EXPECT_FLOAT_EQ(src->getPosition().distance(rcpt->getPosition()),ray->getLongueur()); 

 

    // Add one event
    Event* ev0 = new Event();
	ev0->setPosition(vec3(10, 0, 0));
    std::shared_ptr<Event> SPE0(ev0);
    ray->getEvents()->push_back(SPE0);

	//remove the receptor from the ray
	ray->recepteur = NULL; 
	ray->computeLongueur();

	//1 event and no receptor => length = dist(src,ev0)
	EXPECT_FLOAT_EQ(src->getPosition().distance(ev0->getPosition()),ray->getLongueur()); 

	//set the ray's receptor 
	ray->recepteur = (void*) rcpt; 
	ray->computeLongueur();

	//1 event  => length = dist(src,ev0) + dist(ev0,rcpt)
	EXPECT_FLOAT_EQ(src->getPosition().distance(ev0->getPosition())
			 +ev0->getPosition().distance(rcpt->getPosition()),
			 ray->getLongueur()); 




    // add a second event 
    Event* ev1 = new Event();
	ev1->setPosition(vec3(20, 0, 10));
    std::shared_ptr<Event> SPE1(ev1);
    ray->getEvents()->push_back(SPE1);

	ray->computeLongueur();

	//2 events  => length = dist(src,ev0) +  dist(ev0,ev1) +  dist(ev1,rcpt)
	EXPECT_FLOAT_EQ(src->getPosition().distance(ev0->getPosition())
			+ev0->getPosition().distance(ev1->getPosition())
			+ev1->getPosition().distance(rcpt->getPosition()),
			ray->getLongueur()); 

  

}


// Test the computeTrueLength() method
TEST(test_ray, compute_true_length)
{
    Ray* ray = new Ray();

    Source* src = new Source();
	src->setPosition(vec3(0,0,0));
    ray->source = src;

    Recepteur* rcpt = new Recepteur();
	rcpt->setPosition(vec3(30,0,0));
	ray->recepteur = (void*) rcpt;

	vec3 closestPoint;
	decimal true_length=ray->computeTrueLength(vec3(10,5,0),rcpt->getPosition(),closestPoint);

	//no events => closestPoint is the closest point from vec3(10,5,0) on the horizontal line scr(0,0,0)-rcpt(30,0,0) => vec3(10,0,0)
	ASSERT_TRUE((vec3(10,0,0)==closestPoint)==1); 
	EXPECT_FLOAT_EQ(10.0,true_length); // true length = dist(src, closestPoint)
 


    // Add one event
    Event* ev0 = new Event();
	ev0->setPosition(vec3(10, 0, 20));
    std::shared_ptr<Event> SPE0(ev0);
    ray->getEvents()->push_back(SPE0);

	true_length=ray->computeTrueLength(vec3(15,0,10),rcpt->getPosition(),closestPoint);

	//1 event =>  closestPoint is the closest point from vec3(15,0,10) on the line ev0(10,0,20)-rcpt(30,0,0) => vec3(17.5,0,12.5)
	ASSERT_TRUE((vec3(17.5,0,12.5)==closestPoint)==1); 

	EXPECT_FLOAT_EQ(src->getPosition().distance(ev0->getPosition()) // true length = dist(src, first_event=ev0)	
		      +closestPoint.distance(ev0->getPosition())	  //			 + dist(last_event=ev0,closestPoint)
			  ,true_length);



     // add a second event 
    Event* ev1 = new Event();
	ev1->setPosition(vec3(20, 0, 10));
    std::shared_ptr<Event> SPE1(ev1);
    ray->getEvents()->push_back(SPE1);

	true_length=ray->computeTrueLength(vec3(25,5,0),rcpt->getPosition(),closestPoint);

	//1 event => closestPoint is the closest point from vec3(25,5,0) on the line ev1(20,0,10)-rcpt(30,0,0) => vec3(27.5,0,2.5)
	ASSERT_TRUE((vec3(27.5,0,2.5)==closestPoint)==1); 

	EXPECT_FLOAT_EQ(src->getPosition().distance(ev0->getPosition()) // true length = dist(src, first_event=ev0)		
			  +ev0->getPosition().distance(ev1->getPosition())//			 + ray->computeEventsSequenceLength() = dist(ev0,ev1)  
		      +closestPoint.distance(ev1->getPosition())	  //			 + dist(last_event=ev1,closestPoint)
			  ,true_length);
}


// Test the getLastPertinentEventOrSource() method
TEST(test_ray, get_last_pertinent_event_or_source)
{
	Ray* r=new Ray();

	Source* src = new Source();
	src->setPosition(vec3(0,0,0));
	r->source=src;

	Base* res=r->getLastPertinentEventOrSource();
	ASSERT_TRUE(res==src); // no event => should return the source

	// Add several events with different types
	Event* e1=new Event();
	e1->setType(SPECULARREFLEXION);
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	Event* e2=new Event();
	e2->setType(DIFFRACTION);
	std::shared_ptr<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	SpecularReflexion* e3=new SpecularReflexion();
	e3->setType(SPECULARREFLEXION);
	std::shared_ptr<Event> SPE3(e3);
	r->getEvents()->push_back(SPE3);

	res=r->getLastPertinentEventOrSource(); //pertinent event type is DIFFRACTION by default
	ASSERT_TRUE(res==e2); // return last DIFFRACTION event which is e2
	
	res=r->getLastPertinentEventOrSource(SPECULARREFLEXION); 
	ASSERT_TRUE(res==e3); // return last SPECULARREFLEXION event which is e3
}


// Test the computePertinentLength() method
TEST(test_ray, compute_pertinent_length)
{
	Ray* ray = new Ray();

    Source* src = new Source();
	src->setPosition(vec3(0,0,0));
    ray->source = src;

    Recepteur* rcpt = new Recepteur();
	rcpt->setPosition(vec3(30,0,0));
	ray->recepteur = (void*) rcpt;

	vec3 closestPoint;
	decimal pertinent_length=ray->computePertinentLength(vec3(10,5,0),rcpt->getPosition(),closestPoint);

	//no events => closestPoint is the closest point from vec3(10,5,0) on the horizontal line scr(0,0,0)-rcpt(30,0,0) => vec3(10,0,0)
	ASSERT_TRUE((vec3(10,0,0)==closestPoint)==1); 
	EXPECT_FLOAT_EQ(ray->computeTrueLength(vec3(10,5,0),rcpt->getPosition(),closestPoint),pertinent_length); // no event => pertinent length == true length
 


    // Add one non pertinent event (SPECULARREFLEXION)
    Event* ev0 = new Event();
	ev0->setPosition(vec3(10, 0, 20));
	ev0->setType(SPECULARREFLEXION);
    std::shared_ptr<Event> SPE0(ev0);
    ray->getEvents()->push_back(SPE0);


	pertinent_length=ray->computePertinentLength(vec3(15,0,10),rcpt->getPosition(),closestPoint);

	//closestPoint is the closest point from vec3(15,0,10) on the line ev0(20,0,10)-rcpt(30,0,0) => (vec3(17.5,0,12.5)
	ASSERT_TRUE((vec3(17.5,0,12.5)==closestPoint)==1); 
	EXPECT_FLOAT_EQ(ray->computeTrueLength(vec3(15,0,10),rcpt->getPosition(),closestPoint),pertinent_length); // no pertinent event => pertinent length == true length



    // add a pertient event (DIFFRACTION)
    Event* ev1 = new Event();
	ev1->setPosition(vec3(20, 0, 10));
	ev1->setType(DIFFRACTION);
    std::shared_ptr<Event> SPE1(ev1);
    ray->getEvents()->push_back(SPE1);

	pertinent_length=ray->computePertinentLength(vec3(25,5,0),rcpt->getPosition(),closestPoint);

	//closestPoint is the closest point from vec3(25,5,0) on the line ev1(20,0,10)-rcpt(30,0,0) => vec3(27.5,0,2.5)
	ASSERT_TRUE((vec3(27.5,0,2.5)==closestPoint)==1); 

	EXPECT_FLOAT_EQ(ev1->getPosition().distance(ev1->getPosition()) // pertinent length = dist(last_pertinent_event = ev1, last_event = ev1)= 0	  
		      +closestPoint.distance(ev1->getPosition())	  //			 + dist(lastevent=ev1,closestPoint)
			  ,pertinent_length);


	// Add one non pertinent event (SPECULARREFLEXION)
    Event* ev2 = new Event();
	ev2->setPosition(vec3(26, 8, 8));
	ev2->setType(SPECULARREFLEXION);
    std::shared_ptr<Event> SPE2(ev2);
    ray->getEvents()->push_back(SPE2);

	pertinent_length=ray->computePertinentLength(vec3(28,6,2),rcpt->getPosition(),closestPoint);

	//closestPoint is the closest point from vec3(28,6,2) on the line ev2(26, 8, 8)-rcpt(30,0,0) => vec3(28,4,4)
	ASSERT_TRUE((vec3(28, 4, 4)==closestPoint)==1); 

	EXPECT_FLOAT_EQ(ev1->getPosition().distance(ev2->getPosition()) // pertinent length = dist(last_pertinent_event = ev1, last_event = ev2)	  
		      +closestPoint.distance(ev2->getPosition())	  //			 + dist(lastevent=ev2,closestPoint)
			  ,pertinent_length);
}

// Test the getFaceHistory() method
TEST(test_ray, get_face_history)
{

	Ray* ray = new Ray();

    Source* src = new Source();
    src->setId(1536);
    ray->source = src;

    Recepteur* rcpt = new Recepteur();
    rcpt->setId(5435);
    ray->recepteur = (void*) rcpt;


	Shape* shape1= new Shape();
	shape1->setFaceId(452);
	Shape* shape2= new Shape();
	shape2->setFaceId(753);
	Shape* shape3= new Shape();
	shape3->setFaceId(972);


    // add one event
    Event* ev0 = new Event();
	ev0->setShape(shape1);
    std::shared_ptr<Event> SPE0(ev0);
    ray->getEvents()->push_back(SPE0);

    // add a second event
    Event* ev1 = new Event();
	ev1->setShape(shape2);
    std::shared_ptr<Event> SPE1(ev1);
    ray->getEvents()->push_back(SPE1);

    // add a third event
    Event* ev2 = new Event();
	ev2->setShape(shape3);
    std::shared_ptr<Event> SPE2(ev2);
    ray->getEvents()->push_back(SPE2);


	vector<unsigned int> faceHistory=ray->getFaceHistory();

	EXPECT_DOUBLE_EQ(1536,faceHistory.at(0)); //source
	EXPECT_DOUBLE_EQ(452,faceHistory.at(1));  //first event
	EXPECT_DOUBLE_EQ(753,faceHistory.at(2));  //second event
	EXPECT_DOUBLE_EQ(972,faceHistory.at(3));  //third event
	EXPECT_DOUBLE_EQ(5435,faceHistory.at(4)); //receptor
}


// Test the getFaceHistory() method
TEST(test_ray, get_primitivee_history)
{

	Ray* ray = new Ray();

    Source* src = new Source();
    src->setId(1536);
    ray->source = src;

    Recepteur* rcpt = new Recepteur();
    rcpt->setId(5435);
    ray->recepteur = (void*) rcpt;


	Shape* shape1= new Shape();
	shape1->setPrimitiveId(452);
	Shape* shape2= new Shape();
	shape2->setPrimitiveId(753);
	Shape* shape3= new Shape();
	shape3->setPrimitiveId(972);


    // add one event
    Event* ev0 = new Event();
	ev0->setShape(shape1);
    std::shared_ptr<Event> SPE0(ev0);
    ray->getEvents()->push_back(SPE0);

    // add a second event
    Event* ev1 = new Event();
	ev1->setShape(shape2);
    std::shared_ptr<Event> SPE1(ev1);
    ray->getEvents()->push_back(SPE1);

    // add a third event
    Event* ev2 = new Event();
	ev2->setShape(shape3);
    std::shared_ptr<Event> SPE2(ev2);
    ray->getEvents()->push_back(SPE2);


	vector<unsigned int> primitiveHistory=ray->getPrimitiveHistory();

	EXPECT_EQ(1536,primitiveHistory.at(0)); //source
	EXPECT_EQ(452,primitiveHistory.at(1));  //first event
	EXPECT_EQ(753,primitiveHistory.at(2));  //second event
	EXPECT_EQ(972,primitiveHistory.at(3));  //third event
	EXPECT_EQ(5435,primitiveHistory.at(4)); //receptor
}


// Test the getSolidAngle() method
TEST(test_ray, get_solid_angle)
{
	Ray* ray = new Ray();
	  
	Source* src = new Source();
	src->setInitialRayCount(10000);
    ray->source = src;
	
	bool diffraction=false;
	decimal solidAngle=ray->getSolidAngle(diffraction);

	EXPECT_FLOAT_EQ(M_4PI/10000,solidAngle); // no diffraction => solidAngle = area of the unit sphere / number of rays

	diffraction=true;
	solidAngle=ray->getSolidAngle(diffraction);
	EXPECT_FLOAT_EQ(M_4PI/10000,solidAngle); // diffraction == true but no diffraction event => solidAngle = area of the unit sphere / number of rays


	//Create the cylinder used for the diffraction

	//Create the two shapes of the cylinder
	Triangle* s1=new Triangle();
	Triangle* s2=new Triangle();

	s1->setNormal(vec3(1,0,0));
	s2->setNormal(vec3(0,1,0));

	vector<vec3> vertices;
	vertices.push_back(vec3(0,0,-5));
	vertices.push_back(vec3(0,0,5));

	Cylindre* cylindre=new Cylindre(s1,s2,&vertices,0,1,(decimal)0.2);

	// add a diffraction event
    Diffraction* ev0 = new Diffraction(vec3(0,0,0),vec3(0,1,0),cylindre);

    std::shared_ptr<Event> SPE0(ev0);
    ray->getEvents()->push_back(SPE0);

	diffraction=true;
	solidAngle=ray->getSolidAngle(diffraction);
	decimal expected =(decimal) (M_PI/2*M_2PI/ev0->getInitialNbResponseLeft()); //angleArrivée*2Pi/nb_rayons
	EXPECT_FLOAT_EQ(expected,solidAngle); 

	vec3 incomingDirection=vec3(-2,1,-1);
	incomingDirection.normalize();
	ev0->setIncomingDirection(incomingDirection);
	solidAngle=ray->getSolidAngle(diffraction);
    expected=(decimal)(ev0->getAngle()*M_2PI/ev0->getInitialNbResponseLeft());  //angleArrivée*2Pi/nb_rayons
	EXPECT_FLOAT_EQ(expected,solidAngle); 
	
}

// Test the getThickness() method
TEST(test_ray, get_thickness)
{
	Ray* ray = new Ray();
	  
	Source* src = new Source();
	src->setInitialRayCount(10000);
    ray->source = src;
	
	bool diffraction=false;
	decimal distance=500;
	decimal solidAngle=ray->getSolidAngle(diffraction);
	decimal thickness=(decimal)( 2*distance*sqrt(solidAngle/M_PI));

	EXPECT_FLOAT_EQ(thickness,ray->getThickness(distance,diffraction)); // no diffraction => thickness = 2* distance * sqrt(solidAngle/M_PI)

	diffraction=true;
	solidAngle=ray->getSolidAngle(diffraction);
	thickness=(decimal)(2*distance*sqrt(solidAngle/M_PI));
	EXPECT_FLOAT_EQ(thickness,ray->getThickness(distance,diffraction)); // diffraction == true but no diffraction event => thickness = 2* distance * sqrt(solidAngle/M_PI)


	//Create the cylinder used for the diffraction

	//Create the two shapes of the cylinder
	Triangle* s1=new Triangle();
	Triangle* s2=new Triangle();

	s1->setNormal(vec3(1,0,0));
	s2->setNormal(vec3(0,1,0));

	vector<vec3> vertices;
	vertices.push_back(vec3(0,0,-5));
	vertices.push_back(vec3(0,0,5));

	Cylindre* cylindre=new Cylindre(s1,s2,&vertices,0,1,(decimal)0.2);
	// add a diffraction event
    Diffraction* ev0 = new Diffraction(vec3(0,0,0),vec3(0,1,0),cylindre);

    std::shared_ptr<Event> SPE0(ev0);
    ray->getEvents()->push_back(SPE0);

	diffraction=true;
	solidAngle=ray->getSolidAngle(diffraction);
	thickness= distance*solidAngle;
	EXPECT_FLOAT_EQ(thickness,ray->getThickness(distance,diffraction)); 

	vec3 incomingDirection=vec3(-2,1,-1);
	incomingDirection.normalize();
	ev0->setIncomingDirection(incomingDirection);
	solidAngle=ray->getSolidAngle(diffraction);
    thickness =distance*solidAngle ;
	EXPECT_FLOAT_EQ(thickness,ray->getThickness(distance,diffraction)); 
	
}

// Test the output produced by the method signature()
TEST(test_ray, signature)
{
    Ray* ray = new Ray();

    Source* src = new Source();
    src->setId(1536);
    ray->source = src;

    Recepteur* rcpt = new Recepteur();
    rcpt->setId(5435);
    ray->recepteur = (void*) rcpt;

    // Add several events with different types
    Event* ev0 = new Event();
    ev0->setType(SPECULARREFLEXION);
    std::shared_ptr<Event> SPE0(ev0);
    ray->getEvents()->push_back(SPE0);

    Event* ev1 = new Event();
    ev1->setType(DIFFRACTION);
    std::shared_ptr<Event> SPE1(ev1);
    ray->getEvents()->push_back(SPE1);

    Event* ev2 = new Event();
	ev2->setType(DIFFRACTION);
    std::shared_ptr<Event> SPE2(ev2);
    ray->getEvents()->push_back(SPE2);

    Event* ev3 = new Event();
    ev3->setType(SPECULARREFLEXION);
    std::shared_ptr<Event> SPE3(ev3);
    ray->getEvents()->push_back(SPE3);

    signature sig = ray->getSignature();

    // first and last events are specular reflections => code = 1001 which is equal to 9 in base 10
    unsigned int SR = sig.first, SD = sig.second;
    EXPECT_EQ(9, SD);

    // check the source's id
    unsigned int S = SR >> 20;
    EXPECT_EQ(1536, S);

    // check the receptor's id
    unsigned int R = (SR << 12) >> 12;
    EXPECT_EQ(5435, R);

}
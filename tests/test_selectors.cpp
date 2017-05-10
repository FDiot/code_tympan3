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
#include "Tympan/solvers/AcousticRaytracer/Base.h"
#include "Tympan/solvers/AcousticRaytracer/Geometry/Triangle.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/FaceSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/LengthSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/DiffractionSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/ReflectionSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/CloseEventSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/FermatSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/DiffractionPathSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/DiffractionAngleSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/CleanerSelector.h"
#include "Tympan/solvers/AcousticRaytracer/Tools/SelectorManager.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;

int maxSources=10;
int maxReceptors=10;
int maxPos=1000;
int maxRays=100;


/***************************************
			CleanerSelector
****************************************/

//Test that CleanerSelector.canBeInserted ACCEPTS rays with no events
TEST(test_CleanerSelector_canBeInserted, no_events)
{
	CleanerSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);

	EXPECT_EQ(SELECTOR_ACCEPT,response); //Test response of canBeInserted
	

}

//Test that CleanerSelector.canBeInserted correctly erases events with type NOTHING
TEST(test_CleanerSelector_canBeInserted, nothing_events)
{
	CleanerSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	//Create several events, some with the type NOTHING
	Event* e1=new Event();
	QSharedPointer<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	Event* e2=new Event();
	e2->setType(NOTHING);
	QSharedPointer<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	SpecularReflexion* e3=new SpecularReflexion();
	e3->setType(NOTHING);
	QSharedPointer<Event> SPE3(e3);
	r->getEvents()->push_back(SPE3);

	SpecularReflexion* e4=new SpecularReflexion();
	QSharedPointer<Event> SPE4(e4);
	r->getEvents()->push_back(SPE4);

	Diffraction* e5=new Diffraction();
	QSharedPointer<Event> SPE5(e5);
	r->getEvents()->push_back(SPE5);

	Diffraction* e6=new Diffraction();
	e6->setType(NOTHING);
	QSharedPointer<Event> SPE6(e6);
	r->getEvents()->push_back(SPE6);

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);

	EXPECT_EQ(SELECTOR_REPLACE,response); //Test response of canBeInserted
	EXPECT_EQ(3,r->getEvents()->size());  //Test number of events after erasing NOTHING events
}


//Test that CleanerSelector.insertWithTest returns TRUE for rays with no events
TEST(test_CleanerSelector_insertWithTest, no_events)
{
	CleanerSelector<Ray> selector;
	Ray* r=new Ray();
	
	EXPECT_TRUE(selector.insertWithTest(r)); //Test return value of insertWithTest
	
}


//Test that CleanerSelector.insertWithTest correctly erases events with type NOTHING
TEST(test_CleanerSelector_insertWithTest, nothing_events)
{
	CleanerSelector<Ray> selector;
	Ray* r=new Ray();

	//Create several events, some with the type NOTHING
	Event* e1=new Event();
	e1->setType(NOTHING);
	QSharedPointer<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	Event* e2=new Event();
	QSharedPointer<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	SpecularReflexion* e3=new SpecularReflexion();
	QSharedPointer<Event> SPE3(e3);
	r->getEvents()->push_back(SPE3);

	SpecularReflexion* e4=new SpecularReflexion();
	QSharedPointer<Event> SPE4(e4);
	r->getEvents()->push_back(SPE4);

	Diffraction* e5=new Diffraction();
	e5->setType(NOTHING);
	QSharedPointer<Event> SPE5(e5);
	r->getEvents()->push_back(SPE5);

	Diffraction* e6=new Diffraction();
	QSharedPointer<Event> SPE6(e6);
	r->getEvents()->push_back(SPE6);

	EXPECT_TRUE(selector.insertWithTest(r)); //Test return value of insertWithTest
	EXPECT_EQ(4,r->getEvents()->size());     //Test number of events after erasing NOTHING events
}

/***************************************
			CloseEventSelector
****************************************/

//Test that CloseEventSelector.canBeInserted ACCEPTS rays with less than 2 events
TEST(test_CloseEventSelector_canBeInserted, less_than_2_events)
{
	CloseEventSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	//nb events = 0 < 2   =>   response should be SELECTOR_ACCEPT
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response);

	//Add one event
	Event* e1=new Event();
	QSharedPointer<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//nb events = 1 < 2   =>   response should be SELECTOR_ACCEPT
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response);
}

//Test that CloseEventSelector.canBeInserted ACCEPTS rays with consecutive events of same type
TEST(test_CloseEventSelector_canBeInserted, same_type_events)
{
	CloseEventSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	//Add one event
	SpecularReflexion* e1=new SpecularReflexion();
	e1->setType(SPECULARREFLEXION);
	QSharedPointer<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//Add one event
	SpecularReflexion* e2=new SpecularReflexion();
	e2->setType(SPECULARREFLEXION);
	QSharedPointer<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	//two events of same type   =>   response should be SELECTOR_ACCEPT
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response);

}

//Test that CloseEventSelector.canBeInserted ACCEPTS rays with consecutive events of different type occuring on different shapes
TEST(test_CloseEventSelector_canBeInserted, different_type_events_occuring_on_different_shapes)
{
	CloseEventSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	//Add one event
	SpecularReflexion* e1=new SpecularReflexion();
	e1->setType(SPECULARREFLEXION);
	QSharedPointer<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//Add second event
	Diffraction* e2=new Diffraction();
	e2->setType(DIFFRACTION);
	QSharedPointer<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	//Add a shape to the first event
	Shape* s1=new Shape();
	e1->setShape(s1);

	//Add a different shape to the second event
	Shape* s2=new Shape();
	Cylindre* s3=new Cylindre(s2,s2,0,1,0.2);
	e2->setShape(s3);

	//two events of different type occuring on different shapes  =>   response should be SELECTOR_ACCEPT
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response);

}


//Test that CloseEventSelector.canBeInserted REJECTS rays with consecutive events of different type occuring on the same shape
TEST(test_CloseEventSelector_canBeInserted, different_type_events_occuring_on_same_shape)
{
	CloseEventSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	//Add one event
	SpecularReflexion* e1=new SpecularReflexion();
	e1->setType(SPECULARREFLEXION);
	QSharedPointer<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//Add second event
	Diffraction* e2=new Diffraction();
	e2->setType(DIFFRACTION);
	QSharedPointer<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	//Add a shape to the first event
	Shape* s1=new Shape();
	e1->setShape(s1);

	//Add the same shape to the second event
	Cylindre* s2=new Cylindre(s1,s1,0,1,0.2);
	e2->setShape(s2);

	//Consecutive events of different type occuring on same shape =>  response should be SELECTOR_REJECT
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response);
}


//Test that CloseEventSelector.insertWithTest returns TRUE for rays with less than 2 events
TEST(test_CloseEventSelector_insertWithTest, less_than_2_events)
{
	CloseEventSelector<Ray> selector;
	Ray* r=new Ray();

	//nb events = 0 < 2   =>   return value should be TRUE
	EXPECT_TRUE(selector.insertWithTest(r));

	//Add one event
	Event* e1=new Event();
	QSharedPointer<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//nb events = 1 < 2   =>  return value should be TRUE
	EXPECT_TRUE(selector.insertWithTest(r));
}



//Test that CloseEventSelector.insertWithTest returns TRUE for rays with consecutive events of same type
TEST(test_CloseEventSelector_insertWithTest, same_type_events)
{
	CloseEventSelector<Ray> selector;
	Ray* r=new Ray();

	//Add second event
	Diffraction* e1=new Diffraction();
	e1->setType(DIFFRACTION);
	QSharedPointer<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//Add second event
	Diffraction* e2=new Diffraction();
	e2->setType(DIFFRACTION);
	QSharedPointer<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	//two events of same type   =>   return value should be TRUE
	EXPECT_TRUE(selector.insertWithTest(r));

}


//Test that CloseEventSelector.insertWithTest returns TRUE for rays with consecutive events of different type occuring on different shapes
TEST(test_CloseEventSelector_insertWithTest, different_type_events_occuring_on_different_shapes)
{
	CloseEventSelector<Ray> selector;
	Ray* r=new Ray();

	//Add one event
	Diffraction* e1=new Diffraction();
	e1->setType(DIFFRACTION);
	QSharedPointer<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//Add one event
	SpecularReflexion* e2=new SpecularReflexion();
	e2->setType(SPECULARREFLEXION);
	QSharedPointer<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	//Add a shape to the first event
	Shape* s1=new Shape();
	Cylindre* s2=new Cylindre(s1,s1,0,1,0.2);
	e1->setShape(s2);

	//Add a different shape to each event
	Shape* s3=new Shape();
	e2->setShape(s3);

	//two events of different type occuring on different shapes  =>   return value should be TRUE
	EXPECT_TRUE(selector.insertWithTest(r));

}

//Test that CloseEventSelector.insertWithTest returns FALSE for rays with consecutive events of different type occuring on the same shape
TEST(test_CloseEventSelector_insertWithTest, different_type_events_occuring_on_same_shape)
{
	CloseEventSelector<Ray> selector;
	Ray* r=new Ray();

	//Add one event
	Diffraction* e1=new Diffraction();
	e1->setType(DIFFRACTION);
	QSharedPointer<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//Add second event
	SpecularReflexion* e2=new SpecularReflexion();
	e2->setType(SPECULARREFLEXION);
	QSharedPointer<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	//Add a shape to the first event
	Shape* s1=new Shape();
	Cylindre* s2=new Cylindre(s1,s1,0,1,0.2);
	e1->setShape(s2);

	//Add the same shape to the second event
	e2->setShape(s1);

	//Consecutive events of different type occuring on same shape  =>   return value should be FALSE
	EXPECT_FALSE(selector.insertWithTest(r));
}

/***************************************
			DiffractionAngleSelector
****************************************/

//Test that DiffractionAngleSelector.canBeInserted ACCEPTS rays with no events
TEST(test_DiffractionAngleSelector_canBeInserted, no_events)
{

	DiffractionAngleSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //No event => response should be SELECTOR_ACCEPT
}

//Test that DiffractionAngleSelector.canBeInserted ACCEPTS rays with only SPECULARREFLEXION events
TEST(test_DiffractionAngleSelector_canBeInserted, only_reflexions)
{

	DiffractionAngleSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	//Add SpecularReflexion events
	for(int i=0;i<5;i++){
		SpecularReflexion* e=new SpecularReflexion();
		e->setType(SPECULARREFLEXION);
		QSharedPointer<Event> SPE(e);
		r->getEvents()->push_back(SPE);
		r->nbReflexion++;
	}

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //Only reflexions => response should be SELECTOR_ACCEPT
}


//Test that DiffractionAngleSelector.canBeInserted ACCEPTS  diffracted rays for which the diffraction has the same incoming and outgoing direction
TEST(test_DiffractionAngleSelector_canBeInserted, no_direction_change)
{
	DiffractionAngleSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	//Define initial position and direction
	vec3 initial_position(15,25,10);
	vec3 direction(5,-10,7);

	//Set a source
	Source* src=new Source();
	src->setPosition(initial_position);
	src->setName("src");
	r->source=src;

	//Set a receptor
	vec3 rcpt_pos=initial_position+direction*3.7; //Set the receptor's position to be a linear combination of initial_position and direction
	Recepteur* rcpt=new Recepteur(rcpt_pos,3);
	rcpt->setName("rcpt");
	r->recepteur=rcpt;

	//Define the shape used by the diffraction
	Shape* s1=new Shape();
	Cylindre* s3=new Cylindre(s1,s1,0,1,0.2);

	//Define a DIFFRACTION event with its position between the source and the receptor and aligned with them
	Diffraction* e=new Diffraction();
	e->setType(DIFFRACTION);
	e->setShape(s3);
	vec3 diff_pos=initial_position+direction*1.2; //Set the diffraction event's position to be a linear combination of initial_position and direction
	e->setPosition(diff_pos); 
	QSharedPointer<Event> SPE(e);

	r->getEvents()->push_back(SPE);
	r->nbDiffraction++;

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //incoming direction == outgoing direction => response should be SELECTOR_ACCEPT

}

//Test that DiffractionAngleSelector.canBeInserted ACCEPTS diffracted rays launched in the shadow zone of the obstacle
TEST(test_DiffractionAngleSelector_canBeInserted, ray_launched_in_shadow_zone)
{
	DiffractionAngleSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	//Set a source
	Source* src=new Source();
	src->setPosition(vec3(-5,0,0));
	src->setName("src");
	r->source=src;

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3(5,1,0),3);
	rcpt->setName("rcpt");
	r->recepteur=rcpt;

	//Define the shape used by the diffraction
	Triangle* s1=new Triangle(vec3(0,0,-1),vec3(0,0,1),vec3(-1,1,0),new Material());
	Triangle* s2=new Triangle(vec3(0,0,1),vec3(0,0,-1),vec3(1,1,0),new Material());
	Cylindre* s3=new Cylindre(s1,s2,0,1,0.2);

	//Define a DIFFRACTION event with its position between the source and the receptor and aligned with them
	Diffraction* e=new Diffraction();
	e->setType(DIFFRACTION);
	e->setShape(s3);
	e->setPosition(vec3(0,0,0)); 
	QSharedPointer<Event> SPE(e);

	r->getEvents()->push_back(SPE);
	r->nbDiffraction++;

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //ray launched is the shadow zone => response should be SELECTOR_ACCEPT

}

//Test that DiffractionAngleSelector.canBeInserted REJECTS diffracted rays launched outside the shadow zone of the obstacle
TEST(test_DiffractionAngleSelector_canBeInserted, ray_launched_outside_shadow_zone)
{
	DiffractionAngleSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	//Set a source
	Source* src=new Source();
	src->setPosition(vec3(-5,0,0));
	src->setName("src");
	r->source=src;

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3(5,-1,0),3);
	rcpt->setName("rcpt");
	r->recepteur=rcpt;

	//Define the shape used by the diffraction
	Triangle* s1=new Triangle(vec3(0,0,-1),vec3(0,0,1),vec3(-1,1,0),new Material());
	Triangle* s2=new Triangle(vec3(0,0,1),vec3(0,0,-1),vec3(1,1,0),new Material());
	Cylindre* s3=new Cylindre(s1,s2,0,1,0.2);

	//Define a DIFFRACTION event with its position between the source and the receptor and aligned with them
	Diffraction* e=new Diffraction();
	e->setType(DIFFRACTION);
	e->setShape(s3);
	e->setPosition(vec3(0,0,0)); 
	QSharedPointer<Event> SPE(e);

	r->getEvents()->push_back(SPE);
	r->nbDiffraction++;

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); //ray launched outside the shadow zone => response should be SELECTOR_REJECT

}


//Test that DiffractionAngleSelector.insertWithTest returns TRUE for rays with no events
TEST(test_DiffractionAngleSelector_insertWithTest, no_events)
{

	DiffractionAngleSelector<Ray> selector;
	Ray* r=new Ray();
	EXPECT_TRUE(selector.insertWithTest(r)); //No event => return value should be TRUE
}

//Test that DiffractionAngleSelector.insertWithTest returns TRUE for rays with only SPECULARREFLEXION events
TEST(test_DiffractionAngleSelector_insertWithTest, only_reflexions)
{

	DiffractionAngleSelector<Ray> selector;
	Ray* r=new Ray();

	//Add SpecularReflexion events
	for(int i=0;i<5;i++){
		SpecularReflexion* e=new SpecularReflexion();
		e->setType(SPECULARREFLEXION);
		QSharedPointer<Event> SPE(e);
		r->getEvents()->push_back(SPE);
		r->nbReflexion++;
	}

	EXPECT_TRUE(selector.insertWithTest(r)); //Only reflexions => return value should be TRUE
}


//Test that DiffractionAngleSelector.canBeInserted returns TRUE for diffracted rays for which the diffraction has the same incoming and outgoing direction
TEST(test_DiffractionAngleSelector_insertWithTest, no_direction_change)
{
	DiffractionAngleSelector<Ray> selector;
	Ray* r=new Ray();

	//Define initial position and direction
	vec3 initial_position(15,25,10);
	vec3 direction(5,-10,7);

	//Set a source
	Source* src=new Source();
	src->setPosition(initial_position);
	src->setName("src");
	r->source=src;

	//Set a receptor
	vec3 rcpt_pos=initial_position+direction*3.7; //Set the receptor's position to be a linear combination of initial_position and direction
	Recepteur* rcpt=new Recepteur(rcpt_pos,3);
	rcpt->setName("rcpt");
	r->recepteur=rcpt;

	//Define the shape used by the diffraction
	Shape* s1=new Shape();
	Cylindre* s3=new Cylindre(s1,s1,0,1,0.2);

	//Define a DIFFRACTION event with its position between the source and the receptor and aligned with them
	Diffraction* e=new Diffraction();
	e->setType(DIFFRACTION);
	e->setShape(s3);
	vec3 diff_pos=initial_position+direction*1.2; //Set the diffraction event's position to be a linear combination of initial_position and direction
	e->setPosition(diff_pos); 
	QSharedPointer<Event> SPE(e);

	r->getEvents()->push_back(SPE);
	r->nbDiffraction++;

	EXPECT_TRUE(selector.insertWithTest(r)); //incoming direction == outgoing direction => return value should be TRUE

}

//Test that DiffractionAngleSelector.canBeInserted returns TRUE for diffracted rays launched in the shadow zone of the obstacle
TEST(test_DiffractionAngleSelector_insertWithTest, ray_launched_in_shadow_zone)
{
	DiffractionAngleSelector<Ray> selector;
	Ray* r=new Ray();

	//Set a source
	Source* src=new Source();
	src->setPosition(vec3(-5,0,0));
	src->setName("src");
	r->source=src;

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3(5,1,0),3);
	rcpt->setName("rcpt");
	r->recepteur=rcpt;

	//Define the shape used by the diffraction
	Triangle* s1=new Triangle(vec3(0,0,-1),vec3(0,0,1),vec3(-1,1,0),new Material());
	Triangle* s2=new Triangle(vec3(0,0,1),vec3(0,0,-1),vec3(1,1,0),new Material());
	Cylindre* s3=new Cylindre(s1,s2,0,1,0.2);

	//Define a DIFFRACTION event with its position between the source and the receptor and aligned with them
	Diffraction* e=new Diffraction();
	e->setType(DIFFRACTION);
	e->setShape(s3);
	e->setPosition(vec3(0,0,0)); 
	QSharedPointer<Event> SPE(e);

	r->getEvents()->push_back(SPE);
	r->nbDiffraction++;

	EXPECT_TRUE(selector.insertWithTest(r)); //ray launched is the shadow zone => return value should be TRUE

}

//Test that DiffractionAngleSelector.canBeInserted returns FALSE for diffracted rays launched outside the shadow zone of the obstacle
TEST(test_DiffractionAngleSelector_insertWithTest, ray_launched_outside_shadow_zone)
{
	DiffractionAngleSelector<Ray> selector;
	Ray* r=new Ray();

	//Set a source
	Source* src=new Source();
	src->setPosition(vec3(-5,0,0));
	src->setName("src");
	r->source=src;

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3(5,-1,0),3);
	rcpt->setName("rcpt");
	r->recepteur=rcpt;

	//Define the shape used by the diffraction
	Triangle* s1=new Triangle(vec3(0,0,-1),vec3(0,0,1),vec3(-1,1,0),new Material());
	Triangle* s2=new Triangle(vec3(0,0,1),vec3(0,0,-1),vec3(1,1,0),new Material());
	Cylindre* s3=new Cylindre(s1,s2,0,1,0.2);

	//Define a DIFFRACTION event with its position between the source and the receptor and aligned with them
	Diffraction* e=new Diffraction();
	e->setType(DIFFRACTION);
	e->setShape(s3);
	e->setPosition(vec3(0,0,0)); 
	QSharedPointer<Event> SPE(e);

	r->getEvents()->push_back(SPE);
	r->nbDiffraction++;

	EXPECT_FALSE(selector.insertWithTest(r)); //ray launched outside the shadow zone => return value should be FALSE

}
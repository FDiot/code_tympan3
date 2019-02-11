#include <cstdlib>
#include <time.h> 

#include "gtest/gtest.h"
#include "Acoustic/Recepteur.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Base.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Engine/Simulation.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Triangle.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/FaceSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/CoPlanaritySelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/LengthSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/DiffractionSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/ReflectionSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/CloseEventSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/FermatSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/DiffractionPathSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/DiffractionAngleSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/CleanerSelector.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Tools/SelectorManager.h"
#include "Tools/UnitConverter.h"

#include <iostream>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;

int NTESTS=1000;		//Number of times randomized tests should be executed
int MAX_EVENTS=10;		//Maximum number of events when generating randomized rays
int MAX_RETRY=100;		//Max number of trials when random sampling

//returns a decimal between 0. et 1.
decimal rand_dec(){ return decimal(rand() / (RAND_MAX + 1.));}	

//returns a random unit vector by default (set normalize_vec to false if no normalization is needed)
vec3 rand_vec3(bool normalize_vec=true){						
	vec3 v(rand_dec(),rand_dec(),rand_dec());
	if(normalize_vec)
		v.normalize();
	return v;
}   


/***********************************************************************
						   CleanerSelector
************************************************************************/

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
	e1->setType(SPECULARREFLEXION);
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	Event* e2=new Event();
	e2->setType(NOTHING);
	std::shared_ptr<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	SpecularReflexion* e3=new SpecularReflexion();
	e3->setType(NOTHING);
	std::shared_ptr<Event> SPE3(e3);
	r->getEvents()->push_back(SPE3);

	SpecularReflexion* e4=new SpecularReflexion();
	e4->setType(SPECULARREFLEXION);
	std::shared_ptr<Event> SPE4(e4);
	r->getEvents()->push_back(SPE4);

	Diffraction* e5=new Diffraction();
	e5->setType(DIFFRACTION);
	std::shared_ptr<Event> SPE5(e5);
	r->getEvents()->push_back(SPE5);

	Diffraction* e6=new Diffraction();
	e6->setType(NOTHING);
	std::shared_ptr<Event> SPE6(e6);
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

	SpecularReflexion* e4=new SpecularReflexion();
	e4->setType(SPECULARREFLEXION);
	std::shared_ptr<Event> SPE4(e4);
	r->getEvents()->push_back(SPE4);

	Diffraction* e5=new Diffraction();
	e5->setType(NOTHING);
	std::shared_ptr<Event> SPE5(e5);
	r->getEvents()->push_back(SPE5);

	Diffraction* e6=new Diffraction();
	e6->setType(DIFFRACTION);
	std::shared_ptr<Event> SPE6(e6);
	r->getEvents()->push_back(SPE6);

	EXPECT_TRUE(selector.insertWithTest(r)); //Test return value of insertWithTest
	EXPECT_EQ(4,r->getEvents()->size());     //Test number of events after erasing NOTHING events
}









/***********************************************************************
						  CloseEventSelector
************************************************************************/

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
	std::shared_ptr<Event> SPE1(e1);
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
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//Add one event
	SpecularReflexion* e2=new SpecularReflexion();
	e2->setType(SPECULARREFLEXION);
	std::shared_ptr<Event> SPE2(e2);
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
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//Add second event
	Diffraction* e2=new Diffraction();
	e2->setType(DIFFRACTION);
	std::shared_ptr<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	//Add a shape to the first event
	Shape* s1=new Shape();
	e1->setShape(s1);

	//Add a different shape to the second event
	Shape* s2=new Shape();
	Cylindre* s3=new Cylindre(s2,s2,new std::vector<vec3>(),0,1,(decimal)0.2);
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
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//Add second event
	Diffraction* e2=new Diffraction();
	e2->setType(DIFFRACTION);
	std::shared_ptr<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	//Add a shape to the first event
	Shape* s1=new Shape();
	e1->setShape(s1);

	//Add the same shape to the second event
	Cylindre* s2=new Cylindre(s1,s1,new std::vector<vec3>(),0,1,(decimal)0.2);
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
	std::shared_ptr<Event> SPE1(e1);
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
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//Add second event
	Diffraction* e2=new Diffraction();
	e2->setType(DIFFRACTION);
	std::shared_ptr<Event> SPE2(e2);
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
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//Add one event
	SpecularReflexion* e2=new SpecularReflexion();
	e2->setType(SPECULARREFLEXION);
	std::shared_ptr<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	//Add a shape to the first event
	Shape* s1=new Shape();
	Cylindre* s2=new Cylindre(s1,s1,new std::vector<vec3>(),0,1,(decimal)0.2);
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
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	//Add second event
	SpecularReflexion* e2=new SpecularReflexion();
	e2->setType(SPECULARREFLEXION);
	std::shared_ptr<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	//Add a shape to the first event
	Shape* s1=new Shape();
	Cylindre* s2=new Cylindre(s1,s1,new std::vector<vec3>(),0,1,(decimal)0.2);
	e1->setShape(s2);

	//Add the same shape to the second event
	e2->setShape(s1);

	//Consecutive events of different type occuring on same shape  =>   return value should be FALSE
	EXPECT_FALSE(selector.insertWithTest(r));
}








/***********************************************************************
						DiffractionAngleSelector
************************************************************************/

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
		std::shared_ptr<Event> SPE(e);
		r->getEvents()->push_back(SPE);
		r->setNbReflexion(r->getReflex()+1);	
		}

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //Only reflexions => response should be SELECTOR_ACCEPT
}


//Test that DiffractionAngleSelector.canBeInserted ACCEPTS diffracted rays for which the diffraction has the same incoming and outgoing directions
TEST(test_DiffractionAngleSelector_canBeInserted, no_direction_change)
{
	DiffractionAngleSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	//Define initial and final positions 
	vec3 initial_position(15,25,10);
	vec3 final_position(30,-5,28);

	//Set a source
	Source* src=new Source();
	src->setPosition(initial_position);
	r->setSource(src);

	//Set a receptor
	Recepteur* rcpt=new Recepteur(final_position,3);
	r->setRecepteur(rcpt);

	//Define the shape used by the diffraction
	Shape* s1=new Shape();
	Cylindre* s3=new Cylindre(s1,s1,new std::vector<vec3>(),0,1,(decimal)0.2);

	//Define a DIFFRACTION event with its position between the source and the receptor
	Diffraction* e=new Diffraction();
	e->setType(DIFFRACTION);
	e->setShape(s3); 
	e->setPosition(initial_position+(final_position-initial_position)*(decimal)0.35); //Set the diffraction event's position to be between the source and the receptor
	std::shared_ptr<Event> SPE(e);

	r->getEvents()->push_back(SPE);
	r->setNbDiffraction(r->getDiff()+1);

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
	r->setSource(src);

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3(5,1,0),3);
	r->setRecepteur(rcpt);

	//Define the shape used by the diffraction
	Triangle* s1=new Triangle(vec3(0,0,-1),vec3(0,0,1),vec3(-1,1,0),new Material());
	Triangle* s2=new Triangle(vec3(0,0,1),vec3(0,0,-1),vec3(1,1,0),new Material());
	Cylindre* s3=new Cylindre(s1,s2,new std::vector<vec3>(),0,1,(decimal)0.2);

	//Define a DIFFRACTION event with its position between the source and the receptor and aligned with them
	Diffraction* e=new Diffraction();
	e->setType(DIFFRACTION);
	e->setShape(s3);
	e->setPosition(vec3(0,0,0)); 
	std::shared_ptr<Event> SPE(e);

	r->getEvents()->push_back(SPE);
	r->setNbDiffraction(r->getDiff()+1);
	
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
	r->setSource(src);

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3(5,-1,0),3);
	r->setRecepteur(rcpt);

	//Define the shape used by the diffraction
	Triangle* s1=new Triangle(vec3(0,0,-1),vec3(0,0,1),vec3(-1,1,0),new Material());
	Triangle* s2=new Triangle(vec3(0,0,1),vec3(0,0,-1),vec3(1,1,0),new Material());
	Cylindre* s3=new Cylindre(s1,s2,new std::vector<vec3>(),0,1,(decimal)0.2);

	//Define a DIFFRACTION event with its position between the source and the receptor and aligned with them
	Diffraction* e=new Diffraction();
	e->setType(DIFFRACTION);
	e->setShape(s3);
	e->setPosition(vec3(0,0,0)); 
	std::shared_ptr<Event> SPE(e);

	r->getEvents()->push_back(SPE);
	
	r->setNbDiffraction(r->getDiff()+1);
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
		std::shared_ptr<Event> SPE(e);
		r->getEvents()->push_back(SPE);
		r->setNbReflexion(r->getReflex()+1);
	}

	EXPECT_TRUE(selector.insertWithTest(r)); //Only reflexions => return value should be TRUE
}


//Test that DiffractionAngleSelector.canBeInserted returns TRUE for diffracted rays for which the diffraction has the same incoming and outgoing direction
TEST(test_DiffractionAngleSelector_insertWithTest, no_direction_change)
{
	DiffractionAngleSelector<Ray> selector;
	Ray* r=new Ray();

	//Define initial and final positions 
	vec3 initial_position(7,-4,12);
	vec3 final_position(-13,10,8);

	//Set a source
	Source* src=new Source();
	src->setPosition(initial_position);
	r->setSource(src);

	//Set a receptor
	Recepteur* rcpt=new Recepteur(final_position,3);
	r->setRecepteur(rcpt);

	//Define the shape used by the diffraction
	Shape* s1=new Shape();
	Cylindre* s3=new Cylindre(s1,s1,new std::vector<vec3>(),0,1,(decimal)0.2);

	//Define a DIFFRACTION event with its position between the source and the receptor and aligned with them
	Diffraction* e=new Diffraction();
	e->setType(DIFFRACTION);
	e->setShape(s3); 
	e->setPosition(initial_position+(final_position-initial_position)*(decimal)0.72); //Set the diffraction event's position to be a linear combination of initial_position and direction
	std::shared_ptr<Event> SPE(e);

	r->getEvents()->push_back(SPE);
	r->setNbDiffraction(r->getDiff()+1);

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
	r->setSource(src);

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3(5,1,0),3);
	r->setRecepteur(rcpt);

	//Define the shape used by the diffraction
	Triangle* s1=new Triangle(vec3(0,0,-1),vec3(0,0,1),vec3(-1,1,0),new Material());
	Triangle* s2=new Triangle(vec3(0,0,1),vec3(0,0,-1),vec3(1,1,0),new Material());
	Cylindre* s3=new Cylindre(s1,s2,new std::vector<vec3>(),0,1,(decimal)0.2);

	//Define a DIFFRACTION event with its position between the source and the receptor and aligned with them
	Diffraction* e=new Diffraction();
	e->setType(DIFFRACTION);
	e->setShape(s3);
	e->setPosition(vec3(0,0,0)); 
	std::shared_ptr<Event> SPE(e);

	r->getEvents()->push_back(SPE);
	r->setNbDiffraction(r->getDiff()+1);
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
	r->setSource(src);

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3(5,-1,0),3);
	r->setRecepteur(rcpt);

	//Define the shape used by the diffraction
	Triangle* s1=new Triangle(vec3(0,0,-1),vec3(0,0,1),vec3(-1,1,0),new Material());
	Triangle* s2=new Triangle(vec3(0,0,1),vec3(0,0,-1),vec3(1,1,0),new Material());
	Cylindre* s3=new Cylindre(s1,s2,new std::vector<vec3>(),0,1,(decimal)0.2);

	//Define a DIFFRACTION event with its position between the source and the receptor and aligned with them
	Diffraction* e=new Diffraction();
	e->setType(DIFFRACTION);
	e->setShape(s3);
	e->setPosition(vec3(0,0,0)); 
	std::shared_ptr<Event> SPE(e);

	r->getEvents()->push_back(SPE);
	
	r->setNbDiffraction(r->getDiff()+1);
	EXPECT_FALSE(selector.insertWithTest(r)); //ray launched outside the shadow zone => return value should be FALSE

}







/***********************************************************************
						DiffractionPathSelector
************************************************************************/

//Test that DiffractionPathSelector.canBeInserted ACCEPTS rays with no events
TEST(test_DiffractionPathSelector_canBeInserted, no_events)
{

	DiffractionPathSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //No event => response should be SELECTOR_ACCEPT
}

//Test that DiffractionPathSelector.canBeInserted ACCEPTS rays with only SPECULARREFLEXION events (since their cumul_delta should be 0)
TEST(test_DiffractionPathSelector_canBeInserted, only_reflexions)
{

	DiffractionPathSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;

	//init random number generator
	unsigned int seed=(unsigned int)time(NULL);
	srand(seed);
	cout<<"[          ] Random number generator initialized with seed "<<seed<<endl;

	//Set a source
	Source* src=new Source();
	src->setPosition(vec3((decimal)(rand()%1000+1),(decimal)(rand()%1000+1),(decimal)(rand()%1000+1)));
	r->setSource(src);

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3((decimal)(rand()%1000+1),(decimal)(rand()%1000+1),(decimal)(rand()%1000+1)),3);
	r->setRecepteur(rcpt);

	//Add SpecularReflexion events
	for(int i=0;i<rand()%100;i++){
		SpecularReflexion* e=new SpecularReflexion();
		e->setType(SPECULARREFLEXION);
		e->setPosition(vec3((decimal)(rand()%1000+1),(decimal)(rand()%1000+1),(decimal)(rand()%1000+1)));
		std::shared_ptr<Event> SPE(e);
		r->getEvents()->push_back(SPE);
		r->setNbReflexion(r->getReflex()+1);
	}

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //Only reflexions => response should be SELECTOR_ACCEPT

}


//Test that DiffractionPathSelector.canBeInserted ACCEPTS rays with a low enough cumulative delta
TEST(test_DiffractionPathSelector_canBeInserted, cumul_delta_low_enough)
{

	DiffractionPathSelector<Ray> selector;
	selector.setMaximumDelta(14);

	Ray* r=new Ray();
	unsigned long long replace;

	//Set a source
	Source* src=new Source();
	src->setPosition(vec3(3,7,5));
	r->setSource(src);

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3(30,11,-1),3);
	r->setRecepteur(rcpt);

	Diffraction* e1=new Diffraction();
	e1->setType(DIFFRACTION);
	e1->setPosition(vec3(10,10,8));
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	SpecularReflexion* e2=new SpecularReflexion();
	e2->setType(SPECULARREFLEXION);
	e2->setPosition(vec3(16,7,3));
	std::shared_ptr<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);
	
	SpecularReflexion* e3=new SpecularReflexion();
	e3->setType(SPECULARREFLEXION);
	e3->setPosition(vec3(22,13,-1));
	std::shared_ptr<Event> SPE3(e3);
	r->getEvents()->push_back(SPE3);

	Diffraction* e4=new Diffraction();
	e4->setType(DIFFRACTION);
	e4->setPosition(vec3(27,7,4));
	std::shared_ptr<Event> SPE4(e4);
	r->getEvents()->push_back(SPE4);

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //cumul_delta = 11.49 < 11.5 => response should be SELECTOR_ACCEPT

}


//Test that DiffractionPathSelector.canBeInserted REJECTS rays with a cumulative delta too high
TEST(test_DiffractionPathSelector_canBeInserted, cumul_delta_too_high)
{

	DiffractionPathSelector<Ray> selector;
	selector.setMaximumDelta(11.5);

	Ray* r=new Ray();
	unsigned long long replace;

	//Set a source
	Source* src=new Source();
	src->setPosition(vec3(-5,8,1));
	r->setSource(src);

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3(22,-4,6),3);
	r->setRecepteur(rcpt);

	Diffraction* e1=new Diffraction();
	e1->setType(DIFFRACTION);
	e1->setPosition(vec3(-1,4,-2));
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	SpecularReflexion* e2=new SpecularReflexion();
	e2->setType(SPECULARREFLEXION);
	e2->setPosition(vec3(5,-3,0));
	std::shared_ptr<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);
	
	Diffraction* e3=new Diffraction();
	e3->setType(DIFFRACTION);
	e3->setPosition(vec3(15,7,4));
	std::shared_ptr<Event> SPE3(e3);
	r->getEvents()->push_back(SPE3);

	SpecularReflexion* e4=new SpecularReflexion();
	e4->setType(SPECULARREFLEXION);
	e4->setPosition(vec3(18,-3,-1));
	std::shared_ptr<Event> SPE4(e4);
	r->getEvents()->push_back(SPE4);

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); //cumul_delta = 14.17 > 14 => response should be SELECTOR_REJECT

}


//Test that DiffractionPathSelector.insertWithTest returns TRUE for rays with no events
TEST(test_DiffractionPathSelector_insertWithTest, no_events)
{

	DiffractionPathSelector<Ray> selector;
	Ray* r=new Ray();

	EXPECT_TRUE(selector.insertWithTest(r)); //No event => should return TRUE
}

//Test that DiffractionPathSelector.insertWithTest returns TRUE for rays with only SPECULARREFLEXION events (since their cumul_delta should be 0)
TEST(test_DiffractionPathSelector_insertWithTest, only_reflexions)
{

	DiffractionPathSelector<Ray> selector;
	Ray* r=new Ray();

	//init random number generator
	unsigned int seed=(unsigned int)time(NULL);
	srand(seed);
	cout<<"[          ] Random number generator initialized with seed "<<seed<<endl;

	//Set a source
	Source* src=new Source();
	src->setPosition(vec3((decimal)(rand()%1000+1),(decimal)(rand()%1000+1),(decimal)(rand()%1000+1)));
	r->setSource(src);

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3((decimal)(rand()%1000+1),(decimal)(rand()%1000+1),(decimal)(rand()%1000+1)),3);
	r->setRecepteur(rcpt);

	//Add SpecularReflexion events
	for(int i=0;i<rand()%100;i++){
		SpecularReflexion* e=new SpecularReflexion();
		e->setType(SPECULARREFLEXION);
		e->setPosition(vec3((decimal)(rand()%1000+1),(decimal)(rand()%1000+1),(decimal)(rand()%1000+1)));
		std::shared_ptr<Event> SPE(e);
		r->getEvents()->push_back(SPE);
		r->setNbReflexion(r->getReflex()+1);
	}

	EXPECT_TRUE(selector.insertWithTest(r)); //Only reflexions => should return TRUE

}


//Test that DiffractionPathSelector.insertWithTest reutnrs TRUE for rays with a low enough cumulative delta
TEST(test_DiffractionPathSelector_insertWithTest, cumul_delta_low_enough)
{

	DiffractionPathSelector<Ray> selector;
	selector.setMaximumDelta(14);

	Ray* r=new Ray();

	//Set a source
	Source* src=new Source();
	src->setPosition(vec3(3,7,5));
	r->setSource(src);

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3(30,11,-1),3);
	r->setRecepteur(rcpt);

	Diffraction* e1=new Diffraction();
	e1->setType(DIFFRACTION);
	e1->setPosition(vec3(10,10,8));
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	SpecularReflexion* e2=new SpecularReflexion();
	e2->setType(SPECULARREFLEXION);
	e2->setPosition(vec3(16,7,3));
	std::shared_ptr<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);
	
	SpecularReflexion* e3=new SpecularReflexion();
	e3->setType(SPECULARREFLEXION);
	e3->setPosition(vec3(22,13,-1));
	std::shared_ptr<Event> SPE3(e3);
	r->getEvents()->push_back(SPE3);

	Diffraction* e4=new Diffraction();
	e4->setType(DIFFRACTION);
	e4->setPosition(vec3(27,7,4));
	std::shared_ptr<Event> SPE4(e4);
	r->getEvents()->push_back(SPE4);

	EXPECT_TRUE(selector.insertWithTest(r));  //cumul_delta = 11.49 < 11.5 => should return TRUE

}


//Test that DiffractionPathSelector.insertWithTest returns FALSE for rays with a cumulative delta too high
TEST(test_DiffractionPathSelector_insertWithTest, cumul_delta_too_high)
{

	DiffractionPathSelector<Ray> selector;
	selector.setMaximumDelta(11.5);

	Ray* r=new Ray();

	//Set a source
	Source* src=new Source();
	src->setPosition(vec3(-5,8,1));
	r->setSource(src);

	//Set a receptor
	Recepteur* rcpt=new Recepteur(vec3(22,-4,6),3);
	r->setRecepteur(rcpt);

	Diffraction* e1=new Diffraction();
	e1->setType(DIFFRACTION);
	e1->setPosition(vec3(-1,4,-2));
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	SpecularReflexion* e2=new SpecularReflexion();
	e2->setType(SPECULARREFLEXION);
	e2->setPosition(vec3(5,-3,0));
	std::shared_ptr<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);
	
	Diffraction* e3=new Diffraction();
	e3->setType(DIFFRACTION);
	e3->setPosition(vec3(15,7,4));
	std::shared_ptr<Event> SPE3(e3);
	r->getEvents()->push_back(SPE3);

	SpecularReflexion* e4=new SpecularReflexion();
	e4->setType(SPECULARREFLEXION);
	e4->setPosition(vec3(18,-3,-1));
	std::shared_ptr<Event> SPE4(e4);
	r->getEvents()->push_back(SPE4);

	EXPECT_FALSE(selector.insertWithTest(r));  //cumul_delta = 14.17 > 14 => should reutrn FALSE

}






/***********************************************************************
						 DiffractionSelector
************************************************************************/

//Test DiffractionSelector.canBeInserted with the LESS operator
TEST(test_DiffractionSelector_canBeInserted, less)
{

	DiffractionSelector<Ray> selector;
	selector.setOperator(LESS);
	selector.setMaximumDiffractionOrder(5);
	Ray* r=new Ray();
	unsigned long long replace;

	r->setNbDiffraction(4); 
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 4 < 5 => response should be SELECTOR_ACCEPT

	r->setNbDiffraction(5); 
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 5 !< 5 => response should be SELECTOR_REJECT
}

//Test DiffractionSelector.canBeInserted with the LESS_OR_EQUAL
TEST(test_DiffractionSelector_canBeInserted, less_or_equal)
{

	DiffractionSelector<Ray> selector;
	selector.setMaximumDiffractionOrder(10);
	Ray* r=new Ray();
	unsigned long long replace;

	r->setNbDiffraction(6);
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 6 <= 10 => response should be SELECTOR_ACCEPT

	r->setNbDiffraction(10);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 10 <= 10 => response should be SELECTOR_ACCEPT

	r->setNbDiffraction(15);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 15 !<= 10 => response should be SELECTOR_REJECT
}


//Test DiffractionSelector.canBeInserted with the EQUAL
TEST(test_DiffractionSelector_canBeInserted, equal)
{

	DiffractionSelector<Ray> selector;
	selector.setOperator(EQUAL);
	selector.setMaximumDiffractionOrder(7);
	Ray* r=new Ray();
	unsigned long long replace;

	r->setNbDiffraction(7);
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 7 == 7 => response should be SELECTOR_ACCEPT

	r->setNbDiffraction(3);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 3 != 7 => response should be SELECTOR_REJECT
}


//Test DiffractionSelector.canBeInserted with the GREATER_OR_EQUAL
TEST(test_DiffractionSelector_canBeInserted, greater_or_equal)
{

	DiffractionSelector<Ray> selector;
	selector.setOperator(GREATER_OR_EQUAL);
	Ray* r=new Ray();
	unsigned long long replace;

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 0 !>= 1 => response should be SELECTOR_REJECT

	r->setNbDiffraction(1);
    response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 1 >= 1 => response should be SELECTOR_ACCEPT

	r->setNbDiffraction(2);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 2 >= 1 => response should be SELECTOR_ACCEPT
}


//Test DiffractionSelector.canBeInserted with the GREATER operator
TEST(test_DiffractionSelector_canBeInserted, greater)
{

	DiffractionSelector<Ray> selector;
	selector.setOperator(GREATER);
	selector.setMaximumDiffractionOrder(8);
	Ray* r=new Ray();
	unsigned long long replace;

	r->setNbDiffraction(9);
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //9 > 8 => response should be SELECTOR_ACCEPT

	r->setNbDiffraction(8);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 8 !> 8 => response should be SELECTOR_REJECT
}


//Test DiffractionSelector.insertWithTest with the LESS operator
TEST(test_DiffractionSelector_insertWithTest, less)
{

	DiffractionSelector<Ray> selector;
	selector.setOperator(LESS);
	selector.setMaximumDiffractionOrder(9);
	Ray* r=new Ray();

	r->setNbDiffraction(5);
	EXPECT_TRUE(selector.insertWithTest(r));  // 5 < 9 =>  should return TRUE

	r->setNbDiffraction(9);
	EXPECT_FALSE(selector.insertWithTest(r)); // 9 !< 9 =>  should return FALSE
}

//Test DiffractionSelector.insertWithTest with the LESS_OR_EQUAL
TEST(test_DiffractionSelector_insertWithTest, less_or_equal)
{

	DiffractionSelector<Ray> selector;
	Ray* r=new Ray();

	EXPECT_TRUE(selector.insertWithTest(r));  // 0 <= 1 =>  should return TRUE

	r->setNbDiffraction(1);
	EXPECT_TRUE(selector.insertWithTest(r));  // 1 <= 1 => should return TRUE

	r->setNbDiffraction(2);
	EXPECT_FALSE(selector.insertWithTest(r)); // 2 !<= 2 => should return FALSE
}


//Test DiffractionSelector.insertWithTest with the EQUAL
TEST(test_DiffractionSelector_insertWithTest, equal)
{

	DiffractionSelector<Ray> selector;
	selector.setOperator(EQUAL);
	selector.setMaximumDiffractionOrder(15);
	Ray* r=new Ray();

	r->setNbDiffraction(15);
	EXPECT_TRUE(selector.insertWithTest(r));  // 15==15 =>  should return TRUE

	r->setNbDiffraction(8);
	EXPECT_FALSE(selector.insertWithTest(r)); // 8 != 15 => should return FALSE
}


//Test DiffractionSelector.insertWithTest with the GREATER_OR_EQUAL
TEST(test_DiffractionSelector_insertWithTest, greater_or_equal)
{

	DiffractionSelector<Ray> selector;
	selector.setOperator(GREATER_OR_EQUAL);
	selector.setMaximumDiffractionOrder(6);
	Ray* r=new Ray();

	r->setNbDiffraction(7);
	EXPECT_TRUE(selector.insertWithTest(r));  //7 >= 6 => should return TRUE

	r->setNbDiffraction(6);
	EXPECT_TRUE(selector.insertWithTest(r));  // 6 >= 6 => should return TRUE

	r->setNbDiffraction(2);
	EXPECT_FALSE(selector.insertWithTest(r)); // 2 !>= 6 => should return FALSE
}


//Test DiffractionSelector.insertWithTest with the GREATER operator
TEST(test_DiffractionSelector_insertWithTest, greater)
{

	DiffractionSelector<Ray> selector;
	selector.setOperator(GREATER);
	selector.setMaximumDiffractionOrder(12);
	Ray* r=new Ray();

	r->setNbDiffraction(13);
	EXPECT_TRUE(selector.insertWithTest(r));  // 13 > 12 => should return TRUE

	r->setNbDiffraction(12);
	EXPECT_FALSE(selector.insertWithTest(r)); // 12 !> 12 => should return FALSE
}







/***********************************************************************
							 FaceSelector
************************************************************************/

//Test FaceSelector.canBeInserted with the HISTORY_PRIMITIVE mode
TEST(test_FaceSelector_canBeInserted, history_primitive_mode)
{

	FaceSelector<Ray> selector;
	selector.setModeHistory(HISTORY_PRIMITIVE);

	Ray* r1=new Ray();
	Ray* r2=new Ray();
	Ray* r3=new Ray();
	Ray* r4=new Ray();

	unsigned long long replace;
	Material* mat=new Material();

	//init random number generator
	unsigned int seed=(unsigned int)time(NULL);
	srand(seed);
	cout<<"[          ] Random number generator initialized with seed "<<seed<<endl;

	Simulation simu;
	Scene* scene=simu.getScene();

	//Set a source
	Source src;
	src.setPosition(vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5)));
	r1->setSource(&src);
	r2->setSource(&src);
	r3->setSource(&src);
	r4->setSource(&src);

	//Set a receptor
	Recepteur rcpt(vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5)),3);
	r1->setRecepteur(&rcpt);
	r2->setRecepteur(&rcpt);
	r3->setRecepteur(&rcpt);
	r4->setRecepteur(&rcpt);

	simu.addSource(src);
	simu.addRecepteur(rcpt);

	//Randomly generate four triangles and add theme to the scene
	std::vector<Shape*> shapes;
	for(int i=0;i<4;i++){
		unsigned int p1,p2,p3;

		vec3 vertex=vec3((decimal)(rand()%100+1),(decimal)(rand()%100+1),(decimal)(rand()%100+1));
		vec3 u=vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5));
		vec3 v=vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5));
		
		scene->addVertex(vertex, p1);
		scene->addVertex(vertex+u, p2);
		scene->addVertex(vertex+v, p3);
		shapes.push_back(scene->addTriangle(p1,p2,p3,mat));
	}

	//Define some events and associate a shape and a position to each one of them
	Event* e1=new Event();
	e1->setShape(shapes[0]);
	e1->setPosition(vec3(48,-25,74));
	std::shared_ptr<Event> SPE1(e1);

	Event* e2=new Event();
	e2->setShape(shapes[1]);
	e2->setPosition(vec3(24,11,-62));
	std::shared_ptr<Event> SPE2(e2);

	Event* e3=new Event();
	e3->setShape(shapes[1]);
	e3->setPosition(vec3(37,-17,56)); 
	std::shared_ptr<Event> SPE3(e3);

	Event* e4=new Event();
	e4->setShape(shapes[2]);
	e4->setPosition(vec3(-12,88,-42));
	std::shared_ptr<Event> SPE4(e4);

	Event* e5=new Event();
	e5->setShape(shapes[2]);
	e5->setPosition(vec3(57,-18,33));
	std::shared_ptr<Event> SPE5(e5);

	Event* e6=new Event();
	e6->setShape(shapes[3]);
	e6->setPosition(vec3(78,-51,36));
	std::shared_ptr<Event> SPE6(e6);

	//Assign events to the rays
	r1->getEvents()->push_back(SPE1);
	r1->getEvents()->push_back(SPE2);
	r1->getEvents()->push_back(SPE6);
	r1->getEvents()->push_back(SPE5);

	r2->getEvents()->push_back(SPE1);
	r2->getEvents()->push_back(SPE3);
	r2->getEvents()->push_back(SPE6);
	r2->getEvents()->push_back(SPE4);

	r3->getEvents()->push_back(SPE3);
	r3->getEvents()->push_back(SPE1);
	r3->getEvents()->push_back(SPE6);
	r3->getEvents()->push_back(SPE5);
	
	r4->getEvents()->push_back(SPE2);
	r4->getEvents()->push_back(SPE1);
	r4->getEvents()->push_back(SPE6);
	r4->getEvents()->push_back(SPE4);

	SELECTOR_RESPOND response=selector.canBeInserted(r1,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // no ray selected yet => response should be SELECTOR_ACCEPT
	selector.insert(r1); //select r1

	response=selector.canBeInserted(r2,replace);
	EXPECT_EQ(SELECTOR_REPLACE,response); // same primitiveId sequence as r1 but with shorter length => response should be SELECTOR_REPLACE
	
	response=selector.canBeInserted(r3,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // different primitiveId sequence from r1 and r2 => response should be SELECTOR_ACCEPT
	selector.insert(r3); //select r3

	response=selector.canBeInserted(r4,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // same primitiveId sequence as r3 but with longer length => response should be SELECTOR_REJECT
}



//Test FaceSelector.canBeInserted with the HISTORY_FACE mode
TEST(test_FaceSelector_canBeInserted, history_face_mode)
{

	FaceSelector<Ray> selector;
	selector.setModeHistory(HISTORY_FACE);

	Ray* r1=new Ray();
	Ray* r2=new Ray();
	Ray* r3=new Ray();
	Ray* r4=new Ray();

	unsigned long long replace;
	Material* mat=new Material();
	
	//init random number generator
	unsigned int seed=(unsigned int)time(NULL);
	srand(seed);
	cout<<"[          ] Random number generator initialized with seed "<<seed<<endl;

	Simulation simu;
	Scene* scene=simu.getScene();

	//Set a source
	Source src;
	src.setPosition(vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5)));
	r1->setSource(&src);
	r2->setSource(&src);
	r3->setSource(&src);
	r4->setSource(&src);

	//Set a receptor
	Recepteur rcpt(vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5)),3);
	r1->setRecepteur(&rcpt);
	r2->setRecepteur(&rcpt);
	r3->setRecepteur(&rcpt);
	r4->setRecepteur(&rcpt);

	simu.addSource(src);
	simu.addRecepteur(rcpt);


	//Randomly generate four triangles with a different shapeId and add theme to the scene
	std::vector<Shape*> shapes;
	for(int i=0;i<4;i++){
		unsigned int p1,p2,p3;

		vec3 vertex=vec3((decimal)(rand()%100+1),(decimal)(rand()%100+1),(decimal)(rand()%100+1));
		vec3 u=vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5));
		vec3 v=vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5));
		
		scene->addVertex(vertex, p1);
		scene->addVertex(vertex+u, p2);
		scene->addVertex(vertex+v, p3);
		Shape* shape=scene->addTriangle(p1,p2,p3,mat);
		shape->setFaceId(i);
		shapes.push_back(shape);
	}

	//Define some events and associate a shape and a position to each one of them
	Event* e1=new Event();
	e1->setShape(shapes[0]);
	e1->setPosition(vec3(48,-25,74));
	std::shared_ptr<Event> SPE1(e1);

	Event* e2=new Event();
	e2->setShape(shapes[1]);
	e2->setPosition(vec3(24,11,-62));
	std::shared_ptr<Event> SPE2(e2);

	Event* e3=new Event();
	e3->setShape(shapes[1]);
	e3->setPosition(vec3(37,-17,56)); 
	std::shared_ptr<Event> SPE3(e3);

	Event* e4=new Event();
	e4->setShape(shapes[2]);
	e4->setPosition(vec3(-12,88,-42));
	std::shared_ptr<Event> SPE4(e4);

	Event* e5=new Event();
	e5->setShape(shapes[2]);
	e5->setPosition(vec3(57,-18,33));
	std::shared_ptr<Event> SPE5(e5);

	Event* e6=new Event();
	e6->setShape(shapes[3]);
	e5->setPosition(vec3(78,-51,36));
	std::shared_ptr<Event> SPE6(e6);

	//Assign events to the rays
	r1->getEvents()->push_back(SPE1);
	r1->getEvents()->push_back(SPE2);
	r1->getEvents()->push_back(SPE6);
	r1->getEvents()->push_back(SPE5);

	r2->getEvents()->push_back(SPE1);
	r2->getEvents()->push_back(SPE3);
	r2->getEvents()->push_back(SPE6);
	r2->getEvents()->push_back(SPE4);

	r3->getEvents()->push_back(SPE3);
	r3->getEvents()->push_back(SPE1);
	r3->getEvents()->push_back(SPE6);
	r3->getEvents()->push_back(SPE5);
	
	r4->getEvents()->push_back(SPE2);
	r4->getEvents()->push_back(SPE1);
	r4->getEvents()->push_back(SPE6);
	r4->getEvents()->push_back(SPE4);

	SELECTOR_RESPOND response=selector.canBeInserted(r1,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // no ray selected yet => response should be SELECTOR_ACCEPT
	selector.insert(r1); //select r1

	response=selector.canBeInserted(r2,replace);
	EXPECT_EQ(SELECTOR_REPLACE,response); // same faceId sequence as r1 but with shorter length => response should be SELECTOR_REPLACE
	
	response=selector.canBeInserted(r3,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // different faceId sequence from r1 and r2 => response should be SELECTOR_ACCEPT
	selector.insert(r3); //select r3

	response=selector.canBeInserted(r4,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // same faceId sequence as r3 but with longer length => response should be SELECTOR_REJECT
}


//Test FaceSelector.insertWithTest with the HISTORY_PRIMITIVE mode
TEST(test_FaceSelector_insertWithTest, history_primitive_mode)
{

	FaceSelector<Ray> selector;
	selector.setModeHistory(HISTORY_PRIMITIVE);

	Ray* r1=new Ray();
	Ray* r2=new Ray();
	Ray* r3=new Ray();
	Ray* r4=new Ray();

	Material* mat=new Material();
	
	//init random number generator
	unsigned int seed=(unsigned int)time(NULL);
	srand(seed);
	cout<<"[          ] Random number generator initialized with seed "<<seed<<endl;

	Simulation simu;
	Scene* scene=simu.getScene();

	//Set a source
	Source src;
	src.setPosition(vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5)));
	r1->setSource(&src);
	r2->setSource(&src);
	r3->setSource(&src);
	r4->setSource(&src);

	//Set a receptor
	Recepteur rcpt(vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5)),3);
	r1->setRecepteur(&rcpt);
	r2->setRecepteur(&rcpt);
	r3->setRecepteur(&rcpt);
	r4->setRecepteur(&rcpt);

	simu.addSource(src);
	simu.addRecepteur(rcpt);

	//Randomly generate four triangles and add theme to the scene
	std::vector<Shape*> shapes;
	for(int i=0;i<4;i++){
		unsigned int p1,p2,p3;

		vec3 vertex=vec3((decimal)(rand()%100+1),(decimal)(rand()%100+1),(decimal)(rand()%100+1));
		vec3 u=vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5));
		vec3 v=vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5));
		
		scene->addVertex(vertex, p1);
		scene->addVertex(vertex+u, p2);
		scene->addVertex(vertex+v, p3);
		shapes.push_back(scene->addTriangle(p1,p2,p3,mat));
	}

	//Define some events and associate a shape and a position to each one of them
	Event* e1=new Event();
	e1->setShape(shapes[0]);
	e1->setPosition(vec3(48,-25,74));
	std::shared_ptr<Event> SPE1(e1);

	Event* e2=new Event();
	e2->setShape(shapes[1]);
	e2->setPosition(vec3(24,11,-62));
	std::shared_ptr<Event> SPE2(e2);

	Event* e3=new Event();
	e3->setShape(shapes[1]);
	e3->setPosition(vec3(37,-7,26)); 
	std::shared_ptr<Event> SPE3(e3);

	Event* e4=new Event();
	e4->setShape(shapes[2]);
	e4->setPosition(vec3(-12,88,-42));
	std::shared_ptr<Event> SPE4(e4);

	Event* e5=new Event();
	e5->setShape(shapes[2]);
	e5->setPosition(vec3(57,-18,33));
	std::shared_ptr<Event> SPE5(e5);

	Event* e6=new Event();
	e6->setShape(shapes[3]);
	e4->setPosition(vec3(78,-51,36));
	std::shared_ptr<Event> SPE6(e6);

	//Assign events to the rays
	r1->getEvents()->push_back(SPE1);
	r1->getEvents()->push_back(SPE2);
	r1->getEvents()->push_back(SPE6);
	r1->getEvents()->push_back(SPE5);

	r2->getEvents()->push_back(SPE1);
	r2->getEvents()->push_back(SPE3);
	r2->getEvents()->push_back(SPE6);
	r2->getEvents()->push_back(SPE4);

	r3->getEvents()->push_back(SPE3);
	r3->getEvents()->push_back(SPE1);
	r3->getEvents()->push_back(SPE6);
	r3->getEvents()->push_back(SPE4);
	
	r4->getEvents()->push_back(SPE2);
	r4->getEvents()->push_back(SPE1);
	r4->getEvents()->push_back(SPE6);
	r4->getEvents()->push_back(SPE5);

	EXPECT_TRUE(selector.insertWithTest(r1)); // no ray selected yet => response should return TRUE
	
	EXPECT_TRUE(selector.insertWithTest(r2)); // same primitiveId sequence as r1 but with shorter length => response should return TRUE
	
	EXPECT_TRUE(selector.insertWithTest(r3)); // different primitiveId sequence from r1 and r2 => response should return TRUE

	EXPECT_FALSE(selector.insertWithTest(r4)); // same primitiveId sequence as r3 but with longer length => response should return FALSE
}


//Test FaceSelector.insertWithTest with the HISTORY_FACE mode
TEST(test_FaceSelector_insertWithTest, history_face_mode)
{

	FaceSelector<Ray> selector;
	selector.setModeHistory(HISTORY_FACE);

	Ray* r1=new Ray();
	Ray* r2=new Ray();
	Ray* r3=new Ray();
	Ray* r4=new Ray();

	Material* mat=new Material();

	//init random number generator
	unsigned int seed=(unsigned int)time(NULL);
	srand(seed);
	cout<<"[          ] Random number generator initialized with seed "<<seed<<endl;

	Simulation simu;
	Scene* scene=simu.getScene();

	//Set a source
	Source src;
	src.setPosition(vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5)));
	r1->setSource(&src);
	r2->setSource(&src);
	r3->setSource(&src);
	r4->setSource(&src);

	//Set a receptor
	Recepteur rcpt(vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5)),3);
	r1->setRecepteur(&rcpt);
	r2->setRecepteur(&rcpt);
	r3->setRecepteur(&rcpt);
	r4->setRecepteur(&rcpt);

	simu.addSource(src);
	simu.addRecepteur(rcpt);


	//Randomly generate four triangles with a different shapeId and add theme to the scene
	std::vector<Shape*> shapes;
	for(int i=0;i<4;i++){
		unsigned int p1,p2,p3;

		vec3 vertex=vec3((decimal)(rand()%100+1),(decimal)(rand()%100+1),(decimal)(rand()%100+1));
		vec3 u=vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5));
		vec3 v=vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5));
		
		scene->addVertex(vertex, p1);
		scene->addVertex(vertex+u, p2);
		scene->addVertex(vertex+v, p3);
		Shape* shape=scene->addTriangle(p1,p2,p3,mat);
		shape->setFaceId(i);
		shapes.push_back(shape);
	}

	//Define some events and associate a shape and a position to each one of them
	Event* e1=new Event();
	e1->setShape(shapes[0]);
	e1->setPosition(vec3(48,-25,74));
	std::shared_ptr<Event> SPE1(e1);

	Event* e2=new Event();
	e2->setShape(shapes[1]);
	e2->setPosition(vec3(24,11,-62));
	std::shared_ptr<Event> SPE2(e2);

	Event* e3=new Event();
	e3->setShape(shapes[1]);
	e3->setPosition(vec3(37,-7,26)); 
	std::shared_ptr<Event> SPE3(e3);

	Event* e4=new Event();
	e4->setShape(shapes[2]);
	e4->setPosition(vec3(-12,88,-42));
	std::shared_ptr<Event> SPE4(e4);

	Event* e5=new Event();
	e5->setShape(shapes[2]);
	e5->setPosition(vec3(57,-18,33));
	std::shared_ptr<Event> SPE5(e5);

	Event* e6=new Event();
	e6->setShape(shapes[3]);
	e4->setPosition(vec3(78,-51,36));
	std::shared_ptr<Event> SPE6(e6);

	//Assign events to the rays
	r1->getEvents()->push_back(SPE1);
	r1->getEvents()->push_back(SPE2);
	r1->getEvents()->push_back(SPE6);
	r1->getEvents()->push_back(SPE5);

	r2->getEvents()->push_back(SPE1);
	r2->getEvents()->push_back(SPE3);
	r2->getEvents()->push_back(SPE6);
	r2->getEvents()->push_back(SPE4);

	r3->getEvents()->push_back(SPE3);
	r3->getEvents()->push_back(SPE1);
	r3->getEvents()->push_back(SPE6);
	r3->getEvents()->push_back(SPE4);
	
	r4->getEvents()->push_back(SPE2);
	r4->getEvents()->push_back(SPE1);
	r4->getEvents()->push_back(SPE6);
	r4->getEvents()->push_back(SPE5);

	EXPECT_TRUE(selector.insertWithTest(r1)); // no ray selected yet => response should return TRUE
	
	EXPECT_TRUE(selector.insertWithTest(r2)); // same primitiveId sequence as r1 but with shorter length => response should return TRUE
	
	EXPECT_TRUE(selector.insertWithTest(r3)); // different primitiveId sequence from r1 and r2 => response should return TRUE

	EXPECT_FALSE(selector.insertWithTest(r4)); // same primitiveId sequence as r3 but with longer length => response should return FALSE
}




/***********************************************************************
							 CoPlanaritySelector
************************************************************************/


TEST(test_CoPlanaritySelector_areBothReflections, event_types){


	CoPlanaritySelector<Ray> selector;

	// There can only be co-planarity between two specular reflections
	EXPECT_TRUE(selector.areBothReflections(new SpecularReflexion(),new SpecularReflexion()));

	EXPECT_FALSE(selector.areBothReflections(new SpecularReflexion(),new Diffraction()));
	EXPECT_FALSE(selector.areBothReflections(new SpecularReflexion(),new Event()));
	EXPECT_FALSE(selector.areBothReflections(new Diffraction(),new Event()));
	EXPECT_FALSE(selector.areBothReflections(new Diffraction(),new Diffraction()));
	EXPECT_FALSE(selector.areBothReflections(new Event(),new Event()));

}


TEST(test_CoPlanaritySelector_coPlanarityTest, different_normals)
{
	int test_i=0;
	CoPlanaritySelector<Ray> selector;
	while(test_i<NTESTS){	// Perform NTESTS rand tests
		test_i++;

		// Generate a first random unitary vector used as normal
		vec3 n1=rand_vec3();

		// Generate a second random unitary different from the first one
		vec3 n2(n1);
		int max_retry = MAX_RETRY;
		while(abs(n1*n2-1)<EPSILON_4 && max_retry>0){
			n2 = rand_vec3();
			max_retry--;
		}

		ASSERT_TRUE(max_retry>0);

		// Generate a third vector normal to the two first ones
		vec3 v;
		Cross(n1,n2,v);

		vec3 p1=rand_vec3(false);
		Triangle s1;
		s1.setNormal(n1);
		Event* e1=new SpecularReflexion();
		e1->setPosition(p1);
		e1->setShape(&s1);
	
		vec3 p2=rand_vec3(false);
		Triangle s2;
		s2.setNormal(n2);
		Event* e2=new SpecularReflexion();
		e2->setPosition(p2);
		e2->setShape(&s2);

		// Normals are different => not co-planar
		ASSERT_FALSE(*e1->getShape()->getNormal() == *e2->getShape()->getNormal());
		ASSERT_FALSE(selector.coPlanarityTest(e1,e2)); 

	}
}

TEST(test_CoPlanaritySelector_coPlanarityTest, equal_normals_and_not_coplanar)
{
	int test_i=0;
	CoPlanaritySelector<Ray> selector;
	while(test_i<NTESTS){	// Perform NTESTS rand tests
		test_i++;

		// Generate a first random unitary vector used as normal
		vec3 n1= rand_vec3();

		// Generate a second vector not normal to the first one
		vec3 v=rand_vec3(false);
		int max_retry = 100;
		while(abs(v*n1-1)<EPSILON_4 && max_retry>0){
			v = rand_vec3(false);
			max_retry--;
		}

		ASSERT_TRUE(max_retry>0);

		vec3 p1 = rand_vec3(false);
		Triangle s1;
		s1.setNormal(n1);
		Event* e1=new SpecularReflexion();
		e1->setPosition(p1);
		e1->setShape(&s1);
	

		Triangle s2;
		s2.setNormal(n1);
		Event* e2=new SpecularReflexion();
		e2->setPosition(p1+v);
		e2->setShape(&s2);

		// Same normals but not co-planar
		ASSERT_TRUE(*e1->getShape()->getNormal() == *e2->getShape()->getNormal());
		ASSERT_FALSE(selector.coPlanarityTest(e1,e2)); 

	}
}

TEST(test_CoPlanaritySelector_coPlanarityTest, equal_normals_and_coplanar)
{
	int test_i=0;
	CoPlanaritySelector<Ray> selector;
	while(test_i<NTESTS){	// Perform NTESTS rand tests
		test_i++;

		// Generate a first random unitary vector used as normal
		vec3 n1=rand_vec3();

		// Generate a second random unitary vector different from the first one
		vec3 n2(n1);
		int max_retry=MAX_RETRY;
		while(abs(n1*n2-1)<EPSILON_4 && max_retry>0){
			n2 = rand_vec3();
			max_retry--;
		}

		EXPECT_GT(max_retry,0);

		// Generate a third vector normal to the first two
		vec3 v;
		Cross(n1,n2,v);

		vec3 p1= rand_vec3(false);

		Triangle s1;
		s1.setNormal(n1);
		Event* e1=new SpecularReflexion();
		e1->setPosition(p1);
		e1->setShape(&s1);
	

		Triangle s2;
		s2.setNormal(n1);
		Event* e2=new SpecularReflexion();
		e2->setPosition(p1+v);
		e2->setShape(&s2);

		ASSERT_TRUE(selector.coPlanarityTest(e1,e2));

	}
}


TEST(test_CoPlanaritySelector_haveCoPlanarEvents, no_events)
{

	CoPlanaritySelector<Ray> selector;

	Ray* r1=new Ray();
	Ray* r2=new Ray();

	ASSERT_FALSE(selector.haveCoPlanarEvents(r1,r2));

}

TEST(test_CoPlanaritySelector_haveCoPlanarEvents, no_coplanar_events)
{

	int test_i=0;
	CoPlanaritySelector<Ray> selector;

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	src.setInitialRayCount(1000);
	
	
	//Set a receptor
	Recepteur rcpt(vec3(10,1,0),3);
	

	while(test_i<NTESTS){	// Perform NTESTS rand tests
		test_i++;

		unsigned int nb_events=rand()%MAX_EVENTS+1;

		Ray* r1=new Ray();
		Ray* r2=new Ray();

		r1->setSource(&src);
		r1->setRecepteur(&rcpt);
		r2->setSource(&src);
		r2->setRecepteur(&rcpt);

		for(unsigned int i=0;i<nb_events;i++){
			Event* ev1=new SpecularReflexion();
			Event* ev2=new SpecularReflexion();

			// Generate a first random unitary vector used as normal
			vec3 n1= rand_vec3();

		
			// Generate a second vector different from the first one and not normal to it
			vec3 v=rand_vec3(false);
			int max_retry = 100;
			while(abs(v*n1-1)<EPSILON_4 && max_retry>0){
				v = rand_vec3(false);
				max_retry--;
			}

			ASSERT_TRUE(max_retry>0);


			vec3 p1 = rand_vec3(false);
			Triangle s1;
			s1.setNormal(n1);
			ev1->setPosition(p1);
			ev1->setShape(&s1);
	

			Triangle s2;
			s2.setNormal(n1);
			ev2->setPosition(p1+v);
			ev2->setShape(&s2);

			// Same normals but not co-planar
			ASSERT_TRUE(*ev1->getShape()->getNormal() == *ev2->getShape()->getNormal());

		}
		ASSERT_FALSE(selector.haveCoPlanarEvents(r1,r2)); 
	}
}


TEST(test_CoPlanaritySelector_haveCoPlanarEvents, with_coplanar_events)
{

	int test_i=0;
	CoPlanaritySelector<Ray> selector;

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	src.setInitialRayCount(1000);
	
	//Set a receptor
	Recepteur rcpt(vec3(10,1,0),3);
	

	while(test_i<NTESTS){	// Perform NTESTS rand tests
		test_i++;

		unsigned int nb_events=rand()%MAX_EVENTS+1;

		Ray* r1=new Ray();
		Ray* r2=new Ray();

		r1->setSource(&src);
		r1->setRecepteur(&rcpt);

		r2->setSource(&src);
		r2->setRecepteur(&rcpt);

		for(unsigned int i=0;i<nb_events;i++){
			Event* ev1=new SpecularReflexion();
			Event* ev2=new SpecularReflexion();

			// Generate a first random unitary vector used as normal
			vec3 n1= rand_vec3();

			// Generate a second random unitary vector different from the first one
			vec3 n2(n1);
			int retry=MAX_RETRY;
			while(abs(n1*n2-1)<EPSILON_4 && retry>0){
				n2 = rand_vec3();
				retry--;
			}

			ASSERT_TRUE(retry>0);

			// Generate a third vector normal to the first two
			vec3 v;
			Cross(n1,n2,v);

			vec3 p1 = rand_vec3(false);
			Triangle* s1 = new Triangle();
			s1->setNormal(n1);
			ev1->setPosition(p1);
			ev1->setShape(s1);
	

			Triangle* s2= new Triangle();
			s2->setNormal(n1);
			ev2->setPosition(p1+v);
			ev2->setShape(s2);

			std::shared_ptr<Event> SPE1(ev1);
			r1->getEvents()->push_back(SPE1);

			std::shared_ptr<Event> SPE2(ev2);
			r2->getEvents()->push_back(SPE2);
			

		}
		ASSERT_TRUE(selector.haveCoPlanarEvents(r1,r2)); 
	}
}

/***********************************************************************
							 FermatSelector
************************************************************************/

//Test FermatSelector.canBeInserted REJECTS rays that hit receptors that do not lie in the ray's thickness
TEST(test_FermatSelector_canBeInserted, receptor_outside_thickness_1)
{

	FermatSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;
	
	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	src.setInitialRayCount(1000);
	r->setSource(&src);
	
	//Set a receptor
	Recepteur rcpt(vec3(10,1,0),3);
	r->setRecepteur(&rcpt);

	Intersection result;
	float tmax=2000;
	vec3 dir=rcpt.getPosition()-src.getPosition()+vec3(0,1,0); // direction from source to receptor + some deviation
	dir.normalize();
	r->setDirection(dir);
	r->setPosition(src.getPosition());

	vec3 position=r->getPosition();
   	vec3 direction=r->getDirection();
	rcpt.intersectionRecepteur(position, direction, tmax, result);
	r->setFinalPosition ( r->getPosition() + r->getDirection() * result.t);

	EXPECT_TRUE(result.t<tmax);  //the ray should hit the receptor
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace); //the receptor does not lie in the ray's thickness => response should be SELECTOR_REJECT
	EXPECT_EQ(SELECTOR_REJECT,response);  

}


//Test FaceSelector.canBeInserted REJECTS rays that hit receptors that do not lie in the ray's thickness
TEST(test_FermatSelector_canBeInserted, receptor_outside_thickness_2)
{

	FermatSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;
	
	//Set a source
	Source src;
	src.setPosition(vec3(50,-200,150));
	src.setInitialRayCount(1000);
	r->setSource(&src);
	
	//Set a receptor
	Recepteur rcpt(vec3(60,-210,140),3);
	r->setRecepteur(&rcpt);

	Intersection result;
	float tmax=2000;
	vec3 dir=rcpt.getPosition()-src.getPosition()+vec3(2,0,0); // direction from source to receptor + some deviation
	dir.normalize();
	r->setDirection(dir);
	r->setPosition(src.getPosition());

	vec3 position=r->getPosition();
   	vec3 direction=r->getDirection();
	rcpt.intersectionRecepteur(position, direction, tmax, result);
	r->setFinalPosition ( r->getPosition() + r->getDirection() * result.t);

	EXPECT_TRUE(result.t<tmax);  //the ray should hit the receptor
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace); //the receptor does not lie in the ray's thickness => response should be SELECTOR_REJECT
	EXPECT_EQ(SELECTOR_REJECT,response);  

}

//Test FaceSelector.canBeInserted ACCEPTS rays that hit receptors that lie in the ray's thickness
TEST(test_FermatSelector_canBeInserted, receptor_inside_thickness_1)
{

	FermatSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;
	
	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	src.setInitialRayCount(1000);
	r->setSource(&src);
	
	//Set a receptor
	Recepteur rcpt(vec3(1000,0,0),3);
	r->setRecepteur(&rcpt);

	Intersection result;
	float tmax=2000;
	vec3 dir=rcpt.getPosition()-src.getPosition(); // direction from source to receptor
	dir.normalize();
	r->setDirection(dir);
	r->setPosition(src.getPosition());

	vec3 position=r->getPosition();
   	vec3 direction=r->getDirection();
	rcpt.intersectionRecepteur(position, direction, tmax, result);
	r->setFinalPosition ( r->getPosition() + r->getDirection() * result.t);

	EXPECT_TRUE(result.t<tmax);  //the ray should hit the receptor
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace); //the receptor lies in the ray's thickness => response should be SELECTOR_ACCEPT
	EXPECT_EQ(SELECTOR_ACCEPT,response);  

}

//Test FaceSelector.canBeInserted ACCEPTS rays that hit receptors that lie in the ray's thickness
TEST(test_FermatSelector_canBeInserted, receptor_inside_thickness_2)
{

	FermatSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;
	
	//Set a source
	Source src;
	src.setPosition(vec3(50,-200,150));
	src.setInitialRayCount(1000);
	r->setSource(&src);
	
	//Set a receptor
	Recepteur rcpt(vec3(-800,450,300),3);
	r->setRecepteur(&rcpt);

	Intersection result;
	float tmax=2000;
	vec3 dir=rcpt.getPosition()-src.getPosition(); // direction from source to receptor 
	dir.normalize();
	r->setDirection(dir);
	r->setPosition(src.getPosition());

	vec3 position=r->getPosition();
   	vec3 direction=r->getDirection();
	rcpt.intersectionRecepteur(position, direction, tmax, result);
	r->setFinalPosition ( r->getPosition() + r->getDirection() * result.t);

	EXPECT_TRUE(result.t<tmax);  //the ray should hit the receptor
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace); //the receptor lies in the ray's thickness => response should be SELECTOR_ACCEPT
	EXPECT_EQ(SELECTOR_ACCEPT,response);  

}

//Test FaceSelector.insertWithTest returns FALSE for rays that hit receptors that do not lie in the ray's thickness
TEST(test_FermatSelector_insertWithTest, receptor_outside_thickness_1)
{

	FermatSelector<Ray> selector;
	Ray* r=new Ray();

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	src.setInitialRayCount(1000);
	r->setSource(&src);
	
	//Set a receptor
	Recepteur rcpt(vec3(10,1,0),3);
	r->setRecepteur(&rcpt);

	Intersection result;
	float tmax=2000;
	vec3 dir=rcpt.getPosition()-src.getPosition()+vec3(0,1,0); // direction from source to receptor + some deviation
	dir.normalize();
	r->setDirection(dir);
	r->setPosition(src.getPosition());

	vec3 position=r->getPosition();
   	vec3 direction=r->getDirection();
	rcpt.intersectionRecepteur(position, direction, tmax, result);
	r->setFinalPosition ( r->getPosition() + r->getDirection() * result.t);

	EXPECT_TRUE(result.t<tmax);  //the ray should hit the receptor
	EXPECT_FALSE(selector.insertWithTest(r)); //the receptor does not lie in the ray's thickness => should return FALSE

}


//Test FaceSelector.insertWithTest returns FALSE for rays that hit receptors that do not lie in the ray's thickness
TEST(test_FermatSelector_insertWithTest, receptor_outside_thickness_2)
{

	FermatSelector<Ray> selector;
	Ray* r=new Ray();
	
	//Set a source
	Source src;
	src.setPosition(vec3(50,-200,150));
	src.setInitialRayCount(1000);
	r->setSource(&src);
	
	//Set a receptor
	Recepteur rcpt(vec3(60,-210,140),3);
	r->setRecepteur(&rcpt);

	Intersection result;
	float tmax=2000;
	vec3 dir=rcpt.getPosition()-src.getPosition()+vec3(2,0,0); // direction from source to receptor + some deviation
	dir.normalize();
	r->setDirection(dir);
	r->setPosition(src.getPosition());

	vec3 position=r->getPosition();
   	vec3 direction=r->getDirection();
	rcpt.intersectionRecepteur(position, direction, tmax, result);
	r->setFinalPosition ( r->getPosition() + r->getDirection() * result.t);

	EXPECT_TRUE(result.t<tmax);  //the ray should hit the receptor
	EXPECT_FALSE(selector.insertWithTest(r)); //the receptor does not lie in the ray's thickness => should return FALSE

}

//Test FaceSelector.insertWithTest returns TRUE for rays that hit receptors that do not lie in the ray's thickness
TEST(test_FermatSelector_insertWithTest, receptor_inside_thickness_1)
{

	FermatSelector<Ray> selector;
	Ray* r=new Ray();
	
	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	src.setInitialRayCount(1000);
	r->setSource(&src);
	
	//Set a receptor
	Recepteur rcpt(vec3(1000,0,0),3);
	r->setRecepteur(&rcpt);

	Intersection result;
	float tmax=2000;
	vec3 dir=rcpt.getPosition()-src.getPosition(); // direction from source to receptor
	dir.normalize();
	r->setDirection(dir);
	r->setPosition(src.getPosition());

	vec3 position=r->getPosition();
   	vec3 direction=r->getDirection();
	rcpt.intersectionRecepteur(position, direction, tmax, result);
	r->setFinalPosition ( r->getPosition() + r->getDirection() * result.t);
	
	EXPECT_TRUE(result.t<tmax);  //the ray should hit the receptor
	EXPECT_TRUE(selector.insertWithTest(r)); //the receptor does not lie in the ray's thickness => should return TRUE

}

//Test FaceSelector.insertWithTest returns TRUE for rays that hit receptors that do not lie in the ray's thickness
TEST(test_FermatSelector_insertWithTest, receptor_inside_thickness_2)
{

	FermatSelector<Ray> selector;
	Ray* r=new Ray();
	
	//Set a source
	Source src;
	src.setPosition(vec3(50,-200,150));
	src.setInitialRayCount(1000);
	r->setSource(&src);
	
	//Set a receptor
	Recepteur rcpt(vec3(-800,450,300),3);
	r->setRecepteur(&rcpt);

	Intersection result;
	float tmax=2000;
	vec3 dir=rcpt.getPosition()-src.getPosition(); // direction from source to receptor 
	dir.normalize();
	r->setDirection(dir);
	r->setPosition(src.getPosition());

	vec3 position=r->getPosition();
   	vec3 direction=r->getDirection();
	rcpt.intersectionRecepteur(position, direction, tmax, result);
	r->setFinalPosition ( r->getPosition() + r->getDirection() * result.t);

	EXPECT_TRUE(result.t<tmax);  //the ray should hit the receptor
	EXPECT_TRUE(selector.insertWithTest(r)); //the receptor does not lie in the ray's thickness => should return TRUE

}

/***********************************************************************
							 LengthSelector
************************************************************************/

//Test LengthSelector.canBeInserted with the LESS operator
TEST(test_LengthSelector_canBeInserted, less)
{

	LengthSelector<Ray> selector(1000,LESS);
	Ray* r=new Ray();
	unsigned long long replace;

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	r->setSource(&src);

	//Set receptors
	Recepteur rcpt1(vec3(800,0,0),3);
	Recepteur rcpt2(vec3(0,1000,0),3);
	
	r->setRecepteur(&rcpt1);
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 800 < 1000 => response should be SELECTOR_ACCEPT
	
	r->setRecepteur(&rcpt2);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 1000 !< 1000 => response should be SELECTOR_REJECT
}

//Test LengthSelector.canBeInserted with the LESS_OR_EQUAL operator
TEST(test_LengthSelector_canBeInserted, less_or_equal)
{

	LengthSelector<Ray> selector; //default max_length=2000 and default operator=LESS_OR_EQUAL
	Ray* r=new Ray();
	unsigned long long replace;

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	r->setSource(&src);

	//Set receptors
	Recepteur rcpt1(vec3(1999,0,0),3);
	Recepteur rcpt2(vec3(0,2000,0),3);
	Recepteur rcpt3(vec3(0,0,2001),3);

	r->setRecepteur(&rcpt1);
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 1999 <= 2000 => response should be SELECTOR_ACCEPT
	
	r->setRecepteur(&rcpt2);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 2000 <= 2000 => response should be SELECTOR_ACCEPT

	r->setRecepteur(&rcpt3);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 2001 !<= 2000 => response should be SELECTOR_REJECT
}


//Test LengthSelector.canBeInserted with the EQUAL operator
TEST(test_LengthSelector_canBeInserted, equal)
{

	LengthSelector<Ray> selector(700,EQUAL);
	Ray* r=new Ray();
	unsigned long long replace;

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	r->setSource(&src);

	//Set receptors
	Recepteur rcpt1(vec3(699,0,0),3);
	Recepteur rcpt2(vec3(0,700,0),3);
	Recepteur rcpt3(vec3(0,0,701),3);

	r->setRecepteur(&rcpt1);
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 699 != 700 => response should be SELECTOR_REJECT
	
	r->setRecepteur(&rcpt2);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 700 == 700 => response should be SELECTOR_ACCEPT

	r->setRecepteur(&rcpt3);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 701 != 700 => response should be SELECTOR_REJECT
}

//Test LengthSelector.canBeInserted with the GREATER_OR_EQUAL operator
TEST(test_LengthSelector_canBeInserted, greater_or_equal)
{

	LengthSelector<Ray> selector(900,GREATER_OR_EQUAL);
	Ray* r=new Ray();
	unsigned long long replace;

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	r->setSource(&src);

	//Set receptors
	Recepteur rcpt1(vec3(950,0,0),3);
	Recepteur rcpt2(vec3(0,850,0),3);
	Recepteur rcpt3(vec3(0,0,900),3);

	r->setRecepteur(&rcpt1);
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 950 >= 900 => response should be SELECTOR_ACCEPT
	
	r->setRecepteur(&rcpt2);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 850 !>= 900 => response should be SELECTOR_REJECT

	r->setRecepteur(&rcpt3);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 900 >= 900 => response should be SELECTOR_ACCEPT
}


//Test LengthSelector.canBeInserted with the GREATER operator
TEST(test_LengthSelector_canBeInserted, greater)
{

	LengthSelector<Ray> selector(300,GREATER);
	Ray* r=new Ray();
	unsigned long long replace;

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	r->setSource(&src);

	//Set receptors
	Recepteur rcpt1(vec3(300,0,0),3);
	Recepteur rcpt2(vec3(301,0,0),3);

	r->setRecepteur(&rcpt1);
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 300 !> 300 => response should be SELECTOR_REJECT
	
	r->setRecepteur(&rcpt2);
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 301 > 300 => response should be SELECTOR_ACCEPT

}


//Test LengthSelector.insertWithTest with the LESS operator
TEST(test_LengthSelector_insertWithTest, less)
{

	LengthSelector<Ray> selector(850,LESS);
	Ray* r=new Ray();

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	r->setSource(&src);

	//Set receptors
	Recepteur rcpt1(vec3(700,0,0),3);
	Recepteur rcpt2(vec3(0,850,0),3);
	
	r->setRecepteur(&rcpt1);
	EXPECT_TRUE(selector.insertWithTest(r));	// 700 < 850 => should return TRUE
	
	r->setRecepteur(&rcpt2);
	EXPECT_FALSE(selector.insertWithTest(r));   // 850 !< 850 => should return FALSE
}

//Test LengthSelector.insertWithTest with the LESS_OR_EQUAL operator
TEST(test_LengthSelector_insertWithTest, less_or_equal)
{

	LengthSelector<Ray> selector(450,LESS_OR_EQUAL);
	Ray* r=new Ray();

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	r->setSource(&src);

	//Set receptors
	Recepteur rcpt1(vec3(400,0,0),3);
	Recepteur rcpt2(vec3(0,500,0),3);
	Recepteur rcpt3(vec3(0,0,450),3);

	r->setRecepteur(&rcpt1);
	EXPECT_TRUE(selector.insertWithTest(r));   // 400 <= 450 => should return TRUE
	
	r->setRecepteur(&rcpt2);
	EXPECT_FALSE(selector.insertWithTest(r));  // 500 !<= 450 => should return FALSE

	r->setRecepteur(&rcpt3);
	EXPECT_TRUE(selector.insertWithTest(r));   // 450 <= 450 => should return TRUE
	
}


//Test LengthSelector.insertWithTest with the EQUAL operator
TEST(test_LengthSelector_insertWithTest, equal)
{

	LengthSelector<Ray> selector(325,EQUAL);
	Ray* r=new Ray();

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	r->setSource(&src);

	//Set receptors
	Recepteur rcpt1(vec3(326,0,0),3);
	Recepteur rcpt2(vec3(0,324,0),3);
	Recepteur rcpt3(vec3(0,0,325),3);

	r->setRecepteur(&rcpt1);
	EXPECT_FALSE(selector.insertWithTest(r));   // 326 != 325 => should return FALSE

	r->setRecepteur(&rcpt2);
	EXPECT_FALSE(selector.insertWithTest(r));   // 324 != 325 => should return FALSE

	r->setRecepteur(&rcpt3);
	EXPECT_TRUE(selector.insertWithTest(r));	// 325 == 325 => should return TRUE
}

//Test LengthSelector.insertWithTest with the GREATER_OR_EQUAL operator
TEST(test_LengthSelector_insertWithTest, greater_or_equal)
{

	LengthSelector<Ray> selector(260,GREATER_OR_EQUAL);
	Ray* r=new Ray();

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	r->setSource(&src);

	//Set receptors
	Recepteur rcpt1(vec3(261,0,0),3);
	Recepteur rcpt2(vec3(0,259,0),3);
	Recepteur rcpt3(vec3(0,0,260),3);

	r->setRecepteur(&rcpt1);
	EXPECT_TRUE(selector.insertWithTest(r));   // 261 >= 260 => should return TRUE
	
	r->setRecepteur(&rcpt2);
	EXPECT_FALSE(selector.insertWithTest(r));  // 259 !>= 260 => should return FALSE

	r->setRecepteur(&rcpt3);
	EXPECT_TRUE(selector.insertWithTest(r));   // 260 >= 260 => should return TRUE
}


//Test LengthSelector.insertWithTest with the GREATER operator
TEST(test_LengthSelector_insertWithTest, greater)
{

	LengthSelector<Ray> selector(630,GREATER);
	Ray* r=new Ray();

	//Set a source
	Source src;
	src.setPosition(vec3(0,0,0));
	r->setSource(&src);

	//Set receptors
	Recepteur rcpt1(vec3(630,0,0),3);
	Recepteur rcpt2(vec3(631,0,0),3);

	r->setRecepteur(&rcpt1);
	EXPECT_FALSE(selector.insertWithTest(r));   // 630 !> 630 => should return FALSE
	
	r->setRecepteur(&rcpt2);
	EXPECT_TRUE(selector.insertWithTest(r));	// 631 > 630 => should return TRUE

}




/***********************************************************************
							 ReflectionSelector
************************************************************************/

//Test ReflectionSelector.canBeInserted with the LESS operator
TEST(test_ReflectionSelector_canBeInserted, less)
{

	ReflectionSelector<Ray> selector;
	selector.setOperator(LESS);
	selector.setMaximumReflectionOrder(5);

	Ray* r=new Ray();
	unsigned long long replace;

	r->setNbReflexion(4); 
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 4 < 5 => response should be SELECTOR_ACCEPT

	r->setNbReflexion(5); 
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 5 !< 5 => response should be SELECTOR_REJECT
}

//Test ReflectionSelector.canBeInserted with the LESS_OR_EQUAL
TEST(test_ReflectionSelector_canBeInserted, less_or_equal)
{

	ReflectionSelector<Ray> selector;
	selector.setMaximumReflectionOrder(10);
	Ray* r=new Ray();
	unsigned long long replace;

	r->setNbReflexion(6); 
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 6 <= 10 => response should be SELECTOR_ACCEPT

	r->setNbReflexion(10); 
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 10 <= 10 => response should be SELECTOR_ACCEPT

	r->setNbReflexion(15); 
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 15 !<= 10 => response should be SELECTOR_REJECT
}


//Test ReflectionSelector.canBeInserted with the EQUAL
TEST(test_ReflectionSelector_canBeInserted, equal)
{

	ReflectionSelector<Ray> selector;
	selector.setOperator(EQUAL);
	selector.setMaximumReflectionOrder(7);
	Ray* r=new Ray();
	unsigned long long replace;

	r->setNbReflexion(7); 
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 7 == 7 => response should be SELECTOR_ACCEPT

	r->setNbReflexion(3); 
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 3 != 7 => response should be SELECTOR_REJECT
}


//Test ReflectionSelector.canBeInserted with the GREATER_OR_EQUAL
TEST(test_ReflectionSelector_canBeInserted, greater_or_equal)
{

	ReflectionSelector<Ray> selector;
	selector.setOperator(GREATER_OR_EQUAL);
	Ray* r=new Ray();
	unsigned long long replace;

	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 0 !>= 1 => response should be SELECTOR_REJECT

	r->setNbReflexion(1); 
    response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 1 >= 1 => response should be SELECTOR_ACCEPT

	r->setNbReflexion(2); 
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); // 2 >= 1 => response should be SELECTOR_ACCEPT
}


//Test ReflectionSelector.canBeInserted with the GREATER operator
TEST(test_ReflectionSelector_canBeInserted, greater)
{

	ReflectionSelector<Ray> selector;
	selector.setOperator(GREATER);
	selector.setMaximumReflectionOrder(8);
	Ray* r=new Ray();
	unsigned long long replace;

	r->setNbReflexion(9); 
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //9 > 8 => response should be SELECTOR_ACCEPT

	r->setNbReflexion(8); 
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); // 8 !> 8 => response should be SELECTOR_REJECT
}


//Test ReflectionSelector.canBeInserted with ground reflexions
TEST(test_ReflectionSelector_canBeInserted, ground_reflection)
{

	ReflectionSelector<Ray> selector;
	Ray* r=new Ray();
	unsigned long long replace;
	Material* mat=new Material();
	
	Simulation simu;
	Scene* scene=simu.getScene();
	unsigned int p1,p2,p3,p4,p5,p6;

	//Set two shapes for the events
	scene->addVertex(vec3(1,0,0), p1);
	scene->addVertex(vec3(0,1,0), p2);
	scene->addVertex(vec3(0,0,1), p3);

	scene->addVertex(vec3(-1,0,0), p4);
	scene->addVertex(vec3(0,-1,0), p5);
	scene->addVertex(vec3(0,0,-1), p6);

	Shape* shape1=scene->addTriangle(p1,p2,p3,mat);
	Shape* shape2=scene->addTriangle(p4,p5,p6,mat);


	//Set events
	SpecularReflexion* e1=new SpecularReflexion();
	e1->setShape(shape1);
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	SpecularReflexion* e2=new SpecularReflexion();
	e2->setShape(shape2);
	std::shared_ptr<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	Diffraction* e3=new Diffraction();
	e3->setShape(shape1);
	std::shared_ptr<Event> SPE3(e3);

	r->setNbReflexion(1); 
	SELECTOR_RESPOND response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //1 <= 1 => response should be SELECTOR_ACCEPT

	shape1->setIsSol(true); //flag first event's shape as ground
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_REJECT,response); //ray is reflected on the ground => response should be SELECTOR_REJECT

	selector.setGroundAccepted(true); //accept ground reflexion
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //selector set to accept ground reflexions => response should be SELECTOR_ACCEPT

	r->getEvents()->at(0)=SPE3; //replace first reflexion by a diffraction
	selector.setGroundAccepted(false); //refuse ground reflexion
	response=selector.canBeInserted(r,replace);
	EXPECT_EQ(SELECTOR_ACCEPT,response); //the event interacting with the ground is a diffraction => response should be SELECTOR_ACCEPT
}

//Test ReflectionSelector.insertWithTest with the LESS operator
TEST(test_ReflectionSelector_insertWithTest, less)
{

	ReflectionSelector<Ray> selector;
	selector.setOperator(LESS);
	selector.setMaximumReflectionOrder(9);
	Ray* r=new Ray();

	r->setNbReflexion(5); 
	EXPECT_TRUE(selector.insertWithTest(r));  // 5 < 9 =>  should return TRUE

	r->setNbReflexion(9); 
	EXPECT_FALSE(selector.insertWithTest(r)); // 9 !< 9 =>  should return FALSE
}

//Test ReflectionSelector.insertWithTest with the LESS_OR_EQUAL
TEST(test_ReflectionSelector_insertWithTest, less_or_equal)
{

	ReflectionSelector<Ray> selector;
	Ray* r=new Ray();

	EXPECT_TRUE(selector.insertWithTest(r));  // 0 <= 1 =>  should return TRUE

	r->setNbReflexion(1); 
	EXPECT_TRUE(selector.insertWithTest(r));  // 1 <= 1 => should return TRUE

	r->setNbReflexion(2); 
	EXPECT_FALSE(selector.insertWithTest(r)); // 2 !<= 2 => should return FALSE
}


//Test ReflectionSelector.insertWithTest with the EQUAL
TEST(test_ReflectionSelector_insertWithTest, equal)
{

	ReflectionSelector<Ray> selector;
	selector.setOperator(EQUAL);
	selector.setMaximumReflectionOrder(15);
	Ray* r=new Ray();

	r->setNbReflexion(15); 
	EXPECT_TRUE(selector.insertWithTest(r));  // 15==15 =>  should return TRUE

	r->setNbReflexion(8); 
	EXPECT_FALSE(selector.insertWithTest(r)); // 8 != 15 => should return FALSE
}


//Test ReflectionSelector.insertWithTest with the GREATER_OR_EQUAL
TEST(test_ReflectionSelector_insertWithTest, greater_or_equal)
{

	ReflectionSelector<Ray> selector;
	selector.setOperator(GREATER_OR_EQUAL);
	selector.setMaximumReflectionOrder(6);
	Ray* r=new Ray();

	r->setNbReflexion(7); 
	EXPECT_TRUE(selector.insertWithTest(r));  //7 >= 6 => should return TRUE

	r->setNbReflexion(6); 
	EXPECT_TRUE(selector.insertWithTest(r));  // 6 >= 6 => should return TRUE

	r->setNbReflexion(2); 
	EXPECT_FALSE(selector.insertWithTest(r)); // 2 !>= 6 => should return FALSE
}


//Test ReflectionSelector.insertWithTest with the GREATER operator
TEST(test_ReflectionSelector_insertWithTest, greater)
{

	ReflectionSelector<Ray> selector;
	selector.setOperator(GREATER);
	selector.setMaximumReflectionOrder(12);
	Ray* r=new Ray();

	r->setNbReflexion(13); 
	EXPECT_TRUE(selector.insertWithTest(r));  // 13 > 12 => should return TRUE

	r->setNbReflexion(12); 
	EXPECT_FALSE(selector.insertWithTest(r)); // 12 !> 12 => should return FALSE
}


//Test ReflectionSelector.insertWithTest with ground reflexions
TEST(test_ReflectionSelector_insertWithTest, ground_reflection)
{

	ReflectionSelector<Ray> selector;
	Ray* r=new Ray();

	Simulation simu;
	Scene* scene=simu.getScene();
	unsigned int p1,p2,p3,p4,p5,p6;
	Material* mat=new Material();
	
	//Set two shapes for the events
	scene->addVertex(vec3(1,0,0), p1);
	scene->addVertex(vec3(0,1,0), p2);
	scene->addVertex(vec3(0,0,1), p3);

	scene->addVertex(vec3(-1,0,0), p4);
	scene->addVertex(vec3(0,-1,0), p5);
	scene->addVertex(vec3(0,0,-1), p6);

	Shape* shape1=scene->addTriangle(p1,p2,p3,mat);
	Shape* shape2=scene->addTriangle(p4,p5,p6,mat);


	//Set events
	SpecularReflexion* e1=new SpecularReflexion();
	e1->setShape(shape1);
	std::shared_ptr<Event> SPE1(e1);
	r->getEvents()->push_back(SPE1);

	SpecularReflexion* e2=new SpecularReflexion();
	e2->setShape(shape2);
	std::shared_ptr<Event> SPE2(e2);
	r->getEvents()->push_back(SPE2);

	Diffraction* e3=new Diffraction();
	e3->setShape(shape1);
	std::shared_ptr<Event> SPE3(e3);

	r->setNbReflexion(1); 
	EXPECT_TRUE(selector.insertWithTest(r)); //1 <= 1 => should return TRUE

	shape1->setIsSol(true); //flag first event's shape as ground
	EXPECT_FALSE(selector.insertWithTest(r)); //ray is reflected on the ground => should return FALSE

	selector.setGroundAccepted(true); //accept ground reflexion
	EXPECT_TRUE(selector.insertWithTest(r)); //selector set to accept ground reflexions => should return TRUE

	r->getEvents()->at(0)=SPE3; //replace first reflexion by a diffraction
	selector.setGroundAccepted(false); //refuse ground reflexion
	EXPECT_TRUE(selector.insertWithTest(r)); //the event interacting with the ground is a diffraction => should return TRUE
}



/***********************************************************************
							 Selector
************************************************************************/

//Test the compareToKey structure of Selector
TEST(test_Selector, compareToKey)
{
	CompareToKey comparator;

	std::vector<unsigned int> list1;
	std::vector<unsigned int> list2;

	list1.push_back(5);
	list1.push_back(12);
	list1.push_back(1);
	list1.push_back(8);
	list1.push_back(45);

	list2.push_back(5);
	list2.push_back(12);
	list2.push_back(1);
	list2.push_back(8);
	list2.push_back(45);

	EXPECT_FALSE(comparator(list1,list2));

	list1.pop_back();
	EXPECT_TRUE(comparator(list1,list2));

	list1.push_back(42);
	EXPECT_TRUE(comparator(list1,list2));

	list1.at(0)=6;
	EXPECT_TRUE(comparator(list2,list1));
}


/***********************************************************************
							 SelectorManager
************************************************************************/


//Test the method appendData of SelectorManager
TEST(test_SelectorManager, appendData)
{

	SelectorManager<Ray> manager;

	

	FaceSelector<Ray>* faceSelector=new FaceSelector<Ray>();
	faceSelector->setModeHistory(HISTORY_FACE);

	manager.addSelector(faceSelector);

	Ray* r1=new Ray();
	Ray* r2=new Ray();
	Ray* r3=new Ray();
	Ray* r4=new Ray();

	Material* mat=new Material();
	
	//init random number generator
	unsigned int seed=(unsigned int)time(NULL);
	srand(seed);
	cout<<"[          ] Random number generator initialized with seed "<<seed<<endl;

	Simulation simu;
	Scene* scene=simu.getScene();

	//Set a source
	Source src;
	src.setPosition(vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5)));
	r1->setSource(&src);
	r2->setSource(&src);
	r3->setSource(&src);
	r4->setSource(&src);

	//Set a receptor
	Recepteur rcpt(vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5)),3);
	r1->setRecepteur(&rcpt);
	r2->setRecepteur(&rcpt);
	r3->setRecepteur(&rcpt);
	r4->setRecepteur(&rcpt);

	simu.addSource(src);
	simu.addRecepteur(rcpt);
	
	//set names of rays
	r1->setName("R1");
	r2->setName("R2");
	r3->setName("R3");
	r4->setName("R4");

	//set id of rays
	r1->setConstructId(1);
	r2->setConstructId(2);
	r3->setConstructId(3);
	r4->setConstructId(4);

	//Randomly generate four triangles with a different shapeId and add them to the scene
	std::vector<Shape*> shapes;
	for(int i=0;i<4;i++){
		unsigned int p1,p2,p3;

		vec3 vertex=vec3((decimal)(rand()%100+1),(decimal)(rand()%100+1),(decimal)(rand()%100+1));
		vec3 u=vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5));
		vec3 v=vec3((decimal)(rand()%10+5),(decimal)(rand()%10+5),(decimal)(rand()%10+5));
		
		scene->addVertex(vertex, p1);
		scene->addVertex(vertex+u, p2);
		scene->addVertex(vertex+v, p3);
		Shape* shape=scene->addTriangle(p1,p2,p3,mat);
		shape->setFaceId(i);
		shapes.push_back(shape);
	}

	//Define some events and associate a shape and a position to each one of them
	Event* e1=new Event();
	e1->setShape(shapes[0]);
	e1->setPosition(vec3(48,-25,74));
	std::shared_ptr<Event> SPE1(e1);

	Event* e2=new Event();
	e2->setShape(shapes[1]);
	e2->setPosition(vec3(24,11,-62));
	std::shared_ptr<Event> SPE2(e2);

	Event* e3=new Event();
	e3->setShape(shapes[1]);
	e3->setPosition(vec3(37,-7,26)); 
	std::shared_ptr<Event> SPE3(e3);

	Event* e4=new Event();
	e4->setShape(shapes[2]);
	e4->setPosition(vec3(-12,88,-42));
	std::shared_ptr<Event> SPE4(e4);

	Event* e5=new Event();
	e5->setShape(shapes[2]);
	e5->setPosition(vec3(57,-18,33));
	std::shared_ptr<Event> SPE5(e5);

	Event* e6=new Event();
	e6->setShape(shapes[3]);
	e4->setPosition(vec3(78,-51,36));
	std::shared_ptr<Event> SPE6(e6);

	//Assign events to the rays
	r1->getEvents()->push_back(SPE1);
	r1->getEvents()->push_back(SPE2);
	r1->getEvents()->push_back(SPE6);
	r1->getEvents()->push_back(SPE5);

	r2->getEvents()->push_back(SPE1);
	r2->getEvents()->push_back(SPE3);
	r2->getEvents()->push_back(SPE6);
	r2->getEvents()->push_back(SPE4);

	r3->getEvents()->push_back(SPE3);
	r3->getEvents()->push_back(SPE1);
	r3->getEvents()->push_back(SPE6);
	r3->getEvents()->push_back(SPE4);
	
	r4->getEvents()->push_back(SPE2);
	r4->getEvents()->push_back(SPE1);
	r4->getEvents()->push_back(SPE6);
	r4->getEvents()->push_back(SPE5);


	manager.appendData(r1); 
	std::map<unsigned long long, Ray*> selectedData=manager.getSelectedData();

	EXPECT_EQ(1,selectedData.size()); // no ray selected yet => ray should be added to selected data which should have size 1
	EXPECT_EQ("R1",selectedData.begin()->second->getName()); // the only selected ray should be R1

	manager.appendData(r2); 
	selectedData=manager.getSelectedData();

	EXPECT_EQ(1,selectedData.size()); // same faceId sequence as r1. r2 should replace r1. The number of selected rays should remain 1
	EXPECT_EQ("R2",selectedData.begin()->second->getName()); // the only selected ray should be R2

	manager.appendData(r3); 
	selectedData=manager.getSelectedData();

	EXPECT_EQ(2,selectedData.size()); // different faceId sequence from r1 and r2 -> rays should be selected
	std::map<unsigned long long, Ray*>::iterator it = selectedData.begin();
	EXPECT_EQ("R2",it->second->getName()); // first selected ray should be R2
	it++;
	EXPECT_EQ("R3",it->second->getName()); // second selected ray shopuld be R3
	
	manager.appendData(r4); 
	selectedData=manager.getSelectedData();

	EXPECT_EQ(2,selectedData.size()); // same faceId sequence as r3 but with longer length => rays should be rejected
	it = selectedData.begin();
	EXPECT_EQ("R2",it->second->getName()); // first selected ray should be R2
	it++;
	EXPECT_EQ("R3",it->second->getName()); // second selected ray shopuld be R3

}


/***********************************************************************
							 UnitConverter
************************************************************************/


//Test the fromRadianToCarthesien method
TEST(test_UnitConverter, fromRadianToCarthesien){

	vec3 res;
	Tools::fromRadianToCarthesien(0,decimal(M_PI/4),res);
	ASSERT_TRUE(res.compare(vec3(decimal(0.707107),decimal(0.707107),0)));

	Tools::fromRadianToCarthesien(decimal(M_PI/2),decimal(M_PI/4),res);
	ASSERT_TRUE(res.compare(vec3(0,0,1)));

	Tools::fromRadianToCarthesien(decimal(2*M_PI/3),decimal(5*M_PI/6),res);
	ASSERT_TRUE(res.compare(vec3(decimal(0.433013),decimal(-0.25),decimal(0.866025))));
}

//Test the fromRadianToCarthesien2 method 
TEST(test_UnitConverter, fromRadianToCarthesien2){

	vec3 res;
	Tools::fromRadianToCarthesien2(0,decimal(M_PI/4),res);
	ASSERT_TRUE(res.compare(vec3(0,0,1)));

	Tools::fromRadianToCarthesien2(decimal(M_PI/2),decimal(M_PI/4),res);
	ASSERT_TRUE(res.compare(vec3(decimal(0.707107),decimal(0.707107),0)));

	Tools::fromRadianToCarthesien2(decimal(3*M_PI/4),decimal(2*M_PI/3),res);
    ASSERT_TRUE(res.compare(vec3(decimal(-0.353553),decimal(0.612372),decimal(-0.707107))));
}
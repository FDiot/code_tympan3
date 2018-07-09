/**
 * \file test_acousticraytracer_accelerator.cpp
 * \test Testing the different accelerating structures used by TYMPAN to find the intersections between the rays and the primitives of a scene.
 *
 *  Created on: november, 2017
 *      Author: Fabien DIOT <fabien-externe.diot@edf.fr>
 */

#include "gtest/gtest.h"
#include <cstdlib>
#include <map>

#include "Tympan/geometric_methods/AnalyticRayTracer/RayCourb.h"
#include "Tympan/geometric_methods/AnalyticRayTracer/meteoLin.h"
#include "Tympan/geometric_methods/AnalyticRayTracer/meteo.h"
#include "Tympan/geometric_methods/AnalyticRayTracer/Lancer.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Latitude2DSampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Longitude2DSampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/UniformSphericSampler.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/UniformSphericSampler2.h"

using std::cout;
using std::cerr;
using std::endl;



/***********************************************************************
						  		Step
************************************************************************/
// test the Step default constructor
TEST(test_analyticraytracer, step_default_constructor){

	Step step; 

	// (0,0,0) pos and (0,0,0) norm
	EXPECT_TRUE((vec3(0.,0.,0.)==step.pos)!=0);
	EXPECT_TRUE((vec3(0.,0.,0.)==step.norm)!=0);


	step=Step(vec3(1.,2.,3.)); 

	// (1,2,3) pos and (0,0,0) norm
	EXPECT_TRUE((vec3(1.,2.,3.)==step.pos)!=0);
	EXPECT_TRUE((vec3(0.,0.,0.)==step.norm)!=0);


	step=Step(vec3(1.,2.,3.),vec3(-1.,-2.,-3.)); 

	// (1,2,3) pos and (1,2,3) norm
	EXPECT_TRUE((vec3(1.,2.,3.)==step.pos)!=0);
	EXPECT_TRUE((vec3(-1.,-2.,-3.)==step.norm)!=0);

}


// test the Step copy constructor
TEST(test_analyticraytracer, step_copy_constructor){

	Step step1(vec3(1.,2.,3.),vec3(-1.,-2.,-3.));
	Step step2(step1);

	//step2's properties should be equal to step1's
	EXPECT_TRUE((step1.pos==step2.pos)!=0);
	EXPECT_TRUE((step1.norm==step2.norm)!=0);

}

// test the Ray_adapter method
TEST(test_analyticraytracer, step_ray_adapter){

	Ray ray(vec3(1.,2.,3.), vec3(-1.,-2.,-3.));
	Step step=Step::Ray_adapter(ray);

	//step's pos and norm should be equal to ray's pos and direction
	EXPECT_TRUE((ray.getPosition()==step.pos)!=0);
	EXPECT_TRUE((ray.getDirection()==step.norm)!=0);

}

// test the product of a Step with a scalar
TEST(test_analyticraytracer, step_scalar_product){

	Step step(vec3(1.,2.,3.),vec3(-1.,-2.,-3.));

	//Multiply the step by 10
	Step product=step*10;

	//The new norm and pos should be 10 times the original
	EXPECT_TRUE((vec3(10.,20.,30.)==product.pos)!=0);
	EXPECT_TRUE((vec3(-10.,-20.,-30.)==product.norm)!=0);

}

// test the addition of Steps
TEST(test_analyticraytracer, step_addition){

	Step step1(vec3(1.,2.,3.),vec3(-1.,-2.,-3.));
	Step step2(vec3(10.,20.,30.),vec3(-10.,-20.,-30.));

	//Add the two steps together
	Step addition=step1+step2;

	//The norm and pos should be the sum of the two steps'
	EXPECT_TRUE((vec3(11.,22.,33.)==addition.pos)!=0);
	EXPECT_TRUE((vec3(-11.,-22.,-33.)==addition.norm)!=0);

}

/***********************************************************************
						  		RayCourb
************************************************************************/

// test the RayCourb default constructor
TEST(test_analyticraytracer, raycourb_default_constructor)
{

	RayCourb rayCourb;
	EXPECT_EQ(0,rayCourb.nbReflex);
}


// test the RayCourb vec3 constructor
TEST(test_analyticraytracer, raycourb_vec3_constructor)
{
	vec3 etape(1.0,2.0,3.0);
	RayCourb rayCourb(&etape);

	EXPECT_EQ(0,rayCourb.nbReflex);
	EXPECT_EQ(1,rayCourb.etapes.size());

	EXPECT_TRUE((etape==rayCourb.etapes[0].pos)!=0);
	EXPECT_TRUE((vec3(0.,0.,0.)==rayCourb.etapes[0].norm)!=0);

}


// test the RayCourb copy constructor
TEST(test_analyticraytracer, raycourb_copy_constructor)
{

	RayCourb rayCourb1;

	rayCourb1.nbReflex=4;

	rayCourb1.etapes.push_back(Step(vec3(1.,2.,3.),vec3(4.,5.,6.)));
	rayCourb1.etapes.push_back(Step(vec3(-1.,-2.,-3.),vec3(-4.,-5.,-6.)));

	rayCourb1.position.push_back(3);
	rayCourb1.position.push_back(2);
	rayCourb1.position.push_back(1);

	map<int,int> rencontre;
 	rencontre[1] = 2;
  	rencontre[3] = 4;
  	rencontre[5] = 6;

  	rayCourb1.rencontre=rencontre;

	
	//Create a copy of rayCourb1
	RayCourb rayCourb2(rayCourb1);


	//Test that all properties are equal
	EXPECT_EQ(rayCourb2.nbReflex,rayCourb1.nbReflex);

	EXPECT_EQ(rayCourb2.etapes.size(),rayCourb1.etapes.size());
	EXPECT_TRUE((rayCourb2.etapes[0].pos==rayCourb1.etapes[0].pos)!=0);
	EXPECT_TRUE((rayCourb2.etapes[0].norm==rayCourb1.etapes[0].norm)!=0);
	EXPECT_TRUE((rayCourb2.etapes[1].pos==rayCourb1.etapes[1].pos)!=0);
	EXPECT_TRUE((rayCourb2.etapes[1].norm==rayCourb1.etapes[1].norm)!=0);

	EXPECT_EQ(rayCourb2.position.size(),rayCourb1.position.size());
	EXPECT_EQ(rayCourb2.position[0],rayCourb1.position[0]);
	EXPECT_EQ(rayCourb2.position[1],rayCourb1.position[1]);
	EXPECT_EQ(rayCourb2.position[2],rayCourb1.position[2]);

	EXPECT_TRUE((rayCourb2.rencontre==rayCourb1.rencontre));
}

// test the RayCourb assign operator
TEST(test_analyticraytracer, raycourb_assign_operator)
{

	RayCourb rayCourb1;

	rayCourb1.nbReflex=4;

	rayCourb1.etapes.push_back(Step(vec3(1.,2.,3.),vec3(4.,5.,6.)));
	rayCourb1.etapes.push_back(Step(vec3(-1.,-2.,-3.),vec3(-4.,-5.,-6.)));

	rayCourb1.position.push_back(3);
	rayCourb1.position.push_back(2);
	rayCourb1.position.push_back(1);

	map<int,int> rencontre;
 	rencontre[1] = 2;
  	rencontre[3] = 4;
  	rencontre[5] = 6;

  	rayCourb1.rencontre=rencontre;

	
	//Assign rayCourb1 to rayCourb2
	RayCourb rayCourb2=rayCourb1;


	//Test that all properties are equal
	EXPECT_EQ(rayCourb2.nbReflex,rayCourb1.nbReflex);

	EXPECT_EQ(rayCourb2.etapes.size(),rayCourb1.etapes.size());
	EXPECT_TRUE((rayCourb2.etapes[0].pos==rayCourb1.etapes[0].pos)!=0);
	EXPECT_TRUE((rayCourb2.etapes[0].norm==rayCourb1.etapes[0].norm)!=0);
	EXPECT_TRUE((rayCourb2.etapes[1].pos==rayCourb1.etapes[1].pos)!=0);
	EXPECT_TRUE((rayCourb2.etapes[1].norm==rayCourb1.etapes[1].norm)!=0);

	EXPECT_EQ(rayCourb2.position.size(),rayCourb1.position.size());
	EXPECT_EQ(rayCourb2.position[0],rayCourb1.position[0]);
	EXPECT_EQ(rayCourb2.position[1],rayCourb1.position[1]);
	EXPECT_EQ(rayCourb2.position[2],rayCourb1.position[2]);

	EXPECT_TRUE((rayCourb2.rencontre==rayCourb1.rencontre));
}

// test the purge method
TEST(test_analyticraytracer, raycourb_purge)
{

	RayCourb rayCourb;

	rayCourb.nbReflex=4;

	rayCourb.etapes.push_back(Step(vec3(1.,2.,3.),vec3(4.,5.,6.)));
	rayCourb.etapes.push_back(Step(vec3(-1.,-2.,-3.),vec3(-4.,-5.,-6.)));

	rayCourb.position.push_back(3);
	rayCourb.position.push_back(2);
	rayCourb.position.push_back(1);

	map<int,int> rencontre;
 	rencontre[1] = 2;
  	rencontre[3] = 4;
  	rencontre[5] = 6;

  	rayCourb.rencontre=rencontre;

  	//Check that the sizes are correct
  	EXPECT_EQ(2,rayCourb.etapes.size());
  	EXPECT_EQ(3,rayCourb.position.size());
  	EXPECT_EQ(3,rayCourb.rencontre.size());

  	//Clear arrays
  	rayCourb.purge();

  	//Check that the sizes are 0
  	EXPECT_EQ(0,rayCourb.etapes.size());
  	EXPECT_EQ(0,rayCourb.position.size());
  	EXPECT_EQ(0,rayCourb.rencontre.size());
  	

}

// test the setSize method
TEST(test_analyticraytracer, raycourb_set_size)
{

	RayCourb rayCourb;
	rayCourb.setSize(15);
	EXPECT_EQ(0,rayCourb.etapes.size());
	EXPECT_EQ(15,rayCourb.etapes.capacity());
}


/***********************************************************************
						  		Meteo
************************************************************************/

// test the meteo default constructor
TEST(test_analyticraytracer, meteo_default_constructor)
{

		meteo met;
		EXPECT_DOUBLE_EQ(340.,met.getC0());
		EXPECT_DOUBLE_EQ(0.,met.getWindAngle());


}

// test the meteo constructor
TEST(test_analyticraytracer, meteo_constructor)
{

		meteo met(45.,123.);
		EXPECT_DOUBLE_EQ(123.,met.getC0());
		EXPECT_NEAR(45.,met.getWindAngle(),EPSILON_4);
}


// test the wind angle setter and getter 
TEST(test_analyticraytracer, meteo_wind_angle)
{

		meteo met;
		EXPECT_DOUBLE_EQ(0.,met.getWindAngle());

		met.setWindAngle(274.8);
		EXPECT_NEAR(274.8,met.getWindAngle(),EPSILON_4);

}



/***********************************************************************
						  		MeteoLin
************************************************************************/


// test the default constructor
TEST(test_analyticraytracer, meteolin_default_constructor)
{
	meteoLin metLin;
	EXPECT_DOUBLE_EQ(340.,metLin.getC0());
	EXPECT_DOUBLE_EQ(0.,metLin.getWindAngle());
}


// test the initialization of a meteoLin object
TEST(test_analyticraytracer, meteolin_init)
{

	meteoLin metLin(75.,250.,10.,20.);

	EXPECT_DOUBLE_EQ(250.,metLin.getC0());
	EXPECT_NEAR(75.,metLin.getWindAngle(),EPSILON_4);

	array< array<double, 3>, 3 > jacobian= metLin.getJacobMatrix();

	double trigonometric_angle= -M_PIDIV2-RADIANS(75.); // convert the windrose oriented angle to its corresponding trignometric angle

	//The last column of the two first rows contain the x and y components of the wind velocity
	EXPECT_DOUBLE_EQ(cos(trigonometric_angle)*20,jacobian[0][2]);
	EXPECT_DOUBLE_EQ(sin(trigonometric_angle)*20.,jacobian[1][2]);

	//The remaining cells are zero
	EXPECT_DOUBLE_EQ(0.,jacobian[0][0]);
	EXPECT_DOUBLE_EQ(0.,jacobian[0][1]);

	EXPECT_DOUBLE_EQ(0.,jacobian[1][0]);
	EXPECT_DOUBLE_EQ(0.,jacobian[1][1]);

	EXPECT_DOUBLE_EQ(0.,jacobian[2][0]);
	EXPECT_DOUBLE_EQ(0.,jacobian[2][1]);
	EXPECT_DOUBLE_EQ(0.,jacobian[2][2]);
}



// test the cTemp method
TEST(test_analyticraytracer, meteolin_ctemp)
{

	meteoLin metLin(75.,340.,10.,20.);
	vec3 grad;
	vec3 pos(10.,20.,30.);

	double c=metLin.cTemp(pos,grad);
	
	EXPECT_DOUBLE_EQ(640.,c); //gradC*pos.z+C0
	EXPECT_TRUE((vec3(0.,0.,10.)==grad)!=0); //grad.z=gradC

}


// test the cWind method
TEST(test_analyticraytracer, meteolin_cwind)
{

	meteoLin metLin(75.,250.,10.,20.);
	vec3 pos(10.,20.,30.);

	double trigonometric_angle= -M_PIDIV2-RADIANS(75.); 		// convert the windrose oriented angle to its corresponding trignometric angle
	decimal DVx = decimal(cos(trigonometric_angle) * 20.); 		// compute the x component of the wind 
    decimal DVy = decimal(sin(trigonometric_angle) * 20.);		// compute the y component of the wind 

	vec3 v=metLin.cWind(pos);
	EXPECT_TRUE((vec3(DVx*pos.z,DVy*pos.z,0.)==v)!=0); 
}


/***********************************************************************
						  		Lancer
************************************************************************/


// test the Lancer default constructor
TEST(test_analyticraytracer, lancer_default_constructor)
{		

	Lancer lancer;

	EXPECT_TRUE(lancer._weather!=NULL);
	EXPECT_TRUE(lancer._sampler==NULL);

	EXPECT_FLOAT_EQ(0.001f,lancer.h);
	EXPECT_FLOAT_EQ(3.f,lancer.TMax);
	EXPECT_FLOAT_EQ(1000.f,lancer.dmax);

	EXPECT_EQ(20,lancer.nbRay);

	EXPECT_EQ(0,lancer.temps.size());
	EXPECT_EQ(0,lancer.sources.size());
	EXPECT_EQ(0,lancer.recepteurs.size());
	EXPECT_EQ(0,lancer.MatRes.size());
	EXPECT_EQ(0,lancer._plan.size());

	EXPECT_DOUBLE_EQ(0.,lancer.initialAngleTheta);
	EXPECT_DOUBLE_EQ(360.,lancer.finalAngleTheta);
	EXPECT_DOUBLE_EQ(0.,lancer.initialAnglePhi);
	EXPECT_DOUBLE_EQ(360.,lancer.finalAnglePhi);

	EXPECT_EQ(1,lancer._launchType);
	EXPECT_TRUE(lancer.wantOutFile);    

}

// test the Lancer copy constructor
TEST(test_analyticraytracer, lancer_copy_constructor)
{		

	// Create a Lancer
	Lancer lancer1;

	lancer1.h=1.f;
	lancer1.TMax=2.f;
	lancer1.dmax=3.f;
	lancer1.nbRay=10;

	lancer1.temps.push_back(decimal(1));
	lancer1.sources.push_back(vec3(1.,2.,3.));
	lancer1.recepteurs.push_back(vec3(-1.,-2.,-3.));
	lancer1.MatRes.push_back(NULL);
	vec3 plan(4.,5.,6.);
	lancer1._plan.push_back(&plan);
	
	lancer1.initialAngleTheta=20.;
	lancer1.finalAngleTheta=250.;	
	lancer1.initialAnglePhi=10.;
	lancer1.finalAnglePhi=150.;

	lancer1._launchType=2;
	lancer1.wantOutFile=false;


    // Copy the first lancer
    Lancer lancer2(lancer1);


    // Test that the copied attributes of both lancer are equal
	EXPECT_EQ(lancer2._weather,lancer1._weather);

	EXPECT_EQ(lancer2.h,lancer1.h);
	EXPECT_EQ(lancer2.TMax,lancer1.TMax);
	EXPECT_EQ(lancer2.dmax,lancer1.dmax);

	EXPECT_EQ(lancer2.nbRay,lancer1.nbRay);

	EXPECT_EQ(lancer2.temps,lancer1.temps);
	EXPECT_EQ(lancer2.MatRes,lancer1.MatRes);
	EXPECT_EQ(lancer2._plan,lancer1._plan);

	//for some reason the == operator of vec3 does not 
	//behave as expected when comparing two vector<vec3>
	EXPECT_EQ(lancer1.sources.size(),lancer2.sources.size());
	for(unsigned int i=0;i<lancer1.sources.size();i++){
		EXPECT_TRUE((lancer1.sources[i]==lancer2.sources[i])!=0);
	}

	EXPECT_EQ(lancer1.recepteurs.size(),lancer2.recepteurs.size());
	for(unsigned int i=0;i<lancer1.recepteurs.size();i++){
		EXPECT_TRUE((lancer1.recepteurs[i]==lancer2.recepteurs[i])!=0);
	}

	EXPECT_EQ(lancer2.initialAngleTheta,lancer1.initialAngleTheta);
	EXPECT_EQ(lancer2.finalAngleTheta,lancer1.finalAngleTheta);
	EXPECT_EQ(lancer2.initialAnglePhi,lancer1.initialAnglePhi);
	EXPECT_EQ(lancer2.finalAnglePhi,lancer1.finalAnglePhi);

	EXPECT_EQ(lancer2._launchType,lancer1._launchType);
	EXPECT_EQ(lancer2.wantOutFile,lancer1.wantOutFile);

}


// test the init_sampler method
TEST(test_analyticraytracer, lancer_init_sampler)
{	

	Lancer lancer;

	lancer.initialAngleTheta=10.;
	lancer.finalAngleTheta=20.;	
	lancer.initialAnglePhi=30.;
	lancer.finalAnglePhi=40.;


	lancer.setLaunchType(1);

	//_launchType 1 means we use a Latitude2DSampler for horizontal launching
	Latitude2DSampler* sampler1 = dynamic_cast<Latitude2DSampler*> (lancer._sampler); 
	EXPECT_TRUE(sampler1!=NULL);
	EXPECT_NEAR(10.,sampler1->getStartTheta(),EPSILON_4);
	EXPECT_NEAR(30.,sampler1->getStartPhi(),EPSILON_4);
	EXPECT_NEAR(40.,sampler1->getEndPhi(),EPSILON_4);


	lancer.setLaunchType(2);

	//_launchType 2 means we use a Longitude2DSampler for vertical launching
	Longitude2DSampler* sampler2 = dynamic_cast<Longitude2DSampler*> (lancer._sampler); 
	EXPECT_TRUE(sampler2!=NULL);
	EXPECT_NEAR(10.,sampler2->getStartTheta(),EPSILON_4);
	EXPECT_NEAR(20.,sampler2->getEndTheta(),EPSILON_4);
	EXPECT_NEAR(30.,sampler2->getStartPhi(),EPSILON_4);


	lancer.setLaunchType(3);

	//_launchType 3 means we use a UniformSphericSampler
	UniformSphericSampler* sampler3 = dynamic_cast<UniformSphericSampler*> (lancer._sampler); 
	EXPECT_TRUE(sampler3!=NULL);
	EXPECT_EQ(lancer.nbRay,sampler3->getRealNbRays());


	lancer.setLaunchType(4);

	//_launchType 4 means we use a UniformSphericSampler2
	UniformSphericSampler2* sampler4 = dynamic_cast<UniformSphericSampler2*> (lancer._sampler); 
	EXPECT_TRUE(sampler4!=NULL);
	EXPECT_EQ(lancer.nbRay,sampler4->getRealNbRays());
	

}


// test the purgeMatRes method
TEST(test_analyticraytracer, lancer_purge_mat_res)
{
	Lancer lancer;

	//Define three RayCourb arrays
	RayCourb* rays1=new RayCourb[2];
	RayCourb* rays2=new RayCourb[1];
	RayCourb* rays3=new RayCourb[3];

	//Add the arays to the MatRes of lancer
	lancer.MatRes.push_back(rays1);
	lancer.MatRes.push_back(rays2);
	lancer.MatRes.push_back(rays3);

	//Test the size of MatRes
	EXPECT_EQ(3,lancer.MatRes.size());

	//purge lancer's MatRes
	lancer.purgeMatRes();

	//Check the final size
	EXPECT_EQ(0,lancer.MatRes.size());
}

// test the EqRay method
TEST(test_analyticraytracer, lancer_eq_ray)
{
	meteoLin* weather= new meteoLin(75.,340.,10.,20.);
	vec3 grad;
	vec3 pos(10.,20.,30.);

	double c=weather->cTemp(pos,grad);
	
	EXPECT_DOUBLE_EQ(640.,c); //gradC*pos.z+C0
	EXPECT_TRUE((vec3(0.,0.,10.)==grad)!=0); //grad.z=gradC

	Lancer lancer;
	lancer.setMeteo(weather);
	
}
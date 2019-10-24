/**
 * \file test_OBox.cpp
 * \test Testing of OBox libs used by lot of TYMPAN features
 *
 *  Created on: november, 2017
 *      Author: Albert ALARCON <albert.alarcon@edf.fr>
 */

#include "gtest/gtest.h"
//
#include "Tympan/geometric_methods/AcousticRaytracer/Engine/Simulation.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Source.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/UniformSphericSampler.h"

TEST(test_engine, test_genray)
{
    
    //Create source
    vec3 source_pos=vec3(1,0,0);
    Source src;
    src.setName("Source test");
    src.setPosition(source_pos);
	src.setNbRayLeft(1);
    src.setSampler(new UniformSphericSampler(1));

    // Create default engine
    DefaultEngine def_engine;

    std::vector<Source> vec_source;
    vec_source.push_back(src);
	
    def_engine.setSources(&vec_source);
	
    unsigned long long ray_counter_ini=def_engine.getRayCounter();

	// first call of genRay --> returns a new ray pointer
    Ray* new_ray=def_engine.genRay();

	EXPECT_FLOAT_EQ(new_ray->getMaxt(),10000.);

    EXPECT_FLOAT_EQ(new_ray->getMint(),0.);

    EXPECT_TRUE(new_ray->getPosition()==source_pos);

    // test source features from new_ray 
    Source* src_from_ray=new_ray->getSource();

	EXPECT_TRUE(src_from_ray->getName()==src.getName());

	EXPECT_TRUE(src_from_ray->getPosition()==src.getPosition());

	EXPECT_EQ(src_from_ray->getId(),src.getId());

	EXPECT_EQ(new_ray->getConstructId(),0);

    // test raycounter update
    EXPECT_EQ(def_engine.getRayCounter(),ray_counter_ini+1);

    // new and last call of genRay --> must return NULL pointer
    Ray* new_ray_null=def_engine.genRay();
    
    EXPECT_TRUE(new_ray_null==NULL);

}
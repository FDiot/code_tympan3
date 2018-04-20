/**
 * \file test_acousticraytracer_accelerator.cpp
 * \test Testing the different accelerating structures used by TYMPAN to find the intersections between the rays and the primitives of a scene.
 *
 *  Created on: november, 2017
 *      Author: Fabien DIOT <fabien-externe.diot@edf.fr>
 */

#include "gtest/gtest.h"

#include "Tympan/geometric_methods/AcousticRaytracer/Accelerator/BruteForceAccelerator.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Accelerator/GridAccelerator.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Accelerator/BvhAccelerator.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Accelerator/KdtreeAccelerator.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Triangle.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Cylindre.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Acoustic/Recepteur.h"


// comparison between intersections (used to sort them by their t attribute)
bool compareIntersections(const Intersection& first, Intersection& second)
{
	return ( first.t < second.t );
}


//Initialise a vector of shapes and rays used for the tests
void initShapesAndRays(std::vector<Shape*> &shapes,std::vector<Ray*> &rays){

	// ******** Shapes' Creation ******** //
	//create dummy material
	Material material;

	//create triangles
	Triangle* t1=new Triangle(vec3(4,2,1),vec3(3,1,-1),vec3(5,1,4),&material);
	Triangle* t2=new Triangle(vec3(6,4,0),vec3(5,0,-1),vec3(7,0,5),&material);
	Triangle* t3=new Triangle(vec3(-1,0,6),vec3(-1,0,-1),vec3(-1,3,2),&material);
	Triangle* t4=new Triangle(vec3(1,6,-2),vec3(-1,6,1),vec3(5,6,1),&material);

	//give names to the triangles
	t1->setName("t1");
	t2->setName("t2");
	t3->setName("t3");
	t4->setName("t4");

	//add the triangles to the shapes' list
	shapes.push_back(t1);
	shapes.push_back(t2);
	shapes.push_back(t3);
	shapes.push_back(t4);

	//create receptors
	Recepteur* r1=new Recepteur(vec3(8,(decimal)4/3,(decimal)4/3),(decimal).1);				//Aligned with the barycenters of t1 and t2
	Recepteur* r2=new Recepteur(vec3((decimal)1/2,(decimal)1/2,(decimal)8/3),(decimal).1);	//In front of t3
	Recepteur* r3=new Recepteur(vec3(-4,2,(decimal)5/3),(decimal).1);						//Behind t3
	Recepteur* r4=new Recepteur(vec3((decimal)5/3,4,0),(decimal).1);						//Below t4
	Recepteur* r5=new Recepteur(vec3((decimal)-11/3,-4,(decimal)14/3),(decimal).1);			//Aligned with barycenters of t4 and t3

	//give names to the receptors
	r1->setName("r1");
	r2->setName("r2");
	r3->setName("r3");
	r4->setName("r4");
	r5->setName("r5");

	//add the receptors to the shapes' list
	shapes.push_back(r1);
	shapes.push_back(r2);
	shapes.push_back(r3);
	shapes.push_back(r4);
	shapes.push_back(r5);


	// ******** Rays' Creation ******** //
	vec3 dir1=vec3(1,0,0);
	dir1.normalize();
	Ray* ray1=new Ray(vec3(1,(decimal)4/3,(decimal)4/3),dir1);	//Points to r1 and passes by t1 and t2

	vec3 dir2=vec3(-3,1,(decimal)-2/3);
	dir2.normalize();
	Ray* ray2=new Ray(vec3(2,0,3),dir2);						//Points to r3 and passes by r2 and t3

	vec3 dir3=vec3(0,-1,0);
	dir3.normalize();
	Ray* ray3=new Ray(vec3((decimal)5/3,8,0),dir3);				//Points to r4 and passes by t4

	vec3 dir4=vec3((decimal)-8/3,-5,(decimal)7/3);
	dir4.normalize();
	Ray* ray4=new Ray(vec3((decimal)13/3,11,(decimal)-7/3),dir4);	//Points to r5 and passes by t3 and t4


	//add the rays to the rays' list
	rays.push_back(ray1);
	rays.push_back(ray2);
	rays.push_back(ray3);
	rays.push_back(ray4);
}


/***********************************************************************
						        Accelerators
************************************************************************/


TEST(test_accelerator, brute_force)
{
    
	std::vector<Shape*> shapes;
    std::vector<Ray*> rays;

	initShapesAndRays(shapes,rays);

	//Create a global bounding box as the union of the bounding boxes of all shapes
	BBox globalBbox; 
	for(unsigned int i=0; i < shapes.size();i++) {
		globalBbox=globalBbox.Union(shapes.at(i)->getBBox());
	}

	ASSERT_TRUE(vec3((decimal)-4.0999999,(decimal)-4.0999999,(decimal)-2.01)==globalBbox.getBBMin());
	ASSERT_TRUE(vec3((decimal)8.1000004,(decimal)6.01000002,(decimal)6.01000002)==globalBbox.getBBMax());
	ASSERT_TRUE(vec3((decimal)2.0000002,(decimal)0.95500016,(decimal)2.)==globalBbox.getCentroid());
  
	BruteForceAccelerator accelerator(&shapes,globalBbox);
	ASSERT_TRUE(accelerator.build());
	accelerator.setIntersectionChoice(leafTreatment::ALL); //keep all intersections found

	//ray1's intersections
	std::list<Intersection> result;
	accelerator.traverse(rays.at(0),result); //traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		 //sort intersections by their distance (t attribute)

	std::list<Intersection>::iterator inter=result.begin();
	ASSERT_EQ(3,result.size());				  //should find 3 intersections
	EXPECT_EQ("t1", inter->p->getName());     //intersect the first triangle
	EXPECT_EQ("t2", (++inter)->p->getName()); //then intersect the second triangle
	EXPECT_EQ("r1", (++inter)->p->getName()); //and finally the first receptor


	//ray2's intersections
	result.clear();							 //clear previous results
	accelerator.traverse(rays.at(1),result); //traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		 //sort intersections by their distance (t attribute)

	inter=result.begin();
	ASSERT_EQ(3,result.size());				 //should find 3 intersections
	EXPECT_EQ("r2", inter->p->getName());	 //intersect the second receptor
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the third triangle
	EXPECT_EQ("r3", (++inter)->p->getName());//and finally the third receptor


	//ray3's intersections
	result.clear();							 //clear previous results
	accelerator.traverse(rays.at(2),result); //traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		 //sort intersections by their distance (t attribute)

	inter=result.begin();
	ASSERT_EQ(2,result.size());				 //should find 2 intersections
	EXPECT_EQ("t4", inter->p->getName());	 //intersect the fourth triangle
	EXPECT_EQ("r4", (++inter)->p->getName());//and finally intersect the fourth receptor

	//ray4's intersections
	result.clear();							 //clear previous results
	accelerator.traverse(rays.at(3),result); //traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		 //sort intersections by their distance (t attribute)

	inter=result.begin();					
	ASSERT_EQ(3,result.size());				 //should find 3 intersections
	EXPECT_EQ("t4", inter->p->getName());	 //intersect the third triangle
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the fourth triangle
	EXPECT_EQ("r5", (++inter)->p->getName());//and finally intersect the fifth receptor
}

TEST(test_accelerator, grid)
{
    
	std::vector<Shape*> shapes;
    std::vector<Ray*> rays;

	initShapesAndRays(shapes,rays);

	//Create a global bounding box as the union of the bounding boxes of all shapes
	BBox globalBbox; 
	for(unsigned int i=0; i < shapes.size();i++) {
		globalBbox=globalBbox.Union(shapes.at(i)->getBBox());
	}

	ASSERT_TRUE(vec3((decimal)-4.0999999,(decimal)-4.0999999,(decimal)-2.01)==globalBbox.getBBMin());
	ASSERT_TRUE(vec3((decimal)8.1000004,(decimal)6.01000002,(decimal)6.01000002)==globalBbox.getBBMax());
	ASSERT_TRUE(vec3((decimal)2.0000002,(decimal)0.95500016,(decimal)2.)==globalBbox.getCentroid());

  
	GridAccelerator accelerator(&shapes,globalBbox);
	ASSERT_TRUE(accelerator.build());
	accelerator.setIntersectionChoice(leafTreatment::ALL); //keep all intersections found

	//ray1's intersections
	std::list<Intersection> result;
	accelerator.traverse(rays.at(0),result); //traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		 //sort intersections by their distance (t attribute)

	std::list<Intersection>::iterator inter=result.begin();
	ASSERT_EQ(5,result.size());				  //should find 5 intersections (because one primitive can be contained in multiple cells of the grid)
	EXPECT_EQ("t1", inter->p->getName());     //intersect the first triangle
	EXPECT_EQ("t1", (++inter)->p->getName()); //intersect the first triangle a second time
	EXPECT_EQ("t2", (++inter)->p->getName()); //then intersect the second triangle
	EXPECT_EQ("t2", (++inter)->p->getName()); //then intersect the second triangle a second time
	EXPECT_EQ("r1", (++inter)->p->getName()); //and finally the first receptor


	//ray2's intersections
	result.clear();							//clear previous results
	accelerator.traverse(rays.at(1),result);//traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		//sort intersections by their distance (t attribute)

	inter=result.begin();
	ASSERT_EQ(4,result.size());				 //should find 4 intersections (because one primitive can be contained in multiple cells of the grid)
	EXPECT_EQ("r2", inter->p->getName());	 //intersect the second receptor
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the third triangle
	EXPECT_EQ("r3", (++inter)->p->getName());//and finally the third receptor
	EXPECT_EQ("r3", (++inter)->p->getName());//and finally the third receptor a second time


	//ray3's intersections
	result.clear();							//clear previous results
	accelerator.traverse(rays.at(2),result);//traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		//sort intersections by their distance (t attribute)

	inter=result.begin();
	ASSERT_EQ(3,result.size());				 //should find 3 intersections (because one primitive can be contained in multiple cells of the grid)
	EXPECT_EQ("t4", inter->p->getName());	 //intersect the fourth triangle
	EXPECT_EQ("r4", (++inter)->p->getName());//and finally intersect the fourth receptor
	EXPECT_EQ("r4", (++inter)->p->getName());//and finally intersect the fourth receptor a second time

	//ray4's intersections
	result.clear();							//clear previous results
	accelerator.traverse(rays.at(3),result);//traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		//sort intersections by their distance (t attribute)

	inter=result.begin();					
	ASSERT_EQ(5,result.size());				 //should find 5 intersections (because one primitive can be contained in multiple cells of the grid)
	EXPECT_EQ("t4", inter->p->getName());	 //intersect the third triangle
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the fourth triangle
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the fourth triangle a second time
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the fourth triangle a third time
	EXPECT_EQ("r5", (++inter)->p->getName());//and finally intersect the fifth receptor
}


TEST(test_accelerator, bvh)
{
    
	std::vector<Shape*> shapes;
    std::vector<Ray*> rays;

	initShapesAndRays(shapes,rays);

	//Create a global bounding box as the union of the bounding boxes of all shapes
	BBox globalBbox; 
	for(unsigned int i=0; i < shapes.size();i++) {
		globalBbox=globalBbox.Union(shapes.at(i)->getBBox());
	}

	ASSERT_TRUE(vec3((decimal)-4.0999999,(decimal)-4.0999999,(decimal)-2.01)==globalBbox.getBBMin());
	ASSERT_TRUE(vec3((decimal)8.1000004,(decimal)6.01000002,(decimal)6.01000002)==globalBbox.getBBMax());
	ASSERT_TRUE(vec3((decimal)2.0000002,(decimal)0.95500016,(decimal)2.)==globalBbox.getCentroid());
  
	BvhAccelerator accelerator(&shapes,globalBbox);
	ASSERT_TRUE(accelerator.build());
	accelerator.setIntersectionChoice(leafTreatment::ALL); //keep all intersections found

	//ray1's intersections
	std::list<Intersection> result;
	accelerator.traverse(rays.at(0),result); //traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		 //sort intersections by their distance (t attribute)

	std::list<Intersection>::iterator inter=result.begin();
	ASSERT_EQ(3,result.size());				  //should find 3 intersections
	EXPECT_EQ("t1", inter->p->getName());     //intersect the first triangle
	EXPECT_EQ("t2", (++inter)->p->getName()); //then intersect the second triangle
	EXPECT_EQ("r1", (++inter)->p->getName()); //and finally the first receptor


	//ray2's intersections
	result.clear();							 //clear previous results
	accelerator.traverse(rays.at(1),result); //traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		 //sort intersections by their distance (t attribute)

	inter=result.begin();
	ASSERT_EQ(3,result.size());				 //should find 3 intersections
	EXPECT_EQ("r2", inter->p->getName());	 //intersect the second receptor
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the third triangle
	EXPECT_EQ("r3", (++inter)->p->getName());//and finally the third receptor


	//ray3's intersections
	result.clear();							//clear previous results
	accelerator.traverse(rays.at(2),result);//traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		//sort intersections by their distance (t attribute)

	inter=result.begin();
	ASSERT_EQ(2,result.size());				 //should find 2 intersections
	EXPECT_EQ("t4", inter->p->getName());	 //intersect the fourth triangle
	EXPECT_EQ("r4", (++inter)->p->getName());//and finally intersect the fourth receptor

	//ray4's intersections
	result.clear();							 //clear previous results
	accelerator.traverse(rays.at(3),result); //traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		 //sort intersections by their distance (t attribute)

	inter=result.begin();					
	ASSERT_EQ(3,result.size());				 //should find 3 intersections
	EXPECT_EQ("t4", inter->p->getName());	 //intersect the third triangle
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the fourth triangle
	EXPECT_EQ("r5", (++inter)->p->getName());//and finally intersect the fifth receptor
}

TEST(test_accelerator, kdtree)
{
    
	std::vector<Shape*> shapes;
    std::vector<Ray*> rays;

	initShapesAndRays(shapes,rays);

	//Create a global bounding box as the union of the bounding boxes of all shapes
	BBox globalBbox; 
	for(unsigned int i=0; i < shapes.size();i++) {
		globalBbox=globalBbox.Union(shapes.at(i)->getBBox());
	}
     
	ASSERT_TRUE(vec3((decimal)-4.0999999,(decimal)-4.0999999,(decimal)-2.01)==globalBbox.getBBMin());
	ASSERT_TRUE(vec3((decimal)8.1000004,(decimal)6.01000002,(decimal)6.01000002)==globalBbox.getBBMax());
	ASSERT_TRUE(vec3((decimal)2.0000002,(decimal)0.95500016,(decimal)2.)==globalBbox.getCentroid());

	KdtreeAccelerator accelerator(&shapes,globalBbox);
	ASSERT_TRUE(accelerator.build());
	accelerator.setIntersectionChoice(leafTreatment::ALL); //keep all intersections found

	//ray1's intersections
	std::list<Intersection> result;
	accelerator.traverse(rays.at(0),result); //traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		 //sort intersections by their distance (t attribute)

	std::list<Intersection>::iterator inter=result.begin();
	ASSERT_EQ(5,result.size());				  //should find 5 intersections (because a split can occure on a primitive)
	EXPECT_EQ("t1", inter->p->getName());     //intersect the first triangle
	EXPECT_EQ("t1", (++inter)->p->getName()); //intersect the first triangle a second time
	EXPECT_EQ("t2", (++inter)->p->getName()); //then intersect the second triangle
	EXPECT_EQ("t2", (++inter)->p->getName()); //then intersect the second triangle a second time
	EXPECT_EQ("r1", (++inter)->p->getName()); //and finally the first receptor


	//ray2's intersections
	result.clear();							 //clear previous results 
	accelerator.traverse(rays.at(1),result); //traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		 //sort intersections by their distance (t attribute)

	inter=result.begin();
	ASSERT_EQ(4,result.size());				 //should find 4 intersections (because a split can occure on a primitive)
	EXPECT_EQ("r2", inter->p->getName());	 //intersect the second receptor 
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the third triangle
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the third triangle a second time
	EXPECT_EQ("r3", (++inter)->p->getName());//and finally the third receptor


	//ray3's intersections
	result.clear();							//clear previous results
	accelerator.traverse(rays.at(2),result);//traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		//sort intersections by their distance (t attribute)

	inter=result.begin();
	ASSERT_EQ(2,result.size());				 //should find 2 intersections
	EXPECT_EQ("t4", inter->p->getName());	 //intersect the fourth triangle
	EXPECT_EQ("r4", (++inter)->p->getName());//and finally intersect the fourth receptor

	//ray4's intersections
	result.clear();							 //clear previous results
	accelerator.traverse(rays.at(3),result); //traverse the accelerating structure to find intersections
	result.sort(compareIntersections);		 //sort intersections by their distance (t attribute)

	inter=result.begin();					
	ASSERT_EQ(5,result.size());				 //should find 5 intersections (because a split can occure on a primitive)
	EXPECT_EQ("t4", inter->p->getName());	 //intersect the third triangle
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the fourth triangle
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the fourth triangle a second time
	EXPECT_EQ("t3", (++inter)->p->getName());//then intersect the fourth triangle a third time
	EXPECT_EQ("r5", (++inter)->p->getName());//and finally intersect the fifth receptor
}



/***********************************************************************
						        LeafTreatment
************************************************************************/

TEST(test_leaf_treatment, keep_first)
{

	std::list<Intersection> intersections;

	Intersection inter1;
	inter1.t=-1;
	inter1.p=new Shape();
	inter1.forme=TRIANGLE;
	inter1.normal=vec3(0,0,0);

	Intersection inter2;
	inter2.t=1;
	inter2.p=new Shape();
	inter2.forme=TRIANGLE;
	inter2.normal=vec3(0,0,0);

	Intersection inter3;
	inter3.t=10;
	inter3.p=new Shape();
	inter3.forme=TRIANGLE;
	inter3.normal=vec3(0,0,0);

	//No intersections
	ASSERT_EQ((decimal)1.,leafTreatment::keepFirst(intersections,1.));
	ASSERT_EQ(0,intersections.size());

	//Add intersections
	intersections.push_back(inter1);
	intersections.push_back(inter2);
	intersections.push_back(inter3);

	//no tmin restriction
	ASSERT_EQ((decimal)1.,leafTreatment::keepFirst(intersections,-1));
	ASSERT_EQ(1,intersections.size());

	
}

TEST(test_leaf_treatment, keep_all_before_triangle)
{

	std::list<Intersection> intersections;

	Intersection inter1;
	inter1.t=2;
	inter1.p=new Shape();
	inter1.forme=TRIANGLE;
	inter1.normal=vec3(0,0,0);

	Intersection inter2;
	inter2.t=4;
	inter2.p=new Triangle();
	inter2.forme=TRIANGLE;
	inter2.normal=vec3(0,0,0);

	Intersection inter3;
	inter3.t=6;
	inter3.p=new Shape();
	inter3.forme=TRIANGLE;
	inter3.normal=vec3(0,0,0);

	//No intersections
	EXPECT_FLOAT_EQ((decimal)1.,leafTreatment::keepAllBeforeTriangle(intersections,1.));
	EXPECT_EQ(0,intersections.size());

	//Add intersections
	intersections.push_back(inter1);
	intersections.push_back(inter2);
	intersections.push_back(inter3);

	//Triangle in second position with current tmin=-1
	EXPECT_FLOAT_EQ((decimal)4.,leafTreatment::keepAllBeforeTriangle(intersections,-1.));
	EXPECT_EQ(2,intersections.size());

	//Clear intersections and add them back
	intersections.clear();
	intersections.push_back(inter1);
	intersections.push_back(inter2);
	intersections.push_back(inter3);

	//Triangle in second position with tmin lower than triangle.t
	EXPECT_FLOAT_EQ((decimal)2.,leafTreatment::keepAllBeforeTriangle(intersections,2.));
	EXPECT_EQ(1,intersections.size());

	//Clear intersections and add them back
	intersections.clear();
	intersections.push_back(inter1);
	intersections.push_back(inter3);

	//No triangle
	EXPECT_FLOAT_EQ((decimal)6.,leafTreatment::keepAllBeforeTriangle(intersections,6.));
	EXPECT_EQ(2,intersections.size());
}

TEST(test_leaf_treatment, keep_all_before_visible)
{

	std::list<Intersection> intersections;

	Intersection inter1;
	inter1.t=2;
	inter1.p=new Cylindre();
	inter1.forme=CYLINDRE;
	inter1.normal=vec3(0,0,0);

	Intersection inter2;
	inter2.t=4;
	inter2.p=new Shape();
	inter2.forme=TRIANGLE;
	inter2.normal=vec3(0,0,0);

	Intersection inter3;
	inter3.t=6;
	inter3.p=new Cylindre();
	inter3.forme=CYLINDRE;
	inter3.normal=vec3(0,0,0);

	//No intersections
	EXPECT_FLOAT_EQ((decimal)1.,leafTreatment::keepAllBeforeVisible(intersections,1.));
	EXPECT_EQ(0,intersections.size());

	//Add intersections
	intersections.push_back(inter1);
	intersections.push_back(inter2);
	intersections.push_back(inter3);

	//visible shape in second position with current tmin=-1
	EXPECT_FLOAT_EQ((decimal)4.,leafTreatment::keepAllBeforeVisible(intersections,-1.));
	EXPECT_EQ(2,intersections.size());

	//Clear intersections and add them back
	intersections.clear();
	intersections.push_back(inter1);
	intersections.push_back(inter2);
	intersections.push_back(inter3);

	//visible shape in second position with tmin lower than inter.2.p.t
	EXPECT_FLOAT_EQ((decimal)2.,leafTreatment::keepAllBeforeVisible(intersections,2.));
	EXPECT_EQ(1,intersections.size());

	//Clear intersections and add them back
	intersections.clear();
	intersections.push_back(inter1);
	intersections.push_back(inter3);

	//No triangle
	EXPECT_FLOAT_EQ((decimal)6.,leafTreatment::keepAllBeforeVisible(intersections,6.));
	EXPECT_EQ(2,intersections.size());
}

TEST(test_leaf_treatment, keep_all)
{

	std::list<Intersection> intersections;

	Intersection inter1;
	inter1.t=2;
	inter1.p=new Cylindre();
	inter1.forme=CYLINDRE;
	inter1.normal=vec3(0,0,0);

	Intersection inter2;
	inter2.t=4;
	inter2.p=new Shape();
	inter2.forme=TRIANGLE;
	inter2.normal=vec3(0,0,0);

	Intersection inter3;
	inter3.t=6;
	inter3.p=new Cylindre();
	inter3.forme=CYLINDRE;
	inter3.normal=vec3(0,0,0);

	//No intersections
	EXPECT_FLOAT_EQ((decimal)1.,leafTreatment::keepAll(intersections,1.));
	EXPECT_EQ(0,intersections.size());

	//Add intersections
	intersections.push_back(inter1);
	intersections.push_back(inter2);
	intersections.push_back(inter3);

	//visible shape in second position with current tmin=-1
	EXPECT_FLOAT_EQ((decimal)2.,leafTreatment::keepAll(intersections,-1.));
	EXPECT_EQ(3,intersections.size());

	
}

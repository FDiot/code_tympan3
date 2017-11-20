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
#include "Geometry/mathlib.h"

using std::cout;
using std::cerr;
using std::endl;

/***********************************************************************
								 3d
************************************************************************/


// Test the ABS() function
TEST(test_3d, abs)
{

	double A=14.54;
	double B=-54.85;
	double C=4.68;
	double D=-8.7;

	EXPECT_DOUBLE_EQ(14.54,ABS(A)); 
	EXPECT_DOUBLE_EQ(54.85,ABS(B));
	EXPECT_DOUBLE_EQ(4.68,ABS(C)); 
	EXPECT_DOUBLE_EQ(8.7,ABS(D)); 
	
}

// Test the MAX() function
TEST(test_3d, max)
{

	double A=-65.17;
	double B=734.45;
	double C=41.978;
	double D=-26.87;

	EXPECT_DOUBLE_EQ(B,MAX(A,B)); //B>A => return B
	EXPECT_DOUBLE_EQ(C,MAX(A,C)); //C>A => return C
	EXPECT_DOUBLE_EQ(D,MAX(A,D)); //D>A => return D
	EXPECT_DOUBLE_EQ(B,MAX(B,C)); //B>C => return B
	EXPECT_DOUBLE_EQ(B,MAX(B,D)); //B>D => return B
	EXPECT_DOUBLE_EQ(C,MAX(C,D)); //C>D => return D
}

// Test the MAX3() function
TEST(test_3d, max3)
{

	double A=5.6;
	double B=21.5;
	double C=-9.7;
	double D=-75.8;

	EXPECT_DOUBLE_EQ(B,MAX3(A,B,C)); //B>A>C => return B
	EXPECT_DOUBLE_EQ(B,MAX3(A,B,D)); //B>A>D => return B
	EXPECT_DOUBLE_EQ(A,MAX3(A,C,D)); //A>C>D => return A
	EXPECT_DOUBLE_EQ(B,MAX3(B,D,C)); //B>C>D => return B
}

// Test the MAX() function
TEST(test_3d, min)
{

	double A=78.65;
	double B=-3.65;
	double C=-98.54;
	double D=-87.5;

	EXPECT_DOUBLE_EQ(B,MIN(A,B)); //B<A => return B
	EXPECT_DOUBLE_EQ(C,MIN(A,C)); //C<A => return C
	EXPECT_DOUBLE_EQ(D,MIN(A,D)); //D<A => return D
	EXPECT_DOUBLE_EQ(C,MIN(B,C)); //C<B => return C
	EXPECT_DOUBLE_EQ(D,MIN(B,D)); //D<B => return D
	EXPECT_DOUBLE_EQ(C,MIN(C,D)); //C<D => return C
}

// Test the DEGTORAD() function
TEST(test_3d, degtorad)
{

	double A=0;
	double B=-90;
	double C=180;
	double D=-270;

	EXPECT_DOUBLE_EQ(A*M_PI/180,DEGTORAD(A)); 
	EXPECT_DOUBLE_EQ(B*M_PI/180,DEGTORAD(B));
	EXPECT_DOUBLE_EQ(C*M_PI/180,DEGTORAD(C)); 
	EXPECT_DOUBLE_EQ(D*M_PI/180,DEGTORAD(D)); 
}

// Test the RADTODEG() function
TEST(test_3d, radtodeg)
{

	double A=0;
	double B=1.56;
	double C=-3.48;
	double D=2.45;

	EXPECT_DOUBLE_EQ(A*180/M_PI,RADTODEG(A)); 
	EXPECT_DOUBLE_EQ(B*180/M_PI,RADTODEG(B));
	EXPECT_DOUBLE_EQ(C*180/M_PI,RADTODEG(C)); 
	EXPECT_DOUBLE_EQ(D*180/M_PI,RADTODEG(D)); 
}

// Test the GRATORAD() function
TEST(test_3d, gratorad)
{

	double A=150.15;
	double B=-46.7;
	double C=57.85;
	double D=-19.46;

	EXPECT_DOUBLE_EQ(A*M_PI/200,GRATORAD(A)); 
	EXPECT_DOUBLE_EQ(B*M_PI/200,GRATORAD(B));
	EXPECT_DOUBLE_EQ(C*M_PI/200,GRATORAD(C)); 
	EXPECT_DOUBLE_EQ(D*M_PI/200,GRATORAD(D)); 
}

// Test the RADTOGRA() function
TEST(test_3d, radtogra)
{

	double A=0.56;
	double B=-1.78;
	double C=-4.8;
	double D=3.1;

	EXPECT_DOUBLE_EQ(A*200/M_PI,RADTOGRA(A)); 
	EXPECT_DOUBLE_EQ(B*200/M_PI,RADTOGRA(B));
	EXPECT_DOUBLE_EQ(C*200/M_PI,RADTOGRA(C)); 
	EXPECT_DOUBLE_EQ(D*200/M_PI,RADTOGRA(D)); 
}


// Test the GRATODEG() function
TEST(test_3d, gratodeg)
{

	double A=-14.58;
	double B=-251.4;
	double C=318.78;
	double D=-0.89;

	EXPECT_DOUBLE_EQ(A*180/200,GRATODEG(A)); 
	EXPECT_DOUBLE_EQ(B*180/200,GRATODEG(B));
	EXPECT_DOUBLE_EQ(C*180/200,GRATODEG(C)); 
	EXPECT_DOUBLE_EQ(D*180/200,GRATODEG(D)); 
}


// Test the DEGTOGRA() function
TEST(test_3d, degtogra)
{

	double A=14.72;
	double B=-91.25;
	double C=28.6;
	double D=-15.4;

	EXPECT_DOUBLE_EQ(A*200/180,DEGTOGRA(A)); 
	EXPECT_DOUBLE_EQ(B*200/180,DEGTOGRA(B));
	EXPECT_DOUBLE_EQ(C*200/180,DEGTOGRA(C)); 
	EXPECT_DOUBLE_EQ(D*200/180,DEGTOGRA(D)); 
}


// Test the ROUND() function
TEST(test_3d,round){

	double A=1.499999999;
	double B=4.5;
	double C=-1.4999999;
	double D=-5.5;

	EXPECT_EQ(1,ROUND(A)); 
	EXPECT_EQ(5,ROUND(B));
	EXPECT_EQ(-1,ROUND(C)); 
	EXPECT_EQ(-6,ROUND(D)); 
}


// Test the BORNE() function
TEST(test_3d,borne){

	EXPECT_EQ(10,BORNE(10,0,50)); 
	EXPECT_EQ(50,BORNE(50,0,50)); 
	EXPECT_EQ(50,BORNE(55,0,50)); 
	EXPECT_EQ(0,BORNE(-1,0,50)); 
	EXPECT_EQ(-1,BORNE(-1,-10,50)); 
	EXPECT_EQ(-40,BORNE(-40,-50,-10)); 
	EXPECT_EQ(-10,BORNE(0,-50,-10));
	EXPECT_EQ(-10,BORNE(-10,-50,-10));
	EXPECT_EQ(-50,BORNE(-60,-50,-10));

	EXPECT_EQ(0,BORNE(-1));
	EXPECT_EQ(48,BORNE(48));
	EXPECT_EQ(255,BORNE(256));
}






/***********************************************************************
						        OCoord3D
************************************************************************/

// Test the OCoord3D == operator (precision = 1e6)
TEST(test_ocoord3d,equal_operator){

	EXPECT_TRUE(OCoord3D(1.4576334,67.578215,51.47376)==OCoord3D(1.457634,67.5782142,51.4737591));
	EXPECT_TRUE(OCoord3D(-6.8712344,8.1346852,-48.6739185)==OCoord3D(-6.871235,8.1346848,-48.6739191));
	EXPECT_TRUE(OCoord3D(27.6891245,-4.5246738,-7.6213584)==OCoord3D(27.6891238,-4.5246731,-7.6213575));
}

// Test the OCoord3D != operator 
TEST(test_ocoord3d,not_equal_operator){

	EXPECT_TRUE(OCoord3D(2.4576334,67.578215,51.47376)!=OCoord3D(1.457634,67.5782142,51.4737591));
	EXPECT_TRUE(OCoord3D(-6.8712344,8.1345852,-48.6739185)!=OCoord3D(-6.871235,8.1346848,-48.6739191));
	EXPECT_TRUE(OCoord3D(27.6891245,-4.5246738,-7.6213584)!=OCoord3D(27.6891234,-4.5246731,-7.6213575));
}







/***********************************************************************
						        OVector3D
************************************************************************/


// Test the OVector3D from-to constructor
TEST(test_ovector3d,from_to_constructor){

	EXPECT_TRUE(OVector3D(4,5,1)==OVector3D(OCoord3D(13,49,6),OCoord3D(17,54,7)));
	EXPECT_TRUE(OVector3D(-7,8,9)==OVector3D(OCoord3D(17,-3,5),OCoord3D(10,5,14)));
	EXPECT_TRUE(OVector3D(2,-5,-3)==OVector3D(OCoord3D(4,6,28),OCoord3D(6,1,25)));
}

// Test the OVector3D == operator (precision = 1e6)
TEST(test_ovector3d,equal_operator){

	EXPECT_TRUE(OVector3D(4.6123489,35.1246854,6.1245875)==OVector3D(4.6123498,35.1246845,6.1245884));
	EXPECT_TRUE(OVector3D(-7.4562864,-36.1568634,8.4523659)==OVector3D(-7.4562873,-36.1568625,8.4523668));
	EXPECT_TRUE(OVector3D(-85.4652319,21.8546299,42.1256489)==OVector3D(-85.465231,21.85463,42.125648));
}

// Test the OVector3D != operator (precision = 1e6)
TEST(test_ovector3d,not_equal_operator){

	EXPECT_TRUE(OVector3D(4.6123487,35.1246854,6.1245875)!=OVector3D(4.6123498,35.1246845,6.1245884));
	EXPECT_TRUE(OVector3D(-7.4562864,-36.1568636,8.4523659)!=OVector3D(-7.4562873,-36.1568625,8.4523668));
	EXPECT_TRUE(OVector3D(-85.4652319,21.8546299,42.1256491)!=OVector3D(-85.465231,21.85463,42.125648));
	
}

// Test the OVector3D + operator  
TEST(test_ovector3d,addition_operator){

	EXPECT_TRUE(OVector3D(14,5,68)+OVector3D(-7,5,-8)==OVector3D(7,10,60));
	EXPECT_TRUE(OVector3D(-8,-46,24)+OVector3D(51,-13,-7)==OVector3D(43,-59,17));
	EXPECT_TRUE(OVector3D(-64,76,18)+OVector3D(-27,-65,41)==OVector3D(-91,11,59));
}

// Test the OVector3D - operator  
TEST(test_ovector3d,subtraction_operator){

	EXPECT_TRUE(OVector3D(14,5,68)-OVector3D(-7,5,-8)==OVector3D(21,0,76));
	EXPECT_TRUE(OVector3D(-8,-46,24)-OVector3D(51,-13,-7)==OVector3D(-59,-33,31));
	EXPECT_TRUE(OVector3D(-64,76,18)-OVector3D(-27,-65,41)==OVector3D(-37,141,-23));
}

// Test the OVector3D * operator  
TEST(test_ovector3d,multiplication_operator){

	EXPECT_TRUE(OVector3D(4,5,8)*OVector3D(-7,5,-8)==OVector3D(-28,25,-64));
	EXPECT_TRUE(OVector3D(-8,-6,2)*OVector3D(1,-3,-9)==OVector3D(-8,18,-18));
	EXPECT_TRUE(OVector3D(-3,6,8)*OVector3D(-4,-5,-1)==OVector3D(12,-30,-8));
	EXPECT_TRUE(3*OVector3D(5,-4,12)==OVector3D(15,-12,36));
}

// Test the OVector3D cross() product
TEST(test_ovector3d,cross_product){

	EXPECT_TRUE(OVector3D(4,5,8).cross(OVector3D(-7,5,-8))==OVector3D(-80,-24,55));
	EXPECT_TRUE(OVector3D(-8,-6,2).cross(OVector3D(1,-3,-9))==OVector3D(60,-70,30));
	EXPECT_TRUE(OVector3D(-3,6,8).cross(OVector3D(-4,-5,-1))==OVector3D(34,-35,39));
	EXPECT_TRUE(OVector3D(-3,6,8).cross(-3*OVector3D(-3,6,8))==OVector3D(0,0,0));
}

// Test the OVector3D scalar() product
TEST(test_ovector3d,scalar_product){

	EXPECT_EQ(-67,OVector3D(4,5,8).scalar(OVector3D(-7,5,-8)));
	EXPECT_EQ(-8,OVector3D(-8,-6,2).scalar(OVector3D(1,-3,-9)));
	EXPECT_EQ(-26,OVector3D(-3,6,8).scalar(OVector3D(-4,-5,-1)));
	
}

// Test the OVector3D balance() function
TEST(test_ovector3d,balance){

	OVector3D u=OVector3D(4,5,8);
	u.balance(5,OVector3D(-7,5,-8),-3);
	EXPECT_TRUE(u==OVector3D(41,10,64));

	u=OVector3D(-8,-6,2);
	u.balance(-2,OVector3D(1,-3,-9),7);
	EXPECT_TRUE(u==OVector3D(23,-9,-67));

	u=OVector3D(-3,6,8);
	u.balance(8,OVector3D(-4,-5,-1),-10);
	EXPECT_TRUE(u==OVector3D(16,98,74));
}

// Test the OVector3D norme() function 
TEST(test_ovector3d,norme){

	EXPECT_EQ(sqrt(14.),OVector3D(1,-2,3).norme());
	EXPECT_EQ(sqrt(101.),OVector3D(-4,-6,7).norme());
	EXPECT_EQ(sqrt(170.),OVector3D(5,8,-9).norme());
}

// Test the OVector3D normalization 
TEST(test_ovector3d,normalize){

	OVector3D u=OVector3D(6,1,-4);
	u.normalize();
	EXPECT_TRUE(u*sqrt(53.)==OVector3D(6,1,-4));

	u=OVector3D(-3,7,-1);
	u.normalize();
	EXPECT_TRUE(u*sqrt(59.)==OVector3D(-3,7,-1));

	u=OVector3D(-5,4,3);
	u.normalize();
	EXPECT_TRUE(u*sqrt(50.)==OVector3D(-5,4,3));

}

// Test the OVector3D normal() function 
TEST(test_ovector3d,normal){

	OVector3D u=OVector3D(0,0,0);
	OVector3D n=OVector3D(0,0,1);
	EXPECT_TRUE(u.normal(OVector3D(1,0,0),OVector3D(0,1,0))==n);
	EXPECT_TRUE(u.normal(OVector3D(0,1,0),OVector3D(1,0,0))==-1*n);

	u=OVector3D(0,0,0);
	n=OVector3D(1,1,1);
	EXPECT_TRUE(u.normal(OVector3D(1,0,-1),OVector3D(-1,1,0))==n);
	EXPECT_TRUE(u.normal(OVector3D(-1,1,0),OVector3D(1,0,-1))==-1*n);

	u=OVector3D(4,7,-1);
	n=OVector3D(10,2,0);
	EXPECT_TRUE(u.normal(OVector3D(5,2,0),OVector3D(6,-3,-1))==n);
	EXPECT_TRUE(u.normal(OVector3D(6,-3,-1),OVector3D(5,2,0))==-1*n);
	
	u=OVector3D(-9,4,-3);
	n=OVector3D(-1,-194,-105);
	EXPECT_TRUE(u.normal(OVector3D(7,5,-5),OVector3D(0,-2,8))==n);
	EXPECT_TRUE(u.normal(OVector3D(0,-2,8),OVector3D(7,5,-5))==-1*n);	
}

// Test the OVector3D invert() function 
TEST(test_ovector3d,invert){

	OVector3D u=OVector3D(6,1,-4);
	u.invert();
	EXPECT_TRUE(-1*u==OVector3D(6,1,-4));

	u=OVector3D(-3,7,-1);
	u.invert();
	EXPECT_TRUE(-1*u==OVector3D(-3,7,-1));

	u=OVector3D(-5,4,3);
	u.invert();
	EXPECT_TRUE(-1*u==OVector3D(-5,4,3));

}


// Test the OVector3D angle() function 
TEST(test_ovector3d,angle){

	OVector3D u=OVector3D(1,0,0);
	EXPECT_DOUBLE_EQ(90.,RADTODEG(u.angle(OVector3D(0,1,0))));

	u=OVector3D(1,1,0);
	EXPECT_DOUBLE_EQ(45.,1.0*RADTODEG(u.angle(OVector3D(0,1,0))));

	u=OVector3D(-1,1,0);
	EXPECT_DOUBLE_EQ(135.,RADTODEG(u.angle(OVector3D(1,0,0))));
}

// Test the OVector3D getRotationOz() function 
TEST(test_ovector3d,get_rotation_oz){

	EXPECT_TRUE(OVector3D(0,1,5)==OVector3D(1,0,5).getRotationOz(M_PIDIV2));
	EXPECT_TRUE(OVector3D(-1/sqrt(2.),1/sqrt(2.),3)==OVector3D(0,1,3).getRotationOz(M_PI/4));
	EXPECT_TRUE(OVector3D(-sqrt(3.)/2+1,sqrt(3.)+1./2,-3)==OVector3D(-1,2,-3).getRotationOz(-M_PI/6));
	EXPECT_TRUE(OVector3D((-5-3*sqrt(3.))/2,(-5*sqrt(3.)+3)/2,2)==OVector3D(5,-3,2).getRotationOz(4*M_PI/3));
	
}

// Test the OVector3D getRotationOzBase2() function 
TEST(test_ovector3d,get_rotation_oz_base2){

	EXPECT_TRUE(OVector3D(0,1,5)==OVector3D(1,0,5).getRotationOzBase2(-M_PIDIV2));
	EXPECT_TRUE(OVector3D(-1/sqrt(2.),1/sqrt(2.),3)==OVector3D(0,1,3).getRotationOzBase2(-M_PI/4));
	EXPECT_TRUE(OVector3D(-sqrt(3.)/2+1,sqrt(3.)+1./2,-3)==OVector3D(-1,2,-3).getRotationOzBase2(M_PI/6));
	EXPECT_TRUE(OVector3D((-5-3*sqrt(3.))/2,(-5*sqrt(3.)+3)/2,2)==OVector3D(5,-3,2).getRotationOzBase2(-4*M_PI/3));
	
}

// Test the OVector3D getRotationOy() function 
TEST(test_ovector3d,get_rotation_oy){

	
	EXPECT_TRUE(OVector3D(0,5,1)==OVector3D(1,5,0).getRotationOy(M_PIDIV2));
	EXPECT_TRUE(OVector3D(-3/sqrt(2.),1,3/sqrt(2.))==OVector3D(0,1,3).getRotationOy(M_PI/4));
	EXPECT_TRUE(OVector3D((-sqrt(3.)-3)/2,2,(1-3*sqrt(3.))/2)==OVector3D(-1,2,-3).getRotationOy(-M_PI/6));
	EXPECT_TRUE(OVector3D(sqrt(3.)-2.5,-3,-5*sqrt(3.)/2-1)==OVector3D(5,-3,2).getRotationOy(4*M_PI/3));

}

// Test the OVector3D getRotationOyBase2() function 
TEST(test_ovector3d,get_rotation_oy_base2){

	EXPECT_TRUE(OVector3D(0,5,1)==OVector3D(1,5,0).getRotationOyBase2(-M_PIDIV2));
	EXPECT_TRUE(OVector3D(-3/sqrt(2.),1,3/sqrt(2.))==OVector3D(0,1,3).getRotationOyBase2(-M_PI/4));
	EXPECT_TRUE(OVector3D((-sqrt(3.)-3)/2,2,(1-3*sqrt(3.))/2)==OVector3D(-1,2,-3).getRotationOyBase2(M_PI/6));
	EXPECT_TRUE(OVector3D(sqrt(3.)-2.5,-3,-5*sqrt(3.)/2-1)==OVector3D(5,-3,2).getRotationOyBase2(-4*M_PI/3));

}

// Test the OVector3D getRotationOzOy() function 
TEST(test_ovector3d,get_rotation_oz_oy){


	EXPECT_TRUE(OVector3D(-7,-4,2).getRotationOz(3*M_PIDIV2)==OVector3D(-7,-4,2).getRotationOzOy(3*M_PIDIV2,0));
	EXPECT_TRUE(OVector3D(2,-1,-3).getRotationOy(-M_PI/3)==OVector3D(2,-1,-3).getRotationOzOy(0,-M_PI/3));
	EXPECT_TRUE(OVector3D(5,6,-4).getRotationOy(4*M_PI/3).getRotationOz(-5*M_PI/6)==OVector3D(5,6,-4).getRotationOzOy(-5*M_PI/6,4*M_PI/3));
	
}





/***********************************************************************
						        OPoint3D
************************************************************************/

// Test the OPoint3D setFromOGL() function 
TEST(test_point3d,set_from_ogl){

	OPoint3D point3d;
	point3d.setFromOGL(1,2,3);
	EXPECT_TRUE(OPoint3D(1,-3,2)==point3d);
	
}

// Test the OPoint3D getToOGL() function 
TEST(test_point3d,get_to_ogl){

	OPoint3D point3d(1,2,3);
	float x,y,z;
	point3d.getToOGL(x,y,z);
	EXPECT_EQ(1,x);
	EXPECT_EQ(3,y);
	EXPECT_EQ(-2,z);
}

// Test the OPoint3D distFrom() function 
TEST(test_point3d,dist_from){

	EXPECT_EQ(sqrt(3.),OPoint3D(0,0,0).distFrom(OPoint3D(1,1,1)));
	EXPECT_EQ(sqrt(99.),OPoint3D(4,-9,7).distFrom(OPoint3D(-3,-2,6)));
	EXPECT_EQ(sqrt(144.),OPoint3D(-6,2,5).distFrom(OPoint3D(-2,10,-3)));
}


// Test the OPoint3D dist2DFrom() function 
TEST(test_point3d,dist_2d_from){

	EXPECT_EQ(sqrt(2.),OPoint3D(0,0,0).dist2DFrom(OPoint3D(1,1,1)));
	EXPECT_EQ(sqrt(98.),OPoint3D(4,-9,7).dist2DFrom(OPoint3D(-3,-2,6)));
	EXPECT_EQ(sqrt(80.),OPoint3D(-6,2,5).dist2DFrom(OPoint3D(-2,10,-3)));
}


// Test the OPoint3D checkPointsMaxDistance() function 
TEST(test_point3d,check_points_max_distance){

	  TabPoint3D tab;

	  EXPECT_EQ( OPoint3D::checkPointsMaxDistance(tab,1.),tab);

	  tab.push_back(OPoint3D(4,-1,8));
	  tab.push_back(OPoint3D(7,2,5));
	  tab.push_back(OPoint3D(6,3,2));

	  EXPECT_EQ( OPoint3D::checkPointsMaxDistance(tab,.1),tab);

	  TabPoint3D result=OPoint3D::checkPointsMaxDistance(tab,2.);
	  EXPECT_EQ(7,result.size());
	  EXPECT_EQ(OPoint3D(4,-1,8),result.at(0));
	  EXPECT_EQ(OPoint3D(4.75,-0.25,7.25),result.at(1));
	  EXPECT_EQ(OPoint3D(5.5,0.5,6.5),result.at(2));
	  EXPECT_EQ(OPoint3D(6.25,1.25,5.75),result.at(3));
	  EXPECT_EQ(OPoint3D(7,2,5),result.at(4));
	  EXPECT_EQ(OPoint3D(6.5,2.5,3.5),result.at(5));
	  EXPECT_EQ(OPoint3D(6,3,2),result.at(6));

	  
	  result=OPoint3D::checkPointsMaxDistance(tab,3.);
	  EXPECT_EQ(5,result.size());
	  EXPECT_EQ(OPoint3D(4,-1,8),result.at(0));
	  EXPECT_EQ(OPoint3D(5.5,0.5,6.5),result.at(1));
	  EXPECT_EQ(OPoint3D(7,2,5),result.at(2));
	  EXPECT_EQ(OPoint3D(6.5,2.5,3.5),result.at(3));
	  EXPECT_EQ(OPoint3D(6,3,2),result.at(4));
}





/***********************************************************************
						        OMatrix
************************************************************************/

// Test the OMatrix + operator
TEST(test_3d_omatrix,addition_operator){
	double a[4][4]={{7,-5,9,6},{-1,6,-3,9},{0,-4,-2,-3},{6,8,-8,-1}};
	double b[4][4]={{-6,5,-4,3},{-9,-8,7,1},{3,6,-6,-4},{1,-6,-4,7}};

	//The 4th row and column are [0,0,0,1] because 3D geometrical transformations use homogenous coordinates
	double sum[4][4]={{1,0,5,0},{-10,-2,4,0},{3,2,-8,0},{0,0,0,1}};
	
	OMatrix A(a);
	OMatrix B(b);
	OMatrix S(sum);

	EXPECT_TRUE(A+B==S);
}


// Test the OMatrix - operator
TEST(test_3d_omatrix,subtraction_operator){
	double a[4][4]={{7,-5,9,6},{-1,6,-3,9},{0,-4,-2,-3},{6,8,-8,-1}};
	double b[4][4]={{-6,5,-4,3},{-9,-8,7,1},{3,6,-6,-4},{1,-6,-4,7}};

	//The 4th row and column are [0,0,0,1] because 3D geometrical transformations use homogenous coordinates
	double sub[4][4]={{13,-10,13,0},{8,14,-10,0},{-3,-10,4,0},{0,0,0,1}};
	
	OMatrix A(a);
	OMatrix B(b);
	OMatrix S(sub);

	EXPECT_TRUE(A-B==S);
}


// Test the OMatrix * operator
TEST(test_3d_omatrix,product_operator){
	double a[4][4]={{7,-5,9,6},{-1,6,-3,9},{0,-4,-2,-3},{6,8,-8,-1}};
	double b[4][4]={{-6,5,-4,3},{-9,-8,7,1},{3,6,-6,-4},{1,-6,-4,7}};

	double prod[4][4]={{36,93,-141,22},{-48,-125,28,78},{27,38,-4,-17},{-133,-76,84,51}};
	
	OMatrix A(a);
	OMatrix B(b);
	OMatrix P(prod);

	EXPECT_TRUE(A*B==P);
}


// Test the OMatrix dot() function
TEST(test_3d_omatrix,dot){
	
	double a[4][4]={{7,-5,9,6},{-1,6,-3,9},{0,-4,-2,-3},{6,8,-8,-1}};
	double b[4][4]={{-6,5,-4,3},{-9,-8,7,1},{3,6,-6,-4},{1,-6,-4,7}};
	
	OMatrix A(a);
	OMatrix B(b);

	EXPECT_TRUE(A.dot(OCoord3D(5,-7,3))==OCoord3D(97,-56,22));
	EXPECT_TRUE(B.dot(OCoord3D(-1,4,-5))==OCoord3D(46,-58,51));
}


// Test the OMatrix scale() function
TEST(test_3d_omatrix,scale){
	
	double a[4][4]={{7,-5,9,6},{-1,6,-3,9},{0,-4,-2,-3},{6,8,-8,-1}};
	double b[4][4]={{-6,5,-4,3},{-9,-8,7,1},{3,6,-6,-4},{1,-6,-4,7}};
	
	OMatrix A(a);
	OMatrix B(b);

	EXPECT_TRUE(A.scale(OCoord3D(5,-7,3))==OCoord3D(-0.0980392,0.137255,-0.0588235));
	EXPECT_TRUE(B.scale(OCoord3D(-1,4,-5))==OCoord3D(-0.5,2,-2.5));
	EXPECT_TRUE(B.scale(OCoord3D(-5,1,-1))==OCoord3D(-5,1,-1));
}


// Test the OMatrix * OVector3D operator
TEST(test_3d_omatrix,product_with_vector3d){

	double a[4][4]={{7,-5,9,6},{-1,6,-3,9},{0,-4,-2,-3},{6,8,-8,-1}};
	double b[4][4]={{-6,5,-4,3},{-9,-8,7,1},{3,6,-6,-4},{1,-6,-4,7}};
	OMatrix A(a);
	OMatrix B(b);

	EXPECT_TRUE(A*OVector3D(4,-2,-3)==OVector3D(-0.106796,0.0679612,-0.135922));
	EXPECT_TRUE(B*OVector3D(-1,8,4)==OVector3D(0.26087,-0.234783,0.182609));

}


// Test the OMatrix * OPoint3D operator
TEST(test_3d_omatrix,product_with_point3d){

	double a[4][4]={{7,-5,9,6},{-1,6,-3,9},{0,-4,-2,-3},{6,8,-8,-1}};
	double b[4][4]={{-6,5,-4,3},{-9,-8,7,1},{3,6,-6,-4},{1,-6,-4,7}};
	OMatrix A(a);
	OMatrix B(b);

	EXPECT_TRUE(A*OPoint3D(5,-9,-1)==OPoint3D(77,-47,35));
	EXPECT_TRUE(B*OPoint3D(-7,2,-3)==OPoint3D(67,27,5));
	
}



// Test the OMatrix multNormal() function
TEST(test_3d_omatrix,multNormal){

	double a[4][4]={{7,-5,9,6},{-1,6,-3,9},{0,-4,-2,-3},{6,8,-8,-1}};
	double b[4][4]={{-6,5,-4,3},{-9,-8,7,1},{3,6,-6,-4},{1,-6,-4,7}};
	OMatrix A(a);
	OMatrix B(b);

	EXPECT_TRUE(A.multNormal(OVector3D(4,-2,-3))==OVector3D(0.354839,-0.225806,0.451613));
	EXPECT_TRUE(B.multNormal(OVector3D(-1,8,4))==OVector3D(-0.517241,0.465517,-0.362069));

}


// Test the OMatrix setTranslation() function
TEST(test_3d_omatrix,set_translation){

	OMatrix A,B;

	A.setTranslation(0,0,0);
	B.unite();
	EXPECT_TRUE(A==B);

	A.setTranslation(-9,6,-4);
	double b[4][4]={{1,0,0,-9},{0,1,0,6},{0,0,1,-4},{0,0,0,1}};
	B=OMatrix(b);
	EXPECT_TRUE(A==B);
	EXPECT_TRUE(A*OPoint3D(3,-1,6)==OPoint3D(-6,5,2));
	EXPECT_TRUE(A*OPoint3D(-7,3,-5)==OPoint3D(-16,9,-9));
	EXPECT_TRUE(A*OPoint3D(2,-9,-4)==OPoint3D(-7,-3,-8));
}

// Test the OMatrix setScale() function
TEST(test_3d_omatrix,set_scale){

	OMatrix A,B;

	A.setScale(1,1,1);
	B.unite();
	EXPECT_TRUE(A==B);

	A.setScale(-1,5,3);
	double b[4][4]={{-1,0,0,0},{0,5,0,0},{0,0,3,0},{0,0,0,1}};
	B=OMatrix(b);
	EXPECT_TRUE(A==B);
	EXPECT_TRUE(A*OPoint3D(9,-4,1)==OPoint3D(-9,-20,3));
	EXPECT_TRUE(A*OPoint3D(-4,5,2)==OPoint3D(4,25,6));
	EXPECT_TRUE(A*OPoint3D(-1,8,-5)==OPoint3D(1,40,-15));

}


// Test the OMatrix setRotationOx() function
TEST(test_3d_omatrix,set_rotationox){

	OMatrix A,R1,R2;

	double theta=M_PI/2;
	A.setRotationOx(theta);
	double r1[4][4]={{1,0,0,0},{0,cos(theta),-sin(theta),0},{0,sin(theta),cos(theta),0},{0,0,0,1}};
	R1=OMatrix(r1);
	EXPECT_TRUE(A==R1);
	EXPECT_TRUE(A*OPoint3D(0,0,1)==OPoint3D(0,-1,0));
	EXPECT_TRUE(A*OPoint3D(0,1,0)==OPoint3D(0,0,1));

	theta=M_PI/3;
	A.setRotationOx(theta);
	double r2[4][4]={{1,0,0,0},{0,cos(theta),-sin(theta),0},{0,sin(theta),cos(theta),0},{0,0,0,1}};
	R2=OMatrix(r2);
	EXPECT_TRUE(A==R2);
	EXPECT_TRUE(A*OPoint3D(0,0,1)==OPoint3D(0,-sqrt(3.)/2,1./2));
	EXPECT_TRUE(A*OPoint3D(0,1,0)==OPoint3D(0,1./2,sqrt(3.)/2));
	
}

// Test the OMatrix setRotationOy() function
TEST(test_3d_omatrix,set_rotationoy){

	OMatrix A,R1,R2;

	double theta=-M_PI/4;
	A.setRotationOy(theta);
	double r1[4][4]={{cos(theta),0,sin(theta),0},{0,1,0,0},{-sin(theta),0,cos(theta),0},{0,0,0,1}};
	R1=OMatrix(r1);
	EXPECT_TRUE(A==R1);
	EXPECT_TRUE(A*OPoint3D(1,0,0)==OPoint3D(sqrt(2.)/2,0,sqrt(2.)/2));
	EXPECT_TRUE(A*OPoint3D(0,0,1)==OPoint3D(-sqrt(2.)/2,0,sqrt(2.)/2));

	theta=2*M_PI/3;
	A.setRotationOy(theta);
	double r2[4][4]={{cos(theta),0,sin(theta),0},{0,1,0,0},{-sin(theta),0,cos(theta),0},{0,0,0,1}};
	R2=OMatrix(r2);
	EXPECT_TRUE(A==R2);
	EXPECT_TRUE(A*OPoint3D(1,0,0)==OPoint3D(-1./2,0,-sqrt(3.)/2));
	EXPECT_TRUE(A*OPoint3D(0,0,1)==OPoint3D(sqrt(3.)/2,0,-1./2));

}


// Test the OMatrix setRotationOz() function
TEST(test_3d_omatrix,set_rotationoz){

	OMatrix A,R1,R2;

	double theta=4*M_PI/3;
	A.setRotationOz(theta);
	double r1[4][4]={{cos(theta),-sin(theta),0,0},{sin(theta),cos(theta),0,0},{0,0,1,0},{0,0,0,1}};
	R1=OMatrix(r1);
	EXPECT_TRUE(A==R1);
	EXPECT_TRUE(A*OPoint3D(1,0,0)==OPoint3D(-1./2,-sqrt(3.)/2,0));
	EXPECT_TRUE(A*OPoint3D(0,1,0)==OPoint3D(sqrt(3.)/2,-1./2,0));

	theta=-3*M_PI/4;
	A.setRotationOz(theta);
	double r2[4][4]={{cos(theta),-sin(theta),0,0},{sin(theta),cos(theta),0,0},{0,0,1,0},{0,0,0,1}};
	R2=OMatrix(r2);
	EXPECT_TRUE(A==R2);
	EXPECT_TRUE(A*OPoint3D(1,0,0)==OPoint3D(-sqrt(2.)/2,-sqrt(2.)/2,0));
	EXPECT_TRUE(A*OPoint3D(0,1,0)==OPoint3D(sqrt(2.)/2,-sqrt(2.)/2,0));
	
}


// Test the OMatrix aligneVecteurSurOx() function
TEST(test_3d_omatrix,align_vector_Ox){

	OMatrix M;

	//case of a vector with 0 norme
	OVector3D v(0,0,0);
	int res=M.aligneVecteurSurOx(v);
	EXPECT_EQ(0,res);


	//case of a vector already aligned with Ox
	v=OVector3D(8,0,0);
	res=M.aligneVecteurSurOx(v);
	EXPECT_EQ(1,res);
	EXPECT_TRUE(M*v==v);


	//case of a vector with x=y=0; 
	v=OVector3D(0,0,-5);
	res=M.aligneVecteurSurOx(v);
	EXPECT_EQ(1,res);
	EXPECT_TRUE(M*v==OVector3D(v.norme(),0,0));


	//general cases
	v=OVector3D(7,-4,-3);
	res=M.aligneVecteurSurOx(v);
	EXPECT_EQ(1,res);
	EXPECT_TRUE(M*v==OVector3D(v.norme(),0,0));


	v=OVector3D(-3,5,8);
	res=M.aligneVecteurSurOx(v);
	EXPECT_EQ(1,res);
	EXPECT_TRUE(M*v==OVector3D(v.norme(),0,0));

}


// Test the OMatrix aligneVecteurSurOy() function
TEST(test_3d_omatrix,align_vector_Oy){

	OMatrix M;

	//case of a vector with 0 norme
	OVector3D v(0,0,0);
	int res=M.aligneVecteurSurOy(v);
	EXPECT_EQ(0,res);


	//case of a vector already aligned with Ox
	v=OVector3D(0,23,0);
	res=M.aligneVecteurSurOy(v);
	EXPECT_EQ(1,res);
	EXPECT_TRUE(M*v==v);


	//case of a vector with x=z=0; 
	v=OVector3D(0,16,0);
	res=M.aligneVecteurSurOy(v);
	EXPECT_EQ(1,res);
	EXPECT_TRUE(M*v==OVector3D(0,v.norme(),0));


	//general cases
	v=OVector3D(4,-2,1);
	res=M.aligneVecteurSurOy(v);
	EXPECT_EQ(1,res);
	EXPECT_TRUE(M*v==OVector3D(0,v.norme(),0));


	v=OVector3D(-9,6,5);
	res=M.aligneVecteurSurOy(v);
	EXPECT_EQ(1,res);
	EXPECT_TRUE(M*v==OVector3D(0,v.norme(),0));

}


// Test the OMatrix invert() function
TEST(test_3d_omatrix,invert){
	int res;
	OMatrix unite;
	unite.unite();


	//none invertible case
	double m1[4][4]={{-6,5,-4,3},{-9,-8,7,1},{-12,10,-8,6},{1,-6,-4,7}};
	OMatrix M(m1);
	OMatrix invert=M.getInvert(&res);
	
	EXPECT_EQ(0,res);



	//General cases
	double m2[4][4]={{-6,5,-4,3},{-9,-8,7,1},{3,6,-6,-4},{1,-6,-4,7}};
	M=OMatrix(m2);

	invert=M.getInvert(&res);
	OMatrix product=M*invert;
	
	EXPECT_EQ(1,res);
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			EXPECT_TRUE(abs(product._m[i][j]-unite._m[i][j])<EPSILON_13);
		}
	}


	double m3[4][4]={{7,-5,9,6},{-1,6,-3,9},{0,-4,-2,-3},{6,8,-8,-1}};
	M=OMatrix(m3);

	invert=M.getInvert(&res);
	product=M*invert;

	EXPECT_EQ(1,res);
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			EXPECT_TRUE(abs(product._m[i][j]-unite._m[i][j])<EPSILON_13);
		}
	}

}

// Test the OMatrix mat2x2Det() function
TEST(test_3d_omatrix,mat2x2det){

	EXPECT_DOUBLE_EQ(35.,OMatrix::mat2x2Det(-1,8,-4,-3));
	EXPECT_DOUBLE_EQ(-33.,OMatrix::mat2x2Det(5,2,-1,-7));
	EXPECT_DOUBLE_EQ(14.,OMatrix::mat2x2Det(-6,-5,4,1));
}


// Test the OMatrix mat2x2Det() function
TEST(test_3d_omatrix,mat3x3det){

	EXPECT_DOUBLE_EQ(171.,OMatrix::mat3x3Det(7,-5,9,6,-1,6,-3,9,0));
	EXPECT_DOUBLE_EQ(20.,OMatrix::mat3x3Det(-4,-2,-3,6,8,-8,-1,-4,7));
	EXPECT_DOUBLE_EQ(-475.,OMatrix::mat3x3Det(-6,5,-4,3,-9,-8,7,1,3));
}	


// Test the OMatrix determinant() function
TEST(test_3d_omatrix,determinant){
	double m1[4][4]={{-6,5,-4,3},{-9,-8,7,1},{-12,10,-8,6},{1,-6,-4,7}};
	OMatrix M(m1);
	EXPECT_DOUBLE_EQ(0.,M.determinant());

	double m2[4][4]={{7,-5,9,6},{-1,6,-3,9},{0,-4,-2,-3},{6,8,-8,-1}};
	M=OMatrix(m2);
	EXPECT_DOUBLE_EQ(5480.,M.determinant());
}


// Test the OMatrix adjoint() function
TEST(test_3d_omatrix,adjoint){

	double m1[4][4]={{-7,5,2,-1},{-3,8,-4,1},{-2,0,8,-4},{5,-8,7,-9}};
	OMatrix M(m1);
	OMatrix A=M.getAdjoint();
	double res1[4][4]={{-416,220,249,-40},{-230,286,246,-52},{-192,-18,354,-138},{-176,-146,195,-256}};
	OMatrix R(res1);

	EXPECT_TRUE(R==A);

	
	double m2[4][4]={{-6,4,6,-5},{7,-5,-6,2},{2,8,-9,7},{-9,-5,4,7}};
	M=OMatrix(m2);
	A=M.getAdjoint();
	double res2[4][4]={{975,845,200,255},{321,449,-28,129},{1146,1164,282,204},{828,742,76,42}};
	R=OMatrix(res2);

	EXPECT_TRUE(R==A);
}








/***********************************************************************
						       OGeometrie
************************************************************************/

// Test the OGeometrie intersDemiSegmentAvecSegment() function
TEST(test_3d_ogeometrie,inters_demi_segment_avec_segment){

	EXPECT_TRUE(OGeometrie::intersDemiSegmentAvecSegment(OPoint3D(0,0,0),OPoint3D(10,10,0),OPoint3D(10,-10,0)));
	EXPECT_FALSE(OGeometrie::intersDemiSegmentAvecSegment(OPoint3D(0,11,0),OPoint3D(10,10,0),OPoint3D(10,-10,0)));
	EXPECT_FALSE(OGeometrie::intersDemiSegmentAvecSegment(OPoint3D(0,-11,0),OPoint3D(10,10,0),OPoint3D(10,-10,0)));
	EXPECT_FALSE(OGeometrie::intersDemiSegmentAvecSegment(OPoint3D(20,0,0),OPoint3D(10,10,0),OPoint3D(10,-10,0)));

	EXPECT_TRUE(OGeometrie::intersDemiSegmentAvecSegment(OPoint3D(-2,2,0),OPoint3D(1,1,0),OPoint3D(4,5,0)));
	EXPECT_FALSE(OGeometrie::intersDemiSegmentAvecSegment(OPoint3D(3,6,0),OPoint3D(1,1,0),OPoint3D(4,5,0)));
	EXPECT_FALSE(OGeometrie::intersDemiSegmentAvecSegment(OPoint3D(-10,0,0),OPoint3D(1,1,0),OPoint3D(4,5,0)));
	EXPECT_FALSE(OGeometrie::intersDemiSegmentAvecSegment(OPoint3D(6,2,0),OPoint3D(1,1,0),OPoint3D(4,5,0)));

}

// Test the OGeometrie intersDroitesPoints() function
TEST(test_3d_ogeometrie,inters_droites_points){

	// Points of the two segments
    OPoint3D ptA(-10.0, 23.0, 0.0), ptB(10.0, -17.0, 0.0), ptC(-10.0, -32.0, 0.0), ptD(10.0, 28.0, 0.0), ptI;

    // Both lines cross -> TRUE
    int resu = OGeometrie::intersDroitesPoints(ptA, ptB, ptC, ptD, ptI);
    EXPECT_TRUE(resu == 1);
    EXPECT_TRUE(ptI == OPoint3D(1.0, 1.0, 0.0));

    // Lines do not cross -> FALSE
    ptA._y = -1030, ptB._y = -970;
    resu = OGeometrie::intersDroitesPoints(ptA, ptB, ptC, ptD, ptI);
    EXPECT_TRUE(resu == 0);
}

// Test the OGeometrie intersDroitesPointVecteur() function
TEST(test_3d_ogeometrie, inters_droites_point_vecteur)
{
    //  Define points and vectors
    OPoint3D ptA0(-10.0, 23.0, 0.0), ptA1(10.0, -17.0, 0.0), ptB0(-10.0, -32.0, 0.0), ptB1(10.0, 28.0, 0.0), ptI;
    OVector3D vecA(ptA0, ptA1), vecB(ptB0, ptB1);

    // Both lines cross at (1,1,0) -> TRUE
    int resu = OGeometrie::intersDroitesPointVecteur(ptA0, vecA, ptB0, vecB, ptI);
    EXPECT_TRUE(resu == 1);
    EXPECT_TRUE(ptI == OPoint3D(1.0, 1.0, 0.0));

    // LInes are parallel -> FALSE
    ptA0._y = -1030, ptA1._y = -970;
    vecA = vecB;
    resu = OGeometrie::intersDroitesPointVecteur(ptA0, vecA, ptB0, vecB, ptI);
    EXPECT_TRUE(resu == 0);
}

// Test the OGeometrie symPointDroite() function
TEST(test_3d_ogeometrie, sym_point_droite)
{
    // Defines points
    OPoint3D ptA(-20.0, 0.0, 0.0), ptB(20.0, 0.0, 0.0), ptP(-20.0, 0.0, 2.0), ptI;

    // Compute the point suymetrical to ptP with respect to the axis pTA-pTB
    double k = OGeometrie::symPointDroite(ptA, ptB, ptP, ptI);

    EXPECT_TRUE(ptI == OPoint3D(-20.0, 0.0, -2.0));
    EXPECT_TRUE(k == 0.0);// Projection of P onto the axis AB

}

// Test the OGeometrie pointInPolygonAngleSum() function
TEST(test_3d_ogeometrie, point_in_polygon_angle_sum)
{
    // Array of points
    OPoint3D pts[6];
    pts[0] = OPoint3D(1.0, 1.0, 0.0);
    pts[1] = OPoint3D(5.0, 6.0, 0.0);
    pts[2] = OPoint3D(-1.0, 10.0, 0.0);
    pts[3] = OPoint3D(-10.0, -2.0, 0.0);
    pts[4] = OPoint3D(1.0, -8.0, 0.0);
    pts[5] = OPoint3D(6.0, -3.0, 0.0);

    // Point to test
    OPoint3D ptP(-3.0, 3.0, 0.0);
    bool resu = OGeometrie::pointInPolygonAngleSum(ptP, pts, 6);

    // Poiunt is in the polygon -> TRUE
    EXPECT_TRUE(resu == true);

    // Point is not in the polygon -> FALSE
    ptP._x = 4.0;
    resu = OGeometrie::pointInPolygonAngleSum(ptP, pts, 6);
    EXPECT_FALSE(resu == true);
}

// Test the OGeometrie pointInPolygonAngleSum() function
TEST(test_3d_ogeometrie, pointInPolygonRayCasting)
{
    // Array of points
    OPoint3D pts[6];
    pts[0] = OPoint3D(1.0, 1.0, 0.0);
    pts[1] = OPoint3D(5.0, 6.0, 0.0);
    pts[2] = OPoint3D(-1.0, 10.0, 0.0);
    pts[3] = OPoint3D(-10.0, -2.0, 0.0);
    pts[4] = OPoint3D(1.0, -8.0, 0.0);
    pts[5] = OPoint3D(6.0, -3.0, 0.0);

    // Compute the normal of the face
    OVector3D normale;
    OGeometrie::computeNormal(pts, 6, normale);

    // Bounding box
    OBox box(OCoord3D(-10.0, -8.0, 0.0), OCoord3D(6.0, 10.0, 0.0));

    // Point to be tested
    OPoint3D ptP(-3.0, 3.0, 0.0);
    bool resu = OGeometrie::pointInPolygonRayCasting(ptP, pts, 6);

    // Point in polygon -> TRUE
    EXPECT_TRUE(resu == true);

    // Point not in polygon -> FALSE
    ptP._x = 4.0;
    resu = OGeometrie::pointInPolygonRayCasting(ptP, pts, 6);
    EXPECT_FALSE(resu == true);
}

// Test the OGeometrie shortestSegBetween2Lines() function
TEST(test_3d_ogeometrie, shortest_seg_between_2_lines)
{
    // Points
    OPoint3D pt1(-10.0, 23.0, 0.0), pt2(10.0, -17.0, 0.0), pt3(-10.0, -32.0, 3.0), pt4(10.0, 28.0, 3.0), ptA, ptB;

    // Declare mua et mub
    double* mua = new double;
    double* mub = new double;

    // Segments do not cross
    bool resu = OGeometrie::shortestSegBetween2Lines(pt1, pt2, pt3, pt4, ptA, ptB, mua, mub);
    EXPECT_TRUE(resu == true);
    EXPECT_TRUE(ptA == OPoint3D(1.0, 1.0, 0.0));
    EXPECT_TRUE(ptB == OPoint3D(1.0, 1.0, 3.0));

    // Segments cross
    pt3._z = pt4._z = 0.0;
    resu = OGeometrie::shortestSegBetween2Lines(pt1, pt2, pt3, pt4, ptA, ptB, mua, mub);
    EXPECT_TRUE(resu == true);
    EXPECT_TRUE(ptA == OPoint3D(1.0, 1.0, 0.0));
    EXPECT_TRUE(ptB == OPoint3D(1.0, 1.0, 0.0));
}


// Test the OGeometrie boundingBox() function
TEST(test_3d_ogeometrie, bounding_box)
{
    // Array of points
    OPoint3D pts[6];
    pts[0] = OPoint3D(1.0, 1.0, 5.0);
    pts[1] = OPoint3D(5.0, 6.0, 0.0);
    pts[2] = OPoint3D(-1.0, 10.0, 0.0);
    pts[3] = OPoint3D(-10.0, -2.0, 0.0);
    pts[4] = OPoint3D(1.0, -8.0, 0.0);
    pts[5] = OPoint3D(6.0, -3.0, 0.0);

    // Declaration of ptMin and ptMax (used to return the bounds of the bounding box)
    OPoint3D ptMin, ptMax;

    // Compute bounds
    OGeometrie::boundingBox(pts, 6, ptMin, ptMax);

    EXPECT_TRUE(ptMin == OPoint3D(-10.0, -8.0, 0.0));
    EXPECT_TRUE(ptMax == OPoint3D(6.0, 10.0, 5.0));
}

// Test the OGeometrie computeNormal() function
TEST(test_3d_ogeometrie, compute_normal)
{
    // Array of points
    OPoint3D pts[3];
    pts[0] = OPoint3D(0.0, 0.0, 0.0);
    pts[1] = OPoint3D(1.0, 0.0, 0.0);
    pts[2] = OPoint3D(0.0, 1.0, 0.0);

    // Result vector
    OVector3D normale;

    // Compute normal
    OGeometrie::computeNormal(pts, 3, normale);

	EXPECT_TRUE(normale == OVector3D(0,0,1));
}

/***********************************************************************
						        OSegment3D
************************************************************************/


// Test the equal operator
TEST(test_3d_osegment3d,equal_operator){
	
	EXPECT_TRUE(OSegment3D(OPoint3D(1,-5,7),OPoint3D(4,1,-6))==OSegment3D(OPoint3D(1,-5,7),OPoint3D(4,1,-6)));
	EXPECT_FALSE(OSegment3D(OPoint3D(1,-5,7),OPoint3D(4,1,-6))==OSegment3D(OPoint3D(2,-5,7),OPoint3D(4,1,-6)));
}

// Test the not equal operator
TEST(test_3d_osegment3d,not_equal_operator){
	
	EXPECT_FALSE(OSegment3D(OPoint3D(4,8,-3),OPoint3D(-12,5,7))!=OSegment3D(OPoint3D(4,8,-3),OPoint3D(-12,5,7)));
	EXPECT_TRUE(OSegment3D(OPoint3D(4,8,-3),OPoint3D(-12,5,7))!=OSegment3D(OPoint3D(7,8,1),OPoint3D(-8,-5,3)));
}


// Test the product operator
TEST(test_3d_osegment3d,product_operator){

	double m[4][4]={{7,-5,9,6},{-1,6,-3,9},{0,-4,-2,-3},{6,8,-8,-1}};
	OMatrix mat=OMatrix(m);

	OPoint3D A=OPoint3D(5,-9,-1);
	OPoint3D B=OPoint3D(-7,2,-3);
	OSegment3D segment=OSegment3D(A,B);

	OSegment3D res=segment*mat;

	EXPECT_TRUE(OSegment3D(OPoint3D(77,-47,35),OPoint3D(-80,37,-5))==res);
}

// Test the longueur function
TEST(test_3d_osegment3d,longueur){
	
	OPoint3D A=OPoint3D(3,-5,7);
	OPoint3D B=OPoint3D(5,-4,5);
	OSegment3D segment=OSegment3D(A,B);
	
	EXPECT_TRUE(3==segment.longueur());

	A=OPoint3D(11,-9,-2);
	B=OPoint3D(7,-9,1);
	segment=OSegment3D(A,B);
	
	EXPECT_TRUE(5==segment.longueur());
}


// Test the lengthOfCurvedPath function
TEST(test_3d_osegment3d,length_of_curved_path){
	
	OPoint3D A=OPoint3D(3,-5,7);
	OPoint3D B=OPoint3D(5,-4,5);
	double R=17.32;
	OSegment3D segment=OSegment3D(A,B);
	
	EXPECT_FLOAT_EQ(2*R*asin(3/(2*R)),segment.lengthOfCurvedPath(R));

	R=8.24;
	A=OPoint3D(11,-9,-2);
	B=OPoint3D(7,-9,1);
	segment=OSegment3D(A,B);
	
	EXPECT_FLOAT_EQ(2*R*asin(5/(2*R)),segment.lengthOfCurvedPath(R));
}

// Test the symetrieOf function
TEST(test_3d_osegment3d,symetrie_of){
	
	OPoint3D A=OPoint3D(-10,0,0);
	OPoint3D B=OPoint3D(10,0,0);
	OSegment3D segment=OSegment3D(A,B);

	OPoint3D pt=OPoint3D(0,5,0);
	OPoint3D sym;

	EXPECT_TRUE(1==segment.symetrieOf(pt,sym)); //segments A-B and pt-sym cross
	EXPECT_TRUE(OPoint3D(0,-5,0)==sym);


	A=OPoint3D(-5,5,0);
	B=OPoint3D(5,-5,0);
	segment=OSegment3D(A,B);

	pt=OPoint3D(-2,-2,1);

	EXPECT_TRUE(1==segment.symetrieOf(pt,sym)); //segments A-B and pt-sym cross
	EXPECT_TRUE(OPoint3D(2,2,-1)==sym);

	pt=OPoint3D(-10,10,0);
	EXPECT_TRUE(0==segment.symetrieOf(pt,sym)); //segments A-B and pt-sym DO NOT cross
	EXPECT_TRUE(OPoint3D(-10,10,0)==sym);
}

// Test the projection function
TEST(test_3d_osegment3d,projection){

	OPoint3D A=OPoint3D(-5,3,0);
	OPoint3D B=OPoint3D(5,3,0);
	OSegment3D segment=OSegment3D(A,B);

	OPoint3D ptProj;

	OPoint3D pt=OPoint3D(0,0,0);
	segment.projection(pt,ptProj,0.1);
	EXPECT_TRUE(OPoint3D(0,3,0)==ptProj);

	pt=OPoint3D(2,0,0);
	segment.projection(pt,ptProj,0.1);
	EXPECT_TRUE(OPoint3D(2,3,0)==ptProj);

	pt=OPoint3D(10,0,0);
	int res=segment.projection(pt,ptProj,0.1); //no orthogonal projection
	EXPECT_TRUE(res==0);
	

	A=OPoint3D(2,3,0);
	B=OPoint3D(-1,-3,0);
	segment=OSegment3D(A,B);

	pt=OPoint3D(-1,2,0);
	segment.projection(pt,ptProj,0.1);
	EXPECT_TRUE(OPoint3D(1,1,0)==ptProj);
}

// Test the intersects function
TEST(test_3d_osegment3d,intersects){

	OPoint3D A1=OPoint3D(-5,3,0);
	OPoint3D A2=OPoint3D(5,3,0);
	OSegment3D segmentA=OSegment3D(A1,A2);

	OPoint3D B1=OPoint3D(1,-4,0);
	OPoint3D B2=OPoint3D(2,-4,0);
	OSegment3D segmentB=OSegment3D(B1,B2);

	OPoint3D pt;
	EXPECT_TRUE(INTERS_NULLE==(segmentA.intersects(segmentB,pt,0.1))); //segments are paralell

	A1=OPoint3D(1,3,0);
	A2=OPoint3D(1,-3,0);
	segmentA=OSegment3D(A1,A2);

	B1=OPoint3D(1,0,5);
	B2=OPoint3D(1,0,-5);
	segmentB=OSegment3D(B1,B2);

	EXPECT_TRUE(INTERS_OUI==(segmentA.intersects(segmentB,pt,0.1))); //segments cross
	EXPECT_TRUE(OPoint3D(1,0,0)==pt); //segments cross at (1,0,0)


	A1=OPoint3D(1,3,0);
	A2=OPoint3D(1,1,0);
	segmentA=OSegment3D(A1,A2);

	B1=OPoint3D(1,0,5);
	B2=OPoint3D(1,0,1);
	segmentB=OSegment3D(B1,B2);

	EXPECT_TRUE(INTERS_NULLE==(segmentA.intersects(segmentB,pt,0.1))); //segments are too short to cross
}

// Test the centreOf function
TEST(test_3d_osegment3d,centre_of){

	OPoint3D A=OPoint3D(-1,0,0);
	OPoint3D B=OPoint3D(1,0,0);
	OSegment3D seg=OSegment3D(A,B);

	EXPECT_TRUE(OPoint3D(0,0,0)==seg.centreOf());


	A=OPoint3D(18,-7,21);
	B=OPoint3D(10,-13,-5);
	seg=OSegment3D(A,B);

	EXPECT_TRUE(OPoint3D(14,-10,8)==seg.centreOf());
}

// Test the centerOfCurvedPath function
TEST(test_3d_osegment3d,center_of_curved_path){

	OPoint3D A=OPoint3D(-8,0,0);
	OPoint3D B=OPoint3D(3,0,0);
	OSegment3D seg=OSegment3D(A,B);
	OPoint3D res=seg.centerOfCurvedPath(3);
	EXPECT_TRUE(OPoint3D(-2.5,0,0)==seg.centerOfCurvedPath(3)); //R < half length of AB => take the middle of AB

	A=OPoint3D(-4,0,0);
	B=OPoint3D(4,0,0);
	seg=OSegment3D(A,B);
	res=seg.centerOfCurvedPath(5);
	EXPECT_TRUE(OPoint3D(0,0,3)==seg.centerOfCurvedPath(5)); // Non regression test. Unclear why (0,0,3) is the right answer


}


// Test the swap function
TEST(test_3d_osegment3d,swap){
	
	OPoint3D A=OPoint3D(4,-5,7);
	OPoint3D B=OPoint3D(-5,12,-2);

	OSegment3D segment=OSegment3D(A,B);

	EXPECT_TRUE(OSegment3D(B,A)==segment.swap());
	EXPECT_TRUE(segment==segment.swap().swap());
}


/***********************************************************************
						        OHPlane3D
************************************************************************/


TEST(test_3d_ohplane3d,intersects){
	
	OVector3D normal=OVector3D(1,-2,3);
	OVector3D origin=OPoint3D(5,4,-1);

	OHPlane3D plane=OHPlane3D(normal,origin);

	OPoint3D inter;
	double t;

	int res=plane.intersects(OPoint3D(3,5,-1),OPoint3D(-4,3,0),inter,t);
	EXPECT_EQ(0,res); // segment does not lie in plane and is parallele to it

	res=plane.intersects(OPoint3D(5,4,-1),OPoint3D(-2,2,0),inter,t);
	EXPECT_EQ(2,res); // segment lies in plane and is parallele to it

	res=plane.intersects(OPoint3D(6,2,2),OPoint3D(1,12,-13),inter,t); // segment from origin+normal to origin-4*normal
	EXPECT_EQ(1,res); // segment not parallel to plane and interesects it
	EXPECT_TRUE(inter==origin); // segment crosses the plane at its origin
	EXPECT_TRUE(t==0.2); // the interesection point is at 1/5 of the segment

	res=plane.intersects(OPoint3D(10,4,-1),OPoint3D(4,-8,12),inter,t);
	EXPECT_EQ(0,res); // segment not parallel to plane and does not intersect it
}

/***********************************************************************
						        OBox
************************************************************************/

TEST(test_3d_obox,test_constructors){

	//default constructor
	OBox box;
	EXPECT_EQ(OPoint3D(0,0,0),box._min);
	EXPECT_EQ(OPoint3D(0,0,0),box._max);

	//min-max constructor
	box=OBox(OCoord3D(-1,-2,-3),OCoord3D(1,2,3));
	EXPECT_EQ(OPoint3D(-1,-2,-3),box._min);
	EXPECT_EQ(OPoint3D(1,2,3),box._max);

	//double constructor
	OBox box2=OBox(-1,-2,-3,1,2,3);
	EXPECT_EQ(OPoint3D(-1,-2,-3),box2._min);
	EXPECT_EQ(OPoint3D(1,2,3),box2._max);
	EXPECT_EQ(box,box2);

	//copy constructor
	box2=OBox(box);
	EXPECT_EQ(OPoint3D(-1,-2,-3),box2._min);
	EXPECT_EQ(OPoint3D(1,2,3),box2._max);
	EXPECT_EQ(box,box2);
}

TEST(test_3d_obox,test_is_inside)
{
    // Création des points min et max
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);

    // Création de la box
    OBox box(pt1, pt2);

    // Création du point à tester
    OPoint3D pt(-1.0, -3.0, -2.0);

    // Test positif
    bool resu = box.isInside(pt);
    EXPECT_TRUE(resu == true);

    // Test négatif
    pt._z = 3.0;
    resu = box.isInside(pt);
    EXPECT_FALSE(resu == true);
}


TEST(test_3d_obox,test_is_inside_2d)
{
    // Création des points min et max
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);

    // Création de la box
    OBox box(pt1, pt2);

    // Création du point à tester
    OPoint3D pt(-1.0, -3.0, -2.0);

    // Test positif (1) : Point is inside 2D and 3D box
    bool resu = box.isInside2D(pt);
    EXPECT_TRUE(resu == true);

    // Test positif (2) : Point is inside 2D but not 3D box
    pt._z = 3.0;
    resu = box.isInside2D(pt);
    EXPECT_TRUE(resu == true);

    // Test négatif : Point is outside 2D box
    pt._x = -5;
    resu = box.isInside2D(pt);
    EXPECT_FALSE(resu == true);
}


TEST(test_3d_obox,test_is_in_contact)
{
    // 1st box creation
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);
    OBox box1(pt1, pt2);

    // 2nd box creation
    OCoord3D pt3(4.0, 1.0, 0.0);
    OCoord3D pt4(7.0, 3.0, 2.0);
    OBox box2(pt3, pt4);

    // Test positif (1) : Some point of box2 are inside box1
    bool resu = box1.isInContact(box2);
    EXPECT_TRUE(resu == true);

    // Test positif (2) : box1 & box2 have a face in contact
    box2._min = OPoint3D(5.0, 0.0, -1.0);
    box2._max = OPoint3D(8.0, 5.0, 3.0);
    resu = box1.isInContact(box2);
    EXPECT_TRUE(resu == true);

    // Test négatif : box1 do not intersects with box2
    box2._min._x = 7.0;
    resu = box1.isInContact(box2);
    EXPECT_FALSE(resu == true);
}


TEST(test_3d_obox, test_enlarge)
{
    // Création d'une boite vide
    OBox box;

    // Ajout des points
    box.Enlarge(OPoint3D(1.0, 2.0, -5.0));
    box.Enlarge(OPoint3D(5.0, -3.0, 1.0));

    // Test de la taille de la boite
    EXPECT_TRUE(box._min == OPoint3D(1.0, -3.0, -5.0));
    EXPECT_TRUE(box._max == OPoint3D(5.0, 2.0, 1.0));

    // Test positif : ajout d'un point qui étend la boite
    box.Enlarge(OPoint3D(-3.0, 1.0, 0.0));
    EXPECT_TRUE(box._min == OPoint3D(-3.0, -3.0, -5.0));

    // Test négatif : Ajout d'un point contenu dans la boite
    box.Enlarge(OPoint3D(-1.0, 1.0, -2.0));
    EXPECT_TRUE(box._min == OPoint3D(-3.0, -3.0, -5.0));
}


TEST(test_3d_obox, test_enlarge_box)
{
    // 1st box creation
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);
    OBox box1(pt1, pt2);

    // 2nd box creation
    OCoord3D pt3(4.0, 1.0, 0.0);
    OCoord3D pt4(7.0, 3.0, 2.0);
    OBox box2(pt3, pt4);

    // Redimensionnement de la box 1 avec la box 2
    box1.Enlarge(box2);

    // Test de la taille de la boite résultante
    EXPECT_TRUE(box1._min == OPoint3D(-2.0, -4.0, -3.0));
    EXPECT_TRUE(box1._max == OPoint3D(7.0, 3.0, 2.0));
}


TEST(test_3d_obox,test_translate)
{
    // Création de la box initiale
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);
    OBox box1(pt1, pt2);

    // Vecteur de translation
    OVector3D vec(5.0, 3.0, -2.0);

    // Action de translation
    box1.Translate(vec);

    // Vérification de nouvelles limites de la boite
    EXPECT_TRUE(box1._min == OPoint3D(3.0, -1.0, -5.0));
    EXPECT_TRUE(box1._max == OPoint3D(10.0, 5.0, -1.0));
}


/***********************************************************************
						        OBox2
************************************************************************/
TEST(test_3d_obox2,test_constructors){

	//default constructor
	OBox2 box;
	EXPECT_EQ(OPoint3D(0,0,0),box._min);
	EXPECT_EQ(OPoint3D(0,0,0),box._max);
	EXPECT_EQ(OPoint3D(0,0,0),box.BoxCoord(0));
	EXPECT_EQ(OPoint3D(0,0,0),box.BoxCoord(1));
	EXPECT_EQ(OPoint3D(0,0,0),box.BoxCoord(2));
	EXPECT_EQ(OPoint3D(0,0,0),box.BoxCoord(3));
	EXPECT_EQ(OPoint3D(0,0,0),box.BoxCoord(4));
	EXPECT_EQ(OPoint3D(0,0,0),box.BoxCoord(5));
	EXPECT_EQ(OPoint3D(0,0,0),box.BoxCoord(6));
	EXPECT_EQ(OPoint3D(0,0,0),box.BoxCoord(7));
	EXPECT_EQ(OPoint3D(0,0,0),box.BoxCoord(8));
	EXPECT_EQ(OPoint3D(0,0,0),box._center);
	EXPECT_DOUBLE_EQ(0.,box._length);
	EXPECT_DOUBLE_EQ(0.,box._width);
	EXPECT_DOUBLE_EQ(0.,box._height);

	//length-width-height constructor
	box=OBox2(4,8,12);
	EXPECT_EQ(OPoint3D(-2,-4,-6),box._min);
	EXPECT_EQ(OPoint3D(2,4,6),box._max);
	EXPECT_EQ(OPoint3D(0,0,0),box.BoxCoord(0));
	EXPECT_EQ(OPoint3D(-2,-4,-6),box.BoxCoord(1));
	EXPECT_EQ(OPoint3D(-2,4,-6),box.BoxCoord(2));
	EXPECT_EQ(OPoint3D(2,4,-6),box.BoxCoord(3));
	EXPECT_EQ(OPoint3D(2,-4,-6),box.BoxCoord(4));
	EXPECT_EQ(OPoint3D(2,-4,6),box.BoxCoord(5));
	EXPECT_EQ(OPoint3D(-2,-4,6),box.BoxCoord(6));
	EXPECT_EQ(OPoint3D(-2,4,6),box.BoxCoord(7));
	EXPECT_EQ(OPoint3D(2,4,6),box.BoxCoord(8));
	EXPECT_EQ(OPoint3D(0,0,0),box._center);
	EXPECT_DOUBLE_EQ(4.,box._length);
	EXPECT_DOUBLE_EQ(8.,box._width);
	EXPECT_DOUBLE_EQ(12.,box._height);

	//from OBox constructor
	box=OBox2(OBox(OCoord3D(-1,-2,-3),OCoord3D(1,2,3)));
	EXPECT_EQ(OPoint3D(0,0,0),box.BoxCoord(0));
	EXPECT_EQ(OPoint3D(-1,-2,-3),box.BoxCoord(1));
	EXPECT_EQ(OPoint3D(-1,2,-3),box.BoxCoord(2));
	EXPECT_EQ(OPoint3D(1,2,-3),box.BoxCoord(3));
	EXPECT_EQ(OPoint3D(1,-2,-3),box.BoxCoord(4));
	EXPECT_EQ(OPoint3D(1,-2,3),box.BoxCoord(5));
	EXPECT_EQ(OPoint3D(-1,-2,3),box.BoxCoord(6));
	EXPECT_EQ(OPoint3D(-1,2,3),box.BoxCoord(7));
	EXPECT_EQ(OPoint3D(1,2,3),box.BoxCoord(8));
	EXPECT_EQ(OPoint3D(0,0,0),box._center);
	EXPECT_DOUBLE_EQ(2.,box._length);
	EXPECT_DOUBLE_EQ(4.,box._width);
	EXPECT_DOUBLE_EQ(6.,box._height);

	//copy constructor
	OBox box2=OBox2(box);
	EXPECT_EQ(OPoint3D(0,0,0),box.BoxCoord(0));
	EXPECT_EQ(OPoint3D(-1,-2,-3),box.BoxCoord(1));
	EXPECT_EQ(OPoint3D(-1,2,-3),box.BoxCoord(2));
	EXPECT_EQ(OPoint3D(1,2,-3),box.BoxCoord(3));
	EXPECT_EQ(OPoint3D(1,-2,-3),box.BoxCoord(4));
	EXPECT_EQ(OPoint3D(1,-2,3),box.BoxCoord(5));
	EXPECT_EQ(OPoint3D(-1,-2,3),box.BoxCoord(6));
	EXPECT_EQ(OPoint3D(-1,2,3),box.BoxCoord(7));
	EXPECT_EQ(OPoint3D(1,2,3),box.BoxCoord(8));
	EXPECT_EQ(OPoint3D(0,0,0),box._center);
	EXPECT_DOUBLE_EQ(2.,box._length);
	EXPECT_DOUBLE_EQ(4.,box._width);
	EXPECT_DOUBLE_EQ(6.,box._height);
	EXPECT_EQ(box,box2);
}

TEST(test_3d_obox2, test_is_inside)
{
    // Cration des points min et max
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);

    // Cration de la box
    ORepere3D rep;
    OBox box(pt1, pt2);
    OBox2 box2(box);

    // Cration du point  tester
    OPoint3D pt(-1.0, -3.0, -2.0);

    // Test positif
    bool resu = box2.isInside(pt);
    EXPECT_TRUE(resu);

    // Test ngatif
    pt._z = 3.0;
    resu = box2.isInside(pt);
    EXPECT_FALSE(resu);
}

TEST(test_3d_obox2, test_is_inside_2d)
{
    // Cration des points min et max
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);

    // Cration de la box
    ORepere3D rep;
    OBox box(pt1, pt2);
    OBox2 box2(box);

    // Cration du point  tester
    OPoint3D pt(-1.0, -3.0, -2.0);

    // Test positif (1) : Point is inside 2D and 3D box
    bool resu = box2.isInside2D(pt);
    EXPECT_TRUE(resu);

    // Test positif (2) : Point is inside 2D but not 3D box
    pt._z = 3.0;
    resu = box2.isInside2D(pt);
    EXPECT_TRUE(resu);

    // Test ngatif : Point is outside 2D box
    pt._x = -5;
    resu = box2.isInside2D(pt);
    EXPECT_FALSE(resu);
}

TEST(test_3d_obox2,test_translate)
{
    // Cration des points min et max
    OCoord3D pt1(-2.0, -4.0, -3.0);
    OCoord3D pt2(5.0, 2.0, 1.0);

    // Cration de la box
    ORepere3D rep;
    OBox box(pt1, pt2);
    OBox2 box2(box);

    // Cration du point servant  translater
    OVector3D vt(-1.0, -3.0, -2.0);

    // Test positif (1) : Point is inside 2D and 3D box
    box2.Translate(vt);

    EXPECT_EQ( box2._A, OPoint3D(-3.0, -7.0, -5.0) );
    EXPECT_EQ( box2._center, OPoint3D( 0.5, -4.0, -3.0 ) );
    EXPECT_EQ( box2._H, OPoint3D( 4.0, -1.0, -1.0 ) );
}

TEST(test_3d_obox2, test_box_rotation_oz_oy)
{
    //box centered on the origin
    OCoord3D pt1(-1.0, -1.0, -1.0);
    OCoord3D pt2(1.0, 1.0, 1.0);

    OBox box(pt1, pt2);
    OBox2 box2(box);


	box2.BoxRotationOzOy(3.1415926536/4,3.1415926536/2);

    EXPECT_EQ( box2._A, OPoint3D(1.4142135624, 0,-1) );
    EXPECT_EQ( box2._center, OPoint3D( 0, 0, 0 ) );
    EXPECT_EQ( box2._H, OPoint3D( -1.4142135624, 0, 1 ) );


	//box NOT centered on the origin
	pt1=OCoord3D(0, 0, 0);
    pt2=OCoord3D(2.0, 2.0, 2.0);

    box=OBox(pt1, pt2);
    box2=OBox2(box);


	box2.BoxRotationOzOy(3.1415926536/4,3.1415926536/2);

    EXPECT_EQ( box2._A, OPoint3D(2.4142135324, 1, 0) );
    EXPECT_EQ( box2._center, OPoint3D( 1, 1, 1) );
    EXPECT_EQ( box2._H, OPoint3D( -.4142135624, 1, 2) );

}

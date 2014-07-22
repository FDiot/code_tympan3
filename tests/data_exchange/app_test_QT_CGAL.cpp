/**
 * @file app_test_QT_CGAL.cpp
 * @brief This is a Qt graphical app for testing Altimetry Builder
 *
 *  Created on: 10 déc. 2012
 *      Author: Anthony Truchet <anthony.truchet@logilab.fr>
 */



#include <iostream>
#include <deque>

#define TY_USE_CGAL_QT_IFACE 1 // To enable Qt visualization helpers
#include "Tympan/models/business/AltimetryBuilder.h"

using namespace tympan;


TYPoint ini_points[] =
{
    TYPoint(0,  0, 0.0),
    TYPoint(0, 10, 0.0),
    TYPoint(10, 10, 0.0),
    TYPoint(10,  0, 0.0),
    TYPoint(1,  2, 1.5),
    TYPoint(3,  8, 1.5),
    TYPoint(8,  1, 1.5),
    TYPoint(5,  3, 2.5),
    TYPoint(3,  4, 2.0),
    TYPoint(5,  4, 2.0),
    TYPoint(2,  1, unspecified_altitude),
    TYPoint(7,  8, unspecified_altitude),
    TYPoint(6,  1, unspecified_altitude)
};

TYTabPoint contour(ini_points + 0, ini_points + 4);
TYTabPoint curveB(ini_points + 4, ini_points + 7);
TYTabPoint curveC(ini_points + 7, ini_points + 10);
TYTabPoint curveC1(ini_points + 7, ini_points + 8);
TYTabPoint curveC2(ini_points + 8, ini_points + 10);
TYTabPoint grass1(ini_points + 10, ini_points + 13);


int main(int argc, char** argv)
{
    QApplication app(argc, argv, true);

    material_t grass_mat(new TYSol());
    grass_mat->setName("grass");
    material_t def_mat(new TYSol());
    def_mat->setName("default");

    MaterialPolygon* grass_poly = new MaterialPolygon(grass1 |
                                                      transformed(boost::bind(to_cgal2, _1)),
                                                      grass_mat);
    MaterialPolygon* contour_poly = new MaterialPolygon(contour |
                                                        transformed(boost::bind(to_cgal2, _1)),
                                                        def_mat);

    AltimetryBuilder builder;
    builder.addAsConstraint(contour |
                            transformed(boost::bind(to_cgal2_info, _1)),
                            true);
    builder.addAsConstraint(curveB |
                            transformed(boost::bind(to_cgal2_info, _1)),
                            true);
    //  builder.addAsConstraint(curveC |
    //          transformed(boost::bind(to_cgal2_info, _1)),
    //          true);
    builder.addAsConstraint(curveC1 |
                            transformed(boost::bind(to_cgal2_info, _1)),
                            true);
    builder.addAsConstraint(curveC2 |
                            transformed(boost::bind(to_cgal2_info, _1)),
                            true);

    builder.material_polygons.push_back(contour_poly); contour_poly = NULL;
    builder.material_polygons.push_back(grass_poly); grass_poly  = NULL;
    builder.insertMaterialPolygonsInTriangulation();

    builder.indexFacesMaterial();
    builder.labelFaces(def_mat);

    std::cout << builder.cdt;

    std::pair<QGraphicsView*, QGraphicsScene*> vs =
        builder.buildView(-1, -1, 11, 11);
    QGraphicsView* view = vs.first;
    QGraphicsScene* scene = vs.second;
    builder.addVerticesInfo(scene);
    builder.addFacesInfo(scene);
    view->show();
    return app.exec();
}

/*
 * Copyright (C) <2012> <EDF-R&D> <FRANCE>
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include <GL/glew.h>
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/BBox.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Repere.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Shape.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/Material.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Ray/Ray.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Engine/Simulation.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/Sphere.h"
#include "Tympan/MetierSolver/AcousticRaytracer/Geometry/UniformSphericSampler.h"
#include <time.h>
#include <Mainwindow.h>
#include "Tympan/MetierSolver/AcousticRaytracer/Acoustic/NMPBSolver.h"
#include "ImportManager.h"

#include <iostream>

void declareTestScene(Simulation* simulation, Scene* scene, Material* m1, Material* m2)
{

    scene->addBuilding(vec3(220., -40., 0), vec3(10., 10., 20.), m1);
    scene->addBuilding(vec3(220., -20., 0), vec3(10., 10., 20.), m1);
    scene->addBuilding(vec3(220., 0., 0), vec3(10., 10., 20.), m1);
    scene->addBuilding(vec3(220., 20., 0), vec3(10., 10., 20.), m1);
    scene->addBuilding(vec3(220., 40., 0), vec3(10., 10., 20.), m1);

    unsigned int p1, p2, p3, p4;

    scene->addVertex(vec3(-50., -50., 30.), p1);
    scene->addVertex(vec3(50., -50., 30.), p2);
    scene->addVertex(vec3(50., -10., 30.), p3);
    scene->addVertex(vec3(-50., -10., 30.), p4);
    scene->addTriangle(p1, p2, p3, m2);
    scene->addTriangle(p3, p4, p1, m2);

    scene->addVertex(vec3(-50., -10., 30.), p1);
    scene->addVertex(vec3(50., -10., 30.), p2);
    scene->addVertex(vec3(50., 10., 30.), p3);
    scene->addVertex(vec3(-50., 10., 30.), p4);
    scene->addTriangle(p1, p2, p3, m2);
    scene->addTriangle(p3, p4, p1, m2);

    scene->addVertex(vec3(-50., 10., 30.), p1);
    scene->addVertex(vec3(50., 10., 30.), p2);
    scene->addVertex(vec3(50., 50., 30.), p3);
    scene->addVertex(vec3(-50., 50., 30.), p4);
    scene->addTriangle(p1, p2, p3, m2);
    scene->addTriangle(p3, p4, p1, m2);

    scene->addVertex(vec3(100., -50., 0.), p1);
    scene->addVertex(vec3(180., -50., 0.), p2);
    scene->addVertex(vec3(180., -30., 0.), p3);
    scene->addVertex(vec3(100., -30., 0.), p4);
    scene->addTriangle(p1, p2, p3, m2);
    scene->addTriangle(p3, p4, p1, m2);

    scene->addVertex(vec3(100., 30., 0.), p1);
    scene->addVertex(vec3(180., 30., 0.), p2);
    scene->addVertex(vec3(180., 50., 0.), p3);
    scene->addVertex(vec3(100., 50., 0.), p4);
    scene->addTriangle(p1, p2, p3, m2);
    scene->addTriangle(p3, p4, p1, m2);

    scene->addVertex(vec3(180., -50., 0.), p1);
    scene->addVertex(vec3(250., -50., 0.), p2);
    scene->addVertex(vec3(250., 50., 0.), p3);
    scene->addVertex(vec3(180., 50., 0.), p4);
    scene->addTriangle(p1, p2, p3, m2);
    scene->addTriangle(p3, p4, p1, m2);

    scene->addVertex(vec3(50., -10., 30.), p1);
    scene->addVertex(vec3(150., -10., 30.), p2);
    scene->addVertex(vec3(150., 10., 30.), p3);
    scene->addVertex(vec3(50., 10., 30.), p4);
    scene->addTriangle(p1, p2, p3, m2);
    scene->addTriangle(p3, p4, p1, m2);

    scene->addVertex(vec3(50., -50., 30.), p1);
    scene->addVertex(vec3(50., -10., 30.), p2);
    scene->addVertex(vec3(100., -30., 0.), p3);
    scene->addTriangle(p3, p2, p1, m2);

    scene->addVertex(vec3(100., -30., 0.), p1);
    scene->addVertex(vec3(100., -50., 0.), p2);
    scene->addVertex(vec3(50., -50., 30.), p3);
    scene->addTriangle(p3, p2, p1, m2);

    scene->addVertex(vec3(150., -10., 30.), p1);
    scene->addVertex(vec3(100., -30., 0.), p2);
    scene->addVertex(vec3(50., -10., 30.), p3);
    scene->addTriangle(p3, p2, p1, m2);

    scene->addVertex(vec3(100., -30., 0.), p1);
    scene->addVertex(vec3(150., -10., 30.), p2);
    scene->addVertex(vec3(180., -30., 0.), p3);
    scene->addTriangle(p3, p2, p1, m2);

    scene->addVertex(vec3(180., 30., 0.), p1);
    scene->addVertex(vec3(180., -30., 0.), p2);
    scene->addVertex(vec3(150., -10., 30.), p3);
    scene->addTriangle(p3, p2, p1, m2);

    scene->addVertex(vec3(150., -10., 30.), p1);
    scene->addVertex(vec3(150., 10., 30.), p2);
    scene->addVertex(vec3(180., 30., 0.), p3);
    scene->addTriangle(p3, p2, p1, m2);

    scene->addVertex(vec3(50., 10., 30.), p1);
    scene->addVertex(vec3(50., 50., 30.), p2);
    scene->addVertex(vec3(100., 30., 0.), p3);
    scene->addTriangle(p3, p2, p1, m2);

    scene->addVertex(vec3(50., 50., 30.), p1);
    scene->addVertex(vec3(100., 50., 0.), p2);
    scene->addVertex(vec3(100., 30., 0.), p3);
    scene->addTriangle(p3, p2, p1, m2);

    scene->addVertex(vec3(50., 10., 30.), p1);
    scene->addVertex(vec3(100., 30., 0.), p2);
    scene->addVertex(vec3(180., 30., 0.), p3);
    scene->addTriangle(p3, p2, p1, m2);

    scene->addVertex(vec3(180., 30., 0.), p1);
    scene->addVertex(vec3(150., 10., 30.), p2);
    scene->addVertex(vec3(50., 10., 30.), p3);
    scene->addTriangle(p3, p2, p1, m2);

    unsigned int initialCount = 1000000;

    for (unsigned int i = 0; i < 6; i++)
    {
        for (unsigned int j = 0; j < 6; j++)
        {
            Source source;
            source.setSampler(new UniformSphericSampler());
            source.setPosition(vec3(-50.0 + i * 20. , -30.0 + j * 20., 30.05));
            source.setInitialRayCount(initialCount);
            simulation->addSource(source);

        }
    }

    for (unsigned int i = 0; i < 6; i++)
    {
        Recepteur recep;
        recep.setShape(new Sphere(vec3(50. + i * 10., 0.0, 31.0), 1.0));
        simulation->addRecepteur(recep);
    }

    for (unsigned int i = 0; i < 8; i++)
    {
        Recepteur recep1;
        recep1.setShape(new Sphere(vec3(110.0 + i * 10., -40.0, 1.0), 1.0));
        simulation->addRecepteur(recep1);

        Recepteur recep2;
        recep2.setShape(new Sphere(vec3(110.0 + i * 10., 40.0, 1.0), 1.0));
        simulation->addRecepteur(recep2);
    }

    for (unsigned int i = 0; i < 11; i++)
    {
        Recepteur recep1;
        recep1.setShape(new Sphere(vec3(190.0, -50.0 + i * 10., 1.0), 1.0));
        simulation->addRecepteur(recep1);

        Recepteur recep2;
        recep1.setShape(new Sphere(vec3(200.0, -50.0 + i * 10., 1.0), 1.0));
        simulation->addRecepteur(recep1);

        Recepteur recep3;
        recep1.setShape(new Sphere(vec3(210.0, -50.0 + i * 10., 1.0), 1.0));
        simulation->addRecepteur(recep1);
    }
}

int main(int argc, char* argv[])
{

    //srand(time(NULL));
    srand(23534658528324);
    std::cout << "Executable de test de la librairie de raytracing acoustique" << std::endl << std::endl << std::endl;

    std::cout << "//////////////////////////////////" << std::endl;
    std::cout << "/// Tests sur les formes." << std::endl;
    std::cout << "//////////////////////////////////" << std::endl;
    Shape shape1 = Shape();
    Shape shape2 = Shape("testName");

    std::cout << "Le nom de la premiere forme est " << shape1.getName() << std::endl;
    std::cout << "Le nom de la deuxieme forme est " << shape2.getName() << std::endl;

    shape1.setName(std::string("testSetName"));
    std::cout << "Le nom de la premiere forme apres modification est " << shape1.getName() << std::endl;
    std::cout << "Fin des tests sur les formes." << std::endl << std::endl << std::endl;

    std::cout << "//////////////////////////////////" << std::endl;
    std::cout << "/// Test de boundingBox " << std::endl;
    std::cout << "//////////////////////////////////" << std::endl;
    BBox b1;
    BBox b2 = BBox(vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));

    BBox b3 = BBox(vec3(0.5, 0.5, 0.5), vec3(2.0, 2.0, 2.0));
    std::cout << "Description de la bbox b1 : " << std::endl;
    b1.print();
    std::cout << "Description de la bbox b2 : " << std::endl;
    b2.print();
    std::cout << "Description de la bbox b3 : " << std::endl;
    b3.print();

    std::cout << "b4 : Union entre b1 et b2 : " << std::endl;
    BBox b4 = b1.Union(b2);
    b4.print();

    std::cout << "b5 : Union entre b4 et b3 : " << std::endl;
    BBox b5 = b4.Union(b3);
    b5.print();
    std::cout << "Fin des tests de bounding box." << std::endl << std::endl << std::endl;

    std::cout << "//////////////////////////////////" << std::endl;
    std::cout << "/// Test des Materiaux." << std::endl;
    std::cout << "//////////////////////////////////" << std::endl;
    Material* m1 = new Material();
    Material* m2 = new Material();
    m2->setName(std::string("MyMaterial"));
    m1->r = 255; m1->g = 125; m1->b = 0;
    m1->isNatural = false;
    m2->r = 0; m2->g = 255; m2->b = 0;

    MaterialManager* materialManager = new MaterialManager();
    materialManager->registerNewMaterial(m1);
    materialManager->registerNewMaterial(m2);


    materialManager->print();
    //delete materialManager;
    std::cout << "Fin du test des Materiaux." << std::endl << std::endl << std::endl;

    std::cout << "//////////////////////////////////" << std::endl;
    std::cout << "/// Test de la simulation." << std::endl;
    std::cout << "//////////////////////////////////" << std::endl;

    Recepteur recep;
    recep.setShape(new Sphere(vec3(0.0, 0.0, 1.0), 1.0));


    Source source;
    source.setSampler(new UniformSphericSampler());
    source.setPosition(vec3(10.0, 0.0, 1.0));
    source.setInitialRayCount(10000);
    vec3 newDir = vec3(-4., -4., 2) - source.getPosition();
    newDir.normalize();
    //source.addDirection(newDir);

    std::cout << "Fin de l'intialisation des sources et recepteurs." << std::endl;

    Simulation* simulation = new Simulation();
    //simulation->addSource(source);
    //simulation->addRecepteur(recep);

    /*for(unsigned int i = 0; i < 4; i++){
        Recepteur recep;
        recep.setShape(new Sphere(vec3(0.,0.,(decimal)i),1.));
        simulation->addRecepteur(recep);
    }*/

    /*for(unsigned int i = 0; i < 4; i++){
        Recepteur recep;
        recep.setShape(new Sphere(vec3(-11.,-11.,(decimal)i),1.));
        simulation->addRecepteur(recep);
    }*/

    /*for(unsigned int i = 0; i < 4; i++){
        Recepteur recep;
        recep.setShape(new Sphere(vec3(0.,-11.,(decimal)i),1.));
        simulation->addRecepteur(recep);
    }

    for(unsigned int i = 0; i < 4; i++){
        Recepteur recep;
        recep.setShape(new Sphere(vec3(-11.,0.,(decimal)i),1.));
        simulation->addRecepteur(recep);
    }*/

    simulation->setSolver(new NMPBSolver());

    std::cout << "Fin de la declaration des sources et recepteurs." << std::endl;

    //Ajout de primitives
    Scene* scene = simulation->getScene();
    //declareTestScene(simulation,scene, m1,m2);

    /*unsigned int p1,p2,p3,p4;
    scene->addVertex(vec3(-10,-10,0),p1);
    scene->addVertex(vec3(-10,10,0),p2);
    scene->addVertex(vec3(10,10,0),p3);
    scene->addVertex(vec3(10,-10,0),p4);
    scene->addTriangle(p3,p2,p1,m2);
    scene->addTriangle(p1,p4,p3,m2);

    scene->addBuilding(vec3(-9.,-9.,0),vec3(5.,5.,5.),m1);*/

    ImportManager::ImportFromISympa(*simulation, std::string("Data/config.xml"));

    std::cout << "Fin de la creation de la scene." << std::endl;

    simulation->getSolver()->postTreatmentScene(simulation->getScene(), simulation->getSources(), simulation->getRecepteurs());

    simulation->getScene()->finish();

    /*if(simulation->launchSimulation())
        std::cout<<"Simulation achevee normalement."<<simulation->getSolver()->getValidRays()->size()<<" rayons ont ete trouves."<<std::endl;
    else
        std::cout<<"La simulation ne s'est pas terminee normalement."<<std::endl;*/

    simulation->runBenchmark();

    std::cout << "Fin du test de la simulation." << std::endl;

    QApplication a(argc, argv);
    MainWindow w;
    w.setSimulation(simulation);
    w.show();

    return a.exec();
}

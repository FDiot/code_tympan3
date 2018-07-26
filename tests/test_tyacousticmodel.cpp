/**
*
* @brief Functional tests of the TYAcousticModel class  
* 
*  Created on: january 18, 2018 
*  Author: Philippe CHAPUIS <philippe.chapuis@c-s.fr> 
*
*/
#include "gtest/gtest.h"
#include "Tympan/solvers/DefaultSolver/TYAcousticModel.h"
#include "Tympan/solvers/DefaultSolver/TYSolver.h"
#include "Tympan/models/common/atmospheric_conditions.h"
#include "Tympan/models/solver/entities.hpp"
#include "Tympan/core/defines.h"
#include "Tympan/models/solver/config.h"
#include "Tympan/models/common/plan.h"
#include "Tympan/solvers/DefaultSolver/TYTrajet.h"
#include "Tympan/models/common/mathlib.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Geometry/Scene.h"
#include "Tympan/geometric_methods/AcousticRaytracer/Engine/Simulation.h"
#include <iostream>

/**
* @brief Fixture which provides an instance of TYAcousticModel
*/
class TYAcousticModelTest :
    public::testing::Test
{



public:
    bool m_isInitGlobal = false;

    //constantes globales
    double m_gamma = 1.41;
    double m_perfectGaz = 8.31;
    double m_molareMass = 0.029;
    double m_temperature = 20;
    double m_absoluteZero = 273.15;
    double m_pression = 101325.0;
    double m_hydromzetry = 50.0;
    double m_distance = 600.0;
    double m_tk, m_soundSpeed;
    OSpectre m_spectreLambda;

    //objets chemins
    TYChemin m_chemin_direct;
    TYChemin m_chemin_sol;
    TYChemin m_chemin_ecran;
    TYChemin m_chemin_relex;

    //tableau d'étapes
    TYTabEtape m_tabEtape01;

    //spectre source
    OSpectreComplex m_spectre_source;

    //tableau d'intersection
    std::deque<TYSIntersection> m_tabIntersect;

    //directivité
    OVector3D m_directivityVector;
    tympan::SourceDirectivityInterface* m_directivity;

    //points source et reception
    OPoint3D m_pointSource;
    OPoint3D m_pintReception;
    OSegment3D rayonSR;
    std::vector<tympan::AcousticSource> m_acousticSourceList;
    std::vector<tympan::AcousticReceptor> m_acousticReceptionList;

    TabPoint3D m_tabPoints;

    //méthode d'initialisation des objets globaux
    void initGlobal(bool isFlate){
        m_tk = m_temperature + m_absoluteZero;
        m_soundSpeed = sqrt((m_gamma*m_perfectGaz*m_tk)/m_molareMass);
        m_spectreLambda = OSpectre::getLambda(m_soundSpeed);

        if(isFlate){
            m_pointSource.setCoords(0,50,0);
            m_pintReception.setCoords(600,50,0);
        }else{
            m_pointSource.setCoords(0,50,20);
            m_pintReception.setCoords(600,50,20);
        }


       // m_atmoCondition(100.1,55.4,77.1);
        m_chemin_direct.setType(CHEMIN_DIRECT);
        m_chemin_sol.setType(CHEMIN_SOL);
        m_chemin_ecran.setType(CHEMIN_ECRAN);
        m_chemin_relex.setType(CHEMIN_REFLEX);

        rayonSR._ptA = m_pointSource;
        rayonSR._ptB = m_pintReception;

        m_spectre_source.setDefaultValue(1);
        m_directivityVector.setCoords(1,1,1);
        m_directivity = new tympan::SphericalSourceDirectivity();
        m_directivity->lwAdjustment(m_directivityVector,m_distance);

        tympan::AcousticSource m_acousticSource(m_pointSource,m_spectre_source,m_directivity);
        m_acousticSourceList.push_back(m_acousticSource);
        tympan::AcousticReceptor m_acousticReceptor(m_pintReception);
        m_acousticReceptionList.push_back(m_acousticReceptor);



        m_isInitGlobal = true;
    }

    //méthode cas utilisateur chemin direct
    OSpectre initDirectPathWay(){
        OSpectre _spectreToReturn;
        //remplissage du spectre avecc les valeurs attendues dans _spectreToReturn
        //TODO

        //MAJ distance/longueur
        m_tabEtape01.clear();
        m_chemin_direct.setDistance(m_distance);
        m_chemin_direct.setLongueur(m_distance);

        //MAJ de l'étape
        TYEtape etape01;
        etape01.setType(TYSOURCE);
        etape01.setPoint(m_pointSource);
        etape01._Absorption = m_spectre_source;
        m_tabEtape01.push_back(etape01);

        return _spectreToReturn;

    }

    //méthode cas utilisateur chemin sol
    OSpectre initGroundPathWay(){

        OSpectre _spectreToReturn;
        //remplissage du spectre avecc les valeurs attendues dans _spectreToReturn
        //TODO


        double rr=0.0;


        //Effacement des précedentes etapes
        m_tabEtape01.clear();

        //Etape avant la reflexion
        OPoint3D _ptReflexion(300,50,0);
        TYEtape etape01;
        etape01.setType(TYSOURCE);
        etape01.setPoint(m_pointSource);

        OSegment3D segSourceToReflexion(m_pointSource, _ptReflexion); // On part de la source vers le point de reflexion
        rr = segSourceToReflexion.longueur();

        // Absorption de la source
        etape01._Absorption = m_spectre_source;

        m_tabEtape01.push_back(etape01); // Ajout de l'etape avant reflexion

        //Etape apres la reflexion
        TYEtape etape2;
        etape2.setPoint(_ptReflexion);
        etape2.setType(TYREFLEXIONSOL);

        OSegment3D seg2 = OSegment3D(_ptReflexion, m_pintReception);// Segment Point de reflexion->Point de reception
        rr += seg2.longueur(); // Longueur parcourue sur le trajet reflechi


        tympan::AcousticGroundMaterial *mat = new tympan::AcousticGroundMaterial("sol",20000,1,500);

        // Angle estimation
        OVector3D direction(segSourceToReflexion._ptA, segSourceToReflexion._ptB);
        direction.normalize();

        double angle = (direction*-1).angle(OVector3D(segSourceToReflexion._ptB, rayonSR._ptA));
        double angle2 = (3.14 - angle)/2.0;
        etape2._Absorption = mat->get_absorption(angle2, rr);


        m_tabEtape01.push_back(etape2); //Ajout de l'étape apr?s reflexion

        m_chemin_sol.setLongueur(rr);
        m_chemin_sol.setDistance(m_distance);

        return _spectreToReturn;

    }

    //méthode cas utilisateur chemin écran
    OSpectre initBlockPathWay(){

        OSpectre _spectreToReturn;
        double _longueur = 0.0;
        double _epaisseur = 0.0;

        //objets necessaires ? la création du solver
        tympan::AcousticProblemModel newProblem;
        tympan::AcousticResultModel newResult;
        tympan::LPSolverConfiguration configuration;


        TYSolver mSolver;
        mSolver.solve(newProblem,newResult,configuration);
        TYAcousticModel *modelAcoustic = new TYAcousticModel(mSolver);
        modelAcoustic->init();

        //remplissage du spectre avecc les valeurs attendues dans _spectreToReturn
        //TODO

        //Effacement des précedentes etapes
        m_tabEtape01.clear();

        m_tabPoints.push_back( m_pointSource);
        OPoint3D ptFirst(49,50,35);
        m_tabPoints.push_back(ptFirst);
        OPoint3D ptLast(50,50,35);
        m_tabPoints.push_back(ptLast);

        //m_tabPoints.push_back(m_pintReception);

        TYEtape etape1,etape2,etape3,etape4;
        etape1.setType(TYSOURCE);
        etape1.setPoint(m_pointSource);
        etape1._Absorption = m_spectre_source;
        m_tabEtape01.push_back(etape1);


        OSegment3D segCourant(rayonSR._ptA, ptFirst);
        _longueur = segCourant.longueur();

        OSegment3D pente(OPoint3D(0,50,0),OPoint3D(100,50,0));

        etape2.setPoint(ptFirst);
        etape2._type = TYDIFFRACTION;
        etape2._Absorption = OSpectreComplex(TYComplex(1.0, 0.0));//absorption nulle

        _epaisseur += (OSegment3D(ptFirst, ptLast)).longueur();
        _longueur += _epaisseur;

        m_tabEtape01.push_back(etape2);

        /*--- APRES L'OBSTACLE ---*/
        etape3.setPoint(ptLast);
        etape3._type = TYDIFFRACTION;
        etape3._Absorption = OSpectreComplex(TYComplex(1.0, 0.0));;//absorption nulle


        m_tabEtape01.push_back(etape3);
        _longueur +=OSegment3D(ptLast, rayonSR._ptB).longueur();

        etape4._pt = rayonSR._ptB;
        etape4._Absorption = OSpectreComplex(TYComplex(1.0, 0.0));;//absorption nulle;
        bool diff= true;
        etape4._Attenuation = modelAcoustic->calculAttDiffraction(rayonSR, pente, false, _longueur, _epaisseur, true, false, diff);
        m_tabEtape01.push_back(etape4);


        m_chemin_ecran.setDistance(m_distance);

        m_chemin_ecran.setLongueur(_longueur);

        return _spectreToReturn;

    }

    //méthode cas utilisateur chemin reflexion
    OSpectre initReflexionPathWay(){
        OSpectre _spectreToReturn;
        OPoint3D _ptRecep(0,50,60);
        OPoint3D _ptReflex(300,50,40);


        OPoint3D ptS11(300, 50, 0);
        OPoint3D ptS12(300, 50, 100);
        OPoint3D ptS21(0, 50, 40);
        OPoint3D ptS22(500, 50, 40);

        OSegment3D segV(ptS11, ptS12);
        OSegment3D segH(ptS21, ptS22);

        TYSIntersection m_Intersection01;//,m_Intersection02;
        m_Intersection01.isEcran = true;
        m_Intersection01.isInfra = true;
        m_Intersection01.segInter[0] = segV;
        m_Intersection01.segInter[1] = segH;
        m_Intersection01.bIntersect[1] = true;
        tympan::AcousticBuildingMaterial *mat = new tympan::AcousticBuildingMaterial("Batiment",OSpectreComplex(TYComplex(1.0, 0.0)));
        m_Intersection01.material = mat;

        m_tabIntersect.push_back(m_Intersection01);

        //MAJ distance/longueur
        m_tabEtape01.clear();
        m_chemin_relex.setDistance(m_distance);


        TYEtape etape1,etape2;
        etape1._pt = m_pointSource;
        etape1._type = TYSOURCE;
        etape1._Absorption = m_spectre_source;

        m_tabEtape01.push_back(etape1);

        // Deuxieme etape : du point de reflexion a la fin du rayon
        OSegment3D segSourceToReflexion(m_pointSource, _ptReflex); // On part de la source vers le point de reflexion
        double rr = segSourceToReflexion.longueur();

        OSpectreComplex spectComp = mat->asEyring();

        spectComp = spectComp.mult(-1.0).sum(1.0);
        etape2._pt = _ptReflex;
        etape2._type = TYREFLEXION;
        etape2._Absorption = spectComp;

        m_tabEtape01.push_back(etape2);

        OSegment3D segReflexToRecep(_ptReflex, _ptRecep);
        rr+=segReflexToRecep.longueur();

        m_chemin_relex.setLongueur(rr);

        return _spectreToReturn;

    }
};

// Testing the TYAcousticModel::limAttDiffraction method
TEST(test_TYAcousticModel, limAttDiffraction)
{
    // Data setup:
    double dB;

    OSpectre m_Source;
    OSpectre m_Calcul;

    TYSolver m_Solver;
    TYAcousticModel m_AcousticModel(m_Solver);

    // Cas 1 : Spectre Source < 1 dB (LIMITATION)
    m_Source.setDefaultValue(0.5);        // 0.5 dB pour toutes les frÃ©quences
    m_Calcul.setDefaultValue(0.008);      // 0.008

    OSpectre m_Result = m_AcousticModel.limAttDiffraction(m_Source, m_Calcul);

    int i;
    for(i=0; i<31; i++)
    {
        EXPECT_EQ(1 ,m_Result.getTabValReel()[i]);
    }

    // Cas 2:  ECRAN MINCE (<0.01 m) et puissance en dessous de 20 dB
    // ecran Ã  0.008 m et puissance sonore Ã  5 dB
    dB = pow(10.0, (10.0/10.0));
    m_Source.setDefaultValue(dB);

    m_Result = m_AcousticModel.limAttDiffraction(m_Source, m_Calcul);

    for(i=0; i<31; i++)
    {
        EXPECT_EQ(10.0 ,m_Result.getTabValReel()[i]);
    }


    // Cas 3: ECRAN MINCE et puissance au dessus de 20 dB
    dB = pow(10.0, (26.0/10.0));
    m_Source.setDefaultValue(dB);

    m_Result = m_AcousticModel.limAttDiffraction(m_Source, m_Calcul);

    for(i=0; i<31; i++)
    {
        EXPECT_EQ(20.0, m_Result.toDB().getTabValReel()[i]);
    }


    // Cas 4: ECRAN EPAIS/MULTIPLE et puissance en dessous de 25 dB
    dB = pow(10.0, (15.0/10.0));
    m_Source.setDefaultValue(dB);
    m_Calcul.setDefaultValue(5.0);

    m_Result = m_AcousticModel.limAttDiffraction(m_Source, m_Calcul);

    for(i=0; i<31; i++)
    {
        EXPECT_EQ(15.0 ,m_Result.toDB().getTabValReel()[i]);
    }

    // Cas 5: ECRAN EPAIS/MULTIPLE et puissance au dessous de 25 dB
    dB = pow(10.0, (40.0/10.0));
    m_Source.setDefaultValue(dB);

    m_Result = m_AcousticModel.limAttDiffraction(m_Source, m_Calcul);

    for(i=0; i<31; i++)
    {
        EXPECT_EQ(25.0, m_Result.toDB().getTabValReel()[i]);
    }
}

// Testing the TYAcousticModel::calculC method
TEST_F(TYAcousticModelTest, calculC)
{
    // Data setup:
    initGlobal(false);

    int i;
    double epaisseur, Ce, x, y, a, b;
    const double tiers = 1.0/3.0;

    TYSolver m_Solver;
    TYAcousticModel m_AcousticModel(m_Solver);
    m_AcousticModel.init();

    OSpectre m_Result;


    // Cas 1: epaisseur < 0.01
    epaisseur = 0.001;

    m_Result = m_AcousticModel.calculC(epaisseur);


    for(i=0; i<31; i++){
        EXPECT_NEAR(1.0, m_Result.getTabValReel()[i], 1e-3);
    }

    // Cas 2: epaisseur = 3.0
    epaisseur = 3.0;
    m_Result = m_AcousticModel.calculC(epaisseur);

    for(i=0; i<31; i++){
        x = (5*m_spectreLambda.getTabValReel()[i])/(epaisseur);
        y = pow(x, 2.0);
        a = 1.0+y;
        b = tiers+y;
        Ce = a/b;
        EXPECT_NEAR(Ce, m_Result.getTabValReel()[i], 1e-3);
    }

}

// Testing the TYAcousticModel::calculAttDiffraction method
TEST_F(TYAcousticModelTest, calculAttDiffraction)
{
    // Data setup:
    initGlobal(false);
    int i;
    const double epaisseur = 1;
    double re = 103.4;
    double resultat, Ni, Ce, A, B;
    bool* miroir, isVertical, avantApres, diffOK;
    double frequence[31] = {16, 20, 25, 31.5, 40, 50, 63, 80, 100, 125, 160, 200, 250, 315, 400, 500, 630, 800, 1000, 1250, 1600, 2000, 2500, 3150, 4000, 5000, 6300, 8000, 10000, 12500, 16000};

    OPoint3D ptA(0.0, 20.0, 00.0);
    OPoint3D ptB(100.0, 20.0, 00.0);

    OSegment3D rayon(ptA, ptB);
    OSegment3D penteMoyenne;

    OSpectre m_Retour;
    OSpectre m_expectedSpectre = OSpectre::getEmptyLinSpectre();

    TYSolver mSolver;
    TYAcousticModel mAcousticModel(mSolver);
    mAcousticModel.init();

    // Cas 1: sans miroir, vertical false, condition favorables et avant l'obstacle
    miroir = new bool(false);
    isVertical = false;
    avantApres = false;
    diffOK = false;
    re = 100.0;
    m_Retour = mAcousticModel.calculAttDiffraction(rayon, rayon, miroir, re, epaisseur, isVertical, avantApres, diffOK);

    for(i=0; i< 31; i++){
        Ni = 2*(0.0)/m_spectreLambda.getValueReal(frequence[i]);
        A = 5*m_spectreLambda.getValueReal(frequence[i])/epaisseur;
        B = A*A;
        Ce = (1.0+B)/((1.0/3.0)+B);
        resultat = sqrt(3+20*Ni*Ce);

        EXPECT_NEAR(resultat, m_Retour.getTabValReel()[i], 1e-2);
    }


    // Cas 2: sans miroir, horizontal, conditions favorables et avant l'obstacle
    isVertical = true;
    //miroir = new bool(true);
    re = 103.4;

    m_Retour = mAcousticModel.calculAttDiffraction(rayon, rayon, miroir, re, epaisseur, isVertical, avantApres, diffOK);

    OSpectre C = mAcousticModel.calculC(epaisseur);

    for(i=0; i< 31; i++){
        Ni = (2*(103.4-100.042))/m_spectreLambda.getValueReal(frequence[i]);
        A = 5*m_spectreLambda.getValueReal(frequence[i])/epaisseur;
        B = A*A;
        Ce = (1.0+B)/((1.0/3.0)+B);
        resultat = 3+(20*Ni*Ce);
        m_expectedSpectre.setValue(frequence[i],resultat);
        m_expectedSpectre = mAcousticModel.limAttDiffraction(m_expectedSpectre, C);
        m_expectedSpectre = m_expectedSpectre.racine();

        EXPECT_NEAR(m_expectedSpectre.getTabValReel()[i], m_Retour.getTabValReel()[i], 1e-2);
    }

}

// Testing the TYAcousticModel::computeCheminsAvecEcran method
TEST_F(TYAcousticModelTest, computeCheminsAvecEcran)
{
    // Data setup:
    initGlobal(false);
    OSpectre expectedSpectre = initBlockPathWay();
    OSpectre returnSpectre;
    AtmosphericConditions _atmos_test(101325., 20., 50.);

    EXPECT_EQ(3,m_tabPoints.size());
    EXPECT_EQ(600.0,rayonSR.longueur());

    TYTabChemin tabChemin;
    //objets necessaires ? la création du solver
    tympan::AcousticProblemModel newProblem;
    tympan::AcousticResultModel newResult;
    tympan::LPSolverConfiguration configuration;


    TYSolver mSolver;

    tympan::node_idx x_inter,y_inter,z_inter,x_NoInter,y_NoInter,z_NoInter;
    tympan::triangle_idx idx_triIntersect,idx_triNoIntersect;

    //Test avec un triangle qui doit intercepter le rayon et l'autre non

    x_inter = newProblem.make_node(0,0,0);//node[0]
    y_inter = newProblem.make_node(0,1000,0);//node[1]
    z_inter = newProblem.make_node(100,0,0);//node[2]

    x_NoInter = newProblem.make_node(1000,0,0);//node[3]
    y_NoInter = newProblem.make_node(0,1000,0);//node[4]
    z_NoInter = newProblem.make_node(0,0,0);//node[5]

    idx_triIntersect = newProblem.make_triangle(x_inter,y_inter,z_inter);
    idx_triNoIntersect = newProblem.make_triangle(x_NoInter,y_NoInter,z_NoInter);

    boost::shared_ptr<tympan::AcousticMaterialBase> mat02(new tympan::AcousticMaterialBase("mat02"));
    boost::shared_ptr<tympan::AcousticMaterialBase> mat3(new tympan::AcousticMaterialBase("mat03"));

    newProblem.triangle(idx_triIntersect).made_of.operator =(mat02);
    newProblem.triangle(idx_triNoIntersect).made_of.operator =(mat3);

    mSolver.solve(newProblem,newResult,configuration);


    TYAcousticModel *modelAcoustic = new TYAcousticModel(mSolver);
    modelAcoustic->init();

    // Call to the tested method:
    bool ret = modelAcoustic->computeCheminsAvecEcran(rayonSR, m_acousticSourceList.at(0), m_tabPoints, true, tabChemin, rayonSR.longueur());

    TYChemin returnWay = tabChemin[3];
    returnSpectre = returnWay.getAttenuation().toDB();

    EXPECT_EQ(true,ret);
    EXPECT_EQ(4,tabChemin.size());
    EXPECT_EQ(m_chemin_ecran.getDistance(),returnWay.getDistance());
    EXPECT_NEAR(m_chemin_ecran.getLongueur(),returnWay.getLongueur(),1e-1);

    //vérification du nombre d'étape dnas le tableau
    EXPECT_EQ(4, m_tabEtape01.size());

    //appel de la méthode ? tester
    m_chemin_ecran.calcAttenuation(m_tabEtape01,_atmos_test);

    //comparaison des spectre
    expectedSpectre = m_chemin_ecran.getAttenuation().toDB();

    EXPECT_EQ(expectedSpectre,returnSpectre);



}

// Testing the TYAcousticModel::computeCheminAPlat method
TEST_F(TYAcousticModelTest, computeCheminAPlat)
{
    // Data setup:
    initGlobal(true);
    OSpectre expectedSpectre = initDirectPathWay();
    OSpectre returnSpectre;
    AtmosphericConditions _atmos_test(101325., 20., 50.);

    EXPECT_EQ(600.0,rayonSR.longueur());
    EXPECT_EQ(1, m_tabEtape01.size());

    TYTabChemin tabChemin;
    //objets necessaires ? la création du solver
    tympan::AcousticProblemModel newProblem;
    tympan::AcousticResultModel newResult;
    tympan::LPSolverConfiguration configuration;


    TYSolver mSolver;

    tympan::node_idx x_inter,y_inter,z_inter,x_NoInter,y_NoInter,z_NoInter;
    tympan::triangle_idx idx_triIntersect,idx_triNoIntersect;

    //Test avec un triangle qui doit intercepter le rayon et l'autre non

    x_inter = newProblem.make_node(0,0,0);//node[0]
    y_inter = newProblem.make_node(0,1000,0);//node[1]
    z_inter = newProblem.make_node(1000,0,0);//node[2]

    x_NoInter = newProblem.make_node(1000,0,0);//node[3]
    y_NoInter = newProblem.make_node(0,1000,0);//node[4]
    z_NoInter = newProblem.make_node(0,0,0);//node[5]

    idx_triIntersect = newProblem.make_triangle(x_inter,y_inter,z_inter);
    idx_triNoIntersect = newProblem.make_triangle(x_NoInter,y_NoInter,z_NoInter);

    boost::shared_ptr<tympan::AcousticMaterialBase> mat02(new tympan::AcousticGroundMaterial("sol",20000,1,500));
    boost::shared_ptr<tympan::AcousticMaterialBase> mat3(new tympan::AcousticGroundMaterial("sol",20000,1,500));

    newProblem.triangle(idx_triIntersect).made_of.operator =(mat02);
    newProblem.triangle(idx_triNoIntersect).made_of.operator =(mat3);

    mSolver.solve(newProblem,newResult,configuration);


    TYAcousticModel *modelAcoustic = new TYAcousticModel(mSolver);
    modelAcoustic->init();

    // Call to the tested method:
    modelAcoustic->computeCheminAPlat(rayonSR, m_acousticSourceList.at(0), tabChemin, rayonSR.longueur());

    //chemin direct
    TYChemin returnWay = tabChemin[0];
    returnSpectre = returnWay.getAttenuation().toDB();
    EXPECT_EQ(2,tabChemin.size());
    EXPECT_EQ(m_chemin_direct.getDistance(),returnWay.getDistance());
    EXPECT_NEAR(m_chemin_direct.getLongueur(),returnWay.getLongueur(),1e-1);


    m_chemin_direct.calcAttenuation(m_tabEtape01,_atmos_test);

    //comparaison des spectre
    expectedSpectre = m_chemin_direct.getAttenuation().toDB();

    EXPECT_EQ(expectedSpectre,returnSpectre);

    //chemin sol
    expectedSpectre = initGroundPathWay();
    returnWay = tabChemin[1];
    returnSpectre = returnWay.getAttenuation().toDB();

    EXPECT_EQ(m_chemin_sol.getDistance(),returnWay.getDistance());
    EXPECT_NEAR(m_chemin_sol.getLongueur(),returnWay.getLongueur(),1e0);

    m_chemin_sol.calcAttenuation(m_tabEtape01,_atmos_test);

    //comparaison des spectre
    expectedSpectre = m_chemin_sol.getAttenuation().toDB();

    EXPECT_EQ(expectedSpectre, returnSpectre);

}

// Testing the TYAcousticModel::computeCheminSansEcran method
TEST_F(TYAcousticModelTest, computeCheminSansEcran)
{
    // Data setup:
    initGlobal(false);
    OSpectre expectedSpectre = initDirectPathWay();
    OSpectre returnSpectre;
    AtmosphericConditions _atmos_test(101325., 20., 50.);

    EXPECT_EQ(600.0,rayonSR.longueur());
    EXPECT_EQ(1, m_tabEtape01.size());

    TYTabChemin tabChemin;
    //objets necessaires ? la création du solver
    tympan::AcousticProblemModel newProblem;
    tympan::AcousticResultModel newResult;
    tympan::LPSolverConfiguration configuration;


    TYSolver mSolver;

    tympan::node_idx x_inter,y_inter,z_inter,x_NoInter,y_NoInter,z_NoInter;
    tympan::triangle_idx idx_triIntersect,idx_triNoIntersect;

    //Test avec un triangle qui doit intercepter le rayon et l'autre non

    x_inter = newProblem.make_node(0,0,0);//node[0]
    y_inter = newProblem.make_node(0,1000,0);//node[1]
    z_inter = newProblem.make_node(1000,0,0);//node[2]

    x_NoInter = newProblem.make_node(1000,0,0);//node[3]
    y_NoInter = newProblem.make_node(0,1000,0);//node[4]
    z_NoInter = newProblem.make_node(0,0,0);//node[5]

    idx_triIntersect = newProblem.make_triangle(x_inter,y_inter,z_inter);
    idx_triNoIntersect = newProblem.make_triangle(x_NoInter,y_NoInter,z_NoInter);

    boost::shared_ptr<tympan::AcousticMaterialBase> mat02(new tympan::AcousticGroundMaterial("sol",20000,1,500));
    boost::shared_ptr<tympan::AcousticMaterialBase> mat3(new tympan::AcousticGroundMaterial("sol",20000,1,500));

    newProblem.triangle(idx_triIntersect).made_of.operator =(mat02);
    newProblem.triangle(idx_triNoIntersect).made_of.operator =(mat3);

    mSolver.solve(newProblem,newResult,configuration);


    TYAcousticModel *modelAcoustic = new TYAcousticModel(mSolver);
    modelAcoustic->init();

    // Call to the tested method:
    modelAcoustic->computeCheminSansEcran(rayonSR, m_acousticSourceList.at(0), tabChemin, rayonSR.longueur());

    //chemin direct
    TYChemin returnWay = tabChemin[0];
    returnSpectre = returnWay.getAttenuation().toDB();
    EXPECT_EQ(4,tabChemin.size());
    EXPECT_EQ(m_chemin_direct.getDistance(),returnWay.getDistance());
    EXPECT_NEAR(m_chemin_direct.getLongueur(),returnWay.getLongueur(),1e-1);


    m_chemin_direct.calcAttenuation(m_tabEtape01,_atmos_test);

    //comparaison des spectre
    expectedSpectre = m_chemin_direct.getAttenuation().toDB();

    EXPECT_EQ(expectedSpectre,returnSpectre);

    //chemin sol
    expectedSpectre = initGroundPathWay();
    returnWay = tabChemin[3];
    returnSpectre = returnWay.getAttenuation().toDB();

    EXPECT_EQ(m_chemin_sol.getDistance(),returnWay.getDistance());
    EXPECT_NEAR(m_chemin_sol.getLongueur(),returnWay.getLongueur(),1e0);

    m_chemin_sol.calcAttenuation(m_tabEtape01,_atmos_test);

    //comparaison des spectre
    expectedSpectre = m_chemin_sol.getAttenuation().toDB();
    EXPECT_EQ(expectedSpectre,returnSpectre);

}

// TO DO (SEGMENTATION FAULT)
// Testing the TYAcousticModel::addEtapesSol method
TEST(test_TYAcousticModel, addEtapesSol)
{/*
    // Data setup:
    double rr = 10.77;
    OPoint3D ptDebut1(5.0, 0.0, 5.0);
    OPoint3D ptFin(10.0, 5.0, 5.0);
    OPoint3D ptDebut2(0.0, 0.0, 5.0);
    OPoint3D Source(0.0, 0.0, 0.0);
    OPoint3D Recepteur(10.0, 2.0, 0.0);
    const tympan::Point point(0.0, 0.0, 50.0);
    //bool fromSource = new bool(false);
    //fromSource = false;
    //bool fromReceptor = new bool(false);
    //fromReceptor = false;

    OSegment3D penteMoyenne(Source, Recepteur);

    tympan::Spectrum mSpectrum;
    mSpectrum.setDefaultValue(90);
    tympan::SourceDirectivityInterface* directivity = new tympan::SphericalSourceDirectivity();
    directivity->lwAdjustment(point, rr);
    //tympan::AcousticSource* m_AcousticSource = new tympan::AcousticSource(point, mSpectrum, directivity);
    TYTabEtape mEtapes;

    TYSolver mSolver;
    TYAcousticModel mAcousticModel(mSolver);

    // Call to the tested method:
    // Cas 1 : Depart / Arrive qui ne sont pas au sol, l'onde ne vient pas d'une source (diffraction)
    bool t_Sol1 = mAcousticModel.addEtapesSol(ptDebut1, ptFin, penteMoyenne, *m_AcousticSource, fromSource, fromReceptor, mEtapes, rr);

    // Cas 2 : Depart d'une source au sol et arrivee a un recepteur ne touchant pas le sol
    bool t_Sol2 = mAcousticModel.addEtapesSol(ptDebut2, ptFin, penteMoyenne, *m_AcousticSource, true, true, mEtapes, rr);

    // Check result:
    EXPECT_EQ(true, t_Sol1);
    EXPECT_EQ(true, t_Sol2);

    */
}

// Testing the TYAcousticModel::computeCheminReflexion method
TEST_F(TYAcousticModelTest, computeCheminReflexion)
{
    // Data setup:
    initGlobal(false);
    OSpectre expectedSpectre = initReflexionPathWay();
    OSpectre returnSpectre;
    AtmosphericConditions _atmos_test(101325., 20., 50.);

    //vérification du nombre d'étape dnas le tableau
    EXPECT_EQ(2, m_tabEtape01.size());
    EXPECT_EQ(600.0,rayonSR.longueur());

    TYTabChemin tabChemin;
    //objets necessaires ? la création du solver
    tympan::AcousticProblemModel newProblem;
    tympan::AcousticResultModel newResult;
    tympan::LPSolverConfiguration configuration;


    TYSolver mSolver;

    tympan::node_idx x_inter,y_inter,z_inter,x_NoInter,y_NoInter,z_NoInter;
    tympan::triangle_idx idx_triIntersect,idx_triNoIntersect;

    //Test avec un triangle qui doit intercepter le rayon et l'autre non

    x_inter = newProblem.make_node(0,0,0);//node[0]
    y_inter = newProblem.make_node(0,1000,0);//node[1]
    z_inter = newProblem.make_node(100,0,0);//node[2]

    x_NoInter = newProblem.make_node(1000,0,0);//node[3]
    y_NoInter = newProblem.make_node(0,1000,0);//node[4]
    z_NoInter = newProblem.make_node(0,0,0);//node[5]

    idx_triIntersect = newProblem.make_triangle(x_inter,y_inter,z_inter);
    idx_triNoIntersect = newProblem.make_triangle(x_NoInter,y_NoInter,z_NoInter);

    boost::shared_ptr<tympan::AcousticMaterialBase> mat02(new tympan::AcousticMaterialBase("mat02"));
    boost::shared_ptr<tympan::AcousticMaterialBase> mat3(new tympan::AcousticMaterialBase("mat03"));

    newProblem.triangle(idx_triIntersect).made_of.operator =(mat02);
    newProblem.triangle(idx_triNoIntersect).made_of.operator =(mat3);

    mSolver.solve(newProblem,newResult,configuration);



    TYAcousticModel *modelAcoustic = new TYAcousticModel(mSolver);
    modelAcoustic->init();

    // Call to the tested method:
    modelAcoustic->computeCheminReflexion(m_tabIntersect,rayonSR, m_acousticSourceList.at(0), tabChemin, rayonSR.longueur());

    TYChemin returnWay = tabChemin[0];
    returnSpectre = returnWay.getAttenuation().toDB();

    EXPECT_EQ(1,tabChemin.size());
    EXPECT_EQ(m_chemin_relex.getDistance(),returnWay.getDistance());
    EXPECT_NEAR(m_chemin_relex.getLongueur(),returnWay.getLongueur(),1e-1);



    //appel de la méthode ? tester
    m_chemin_relex.calcAttenuation(m_tabEtape01,_atmos_test);

    //comparaison des spectre
    expectedSpectre = m_chemin_relex.getAttenuation().toDB();

    EXPECT_EQ(expectedSpectre,returnSpectre);
}

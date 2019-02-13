/**
 * @file misc.cpp
 *
 * @brief Implementation of misc utilities for testing.
 */

#include<iostream>
#include<string>
#include<deque>

#include<boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>

#include "testutils.h"

using namespace std;
using namespace boost;

deque<deque<string> > readCsvAsStringTable(istream& in)
{
    typedef tokenizer< escaped_list_separator<char> > Tokenizer;
    deque<deque<string> > table;
    string line;
    while (getline(in, line))
    {
        deque<string> line_array;
        Tokenizer tok(line);
        BOOST_FOREACH(const string & str_rec, tok)
        {
            line_array.push_back(trim_copy(str_rec));
        }
        table.push_back(line_array);
    }
    return table;
}



//m�thode d'initialisation des objets globaux
void TYInitUserCase::initGlobal(double p_distance){
    m_isInitGlobal = false;
    m_spectrPrecision = 5.0;
    //constantes globales
    m_gamma = 1.41;
    m_perfectGaz = 8.31;
    m_molareMass = 0.029;
    m_temperature = 20.0;
    m_absoluteZero = 273.15;
    m_pression = 101325.0;
    m_hydromzetry = 50.0;
    m_distance = p_distance;

    m_tk = m_temperature + m_absoluteZero;
    m_soundSpeed = sqrt((m_gamma*m_perfectGaz*m_tk)/m_molareMass);
    m_spectreLambda = OSpectre::getLambda(m_soundSpeed);

    m_pointSource.setCoords(0,50,20);
    m_pintReception.setCoords(p_distance,50,20);

    m_chemin_direct.setType(CHEMIN_DIRECT);
    m_chemin_sol.setType(CHEMIN_SOL);
    m_chemin_ecran.setType(CHEMIN_ECRAN);
    m_chemin_relex.setType(CHEMIN_REFLEX);

    rayonSR._ptA = m_pointSource;
    rayonSR._ptB = m_pintReception;

    m_spectre_source.setDefaultValue(90);
    m_directivityVector.setCoords(1,1,1);
    m_directivity = new tympan::SphericalSourceDirectivity();
    m_directivity->lwAdjustment(m_directivityVector,m_distance);

    tympan::AcousticSource m_acousticSource(m_pointSource,m_spectre_source,m_directivity);
    m_acousticSourceList.push_back(m_acousticSource);
    tympan::AcousticReceptor m_acousticReceptor(m_pintReception);
    m_acousticReceptionList.push_back(m_acousticReceptor);

    m_isInitGlobal = true;
}

//m�thode cas utilisateur chemin direct
OSpectre TYInitUserCase::initDirectPathWay(bool p_isFlate){

    if(p_isFlate){
        m_pointSource.setCoords(0,50,0);
        m_pintReception.setCoords(m_distance,50,0);
    }else{
        m_pointSource.setCoords(0,50,20);
        m_pintReception.setCoords(m_distance,50,20);
    }

    OSpectre _spectreToReturn;
    //remplissage du spectre avecc les valeurs attendues
    _spectreToReturn.setValue(16.0,89.96);
    _spectreToReturn.setValue(20.0,89.95);
    _spectreToReturn.setValue(25.0,89.92);
    _spectreToReturn.setValue(31.5,89.89);
    _spectreToReturn.setValue(40.0,89.83);
    _spectreToReturn.setValue(50.0,89.75);
    _spectreToReturn.setValue(63.0,89.65);
    _spectreToReturn.setValue(80.0,89.53);
    _spectreToReturn.setValue(100.0,89.38);
    _spectreToReturn.setValue(125.0,89.22);
    _spectreToReturn.setValue(160.0,89.03);
    _spectreToReturn.setValue(200.0,88.81);
    _spectreToReturn.setValue(250.0,88.54);
    _spectreToReturn.setValue(315.0,88.14);
    _spectreToReturn.setValue(400.0,87.53);
    _spectreToReturn.setValue(500.0,86.66);
    _spectreToReturn.setValue(630.0,85.28);
    _spectreToReturn.setValue(800.0,83.09);
    _spectreToReturn.setValue(1000.0,80.01);
    _spectreToReturn.setValue(1250.0,75.54);
    _spectreToReturn.setValue(1600.0,68.53);
    _spectreToReturn.setValue(2000.0,60.10);
    _spectreToReturn.setValue(2500.0,49.90);
    _spectreToReturn.setValue(3150.0,38.41);
    _spectreToReturn.setValue(4000.0,27.12);
    _spectreToReturn.setValue(5000.0,18.47);
    _spectreToReturn.setValue(6300.0,12.02);
    _spectreToReturn.setValue(8000.0,7.73);
    _spectreToReturn.setValue(10000.0,5.26);
    _spectreToReturn.setValue(12500.0,3.67);
    _spectreToReturn.setValue(16000.0,2.48);


    //MAJ distance/longueur
    m_tabEtape01.clear();
    m_chemin_direct.setDistance(m_distance);
    m_chemin_direct.setLongueur(m_distance);

    //MAJ de l'�tape
    TYEtape etape01;
    etape01.setType(TYSOURCE);
    etape01.setPoint(m_pointSource);
    etape01._Absorption = m_spectre_source;
    m_tabEtape01.push_back(etape01);

    return _spectreToReturn;

}

//m�thode cas utilisateur chemin sol
OSpectre TYInitUserCase::initGroundPathWay(){

    OSpectre _spectreToReturn;
    //remplissage du spectre avecc les valeurs attendues
    _spectreToReturn.setValue(16.0,22.10);
    _spectreToReturn.setValue(20.0,70.76);
    _spectreToReturn.setValue(25.0,119.20);
    _spectreToReturn.setValue(31.5,90.62);
    _spectreToReturn.setValue(40.0,28.74);
    _spectreToReturn.setValue(50.0,48.37);
    _spectreToReturn.setValue(63.0,40.67);
    _spectreToReturn.setValue(80.0,35.65);
    _spectreToReturn.setValue(100.0,30.33);
    _spectreToReturn.setValue(125.0,25.21);
    _spectreToReturn.setValue(160.0,19.84);
    _spectreToReturn.setValue(200.0,15.40);
    _spectreToReturn.setValue(250.0,11.48);
    _spectreToReturn.setValue(315.0,8.07);
    _spectreToReturn.setValue(400.0,5.35);
    _spectreToReturn.setValue(500.0,3.58);
    _spectreToReturn.setValue(630.0,2.50);
    _spectreToReturn.setValue(800.0,2.10);
    _spectreToReturn.setValue(1000.0,2.25);
    _spectreToReturn.setValue(1250.0,2.71);
    _spectreToReturn.setValue(1600.0,3.31);
    _spectreToReturn.setValue(2000.0,3.71);
    _spectreToReturn.setValue(2500.0,3.78);
    _spectreToReturn.setValue(3150.0,3.46);
    _spectreToReturn.setValue(4000.0,2.82);
    _spectreToReturn.setValue(5000.0,2.14);
    _spectreToReturn.setValue(6300.0,1.54);
    _spectreToReturn.setValue(8000.0,1.08);
    _spectreToReturn.setValue(10000.0,0.80);
    _spectreToReturn.setValue(12500.0,0.61);
    _spectreToReturn.setValue(16000.0,0.45);


    //Effacement des pr�cedentes etapes
    m_tabEtape01.clear();

    //Etape avant la reflexion
    OPoint3D _ptReflexion(50,50,0);
    TYEtape etape01;
    etape01.setType(TYSOURCE);
    etape01.setPoint(m_pointSource);

    OSegment3D segSourceToReflexion(m_pointSource, _ptReflexion); // On part de la source vers le point de reflexion
    double rr = segSourceToReflexion.longueur();

    // Absorption de la source
    etape01._Absorption = m_spectre_source;

    m_tabEtape01.push_back(etape01); // Ajout de l'etape avant reflexion

    //Etape apres la reflexion
    TYEtape etape2;
    etape2.setPoint(_ptReflexion);
    etape2.setType(TYREFLEXIONSOL);

    OSegment3D seg2 = OSegment3D(_ptReflexion, m_pintReception);// Segment Point de reflexion->Point de reception
    rr += seg2.longueur(); // Longueur parcourue sur le trajet reflechi

    tympan::AcousticGroundMaterial *mat = new tympan::AcousticGroundMaterial("sol",20000,1,5000);

    // Angle estimation
    OVector3D direction(segSourceToReflexion._ptA, segSourceToReflexion._ptB);
    direction.normalize();

    double angle = (direction*-1).angle(OVector3D(segSourceToReflexion._ptB, rayonSR._ptB));
    double angle2 = (3.14 - angle)/2.0;
    etape2._Absorption = mat->get_absorption(angle2, rr);

    m_tabEtape01.push_back(etape2); //Ajout de l'�tape apr?s reflexion

    m_chemin_sol.setLongueur(rr);
    m_chemin_sol.setDistance(m_distance);

    return _spectreToReturn;

}

//m�thode cas utilisateur chemin �cran
OSpectre TYInitUserCase::initBlockPathWay(){

    OSpectre _spectreToReturn;
    double _longueur = 0.0;
    double _epaisseur = 0.0;

    //objets necessaires ? la cr�ation du solver
    tympan::AcousticProblemModel newProblem;
    tympan::AcousticResultModel newResult;
    tympan::LPSolverConfiguration configuration;


    TYSolver mSolver;
    mSolver.solve(newProblem,newResult,configuration);
    TYAcousticModel *modelAcoustic = new TYAcousticModel(mSolver);
    modelAcoustic->init();

    //remplissage du spectre avecc les valeurs attendues
    _spectreToReturn.setValue(16.0,26.70);
    _spectreToReturn.setValue(20.0,24.53);
    _spectreToReturn.setValue(25.0,22.44);
    _spectreToReturn.setValue(31.5,20.38);
    _spectreToReturn.setValue(40.0,18.38);
    _spectreToReturn.setValue(50.0,16.63);
    _spectreToReturn.setValue(63.0,14.96);
    _spectreToReturn.setValue(80.0,13.37);
    _spectreToReturn.setValue(100.0,12.01);
    _spectreToReturn.setValue(125.0,10.78);
    _spectreToReturn.setValue(160.0,9.54);
    _spectreToReturn.setValue(200.0,8.52);
    _spectreToReturn.setValue(250.0,7.60);
    _spectreToReturn.setValue(315.0,6.73);
    _spectreToReturn.setValue(400.0,5.90);
    _spectreToReturn.setValue(500.0,5.18);
    _spectreToReturn.setValue(630.0,4.48);
    _spectreToReturn.setValue(800.0,3.79);
    _spectreToReturn.setValue(1000.0,3.16);
    _spectreToReturn.setValue(1250.0,2.56);
    _spectreToReturn.setValue(1600.0,1.94);
    _spectreToReturn.setValue(2000.0,1.43);
    _spectreToReturn.setValue(2500.0,0.99);
    _spectreToReturn.setValue(3150.0,0.63);
    _spectreToReturn.setValue(4000.0,0.37);
    _spectreToReturn.setValue(5000.0,0.21);
    _spectreToReturn.setValue(6300.0,0.11);
    _spectreToReturn.setValue(8000.0,0.06);
    _spectreToReturn.setValue(10000.0,0.03);
    _spectreToReturn.setValue(12500.0,0.02);
    _spectreToReturn.setValue(16000.0,0.01);


    //Effacement des pr�cedentes etapes
    m_tabEtape01.clear();

    OPoint3D ptFirst(49,50,35);
    OPoint3D ptLast(50,50,35);

    m_tabPoints.push_back(m_pointSource);
    m_tabPoints.push_back(ptFirst);
    m_tabPoints.push_back(ptLast);

    TYEtape etape1,etape2,etape3,etape4;
    etape1.setType(TYSOURCE);
    etape1.setPoint(m_pointSource);
    etape1._Absorption = m_spectre_source;
    m_tabEtape01.push_back(etape1);

    OSegment3D segCourant(rayonSR._ptA, ptFirst);
    _longueur = segCourant.longueur();

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
    etape4._Attenuation = modelAcoustic->calculAttDiffraction(rayonSR, rayonSR, false, _longueur, _epaisseur, true, false, diff);
    m_tabEtape01.push_back(etape4);

    m_chemin_ecran.setDistance(m_distance);

    m_chemin_ecran.setLongueur(_longueur);

    return _spectreToReturn;

}

//m�thode cas utilisateur chemin reflexion
OSpectre TYInitUserCase::initReflexionPathWay(){
    OSpectre _spectreToReturn;
    m_pointSource.setCoords(0,50,20);
    OPoint3D _ptRecep(0,50,40);
    OPoint3D _ptReflex(m_distance,50,30);

    OPoint3D ptS11(m_distance, 50, 0);
    OPoint3D ptS12(m_distance, 50, 100);
    OPoint3D ptS21(0, 50, 40);
    OPoint3D ptS22(500, 50, 40);

    OSegment3D segV(ptS11, ptS12);
    OSegment3D segH(ptS21, ptS22);

    TYSIntersection m_Intersection01;
    m_Intersection01.isEcran = true;
    m_Intersection01.isInfra = true;
    m_Intersection01.segInter[0] = segV;
    m_Intersection01.segInter[1] = segH;
    m_Intersection01.bIntersect[1] = true;
    tympan::AcousticBuildingMaterial *mat = new tympan::AcousticBuildingMaterial("Batiment",OSpectreComplex(TYComplex(1.0, 0.0)));
    m_Intersection01.material = mat;

    m_tabIntersect.push_back(m_Intersection01);

    //remplissage du spectre avecc les valeurs attendues
    _spectreToReturn.setValue(16.0,89.94);
    _spectreToReturn.setValue(20.0,89.90);
    _spectreToReturn.setValue(25.0,89.85);
    _spectreToReturn.setValue(31.5,89.78);
    _spectreToReturn.setValue(40.0,89.66);
    _spectreToReturn.setValue(50.0,89.51);
    _spectreToReturn.setValue(63.0,89.31);
    _spectreToReturn.setValue(80.0,89.06);
    _spectreToReturn.setValue(100.0,88.76);
    _spectreToReturn.setValue(125.0,88.43);
    _spectreToReturn.setValue(160.0,88.07);
    _spectreToReturn.setValue(200.0,87.63);
    _spectreToReturn.setValue(250.0,87.07);
    _spectreToReturn.setValue(315.0,86.29);
    _spectreToReturn.setValue(400.0,85.14);
    _spectreToReturn.setValue(500.0,83.39);
    _spectreToReturn.setValue(630.0,80.75);
    _spectreToReturn.setValue(800.0,76.65);
    _spectreToReturn.setValue(1000.0,71.04);
    _spectreToReturn.setValue(1250.0,63.29);
    _spectreToReturn.setValue(1600.0,52.04);
    _spectreToReturn.setValue(2000.0,39.97);
    _spectreToReturn.setValue(2500.0,27.51);
    _spectreToReturn.setValue(3150.0,16.25);
    _spectreToReturn.setValue(4000.0,8.07);
    _spectreToReturn.setValue(5000.0,3.73);
    _spectreToReturn.setValue(6300.0,1.57);
    _spectreToReturn.setValue(8000.0,0.64);
    _spectreToReturn.setValue(10000.0,0.29);
    _spectreToReturn.setValue(12500.0,0.14);
    _spectreToReturn.setValue(16000.0,0.06);


    //MAJ distance/longueur
    m_tabEtape01.clear();
    m_chemin_relex.setDistance(20.0);


    TYEtape etape1,etape2;
    etape1._pt = m_pointSource;
    etape1._type = TYSOURCE;
    etape1._Absorption = m_spectre_source;

    m_tabEtape01.push_back(etape1);

    // Deuxieme etape : du point de reflexion a la fin du rayon
    OSegment3D segSourceToReflexion(m_pointSource, _ptReflex); // On part de la source vers le point de reflexion
    double rr = segSourceToReflexion.longueur();

    etape2._pt = _ptReflex;
    etape2._type = TYREFLEXION;

    OSpectre temp = OSpectre::getEmptyLinSpectre();
    etape2._Absorption = temp.mult(-1.0).sum(1.0);

    m_tabEtape01.push_back(etape2);

    OSegment3D segReflexToRecep(_ptReflex, _ptRecep);
    rr+=segReflexToRecep.longueur();
    m_chemin_relex.setLongueur(rr);

    return _spectreToReturn;

}

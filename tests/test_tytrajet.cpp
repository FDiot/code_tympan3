/**
*
* @brief Functional tests of the TYTrajet class  
* 
*  Created on: january 18, 2018 
*  Author: Philippe CHAPUIS <philippe.chapuis@c-s.fr> 
*
*/

#define _USE_MATH_DEFINES

#include "gtest/gtest.h"
#include "Tympan/solvers/DefaultSolver/TYTrajet.h"
#include "Tympan/solvers/DefaultSolver/TYChemin.h"
#include "Tympan/models/solver/entities.hpp"
#include "Tympan/solvers/DefaultSolver/TYSolver.h"
#include "Tympan/models/solver/acoustic_problem_model.hpp"
#include "Tympan/models/solver/data_model_common.hpp"
#include "Tympan/solvers/DefaultSolver/TYAcousticModel.h"
#include <math.h>

# define M_PI           3.14159265358979323846  /* pi */

/**
* @brief Fixture which provides an instance of TYTrajet
*/
class TYTrajetTest :
    public testing::Test
{
private:
    //TYTrajet Trajet;

public:
    bool m_isInitGlobal;

    //constantes globales
    double m_gamma;
    double m_perfectGaz;
    double m_molareMass;
    double m_temperature;
    double m_absoluteZero;
    double m_pression;
    double m_hydromzetry;
    double m_distance;
    double m_tk, m_soundSpeed;
    OSpectre m_spectreLambda;

    //objets chemins
    TYChemin m_chemin_direct;
    TYChemin m_chemin_sol;
    TYChemin m_chemin_ecran;
    TYChemin m_chemin_relex;

    //tableau d'�tapes
    TYTabEtape m_tabEtape01;

    //spectre source
    OSpectreComplex m_spectre_source;

    //directivit�
    OVector3D m_directivityVector;
    tympan::SourceDirectivityInterface* m_directivity;

    //points source et reception
    OPoint3D m_pointSource;
    OPoint3D m_pintReception;
    OSegment3D rayonSR;
    std::vector<tympan::AcousticSource> m_acousticSourceList;
    std::vector<tympan::AcousticReceptor> m_acousticReceptionList;

    //m�thode d'initialisation des objets globaux
    void initGlobal(){
        m_isInitGlobal = false;
        m_gamma = 1.41;
        m_perfectGaz = 8.31;
        m_molareMass = 0.029;
        m_temperature = 20;
        m_absoluteZero = 273.15;
        m_pression = 101325.0;
        m_hydromzetry = 50.0;
        m_distance = 100.0;

        m_tk = m_temperature + m_absoluteZero;
        m_soundSpeed = sqrt((m_gamma*m_perfectGaz*m_tk)/m_molareMass);
        m_spectreLambda = OSpectre::getLambda(m_soundSpeed);

        m_pointSource.setCoords(0,50,20);
        m_pintReception.setCoords(100,50,20);

       // m_atmoCondition(100.1,55.4,77.1);
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
    OSpectre initDirectPathWay(){
        OSpectre _spectreToReturn;
        //remplissage du spectre avecc les valeurs attendues dans _spectreToReturn
        //TODO

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

    //m�thode cas utilisateur chemin �cran
    OSpectre initBlockPathWay(){

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

        //remplissage du spectre avecc les valeurs attendues dans _spectreToReturn
        //TODO

        //Effacement des pr�cedentes etapes
        m_tabEtape01.clear();

        OPoint3D ptFirst(49,50,35);
        OPoint3D ptLast(50,50,35);

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
    OSpectre initReflexionPathWay(){
        OSpectre _spectreToReturn;
        OPoint3D _ptRecep(0,50,0);
        OPoint3D _ptReflex(100,50,10);
        //remplissage du spectre avecc les valeurs attendues dans _spectreToReturn
        //TODO

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

        etape2._pt = _ptReflex;
        etape2._type = TYREFLEXION;
        etape2._Absorption = OSpectreComplex(TYComplex(1.0, 0.0));

        m_tabEtape01.push_back(etape2);

        OSegment3D segReflexToRecep(_ptReflex, _ptRecep);
        rr+=segReflexToRecep.longueur();

        m_chemin_relex.setLongueur(rr);

        return _spectreToReturn;

    }
};

// Testing the TYTrajet::getPEnergetique method
TEST_F(TYTrajetTest, getPEnergetique)
{
    //initialisation des objets communs
    initGlobal();

    if(m_isInitGlobal){
        //variable condition atmospherique
        AtmosphericConditions _atmos_test(101325., 20., 50.);
        OSpectre _expectedSpectre;
        OSpectre _returnSpectreDB;


        // On pose les bases du trajet --> où se trouvent la Source et le Recepteur
        TYTrajet mTrajet(m_acousticSourceList.at(0), m_acousticReceptionList.at(0));

        //CAS 1 CHEMIN DIRECT
        //appel de la m�thode cas chemin direct
        _expectedSpectre = initDirectPathWay();

        //v�rification du nombre d'�tape dnas le tableau
        EXPECT_EQ(1, m_tabEtape01.size());

        m_chemin_direct.calcAttenuation(m_tabEtape01, _atmos_test);

        mTrajet.addChemin(m_chemin_direct);
        mTrajet.addCheminDirect(m_chemin_direct);

        _returnSpectreDB = mTrajet.getPEnergetique(_atmos_test);

        EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);


        // CAS CHEMIN_ECRAN
        // On pose les bases du trajet --> où se trouvent la Source et le Recepteur
        TYTrajet mTrajetEcran(m_acousticSourceList.at(0), m_acousticReceptionList.at(0));

        //appel de la m�thode cas chemin direct
        _expectedSpectre = initBlockPathWay();

        //v�rification du nombre d'�tape dnas le tableau
        EXPECT_EQ(4, m_tabEtape01.size());

        m_chemin_ecran.calcAttenuation(m_tabEtape01, _atmos_test);

        mTrajetEcran.addChemin(m_chemin_ecran);
        mTrajetEcran.addCheminDirect(m_chemin_direct);

        m_chemin_ecran.calcAttenuation(m_tabEtape01, _atmos_test);

        _returnSpectreDB = mTrajetEcran.getPEnergetique(_atmos_test);

        EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);

    }

}

// Testing the TYTrajet::getPInterference method
TEST_F(TYTrajetTest, getPInterference)
{
    //initialisation des objets communs
    initGlobal();

    if(m_isInitGlobal){
        //variable condition atmospherique
        AtmosphericConditions _atmos_test(101325., 20., 50.);
        OSpectre _expectedSpectre;
        OSpectre _returnSpectreDB;


        // On pose les bases du trajet --> où se trouvent la Source et le Recepteur
        TYTrajet mTrajet(m_acousticSourceList.at(0), m_acousticReceptionList.at(0));

        // Cas CHEMIN DIRECT
        //appel de la m�thode cas chemin direct
        _expectedSpectre = initDirectPathWay();

        //v�rification du nombre d'�tape dnas le tableau
        EXPECT_EQ(1, m_tabEtape01.size());

        m_chemin_direct.calcAttenuation(m_tabEtape01, _atmos_test);

        mTrajet.addChemin(m_chemin_direct);
        mTrajet.addCheminDirect(m_chemin_direct);

        _returnSpectreDB = mTrajet.getPInterference(_atmos_test);


        EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);


        // CAS CHEMIN_ECRAN
        // On pose les bases du trajet --> où se trouvent la Source et le Recepteur
        TYTrajet mTrajetEcran(m_acousticSourceList.at(0), m_acousticReceptionList.at(0));

        //appel de la m�thode cas chemin direct
        _expectedSpectre = initBlockPathWay();

        //v�rification du nombre d'�tape dnas le tableau
        EXPECT_EQ(4, m_tabEtape01.size());

        m_chemin_ecran.calcAttenuation(m_tabEtape01, _atmos_test);

        mTrajetEcran.addChemin(m_chemin_ecran);
        mTrajetEcran.addCheminDirect(m_chemin_direct);

        m_chemin_ecran.calcAttenuation(m_tabEtape01, _atmos_test);

        _returnSpectreDB = mTrajetEcran.getPInterference(_atmos_test);


        EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);


        // Cas CHEMIN REFLEX
        // On pose les bases du trajet --> où se trouvent la Source et le Recepteur
        TYTrajet mTrajetREFLEX(m_acousticSourceList.at(0), m_acousticReceptionList.at(0));

        //appel de la m�thode cas chemin direct
        _expectedSpectre = initReflexionPathWay();

        //v�rification du nombre d'�tape dnas le tableau
        EXPECT_EQ(2, m_tabEtape01.size());

        m_chemin_relex.calcAttenuation(m_tabEtape01, _atmos_test);

        mTrajetREFLEX.addChemin(m_chemin_relex);
        mTrajetREFLEX.addCheminDirect(m_chemin_direct);

        m_chemin_relex.calcAttenuation(m_tabEtape01, _atmos_test);

        _returnSpectreDB = mTrajetREFLEX.getPInterference(_atmos_test);

        EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);

    }
}




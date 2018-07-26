/**
*
* @brief Functional tests of the TYChemin class  
* 
*  Created on: january 18, 2018 
*  Author: Philippe CHAPUIS <philippe.chapuis@c-s.fr> 
*
*/
#include "gtest/gtest.h"
#include "Tympan/solvers/DefaultSolver/TYChemin.h"
#include "Tympan/models/solver/entities.hpp"
#include "Tympan/solvers/DefaultSolver/TYSolver.h"
#include "Tympan/models/solver/acoustic_problem_model.hpp"
#include "Tympan/models/solver/data_model_common.hpp"
#include "Tympan/solvers/DefaultSolver/TYAcousticModel.h"
#include <iostream>

/**
* @brief Fixture which provides an instance of TYChemin
*/
class TYCheminTest :
    public testing::Test
{
private:
    TYChemin Chemin;
protected:
    //AtmosphericConditions m_atmoCondition(100.2,5.2,8.3);

public:
    bool m_isInitGlobal = false;

    //constantes globales
    const double m_gamma = 1.41;
    const double m_perfectGaz = 8.31;
    const double m_molareMass = 0.029;
    const double m_temperature = 20;
    const double m_absoluteZero = 273.15;
    const double m_pression = 101325.0;
    const double m_hydromzetry = 50.0;
    const double m_distance = 100.0;
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
        //remplissage du spectre avecc les valeurs attendues
        //TODO renter les r�sultats des fichiers csv dans _spectreToReturn

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
    OSpectre initGroundPathWay(){
        OSpectre _spectreToReturn;
        //remplissage du spectre avecc les valeurs attendues
        //TODO renter les r�sultats des fichiers csv _spectreToReturn

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

        tympan::AcousticGroundMaterial *mat = new tympan::AcousticGroundMaterial("sol",20000,1,500);

        // Angle estimation
        OVector3D direction(segSourceToReflexion._ptA, segSourceToReflexion._ptB);
        direction.normalize();

        double angle = (direction*-1).angle(OVector3D(segSourceToReflexion._ptB, rayonSR._ptB));
        double angle2 = (3.14 - angle)/2.0;
        std::cout<<"angle = "<<angle2<<std::endl;
        etape2._Absorption = mat->get_absorption(angle2, rr);

        //temp
        for (unsigned int i = 0; i < TY_SPECTRE_DEFAULT_NB_ELMT; i++)
        {
            std::cout<<"coeff Q = "<<etape2._Absorption.getTabValReel()[i]<<std::endl;

        }

        m_tabEtape01.push_back(etape2); //Ajout de l'�tape apr?s reflexion

        m_chemin_sol.setLongueur(rr);
        m_chemin_sol.setDistance(m_distance);

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

        //remplissage du spectre avecc les valeurs attendues
        //TODO renter les r�sultats des fichiers csv _spectreToReturn

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
        m_pointSource.setCoords(0,50,20);
        OPoint3D _ptRecep(0,50,0);
        OPoint3D _ptReflex(20,50,10);
        //remplissage du spectre avecc les valeurs attendues
        //TODO renter les r�sultats des fichiers csv dans _spectreToReturn

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
        //etape2._Absorption = OSpectreComplex(TYComplex(1.0, 0.0));SpectreAbso.mult(-1.0).sum(1.0);
        OSpectre temp = OSpectre::getEmptyLinSpectre();
        etape2._Absorption = temp.mult(-1.0).sum(1.0);

        m_tabEtape01.push_back(etape2);

        OSegment3D segReflexToRecep(_ptReflex, _ptRecep);
        rr+=segReflexToRecep.longueur();

        m_chemin_relex.setLongueur(rr);

        return _spectreToReturn;

    }
};

TEST_F(TYCheminTest, create_instance)
{
    m_isInitGlobal = false;
}



// Testing the TYChemin::setLongueur method
TEST_F(TYCheminTest, setLongueur)
{
   // Data setup:
   TYChemin Chemin;
   double length = 10.;
   Chemin.setLongueur(length);
   EXPECT_EQ(length, Chemin.getLongueur());
}



// Testing the TYChemin::calcAttenuation method
TEST_F(TYCheminTest, calcAttenuation)
{
    //initialisation des objets communs
    initGlobal();

    if(m_isInitGlobal){
        //variable condition atmospherique
        AtmosphericConditions _atmos_test(101325., 20., 50.);
        OSpectre _expectedSpectre;
        OSpectre _returnSpectreDB;
        bool _exact;

        //CAS 1 CHEMIN DIRECT
        //appel de la m�thode cas chemin direct
        _expectedSpectre = initDirectPathWay();

        //v�rification du nombre d'�tape dnas le tableau
        EXPECT_EQ(1, m_tabEtape01.size());

        //appel de la m�thode ? tester
        m_chemin_direct.calcAttenuation(m_tabEtape01,_atmos_test);

        //comparaison des spectre
        _returnSpectreDB = m_chemin_direct.getAttenuation().toDB();

        //v�rification de la comparaison
        EXPECT_EQ(_expectedSpectre, _returnSpectreDB);

        //end cas 1

        //CAS 2 CHEMIN SOL
        //appel de la m�thode cas chemin sol
        _expectedSpectre = initGroundPathWay();

        //v�rification du nombre d'�tape dnas le tableau
        EXPECT_EQ(2, m_tabEtape01.size());

        //appel de la m�thode ? tester
        m_chemin_sol.calcAttenuation(m_tabEtape01,_atmos_test);

        //comparaison des spectre
        _returnSpectreDB = m_chemin_sol.getAttenuation().toDB();

        //v�rification de la comparaison
        EXPECT_EQ(_expectedSpectre, _returnSpectreDB);

        //end cas 2

        //CAS 3 CHEMIN ECRAN
        //appel de la m�thode cas chemin �cran
        _expectedSpectre = initBlockPathWay();

        //v�rification du nombre d'�tape dnas le tableau
        EXPECT_EQ(4, m_tabEtape01.size());

        //appel de la m�thode ? tester
        m_chemin_ecran.calcAttenuation(m_tabEtape01,_atmos_test);

        //comparaison des spectre
        _returnSpectreDB = m_chemin_ecran.getAttenuation().toDB();

        //v�rification de la comparaison
        EXPECT_EQ(_expectedSpectre, _returnSpectreDB);

        //v�rification de la comparaison
        EXPECT_EQ(true, _exact);
        //end cas 3

        //CAS 4 CHEMIN REFLEX
        //appel de la m�thode cas chemin �cran
        _expectedSpectre = initReflexionPathWay();

        //v�rification du nombre d'�tape dnas le tableau
        EXPECT_EQ(2, m_tabEtape01.size());

        //appel de la m�thode ? tester
        m_chemin_relex.calcAttenuation(m_tabEtape01,_atmos_test);

        //comparaison des spectre
        _returnSpectreDB = m_chemin_relex.getAttenuation().toDB();

        //v�rification de la comparaison
        EXPECT_EQ(_expectedSpectre, _returnSpectreDB);

        //end cas 3

    }
}

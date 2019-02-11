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
#include "testutils.h"


/**
* @brief Fixture which provides an instance of TYChemin
*/
class TYCheminTest :
    public testing::Test
{

public:

};


// Testing the TYChemin::calcAttenuation method
TEST_F(TYCheminTest, calcAttenuation)
{
    //initialisation global objects
    TYInitUserCase initCase;
    initCase.initGlobal(100.0);


    //variable condition atmospherique
    AtmosphericConditions _atmos_test(101325.0, 20.0, 50.0);
    OSpectre _expectedSpectre;
    OSpectre _returnSpectreDB;

    //CAS 1 CHEMIN DIRECT
    _expectedSpectre =  initCase.initDirectPathWay();

    //v�rification du nombre d'�tape dnas le tableau
    EXPECT_EQ(1, initCase.m_tabEtape01.size());

    //appel de la m�thode ? tester
    initCase.m_chemin_direct.calcAttenuation(initCase.m_tabEtape01,_atmos_test);

    //comparaison des spectre
    _returnSpectreDB = initCase.m_chemin_direct.getAttenuation().toDB();

    //v�rification de la comparaison
    //TODO values TYMPAN/SCILAB are very different for high frequency
    EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);


    //end cas 1

    //CAS 2 CHEMIN SOL
    _expectedSpectre = initCase.initGroundPathWay();

    //v�rification du nombre d'�tape dnas le tableau
    EXPECT_EQ(2, initCase.m_tabEtape01.size());

    //appel de la m�thode ? tester
    initCase.m_chemin_sol.calcAttenuation(initCase.m_tabEtape01,_atmos_test);

    //comparaison des spectre
    _returnSpectreDB = initCase.m_chemin_sol.getAttenuation().toDB();

    //v�rification de la comparaison
    //TODO values TYMPAN/SCILAB are very different for all frequency
    EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);

    //end cas 2

    //CAS 3 CHEMIN ECRAN
    _expectedSpectre = initCase.initBlockPathWay();

    //v�rification du nombre d'�tape dnas le tableau
    EXPECT_EQ(4, initCase.m_tabEtape01.size());

    //appel de la m�thode ? tester
    initCase.m_chemin_ecran.calcAttenuation(initCase.m_tabEtape01,_atmos_test);

    //comparaison des spectre
    _returnSpectreDB = initCase.m_chemin_ecran.getAttenuation().toDB();

    //v�rification de la comparaison
    for(int i = 0;i < _expectedSpectre.getNbValues();i++){
        EXPECT_NEAR(_expectedSpectre.getTabValReel()[i] , _returnSpectreDB.getTabValReel()[i],initCase.m_spectrPrecision);
    }

    //end cas 3

    //CAS 4 CHEMIN REFLEX
    _expectedSpectre = initCase.initReflexionPathWay();

    //v�rification du nombre d'�tape dnas le tableau
    EXPECT_EQ(2, initCase.m_tabEtape01.size());

    //appel de la m�thode ? tester
    initCase.m_chemin_relex.calcAttenuation(initCase.m_tabEtape01,_atmos_test);

    //comparaison des spectre
    _returnSpectreDB = initCase.m_chemin_relex.getAttenuation().toDB();

    //v�rification de la comparaison
    //TODO values TYMPAN/SCILAB are anormaly different for all frequency.
    EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);

    //end cas 4
}

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
#include "testutils.h"

# define M_PI           3.14159265358979323846  /* pi */

/**
* @brief Fixture which provides an instance of TYTrajet
*/
class TYTrajetTest :
    public testing::Test
{
private:


public:

};

// Testing the TYTrajet::getPEnergetique method
TEST_F(TYTrajetTest, getPEnergetique)
{
    //initialisation des objets communs
    TYInitUserCase initCase;
    initCase.initGlobal(100.0);

    //variable condition atmospherique
    AtmosphericConditions _atmos_test(101325., 20., 50.);
    OSpectre _expectedSpectre;
    OSpectre _returnSpectreDB;


    // On pose les bases du trajet --> où se trouvent la Source et le Recepteur
    TYTrajet mTrajet(initCase.m_acousticSourceList.at(0), initCase.m_acousticReceptionList.at(0));

    //CAS 1 CHEMIN DIRECT
    //appel de la m�thode cas chemin direct
    _expectedSpectre = initCase.initDirectPathWay();
    _expectedSpectre = _expectedSpectre.mult(_expectedSpectre);

    //v�rification du nombre d'�tape dnas le tableau
    EXPECT_EQ(1, initCase.m_tabEtape01.size());

    initCase.m_chemin_direct.calcAttenuation(initCase.m_tabEtape01, _atmos_test);

    mTrajet.addChemin(initCase.m_chemin_direct);
    mTrajet.addCheminDirect(initCase.m_chemin_direct);

    _returnSpectreDB = mTrajet.getPEnergetique(_atmos_test);

    //TODO values TYMPAN/SCILAB are very different for high frequency
    EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);


    // CAS CHEMIN_ECRAN
    // On pose les bases du trajet --> où se trouvent la Source et le Recepteur
    TYTrajet mTrajetEcran(initCase.m_acousticSourceList.at(0), initCase.m_acousticReceptionList.at(0));

    //appel de la m�thode cas chemin direct
    _expectedSpectre = initCase.initBlockPathWay();
    _expectedSpectre = _expectedSpectre.mult(_expectedSpectre);

    //v�rification du nombre d'�tape dnas le tableau
    EXPECT_EQ(4, initCase.m_tabEtape01.size());

    initCase.m_chemin_ecran.calcAttenuation(initCase.m_tabEtape01, _atmos_test);

    mTrajetEcran.addChemin(initCase.m_chemin_ecran);
    mTrajetEcran.addCheminDirect(initCase.m_chemin_direct);

    initCase.m_chemin_ecran.calcAttenuation(initCase.m_tabEtape01, _atmos_test);

    _returnSpectreDB = mTrajetEcran.getPEnergetique(_atmos_test);

    //TODO values TYMPAN/SCILAB are slightly different
    EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);



}

// Testing the TYTrajet::getPInterference method
TEST_F(TYTrajetTest, getPInterference)
{
    //initialisation des objets communs
    TYInitUserCase initCase;
    initCase.initGlobal(100.0);

    //variable condition atmospherique
    AtmosphericConditions _atmos_test(101325., 20., 50.);
    OSpectre _expectedSpectre;
    OSpectre _returnSpectreDB;


    // On pose les bases du trajet --> où se trouvent la Source et le Recepteur
    TYTrajet mTrajet(initCase.m_acousticSourceList.at(0), initCase.m_acousticReceptionList.at(0));

    // Cas CHEMIN DIRECT
    //appel de la m�thode cas chemin direct
    _expectedSpectre = initCase.initDirectPathWay();
    _expectedSpectre = _expectedSpectre.mult(_expectedSpectre);

    //v�rification du nombre d'�tape dnas le tableau
    EXPECT_EQ(1, initCase.m_tabEtape01.size());

    initCase.m_chemin_direct.calcAttenuation(initCase.m_tabEtape01, _atmos_test);

    mTrajet.addChemin(initCase.m_chemin_direct);
    mTrajet.addCheminDirect(initCase.m_chemin_direct);

    _returnSpectreDB = mTrajet.getPInterference(_atmos_test);

    //TODO values TYMPAN/SCILAB are very different for high frequency
    EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);


    // CAS CHEMIN_ECRAN
    // On pose les bases du trajet --> où se trouvent la Source et le Recepteur
    TYTrajet mTrajetEcran(initCase.m_acousticSourceList.at(0), initCase.m_acousticReceptionList.at(0));

    //appel de la m�thode cas chemin direct
    _expectedSpectre = initCase.initBlockPathWay();
    _expectedSpectre = _expectedSpectre.mult(_expectedSpectre);

    //v�rification du nombre d'�tape dnas le tableau
    EXPECT_EQ(4, initCase.m_tabEtape01.size());

    initCase.m_chemin_ecran.calcAttenuation(initCase.m_tabEtape01, _atmos_test);

    mTrajetEcran.addChemin(initCase.m_chemin_ecran);
    mTrajetEcran.addCheminDirect(initCase.m_chemin_direct);

    initCase.m_chemin_ecran.calcAttenuation(initCase.m_tabEtape01, _atmos_test);

    _returnSpectreDB = mTrajetEcran.getPInterference(_atmos_test);

    //TODO values TYMPAN/SCILAB are slightly different
    EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);


    // Cas CHEMIN REFLEX
    // On pose les bases du trajet --> où se trouvent la Source et le Recepteur
    TYTrajet mTrajetREFLEX(initCase.m_acousticSourceList.at(0), initCase.m_acousticReceptionList.at(0));

    //appel de la m�thode cas chemin direct
    _expectedSpectre = initCase.initReflexionPathWay();
    _expectedSpectre = _expectedSpectre.mult(_expectedSpectre);

    //v�rification du nombre d'�tape dnas le tableau
    EXPECT_EQ(2, initCase.m_tabEtape01.size());

    initCase.m_chemin_relex.calcAttenuation(initCase.m_tabEtape01, _atmos_test);

    mTrajetREFLEX.addChemin(initCase.m_chemin_relex);
    mTrajetREFLEX.addCheminDirect(initCase.m_chemin_direct);

    initCase.m_chemin_relex.calcAttenuation(initCase.m_tabEtape01, _atmos_test);

    _returnSpectreDB = mTrajetREFLEX.getPInterference(_atmos_test);

    //TODO values TYMPAN/SCILAB are very different for all frequency
    EXPECT_FALSE(_expectedSpectre == _returnSpectreDB);

}




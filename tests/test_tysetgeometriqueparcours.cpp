/**
*
* @brief Functional tests of the TYSetGeometriqueParcours class  
* 
*  Created on: january 18, 2018 
*  Author: Philippe CHAPUIS <philippe.chapuis@c-s.fr> 
*
*/
#include "gtest/gtest.h"
#include "Tympan/geometric_methods/ConvexHullFinder/TYSetGeometriqueParcours.h"
#include <iostream>

/**
* @brief Fixture which provides an instance of TYSetGeometriqueParcours
*/
class TYSetGeometriqueParcoursTest :
    public testing::Test
{
    private:
        TYSetGeometriqueParcours SetGeometriqueParcours;

    public:
        TYSetGeometriqueParcours geoParcoursSR, geoParcours;
        TYPointParcours S, R;
        TYPointParcours P1, P2, P3, P4, P5, P6;
        TYPolyligneParcours polyligneSR;
        TYPolyligneParcours polyligneP1, polyligneP2, polyligneP3, polyligneP4, polyligneP5, polyligneP6, polyligneP7;

    protected:

        virtual void SetUp()
        {
            // Parcours de Points
            S.Identifiant = 0;
            S.x = 0.0;
            S.y = 0.0;
            S.isEcran = S.isInfra = true;

            R.Identifiant = 1;
            R.x = 10.0;
            R.y = 10.0;
            R.isEcran = R.isInfra = true;

            P1.Identifiant = 0;
            P1.x = 1.0;
            P1.y = 3.0;
            P1.isEcran = P1.isInfra = true;

            P2.Identifiant = 1;
            P2.x = 3.0;
            P2.y = 3.0;
            P2.isEcran = P2.isInfra = true;

            P3.Identifiant = 2;
            P3.x = 6.0;
            P3.y = 4.0;
            P3.isEcran = P3.isInfra = true;

            P4.Identifiant = 3;
            P4.x = 4.0;
            P4.y = 7.0;
            P4.isEcran = P4.isInfra = true;

            P5.Identifiant = 4;
            P5.x = 7.0;
            P5.y = 7.0;
            P5.isEcran = P5.isInfra = true;

            P6.Identifiant = 5;
            P6.x = 10.0;
            P6.y = 7.0;
            P6.isEcran = P6.isInfra = true;

            // Polylignes
            polyligneSR.ajouteSegment(&S, &R);
            polyligneP2.ajouteSegment(&P1, &P2);
            polyligneP3.ajouteSegment(&P2, &P3);
            polyligneP4.ajouteSegment(&P3, &P4);
            polyligneP5.ajouteSegment(&P4, &P5);
            polyligneP6.ajouteSegment(&P5, &P6);

            // Geometrique Parcours
            geoParcoursSR._nNbPointTotal = 2;
            geoParcoursSR._nNbPolylines = 1;
            geoParcoursSR._ListePoint = new TYPointParcours[2];
            geoParcoursSR._ListePoint[0] = S;
            geoParcoursSR._ListePoint[1] = R;
            geoParcoursSR._ListePolylines = new TYPolyligneParcours[1];
            geoParcoursSR._ListePolylines[0] = polyligneSR;

            geoParcours._nNbPointTotal = 6;
            geoParcours._nNbPolylines = 5;
            geoParcours._ListePoint = new TYPointParcours[6];
            geoParcours._ListePoint[0] = P1;
            geoParcours._ListePoint[1] = P2;
            geoParcours._ListePoint[2] = P3;
            geoParcours._ListePoint[3] = P4;
            geoParcours._ListePoint[4] = P5;
            geoParcours._ListePoint[5] = P6;
            geoParcours._ListePolylines = new TYPolyligneParcours[5];
            geoParcours._ListePolylines[0]=polyligneP2;
            geoParcours._ListePolylines[1]=polyligneP3;
            geoParcours._ListePolylines[2]=polyligneP4;
            geoParcours._ListePolylines[3]=polyligneP5;
            geoParcours._ListePolylines[4]=polyligneP6;

        }

        virtual void TearDown()
        {
            geoParcours._ListePolylines = nullptr;
            geoParcours._ListePoint = nullptr;
            geoParcours._nNbPointTotal = 0;
            geoParcours._nNbPolylineAllouee = 0;
            geoParcours._nNbPolylines = 0;

            geoParcoursSR._ListePolylines = nullptr;
            geoParcoursSR._ListePoint = nullptr;
            geoParcoursSR._nNbPointTotal = 0;
            geoParcoursSR._nNbPolylineAllouee = 0;
            geoParcoursSR._nNbPolylines = 0;


        }
};

TEST_F(TYSetGeometriqueParcoursTest, create_instance)
{
}

TEST_F(TYSetGeometriqueParcoursTest, IsConstruct)
{
    //EXPECT_EQ(0, geoParcoursSR._ListePolylines[0].indexePoint1());
    //EXPECT_EQ(1, geoParcoursSR._ListePolylines[0].indexePoint2());
}


// DONE
// Testing the TYSetGeometriqueParcours::Copy method
TEST_F(TYSetGeometriqueParcoursTest, Copy)
{
    TYSetGeometriqueParcours copieGeoParcoursP0;

    // Call to the tested method:
    copieGeoParcoursP0.Copy(geoParcoursSR);

    EXPECT_EQ(true, (copieGeoParcoursP0._nNbPointTotal == geoParcoursSR._nNbPointTotal));
    EXPECT_EQ(true, (copieGeoParcoursP0._nNbPolylines == geoParcoursSR._nNbPolylines));

}

// DONE
// Testing the TYSetGeometriqueParcours::SupressionPolylignesRedondantes method
TEST_F(TYSetGeometriqueParcoursTest, SupressionPolylignesRedondantes)
{
    // Data pour le test redondant
    TYPointParcours t_S;
    t_S.Identifiant = 0;
    t_S.isEcran = t_S.isInfra = true;
    t_S.x = 2.0;
    t_S.y = 3.0;

    TYPointParcours t_R;
    t_R.Identifiant = 1;
    t_R.x = 4.0;
    t_R.y = 5.0;
    t_R.isEcran = t_R.isInfra = true;

    TYPointParcours t_P1;
    t_P1.Identifiant = 2;
    t_P1.isEcran = t_P1.isInfra = true;
    t_P1.x = 3.0;
    t_P1.y = 4.0;

    TYPolyligneParcours polyligneR1;
    polyligneR1.ajouteSegment(&t_S, &t_R);
    TYPolyligneParcours polyligneR2;
    polyligneR2.ajouteSegment(&t_R, &t_P1);
    TYPolyligneParcours polyligneR3;
    polyligneR3.ajouteSegment(&t_P1, &t_R);

    polyligneR1._PolyligneP1 = &polyligneR2;
    polyligneR2._PolyligneP0 = &polyligneR1;
    polyligneR2._PolyligneP1 = &polyligneR3;
    polyligneR3._PolyligneP0 = &polyligneR2;
    polyligneR3._PolyligneP1 = &polyligneR2;

    TYSetGeometriqueParcours geoParcoursRevient;
    geoParcoursRevient._nNbPolylines = 3;
    geoParcoursRevient._ListePolylines = new TYPolyligneParcours[3];
    geoParcoursRevient._ListePolylines[0] = polyligneR1;
    geoParcoursRevient._ListePolylines[1] = polyligneR2;
    geoParcoursRevient._ListePolylines[2] = polyligneR3;
    geoParcoursRevient._nNbPointTotal = 3;
    geoParcoursRevient._ListePoint = new TYPointParcours[3];
    geoParcoursRevient._ListePoint[0] = t_S;
    geoParcoursRevient._ListePoint[1] = t_R;
    geoParcoursRevient._ListePoint[2] = t_P1;

    EXPECT_EQ(3,geoParcoursRevient._nNbPolylines);

    // Call to the tested method:
    int nRevient = geoParcoursRevient.SupressionPolylignesRedondantes();

    EXPECT_EQ(1, nRevient);
    EXPECT_EQ(2, geoParcoursRevient._nNbPolylines);
    EXPECT_EQ(true, geoParcoursRevient._ListePolylines[0]._PolyligneP1 == &polyligneR2);

    geoParcoursRevient._ListePolylines = nullptr;
    geoParcoursRevient._ListePoint = nullptr;
    geoParcoursRevient._nNbPointTotal = 0;
    geoParcoursRevient._nNbPolylineAllouee = 0;
    geoParcoursRevient._nNbPolylines = 0;


}

// DONE
// Testing the TYSetGeometriqueParcours::MergePointsDoubles method
TEST_F(TYSetGeometriqueParcoursTest, MergePointsDoubles)
{


    // Points
    TYPointParcours P10,P11, P12,P11_bis,P13;
    P10.Identifiant = 0;
    P10.x = 1.0;
    P10.y = 3.0;
    P10.isEcran = P10.isInfra = true;

    P11.Identifiant = 1;
    P11.x = 3.0;
    P11.y = 3.0;
    P11.isEcran = P11.isInfra = true;

    P11_bis.Identifiant = 2;
    P11_bis.x = 3.0;
    P11_bis.y = 3.0;
    P11_bis.isEcran = P11_bis.isInfra = true;

    P12.Identifiant = 3;
    P12.x = 3.0;
    P12.y = 6.0;
    P12.isEcran = P12.isInfra = true;

    P13.Identifiant = 3;
    P13.x = 7.0;
    P13.y = 3.0;
    P13.isEcran = P13.isInfra = true;



    TYPolyligneParcours polyligneMP1;
    polyligneMP1.ajouteSegment(&P10, &P11);
    TYPolyligneParcours polyligneMP2;
    polyligneMP2.ajouteSegment(&P11, &P12);
    TYPolyligneParcours polyligneMP3;
    polyligneMP3.ajouteSegment(&P11_bis, &P13);


    polyligneMP1._PolyligneP1 = &polyligneMP2;
    polyligneMP2._PolyligneP0 = &polyligneMP1;
    polyligneMP3._PolyligneP0 = &polyligneMP2;


    // Geometriques Parcours
    TYSetGeometriqueParcours geoParcoursMP;
    geoParcoursMP._nNbPointTotal = 5;
    geoParcoursMP._nNbPolylines = 3;
    geoParcoursMP._ListePolylines = new TYPolyligneParcours[3];
    geoParcoursMP._ListePolylines[0] = polyligneMP1;
    geoParcoursMP._ListePolylines[1] = polyligneMP2;
    geoParcoursMP._ListePolylines[2] = polyligneMP3;
    geoParcoursMP._ListePoint = new TYPointParcours[5];
    geoParcoursMP._ListePoint[0] = P10;
    geoParcoursMP._ListePoint[1] = P11;
    geoParcoursMP._ListePoint[2] = P11_bis;
    geoParcoursMP._ListePoint[3] = P12;
    geoParcoursMP._ListePoint[4] = P13;

    // Check before result:
    EXPECT_EQ(5, geoParcoursMP._nNbPointTotal);
    EXPECT_EQ(3, geoParcoursMP._nNbPolylines);

    // Call to the tested method:
    int nDoublons = geoParcoursMP.MergePointsDoubles();

    // Check result:
    EXPECT_EQ(1, nDoublons);
    EXPECT_EQ(5, geoParcoursMP._nNbPointTotal);
    EXPECT_EQ(3, geoParcoursMP._nNbPolylines);

    geoParcoursMP._ListePolylines = nullptr;
    geoParcoursMP._ListePoint = nullptr;
    geoParcoursMP._nNbPointTotal = 0;
    geoParcoursMP._nNbPolylineAllouee = 0;
    geoParcoursMP._nNbPolylines = 0;


}


// DONE
// Testing the TYSetGeometriqueParcours::RamenerPointsTraversantLaFrontiere method
TEST_F(TYSetGeometriqueParcoursTest, RamenerPointsTraversantLaFrontiere)
{
    // Data setup:
    bool* PointAGauche;
    bool* PointADroite;
    int* IndexPointFrontiere = new int[100];
    int NbPointFrontiere = 0;
    bool* EstUnPointIntersectant = new bool[100];
    bool bCoteGauche = true;


   geoParcours.MarquePointsADroiteEtAGauche(S, R, PointAGauche, PointADroite);

   // Call to the tested method:
   geoParcours.RamenerPointsTraversantLaFrontiere(S, R, IndexPointFrontiere, NbPointFrontiere, EstUnPointIntersectant, bCoteGauche, PointAGauche, PointADroite);
   
   // Check result:
   EXPECT_EQ(5, NbPointFrontiere);
   EXPECT_EQ(1, IndexPointFrontiere[0]);
   EXPECT_EQ(1, IndexPointFrontiere[1]);
   EXPECT_EQ(3, IndexPointFrontiere[2]);
   EXPECT_EQ(4, IndexPointFrontiere[3]);

   // Destruction
   delete PointADroite;
   delete PointAGauche;
   delete IndexPointFrontiere;
   delete EstUnPointIntersectant;
}

// DONE
// Testing the TYSetGeometriqueParcours::MarquePointsADroiteEtAGauche method
TEST_F(TYSetGeometriqueParcoursTest, MarquePointsADroiteEtAGauche)
{
    // Data setup:
    bool* PointAGauche;
    bool* PointADroite;


   // Call to the tested method:
   geoParcours.MarquePointsADroiteEtAGauche(S, R, PointAGauche, PointADroite);
   

   // Check result:

   EXPECT_EQ(false, PointADroite[0]);
   EXPECT_EQ(true, PointAGauche[0]);
   EXPECT_EQ(false, PointADroite[1]);
   EXPECT_EQ(false, PointAGauche[1]);
   EXPECT_EQ(true, PointADroite[2]);
   EXPECT_EQ(false, PointAGauche[2]);
   EXPECT_EQ(false, PointADroite[3]);
   EXPECT_EQ(true, PointAGauche[3]);
   EXPECT_EQ(false, PointADroite[4]);
   EXPECT_EQ(false, PointAGauche[4]);
   EXPECT_EQ(true, PointADroite[5]);
   EXPECT_EQ(false, PointAGauche[5]);


   // Destruction
   delete PointADroite;
   delete PointAGauche;
}

// DONE
// Testing the TYSetGeometriqueParcours::SeparationDroiteGauche method
TEST_F(TYSetGeometriqueParcoursTest, SeparationDroiteGauche)
{
    // Data setup:
    // Geometrique Parcours
    TYSetGeometriqueParcours  geoGauche, geoDroite;

    bool* PointAGauche;
    bool* PointADroite;

    // Call to the tested method:
    geoParcours.MarquePointsADroiteEtAGauche(S, R, PointAGauche, PointADroite);
    geoParcours.SeparationDroiteGauche(PointAGauche, PointADroite, geoGauche, geoDroite);

    // Check result:

    // DROITE
    EXPECT_EQ(3, geoDroite._nNbPolylines);
    // Pour la 1ère Polyligne Droite
    EXPECT_EQ(1, geoDroite._ListePolylines[0].indexePoint1());
    EXPECT_EQ(2, geoDroite._ListePolylines[0].indexePoint2());
    // Pour la 2ème Polyligne Droite
    EXPECT_EQ(2, geoDroite._ListePolylines[1].indexePoint1());
    EXPECT_EQ(3, geoDroite._ListePolylines[1].indexePoint2());
    // Pour la 3ème Polyligne Droite
    EXPECT_EQ(4, geoDroite._ListePolylines[2].indexePoint1());
    EXPECT_EQ(5, geoDroite._ListePolylines[2].indexePoint2());


    // GAUCHE
    EXPECT_EQ(3, geoGauche._nNbPolylines);
    // Pour la 1ère Polyligne Gauche
    EXPECT_EQ(0, geoGauche._ListePolylines[0].indexePoint1());
    EXPECT_EQ(1, geoGauche._ListePolylines[0].indexePoint2());
    // Pour la 2ème Polyligne Gauche
    EXPECT_EQ(2, geoGauche._ListePolylines[1].indexePoint1());
    EXPECT_EQ(3, geoGauche._ListePolylines[1].indexePoint2());
    // Pour la 3ème Polyligne Gauche
    EXPECT_EQ(3, geoGauche._ListePolylines[2].indexePoint1());
    EXPECT_EQ(4, geoGauche._ListePolylines[2].indexePoint2());


    // Destruction

    delete PointADroite;
    delete PointAGauche;
}

// DONE
// Testing the TYSetGeometriqueParcours::TriePointsIntersectionSuivantSR method
TEST_F(TYSetGeometriqueParcoursTest, TriePointsIntersectionSuivantSR)
{
    // Data setup
    bool* PointsAGauche;
    bool* PointsADroite;
    int* IndexePointsFrontiere = new int[geoParcours._nNbPointTotal];
    int NbPointFrontiere = 0;
    bool* EstUnPointIntersectant = new bool[geoParcours._nNbPointTotal];

    // Call to the tested method:
    geoParcours.MarquePointsADroiteEtAGauche(S, R, PointsADroite, PointsAGauche);
    geoParcours.RamenerPointsTraversantLaFrontiere(S, R, IndexePointsFrontiere, NbPointFrontiere, EstUnPointIntersectant, false, PointsAGauche, PointsADroite);

    geoParcours.TriePointsIntersectionSuivantSR(S, R, IndexePointsFrontiere, NbPointFrontiere);

    // Check result:
    EXPECT_EQ(5 ,NbPointFrontiere);
    EXPECT_EQ(3 ,IndexePointsFrontiere[0]);
    EXPECT_EQ(1 ,IndexePointsFrontiere[1]);
    EXPECT_EQ(1 ,IndexePointsFrontiere[2]);
    EXPECT_EQ(4 ,IndexePointsFrontiere[3]);
    EXPECT_EQ(4 ,IndexePointsFrontiere[4]);


    // Destruction
    delete PointsADroite;
    delete PointsAGauche;
    delete IndexePointsFrontiere;
    delete EstUnPointIntersectant;
}

// DONE
// Testing the TYSetGeometriqueParcours::AjoutePointALaPolyLigne method
TEST_F(TYSetGeometriqueParcoursTest, AjoutePointALaPolyLigne)
{

    // Data setup:
    TYPointParcours PS,PR,Padd;

    PS.Identifiant = 0;
    PS.x = 1.0;
    PS.y = 7.0;
    PS.isEcran = PS.isInfra = true;

    PR.Identifiant = 1;
    PR.x = 8.0;
    PR.y = 7.0;
    PR.isEcran = PR.isInfra = true;

    Padd.Identifiant = 2;
    Padd.x = 10.0;
    Padd.y = 7.0;
    Padd.isEcran = Padd.isInfra = true;


    // Polylignes
    TYPolyligneParcours polyligneP0;
    polyligneP0.ajouteSegment(&PS, &PR);

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcoursP;
    geoParcoursP._nNbPolylines = 1;
    geoParcoursP._ListePolylines = new TYPolyligneParcours[1];
    geoParcoursP._ListePolylines[0] = polyligneP0;
    geoParcoursP._nNbPointTotal = 2;
    geoParcoursP._ListePoint = new TYPointParcours[100];
    geoParcoursP._ListePoint[0] = PS;
    geoParcoursP._ListePoint[1] = PR;

    // Call to the tested method:
    bool t_ajout = geoParcoursP.AjoutePointALaPolyLigne(0, Padd);

    // Check result:
    EXPECT_EQ(true, t_ajout);
    EXPECT_EQ(3, geoParcoursP._nNbPointTotal);
    EXPECT_EQ(2, geoParcoursP._ListePolylines[0].indexePoint(2));

    // Destruction
    geoParcoursP._ListePolylines = nullptr;
    geoParcoursP._ListePoint = nullptr;
    geoParcoursP._nNbPointTotal = 0;
    geoParcoursP._nNbPolylineAllouee = 0;
    geoParcoursP._nNbPolylines = 0;

}

// DONE
// Testing the TYSetGeometriqueParcours::AjouteLesPointsComprisEntre method
TEST_F(TYSetGeometriqueParcoursTest, AjouteLesPointsComprisEntre)
{
    // Data setup:
    TYPointParcours PS,PR,Padd;

    PS.Identifiant = 0;
    PS.x = 1.0;
    PS.y = 7.0;
    PS.isEcran = PS.isInfra = true;

    PR.Identifiant = 1;
    PR.x = 8.0;
    PR.y = 7.0;
    PR.isEcran = PR.isInfra = true;

    Padd.Identifiant = 2;
    Padd.x = 10.0;
    Padd.y = 7.0;
    Padd.isEcran = Padd.isInfra = true;


    // Polylignes
    TYPolyligneParcours polyligneP0;
    polyligneP0.ajouteSegment(&PS, &PR);

    TYSetGeometriqueParcours geoAjoute;
    geoAjoute._nNbPointTotal = 3;
    geoAjoute._nNbPolylines = 1;
    geoAjoute._ListePoint = new TYPointParcours[4];
    geoAjoute._ListePoint[0] = PS;
    geoAjoute._ListePoint[1] = PR;
    geoAjoute._ListePoint[2] = Padd;
    geoAjoute._ListePolylines = new TYPolyligneParcours[1];
    geoAjoute._ListePolylines[0] = polyligneP0;

    // Call to the tested method
    int PointsAjoutes = geoAjoute.AjouteLesPointsComprisEntre(geoParcours, 0, 0, 2);

    // Check result:
    EXPECT_EQ(1, PointsAjoutes);
    EXPECT_EQ(1, geoAjoute._ListePolylines[0].indexePoint(2));

    // Destruction
    geoAjoute._ListePolylines = nullptr;
    geoAjoute._ListePoint = nullptr;
    geoAjoute._nNbPointTotal = 0;
    geoAjoute._nNbPolylineAllouee = 0;
    geoAjoute._nNbPolylines = 0;
}

// DONE
// Testing the TYSetGeometriqueParcours::ParcourtPolyligneAPartirDe method
TEST_F(TYSetGeometriqueParcoursTest, ParcourtPolyligneAPartirDe)
{
    // Data setup:
    TYSetGeometriqueParcours geoPremierePasse;
    geoPremierePasse._ListePolylines    = new TYPolyligneParcours[1];
    geoPremierePasse._ListePolylines[0].allouer(4 * geoParcours._nNbPointTotal);
    geoPremierePasse._ListePoint        = new TYPointParcours[4 * geoParcours._nNbPointTotal];
    geoPremierePasse._nNbPointTotal = 0;
    geoPremierePasse._nNbPolylines = 1;

    bool* PointsAGauche;
    bool* PointsADroite;
    int* IndexePointsFrontiere = new int[geoParcours._nNbPointTotal];
    int NbPointFrontiere = 0;
    bool* EstUnPointIntersectant = new bool[geoParcours._nNbPointTotal];

    geoParcours.MarquePointsADroiteEtAGauche(S, R, PointsAGauche, PointsADroite);
    geoParcours.RamenerPointsTraversantLaFrontiere(S, R, IndexePointsFrontiere, NbPointFrontiere, EstUnPointIntersectant, true, PointsAGauche, PointsADroite);

    Connexite* Connexes = new Connexite[geoParcours._nNbPointTotal];

    // Call to the tested method:
    geoParcours.ListerPointsConnexes(Connexes);

    TYPolyligneParcours* PolyligneCourante = &(geoParcours._ListePolylines[0]);

    // Call to the tested method:
    int IndexPoint = geoParcours.ParcourtPolyligneAPartirDe(0, PolyligneCourante, EstUnPointIntersectant, geoPremierePasse);

    // Check result:
    EXPECT_EQ(1, geoPremierePasse._nNbPointTotal);
    EXPECT_EQ(1, geoPremierePasse._nNbPolylines);
    EXPECT_EQ(1, IndexPoint);
    EXPECT_EQ(1,geoPremierePasse._ListePolylines[0].indexePoint1());

    // Destruction
    geoPremierePasse.~TYSetGeometriqueParcours();
    delete PointsADroite;
    delete PointsAGauche;
    delete IndexePointsFrontiere;
    delete EstUnPointIntersectant;
}

// DONE
// Testing the TYSetGeometriqueParcours::PolylignesInfraFermees method
TEST_F(TYSetGeometriqueParcoursTest, PolylignesInfraFermees)
{
    // Data setup:
    // Points
    TYPointParcours P0,P1,P2,P3,P4;
    P0.Identifiant = 0;
    P0.x = 1.0;
    P0.y = 3.0;
    P0.isInfra = true;
    P0.isEcran = true;

    P1.Identifiant = 1;
    P1.x = 3.0;
    P1.y = 3.0;
    P1.isInfra = true;
    P1.isEcran = true;

    P2.Identifiant = 2;
    P2.x = 3.0;
    P2.y = 1.0;
    P2.isInfra = true;
    P2.isEcran = true;

    P3.Identifiant = 0;
    P3.x = 1.0;
    P3.y = 5.0;
    P3.isInfra = true;
    P3.isEcran = false;

    P4.Identifiant = 1;
    P4.x = 5.0;
    P4.y = 5.0;
    P4.isInfra = true;
    P4.isEcran = false;



    // Polylignes
    TYPolyligneParcours polyligneIF0, polyligneIF1, polyligneIF2,polyligneO1;
    polyligneIF0.ajouteSegment(&P0, &P1);
    polyligneIF1.ajouteSegment(&P1, &P2);
    polyligneIF2.ajouteSegment(&P2, &P0);

    polyligneIF0._PolyligneP1 = &polyligneIF1;
    polyligneIF1._PolyligneP0 = &polyligneIF0;
    polyligneIF1._PolyligneP1 = &polyligneIF2;
    polyligneIF2._PolyligneP0 = &polyligneIF1;

    polyligneIF2._PolyligneP1 = &polyligneIF0;


    polyligneO1.ajouteSegment(&P3, &P4);


    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcoursF;
    geoParcoursF._nNbPolylines = 3;
    geoParcoursF._nNbPointTotal = 3;
    geoParcoursF._ListePolylines = new TYPolyligneParcours[3];
    geoParcoursF._ListePolylines[0] = polyligneIF0;
    geoParcoursF._ListePolylines[1] = polyligneIF1;
    geoParcoursF._ListePolylines[2] = polyligneIF2;
    geoParcoursF._ListePoint = new TYPointParcours[3];
    geoParcoursF._ListePoint[0] = P0;
    geoParcoursF._ListePoint[1] = P1;
    geoParcoursF._ListePoint[2] = P2;

    P1.isEcran = true;
    P2.isEcran = true;

    TYSetGeometriqueParcours geoParcoursO;
    geoParcoursO._nNbPolylines = 1;
    geoParcoursO._nNbPointTotal = 2;
    geoParcoursO._ListePolylines = new TYPolyligneParcours[1];
    geoParcoursO._ListePolylines[0] = polyligneO1;
    geoParcoursO._ListePoint = new TYPointParcours[2];
    geoParcoursO._ListePoint[0] = P3;
    geoParcoursO._ListePoint[1] = P4;

    // Call to the tested method:
    bool O_F_SF = geoParcoursF.PolylignesInfraFermees();

    bool O_F_S0 = geoParcoursO.PolylignesInfraFermees();

    // Check result:
    EXPECT_EQ(false, O_F_S0);
    EXPECT_EQ(true, O_F_SF);

    // Destruction
    geoParcoursF._ListePolylines = nullptr;
    geoParcoursF._ListePoint = nullptr;
    geoParcoursF._nNbPointTotal = 0;
    geoParcoursF._nNbPolylineAllouee = 0;
    geoParcoursF._nNbPolylines = 0;

    geoParcoursO._ListePolylines = nullptr;
    geoParcoursO._ListePoint = nullptr;
    geoParcoursO._nNbPointTotal = 0;
    geoParcoursO._nNbPolylineAllouee = 0;
    geoParcoursO._nNbPolylines = 0;


}

// DONE
// Testing the TYSetGeometriqueParcours::ListerPointsConnexes method
TEST_F(TYSetGeometriqueParcoursTest, ListerPointsConnexes)
{
    // Data setup:
    // P1 est lié �  S / P2 / P3 / P4
    // P2 est lié �  P1 / P5 / R
    TYPolyligneParcours polylignePC1;
    polylignePC1.ajouteSegment(&S, &P1);
    TYPolyligneParcours polylignePC2;
    polylignePC2.ajouteSegment(&P1, &P2);
    TYPolyligneParcours polylignePC3;
    polylignePC3.ajouteSegment(&P1, &P3);
    TYPolyligneParcours polylignePC4;
    polylignePC4.ajouteSegment(&P1, &P4);
    TYPolyligneParcours polylignePC5;
    polylignePC5.ajouteSegment(&P2, &P5);
    TYPolyligneParcours polylignePC6;
    polylignePC6.ajouteSegment(&P2, &R);
    TYPolyligneParcours polylignePC7;
    polylignePC7.ajouteSegment(&P5, &P6);
    TYPolyligneParcours polylignePC8;
    polylignePC8.ajouteSegment(&P6, &R);

    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcoursPC;
    geoParcoursPC._nNbPointTotal = 8;
    geoParcoursPC._ListePoint = new TYPointParcours[geoParcoursPC._nNbPointTotal];
    geoParcoursPC._ListePoint[0] = S;
    geoParcoursPC._ListePoint[1] = R;
    geoParcoursPC._ListePoint[2] = P1;
    geoParcoursPC._ListePoint[3] = P2;
    geoParcoursPC._ListePoint[4] = P3;
    geoParcoursPC._ListePoint[5] = P4;
    geoParcoursPC._ListePoint[6] = P5;
    geoParcoursPC._ListePoint[7] = P6;
    geoParcoursPC._nNbPolylines = 8;
    geoParcoursPC._ListePolylines = new TYPolyligneParcours[geoParcoursPC._nNbPolylines];
    geoParcoursPC._ListePolylines[0] = polylignePC1;
    geoParcoursPC._ListePolylines[1] = polylignePC2;
    geoParcoursPC._ListePolylines[2] = polylignePC3;
    geoParcoursPC._ListePolylines[3] = polylignePC4;
    geoParcoursPC._ListePolylines[4] = polylignePC5;
    geoParcoursPC._ListePolylines[5] = polylignePC6;
    geoParcoursPC._ListePolylines[6] = polylignePC7;
    geoParcoursPC._ListePolylines[7] = polylignePC8;

    Connexite* Connexes = new Connexite[geoParcoursPC._nNbPointTotal];

    // Call to the tested method:
    bool t_connexe = geoParcoursPC.ListerPointsConnexes(Connexes);

    // Check result:
    // A Verifier
    EXPECT_EQ(true, t_connexe);
    EXPECT_EQ(2, Connexes->NbSegmentsConnexes);
   // Premier point
    EXPECT_EQ(2, Connexes[0].NbSegmentsConnexes);
    EXPECT_EQ(2, Connexes[1].NbSegmentsConnexes);
    EXPECT_EQ(0, Connexes[2].NbSegmentsConnexes);
    EXPECT_EQ(0, Connexes[3].NbSegmentsConnexes);
    EXPECT_EQ(0, Connexes[4].NbSegmentsConnexes);
    EXPECT_EQ(0, Connexes[5].NbSegmentsConnexes);
    EXPECT_EQ(0, Connexes[6].NbSegmentsConnexes);
    EXPECT_EQ(0, Connexes[7].NbSegmentsConnexes);
    //vérifier contenu des Connexes[i].Index[0] et Index[1]

    // Destruction
    geoParcoursPC._ListePolylines = nullptr;
    geoParcoursPC._ListePoint = nullptr;
    geoParcoursPC._nNbPointTotal = 0;
    geoParcoursPC._nNbPolylineAllouee = 0;
    geoParcoursPC._nNbPolylines = 0;
    delete Connexes;
}

// DONE
// Testing the TYSetGeometriqueParcours::intersects method
TEST_F(TYSetGeometriqueParcoursTest, intersects)
{
    // Data setup:
    TYPolyligneParcours polyligneI1;
    polyligneI1.ajouteSegment(&P2, &P5);
    TYPolyligneParcours polyligneI2;
    polyligneI2.ajouteSegment(&P3, &P4);

    TYSetGeometriqueParcours geoParcoursI;
    geoParcoursI._nNbPointTotal = 5;
    geoParcoursI._ListePoint = new TYPointParcours[geoParcoursI._nNbPointTotal];
    geoParcoursI._ListePoint[0] = P1;
    geoParcoursI._ListePoint[1] = P2;
    geoParcoursI._ListePoint[2] = P3;
    geoParcoursI._ListePoint[3] = P4;
    geoParcoursI._ListePoint[4] = P5;
    geoParcoursI._nNbPolylines = 2;
    geoParcoursI._ListePolylines = new TYPolyligneParcours[2];
    geoParcoursI._ListePolylines[0] = polyligneI1;
    geoParcoursI._ListePolylines[1] = polyligneI2;

    // Call to the tested method:
    bool t_isNotIntersect = geoParcoursI.intersects(P1, P4);
    bool t_isIntersect = geoParcoursI.intersects(P2, P5);

    // Check result
    EXPECT_EQ(false, t_isNotIntersect);
    EXPECT_EQ(true, t_isIntersect);

    // Destruction
    geoParcoursI._ListePolylines = nullptr;
    geoParcoursI._ListePoint = nullptr;
    geoParcoursI._nNbPointTotal = 0;
    geoParcoursI._nNbPolylineAllouee = 0;
    geoParcoursI._nNbPolylines = 0;
}

// DONE
// Testing the TYSetGeometriqueParcours::AppartienneMemePolyligne method
TEST_F(TYSetGeometriqueParcoursTest, AppartienneMemePolyligne)
{
    // Data setup:
    // Polylignes

    TYPolyligneParcours polyligneA1;
    polyligneA1.ajouteSegment(&P1, &P2);
    TYPointParcours POnTheLine,POutOffLine;
    POnTheLine.Identifiant = 3;
    POnTheLine.x = 2.0;
    POnTheLine.y = 3.0;
    polyligneA1.ajoutePoint(2,&POnTheLine);

    POnTheLine.isEcran = POnTheLine.isInfra = true;
    POutOffLine.Identifiant = 3;
    POutOffLine.x = 2.0;
    POutOffLine.y = 5.0;
    POutOffLine.isEcran = POutOffLine.isInfra = true;


    // Geometrique parcours
    TYSetGeometriqueParcours geoParcoursA;
    geoParcoursA._nNbPointTotal = 4;
    geoParcoursA._ListePoint = new TYPointParcours[4];
    geoParcoursA._ListePoint[0] = P1;
    geoParcoursA._ListePoint[1] = P2;
    geoParcoursA._ListePoint[2] = POnTheLine;
    geoParcoursA._ListePoint[3] = POutOffLine;
    geoParcoursA._nNbPolylines = 1;
    geoParcoursA._ListePolylines = new TYPolyligneParcours[1];
    geoParcoursA._ListePolylines[0] = polyligneA1;

    // Call to the tested method:
    bool t_samePolyline = geoParcoursA.AppartienneMemePolyligne(&P1,&P2,&POnTheLine);
    bool t_noteSamePolyline = geoParcoursA.AppartienneMemePolyligne(&P1,&P2,&POutOffLine);

    // Check result:
    EXPECT_EQ(true, t_samePolyline);
    EXPECT_EQ(false, t_noteSamePolyline);

    // Destruction
    geoParcoursA._ListePolylines = nullptr;
    geoParcoursA._ListePoint = nullptr;
    geoParcoursA._nNbPointTotal = 0;
    geoParcoursA._nNbPolylineAllouee = 0;
    geoParcoursA._nNbPolylines = 0;
}

// DONE
// Testing the TYSetGeometriqueParcours::SelectionnePointsEntreSetRetDuCoteDeSR method
TEST_F(TYSetGeometriqueParcoursTest, SelectionnePointsEntreSetRetDuCoteDeSR)
{

    // Pour Trajet
    bool* PointAGauche;
    bool* PointADroite;
    int* IndexPointFrontiere = new int[100];
    int NbPointFrontiere = 0;
    bool* EstUnPointIntersectant = new bool[100];
    bool bCoteGauche = true;

    geoParcours.MarquePointsADroiteEtAGauche(S, R, PointAGauche, PointADroite);
    geoParcours.RamenerPointsTraversantLaFrontiere(S, R, IndexPointFrontiere, NbPointFrontiere, EstUnPointIntersectant, bCoteGauche, PointAGauche, PointADroite);
    geoParcours.TriePointsIntersectionSuivantSR(S, R, IndexPointFrontiere, NbPointFrontiere);

    TYPointParcours** TableauDePoints = new TYPointParcours*[geoParcours._nNbPointTotal + 2];     // [8 + 2]

    int t_indirect = geoParcours.SelectionnePointsEntreSetRetDuCoteDeSR(&geoParcoursSR, TableauDePoints, geoParcours._nNbPointTotal);

    // Check result:

    EXPECT_EQ(6, t_indirect);
    EXPECT_EQ(0, TableauDePoints[0]->Identifiant);
    EXPECT_EQ(1, TableauDePoints[1]->Identifiant);
    EXPECT_EQ(0, TableauDePoints[2]->Identifiant);
    EXPECT_EQ(1, TableauDePoints[3]->Identifiant);
    EXPECT_EQ(3, TableauDePoints[4]->Identifiant);
    EXPECT_EQ(4, TableauDePoints[5]->Identifiant);

    // Destruction
    delete PointADroite;
    delete PointAGauche;
    delete IndexPointFrontiere;
    delete EstUnPointIntersectant;
}

// DONE
// Testing the TYSetGeometriqueParcours::CreerTrajetAPartirDuneListeDePointsTriee method
TEST_F(TYSetGeometriqueParcoursTest, CreerTrajetAPartirDuneListeDePointsTriee)
{
    // Data setup:
    // Geometrique Parcours
    TYSetGeometriqueParcours geoParcoursD;

    TYSetGeometriqueParcours geoParcoursG;


    TYPointParcours* TableauDePoints[4];
    TableauDePoints[0] = &P1;
    TableauDePoints[1] = &P2;
    TableauDePoints[2] = &P3;
    TableauDePoints[3] = &P4;

    int NbPoints = 4;
    bool GardeIdentifiant = 1;

    // Call to the tested method:
    geoParcoursD.CreerTrajetAPartirDuneListeDePointsTriee(TableauDePoints, NbPoints, 1, GardeIdentifiant);
    geoParcoursG.CreerTrajetAPartirDuneListeDePointsTriee(TableauDePoints, NbPoints, 0, GardeIdentifiant);

    // Check result:
    EXPECT_EQ(4, geoParcoursD._nNbPointTotal);
    EXPECT_EQ(1, geoParcoursD._nNbPolylines);
    EXPECT_EQ(0, geoParcoursD._ListePoint[0].Identifiant);
    EXPECT_EQ(1, geoParcoursD._ListePoint[1].Identifiant);
    EXPECT_EQ(2, geoParcoursD._ListePoint[2].Identifiant);
    EXPECT_EQ(3, geoParcoursD._ListePoint[3].Identifiant);

    EXPECT_EQ(4, geoParcoursG._nNbPointTotal);
    EXPECT_EQ(1, geoParcoursG._nNbPolylines);
    EXPECT_EQ(3, geoParcoursG._ListePoint[0].Identifiant);
    EXPECT_EQ(2, geoParcoursG._ListePoint[1].Identifiant);
    EXPECT_EQ(1, geoParcoursG._ListePoint[2].Identifiant);
    EXPECT_EQ(0, geoParcoursG._ListePoint[3].Identifiant);

    // Destruction
    geoParcoursD._ListePolylines = nullptr;
    geoParcoursD._ListePoint = nullptr;
    geoParcoursD._nNbPointTotal = 0;
    geoParcoursD._nNbPolylineAllouee = 0;
    geoParcoursD._nNbPolylines = 0;

    geoParcoursG._ListePolylines = nullptr;
    geoParcoursG._ListePoint = nullptr;
    geoParcoursG._nNbPointTotal = 0;
    geoParcoursG._nNbPolylineAllouee = 0;
    geoParcoursG._nNbPolylines = 0;
}

// DONE
// Testing the TYSetGeometriqueParcours::EnveloppeConvexeLes2PremiersPointsEtant method
TEST_F(TYSetGeometriqueParcoursTest, EnveloppeConvexeLes2PremiersPointsEtant)
{
    // Data setup:
    // Parametres

    bool* PointAGauche;
    bool* PointADroite;
    int* IndexPointFrontiere = new int[100];
    int NbPointFrontiere = 0;
    bool* EstUnPointIntersectant = new bool[100];
    bool bCoteGauche = true;

    TYPointParcours** TableauDePoints = new TYPointParcours*[geoParcours._nNbPointTotal + 2];

    geoParcours.MarquePointsADroiteEtAGauche(S, R, PointAGauche, PointADroite);
    geoParcours.RamenerPointsTraversantLaFrontiere(S, R, IndexPointFrontiere, NbPointFrontiere, EstUnPointIntersectant, bCoteGauche, PointAGauche, PointADroite);
    geoParcours.TriePointsIntersectionSuivantSR(S, R, IndexPointFrontiere, NbPointFrontiere);
    int nNbrPointsSelectionnes = geoParcours.SelectionnePointsEntreSetRetDuCoteDeSR(&geoParcoursSR, TableauDePoints, geoParcours._nNbPointTotal);

    TYPointParcours** TableauDePointsECOut = new TYPointParcours*[nNbrPointsSelectionnes];      // 9 semblerait-il

    // Call to the tested method:
    int NbPointEC = TYSetGeometriqueParcours::EnveloppeConvexeLes2PremiersPointsEtant(TableauDePoints, nNbrPointsSelectionnes, TableauDePointsECOut, false);

    // Check result:
    EXPECT_EQ(3, NbPointEC);
    EXPECT_EQ(6, nNbrPointsSelectionnes);
    EXPECT_EQ(0, TableauDePointsECOut[0]->Identifiant);
    EXPECT_EQ(0, TableauDePointsECOut[1]->Identifiant);
    EXPECT_EQ(1, TableauDePointsECOut[2]->Identifiant);


    // Destruction
    delete TableauDePoints;
    delete TableauDePointsECOut;
}

// DONE
// Testing the TYSetGeometriqueParcours::PremierePasse method
TEST_F(TYSetGeometriqueParcoursTest, PremierePasse)
{
    // Data setup:
    TYSetGeometriqueParcours geoPremierePasse;


    bool* PointAGauche;
    bool* PointADroite;
    int* IndexPointFrontiere = new int[100];
    int NbPointFrontiere = 0;
    bool* EstUnPointIntersectant = new bool[100];
    bool bCoteGauche = true;

    Connexite* Connexes = new Connexite[geoParcours._nNbPointTotal];
    geoParcours.ListerPointsConnexes(Connexes);

    geoParcours.MarquePointsADroiteEtAGauche(S, R, PointAGauche, PointADroite);
    geoParcours.RamenerPointsTraversantLaFrontiere(S, R, IndexPointFrontiere, NbPointFrontiere, EstUnPointIntersectant, bCoteGauche, PointAGauche, PointADroite);

    bool FirstPasse = geoParcours.PremierePasse(S, R, IndexPointFrontiere, NbPointFrontiere, EstUnPointIntersectant, Connexes, geoPremierePasse);

    // Check result:
    EXPECT_EQ(true, FirstPasse);
    EXPECT_EQ(12, geoPremierePasse._nNbPointTotal);

     //Destruction
    delete [] PointADroite;
    delete [] PointAGauche;
    delete [] IndexPointFrontiere;
    delete [] EstUnPointIntersectant;
    delete [] Connexes;
}

// DONE
// Testing the TYSetGeometriqueParcours::SecondePasse method
TEST_F(TYSetGeometriqueParcoursTest, SecondePasse)
{
    // Data setup:
    TYSetGeometriqueParcours geoPremierePasse, geoSecondePasse;


    bool* PointAGauche;
    bool* PointADroite;
    int* IndexPointFrontiere = new int[100];
    int NbPointFrontiere = 0;
    bool* EstUnPointIntersectant = new bool[100];
    bool bCoteGauche = true;
    TYPointParcours** TableauDePoints = new TYPointParcours*[geoParcours._nNbPointTotal + 2];

    Connexite* Connexes = new Connexite[geoParcours._nNbPointTotal];
    geoParcours.ListerPointsConnexes(Connexes);

    geoParcours.MarquePointsADroiteEtAGauche(S, R, PointAGauche, PointADroite);
    geoParcours.RamenerPointsTraversantLaFrontiere(S, R, IndexPointFrontiere, NbPointFrontiere, EstUnPointIntersectant, bCoteGauche, PointAGauche, PointADroite);

    // Call to the tested method:
    geoParcours.PremierePasse(S, R, IndexPointFrontiere, NbPointFrontiere, EstUnPointIntersectant, Connexes, geoPremierePasse);

    int nNbrPointsSelectionnes = geoParcours.SelectionnePointsEntreSetRetDuCoteDeSR(&geoParcoursSR, TableauDePoints, geoParcours._nNbPointTotal);

    TYPointParcours** TableauDePointsECOut = new TYPointParcours*[nNbrPointsSelectionnes];

    // Call to the tested method:
    int NbPointEC = TYSetGeometriqueParcours::EnveloppeConvexeLes2PremiersPointsEtant(TableauDePoints, nNbrPointsSelectionnes, TableauDePointsECOut, false);

    bool b_SecondePasse = geoParcours.SecondePasse(geoPremierePasse, geoSecondePasse, true, TableauDePointsECOut, NbPointEC);

    // Check result:
    EXPECT_EQ(true, b_SecondePasse);
    EXPECT_EQ(12, geoPremierePasse._nNbPointTotal);
    EXPECT_EQ(3, geoSecondePasse._nNbPointTotal);
    EXPECT_EQ(0, geoSecondePasse._ListePolylines[0].indexePoint(0));
    EXPECT_EQ(0, geoSecondePasse._ListePolylines[0].indexePoint(1));
    EXPECT_EQ(1, geoSecondePasse._ListePolylines[0].indexePoint(2));


    // Destruction
    delete [] EstUnPointIntersectant;
    delete [] TableauDePoints;
    delete [] Connexes;
    delete [] TableauDePointsECOut;
    delete [] PointADroite;
    delete [] PointAGauche;
}

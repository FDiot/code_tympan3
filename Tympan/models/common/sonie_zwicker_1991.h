/*
 * Copyright (C) <2012-2014> <EDF-R&D> <FRANCE>
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

#ifndef TY_MC_SONIE_ZWICKER_1991
#define TY_MC_SONIE_ZWICKER_1991

/**
 * \brief Class to define the loudness (according Zwicker, 1991)
 */
class sonie
{
public :
	/// Constructor
    sonie(double* vectToct, const unsigned short& champ = 0);
    /// Destructor
    ~sonie();
    /// Renvoie la sonie totale
    double getNTot() { return N_Tot; }
    /// Renvoie le niveau d'isosonie en phone
    double getLN() { return LN; }
    /// Renvoie la sonie sp�cifique
    double* getNSpecif() { return N_Specif; }
    /// Renvoie le vecteur des bandes de Barks
    double* getBarks() { return BarkAxis; }
    /// Indique si le calcul s'est correctement effectu�
    bool isOk() { return _isOk; }

private  :
    /// V�rification des donn�es d'entr�e
    bool validation();

    /// * Fonction de calcul de la sonie
    void exec();

    /// Renvoie le niveau d'isosonie en phone
    double calcIsoSonie(const double& val);

private :
    /// Dimension du vecteur des valaur en 1/3 d'octave
    static const unsigned short Nbandes3Oct;

    /**
        Gammes des niveaux des tiers d'octave pour
        les pond�rations en basses fr�quences selon les courbes d'isosonie
    */
    static const double RAP[];

    /**
        Pond�ration des bandes basses fr�quences selon les courbes d'isosonie donn�es pour
        les 8 gammes de niveauxd�finies dans RAP
    */
    static const double DLL[8][11];

    /**
        Niveaux dans les bandes critiques au seuil absolu d'audition sans prendre en compte
        les caract�ristiques detransmission de l'oreille
    */
    static const double LTQ[];

    /**
        Corrections de niveaux dues aux caract�ristiques de transmissions de l'oreille
    */
    static const double A0[];

    /**
        Diff�rences de niveaux entre champs libre et champs diffus pour chaque bande
    */
    static const double DDF[];

    /**
        limites sup�rieures des bandes critiques approxim�es
        ( exprim�es en num�ro de bande critique)
    */
    static const double ZUP[];

    /**
        Adaptation des niveaux en tiers d'octave aux niveaux en bandes critiques
     */
    static const double DCB[];

    /**
        gammes des niveaux de sonie sp�cifique pour d�terminer les pentes des courbes
        de flancs sup�rieurs dans la courbe de sonie sp�cifique
    */
    static const double RNS[];

    /**
        pente des courbes de flancs sup�rieurs dans la courbe de sonie sp�cifique pour
        les niveaux RNS en fonction du num�ro de la bande critique
    */
    static const double USL[18][8];

    /// Tableau de 28 doubles repr�sentant les valeurs par 1/3 d'octave sur la bande 25-10000 Hz
    double* VectNiv3Oct;
    /// Type de champ (0 = champ libre, 1 = champ diffus)
    unsigned short Champ;
    /// Sonie totale
    double N_Tot;
    /// Niveau d'isosonie en phone
    double LN;
    /// Sonie Sp�cifique
    double* N_Specif;
    /// Vecteur des de bark sur lequel est calcul� N_specif.
    double* BarkAxis;
    /// Indication de bonne ex�cution du calcul
    bool _isOk;
};
#endif // TY_MC_SONIE_ZWICKER_1991

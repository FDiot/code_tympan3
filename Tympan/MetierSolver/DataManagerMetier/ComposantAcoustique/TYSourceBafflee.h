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

/*
 *
 */

#ifndef __TY_SOURCE_BAFFLEE__
#define __TY_SOURCE_BAFFLEE__

#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticRectangle.h"
#include "TYSourcePonctuelle.h"

// nombre de valeurs de ka dans le tableau
#ifdef NB_KA
#   undef NB_KA
#   define NB_KA 9
#else
#   define NB_KA 9
#endif

// nombre de valeurs de theta dans le tableau
#ifdef NB_THETA
#   undef NB_THETA
#   define NB_THETA 41
#else
#   define NB_THETA 41
#endif

class OSegment3D;

/**
 * Classe de definition d'une source bafflee.
 * Ce type de source correspond aux bouches de ventilation.
 * Un rectangle acoustique est associee a cette source, il
 * correspond a la face sur laquelle se trouve la source.
 */
class TYSourceBafflee: public TYSourcePonctuelle
{
    OPROTOSUPERDECL(TYSourceBafflee, TYSourcePonctuelle)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSourceBafflee();
    /**
     * Constructeur par copie.
     */
    TYSourceBafflee(const TYSourceBafflee& other);
    /**
     * Destructeur.
     */
    virtual ~TYSourceBafflee();

    ///Operateur =.
    TYSourceBafflee& operator=(const TYSourceBafflee& other);
    ///Operateur ==.
    bool operator==(const TYSourceBafflee& other) const;
    ///Operateur !=.
    bool operator!=(const TYSourceBafflee& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get du rectangle acoustique associe a cette source.
     */
    //    void setAcousticRectangle(LPTYAcousticRectangle pAcousticRectangle) ;
    void setAcousticRectangle(LPTYRectangle pAcousticRectangle) ;

    /**
     * Set/Get du rectangle acoustique associe a cette source.
     */
    //    LPTYAcousticRectangle getAcousticRectangle() { return _pAcousticRectangle; }
    LPTYRectangle getAcousticRectangle() { return _pAcousticRectangle; }

    /**
     * Set/Get du rectangle acoustique associe a cette source.
     */
    //    const LPTYAcousticRectangle getAcousticRectangle() const { return _pAcousticRectangle; }
    const LPTYRectangle getAcousticRectangle() const { return _pAcousticRectangle; }

    /**
     * Calcul de la puissance apparente de la source dans la direction du segment
     */
    OSpectre lwApparenteSrcDest(const OSegment3D& seg, const TYAtmosphere& atmos, const int& expGeo = 0, const int& regime = -1) const;


private:
    /**
     * Recherche dans le tableau _tabCorrLWBaffle[] du coefficient de correction.
     * Exacte copie du code de TYMPAN II.
     *
     * @param ka Indice KA.
     *
     * @return Le coefficient de correction.
     */
    double normeQBaffle(const int& indice_Ka, const int& indice_theta, const double& ka, const double& theta) const;

    /**
     * recherche de l'indice correspondant a la valeur de Ka dans le tableau _tabKa
     */
    int getIndiceKa(const double& ka) const;


    // Membres
protected:
    ///Le rectangle acoustique associe a cette source.
    //    LPTYAcousticRectangle _pAcousticRectangle;
    LPTYRectangle _pAcousticRectangle;

private:
    // Tableaux utilises dans le calcul de la directivite.
    static const double _tabQ[NB_KA][NB_THETA];
    static const double _tabKa[NB_KA];
};


///Smart Pointer sur TYSourceBafflee.
typedef SmartPtr<TYSourceBafflee> LPTYSourceBafflee;

///Noeud geometrique de type TYSourceBafflee.
typedef TYGeometryNode TYSourceBaffleeGeoNode;
///Smart Pointer sur TYSourceBaffleeGeoNode.
typedef SmartPtr<TYSourceBaffleeGeoNode> LPTYSourceBaffleeGeoNode;
///Collection de noeuds geometriques de type TYSourceBafflee.
typedef std::vector<LPTYSourceBaffleeGeoNode> TYTabSourceBaffleeGeoNode;



#endif // __TY_SOURCE_BAFFLEE__

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

#ifndef __TY_PLAN_EAU__
#define __TY_PLAN_EAU__


#include "TYTerrain.h"
#include "TYCourbeNiveau.h"


/**
 * Classe pour la representation d'un plan eau.
 * Un plan d'eau est un terrain particulier, chaque points constituant la
 * delimitation ont a meme altitude.
 * Les plans d'eau sont pris en compte pour la generation de l'altimetrie.
 */
class TYPlanEau: public TYTerrain
{
    OPROTODECL(TYPlanEau)
    TY_EXTENSION_DECL_ONLY(TYPlanEau)
    TY_EXT_GRAPHIC_DECL_ONLY(TYPlanEau)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYPlanEau();
    /**
     * Constructeur par copie.
     */
    TYPlanEau(const TYPlanEau& other);
    /**
     * Constructeur.
     */
    TYPlanEau(const TYTabPoint& pts, double alt);
    /**
     * Destructeur.
     */
    virtual ~TYPlanEau();

    ///Operateur =.
    TYPlanEau& operator=(const TYPlanEau& other);
    ///Operateur ==.
    bool operator==(const TYPlanEau& other) const;
    ///Operateur !=.
    bool operator!=(const TYPlanEau& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void setIsGeometryModified(bool isModified);

    /**
     * Set/Get de la courbe de niveau associee a ce plan d'eau.
     */
    LPTYCourbeNiveau getCrbNiv() { return _pCrbNiv; }

    /**
     * Set/Get de la courbe de niveau associee a ce plan d'eau.
     */
    void setCrbNiv(LPTYCourbeNiveau pCrbNiv)
    {
        _pCrbNiv = pCrbNiv;
        _pCrbNiv->setParent(this);
        setIsGeometryModified(true);
    }

    /**
     * Set/Get de l'_altitude.
     */
    double getAltitude() const { return _pCrbNiv->getAltitude(); }
    /**
     * Set/Get de l'_altitude.
     */
    void setAltitude(double alt) { _pCrbNiv->setAltitude(alt); setIsGeometryModified(true); }

    /**
     * Set/Get de la liste de points.
     */
    virtual TYTabPoint& getListPoints() { return _pCrbNiv._pObj->getListPoints(); }
    /**
     * Set/Get de la liste de points.
     */
    virtual const TYTabPoint& getListPoints() const { return _pCrbNiv._pObj->getListPoints(); }
    /**
     * Set/Get de la liste de points.
     */
    virtual void setListPoints(const TYTabPoint& liste) { _pCrbNiv->setListPoints(liste); setIsGeometryModified(true); }

    /**
     * Set/Get de la distance minimale entre les points pour la triangulation de Delaunay
     */
    void setDistMax(const double dist) { _pCrbNiv->setDistMax(dist);}

    /**
     * Get/Set de la distance minimale entre deux points
     */
    double getDistMax() { return _pCrbNiv->getDistMax(); }
    const double getDistMax() const { return _pCrbNiv->getDistMax(); };

    /**
     * Get/Set de l'etat de la valeur distance min
     */
    bool getIsDMaxDefault() { return _pCrbNiv->getIsDMaxDefault(); }
    const bool getIsDMaxDefault() const { return _pCrbNiv->getIsDMaxDefault(); }

    /**
     * Get/Set de l'etat de la valeur distance min par defaut
     */
    void setIsDMaxDefault(const bool& etat) { _pCrbNiv->setIsDMaxDefault(etat); }

    /**
     * Mise a jour de la distance max
     */
    void updateDistMax() { _pCrbNiv->updateDistMax(); }


    // Membres
protected:
    ///Une courbe de niveau correspondant a ce plan d'eau.
    LPTYCourbeNiveau _pCrbNiv;
};


///Noeud geometrique de type TYPlanEau.
typedef TYGeometryNode TYPlanEauGeoNode;
///Smart Pointer sur TYPlanEauGeoNode.
typedef SmartPtr<TYPlanEauGeoNode> LPTYPlanEauGeoNode;
///Collection de noeuds geometriques de type TYPlanEau.
typedef std::vector<LPTYPlanEauGeoNode> TYTabPlanEauGeoNode;


#endif // __TY_PLAN_EAU__

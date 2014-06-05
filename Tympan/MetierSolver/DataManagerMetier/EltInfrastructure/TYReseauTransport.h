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
 *
 *
 *
 */

#ifndef __TY_RESEAUTRANSPORT__
#define __TY_RESEAUTRANSPORT__


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticLine.h"

/**
 * Comprend les proprietes acoustiques et geometriques d'un reseau de transport.
 */
class TYReseauTransport: public TYAcousticLine
{
    OPROTODECL(TYReseauTransport)
    TY_EXTENSION_DECL_ONLY(TYReseauTransport)
    TY_EXT_GRAPHIC_DECL_ONLY(TYReseauTransport)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYReseauTransport();
    /**
     * Constructeur par copie.
     */
    TYReseauTransport(const TYReseauTransport& other);
    /**
     * Destructeur.
     */
    virtual ~TYReseauTransport();

    ///Operateur =.
    TYReseauTransport& operator=(const TYReseauTransport& other);
    ///Operateur ==.
    bool operator==(const TYReseauTransport& other) const;
    ///Operateur !=.
    bool operator!=(const TYReseauTransport& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Set/Get du nombre de brins.
     */
    int getNbBrins() const { return _nbBrins; }

    /**
     * Set/Get du nombre de brins.
     */
    void setNbBrins(int nb) { _nbBrins = nb; }

    /**
     * Set/Get de la tension.
     */
    double getTension() const { return _tension; }

    /**
     * Set/Get de la tension.
     */
    void setTension(double tension) { _tension = tension; }

    /**
     * Set/Get de la puissance transportee.
     */
    double getPuissance() const { return _puissance; }

    /**
     * Set/Get de la puissance transportee.
     */
    void setPuissance(double puissance) { _puissance = puissance; }

    /**
     * Set/Get de la HauteurMoyenne.
     */
    double getHauteurMoyenne() const { return _hauteurMoyenne; }

    /**
     * Set/Get de la HauteurMoyenne.
     */
    void setHauteurMoyenne(double hauteur) { _hauteurMoyenne = hauteur; }

    /**
     * Mise a jour des caracteristiques acoustiques de la route
     */
    virtual bool updateAcoustic(const bool& force = false);


    // Membres
protected:
    ///Nombre de brins
    int _nbBrins;
    ///Tension
    double _tension;
    ///Puissance transportee
    double _puissance;
    ///Hauteur Moyenne
    double _hauteurMoyenne;
};


///Noeud geometrique de type TYReseauTransport.
typedef TYGeometryNode TYReseauTransportGeoNode;
///Smart Pointer sur TYReseauTransportGeoNode.
typedef SmartPtr<TYReseauTransportGeoNode> LPTYReseauTransportGeoNode;
///Collection de noeuds geometriques de type TYReseauTransport.
typedef std::vector<LPTYReseauTransportGeoNode> TYTabReseauTransportGeoNode;


#endif // __TY_RESEAUTRANSPORT__

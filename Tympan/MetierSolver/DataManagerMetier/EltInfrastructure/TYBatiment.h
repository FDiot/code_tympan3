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

#ifndef __TY_BATIMENT__
#define __TY_BATIMENT__


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticVolumeNode.h"
#include "TYEtage.h"

/**
 * Classe batiment.
 */
class TYBatiment: public TYAcousticVolumeNode
{
    OPROTOSUPERDECL(TYBatiment, TYAcousticVolumeNode)
    TY_EXTENSION_DECL_ONLY(TYBatiment)
    TY_EXT_GRAPHIC_DECL_ONLY(TYBatiment)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYBatiment();
    /**
     * Constructeur par copie.
     */
    TYBatiment(const TYBatiment& other);
    /**
     * Destructeur.
     */
    virtual ~TYBatiment();

    ///Operateur =.
    TYBatiment& operator=(const TYBatiment& other);
    ///Operateur ==.
    bool operator==(const TYBatiment& other) const;
    ///Operateur !=.
    bool operator!=(const TYBatiment& other) const;

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void setDensiteSrcsH(double densite, bool recursif = true);
    virtual void setDensiteSrcsV(double densite, bool recursif = true);

    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual void distriSrcs();
    virtual bool setSrcsLw();

    virtual double volume() const;
    virtual double surface() const;
    virtual TYTabVector normals() const;
    virtual TYTabPoint sommets() const;
    virtual TYTabLPPolygon faces() const;
    virtual TYBox volEnglob() const;
    virtual TYPoint centreGravite() const;
    virtual int intersects(const OSegment3D& seg, TYTabPoint& ptList) const;
    virtual int isInside(const TYPoint& pt) const;
    virtual bool updateAcoustic(const bool& force = false);
    virtual LPTYSpectre getRealPowerSpectrum();


    // Membres
protected:

};


///Noeud geometrique de type TYBatiment.
typedef TYGeometryNode TYBatimentGeoNode;
///Smart Pointer sur TYBatimentGeoNode.
typedef SmartPtr<TYBatimentGeoNode> LPTYBatimentGeoNode;
///Collection de noeuds geometriques de type TYBatiment.
typedef std::vector<LPTYBatimentGeoNode> TYTabBatimentGeoNode;


#endif // __TY_BATIMENT__

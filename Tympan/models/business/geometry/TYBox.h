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

#ifndef __TY_BOX__
#define __TY_BOX__

class OSegment3D;

#include "TYPoint.h"
#include "Tympan/models/business/TYColorInterface.h"

/**
 * Classe de definition d'une boite.
 * Une boite est representee par ses dimensions en X, Y et Z,
 * ainsi que par un point pour indiquer sa position.
 */
class TYBox: public TYElement, public TYColorInterface
{
    OPROTODECL(TYBox)
    TY_EXTENSION_DECL_ONLY(TYBox)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYBox();
    /**
     * Constructeur par copie.
     */
    TYBox(const TYBox& other);
    /**
     * Destructeur.
     */
    virtual ~TYBox();

    ///Operateur =.
    TYBox& operator=(const TYBox& other);
    ///Operateur ==.
    bool operator==(const TYBox& other) const;
    ///Operateur !=.
    bool operator!=(const TYBox& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Calcule la liste des points d'intersection de cette boite avec le segment passe.
     *
     * @param seg Segment dont on veut l'intersection.
     * @param ptList Les point resultant de l'intersection.
     *
     * @return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    int intersects(const OSegment3D& seg, TYTabPoint& ptList) const;

    /**
     * Test si le point passe se trouve a l'interieur de cette boite.
     *
     * @param pt Point dont on teste l'appartenance a cette boite.
     *
     * @return INTERS_OUI (1) si le point est a l'interieur de cette boite,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) si le point est a l'exterieur de cette boite.
     */
    int isInside(const TYPoint& pt) const;

    /**
     * Set/Get de la position du centre de cette boite.
     */
    void setPosition(const TYPoint& position) { _position = position; }
    /**
     * Set/Get de la position du centre de cette boite.
     */
    TYPoint& getPosition() { return _position; }
    /**
     * Set/Get de la position du centre de cette boite.
     */
    const TYPoint& getPosition() const { return _position; }


    // Membres
public:
    ///Dimension en X.
    double _sizeX;
    ///Dimension en Y.
    double _sizeY;
    ///Dimension en Z.
    double _sizeZ;

protected:
    ///Position du centre de cette boite
    TYPoint _position;
};


#endif // __TY_BOX__

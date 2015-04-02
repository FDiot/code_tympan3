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

#ifndef __TY_SEGMENT__
#define __TY_SEGMENT__

#include "Tympan/models/business/TYElement.h"
#include "Tympan/models/business/TYColorInterface.h"
#include "Tympan/models/common/3d.h"
#include "TYPoint.h"

// TYRectangle.h est inclu a la fin de la declaration de TYSegment.
class TYRectangle;

/**
 * Classe de definition d'un segment.
 */
class TYSegment: public TYElement, public OSegment3D, public TYColorInterface
{
    OPROTODECL(TYSegment)
    TY_EXTENSION_DECL_ONLY(TYSegment)
    TY_EXT_GRAPHIC_DECL_ONLY(TYSegment)

    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    TYSegment();
    /**
     * Constructeur par copie.
     */
    TYSegment(const TYSegment& other);
    /**
     * Constructeur a partir de 2 points.
     */
    TYSegment(TYPoint ptA, TYPoint ptB);
    /**
     * Destructeur.
     */
    virtual ~TYSegment();

    ///Operateur =.
    virtual TYSegment& operator=(const TYSegment& other);
    ///Operateur ==.
    virtual bool operator==(const TYSegment& other) const;
    ///Operateur !=.
    virtual bool operator!=(const TYSegment& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * \brief Comparison function for unordered segment
     *
     * \param s1 Segment 1.
     * \param s2 Segment 2.
     * \return True if the two segments are equal
     */
    static bool unorderedIsEqual(const TYSegment& s1, const TYSegment& s2);
};


#include "TYRectangle.h"


#endif // __TY_SEGMENT__



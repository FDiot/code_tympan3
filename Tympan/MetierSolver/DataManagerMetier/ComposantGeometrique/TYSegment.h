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

#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/DataManagerCore/TYColorInterface.h"
#include "Tympan/MetierSolver/CommonTools/OMatrix.h"
#include "TYPoint.h"

// TYRectangle.h est inclu a la fin de la declaration de TYSegment.
class TYRectangle;

/**
 * Classe de definition d'un segment.
 */
class TYSegment: public TYElement, public TYColorInterface
{
    OPROTOSUPERDECL(TYSegment, TYElement)
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
    TYSegment& operator=(const TYSegment& other);
    ///Operateur ==.
    bool operator==(const TYSegment& other) const;
    ///Operateur !=.
    bool operator!=(const TYSegment& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * Multiplication avec une matrice.
     *
     * @param matrix La matrice a appliquer aux points de ce segment.
     *
     * @return Une copie de ce segment transforme.
     */
    TYSegment operator*(const OMatrix& matrix) const;

    /**
     * Retourne la longueur du segment.
     */
    double longueur() const;

    /**
     * Retourne le symetrique du point passe.
     *
     * @param pt Le point dont on veut le symetrique.
     * @param ptSym Le point resultant de la symetrie.
     *
     * @return 1 si le calcul a reussi, 0 sinon.
     */
    int symetrieOf(const TYPoint& pt, TYPoint& ptSym) const;

    /**
     * Retourne le projete du point passe.
     *
     * @param pt Le point dont on veut le projete.
     * @param ptProj Le point resultant de la projection.
     *
     * @return 1 si le calcul a reussi, 0 sinon.
     */
    int projection(const TYPoint& pt, TYPoint& ptProj) const;

    /**
     * Retourne le point d'intersection avec le segment passe.
     *
     * @param seg Segment dont on veut l'intersection.
     * @param ptSym Le point resultant de l'intersection.
     *
     * @return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    int intersects(const TYSegment& seg, TYPoint& pt) const;
    /**
     * Retourne la position du milieu du segment
     *
     * @return TYPoint
     */
    TYPoint centreOf() const;
    /**
     * Retourne la position du centre de l'arc de cercle de rayon R reliant les deux extremites du segment
     *
     * @return TYPoint Position du centre du cercle
     */
    TYPoint centerOfCurvedPath(const double& R) const;
    /**
     * Retourne la hauteur du sommet de l'arc de cercle de rayon R reliant les deux extremites du segment
     *
     * @return double Hauteur du sommet
     */
    double topOfCurvedPath(const double& R) const;

    /**
     * Calcule la longueur du trajet de rayon R passant par les extremites du segment
     * @param R rayon du cercle de rayon R
     * @return un double
     */
    double lengthOfCurvedPath(const double& R);

    /**
     * Construit un OVector3D a partir d'un segment utile pour la directivite des sources
     *
     * @return une reference sur un vecteur3D
     */
    OVector3D toVector3D() const { return OVector3D(_ptA, _ptB); }

    /**
     * \brief Comparison function for unordered segment
     *
     * \param s1 Segment 1.
     * \param s2 Segment 2.
     * \return True if the two segments are equal
     */
    static bool unorderedIsEqual(const TYSegment& s1, const TYSegment& s2);

    // Membres
public:
    ///Extremite A.
    TYPoint _ptA;
    ///Extremite B.
    TYPoint _ptB;
};


#include "TYRectangle.h"


#endif // __TY_SEGMENT__



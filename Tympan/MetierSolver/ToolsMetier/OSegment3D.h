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

#ifndef __O_SEGMENT_3D__
#define __O_SEGMENT_3D__


#include "OPoint3D.h"
#include "OVector3D.h"
#include "OMatrix.h"


/**
 * \file OSegment3D.h
 * \class OSegment3D
 * \brief Classe de definition d'un segment.
 * \author Projet_Tympan
 */

class OSegment3D
{
    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    OSegment3D();
    /**
     * Constructeur par copie.
     */
    OSegment3D(const OSegment3D& other);
    /**
     * Constructeur a partir de 2 points.
     */
    OSegment3D(const OPoint3D& ptA, const OPoint3D& ptB);
    /**
     * Destructeur.
     */
    virtual ~OSegment3D();

    ///Operateur =.
    OSegment3D& operator=(const OSegment3D& other);
    ///Operateur ==.
    bool operator==(const OSegment3D& other) const;
    ///Operateur !=.
    bool operator!=(const OSegment3D& other) const;

    /**
     * \fn OSegment3D operator*(const OMatrix& matrix) const;
     * \brief Multiplication avec une matrice.
     *
     * \param matrix La matrice a appliquer aux points de ce segment.
     *
     * \return Une copie de ce segment transforme.
     */
    OSegment3D operator*(const OMatrix& matrix) const;

    /**
     * \fn double longueur() const;
     * \brief Retourne la longueur du segment.
     */
    double longueur() const;

    /**
     * \fn int symetrieOf(const OPoint3D& pt, OPoint3D& ptSym) const;
     * \brief Retourne le symetrique du point passe.
     *
     * \param pt Le point dont on veut le symetrique.
     * \param ptSym Le point resultant de la symetrie.
     *
     * \return 1 si le calcul a reussi, 0 sinon.
     */
    int symetrieOf(const OPoint3D& pt, OPoint3D& ptSym) const;

    /**
     * \fn int projection(const OPoint3D& pt, OPoint3D& ptProj,double seuilConfondus) const;
     * \brief Retourne le projete du point passe.
     *
     * \param pt Le point dont on veut le projete.
     * \param ptProj Le point resultant de la projection.
     *
     * \return 1 si le calcul a reussi, 0 sinon.
     */
    int projection(const OPoint3D& pt, OPoint3D& ptProj, double seuilConfondus) const;

    /**
     * \fn int intersects(const OSegment3D& seg, OPoint3D & pt,double seuilConfondus) const;
     * \brief Retourne le point d'intersection avec le segment passe.
     *
     * \param seg Segment dont on veut l'intersection.
     * \param ptSym Le point resultant de l'intersection.
     * \param seuilConfondus distance mini entre le point et le segment
     * en dessous de laquelle on considere qu'ils sont confondus
     *
     * \return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    int intersects(const OSegment3D& seg, OPoint3D& pt, double seuilConfondus) const;

    /**
     * \fn OPoint3D centreOf() const;
     * \brief Retourne la position du milieu du segment
     *
     * \return OPoint3D
     */
    OPoint3D centreOf() const;

    /**
     * \fn OPoint3D centerOfCurvedPath(const double &R) const;
     * \brief Retourne la position du centre de l'arc de cercle de rayon R reliant les deux extremites du segment
     *
     * \return OPoint3D Position du centre du cercle
     */
    OPoint3D centerOfCurvedPath(const double& R) const;

    /**
     * \fn double lengthOfCurvedPath(const double& R);
     * \brief Calcule la longueur du trajet de rayon R passant par les extremites du segment
     * \param R rayon du cercle de rayon R
     * \return un double
     */
    double lengthOfCurvedPath(const double& R);

    /**
     * \fn OVector3D toVector3D() const
     * \brief Construit un OVector3D a partir d'un segment utile pour la directivite des sources
     *
     * \return une reference sur un vecteur3D
     */
    OVector3D toVector3D() const { return OVector3D(_ptA, _ptB); }

    /**
     * \fn OSegment3D swap() const;
     * \brief Retourne le segment
     */
    OSegment3D swap() const;


    // Membres
public:
    ///Extremite A.
    OPoint3D _ptA;
    ///Extremite B.
    OPoint3D _ptB;
};




#endif // __O_SEGMENT_3D__



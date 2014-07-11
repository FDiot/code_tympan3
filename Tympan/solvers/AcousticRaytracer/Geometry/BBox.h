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

#ifndef BBOX_H
#define BBOX_H

/*!
 * \file BBox.h
 * \class BBox
 * \brief Definition d'une boîte englobante alignee sur les axes (BBox AABB) ainsi que quelques operations usuelles.
 */

#include "Tympan/models/common/mathlib.h"
//#include "Tympan/solvers/AcousticRaytracer/Ray/Ray.h"
#include <iostream>
using namespace std;

class BBox
{
public:

    //public data
    vec3 pMin;          /*!< Coin inferieur de la boîte englobante*/
    vec3 pMax;          /*!< Coin superieur de la boîte englobante*/
    vec3 centroid;      /*!< Centre de la boîte englobante*/
    bool isNull;        /*!< Determine si une BBox est initialisee ou non*/


    //public methods
    /*!
    *  \fn BBox()
    *  \brief Constructeur
    *
    *  Constructeur de base de la classe BBox. Les valeurs initiales sont au point d'origine
    */
    BBox() : isNull(true) {}

    /*!
    *  \fn BBox(vec3 &_pMin, vec3 &_pMax)
    *  \brief Constructeur
    *
    *  Constructeur de la classe BBox avec initialisation des coins de la boîte englobante et calcul du centroid.
    *  \param _pMin : Definition du coin inferieur de la boîte englobante
    *  \param _pMax : Definition du coin superieur de la boîte englobante
    */
    BBox(const vec3& _pMin, const vec3& _pMax) : isNull(false) {pMin = _pMin; pMax = _pMax; centroid = (pMin + pMax) / 2.0f;}

    /*
    * \fn BBox(const BBox &other)
    * \brief Constructeur par copie)
    * \param other : BBox a copier
    */
    BBox(const BBox& other) { pMin = vec3(other.pMin); pMax = vec3(other.pMax); centroid = (pMin + pMax) / 2.0f; isNull = other.isNull;}

    /*!
    *  \fn vec3 getBBMin()
    *  \brief Getter
    *
    *  Recupere le coin inferieur de la boîte englobante
    *  \return : Renvoi la valeur du coin inferieur (type vec3)
    */
    vec3 getBBMin() {return pMin;}

    /*!
    *  \fn vec3 getBBMax()
    *  \brief Getter
    *
    *  Recupere le coin superieur de la boîte englobante
    *  \return : Renvoi la valeur du coin superieur (type vec3)
    */
    vec3 getBBMax() {return pMax;}

    /*!
    *  \fn vec3 getCentroid()
    *  \brief Getter
    *
    *  Recupere le centre de la boîte englobante
    *  \return : Renvoi la valeur du centre (type vec3)
    */
    vec3 getCentroid() {return centroid;}

    /*!
    *  \fn void setBBMin(vec3 &_pMin)
    *  \brief Setter
    *
    *  Modifie le coin inferieur de la boîte et recalcul le nouveau centre de la boîte.
    *  \param _pMin : Nouveau coin inferieur de la boîte englobante
    */
    void setBBMin(vec3& _pMin) {pMin = _pMin; centroid = (pMin + pMax) / 2.0f;}

    /*!
    *  \fn void setBBMax(vec3 &_pMax)
    *  \brief Setter
    *
    *  Modifie le coin inferieur de la boîte et recalcul le nouveau centre de la boîte.
    *  \param _pMax : Nouveau coin superieur de la boîte englobante
    */
    void setBBMax(vec3& _pMax) {pMax = _pMax; centroid = (pMin + pMax) / 2.0f;}


    /*!
    *  \fn double SurfaceArea() const
    *  \brief Renvoie l'aire de  la boîte englobante
    *
    *  Renvoie la somme des aires des surfaces de la boîte englobante. Utilisee pour le calcul du SAH pour les structures acceleratrices
    */
    double SurfaceArea() const
    {
        vec3 d = pMax - pMin;
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    /*!
    *  \fn BBox Union(const BBox &b, const vec3 &p)
    *  \brief Union d'une boîte englobante et d'un point
    *
    *  Fait la fusion d'une boîte englobante et d'un point et renvoie une nouvelle boîte englobante.
    *  \param b : Boîte englobante d'origine (ne sera pas modifiee)
    *  \param p : Point a inserer dans la boîte englobante resultat
    *  \return Renvoie une nouvelle boîte englobant issue de la fusion du point et de la boîte englobante
    */
    BBox Union(const BBox& b, const vec3& p)
    {
        BBox ret = b;
        ret.pMin.x = min(b.pMin.x, p.x);
        ret.pMin.y = min(b.pMin.y, p.y);
        ret.pMin.z = min(b.pMin.z, p.z);
        ret.pMax.x = max(b.pMax.x, p.x);
        ret.pMax.y = max(b.pMax.y, p.y);
        ret.pMax.z = max(b.pMax.z, p.z);
        ret.centroid = (ret.pMin + ret.pMax) * 0.5;
        return ret;
    }

    /*!
    *  \fn BBox Union(const vec3 &p)
    *  \brief Union d'une boîte englobante et d'un point
    *
    *  Fait la fusion d'une boîte englobante et d'un point et renvoie une nouvelle boîte englobante.
    *  \param b : Boîte englobante d'origine (ne sera pas modifiee)
    *  \param p : Point a inserer dans la boîte englobante resultat
    *  \return Renvoie une nouvelle boîte englobant issue de la fusion du point et de la boîte englobante
    */
    BBox Union(const vec3& p)
    {
        BBox ret;
        ret.isNull = false;
        ret.pMin.x = min(pMin.x, p.x);
        ret.pMin.y = min(pMin.y, p.y);
        ret.pMin.z = min(pMin.z, p.z);
        ret.pMax.x = max(pMax.x, p.x);
        ret.pMax.y = max(pMax.y, p.y);
        ret.pMax.z = max(pMax.z, p.z);
        ret.centroid = (ret.pMin + ret.pMax) * 0.5;
        return ret;
    }

    /*!
    * \fn BBox Union(const BBox &b2)
    *  \brief Union dde deux boîtes englobantes
    *
    *  Fait la fusion de deux boîtes englobantes et renvoie une nouvelle boîte englobante.
    *  \param b1 : Premiere boîte englobante(ne sera pas modifiee)
    *  \param b2 : Deuxieme boîte englobante(ne sera pas modifiee)
    *  \return Renvoie une nouvelle boîte englobant issue de la fusion des deux boîtes englobantes
    */
    BBox Union(const BBox& b2)
    {
        BBox ret;
        if (isNull)
        {
            ret = b2;
        }
        else if (b2.isNull)
        {
            ret = *this;
        }
        else
        {
            ret.isNull = false;
            ret.pMin.x = min(pMin.x, b2.pMin.x);
            ret.pMin.y = min(pMin.y, b2.pMin.y);
            ret.pMin.z = min(pMin.z, b2.pMin.z);
            ret.pMax.x = max(pMax.x, b2.pMax.x);
            ret.pMax.y = max(pMax.y, b2.pMax.y);
            ret.pMax.z = max(pMax.z, b2.pMax.z);
            ret.centroid = (ret.pMin + ret.pMax) * 0.5;
        }
        return ret;
    }

    /*!
    * \fn BBox Union(const BBox &b1, const BBox &b2)
    *  \brief Union dde deux boîtes englobantes
    *
    *  Fait la fusion de deux boîtes englobantes et renvoie une nouvelle boîte englobante.
    *  \param b1 : Premiere boîte englobante(ne sera pas modifiee)
    *  \param b2 : Deuxieme boîte englobante(ne sera pas modifiee)
    *  \return Renvoie une nouvelle boîte englobant issue de la fusion des deux boîtes englobantes
    */
    BBox Union(const BBox& b1, const BBox& b2)
    {
        BBox ret = b1;
        ret.pMin.x = min(b1.pMin.x, b2.pMin.x);
        ret.pMin.y = min(b1.pMin.y, b2.pMin.y);
        ret.pMin.z = min(b1.pMin.z, b2.pMin.z);
        ret.pMax.x = max(b1.pMax.x, b2.pMax.x);
        ret.pMax.y = max(b1.pMax.y, b2.pMax.y);
        ret.pMax.z = max(b1.pMax.z, b2.pMax.z);
        ret.centroid = (ret.pMin + ret.pMax) * 0.5;
        return ret;
    }

    /*!
    * \fn bool isInBox(const vec3 &p) const
    * \brief Test si un point est dans la boîte englobante
    * \return Renvoie vrai si le point est dans la boîte englobante
    */
    inline bool isInBox(const vec3& p) const
    {
        return p.x >= pMin.x && p.y >= pMin.y && p.z >= pMin.z && p.x <= pMax.x && p.y <= pMax.y && p.z <= pMax.z;
    }

    /*!
    * \fn bool intersectBox(const BBox &box)
    * \brief Test si il y a une intersection entre les boîtes englobantes.
    * \return Renvoie vrai si il y a contact entre les bbox. Renvoie vrai egalement si un boîte est incluse dans l'autre.
    */
    inline bool intersectBox(const BBox& box)
    {
        if (isNull || box.isNull) { return false; }
        bool x = (pMax.x >= box.pMin.x) && (pMin.x <= box.pMax.x);
        bool y = (pMax.y >= box.pMin.y) && (pMin.y <= box.pMax.y);
        bool z = (pMax.z >= box.pMin.z) && (pMin.z <= box.pMax.z);
        return (x && y && z);
    }

    /*!
    *  \fn int MaximumExtend() const
    *  \brief Renvoie la dimension dominante de la boîte englobante
    *
    *  Renvoie la dimension maximale de la boîte englobante. On utilise le meme indice que ceux utilises par les vecteurs. 0 pour x, 1 pour y et 2 pour z
    *  \return Renvoie l'indice de la dimension dominante
    */
    int MaximumExtend() const
    {
        vec3 diag = pMax - pMin;
        if (diag.x > diag.y && diag.x > diag.z)
        {
            return 0;
        }
        else if (diag.y > diag.z)
        {
            return 1;
        }
        else
        {
            return 2;
        }
    }

    /*!
    *  \fn bool IntersectP( Ray *ray, float *hitt0, float *hitt1) const
    *  \brief Calcul l'intersection entre un rayon et une boîte englobante
    *
    *  Calcul l'intersection entre un rayon et une boîte englobante et renvoie si on touche la boîte englobante
    *  \param ray : pointeur vers un rayon avec une origine et une direction
    *  \param hitt0 : pointeur vers un float qui sert a stocker le t du point d'entree du rayon avec la boîte englobante
    *  \param hitt1 : pointeur vers un float qui sert a stocker le t du point de sortie du rayon avec la boîte englobante
    *  \return Renvoie vrai si le rayon touche la boîte englobante, faux dans le cas contraire
    */
    bool IntersectP(vec3 pos, vec3 dir, decimal* hitt0 = NULL, decimal* hitt1 = NULL) const
    {
        if (isNull) { return false; }
        //float t0 = ray->mint; float t1 = ray->maxt;
        decimal t0 = 0, t1 = 1000000;
        decimal invRayDir, tNear, tFar;
        vec3 o = pos;//vec3(ray->position);
        vec3 d = dir;//vec3(ray->direction);

        //Composante sur X
        invRayDir = 1 / d.x;
        tNear = (pMin.x - o.x) * invRayDir;
        tFar = (pMax.x - o.x) * invRayDir;

        if (tNear > tFar) { swap(tNear, tFar); }
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1) { return false; }

        //Composante sur Y
        invRayDir = 1 / d.y;
        tNear = (pMin.y - o.y) * invRayDir;
        tFar = (pMax.y - o.y) * invRayDir;

        if (tNear > tFar) { swap(tNear, tFar); }
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1) { return false; }

        //Composante sur Z
        invRayDir = 1 / d.z;
        tNear = (pMin.z - o.z) * invRayDir;
        tFar = (pMax.z - o.z) * invRayDir;

        if (tNear > tFar) { swap(tNear, tFar); }
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1) { return false; }

        if (hitt0) { *hitt0 = t0; }
        if (hitt1) { *hitt1 = t1; }
        return true;
    }

    /*!
    *  \fn void print()
    *  \brief Fonction d'affichage d'une boîte englobante
    *
    *  Affiche le coin inferieur et superieur de la boîte englobante.
    */
    void print()
    {
        if (isNull)   //std::cout << "Non initialisee." << std::endl;
        {
        }
        else   //std::cout << "Initialisee." << std::endl;
        {
        }
    }

    vec3& operator[](int i)
    {
        if (i == 0) { return pMin; }
        else { return pMax; }
    }
    const vec3& operator[](int i) const
    {
        if (i == 0) { return pMin; }
        else { return pMax; }
    }

    decimal diag() { return sqrt((pMax.x - pMin.x) * (pMax.x - pMin.x) + (pMax.y - pMin.y) * (pMax.y - pMin.y) + (pMax.z - pMin.z) * (pMax.z - pMin.z)); }

    bool Inside(vec3& pt)
    {
        return (pt.x >= pMin.x && pt.x <= pMax.x &&
                pt.y >= pMin.y && pt.y <= pMax.y &&
                pt.z >= pMin.z && pt.z <= pMax.z);
    }

};

#endif // BBOX_H

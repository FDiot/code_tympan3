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

#ifndef __TY_VOLUMEINTERFACE__
#define __TY_VOLUMEINTERFACE__


class OSegment3D;

#include "TYBox.h"
#include "TYPoint.h"
#include "TYGeometryNode.h"


/**
 * Classe abstraite qui offre une interface utilisable par des composants ayant des proprietes geometriques volumiques.
 *
 * @author Projet_Tympan
 *
 */
class TYVolumeInterface
{
    // Methodes
public:
    /**
     * @name Construction
     */
    //@{

    /**
     * Constructeur par defaut.
     */
    TYVolumeInterface() {}
    /**
     * Destructeur.
     */
    virtual ~TYVolumeInterface() {};

    //@}

    /**
     * @name Interface Volume
     */
    //@{

    /**
     * Calcule le volume total de ce volume.
     *
     * @return Le volume total.
     */
    virtual double volume() const = 0;


    /**
     * Calcule la surface totale de ce volume.
     *
     * @return La surface totale.
     */
    virtual double surface() const = 0;

    /**
     * Calcule la surface active de la machine (somme des surfaces des elements actifs)
     *
     * @return surface des elements actifs
     */
    virtual double activeSurface() const = 0;
    virtual double activeSurface() = 0;

    /**
     * Calcule les normales des surfaces composant ce volume.
     *
     * @return Un tableau de vecteurs.
     */
    virtual TYTabVector normals() const = 0;

    /**
     * Retourne l'ensemble des sommets composant ce volume.
     *
     * @return Un tableau de points.
     */
    virtual TYTabPoint sommets() const = 0;

    /**
     * Retourne l'ensemble des faces (polygones) composant ce volume.
     *
     * @return Un tableau de pointeurs de surfaces.
     */
    virtual TYTabLPPolygon faces() const = 0;

    /**
     * Calcule la boite englobant ce volume.
     *
     * @return La boite englobante.
     */
    virtual TYBox volEnglob() const;

    /**
     * Recelcul du volume englobant
     */
    virtual void updateVolEnglob() { _volEnglob = volEnglob(); }

    /**
     * Retourne la boite englobante
     */
    const TYBox& getVolEnglob() const { return _volEnglob; }

    /**
     * Retourne le rayon de la sphere englobant le volume
     */
    const double& getRayonSphere() const { return _rayonSphere; }

    /**
     * Calcul du rayon de la sphere englobante
     */
    void calculRayonSphere(const TYBox& volEnglob);

    /**
     * Calcule le centre de gravite de ce volume.
     *
     * @return Le centre de gravite.
     */
    virtual TYPoint centreGravite() const = 0;

    /**
     * Calcule le centre de gravite du volume
     */
    virtual void calculCentreGravite() { _centreGravite = centreGravite(); }

    /**
     * Retourne le centre de gravite du volume (precalcule);
     */
    const TYPoint& getCentreGravite() const { return _centreGravite; }

    /**
     * Calcule la liste des points d'intersection de ce volume avec le segment passe.
     *
     * @param seg Segment dont on veut l'intersection.
     * @param ptList Les point resultant de l'intersection.
     *
     * @return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    virtual int intersects(const OSegment3D& seg, TYTabPoint& ptList) const;

    /**
     * Test si le point passe se trouve a l'interieur de ce volume.
     *
     * @param pt Point dont on teste l'appartenance au volume.
     *
     * @return INTERS_OUI (1) si le point est a l'interieur de ce volume,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) si le point est a l'exterieur de ce volume.
     */
    virtual int isInside(const TYPoint& pt) const = 0;

    /**
     * Inversion des normales des faces d'un volume
     */
    virtual void inverseNormales() {};

private :
    //@}

protected:

    TYBox _volEnglob;
    double _rayonSphere;
    TYPoint _centreGravite;


};


#endif // __TY_VOLUMEINTERFACE__

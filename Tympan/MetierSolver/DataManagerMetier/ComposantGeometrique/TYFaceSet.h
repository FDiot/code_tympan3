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

#ifndef __TY_FACESET__
#define __TY_FACESET__


#include "TYRepere.h"
#include "TYVolumeInterface.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYFaceSetWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYFaceSetGraphic.h"
#endif

/**
 * Classe de definition d'un volume decrit sous forme d'un ensemble de facettes.
 *
 * @author Projet_Tympan
 *
 */
class TYFaceSet: public TYElement, public TYVolumeInterface
{
    OPROTOSUPERDECL(TYFaceSet, TYElement)
    TY_EXTENSION_DECL(TYFaceSet)
    TY_EXT_GRAPHIC_DECL(TYFaceSet)

    // Methodes
public:
    /**
     * Constructeur par defaut.
     */
    TYFaceSet();
    /**
     * Constructeur par copie.
     */
    TYFaceSet(const TYFaceSet& other);

    /**
     * Destructeur.
     */
    virtual ~TYFaceSet();

    ///Operateur =.
    TYFaceSet& operator=(const TYFaceSet& other);
    ///Operateur ==.
    bool operator==(const TYFaceSet& other) const;
    ///Operateur !=.
    bool operator!=(const TYFaceSet& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    //////////////////////////////////////////////////////
    // Fonctions de l'interface TYVolumeInterface

    virtual double volume() const;
    virtual double surface() const;
    virtual double activeSurface() const;
    virtual double activeSurface();
    virtual TYTabVector normals() const;
    virtual TYTabPoint sommets() const;
    virtual TYTabLPPolygon faces() const;
    virtual TYPoint centreGravite() const;

    /**
     * Calcule la liste des points d'intersection de ce volume avec le segment passe.
     *
     * @param seg Segment dont on veut l'intersection.
     * @param ptList Les points resultant de l'intersection.
     *
     * @return INTERS_OUI (1) si l'intersection existe,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) s'il n'y a pas d'intersection.
     */
    virtual int intersects(const OSegment3D& seg, TYTabPoint& ptList) const;

    /**
     * Test si le point se trouve a l'interieur de ce volume.
     *
     * @param pt Point dont on teste l'appartenance au volume.
     *
     * @return INTERS_OUI (1) si le point est a l'interieur de ce volume,
     *         INTERS_CONFONDU (2) si les elements sont confondus,
     *         INTERS_NULLE (0) si le point est a l'exterieur de ce volume.
     */
    virtual int isInside(const TYPoint& pt) const;

    /////////////////////////////////////////////////////////////
    // Gestion du tableau contenant les faces...

    /**
     * Vide le tableau de points.
     */
    void purge();

    /**
     * Retourne le nombre de points composants ce polygone.
     */
    size_t getNbFaces() const { return _faces.size(); }

    /**
     * Set/Get de la collection de faces.
     */
    void setFaces(const TYTabLPPolygon& faces) { _faces = faces; UpdateInternals(); setIsGeometryModified(true); }

    /**
     * Transforme les points du polygone avec la matrice passee en parametre.
     */
    void transform(const OMatrix& matrix);

    virtual void inverseNormales();

    // Membres
private:

    ///Faces
    TYTabLPPolygon  _faces;
    ///Sommets.
    TYTabPoint      _sommets;
    ///Normales
    TYTabVector     _normals;

    void UpdateInternals();
};


#endif // __TY_FACESET__

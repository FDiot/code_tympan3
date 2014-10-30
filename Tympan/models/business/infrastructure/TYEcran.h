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

#ifndef __TY_ECRAN__
#define __TY_ECRAN__

class OSegment3D;


#include "Tympan/models/business/geometry/TYFaceSet.h"
#include "Tympan/models/business/geoacoustic/TYAcousticFaceSet.h"

/**
 * Classe representant un ecran mince
 */
class TYEcran: public TYAcousticFaceSet
{
    OPROTODECL(TYEcran)
    TY_EXTENSION_DECL_ONLY(TYEcran)
    TY_EXT_GRAPHIC_DECL_ONLY(TYEcran)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYEcran();
    /**
     * Constructeur par copie.
     */
    TYEcran(const TYEcran& other);
    /**
     * Destructeur.
     */
    virtual ~TYEcran();

    /**
     * Operateur de copie.
     */
    TYEcran& operator=(const TYEcran& other);
    /**
     * Operateur de comparaison.
     */
    bool operator==(const TYEcran& other) const;
    /**
     * Operateur de comparaison.
     */
    bool operator!=(const TYEcran& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void getChilds(LPTYElementArray& childs, bool recursif = true);

    virtual int intersects(const OSegment3D& seg, TYTabPoint& ptList) const;

    virtual TYTabAcousticSurfaceGeoNode acousticFaces();

    /**
     * This method defined in TYVolumeInterface is willingly implemented in TYEcran and not in TYFaceSet
     * because TYEcran has always an horizontal lower face, that is not the case for any TYFaceSet
     */
    virtual int isInside(const TYPoint& pt) const;

    void setacousticFacesPourCalcul(bool bPourCalculTrajet);

    /**
     * Retourne un tableau de points representant le contour de l'etage.
     *
     * @return Un tableau de points.
     */
    TYTabPoint getContour() const;

    /**
     * Retourne le tableau des faces infrieurs d'un cran (s'il comporte plusieurs segments)
     */
    TYTabLPPolygon getContours() const;

    /**
     * Etablie la hauteur des murs composant cet etage.
     *
     * @param hauteur La nouvelle hauteur des murs.
     */
    void setHauteur(double hauteur);
    /**
     * Retourne la hauteur des murs composant cet etage.
     *
     * @return La hauteur des murs.
     */
    double getHauteur() const;

    /**
     * Etablie l'epaisseur de l'ecran.
     *
     * @param epaisseur La nouvelle hauteur de l'ecran.
     */
    void setEpaisseur(double epaisseur);
    /**
     * Retourne l'epaisseur de l'ecran.
     *
     * @return L'epaisseur de l'ecran.
     */
    double getEpaisseur() const;

    /**
     * Creer des ecrans selon le trace represente par une serie de points.
     *
     * @param tabPts Un tableau de points decrivant le trace de l'ecran.
     * @param hauteur La hauteur des murs a creer.
     */
    bool setElements(TYTabPoint tabPts, double hauteur = 2.0, double epaisseur = 0.5);

    /**
     * Met a jour la geometrie de l'objet si les parametres de hauteur ou de largeur ont
     * ete modifies.
     */
    void updateGeometry();

protected:

    // Membres
protected:

    /// hauteur de l'ecran
    double  _hauteur;
    /// epaisseur de l'ecran
    double  _epaisseur;

private:

    /// squelette du trace au sol de l'ecran
    /// copie du tableau passe dans setElements()
    TYTabPoint  _squelette;

    /// la hauteur a ete modifiee
    bool _bHauteurModified;
    /// l'epaisseur a ete modifiee
    bool _bEpaisseurModified;

    //Ajoute pour obtenir un contour simple pour les ecrans
    bool _bPourCalculTrajet;

    // Fonctions annexes pour la construction des faces du volume de l'ecran
    TYPolygon* newFace(OVector3D s0, OVector3D s1, OVector3D s2, OVector3D s3);
};


///Noeud geometrique de type TYEcran.
typedef TYGeometryNode TYEcranGeoNode;
///Smart Pointer sur TYEcranGeoNode.
typedef SmartPtr<TYEcranGeoNode> LPTYEcranGeoNode;
///Collection de noeuds geometriques de type TYEcran.
typedef std::vector<LPTYEcranGeoNode> TYTabEcranGeoNode;


#endif // __TY_ECRAN__

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

#ifndef __TY_ACOUSTICBOX__
#define __TY_ACOUSTICBOX__


#include "TYAcousticRectangleNode.h"
#include "TYAcousticVolume.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticBoxWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYAcousticBoxGraphic.h"
#endif


/**
 * Permet des creer des boites acoustiques.
 */
class TYAcousticBox: public TYAcousticVolume
{
    OPROTOSUPERDECL(TYAcousticBox, TYAcousticVolume)
    TY_EXTENSION_DECL(TYAcousticBox)
    TY_EXT_GRAPHIC_DECL(TYAcousticBox)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticBox();
    /**
     * Constructeur par copie.
     */
    TYAcousticBox(const TYAcousticBox& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticBox();

    /**
     * Definit les faces de l'objet a partir de ses dimensions.
     *
     * @param larg La largeur de la boite.
     * @param lon La longueur de la boite.
     * @param haut La hauteur de la boite.
     *
     */
    void setDimension(float larg, float lon, float haut);
    /**
     * Retourne les dimensions de la boite.
     *
     * @param larg La largeur de la boite.
     * @param lon La longueur de la boite.
     * @param haut La hauteur de la boite.
     *
     */
    void getDimension(float& larg, float& lon, float& haut);

    ///Operateur =.
    TYAcousticBox& operator=(const TYAcousticBox& other);
    ///Operateur ==.
    bool operator==(const TYAcousticBox& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticBox& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void getChilds(TYElementCollection& childs, bool recursif = true);

    virtual void setDensiteSrcsH(double densite, bool recursif = true);
    virtual void setDensiteSrcsV(double densite, bool recursif = true);

    virtual void setRegime(TYSpectre& Spectre, int regime = -1, bool recursif = false);
    virtual bool remRegime(int regime);
    virtual void setCurRegime(int regime);
    virtual void loadRegime(int regimeNb = -1);
    //  virtual int addRegime(const TYRegime& regime);
    virtual int addRegime();
    virtual void propagateAtt(LPTYAttenuateur pAtt);
    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual void distriSrcs();
    virtual void remAllSrcs();
    virtual bool setSrcsLw();
    virtual void setIsRayonnant(bool rayonnant = true, bool recursif = true);

    virtual double volume() const;
    virtual double surface() const;
    virtual double activeSurface() const;
    virtual double activeSurface();
    virtual TYTabVector normals() const;
    virtual TYTabPoint sommets() const;
    virtual TYTabLPPolygon faces() const;
    virtual TYBox volEnglob() const;
    virtual TYPoint centreGravite() const;
    virtual int intersects(const OSegment3D& seg, TYTabPoint& ptList) const;
    virtual int isInside(const TYPoint& pt) const;

    virtual TYTabAcousticSurfaceGeoNode acousticFaces();
    virtual bool findAcousticSurface(const TYAcousticSurface* pAccSurf, OMatrix* pMatrix = 0);

    /**
     * Retourne une liste des sous faces porteuses de sources cheminee ou bafflees
     */
    TYTabLPAcousticSurface getSubFace();

    /**
     * Set/Get des faces en fonction de leur indice.
     */
    LPTYAcousticRectangleNodeGeoNode getFace(int numFace) { return _faces[numFace]; }
    /**
     * Set/Get des faces en fonction de leur indice.
     */
    const LPTYAcousticRectangleNodeGeoNode getFace(int numFace) const { return _faces[numFace]; }
    /**
     * Set/Get des faces en fonction de leur indice.
     */
    void setFace(const LPTYAcousticRectangleNode pAccRect, int numFace);

    /**
     * Mise a jour des parametres acoustiques du volume
     */
    bool updateAcoustic(const bool& force = false);

    /**
     * Get/Set du numero de regime suivant
     */
    virtual void setNextRegimeNb(const int& next);

    /**
     * Get/Set du nom du regime
     */
    virtual void setRegimeName(const QString& name);

    /// Export au format csv sur un flux transmis
    virtual void exportCSV(std::ofstream& ofs);

    // Membres
protected:
    LPTYAcousticRectangleNodeGeoNode _faces[6];

    TYAcousticRectangleNode* getFaceElement(unsigned long index) const
    {
        TYElement* pElement = _faces[index]->getElement();
        TYAcousticRectangleNode* pRectangleElement = TYAcousticRectangleNode::safeDownCast(pElement);

        return pRectangleElement;
    }
};


///Noeud geometrique de type TYAcousticBox.
typedef TYGeometryNode TYAcousticBoxGeoNode;
///Smart Pointer sur TYAcousticBoxGeoNode.
typedef SmartPtr<TYAcousticBoxGeoNode> LPTYAcousticBoxGeoNode;
///Collection de noeuds geometriques de type TYAcousticBox.
typedef std::vector<LPTYAcousticBoxGeoNode> TYTabAcousticBoxGeoNode;


#endif // __TY_ACOUSTICBOX__

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

#ifndef __TY_ACOUSTICCYLINDER__
#define __TY_ACOUSTICCYLINDER__

#include "TYAcousticCircle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourceSurfacic.h"
#include "TYAcousticVolume.h"


/**
 * Permet de creer des volumes acoustiques cylindriques.
 */
class TYAcousticCylinder: public TYAcousticVolume
{
    OPROTOSUPERDECL(TYAcousticCylinder, TYAcousticVolume)
    TY_EXTENSION_DECL_ONLY(TYAcousticCylinder)
    TY_EXT_GRAPHIC_DECL_ONLY(TYAcousticCylinder)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticCylinder();
    /**
     * Constructeur par copie.
     */
    TYAcousticCylinder(const TYAcousticCylinder& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticCylinder();

    ///Operateur =.
    TYAcousticCylinder& operator=(const TYAcousticCylinder& other);
    ///Operateur ==.
    bool operator==(const TYAcousticCylinder& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticCylinder& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void getChilds(LPTYElementArray& childs, bool recursif = true);

    virtual void setDensiteSrcsH(double densite, bool recursif = true);
    virtual void setDensiteSrcsV(double densite, bool recursif = true);

    //  virtual int addRegime(const TYRegime& regime);
    virtual int addRegime();
    virtual void setRegime(TYSpectre& Spectre, int regime = -1, bool recursif = false);
    virtual bool remRegime(int regime);
    virtual void loadRegime(int regimeNb = -1);
    virtual void setCurRegime(int regime);
    virtual void propagateAtt(LPTYAttenuateur pAtt);
    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual void distriSrcs();
    virtual void remAllSrcs();
    virtual bool setSrcsLw();
    virtual void setIsRayonnant(bool rayonnant = true, bool recursif = true);

    virtual double volume() const;
    virtual double surface() const;
    virtual double activeSurface() const ;
    virtual double activeSurface() ;
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
     * Mise a jour des parametres acoustiques du volume
     */
    bool updateAcoustic(const bool& force = false);

    /**
     * Retourne une liste des sous faces porteuses de sources cheminee ou bafflees
     */
    TYTabLPAcousticSurface getSubFace();

    /**
     * Retourne le diametre du cercle du bas.
     *
     * @return Le diametre du cercle du bas.
     */
    double getDiameter() const;
    /**
     * Fixe le diametre des 2 cercles.
     *
     * @param diameter Le nouveau diametre.
     */
    void setDiameter(double diameter);

    /**
     * Retourne la hauteur de ce cylindre, elle correspond a la distance
     * entre les centres des cercles du cylindre.
     *
     * @return La hauteur du cylindre.
     */
    double getHauteur() const;
    /**
     * Fixe la hauteur du cylindre.
     * Seule la position du cercle de dessus est modifiee, les 2 cercles
     * du cylindre sont paralleles.
     *
     * @param hauteur La nouvelle hauteur du cylindre.
     */
    void setHauteur(double hauteur);

    /**
     * Set/Get de l'element SourceSurfacic.
     */
    LPTYSourceSurfacic getSrcSurf() { return _pEnveloppe->getSrcSurf(); }
    /**
     * Set/Get de l'element SourceSurfacic.
     */
    const LPTYSourceSurfacic getSrcSurf() const { return _pEnveloppe->getSrcSurf(); }
    /**
     * Set/Get de l'element SourceSurfacic.
     */
    void setSrcSurf(const LPTYSourceSurfacic pSrc) { _pEnveloppe->setSrcSurf(pSrc); }

    /**
     * Set/Get de la surface de dessus.
     */
    LPTYAcousticCircle getCircTop() { return _pCircTop; }
    /**
     * Set/Get de la surface de dessus.
     */
    const LPTYAcousticCircle getCircTop() const { return _pCircTop; }
    /**
     * Set/Get de la surface de dessus.
     */
    void setCircTop(const LPTYAcousticCircle pCirc)
    {
        _pCircTop = pCirc;
        _pCircTop->setParent(this);
        setIsGeometryModified(true);
    }

    /**
     * Set/Get de la surface de dessous.
     */
    LPTYAcousticCircle getCircBottom() { return _pCircBottom; }
    /**
     * Set/Get de la surface de dessous.
     */
    const LPTYAcousticCircle getCircBottom() const { return _pCircBottom; }
    /**
     * Set/Get de la surface de dessous.
     */
    void setCircBottom(const LPTYAcousticCircle pCirc)
    {
        _pCircBottom = pCirc;
        _pCircBottom->setParent(this);
        setIsGeometryModified(true);
    }

    /**
     * Get/Set de l'enveloppe
     * On utilise getAcEnveloppe au lieu de getEnveloppe qui paraitrait plus naturel
     * a cause d'une ambiguite avec une commande getEnveloppe introduite anterieurement
     * dans le code pour l'affichage et qu'on ne souhaite pas modifier pour limiter
     * l'impact de la modification
     */
    void setAcEnveloppe(const LPTYAcousticSurface pEnv) { _pEnveloppe = pEnv; }
    LPTYAcousticSurface getAcEnveloppe() { return _pEnveloppe; }
    const LPTYAcousticSurface getAcEnveloppe() const { return _pEnveloppe; }

    /**
     * Genere des rectangles pour representer l'enveloppe de ce cylindre.
     *
     * @param n Le nombre de rectangles a generer.
     *          Si n=-1, n sera calcule a partir de la precision de distance definie par defaut.
     *
     * @return Un tableau de rectangles.
     */
    virtual TYTabRectangle getEnveloppe(int n = -1) const;

    /**
     * Calcul le centre de ce cylindre.
     *
     * @return Le centre de ce cylindre.
     */
    virtual TYPoint getCenter() const;

    /**
     * Get/Set du numero de regime suivant
     */
    virtual void setNextRegimeNb(const int& next);

    /**
     * Get/Set du nom du regime
     */
    virtual void setRegimeName(const QString& name);

    /**
     * Calcul de la surface de l'enveloppe
     */
    void updateSurfEnveloppe();

    /// Export au format csv sur un flux transmis
    virtual void exportCSV(std::ofstream& ofs);

    // Membres
protected:
    /// Enveloppe
    LPTYAcousticSurface _pEnveloppe;
    ///Surface de dessus.
    LPTYAcousticCircle _pCircTop;
    ///Surface de dessous.
    LPTYAcousticCircle _pCircBottom;
};


///Noeud geometrique de type TYAcousticCylinder.
typedef TYGeometryNode TYAcousticCylinderGeoNode;
///Smart Pointer sur TYAcousticCylinderGeoNode.
typedef SmartPtr<TYAcousticCylinderGeoNode> LPTYAcousticCylinderGeoNode;
///Collection de noeuds geometriques de type TYAcousticCylinder.
typedef std::vector<LPTYAcousticCylinderGeoNode> TYTabAcousticCylinderGeoNode;


#endif // __TY_ACOUSTICCYLINDER__

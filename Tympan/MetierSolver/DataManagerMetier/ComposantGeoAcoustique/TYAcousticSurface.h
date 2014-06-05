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

#ifndef __TY_ACOUSTICSURFACE__
#define __TY_ACOUSTICSURFACE__


#include "Tympan/MetierSolver/DataManagerCore/TYColorInterface.h"
#include "TYAcousticInterface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYSurfaceInterface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourceSurfacic.h"
#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRectangle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYGeometryNode.h"


/**
 * Permet de creer de elements acoustiques surfaciques.
 */
class TYAcousticSurface: public TYElement, public TYColorInterface, public TYAcousticInterface, public TYSurfaceInterface
{
    OPROTODECL(TYAcousticSurface)
    TY_EXTENSION_DECL_ONLY(TYAcousticSurface)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticSurface();
    /**
     * Constructeur par copie.
     */
    TYAcousticSurface(const TYAcousticSurface& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticSurface();

    ///Operateur =.
    TYAcousticSurface& operator=(const TYAcousticSurface& other);
    ///Operateur ==.
    bool operator==(const TYAcousticSurface& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticSurface& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void setIsAcousticModified(bool isModified);

    virtual void setDensiteSrcsH(double densite, bool recursif = true);
    virtual void setDensiteSrcsV(double densite, bool recursif = true);

    virtual void propagateRegime();
    virtual void propagateAtt(LPTYAttenuateur pAtt);
    virtual void setCurRegime(int regime);
    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual void distriSrcs();
    virtual void remAllSrcs();
    virtual bool setSrcsLw();
    virtual bool updateAcoustic(const bool& force = false);

    virtual double surface() const;
    virtual OVector3D normal() const;
    virtual OPlan plan() const;
    virtual TYTabPoint getContour(int n = -1) const;
    virtual TYTabPoint3D getOContour(int n = -1) const;
    virtual int intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const;
    virtual int intersects(const OSegment3D& seg, OPoint3D& pt) const;
    virtual int intersects(const OPoint3D& pt) const;

    /**
     * Get du spectre de puissance avec son attenuateur eventuel
     */
    virtual LPTYSpectre getRealPowerSpectrum();

    /**
     * retourne le plan associe a cette surface
     */
    virtual const OPlan& getPlan() const { return _pBoundingRect->getPlan(); }

    /**
     * Set/Get de la source surfacique.
     */
    LPTYSourceSurfacic getSrcSurf() { return _pSrcSurf; }
    /**
     * Set/Get de la source surfacique.
     */
    const LPTYSourceSurfacic getSrcSurf() const { return _pSrcSurf; }
    /**
     * Set/Get de la source surfacique.
     */
    void setSrcSurf(const LPTYSourceSurfacic pSrcSurf) { _pSrcSurf = pSrcSurf; _pSrcSurf->setParent(this); }

    /**
     * Set/Get du rectangle englobant.
     */
    TYRectangle* getBoundingRect() { return _pBoundingRect; }

    /**
     * Set/Get de l'indicateur qui precise si la surface est un sous-element d'une face.
     */
    bool getIsSub() { return _isSub; }
    /**
     * Set/Get de l'indicateur qui precise si la surface est un sous-element d'une face.
     */
    void setIsSub(bool isSub) { _isSub = isSub;}

    /**
     * Retourne le materiau de construction associe a cette Surface
     * via son volume parent, s'il existe.
     */
    LPTYMateriauConstruction getMateriau();
    /**
     * Retourne le materiau de construction associe a cette Surface
     * via son volume parent, s'il existe.
     */
    const LPTYMateriauConstruction getMateriau() const;

    /**
     * Indique a la surface le spectre global du volume pour lui
     * permettre de calculer sa puissance (connaissant sa surface
     * et la surface globale)
     * @param le spectre global du volume
     * @return le spectre de la surface
     */
    TYSpectre setGlobalLW(const TYSpectre& spectre, const double& surfGlobale, const int& regime = -1);

    /// Get/set de l'etat rayonnant ou pas
    virtual void setIsRayonnant(bool rayonnant = true, bool recursif = true);


    /// Export au format csv sur un flux transmis
    virtual void exportCSV(std::ofstream& ofs);

    // Membres
protected:
    ///Source surfacique.
    LPTYSourceSurfacic _pSrcSurf;

    ///Rectangle englobant.
    LPTYRectangle _pBoundingRect;

    ///Indicateur qui precise si la surface est un sous-element d'une face (fenetre ou bouche d'aeration).
    bool _isSub;
};

///Tableau de TYAcousticSurfaces
typedef std::vector<LPTYAcousticSurface> TYTabLPAcousticSurface;
///Noeud geometrique de type TYAcousticSurface.
typedef TYGeometryNode TYAcousticSurfaceGeoNode;
///Smart Pointer sur TYAcousticSurfaceGeoNode.
typedef SmartPtr<TYAcousticSurfaceGeoNode> LPTYAcousticSurfaceGeoNode;
///Collection de noeuds geometriques de type TYAcousticSurface.
typedef std::vector<LPTYAcousticSurfaceGeoNode> TYTabAcousticSurfaceGeoNode;


#endif // __TY_ACOUSTICSURFACE__


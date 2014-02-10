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

#ifndef __TY_ACOUSTICSURFACENODE__
#define __TY_ACOUSTICSURFACENODE__


#include "Tympan/MetierSolver/DataManagerCore/TYColorInterface.h"
#include "TYAcousticInterface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYSurfaceInterface.h"
#include "TYAcousticSurface.h"
#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYRectangle.h"
#include "TYAcousticCircle.h"
#include "TYAcousticSemiCircle.h"
#include "TYAcousticRectangle.h"
#include "TYAcousticVolume.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticSurfaceNodeWidget.h"
#endif


/**
 * Composee d'une ou plusieurs surfaces acoustiques..
 */
class TYAcousticSurfaceNode: public TYElement, public TYColorInterface, public TYAcousticInterface, public TYSurfaceInterface
{
    OPROTOSUPERDECL(TYAcousticSurfaceNode, TYElement)
    TY_EXTENSION_DECL(TYAcousticSurfaceNode)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticSurfaceNode();
    /**
     * Constructeur par copie.
     */
    TYAcousticSurfaceNode(const TYAcousticSurfaceNode& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticSurfaceNode();

    ///Operateur =.
    TYAcousticSurfaceNode& operator=(const TYAcousticSurfaceNode& other);
    ///Operateur ==.
    bool operator==(const TYAcousticSurfaceNode& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticSurfaceNode& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void setIsAcousticModified(bool isModified);

    virtual void getChilds(TYElementCollection& childs, bool recursif = true);

    virtual void setDensiteSrcsH(double densite, bool recursif = true);
    virtual void setDensiteSrcsV(double densite, bool recursif = true);

    //  virtual int addRegime(const TYRegime& regime);
    virtual int addRegime();
    virtual void setRegime(TYSpectre& Spectre, int regime = -1, bool recursif = false);
    virtual bool remRegime(int regime);
    virtual void loadRegime(int regimeNb = -1);
    virtual void setCurRegime(int regime);

    /**
     * Correction "ad hoc" du bug de nombre de regime pour les sources machines et bâtiments
     * Supprime les regimes excedentaires
     * Necessaire uniquement pour les anciennes versions de fichiers XML (retrocompatibilite)
     */
    virtual void correctNbRegimes();

    virtual void propagateAtt(LPTYAttenuateur pAtt);
    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual void distriSrcs();
    virtual void remAllSrcs();
    virtual bool setSrcsLw();
    virtual bool updateAcoustic(const bool& force = false);
    virtual void setIsRayonnant(bool rayonnant = true, bool recursif = true);

    virtual double surface() const;
    virtual double activeSurface() const;
    virtual OVector3D normal() const;
    virtual OPlan plan() const;
    virtual TYTabPoint getContour(int n = -1) const;
    virtual TYTabPoint3D getOContour(int n = -1) const;
    virtual int intersects(const TYSurfaceInterface* pSurf, OSegment3D& seg) const;
    virtual int intersects(const OSegment3D& seg, OPoint3D& pt) const;
    virtual int intersects(const OPoint3D& pt) const;
    virtual void setUseAtt(bool state);

    /**
     * retourne le plan associe a cette surface
     */
    virtual const OPlan& getPlan() const { return _pBoundingRect->getPlan(); }

    /**
     * retourne la surface (en m²) rayonnante
     */
    virtual double activeSurface();

    /**
     * Appelle le destructeur de chaque element contenus dans le tableau
     * d'acoustic surface, et vide celui-ci.
     */
    void purge();

    /**
     * Set/Get du ombre de AcousticSurfaces associes.
     */
    size_t getNbChild() const { return _tabAcousticSurf.size(); }

    /**
     * Set/Get du tableau des AcousticSurfaces.
     */
    TYTabAcousticSurfaceGeoNode& getTabAcousticSurf() { return _tabAcousticSurf; }
    /**
     * Set/Get du tableau des AcousticSurfaces.
     */
    const TYTabAcousticSurfaceGeoNode& getTabAcousticSurf() const { return _tabAcousticSurf; }
    /**
     * Set/Get du tableau des AcousticSurfaces.
     */
    void setTabAcousticSurf(const TYTabAcousticSurfaceGeoNode& tab) { _tabAcousticSurf = tab; }

    /**
     * Set/Get du rectangle englobant.
     */
    LPTYRectangle getBoundingRect() { return _pBoundingRect; setIsGeometryModified(true); }
    /**
     * Set/Get du rectangle englobant.
     */
    const LPTYRectangle getBoundingRect() const { return _pBoundingRect; }

    /**
     * Ajout d'un AcousticSurfacepAccSurfGeoNode au tableau des AcousticSurfaces.
     *
     * @param pAccSurfGeoNode Element a ajouter.
     *
     * @return <code>true</code> si l'element a ete correctement ajoute;
     *         <code>false</code> sinon.
     */
    bool addAcousticSurf(LPTYAcousticSurfaceGeoNode pAccSurfGeoNode);

    /**
     * Ajout d'un AcousticSurface au tableau des AcousticSurfaces.
     *
     * @param pAccSurf Element a ajouter.
     * @param repere Repere a associer a l'element.
     *
     * @return <code>true</code> si l'element a ete correctement ajoute;
     *         <code>false</code> sinon.
     */
    bool addAcousticSurf(LPTYAcousticSurface pAccSurf, const TYRepere& repere);

    /**
     * Ajout d'un AcousticSurface au tableau des AcousticSurfaces.
     *
     * @param pAccSurf Element a ajouter.
     *
     * @return <code>true</code> si l'element a ete correctement ajoute;
     *         <code>false</code> sinon.
     */
    bool addAcousticSurf(LPTYAcousticSurface pAccSurf);

    /**
     * Suppression d'un AcousticSurfaceGeoNode au tableau des AcousticSurfaces.
     *
     * @param pAccSurfGeoNode Element a supprimer.
     *
     * @return <code>true</code> si l'element a ete trouve et supprime;
     *         <code>false</code> sinon.
     */
    virtual bool remAcousticSurf(const LPTYAcousticSurfaceGeoNode pAccSurfGeoNode);

    /**
     * Suppression d'un AcousticSurface au tableau des AcousticSurfaces.
     *
     * @param pAccSurf Element a supprimer.
     *
     * @return <code>true</code> si l'element a ete trouve et supprime;
     *         <code>false</code> sinon.
     */
    virtual bool remAcousticSurf(const LPTYAcousticSurface pAccSurf);

    /**
     * Suppression d'un AcousticSurface au tableau des AcousticSurfaces
     * a partir de son identifiant.
     *
     * @param idAccSurf Identifiant de l'element a supprimer.
     *
     * @return <code>true</code> si l'element a ete trouve et supprime;
     *         <code>false</code> sinon.
     */
    virtual bool remAcousticSurf(QString idAccSurf);

    /**
     * Retourne une surface acoustique en fonction de son indice dans
     * le tableau.
     *
     * @param index Indice dans le tableau.
     *
     * @return Un surface acoustique ou NULL si l'index n'est pas valide.
     */
    LPTYAcousticSurface getAcousticSurf(int index);

    /**
     * Retrouve le GeoNode associe a une acoustic surface.
     *
     * @param pAccSurf L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYAcousticSurfaceGeoNode findAcousticSurf(const LPTYAcousticSurface pAccSurf);

    /**
     * Attribue la puissance au pro rata de la surface de l'element
     */
    TYSpectre setGlobalLW(const TYSpectre& spectre, const double& surfGlobale, const int& regime = -1);

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
    ///Tableau des AcousticSurfaces.
    TYTabAcousticSurfaceGeoNode _tabAcousticSurf;

    ///Rectangle englobant.
    LPTYRectangle _pBoundingRect;
};


///Noeud geometrique de type TYAcousticSurfaceNode.
typedef TYGeometryNode TYAcousticSurfaceNodeGeoNode;
///Smart Pointer sur TYAcousticSurfaceNodeGeoNode.
typedef SmartPtr<TYAcousticSurfaceNodeGeoNode> LPTYAcousticSurfaceNodeGeoNode;
///Collection de noeuds geometriques de type TYAcousticSurfaceNode.
typedef std::vector<LPTYAcousticSurfaceNodeGeoNode> TYTabAcousticSurfaceNodeGeoNode;


#endif // __TY_ACOUSTICSURFACENODE__

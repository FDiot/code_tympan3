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

#ifndef __TY_ACOUSTICFACESET__
#define __TY_ACOUSTICFACESET__

#include "Tympan/MetierSolver/ToolsMetier/OSegment3D.h"
#include "TYAcousticVolume.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYFaceSet.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYMateriauConstruction.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourcePonctuelle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYSegment.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYAcousticFaceSetWidget.h"
#endif


/**
 * Permet de creer de elements acoustiques volumiques.
 */
class TYAcousticFaceSet: public TYAcousticVolume
{
    OPROTOSUPERDECL(TYAcousticFaceSet, TYAcousticVolume)
    TY_EXTENSION_DECL(TYAcousticFaceSet)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticFaceSet();
    /**
     * Constructeur par copie.
     */
    TYAcousticFaceSet(const TYAcousticFaceSet& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticFaceSet();

    ///Operateur =.
    TYAcousticFaceSet& operator=(const TYAcousticFaceSet& other);
    ///Operateur ==.
    bool operator==(const TYAcousticFaceSet& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticFaceSet& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);
    virtual DOM_Element toXML(DOM_Element& domElement, bool withfaces);
    virtual int fromXML(DOM_Element domElement, bool withfaces);

    virtual void getChilds(TYElementCollection& childs, bool recursif = true);

    virtual void propagateRegime();
    virtual void propagateAtt(LPTYAttenuateur pAtt);
    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual void distriSrcs();
    virtual void remAllSrcs();
    virtual bool setSrcsLw();
    virtual bool updateAcoustic(const bool& force = false);


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
    virtual int intersects(const TYSegment& seg, TYTabPoint& ptList) const;
    virtual int isInside(const TYPoint& pt) const;

    virtual void setIsRayonnant(bool rayonnant = true, bool recursif = true);

    /**
     * Retourne une liste des sous faces porteuses de sources cheminee ou bafflees
     */
    virtual TYTabLPAcousticSurface getSubFace();

    /**
     * Retourne l'ensemble des faces acoustiques composant ce volume.
     * Le GeoNode associe permet de convertir la face dans le repere
     * de ce volume.
     *
     * @return Un tableau de surfaces acoustiques.
     */
    virtual TYTabAcousticSurfaceGeoNode acousticFaces();

    /**
     * Recherche si une face acoustique compose directement ou indirectement
     * ce volume acoustique. Si une matrice est passee en parametre elle sera
     * calculee pour effectuer le changement de repere de la face vers le volume.
     *
     * @param pAccSurf La surface acoustique a rechercher.
     * @param pMatrix La matrice a mettre a jour.
     *
     * @return <code>true</code> si la face est un composant de ce volume;
     *         <code>false</code> sinon.
     */
    virtual bool findAcousticSurface(const TYAcousticSurface* pAccSurf, OMatrix* pMatrix = 0);

    /**
     * Retourne la puissance apparente de la source ponctuelle passee
     * dans la direction donnee par le segment passe.
     *
     * @param pSrc  Source ponctuelle dont on veut la puissance.
     * @param seg   Direction.
     *
     * @return La puissance apparente resultante.
     */
    virtual double lwApp(const TYSourcePonctuelle* pSrc, TYSegment seg) const;

    /**
     * Indique au volume le spectre global de la machine (volumeNode) pour lui
     * permettre de calculer sa puissance (connaissant sa surface et la surface globale)
     * @param le spectre global de la machine
     * @return le spectre du volume
     */
    virtual TYSpectre setGlobalLW(const TYSpectre& spectre, const double& surfGlobale, const int& regime = -1);


    /**
     * Set/Get du materiau de construction pour les faces de ce Volume.
     */
    LPTYMateriauConstruction getMateriau() { return _pMateriau; }
    /**
     * Set/Get du materiau de construction pour les faces de ce Volume.
     */
    const LPTYMateriauConstruction getMateriau() const { return _pMateriau; }
    /**
     * Set/Get du materiau de construction pour les faces de ce Volume.
     */
    void setMateriau(LPTYMateriauConstruction pMat);

    /**
     * Vide le tableau de faces.
     */
    void delFaces() { _faces.purge(); }

    /**
     * Retourne le nombre de points composants ce polygone.
     */
    size_t getNbFaces() const { return _faces.getNbFaces(); }

    /**
     * Set/Get de la collection de faces.
     */
    void setFaces(const TYTabLPPolygon& faces) { _faces.setFaces(faces); setIsGeometryModified(true); }

    TYFaceSet* getFaceSet() { return &_faces; }

    /**
     * Inversion de la normale des faces
     */
    virtual void inverseNormales() { _faces.inverseNormales(); }


    // Membres
protected:

    /// Ensemble des faces
    TYFaceSet   _faces;
};


///Noeud geometrique de type TYAcousticFaceSet.
typedef TYGeometryNode TYAcousticFaceSetGeoNode;
///Smart Pointer sur TYAcousticFaceSetGeoNode.
typedef SmartPtr<TYAcousticFaceSetGeoNode> LPTYAcousticFaceSetGeoNode;
///Collection de noeuds geometriques de type TYAcousticFaceSet.
typedef std::vector<LPTYAcousticFaceSetGeoNode> TYTabAcousticVolumeGeoNode;


#endif // __TY_ACOUSTICFACESET__

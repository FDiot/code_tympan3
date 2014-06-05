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

#ifndef __TY_ACOUSTICVOLUME__
#define __TY_ACOUSTICVOLUME__

#include "TYAcousticSurface.h"
#include "Tympan/MetierSolver/DataManagerCore/TYColorInterface.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYVolumeInterface.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYMateriauConstruction.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYSourcePonctuelle.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeometrique/TYSegment.h"


/**
 * Permet de creer de elements acoustiques volumiques.
 */
class TYAcousticVolume: public TYElement, public TYColorInterface, public TYAcousticInterface, public TYVolumeInterface
{
    OPROTOSUPERDECL(TYAcousticVolume, TYElement)
    TY_EXTENSION_DECL_ONLY(TYAcousticVolume)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticVolume();
    /**
     * Constructeur par copie.
     */
    TYAcousticVolume(const TYAcousticVolume& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticVolume();

    ///Operateur =.
    TYAcousticVolume& operator=(const TYAcousticVolume& other);
    ///Operateur ==.
    bool operator==(const TYAcousticVolume& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticVolume& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void setIsAcousticModified(bool isModified);


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
     * Indique au volume le spectre global de la machine (volumeNode) pour lui
     * permettre de calculer sa puissance (connaissant sa surface et la surface globale)
     * @param le spectre global de la machine
     * @return le spectre du volume
     */
    virtual TYSpectre setGlobalLW(const TYSpectre& spectre, const double& surfGlobale, const int& regime = -1);


    /**
     * Set/Get du materiau de construction pour les faces de ce Volume.
     */
    TYMateriauConstruction* getMateriau() const { return _pMateriau._pObj; }

    /**
     * Set/Get du materiau de construction pour les faces de ce Volume.
     */
    void setMateriau(LPTYMateriauConstruction pMat);

    /**
     * Get/Set du numero de regime suivant
     */
    virtual void setNextRegimeNb(const int& next) { _nextRegime = next; }

    /**
     * Get/Set du nom du regime
     */
    virtual void setRegimeName(const QString& name);

    /**
     * Inversion de la normale des faces
     */
    virtual void inverseNormales();

    /**
     * Changement d'etat du statut de la normale
     */
    void setNormalStatus() { _forceNormales = !_forceNormales; }

    /**
     * Correction "ad hoc" du bug de nombre de regime pour les sources machines et bâtiments
     * Supprime les regimes excedentaires
     * Necessaire uniquement pour les anciennes versions de fichiers XML (retrocompatibilite)
     */
    virtual void correctNbRegimes();

    // Membres
protected:
    ///Materiau de contruction pour les faces de ce Volume.
    LPTYMateriauConstruction _pMateriau;

    /// Persistence du forcage des normales
    bool _forceNormales;
};


///Noeud geometrique de type TYAcousticVolume.
typedef TYGeometryNode TYAcousticVolumeGeoNode;
///Smart Pointer sur TYAcousticVolumeGeoNode.
typedef SmartPtr<TYAcousticVolumeGeoNode> LPTYAcousticVolumeGeoNode;
///Collection de noeuds geometriques de type TYAcousticVolume.
typedef std::vector<LPTYAcousticVolumeGeoNode> TYTabAcousticVolumeGeoNode;


#endif // __TY_ACOUSTICVOLUME__


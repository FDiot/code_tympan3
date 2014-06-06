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

#ifndef __TY_ACOUSTICVOLUMENODE__
#define __TY_ACOUSTICVOLUMENODE__


#include "TYAcousticCylinder.h"
#include "TYAcousticSemiCylinder.h"
#include "TYAcousticBox.h"


/**
 * Composee d'un ou plusieurs volumes acoustiques..
 */
class TYAcousticVolumeNode: public TYElement, public TYColorInterface, public TYAcousticInterface, public TYVolumeInterface
{
    OPROTOSUPERDECL(TYAcousticVolumeNode, TYElement)
    TY_EXTENSION_DECL_ONLY(TYAcousticVolumeNode)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticVolumeNode();
    /**
     * Constructeur par copie.
     */
    TYAcousticVolumeNode(const TYAcousticVolumeNode& other);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticVolumeNode();

    ///Operateur =.
    TYAcousticVolumeNode& operator=(const TYAcousticVolumeNode& other);
    ///Operateur ==.
    bool operator==(const TYAcousticVolumeNode& other) const;
    ///Operateur !=.
    bool operator!=(const TYAcousticVolumeNode& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void getChilds(LPTYElementArray& childs, bool recursif = true);

    virtual void setDensiteSrcsH(double densite, bool recursif = true);
    virtual void setDensiteSrcsV(double densite, bool recursif = true);
    virtual void propagateAtt(LPTYAttenuateur pAtt);

    virtual int addRegime();
    virtual void setRegime(TYSpectre& Spectre, int regime = -1, bool recursif = false);
    virtual bool remRegime(int regime);
    virtual void loadRegime(int regimeNb = -1);
    virtual void setCurRegime(int regime);
    virtual void setRegimeName(const QString& name);

    /**
     * Correction "ad hoc" du bug de nombre de regime pour les sources machines et bâtiments
     * Supprime les regimes excedentaires
     * Necessaire uniquement pour les anciennes versions de fichiers XML (retrocompatibilite)
     */
    virtual void correctNbRegimes();

    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual void distriSrcs();
    virtual void remAllSrcs();
    virtual bool setSrcsLw();
    virtual bool updateAcoustic(const bool& force = false);
    virtual void setIsRayonnant(bool rayonnant = true, bool recursif = true);


    virtual double volume() const;
    virtual double surface() const;
    virtual double activeSurface() const;
    virtual double activeSurface() ;
    virtual TYTabVector normals() const;
    virtual TYTabPoint sommets() const;
    virtual TYTabLPPolygon faces() const;
    virtual TYBox volEnglob() const;
    virtual TYPoint centreGravite() const;
    virtual int intersects(const OSegment3D& seg, TYTabPoint& ptList) const;
    virtual int isInside(const TYPoint& pt) const;

    /**
     * Get du spectre de puissance avec son attenuateur eventuel
     */
    virtual LPTYSpectre getRealPowerSpectrum();

    /**
     * Retourne l'ensemble des faces acoustiques composant ce volume.
     * Le GeoNode associe permet de convertir la face dans le repere
     * de ce volume.
     *
     * @return Un tableau de surfaces acoustiques.
     */
    virtual TYTabAcousticSurfaceGeoNode acousticFaces();

    /**
     * Appelle le destructeur de chaque element contenus dans le tableau
     * d'acoustic volume, et vide celui-ci.
     */
    void purge();

    /**
     * Set/Get du nombre de AcousticVolumes associes.
     */
    size_t getNbChild() const { return _tabAcousticVol.size(); }

    /**
     * Set/Get du tableau des AcousticVolumes.
     */
    TYTabAcousticVolumeGeoNode& getTabAcousticVol() { return _tabAcousticVol; }
    /**
     * Set/Get du tableau des AcousticVolumes.
     */
    const TYTabAcousticVolumeGeoNode& getTabAcousticVol() const { return _tabAcousticVol; }
    /**
     * Set/Get du tableau des AcousticVolumes.
     */
    void setTabAcousticVol(const TYTabAcousticVolumeGeoNode& tab) { _tabAcousticVol = tab; }

    /**
     * Ajout d'un AcousticVolume au tableau des AcousticVolumes.
     *
     * @param pAccVolGeoNode Element a ajouter.
     * @param recursif booleen indique si le volume doit heriter des proprietes acoustiques du pere
     *
     * @return <code>true</code> si l'element a ete correctement ajoute;
     *         <code>false</code> sinon.
     */
    bool addAcousticVol(LPTYAcousticVolumeGeoNode pAccVolGeoNode, bool recursif = true);

    /**
     * Ajout d'un AcousticVolume au tableau des AcousticVolumes.
     *
     * @param pAccVol Element a ajouter.
     * @param repere Repere a associer a l'element.
     *
     * @return <code>true</code> si l'element a ete correctement ajoute;
     *         <code>false</code> sinon.
     */
    bool addAcousticVol(LPTYAcousticVolume pAccVol, const TYRepere& repere, bool recursif = true);

    /**
     * Ajout d'un AcousticVolume au tableau des AcousticVolumes.
     *
     * @param pAccVol Element a ajouter.
     *
     * @return <code>true</code> si l'element a ete correctement ajoute;
     *         <code>false</code> sinon.
     */
    bool addAcousticVol(LPTYAcousticVolume pAccVol, bool recursif = true);

    /**
     * Suppression d'un AcousticVolumeGeoNode au tableau des AcousticVolumes.
     *
     * @param pAccVolGeoNode Element a supprimer.
     *
     * @return <code>true</code> si l'element a ete trouve et supprime;
     *         <code>false</code> sinon.
     */
    bool remAcousticVol(const LPTYAcousticVolumeGeoNode pAccVolGeoNode);

    /**
     * Suppression d'un AcousticVolume au tableau des AcousticVolumes.
     *
     * @param pAccVol Element a supprimer.
     *
     * @return <code>true</code> si l'element a ete trouve et supprime;
     *         <code>false</code> sinon.
     */
    bool remAcousticVol(const LPTYAcousticVolume pAccVol);

    /**
     * Suppression d'un AcousticVolume au tableau des AcousticVolumes
     * a partir de son identifiant.
     *
     * @param idAccVol Identifiant de l'element a supprimer.
     *
     * @return <code>true</code> si l'element a ete trouve et supprime;
     *         <code>false</code> sinon.
     */
    bool remAcousticVol(QString idAccVol);

    /**
     * Retourne un volume acoustique en fonction de son indice dans
     * le tableau.
     *
     * @param index Indice dans le tableau.
     *
     * @return Un volume acoustique ou NULL si l'index n'est pas valide.
     */
    LPTYAcousticVolume getAcousticVol(int index);

    /**
     * Retrouve le GeoNode associe a un acoustic volume.
     *
     * @param pAccVol L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYAcousticVolumeGeoNode findAcousticVol(const LPTYAcousticVolume pAccVol);

    /// Export au format csv sur un flux transmis
    virtual void exportCSV(std::ofstream& ofs);

    // Membres
protected:
    ///Tableau des AcousticVolumes.
    TYTabAcousticVolumeGeoNode _tabAcousticVol;
};


///Noeud geometrique de type TYAcousticVolumeNode.
typedef TYGeometryNode TYAcousticVolumeNodeGeoNode;
///Smart Pointer sur TYAcousticVolumeNodeGeoNode.
typedef SmartPtr<TYAcousticVolumeNodeGeoNode> LPTYAcousticVolumeNodeGeoNode;
///Collection de noeuds geometriques de type TYAcousticVolumeNode.
typedef std::vector<LPTYAcousticVolumeNodeGeoNode> TYTabAcousticVolumeNodeGeoNode;


#endif // __TY_ACOUSTICVOLUMENODE__

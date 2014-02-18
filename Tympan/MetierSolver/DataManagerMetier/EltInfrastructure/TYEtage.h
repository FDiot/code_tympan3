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

#ifndef __TY_ETAGE__
#define __TY_ETAGE__

class OSegment3D;


#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticVolume.h"
#include "TYMur.h"
#include "TYMachine.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantAcoustique/TYUserSourcePonctuelle.h"
#include "TYDalle.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerIHM/TYEtageWidget.h"
#include "Tympan/GraphicIHM/DataManagerGraphic/TYEtageGraphic.h"
#endif


/**
 * Centralise les elements d'un etage.
 *
 * @author Projet_Tympan
 *
 */
class TYEtage: public TYAcousticVolume
{
    OPROTOSUPERDECL(TYEtage, TYAcousticVolume)
    TY_EXTENSION_DECL(TYEtage)
    TY_EXT_GRAPHIC_DECL(TYEtage)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYEtage();
    /**
     * Constructeur par copie.
     */
    TYEtage(const TYEtage& other);
    /**
     * Destructeur.
     */
    virtual ~TYEtage();

    /**
     * Operateur de copie.
     */
    TYEtage& operator=(const TYEtage& other);
    /**
     * Operateur de comparaison.
     */
    bool operator==(const TYEtage& other) const;
    /**
     * Operateur de comparaison.
     */
    bool operator!=(const TYEtage& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    virtual void getChilds(TYElementCollection& childs, bool recursif = true);

    virtual void setDensiteSrcsH(double densite, bool recursif = true);
    virtual void setDensiteSrcsV(double densite, bool recursif = true);

    virtual void setRegime(TYSpectre& Spectre, int regime = -1, bool recursif = true);
    virtual bool remRegime(int regime);
    virtual void setCurRegime(int regime);
    virtual int addRegime(TYRegime regime);
    virtual int addRegime();
    virtual void setNextRegimeNb(const int& next);
    virtual void setRegimeName(const QString& name);
    virtual void propagateAtt(LPTYAttenuateur pAtt);

    /// Export au format csv sur un flux transmis
    virtual void exportCSV(std::ofstream& ofs);


    virtual TYTabSourcePonctuelleGeoNode getSrcs() const;
    virtual TYSourcePonctuelle srcPonctEquiv() const;
    virtual void distriSrcs();
    virtual bool setSrcsLw();

    virtual double volume() const;
    virtual double surface() const;
    virtual TYTabVector normals() const;
    virtual TYTabPoint sommets() const;
    virtual TYTabLPPolygon faces() const;
    virtual TYBox volEnglob() const;
    virtual TYPoint centreGravite() const;
    virtual int intersects(const OSegment3D& seg, TYTabPoint& ptList) const;
    virtual int isInside(const TYPoint& pt) const;

    virtual void remAllSrcs();
    virtual TYTabAcousticSurfaceGeoNode acousticFaces();
    virtual bool findAcousticSurface(const TYAcousticSurface* pAccSurf, OMatrix* pMatrix = 0);
    virtual bool updateAcoustic(const bool& force = false);
    virtual double activeSurface() const;
    virtual TYSpectre setGlobalLW(const TYSpectre& spectre, const double& surfGlobale, const int& regime = -1);

    void setacousticFacesPourCalcul(bool bPourCalculTrajet);


    /**
     * Get de la liste des murs.
     */
    TYTabMurGeoNode& getTabMur() { return _tabMur; }
    /**
     * Get de la liste des murs.
     */
    const TYTabMurGeoNode& getTabMur() const { return _tabMur; }

    /**
     * Ajoute une entree a la liste des murs.
     */
    bool addMur(LPTYMur pMur);

    /**
     * Suppression d'un mur au tableau.
     *
     * @param pMur Le mur a supprimer.
     *
     * @return <code>true</code> si le mur a ete trouve et supprime;
     *         <code>false</code> sinon.
     */
    bool remMur(const LPTYMur pMur);

    /**
     * Suppression d'un mur au tableau
     * a partir de son identifiant.
     *
     * @param pMur Le mur a supprimer.
     *
     * @return <code>true</code> si le mur a ete trouve et supprime;
     *         <code>false</code> sinon.
     */
    bool remMur(QString idMur);

    /**
     * Creer des murs selon le contour represente par une serie de points.
     *
     * @param tabPts Un tableau de points decrivant le contour de cet etage.
     * @param hauteur La hauteur des murs a creer.
     * @param close Indique si le dernier point et le premier point doivent etre
     *               utilises pour creer un mur et fermer l'etage.
     */
    bool setMurs(const TYTabPoint& tabPts, double hauteur = 2.0, bool close = true);

    /**
     * Suppression de tous les murs.
     */
    void remMurs();

    /**
     * Set de l'indicateur de fermeture des murs.
     */
    void setClosed(bool closed) { _closed = closed; }
    /**
     * Get de l'indicateur de fermeture des murs.
     */
    bool getClosed() { return _closed; }

    /**
     * Get du sol.
     */
    LPTYDalle getSol() { return _pSol; }
    /**
     * Get du sol.
     */
    const LPTYDalle getSol() const { return _pSol; }

    /**
     * Get du plafond.
     */
    LPTYDalle getPlafond() { return _pPlafond; }
    /**
     * Get du plafond.
     */
    const LPTYDalle getPlafond() const { return _pPlafond; }

    /**
     * Retourne un tableau de points representant le contour de l'etage.
     *
     * @return Un tableau de points.
     */
    TYTabPoint getContour() const;

    /**
     * Etablie la hauteur des murs composant cet etage.
     *
     * @param hauteur La nouvelle hauteur des murs.
     */
    void setHauteur(double hauteur);
    /**
     * Retourn la hauteur des murs composant cet etage.
     *
     * @return La hauteur des murs.
     */
    double getHauteur() const;
    /**
     * Retourne le mode de calcul du champ reverbere
     *
     * @return un short indiquant le mode de calcul
     */
    short getTypeReverb() { return _typeReverb; }
    /**
     * Affecte le mode de calcul du champ reverbere
     * Sabine   = 0
     * Kuttruff = 1
     */
    void setTypeReverb(const short& typeReverb = 0) { _typeReverb = typeReverb; }

    /**
     * Get/Set de la paroi par defaut
     */
    void setParoi(const LPTYParoi pParoi);
    /**
     * Get/Set de la paroi par defaut
     */
    LPTYParoi getParoi() { return _pParoi; }

    /**
     * Affecte la paroi par defauts aux murs des bâtiments
     */
    void updateParoi();

    /**
     * Set/Get de la liste des machines.
     */
    TYTabMachineGeoNode& getTabMachine() { return _tabMachine; }
    /**
     * Set/Get de la liste des machines.
     */
    const TYTabMachineGeoNode& getTabMachine() const { return _tabMachine; }
    /**
     * Set/Get de la liste des machines.
     */
    void setTabMachine(const TYTabMachineGeoNode& tab) { _tabMachine = tab; }
    /**
     * Ajoute une entree a la liste des machines et la position associee.
     */
    bool addMachine(LPTYMachineGeoNode pMachineGeoNode);
    /**
     * Ajoute une entree a la liste des machines et la position associee.
     */
    bool addMachine(LPTYMachine pMachine, const TYRepere& pos);
    /**
     * Ajoute une entree a la liste des machines.
     */
    bool addMachine(LPTYMachine pMachine);
    /**
     * Supprime une entree a la liste des machines.
     */
    bool remMachine(const LPTYMachineGeoNode pMachineGeoNode);
    /**
     * Supprime une entree a la liste des machines.
     */
    bool remMachine(const LPTYMachine pMachine);
    /**
     * Supprime une entree a la liste des machines a partir de son
     * identifiant.
     */
    bool remMachine(QString idMachine);
    /**
     * Suppression de tous les elements.
     */
    void remAllMachine();
    /**
     * Retourne une machine a partir de son index.
     */
    LPTYMachine getMachine(int index) { return TYMachine::safeDownCast(_tabMachine.at(index)->getElement()); }
    /**
     * Retrouve le GeoNode associe a une machine.
     *
     * @param pAccVol L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYMachineGeoNode findMachine(const LPTYMachine pMachine);

    // BEGIN
    /**
     * Set/Get de la liste des sources.
     */
    TYTabUserSourcePonctuelleGeoNode& getTabSources() { return _tabSources; }
    /**
     * Set/Get de la liste des sources.
     */
    const TYTabUserSourcePonctuelleGeoNode& getTabSources() const { return _tabSources; }
    /**
     * Set/Get de la liste des sources.
     */
    void setTabSources(const TYTabUserSourcePonctuelleGeoNode& tab) { _tabSources = tab; }
    /**
     * Ajoute une entree a la liste des sources et la position associee.
     */
    bool addSource(LPTYUserSourcePonctuelleGeoNode pSourceGeoNode);
    /**
     * Ajoute une entree a la liste des sources et la position associee.
     */
    bool addSource(LPTYUserSourcePonctuelle pSource, const TYRepere& pos);
    /**
     * Ajoute une entree a la liste des sources.
     */
    bool addSource(LPTYUserSourcePonctuelle pSource);
    /**
     * Supprime une entree a la liste des sources.
     */
    bool remSource(const LPTYUserSourcePonctuelleGeoNode pSourceGeoNode);
    /**
     * Supprime une entree a la liste des sources.
     */
    bool remSource(const LPTYUserSourcePonctuelle pSource);
    /**
     * Supprime une entree a la liste des sources a partir de son
     * identifiant.
     */
    bool remSource(QString idSource);
    /**
     * Suppression de tous les elements.
     */
    void remAllSources();
    /**
     * Retourne une source a partir de son index.
     */
    LPTYUserSourcePonctuelle getSource(int index) { return TYUserSourcePonctuelle::safeDownCast(_tabSources.at(index)->getElement()); }
    /**
     * Retrouve le GeoNode associe a une machine.
     *
     * @param pAccVol L'element a chercher.
     *
     * @return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYUserSourcePonctuelleGeoNode findSource(const LPTYUserSourcePonctuelle pSource);

    // END
    /**
     * Recherche les faces de machines posees au sol
     */
    void findFaceMachineSol();




    /**
     * Calcule le coefficient de Sabine de cet etage.
     *
     */
    void coeffSabine();

    /**
     * Calcule le coefficient de transmission au point donne.
     *
     * @param pSrcPonct Source ponctuelle pour le calcul du coefficient de transmission.
     *
     * @return Le coefficient de transmission au point passe.
     */
    double coeffTransmFrom(const TYSourcePonctuelle* pSrcPonct) const;

    /**
     * Calcule le temps de reverberation de l'etage.
     */
    void  calculTempsReverb();

    /**
     * Get/Set du TR de la salle
     */
    TYSpectre& getTR() { return _TR; }
    void setTR(const TYSpectre& TR) { _TR = TR; }

    /**
     * Calcul du champ direct
     * @param un point
     *
     * @return un spectre de pression acoustique
     */
    OSpectre champDirect(const OPoint3D& unPoint);

    /**
     * Calcul du champ reverbere selon Sabine
     * @param un point
     *
     * @return un spectre de pression acoustique
     */
    void calculChampRevSabine();

    /**
     *Calcul du champ reverbere selon KUTTRUFF
     * @param un point
     *
     * @return un spectre de pression acoustique
     */
    void calculChampRevKuttruff();

    /**
     * Calcul du champ reverbere selon l'option choisie par l'utilisateur
     */
    void calculChampReverbere();

    /**
     * Get/Set du champ reverbere
     */
    TYSpectre& getReverb() { return _reverb; }
    void setReverb(const TYSpectre& reverb) { _reverb = reverb; }

    /**
     * Get/Set du tableau des regimes des machines
     */
    tabMapPtrElementRegime& getTabRegimeMachines() { return _tabRegimesMachines; }
    const tabMapPtrElementRegime& getTabRegimeMachines() const { return _tabRegimesMachines; }

    /**
     * Get/Set du tableau des regimes des machines
     */
    void setTabRegimeMachines(const tabMapPtrElementRegime& tabRegimesMachines) { _tabRegimesMachines = tabRegimesMachines; }

    /**
     * Get/Set du tableau des regimes des machines pour un regime de l'etage donne
     */
    TYMapPtrElementInt& getMapRegimes(const int& regime) { return _tabRegimesMachines[regime]; }

    /**
     * Get/Set du tableau des regimes des machines pour un regime de l'etage donne
     */
    void setMapRegimes(const int& regime, TYMapPtrElementInt mapRegimes) { _tabRegimesMachines[regime] = mapRegimes; }

    /**
     * Get/Set du tableau des etats des machines
     */
    tabMapPtrElementBool& getTabEtatMachines() { return _tabEtatMachines; }
    const tabMapPtrElementBool& getTabEtatMachines() const { return _tabEtatMachines; }

    /**
     * Get/Set du tableau des etats des machines
     */
    void setTabEtatMachines(const tabMapPtrElementBool& tabEtatMachines) { _tabEtatMachines = tabEtatMachines; }

    /**
     * Get/Set du tableau des etats des machines pour un regime de l'etage donne
     */
    TYMapPtrElementBool& getMapEtats(const int& regime) { return _tabEtatMachines[regime]; }

    /**
     * Get/Set du tableau des etats des machines pour un regime de l'etage donne
     */
    void setMapEtats(const int& regime, TYMapPtrElementBool mapEtats) { _tabEtatMachines[regime] = mapEtats; }

protected:
    /**
     * Recalcul le sol et le plafond de cet etage en fonction
     * des murs. _closed doit etre positionne.
     */
    void updateSolPlafond();

    /**
     * Calcule la puissance rayonnee a un element de mur
     */
    TYSpectre getPuissanceRayonnee(LPTYAcousticSurface pSurface, const OMatrix matMur, const LPTYParoi pMurParoi);

    /**
     * Mise a jour de la hauteur des elements dans l'etage (prise en compte hauteur)
     */
    void updateZSource();

    /**
     * Verification de la coherence des tableaux de gestion des regimes et de etats des machines et sources integrees
     */
    void verifTabRegimeSources();

    // Membres
protected:
    ///Liste des murs.
    TYTabMurGeoNode _tabMur;

    ///Dalle pour le sol.
    LPTYDalle _pSol;

    ///Dalle pour le plafond.
    LPTYDalle _pPlafond;

    ///Liste des sources.
    TYTabMachineGeoNode _tabMachine;

    /// Liste des sources ponctuelles
    TYTabUserSourcePonctuelleGeoNode _tabSources;

    /// Paroi par defaut de l'etage
    LPTYParoi _pParoi;

    ///Indique si les murs constituant l'etage sont fermes.
    bool _closed;

    ///Precise le mode de calcul du champ reverbere
    int _typeReverb;

    /// Surface d'absorption de la salle (comprend la suface des machines)
    double _surfAbsorbante;

    /// Volume libre de la salle (volume de la salle vide - volume des machines)
    double _volumeLibre;

    /// Spectre du coefficient de Sabine de l'etage
    OSpectre _absoSabine;

    /// Spectre de TR
    TYSpectre _TR;

    /// Spectre du champ reverbere
    TYSpectre _reverb;

    /// Gestion des regimes des machines
    tabMapPtrElementRegime _tabRegimesMachines;

    /// Gestion des etats des machines
    tabMapPtrElementBool _tabEtatMachines;

private:
    //Ajoute pour obtenir un contour simple pour les ecrans
    bool _bPourCalculTrajet;
    ///Collection des faces des machines collees au sol.
    TYTabAcousticSurfaceGeoNode _facesMachineSol;
};


///Noeud geometrique de type TYEtage.
typedef TYGeometryNode TYEtageGeoNode;
///Smart Pointer sur TYEtageGeoNode.
typedef SmartPtr<TYEtageGeoNode> LPTYEtageGeoNode;
///Collection de noeuds geometriques de type TYEtage.
typedef std::vector<LPTYEtageGeoNode> TYTabEtageGeoNode;


#endif // __TY_ETAGE__

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

/* ==============================================================================
    TO DO :
            1. Sauvegarder en XML l'information _geomPrecision
    ============================================================================= */

#ifndef __TY_CALCUL__
#define __TY_CALCUL__

#include <memory>

#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYAtmosphere.h"
#include "TYMaillage.h"
#include "TYPointControl.h"
#include "TYResultat.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYRay.h"
#include "Tympan/MetierSolver/DataManagerMetier/ComposantGeoAcoustique/TYAcousticEdge.h"

class TYProjet;
namespace tympan {
    class AcousticProblemModel;
    class AcousticResultModel;
} // namespace tympan


/**
 * \file TYCalcul.h
 * \class TYCalcul
 * \version v 1.2
 * \brief Programme de calcul.
 * \author Projet_Tympan
 * \date 2008/01/25 14:06:42
 *
 * Le programme qui definit les Calculs.
 *
 */
class TYCalcul: public TYElement
{
    OPROTODECL(TYCalcul)
    TY_EXTENSION_DECL_ONLY(TYCalcul)
    TY_EXT_GRAPHIC_DECL_ONLY(TYCalcul)

public:
    /**
     * \fn enum Constants
     * brief Constantes liees au calcul.
     *
     * Le nombre de couples sources recepteurs est limite a 2 exposant 32. Le
     * nombre total de bits utilises pour representer un couple source recepteur
     * ne doit pas depasser 32.
     *
     */
    enum Constants
    {
        NB_BITS_SOURCES = 10,                           /**< Nombre de bits utilises pour indexer les sources. */
        NB_BITS_RECEPTEURS = 22,                        /**< Nombre de bits utilises pour indexer les recepteurs. */
        MAX_SOURCES = 1 << NB_BITS_SOURCES,             /**< Nombre maxi de sources (puissance de 2 obligatoire). */
        MAX_RECEPTEURS = 1 << NB_BITS_RECEPTEURS,       /**< Nombre maxi de sources (puissance de 2 obligatoire). */
        MASQUE_INDEX_SOURCES = MAX_SOURCES - 1,         /**< Masque pour acceeder a l'index d'une source dans un couple. */
        MASQUE_INDEX_RECEPTEUR = MAX_RECEPTEURS - 1,    /**< Masque pour acceeder a l'index d'une source dans un couple. */
    };

    /**
     * \fn enum CalculState
     * \brief Les differents etats possible pour un calcul.
     */
    enum CalculState
    {
        Locked,     /**< Le calcul ne peut etre execute (modifie). */
        Actif,      /**< Le calcul peut etre execute (modifie). */
    };


    /**
     * \brief Constructeur.
     *  Constructeur de la classe TYCalcul.
     *  \param LPTYProjet  pParent=Nulle
     */
    TYCalcul(LPTYProjet pParent = NULL);
    /**
     *\brief Constructeur par copie.
     *  Constructeur par copie de la classe TYCalcul.
     */
    TYCalcul(const TYCalcul& other);
    /**
     *\brief Destructeur.
     * Deconstructeur de la classe TYCalcul
     */
    virtual ~TYCalcul();

    ///Operateur =.
    TYCalcul& operator=(const TYCalcul& other);
    ///Operateur ==.
    bool operator==(const TYCalcul& other) const;
    ///Operateur !=.
    bool operator!=(const TYCalcul& other) const;

    virtual bool deepCopy(const TYElement* pOther, bool copyId = true);

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);
    virtual int fromXML(DOM_Element domElement);

    /**
     * \fn void purge()
     * \brief Reinitialise ce calcul.
     */
    void purge();

    /**
     * Vide le resultat de ce calcul
     */
    void clearResult();

    /**
     * Retourne le projet parent.
     */
    TYProjet* getProjet();

    /**
     * \fn int getNumero()
     * \brief Get du numero
     * \return _numero
     */
    int getNumero() const { return _numero; }

    /**
     * \fn void setNumero(int num)
     * \brief Set du numero
     */
    void setNumero(int num) { _numero = num; }

    /**
     * \fn QString getAuteur()
     * \brief Get du nom de l'auteur.
     * \return _auteur
     */
    QString getAuteur() const { return _auteur; }

    /**
     * \fn void setAuteur(QString auteur)
     * \brief Set du nom de l'auteur.
     */
    void setAuteur(QString auteur) { _auteur = auteur; }

    /**
     * fn QString getDateCreation()
     *    void setDateCreation(const QString& date)
     * \brief Set/Get de la date de creation.
     * \return _dateCreation
     */
    QString getDateCreation() const { return _dateCreation; }
    void setDateCreation(const QString& date) { _dateCreation = date; }

    /**
     * fn QString getDateModif()
     * \brief Get de la date de modification.
     * \return _dateModif
     */
    QString getDateModif() const { return _dateModif; }

    /**
     * fn void setDateModif(const QString& date)
     * \brief Set de la date de modification.
     */
    void setDateModif(const QString& date) { _dateModif = date; }

    /**
     * \fn QString getComment()
     * \brief Get des commentaires.
     * \return _comment
     */
    QString getComment() const { return _comment; }

    /**
     * \fn void setComment(const QString& comment)
     * \brief Set des commentaires.
     */
    void setComment(const QString& comment) { _comment = comment; }

    /**
     *\fn int getExpansGeo()
     *\brief Get de l'expansion geometrique (0 : 2pi, 1 : 4pi)
     *\return _expansGeo
     */
    int getExpansGeo() const { return _expansGeo; }

    /**
     *\fn void setExpansGeo(int exp)
     *\brief Set de l'expansion geometrique (0 : 2pi, 1 : 4pi)
     */
    void setExpansGeo(int exp) { _expansGeo = exp; }

    /**
     * \fn  bool getIsUpTodate()
     * \brief Get de l'etat calcul a jour ou non par rapport a l'infrastructure.
     * \return _upTodate
     */
    bool getIsUpTodate() const { return _upTodate; }

    /**
     * \fn  void setIsUpTodate(bool upTodate)
     * \brief Set de l'etat calcul a jour ou non par rapport a l'infrastructure.
     */
    void setIsUpTodate(bool upTodate) { _upTodate = upTodate; }

    /**
     * \fn  bool getUseSol()
     * \brief Get du test de la prise ou non en compte du sol.
     * \return _useSol
     */
    bool getUseSol() const { return _useSol; }

    /**
     * \fn  void setUseSol(bool use)
     * \brief Set du test de la prise ou non en compte du sol.
     */
    void setUseSol(bool use) { _useSol = use; }

    /**
     *\fn bool getUseAnechoic()
     *\brief Get des conditions anechoiques (true) ou semi-anechoi��que (false = default)
     *\return _anechoic
     */
    bool getUseAnechoic() { return _anechoic; }

    /**
     *\fn void setUseAnechoic(const bool anechoic = false)
     *\brief Set des conditions anechoiques (true) ou semi-anechoique (false = default)
     */
    void setUseAnechoic(const bool anechoic = false) { _anechoic = anechoic; }

    /**
     *\fn   bool getUseVegetation()
     *\brief Get du test de la prise ou non en compte de la vegetation.
     *\return _useVegetation
     */
    bool getUseVegetation() const { return _useVegetation; }
    /**
     *\fn   void setUseVegetation(bool use)
     *\brief Set du test de la prise ou non en compte de la vegetation.
     */
    void setUseVegetation(bool use) { _useVegetation = use; }

    /**
     * \fn bool getUseAtmosphere()
     *\brief Get du test de la prise ou non en compte de l'atmosphere.
     *\return _useAtmosphere
     */
    bool getUseAtmosphere() const { return _useAtmosphere; }
    /**
     * \fn void setUseAtmosphere(const bool use = true)
     *\brief Set du test de la prise ou non en compte de l'atmosphere.
     */
    void setUseAtmosphere(const bool use = true);

    /**
     *\fn bool getUseEcran()
     *\brief Get du test de la prise ou non en compte des murs, batiments et machines.
     *\return _useEcran
     */
    bool getUseEcran() const { return _useEcran; }
    /**
     *\fn void setUseEcran(bool use)
     *\brief Set du test de la prise ou non en compte des murs, batiments et machines.
     */
    void setUseEcran(bool use) { _useEcran = use; }

    /**
     * \fn bool getUseReflexion()
     * \brief Get du test de la prise ou non en compte de la reflexion.
     * \return _useReflexion
     */
    bool getUseReflexion() const { return _useReflexion; }
    /**
     * \fn void setUseReflexion(bool use)
     * \brief Set du test de la prise ou non en compte de la reflexion.
     */
    void setUseReflexion(bool use) { _useReflexion = use; }

    /**
     * \fn  bool getInterference()
     * \brief Get du test du flag de calcul energetique ou avec interference.
     * \return _interference
     */
    bool getInterference() const { return _interference; }
    /**
     * \fn  void setInterference(bool use)
     * \brief Set du test du flag de calcul energetique ou avec interference.
     */
    void setInterference(bool use) { _interference = use; }

    /**
     *\fn double getParamH()
     *\brief Get du parametre h1.
     *\return _h1
     */
    double getParamH() const { return _h1; }
    /**
     *\fn void setParamH(double h1)
     *\brief Set du parametre h1.
     */
    void setParamH(double h1) { _h1 = h1; }

    /**
     *\fn void setDistanceSRMin(const double& dist)
     *\brief Set du parametre _distanceSRMin
     */
    void setDistanceSRMin(const double& dist) { _distanceSRMin = dist; }
    /**
     *\fn double getDistanceSRMin()
     *\brief Get du parametre _distanceSRMin
     *\return _distanceSRMin
     */
    double getDistanceSRMin() { return _distanceSRMin; }

    /**
     * \fn bool getCondFav()
     * \brief Get du test des conditions favorables ou non a la propagation.
     * return _condFav
     */
    bool getCondFav() const { return _condFav; }
    /**
     * \fn void setCondFav(bool cond)
     * \brief Set du test des conditions favorables ou non a la propagation.
     */
    void setCondFav(bool cond) { _condFav = cond; }

    /**
     * \fn void setFreqMinFreq(float freq)
     * \brief Set de la frequence minimale de la bande de frequence des spectres.
     */
    void setFreqMinFreq(float freq);
    /**
     * \fn float getFreqMinFreq()
     * \brief Get de la frequence minimale de la bande de frequence des spectres.
     * \return _freqMinSpectre
     */
    float getFreqMinFreq() const { return _freqMinSpectre; }

    /**
     * \fn void setFreqMaxFreq(float freq)
     * \brief Set de la frequence maximale de la bande de frequence des spectres.
     */
    void setFreqMaxFreq(float freq);
    /**
     * \fn float getFreqMaxFreq()
     * \brief Get de la frequence maximale de la bande de frequence des spectres.
     * \return _freqMaxSpectre
     */
    float getFreqMaxFreq() const { return _freqMaxSpectre; }


    /**
     *\fn void setElementSelection(TYListID selection)
     *\brief Set de la selection d'elements actifs dans ce Calcul.
     */
    void setElementSelection(TYListID selection);
    /**
     *\fn TYListID getElementSelection()
     *\brief Get de la selection d'elements actifs dans ce Calcul.
     *\return _elementSelection
     */
    TYListID getElementSelection() const { return _elementSelection; }

    /**
     *\fn void clearSelection()
     *\brief Vide la selection d'elements actifs dans ce Calcul.
     */
    void clearSelection();

    /**
     * \fn bool addToSelection(TYUUID id)
     * \brief Ajoute l'element a la selection de ce Calcul.
     */
    bool addToSelection(TYUUID id);
    /**
     * \fn bool addToSelection(TYElement * pElt, bool recursif = true)
     * \brief Ajoute l'element a la selection de ce Calcul.
     */
    void addToSelection(TYElement* pElt, bool recursif = true);

    /**
     * \fn bool remToSelection(TYUUID id)
     * \brief Supprime l'element de la selection de ce Calcul.
     */
    bool remToSelection(TYUUID id);
    /**
     * \fn bool remToSelection(TYElement * pElt, bool recursif = true)
     * \brief Supprime l'element de la selection de ce Calcul.
     */
    bool remToSelection(TYElement* pElt, bool recursif = true);

    /**
     * \fn bool isInSelection(TYUUID id)
     * \brief Test si l'element est present dans la selection de ce Calcul.
     */
    bool isInSelection(TYUUID id);

    /**
     * \fn bool isInSelection(LPTYElement pElt)
     * \brief Test si l'element est present dans la selection de ce Calcul.
     */
    bool isInSelection(LPTYElement pElt) { assert(pElt); return isInSelection(pElt->getID().toString()); }

    /**
     * \fn  TYMapPtrElementBool& getEmitAcVolNode()
     * \brief Get du tableau contenant l'etat de chaque volumeNode en emission.
     * \return _emitAcVolNode
     */
    TYMapPtrElementBool& getEmitAcVolNode() { return _emitAcVolNode; }
    /**
     * \fn  const TYMapPtrElementBool& getEmitAcVolNode() const
     * \brief Get du tableau contenant l'etat de chaque volumeNode en emission.
     * \return _emitAcVolNode
     */
    const TYMapPtrElementBool& getEmitAcVolNode() const { return _emitAcVolNode; }
    /**
     * \fn  void setEmitAcVolNode(const TYMapPtrElementBool& etat)
     * \brief Set du tableau contenant l'etat de chaque volumeNode en emission.
     */
    void setEmitAcVolNode(const TYMapPtrElementBool& etat) { _emitAcVolNode = etat; }

    /**
     * \fn TYMapPtrElementInt& getMapElementRegime()
     * \brief Get du tableau des regimes
     * \return _mapElementRegime
     */
    TYMapPtrElementInt& getMapElementRegime() { return _mapElementRegime; }

    /**
     * \fn const TYMapPtrElementInt& getMapElementRegime()const
     * \brief Get du tableau des regimes
     * \return _mapElementRegime
     */
    const TYMapPtrElementInt& getMapElementRegime() const { return _mapElementRegime; }

    /**
     * \fn void setMapElementRegime(const TYMapPtrElementInt& mapElementRegime)
     * \brief Set du tableau des regimes
     */
    void setMapElementRegime(const TYMapPtrElementInt& mapElementRegime) { _mapElementRegime = mapElementRegime; }

    /**
     * \fn LPTYAtmosphere getAtmosphere() { return _pAtmosphere; }
     * \brief Get de l'atmosphere.
     * \return _pAtmosphere
     */
    LPTYAtmosphere getAtmosphere() { return _pAtmosphere; }
    /**
     * \fn const LPTYAtmosphere getAtmosphere() const
     * \brief Get de l'atmosphere.
     * \return _pAtmosphere
     */
    const LPTYAtmosphere getAtmosphere() const { return _pAtmosphere; }
    /**
     * \fn void setAtmosphere(const LPTYAtmosphere pAtmo)
     * \brief Set de l'atmosphere.
     */
    void setAtmosphere(const LPTYAtmosphere pAtmo);

    /**
     *\fn TYTabMaillageGeoNode& getMaillages() { return _maillages; }
     *\brief Get de la collection de maillages.
     *\return _maillages
     */
    TYTabMaillageGeoNode& getMaillages() { return _maillages; }
    /**
     *\fn const TYTabMaillageGeoNode& getMaillages() const
     *\brief Get de la collection de maillages.
     *\return _maillages
     */
    const TYTabMaillageGeoNode& getMaillages() const { return _maillages; }
    /**
     *\fn void setMaillages(const TYTabMaillageGeoNode& maillages) { _maillages = maillages; setIsGeometryModified(true); }
     *\brief Set de la collection de maillages.
     */
    void setMaillages(const TYTabMaillageGeoNode& maillages) { _maillages = maillages; setIsGeometryModified(true); }

    /**
     *\fn bool addMaillage(LPTYMaillageGeoNode pMaillageGeoNode)
     *\brief  Ajout d'un maillage.
     */
    bool addMaillage(LPTYMaillageGeoNode pMaillageGeoNode);
    /**
     *\fn bool addMaillage(LPTYMaillage pMaillage)
     *\brief  Ajout d'un maillage.
     */
    bool addMaillage(LPTYMaillage pMaillage);

    /**
     *\fn bool remMaillage(const LPTYMaillageGeoNode pMaillageGeoNode)
     *\brief  Suppression d'un maillage.
     */
    bool remMaillage(const LPTYMaillageGeoNode pMaillageGeoNode);

    /**
     *\fn bool remMaillage(const LPTYMaillage pMaillage)
     *\brief  Suppression d'un maillage.
     */
    bool remMaillage(const LPTYMaillage pMaillage);

    /**
     * \fn bool remMaillage(QString idMaillage)
     * \brief Suppression d'un maillage a partir de son identifiant.
     */
    bool remMaillage(QString idMaillage);

    /**
     * \fn  bool remAllMaillage()
     * \brief Suppression de tous les maillages.
     */
    bool remAllMaillage();

    /**
     * \fn LPTYMaillage getMaillage(int index)
     * \brief Retourne un maillage a partir de son index.
     * \return TYMaillage::safeDownCast(_maillages.at(index)->getElement())
     */
    LPTYMaillage getMaillage(int index) { return TYMaillage::safeDownCast(_maillages.at(index)->getElement()); }

    /**
     * \fn LPTYMaillageGeoNode findMaillage(const LPTYMaillage pMaillage).
     * \brief Retrouve le GeoNode associe a un maillage.
     * \param pMaillage L'element a chercher.
     * \return Le GeoNode associe a l'element a chercher si trouve sinon NULL.
     */
    LPTYMaillageGeoNode findMaillage(const LPTYMaillage pMaillage);

    /*!
        \brief Ajoute un point de controle au tableau des resultats
        \param : TYPoint* : Pointeur sur le point a ajouter
     */
    void addPtCtrlToResult(TYPointControl* pPoint);

    /*!
        \brief Supprime un point de controle au tableau des resultats
        \param : TYPoint* : Pointeur sur le point a ajouter
     */
    void remPtCtrlFromResult(TYPointControl* pPoint);

    /**
     * \fn  LPTYResultat getResultat()
     * \brief Get du resultat.
     * \return _pResultat
     */
    const LPTYResultat getResultat() const { return _pResultat; }

    /**
     * \fn const LPTYResultat getResultat()
     * \brief Set du resultat.
     */
    void setResultat(const LPTYResultat pResultat) { _pResultat = pResultat; _pResultat->setParent(this); }

    /**
     * \fn bool updateAltiRecepteurs()
     * \brief Mets a jour l'altitude des maillage
     *   Mets a jour l'altitude des maillage en fonction de l'altimetrie du site associe au projet parent a ce calcul.
     */
    bool updateAltiRecepteurs();

    /**
     * \fn bool updateAltiRecepteurs(const TYAltimetrie* pAlti)
     * \brief Mets a jour l'altitude des maillage
     *   Mets a jour l'altitude des maillage en fonction de l'altimetrie du site associe au projet parent a ce calcul.
     */
    bool updateAltiRecepteurs(const TYAltimetrie* pAlti);

    /**
     * \fn  bool updateAltiMaillage(TYMaillageGeoNode * pMaillageGeoNode, const TYAltimetrie* pAlti )
     * \brief Met a niveau l'altimetrie d'un maillage
     */
    bool updateAltiMaillage(TYMaillageGeoNode* pMaillageGeoNode, const TYAltimetrie* pAlti);

    /**
     * \fn bool updateAltiMaillage(TYMaillageGeoNode * pMaillageGeoNode)
     * \brief Met a niveau l'altimetrie d'un maillage
     */
    bool updateAltiMaillage(TYMaillageGeoNode* pMaillageGeoNode);  // Remis pour compatibilite

    /**
     * \fn void selectActivePoint(const LPTYSite pSite)
     * \brief Selectionne les points actifs du maillage
     */
    void selectActivePoint(const LPTYSiteNode pSite);

    /**
     * \fn void buildValidTrajects(const TYTabSourcePonctuelleGeoNode& sources, TYTabPointCalculGeoNode& recepteurs)
     * \brief construit le tableau des trajets et la matrice resultat en supprimant les points trop proches d'une source
     */
    void buildValidTrajects(const TYTabSourcePonctuelleGeoNode& sources, TYTabPointCalculGeoNode& recepteurs);

    /**
     * \fn void getAllRecepteurs(TYTabPointCalculGeoNode& tabRecepteur)
     * \brief Construit la liste des recepteurs
     */
    void getAllRecepteurs(TYTabPointCalculGeoNode& tabRecepteur);

    /**
     * \fn void getAllSources(TYMapElementTabSources& mapElementSources, TYTabSourcePonctuelleGeoNode& tabSources)
     * \brief Construit la liste des sources a partir du tableau associatif "Machine"/liste de sources
     */
    void getAllSources(TYMapElementTabSources& mapElementSources, TYTabSourcePonctuelleGeoNode& tabSources);

    /**
     * \fn bool isCalculPossible(const int& nbSources, const int& nbRecepteurs, const LPTYSite pMergeSite)
     *\brief verification des calculs
     *   Verifie que le calcul est possible: au moins une source, au moins un
     *   recepteur, pas de depassement du max de sources et de recepteurs.
     */
    bool isCalculPossible(const int& nbSources, const int& nbRecepteurs, const LPTYSiteNode pMergeSite);

    /**
     * \fn bool go()
     * \brief Lance l'execution du Calcul.
     * \return <code>true</code> si le calcul a reussi;
     *         <code>false</code> sinon.
     */
    bool go();

    /**
     * \fn void getCalculElements(LPTYSiteNode pSite)
     * \brief Recupere tous les elements de la scene qui participent au calcul.
     * \param pSite Site a partir duquel se fait la selection.
     */
    void getCalculElements(LPTYSiteNode pSite);

    /**
     * \fn void setSite(LPTYSiteNode pSite)
     * \brief Definition du site sur lequel va s'effectuer le calcul
     */
    void setSite(LPTYSiteNode pSite);

    /**
     * \fn LPTYSiteNode getSite()
     * \brief Recuperation du site de calcul
     * \return _pSiteCalcul
     */
    LPTYSiteNode getSite();

    /**
     * \fn bool getCalculTrajetsHorizontaux()
     * \brief Get du calcul des trajets horizontaux
     * \return _bCalculTrajetsHorizontaux
     */
    bool getCalculTrajetsHorizontaux() { return _bCalculTrajetsHorizontaux; }

    /**
     * \fn const bool getCalculTrajetsHorizontaux()const
     * \brief Get du calcul des trajets horizontaux
     * \return _bCalculTrajetsHorizontaux
     */
    const bool getCalculTrajetsHorizontaux() const { return _bCalculTrajetsHorizontaux; }

    /**
     * \fn void setCalculTrajetsHorizontaux(const bool& bCalculTrajetsHorizontaux)
     * \brief Set du calcul des trajets horizontaux
     */
    void setCalculTrajetsHorizontaux(const bool& bCalculTrajetsHorizontaux) { _bCalculTrajetsHorizontaux = bCalculTrajetsHorizontaux; }

    /**
     * \fn int getTypeCalculSol()
     * \brief Get du type de calcul de reflexion sur le sol local ou autre methode de calcul
     * \return _typeCalculSol
     */
    int getTypeCalculSol() { return _typeCalculSol; }

    /**
     * \fn const int getTypeCalculSol()const
     * \brief Get du type de calcul de reflexion sur le sol local ou autre methode de calcul
     * \return _typeCalculSol
     */
    const int getTypeCalculSol() const { return _typeCalculSol; }

    /**
     * \fn void setTypeCalculSol(const int& typeCalcul)
     * \brief Set du type de calcul de reflexion sur le sol local ou autre methode de calcul
     */
    void setTypeCalculSol(const int& typeCalcul) { _typeCalculSol = typeCalcul; }

    /**
     * \fn std::vector<TYTrajet>& getTabTrajet()
     * \brief Recuperation du tableau des trajets
     * \return _tabTrajets
     */
    std::vector<TYTrajet>& getTabTrajet() { return _tabTrajets; }

    /**
     * \fn void setModifiable(bool modify)
     * \brief Set attribut modifiable
     */
    void setState(int state);

    /**
     * \fn int getState()
     * \brief Get de l'etat du calcul.
     * \return _state
     */
    int getState() { return _state; }

    /**
     * \fn const int getState()const
     * \brief Get de l'etat du calcul.
     * \return _state
     */
    const int getState() const { return _state; }

    /**
     * \fn bool getStatusPartialResult()const
     * \brief Get de la conservation des resultats partiels
     * \return _pResultat->getPartialState()
     */
    bool getStatusPartialResult() const { return _pResultat->getPartialState(); }

    /**
     * \fn void setStatusPartialResult(const bool& status)
     * \brief Set de la conservation des resultats partiels
     */
    void setStatusPartialResult(const bool& status) { _pResultat->setPartialState(status); }

    /**
     * \fn bool getSaveRay()
     * \brief Return the status of ray persistence
     */
    bool getSaveRay() const { return _bSaveRay; }

    /**
     * \fn double getSeuilConfondu()
     *     void setSeuilConfondu(const double & seuil)
     * \brief Get/Set de la valeur de distance minimale entre deux points
     * \return _seuilConfondus
     */

    void setSeuilConfondu(const double& seuil) { _seuilConfondus = seuil; }
    double getSeuilConfondu() { return _seuilConfondus; }
    const double getSeuilConfondu() const { return _seuilConfondus; }

    /**
     * \fn bool getUseExternalSoft()
     *     void setUseExternalSoft(const bool & use)
     * \brief Indique si le calcul fait appel a une fonction interne
     * \return _useExternal
     */
    bool getUseExternalSoft() { return _useExternal; }
    void setUseExternalSoft(const bool& use) { _useExternal = use; }

    /**
     * \fn unsigned short getExternal()
     *     void setExternal(const unsigned short& ref)
     * \brief Get/Set du numero de la methode externe appelee
     * \return _refExternal
     */
    unsigned short getExternal() { return _refExternal; }
    void setExternal(const unsigned short& ref) { _refExternal = ref; }

    /**
     * \fn double getGeomPrecision()
     *     void setGeomPrecision(const double& geomPrecision)
     * \brief Get/Set de la precision sur la connaissance de la geometrie du modele
     * \return _geomPrecision
     */
    double getGeomPrecision() { return _geomPrecision; }
    void setGeomPrecision(const double& geomPrecision) { this->_geomPrecision = _geomPrecision; }

    /**
     * \fn OGenID getSolverId()
     *     void setSolverId(const OGenID& iD)
     * \brief Get/Set du solveur ID
     * \return _solverId
     */
    OGenID getSolverId() { return _solverId; }
    const OGenID getSolverId() const { return _solverId; }
    void setSolverId(const OGenID& iD) { _solverId = iD; }

    /**
     * \fn  bool HARM_AveragingEnabled()
     *      bool HARM_airAbsorptionEnabled()
     *      bool HARM_turbulenceScatteringEnabled()
     *      bool HARM_diffHaddenPierceEnabled()
     *      bool HARM_randomTerrainEnabled()
     *      unsigned int HARM_getOptions()
     *      unsigned short getTypeSaisieMeteo()
     *      unsigned int getNbTHread()
     * \fn void HARM_SetAveragingEnabled(const bool& b)
     *     void HARM_SetAirAbsorptionEnabled(const bool& b)
     *     void HARM_SetTurbulenceScatteringEnabled(const bool& b)
     *     void HARM_SetDiffHaddenPierceEnabled(const bool& b)
     *     void HARM_SetRandomTerrainEnabled(const bool& b)
     * \brief Get/Set des proprietes du calcul harmonoise
     * \return HARM_averaging
     *         HARM_airAbsorption
     *         HARM_turbulenceScattering
     *         HARM_diffHaddenPierce
     *         HARM_randomTerrain
     *         _typeSaisieMeteo
     *         _nbThread
     */
    bool HARM_AveragingEnabled()            { return HARM_averaging             !=  0 ? true : false ; }
    bool HARM_airAbsorptionEnabled()        { return HARM_airAbsorption         !=  0 ? true : false ; }
    bool HARM_turbulenceScatteringEnabled() { return HARM_turbulenceScattering  !=  0 ? true : false ; }
    bool HARM_diffHaddenPierceEnabled()     { return HARM_diffHaddenPierce      !=  0 ? true : false ; }
    bool HARM_randomTerrainEnabled()        { return HARM_randomTerrain         !=  0 ? true : false ; }

    void HARM_SetAveragingEnabled(const bool& b)            { HARM_averaging             = b !=  0 ? 2  : 0 ; }
    void HARM_SetAirAbsorptionEnabled(const bool& b)        { HARM_airAbsorption         = b !=  0 ? 4  : 0 ; }
    void HARM_SetTurbulenceScatteringEnabled(const bool& b) { HARM_turbulenceScattering  = b !=  0 ? 8  : 0 ; }
    void HARM_SetDiffHaddenPierceEnabled(const bool& b)     { HARM_diffHaddenPierce      = b !=  0 ? 16 : 0 ; }
    void HARM_SetRandomTerrainEnabled(const bool& b)        { HARM_randomTerrain         = b !=  0 ? 32 : 0 ; }

    unsigned int HARM_getOptions() { return (HARM_averaging | HARM_airAbsorption | HARM_turbulenceScattering | HARM_diffHaddenPierce | HARM_randomTerrain); }

    unsigned short getTypeSaisieMeteo() const { return _typeSaisieMeteo; }

    unsigned int getNbTHread() const { return _nbThread; }

    /*!
    * \fn void addRay(LPTYRay &ray)
    * \brief Ajoute un rayon issue du lancer de rayon convertie au format Tympan.
    * \param ray : Référence vers un rayon Tympan à ajouter
    */
    void addRay(LPTYRay ray) { _tabRays.push_back(ray);}

    /*!
    * \fn TYTabRay getAllRays()
    * \brief Renvoie un tableau contenant tous les rayons trouvés par le lancer de rayons.
    * \return Renvoie le tableau de rayons Tympan
    */
    TYTabRay& getAllRays() { return _tabRays; }

    /*!
     * \fn void setTabRay(const TYTabRay& tabRay)
     * \brief set the vector of TYRays
     */
    void setTabRays(const TYTabRay& tabRays) { _tabRays = tabRays; }

    /*!
    * \fn void addAcousticEdge(TYAcousticEdge &edge)
    * \brief Ajoute une arête de diffraction générée par le lancer de rayons.
    * \param : Référence vers une arête Tympan à ajouter.
    */
    void addAcousticEdge(TYAcousticEdge& edge) { _tabEdges.push_back(edge); }

    /*!
    * \fn TYTabAcousticEdge getAllAcousticEdges()
    * \brief Renvoie un tableau contenant l'ensemble des arêtes de diffractions générées par le lancer de rayon.
    * \return Renvoie le tableau d'arêtes Tympan.
    */
    TYTabAcousticEdge& getAllAcousticEdges() { return _tabEdges; }

    std::unique_ptr<tympan::AcousticResultModel>  _acousticResult;
    std::unique_ptr<tympan::AcousticProblemModel> _acousticProblem;

protected:
    /**
     * \fn void updateGraphicMaillage()
     * \brief Mets a jour l'objet graphique des maillage apres calcul.
     */
    void updateGraphicMaillage();

    // Membres
protected:
    ///Numero du Calcul.
    int _numero;
    ///Nom de l'auteur.
    QString _auteur;
    ///Date de creation.
    QString _dateCreation;
    ///Date de modification.
    QString _dateModif;
    ///Commentaires.
    QString _comment;

    ///etat a jour du calcul
    bool _upTodate;

    ///Expansion geometrique (0 : 2pi, 1 : 4pi).
    int _expansGeo;
    ///sol reel/sol reflechissant.
    bool _useSol;

    /// Type de calcul du sol
    int _typeCalculSol;

    ///avec ou sans sol (anechoique ou semi-anechoique)
    bool _anechoic;
    ///Conditions favorables ou non a la propagation.
    bool _condFav;
    ///Prise ou non en compte de la vegetation.
    bool _useVegetation;
    ///Prise ou non en compte de l'atmosphere.
    bool _useAtmosphere;
    ///Prise ou non en compte des murs, batiments et machines presents dans le site.
    bool _useEcran;
    /// Calcul des trajets horizontaux
    bool _bCalculTrajetsHorizontaux;


    ///Prise ou non de la reflexion.
    bool _useReflexion;
    ///Calcul energetique ou avec interference.
    bool _interference;

    /// Sauvegarde des rayon (TYRay)
    bool _bSaveRay;

    ///Parametre h1 pour un calcul avec conditions favorables.
    double _h1;
    ///Distance minimale entre une source et un recepteur
    double _distanceSRMin;

    ///Precision sur la connaissance de la geometrie du modele exprime en pour-cent
    double _geomPrecision;

    /// Distance minimale entre deux points consideres comme confondus
    double _seuilConfondus;

    /// Parametres de calcul harmonoise
    unsigned int HARM_averaging; // Lissage ou pas
    unsigned int HARM_airAbsorption; // Prise en compte de l'absorption atmospherique
    unsigned int HARM_turbulenceScattering; // Prise en compte de la diffusion turbulente
    unsigned int HARM_diffHaddenPierce; // Formule de diffraction de Hadde-Pierce
    unsigned int HARM_randomTerrain; // Perturbation aleatoire du profil du terrain

    ///Frequence minimale pour les spectres.
    float _freqMinSpectre;
    ///Frequence maximale pour les spectres.
    float _freqMaxSpectre;

    /// Etat du calcul actif/bloque
    int _state;

    /// Utilisation d'un calcul externe
    bool _useExternal;

    /// Type de calcul
    unsigned short _refExternal;

    unsigned short _typeSaisieMeteo;


    /// Site sur lequel va s'effectuer le calcul
    LPTYSiteNode _pSiteCalcul;

    ///Tableau des ID des elements presents dans la scene.
    TYListID _elementSelection;
    ///Etat (rayonnant/non rayonnant) des elements de la scene.
    TYMapPtrElementBool _emitAcVolNode;
    ///Regime des elemenst de la scene
    TYMapPtrElementInt _mapElementRegime;

    ///Atmosphere.
    LPTYAtmosphere _pAtmosphere;

    ///Collections de Maillages.
    TYTabMaillageGeoNode _maillages;

    ///Resultat.
    LPTYResultat _pResultat;

    /// Tableau des trajets du calcul
    std::vector<TYTrajet> _tabTrajets; // Tableau des trajets

    /// Identification du solveur utilise
    OGenID _solverId;

    /// Nombre de process de calcul simultane
    unsigned int _nbThread;

    //Rayons valides produit par le lancer de rayons
    TYTabRay _tabRays;

    //Ensemble des aretes de diffractions générées par le lancer de rayons
    TYTabAcousticEdge _tabEdges;
};

#include "TYProjet.h"


#endif // __TY_CALCUL__

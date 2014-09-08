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

#include "Tympan/core/interfaces.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"
#include "Tympan/models/business/TYRay.h"
#include "Tympan/models/business/TYMaillage.h"
#include "Tympan/models/business/TYPointControl.h"
#include "Tympan/models/business/TYResultat.h"

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
    LPTYResultat getResultat() { return _pResultat; }

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
     * \fn bool isCalculPossible(const int& nbSources, const int& nbRecepteurs, const LPTYSite pMergeSite)
     *\brief verification des calculs
     *   Verifie que le calcul est possible: au moins une source, au moins un
     *   recepteur, pas de depassement du max de sources et de recepteurs.
     */
    bool isCalculPossible(const int& nbSources, const int& nbRecepteurs, const LPTYSiteNode pMergeSite);

    /**
     * \fn bool go(SolverInterface* pSolver)
     * \brief Lance l'execution du Calcul.
     * \return <code>true</code> si le calcul a reussi;
     *         <code>false</code> sinon.
     */
    bool go(SolverInterface* pSolver);

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
     * -     * \fn OGenID getSolverId()
     *       -     *     void setSolverId(const OGenID& iD)
     *       -     * \brief Get/Set du solveur ID
     *       -     * \return _solverId
     */
    void setSolverId(const OGenID& iD) { _solverId = iD; }
    const OGenID getSolverId() const { return _solverId; }
    OGenID getSolverId() { return _solverId; }

    void goPostprocessing();
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
    // solver to be used to solve this "calcul"
    OGenID _solverId;

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
    /// Etat du calcul actif/bloque
    int _state;

    /// Site sur lequel va s'effectuer le calcul
    LPTYSiteNode _pSiteCalcul;

    ///Tableau des ID des elements presents dans la scene.
    TYListID _elementSelection;
    ///Etat (rayonnant/non rayonnant) des elements de la scene.
    TYMapPtrElementBool _emitAcVolNode;
    ///Regime des elemenst de la scene
    TYMapPtrElementInt _mapElementRegime;

    ///Collections de Maillages.
    TYTabMaillageGeoNode _maillages;

    ///Resultat.
    LPTYResultat _pResultat;
};

#include "TYProjet.h"


#endif // __TY_CALCUL__

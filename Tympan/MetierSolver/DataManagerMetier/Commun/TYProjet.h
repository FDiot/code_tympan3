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

#ifndef __TY_PROJET__
#define __TY_PROJET__


#include "TYCalcul.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"
#include "TYPointControl.h"

#if TY_USE_IHM
#include "Tympan/GraphicIHM/DataManagerGraphic/TYProjetGraphic.h"
#endif

#include <iostream>
#include <vector>
using namespace std;


/**
 * \file TYProjet.h
 * \class TYProjet
 * \brief classe de definition d'un projet.
 *
 * \vesion v 1.1
 * \date 2008/01/21
 * \author Projet_Tympan
 *
 */
class TYProjet: public TYElement
{
    OPROTOSUPERDECL(TYProjet, TYElement)
    TY_EXTENSION_DECL_ONLY(TYProjet)
    TY_EXT_GRAPHIC_DECL(TYProjet)

    // Methodes
public:
    /**
     * \fn TYProjet()
     * \brief Constructeur.
     *  Le constructeur de la classe TYProjet
     */
    TYProjet();

    /**
     * \fn TYProjet(const TYProjet& other)
     * \brief Constructeur par copie .
     *  Le constructeur par copie de la classe TYProje .
     */
    TYProjet(const TYProjet& other);

    /**
     * \fn
     * \brief Destructeur
     * Le destructeur de la classe TYProjet .
     */
    virtual ~TYProjet();

    ///Operateur =.
    TYProjet& operator=(const TYProjet& other);

    ///Operateur ==.
    bool operator==(const TYProjet& other) const;

    ///Operateur !=.
    bool operator!=(const TYProjet& other) const;

    virtual std::string toString() const;

    virtual DOM_Element toXML(DOM_Element& domElement);

    virtual int fromXML(DOM_Element domElement);

    /**
     * \fn QString getAuteur()
     * \brief Get du nom de l'auteur.
     * \retuen _auteur
     */
    QString getAuteur() const { return _auteur; }

    /**
     * \fn void setAuteur(const QString& auteur)
     * \brief Set du nom de l'auteur.
     */
    void setAuteur(const QString& auteur) { _auteur = auteur; }

    /**
     * \fn QString getDateCreation() const
     * \brief Get de la date de creation.
     * \return _dateCreation
     */
    QString getDateCreation() const { return _dateCreation; }

    /**
     * \fn    void setDateCreation(const QString& date)
     * \brief Set de la date de creation.
     */
    void setDateCreation(const QString& date) { _dateCreation = date; }

    /**
     * \fn QString getDateModif()const
     * \brief Get de la date de modification.
     * \return _dateModif
     */
    QString getDateModif() const { return _dateModif; }

    /**
     *  \fn   void setDateModif(const QString& date)
     * \brief Set de la date de modification.
     */
    void setDateModif(const QString& date) { _dateModif = date; }

    /**
     * \fn QString getComment() const { return _comment; }const
     * \brief Get des commentaires.
     * \return _comment
     */
    QString getComment() const { return _comment; }

    /**
     * \fn void setComment(const QString& comment) { _comment = comment;
     * \brief Set des commentaires.
     */
    void setComment(const QString& comment) { _comment = comment; }

    /**
     * \fn LPTYSiteNode getSite()
     * \brief Get du site.
     * \return _pSite
     */
    LPTYSiteNode getSite() { return _pSite; }

    /**
     * \fn const LPTYSiteNode getSite() const
     * \brief Get du site.
     * \return _pSite
     */
    const LPTYSiteNode getSite() const { return _pSite; }

    /**
     * \fn  void setSite(const LPTYSiteNode pSite)
     * \brief Set du site.
     */
    void setSite(const LPTYSiteNode pSite);

    /**
     * Set/Get du maillage des points de controle.
     */
    TYTabLPPointControl& getPointsControl() { return _pointsControl; }
    /**
     * Set/Get du maillage des points de controle.
     */
    const TYTabLPPointControl& getPointsControl() const { return _pointsControl; }
    /**
     * Set/Get du maillage des points de controle.
     */
    void setPointsControl(const TYTabLPPointControl& pointsControl) { _pointsControl = pointsControl; setIsGeometryModified(true); }
    /**
     * Ajout un nouveau point de controle.
     */
    bool addPointControl(LPTYPointControl pPointControl);
    /**
     * Supprime un point de controle.
     */
    bool remPointControl(LPTYPointControl pPointControl);

    /**
     * \fn LPTYPointControl duplicatePointControle(const LPTYPointControl& pPoint)
     * \briefDuplique pPoint.
     * \param pPoint PointControl a dupliquer.
     * \return Le nouveau point de controle.
     */
    LPTYPointControl duplicatePointControl(const LPTYPointControl& pPoint);

    /**
     * Retourne un point de controle.
     */
    LPTYPointControl getPointControl(int index) { return _pointsControl[index]; }
    /**
     * Supprime tous les points de controle.
     */
    bool remAllPointControl();

    /**
     * Mets a jour l'altitude des maillage en fonction de l'altimetrie du
     * site associe au projet parent a ce calcul.
     */
    bool updateAltiRecepteurs();

    bool updateAltiRecepteurs(const TYAltimetrie* pAlti);

    /**
     * Met a niveau l'altimetrie d'un point de controle
     */
    bool updateAltiPointControle(TYPointControl* pPtControl, const TYAltimetrie* pAlti);


    /**
     * \fn TYTabLPCalcul& getListCalcul()
     * \brief Set/Get de la liste des Calcul.
     * \return _listCalcul
     */
    TYTabLPCalcul& getListCalcul() { return _listCalcul; }

    /**
     * Set/Get de la liste des Calcul.
     */
    const TYTabLPCalcul& getListCalcul() const { return _listCalcul; }

    /**
     * Set/Get de la liste des Calcul.
     */
    void setListCalcul(const TYTabLPCalcul& list) { _listCalcul = list; setIsGeometryModified(true); }

    /**
     * \fn bool addCalcul(LPTYCalcul pCalcul)
     * \brief Ajout d'un Calcul a la liste des Calcul.
     * \param pCalcul Le Calcul a ajouter.
     */
    bool addCalcul(LPTYCalcul pCalcul);

    /**
     * \fn bool remCalcul(const LPTYCalcul pCalcul)
     * \brief Suppression d'un Calcul a la liste des Calcul.
     * \param pCalcul Le Calcul a supprimer.
     */
    bool remCalcul(const LPTYCalcul pCalcul);

    /**
     * \fn bool remCalcul(QString idCalcul)
     * \brief Suppression d'un Calcul a la liste des Calcul.
     * \param Calcul L'ID du Calcul a supprimer.
     */
    bool remCalcul(QString idCalcul);

    /**
     * \fn void remAllCalcul()
     * \brief Suppression de tous les elements.
     */
    void remAllCalcul();

    /**
     * \fn LPTYCalcul getCurrentCalcul()
     * \brief Set/Get du pointeur du Calcul courant.
     * \return _pCurrentCalcul
     */
    LPTYCalcul getCurrentCalcul() { return _pCurrentCalcul; }

    /**
     *  \fn   const LPTYCalcul getCurrentCalcul() const
     * \brief Set/Get du pointeur du Calcul courant.
     */
    const LPTYCalcul getCurrentCalcul() const { return _pCurrentCalcul; }

    /**
     *  \fn  void setCurrentCalcul(LPTYCalcul pCurCalcul)
     * \brief Set/Get du pointeur du Calcul courant.
     */
    void setCurrentCalcul(LPTYCalcul pCurCalcul);

    /**
     * \fn LPTYCalcul duplicateCalcul(const LPTYCalcul pCalcul)
     * \briefDuplique pCalcul.
     * \param pCalcul Calcul a dupliquer.
     * \return Le nouveau calcul.
     */
    LPTYCalcul duplicateCalcul(const LPTYCalcul pCalcul);

    /*!
     * \brief copie l'etat des points de controle pour un calcul pour un autre calcul
     * \param pCalculRef : Calcul dont l'etat pour les points doit etre copie
     * \param pCalculNew : Calcul pour lequel l'etat doit etre duplique
     */
    void duplicatePtCalcState(const TYCalcul* pCalculRef, TYCalcul* pCalculNew);

    /**
     * \fn void remElmtFromCalculs(TYElement* pElement)
     * \brief Supprime un element de tous les calculs
     */
    void remElmtFromCalculs(TYElement* pElement);

    /**
     * \fn void remTabElmtFromCalculs( vector<LPTYGeometryNode> tabGeoNode)
     * \brief Supprime un ensemble d'elements des calculs
     */
    void remTabElmtFromCalculs(vector<LPTYGeometryNode> tabGeoNode);

    /**
     * \fn void setDelaunayTolerence(const double & val)
     *     double getDelaunayTolerence()
     *     const double getDelaunayTolerence()
     * \brief Get/Set du parametre de Delaunay
     * \return _delaunayTolerance
     */
    void setDelaunayTolerence(const double& val) { _delaunayTolerance = val; }
    double getDelaunayTolerence() { return _delaunayTolerance; }
    const double getDelaunayTolerence() const { return _delaunayTolerance; }

    /**
     * \fn void setDMax(const double & val)
     *     double getDMax()
     *     const double getDMax()
     * \brief Get/set de la distance max entre les points
     * \return _maxDistBetweenPoints
     */
    void setDMax(const double& val);
    double getDMax() { return _maxDistBetweenPoints; }
    const double getDMax() const { return _maxDistBetweenPoints; }

    /**
     * \fn void setUseDefGeomVal(const bool & bVal)
     *     bool getUseDefGeomVal()
     *     const bool getUseDefGeomVal()
     * \brief Get/Set de l'option d'utilisation des valeurs par defaut
     * \return _useDefaultGeomVal
     */
    void setUseDefGeomVal(const bool& bVal) { _useDefaultGeomVal = bVal; verifGeometricParam(); }
    bool getUseDefGeomVal() { return _useDefaultGeomVal; }
    const bool getUseDefGeomVal() const { return _useDefaultGeomVal; }

    /**
     * \fn void verifGeometricParam()
     * \brief verification des parametres de geometrisation (si necessaire)
     */
    void verifGeometricParam();

    /**
     * \fn void forceUseDefault()
     * \brief Force l'utilisation des parametres par defaut
     */
    void forceUseDefault();

    /**
     * \fn const double getDefaultDelaunayTolerence()
     * \brief Relecture de la valeur de tolerence de Delaunay par defaut
     */
    const double getDefaultDelaunayTolerence() const;

    /**
     * \fn const double getDefaultDMax()
     * \brief Relecture de la distance minimale entre deux points par defaut
     */
    const double getDefaultDMax() const;

    /**
     * \fn bool getStatusSolver()
     *     const bool getStatusSolver()
     *     void setStatusSolver(const bool& bStatus)
     * \brief Get/Set du statut de disponibilite du solveur du calcul courant
     * \return _bStatusSolver
     */
    bool getStatusSolver() { return _bStatusSolver; }
    const bool getStatusSolver() const { return _bStatusSolver; }
    void setStatusSolver(const bool& bStatus) { _bStatusSolver = bStatus; }

public:
    // CLM-NT33 : Sauvegarde sans resultats
    static bool gSaveValues;

    // Membres
protected:
    ///Nom de l'auteur.
    QString _auteur;
    ///Date de creation.
    QString _dateCreation;
    ///Date de modification.
    QString _dateModif;
    ///Commentaires.
    QString _comment;

    ///Site
    LPTYSiteNode _pSite;

    ///Liste des Calcul
    TYTabLPCalcul _listCalcul;

    ///Liste des points de controles.
    TYTabLPPointControl _pointsControl;

    ///Un pointeur sur le Calcul courant.
    LPTYCalcul _pCurrentCalcul;

    /// Parametres de geometrisation
    double _delaunayTolerance;
    double _maxDistBetweenPoints;
    bool _useDefaultGeomVal;

    // Statut de disponibilite du solveur du calcul courant
    bool _bStatusSolver;

};


#endif // __TY_PROJET__

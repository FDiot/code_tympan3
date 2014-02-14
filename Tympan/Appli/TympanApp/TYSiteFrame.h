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

/**
 * \file TYSiteFrame.h
 * \brief Frame pour la gestion de site (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 */

#ifndef __TY_SITE_FRAME__
#define __TY_SITE_FRAME__

#include <qwidget.h>
#include <qtreewidget.h>

#include "TYAppDefines.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"
#include "Tympan/MetierSolver/DataManagerMetier/EltMateriaux/TYAtmosphere.h"
#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYCalcul.h"

class QMenuBar;
class QListView;

/**
 * \class TYSiteFrame
 * \brief Frame pour la gestion de site.
 */
class TYSiteFrame : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYSiteFrame(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYSiteFrame();

    /**
     * Set/get le site node root.
     */
    TYSiteNode* getSiteNodeRoot() { return _pSiteNodeRoot; }

    /**
     * Vide la list view.
     */
    void clear() { _pListView->clear(); }

    /**
     * Sauvegarde au format CSV
     */
    void exportEXCEL(TYElement* pElement);

    /**
     * Boite de dialogue position
     */
    void changePos(TYElement* pElement);

    /**
     * Boite de dialogue de rotation
     */
    void changeRotation(TYElement* pElement);

    /**
     * Mise en evidence d'un element (highlight)
     */
    void localise(TYElement* pElement, TYElementGraphic* pGraphicObject);

    /**
     * Effacement d'un element de l'arborescence
     */
    bool supprime(TYElementListItem* eltItem);

    /**
     * Import d'un element depuis la librairie
     */
    void importFromLib(const unsigned int& filter, TYElement* pElement);

    /**
     * Import d'un element depuis un fichier XML
     */
    void addEltXML(const char* className, TYElement* pElement);

    /**
     * Ajout d'un nouvel element
     */
    void newElt(const char* className, TYElement* pElement);

    /**
     * Ajout d'un element (cree depuis un fichier ou nouveau)
     */
    void addElt(TYElement* pElement, TYElement* pElt);

    /**
     * Selection ou deselection de tous les elements "cochables" du meme type
     */
    void selectOrUnselectAll(TYElementListItem* item, const bool& bSelect);

    /**
     * Remet le parametre distance max entre deux points d'une courbe (niveau ou plan d'eau) a la valeur par defaut
     */
    void setDMaxDefault(TYElementListItem* item);

public slots:
    /**
     * Set/get le site node root.
     */
    void setSiteNodeRoot(LPTYSiteNode pSiteNode);

    /**
     * Mets a jour la liste a partir du site node root.
     */
    void updateList();

    /**
     * Mets a jour le calcul courant.
     */
    void setCurrentCalcul(LPTYCalcul pCalcul) { _pCurrentCalcul = pCalcul; updateList(); }

    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addToList(LPTYSiteNode pElement, TYElementListItem* parent = NULL);
    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addToList(LPTYInfrastructure pElement, TYElementListItem* parent = NULL);
    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addToList(LPTYTopographie pElement, TYElementListItem* parent = NULL);
    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addToList(LPTYMateriauConstruction pElement, TYElementListItem* parent = NULL);
    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addToList(LPTYSol pElement, TYElementListItem* parent = NULL);
    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addToList(LPTYSpectre pElement, TYElementListItem* parent = NULL);
    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addToList(LPTYSourcePonctuelle pElement, TYElementListItem* parent = NULL);
    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addToList(LPTYMachine pElement, TYElementListItem* parent = NULL);
    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addToList(LPTYBatiment pElement, TYElementListItem* parent = NULL);
    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addToList(LPTYAtmosphere pElement, TYElementListItem* parent = NULL);

    /**
     * Supprime un element dans l'infrastructure.
     */
    bool remInInfra(LPTYSiteNode _siteNode, TYElementListItem* item, const char* className);

    /**
     * Supprime un element dans la topographie.
     */
    bool remInTopo(LPTYSiteNode _siteNode, TYElementListItem* item, const char* className);

    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuEvent(QContextMenuEvent* e);

    /**
     * Supprime un element du site.
     *
     * @param item Item de la liste correspondant a l'element a supprimer.
     * @return Vrai si l'element a bien ete supprime.
     */
    bool remFromList(TYElementListItem* item);

    /**
     * Mise a jour d'un l'element.
     *
     * @param item Item de la liste correspondant a l'element.
     */
    void apply(QTreeWidgetItem*, int);

    /**
     * Ouverture d'un modeleur (sur double clic)
     */
    void openModeler(QTreeWidgetItem*, int);

private :
    /**
     * Effectue la suppression effective de l'element en fonction du contexte
     * Est appelee par remFromList(...)
     */
    bool remFromSite(TYElement* pElement);

protected:
    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addEltToList(LPTYElement pElement, TYElementListItem* parent = NULL, const bool& chekable = false);


protected:
    QMenuBar* _pMenuBar;
    QTreeWidget* _pListView;

    ///Le site node root.
    LPTYSiteNode _pSiteNodeRoot;

    ///L'Id du calcul courant.
    LPTYCalcul _pCurrentCalcul;
};

#endif //__TY_SITE_FRAME__

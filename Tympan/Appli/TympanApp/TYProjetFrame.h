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
 * \file TYProjetFrame.h
 * \brief Frame pour la gestion de projet (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_PROJET_FRAME__
#define __TY_PROJET_FRAME__


#include <qwidget.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QTreeWidget>

#include "TYAppDefines.h"
#include "Tympan/models/business/TYProjet.h"

class QMenuBar;
class TYSpectresManagerFrame;


/**
 * \class TYProjetFrame
 * \brief Frame pour la gestion de projet.
 */
class TYProjetFrame : public QWidget
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYProjetFrame(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYProjetFrame();

    /**
     * Set/get du projet.
     */
    LPTYProjet getProjet() { return _pProjet; }

    /**
     * Vide la list view.
     */
    void clear() { _pListView->clear(); }

    TYSpectresManagerFrame* getTYSpectresManagerFrame() { return _pSpectresManagerFrame;};

    /**
     * Indique dans le treeview si le calcul doit etre relance
     */
    void setCalculDone(bool state);


public slots:
    /**
     * Set/get du projet.
     */
    void setProjet(LPTYProjet pProjet);

    /**
     * Mets a jour de toute la liste.
     */
    void updateList();

    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addToList(LPTYProjet pElement, TYElementListItem* parent = NULL);
    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addToList(LPTYCalcul pElement, TYElementListItem* parent = NULL);

    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuEvent(QContextMenuEvent* e);


    void keyPressEvent(QKeyEvent* e);

    /**
     * Action sur double clic : ouverture du modeleur ou affichage des proprietes
     */
    void doubleClic(QTreeWidgetItem*, int);

    /**
     * Mise a jour d'un l'element.
     *
     * @param item Item de la liste correspondant a l'element.
     */
    void apply(QTreeWidgetItem*, int);

signals :
    /**
     * Signal de changement de calcul.
     */
    void changeCurrentCalcul(LPTYCalcul pCalcul);
    /**
     * Signal de changement de site.
     */
    void changeSite(LPTYSiteNode pSite);

protected:
    /**
     * Ajoute un element a la liste.
     * Si le parent est NULL, l'item est ajoute a la racine.
     */
    TYElementListItem* addEltToList(LPTYElement pElement, TYElementListItem* parent = NULL);

    /**
     * Supprime un element du site.
     *
     * @param item Item de la liste corrspondant a l'element a supprimer.
     * @return Vrai si l'element a bien ete supprime.
     */
    bool remFromList(TYElementListItem* item);

    /**
     * Export du maillage au format csv
     */
    void exportMaillageCSV(const LPTYMaillage pMaillage);

    /**
     * Change l'affichage d'un item du treeview de calcul grise ou non
     */
    void setCalculItemState(TYElementListItem* pCalcItem, bool state);

    /**
     * Selection ou deselection de tous les elements "cochables" du meme type
     */
    void selectOrUnselectAll(TYElementListItem* item, const bool& bSelect);

protected:
    QMenuBar* _pMenuBar;
    QTreeWidget* _pListView;

    ///Le projet.
    LPTYProjet _pProjet;

    ///Le calcul courant.
    LPTYCalcul _pCurrentCalcul;

    ///L'item du treeview du projet
    TYElementListItem* _pCurrentCalculItem;

    TYSpectresManagerFrame* _pSpectresManagerFrame;
};

#endif //__TY_PROJET_FRAME__

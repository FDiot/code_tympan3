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
 * \file TYResultatTreeDialog.h (fichier header)
 * \brief Boite de dialogue pour la representation par une arborescence des elements contribuant au resultat d'un point de calcul (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_RESULTAT_TREE_DIALOG__
#define __TY_RESULTAT_TREE_DIALOG__


#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYResultat.h"

#include <qdialog.h>
#include <qmap.h>
//Added by qt3to4:
#include <QShowEvent>
#include <QCloseEvent>
class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;


typedef struct
{
    // Utilisation d'un map la ou un set aurait suffit, mais ca voulait pas compiler
    std::map <TYElement*, TYElement*> pChildsList;
    OSpectre spectre;
} childTreeItem;


/**
 * \class TYResultatTreeDialog
 * \brief Boite de dialogue pour la representation par une arborescence des elements contribuant au resultat d'un point de calcul
 */
class TYResultatTreeDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYResultatTreeDialog(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYResultatTreeDialog();

    void followChilds(QMap<TYElement*, childTreeItem*>& mapElementChilds,
                      QMap<TYElement*, childTreeItem*>::iterator it,
                      QMap<TYElement*, QTreeWidgetItem*>& mapParentItem,
                      QTreeWidgetItem* pCurItem);

public slots:
    /**
     * Affiche l'arborescence de contribution pour ce point de calcul.
     */
    void set(LPTYResultat pResultat, LPTYPointCalcul pPtCalcul);

    /**
     * Mets a jour l'arborescence de contribution.
     */
    void updateContent();


protected:
    virtual void showEvent(QShowEvent* pEvent);
    virtual void closeEvent(QCloseEvent* pEvent);


protected:
    ///Bouton pour fermer la boite de dialogue.
    QPushButton* _pCloseButton;

    ///La liste pour la representation de l'arborescence.
    QTreeWidget* _pListView;

    ///Le resultat associe.
    LPTYResultat _pResultat;

    ///Le point de calcul associe.
    LPTYPointCalcul _pPtCalcul;
};


#endif //__TY_RESULTAT_TREE_DIALOG__

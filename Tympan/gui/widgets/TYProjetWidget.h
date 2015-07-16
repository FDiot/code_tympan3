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
 * \file TYProjetWidget.h
 * \brief outil IHM pour un projet (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 *
 */

#ifndef __TY_PROJET_WIDGET__
#define __TY_PROJET_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYProjet;
class QLineEdit;
class QGridLayout;
class QLabel;
class QTreeWidget;
class QTreeWidgetItem;
class QGroupBox;
class QPushButton;
class QTextEdit;
class QDateEdit;
class TYElementWidget;
class QCheckBox;
class QTabWidget;
class QTableWidget;

/**
 * \class TYProjetWidget
 * \brief classe de l'objet IHM pour un objet
 */
class TYProjetWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYProjet)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYProjetWidget(TYProjet* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYProjetWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget du Calcul courant.
     */
    void editCurCalcul();
    /**
     * Edite la widget du site.
     */
    void editSite();

    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuEvent(QContextMenuEvent* e);

    /**
     * Edite la widget du Calcul selectionne.
     *
     * @param item Element de la liste correspondant au Calcul selectionne.
     */
    void editCalcul(QTreeWidgetItem* item);

    /**
     * Change l'etat des parametres de geometrisation
     */
    void updateDefault();

private:
    void updateControlPointsTab();
    void updateNoiseMapsTab();

    // Membres
protected:

    QGridLayout* _projetLayout;

    QGroupBox* _groupBox;
    QLabel* _labelAuteur;
    QLineEdit* _lineEditAuteur;
    QLabel* _labelDateCreation;
    QDateEdit* _editDateCreation;
    QLabel* _labelDateModif;
    QDateEdit* _editDateModif;
    QLabel* _labelComment;
    QTextEdit* _lineEditComment;

    QPushButton* _pushButtonSite;
    QGroupBox* _groupBoxSite;
    QLineEdit* _lineEditNomSite;
    QLabel* _labelDistMax;
    QLineEdit* _lineEditDistMax;
    QLabel* _labelDelaunayTolerence;
    QLineEdit* _lineEditDelaunayTolerence;
    QLabel* _labelUseDefault;
    QPushButton* _pushButtonUseDefault;

    QGroupBox* _groupBoxCurCalcul;
    QLineEdit* _lineEditNomCurCalcul;
    QPushButton* _pushButtonCurCalcul;

    QGroupBox* _groupBoxTab;
    QTreeWidget* _listViewTabCalcul;

    // Use of tabs
    QTabWidget* _tabWidget;

    // Table des points de controle
    QTableWidget*_tableauPointsControle;

    /// Onglet des points de controle
    QTableWidget* _tableauMaillages;

    TYElementWidget* _elmW;
};


#endif // __TY_PROJET_WIDGET__

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
 * \file TYEtageWidget.h
 * \brief outil IHM pour un etage (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ETAGE_WIDGET__
#define __TY_ETAGE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYEtage;
class QLineEdit;
class QGridLayout;
class QLabel;
class QTreeWidget;
class QGroupBox;
class QPushButton;
class QTreeWidgetItem;
class QButtonGroup;
class QTableWidget;
class QTabWidget;
class TYAcousticVolumeWidget;
class QCheckBox;
class QRadioButton;

/**
 * \class TYEtageWidget
 * \brief classe de l'objet IHM pour un etage
 */
class TYEtageWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYEtage)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYEtageWidget(TYEtage* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYEtageWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget du sol.
     */
    void editSol();
    /**
     * Edite la widget du plafond.
     */
    void editPlafond();

    /**
     * Edite la widget du mur selectionne.
     *
     * @param item Element de la liste correspondant au mur selectionne.
     */
    void editMur(const int& item);

    /**
     * Edite la widget de la machine selectionne.
     *
     * @param item Element de la liste correspondant a la machine selectionne.
     */
    void editMachine(const int& item);

    /**
     * Edite la widget de la source selectionnee.
     *
     * @param item Element de la liste correspondant a la source selectionne.
     */
    void editSource(const int& item);

    /// Affichage du champ reverbere
    void showReverb();

    /// Affichage du temps de reverberation
    void showTR();

    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuEvent(QContextMenuEvent* e);

    /**
     * Gestion du changement de regime de l'etage
     */
    void propagateRegime(int regime);

private:
    void clearTables();

    /** Recuperation des informations des murs */
    void updateFromMurs();
    /** Mise a jour des informations des murs */
    void applyToMurs();


    // Membres
protected:
    QButtonGroup* _buttonGroupReverb;
    QRadioButton* _pRadioButtonSabine;
    QRadioButton* _pRadioButtonKuttruff;

    QGroupBox* _groupBoxDalles;
    QGroupBox* _groupBoxSol;
    QLineEdit* _lineEditNomSol;
    QPushButton* _pushButtonSol;
    QCheckBox* _checkBoxSol;
    QGroupBox* _groupBoxPlafond;
    QLineEdit* _lineEditNomPlafond;
    QPushButton* _pushButtonPlafond;
    QCheckBox* _checkBoxPlafond;


    //  QWidget* _pMursWidget;
    QGroupBox* _groupBoxListMur;
    QTableWidget* _tableFaces;

    // Gestion des onglet pour les proprietes
    QTabWidget* _tabWidget;

    // Gestion des etats des machines/sources
    QGroupBox* _groupBoxListMachine;

    QTabWidget* _tabWidgetSources;
    QTableWidget* _tableMachine;
    QTableWidget* _tableSource;


    QGridLayout* _etageLayout;
    QGridLayout* _groupBoxDallesLayout;
    QGridLayout* _groupBoxSolLayout;
    QGridLayout* _groupBoxPlafondLayout;
    QGridLayout* _groupBoxListMurLayout;
    QGridLayout* _groupBoxListMachineLayout;

    QGroupBox* _groupBoxDimensions;
    QGridLayout* _groupBoxDimensionsLayout;
    QLabel*     _dimensionsHauteurLabel;
    QLineEdit*  _dimensionsHauteurLineEdit;

    TYAcousticVolumeWidget* _elmW;
};


#endif // __TY_ETAGE_WIDGET__

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
 * \file TYSiteNodeWidget.h
 * \brief outil IHM pour un ensemble de sites (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_SITENODE_WIDGET__
#define __TY_SITENODE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYSiteNode;
class TYElementWidget;
class QLineEdit;
class QGridLayout;
class QLabel;
class QTreeWidget;
class QTreeWidgetItem;
class QGroupBox;
class QPushButton;
class QCheckBox;
class QComboBox;

/**
 * \class TYSiteNodeWidget
 * \brief classe de l'objet IHM pour un ensemble de sites
 */
class TYSiteNodeWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYSiteNode)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYSiteNodeWidget(TYSiteNode* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYSiteNodeWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    // Code de TYSiteWidget
    /**
     * Edite la widget de la topographie.
     */
    void editTopo();
    /**
     * Edite la widget de l'infrastructure.
     */
    void editInfra();

    /**
     * Permet d'utiliser ou non le fichier topographique.
     */
    void changeStateTopoFile();

    /**
     * Edite une fenetre pour le choix du fichier de topographie.
     */
    void chooseTopoFile();

    // Code de TYSiteNodeWidget
    /**
     * Edite la widget du site selectionne.
     *
     * @param item Element selectionne de la liste.
     */
    void editSite(QTreeWidgetItem* item);

    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuEvent(QContextMenuEvent* e);

    /// Active/desactive la boite de saisie de l'altitude de la courbe de niveau
    void updateEditAlti();

    /// Changement du systeme de coordonnees
    void setSIGCoord(int typeCoord);

protected :
    /// Mise a jour de la liste des systemes de coordonnees SIG
    void updateSIGList();

private :
    /// Mise a jour des widgets lies au SIG
    void updateSIGData();


    // Membres
protected:

    // Elements de feu TYSiteWidget
    QGroupBox* _groupBox_0;
    QLabel* _labelEchelle;
    QLineEdit* _lineEditEchelle;
    QCheckBox* _checkBoxUseTopoFile;
    QLineEdit* _lineEditTopoFile;
    QLabel* _labelTopoFile;
    QGroupBox* _groupBoxInfra;
    QPushButton* _pushButtonInfra;
    QLineEdit* _lineEditNomInfra;
    QGroupBox* _groupBoxTopo;
    QLineEdit* _lineEditNomTopo;
    QPushButton* _pushButtonTopo;
    QPushButton* _pushButtonTopoFile;

    QGridLayout* _groupBox_0Layout;
    QGridLayout* _groupBoxInfraLayout;
    QGridLayout* _groupBoxTopoLayout;

    // Element de TYSiteNodeWidget
    QGroupBox* _groupBox_3;
    QLabel* _labelRoot;
    QLabel* _labelUseEmprise;
    QCheckBox* _checkBoxRoot;
    QCheckBox* _checkBoxUseEmprise;
    QGroupBox* _groupBox;
    QTreeWidget* _listViewListSite;

    // Saisie de l'altitude de la courbe de niveau associee a l'emprise
    QLineEdit* _lineEditAltiEmprise;
    QLabel* _labelAltiEmprise;

    // Choix du SIG
    QLabel* _labelModeCoord;
    QComboBox* _comboSIG;

    // Coordonees SIG
    QLabel* _labelSIG_X;
    QLineEdit* _lineEditSIG_X;
    QLabel* _labelSIG_Y;
    QLineEdit* _lineEditSIG_Y;


    QGridLayout* _sitenodeLayout;
    QGridLayout* _groupBox_3Layout;
    QGridLayout* _groupBoxLayout;

    TYElementWidget* _elmW;
};


#endif // __TY_SITENODE_WIDGET__

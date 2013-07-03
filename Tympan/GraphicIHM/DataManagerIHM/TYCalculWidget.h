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
 *
 * \file TYCalculWidget.h
 * \brief outil IHM pour un calcul (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_CALCUL_WIDGET__
#define __TY_CALCUL_WIDGET__


#include "TYWidget.h"

#include <vector>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>
using std::vector;
typedef vector<TYElement*> tabPtrElement;

class TYCalcul;
class TYEtatsWidget;
class QLineEdit;
class QGridLayout;
class QLabel;
class QTreeWidget;
class QGroupBox;
class QPushButton;
class QCheckBox;
class QTreeWidgetItem;
class QPoint;
class QTextEdit;
class QDateEdit;
class TYElementWidget;
class QTableWidget;
class QTabWidget;
class QRadioButton;
class QButtonGroup;
class QComboBox;
class TYAtmosphereWidget;

/**
 * \class TYCalculWidget
 * \brief classe de l'objet IHM pour un calcul
 */
class TYCalculWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYCalcul)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYCalculWidget(TYCalcul* pElement, QWidget* _pParent = NULL);
 
	/**
     * Destructeur.
     */
    virtual ~TYCalculWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget de l'atmosphere.
     */
    void editAtmosphere();
    /**
     * Edite la widget du resultat.
     */
    void editResultat();
    /**
     * Edite la widget du maillage.
     */
    void editMaillage(QTreeWidgetItem*);

    /**
     * Choix du calcul des parcours lateraux pour les ecrans
     */
    void updateUseEcran();

    /**
     * Choix du calcul des reflexions
     * Ce choix impose le calcul des trajets lateraux
     */
    void updateUseReflexion();

    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuEvent(QContextMenuEvent* e);


protected:
    void updateBoxSol();

    // Membres
protected:

    QLineEdit* _lineEditAuteur;
    QDateEdit* _editDateCreation;
    QDateEdit* _editDateModif;
    QTextEdit* _lineEditComment;
    QLabel* _labelAuteur;
    QLabel* _labelDateCreation;
    QLabel* _labelDateModif;
    QLabel* _labelComment;

    // Utilisation du sol
    QCheckBox* _checkBoxUseSol;
    QLabel* _labelUseSol;

    QRadioButton* _pRadioButtonSolReflechissant;
    QRadioButton* _pRadioButtonSolReel;

    QComboBox* _comboBoxTypeSol;

    // Affichage du type de calcul
	QComboBox* _comboSolver;

    QCheckBox* _checkBoxCondFav;
    QLabel* _labelCondFav;
    QRadioButton* _pRadioButtonCondFav;
    QRadioButton* _pRadioButtonCondHomo;

    QLineEdit* _lineEditParamH;
    QLabel* _labelParamH;
    QCheckBox* _checkBoxUseVegetation;
    QLabel* _labelUseVegetation;
    QCheckBox* _checkBoxUseAtmosphere;
    QLabel* _labelUseAtmosphere;

    // Calcul Energetique ou interferences
    QRadioButton* _radioButtonEnergetique;
    QRadioButton* _radioButtonInterference;

    // Utilisation des ecrans
    QLabel* _labelUseEcran;
    QCheckBox* _checkBoxUseEcran;
    QLabel* _labelParcoursLateraux;
    QCheckBox* _checkBoxParcoursLateraux;

    QCheckBox* _checkBoxUseReflexion;
    QLabel* _labelUseReflexion;
    QLineEdit* _lineEditDistanceSRMin;
    QGroupBox* _groupBox;
    QGroupBox* _groupBoxFlag;
    QGroupBox* _groupBoxAtmosphere;
    QGroupBox* _groupBoxMaillage;
    QGroupBox* _groupBoxResultat;

    QLineEdit* _lineEditNomAtmosphere;
    QPushButton* _pushButtonAtmosphere;

    // Presentation des resultats
    QLineEdit* _lineEditNomResultat;
    QPushButton* _pushButtonResultat;

    QLabel* _labelStoreGlobalMatrix;
    QCheckBox* _checkBoxStoreGlobalMatrix;

    QGridLayout* _calculLayout;

    TYElementWidget* _elmW;

    QWidget* _maillagesWidget;
    TYEtatsWidget* _etatsWidget;

    QButtonGroup* _buttonGroupState;

    QRadioButton* _pRadioButtonActif;
    QRadioButton* _pRadioButtonLocked;

    /// Gestion des differentes options sous forme d'onglet
    QTabWidget* _tabWidget;

    /// Onglet de description de la meteo
    TYAtmosphereWidget* _meteoWidget;

    /// Onglet des points de controle
    QTableWidget* _tableauPointControle;

    /// Onglet des points de controle
    QTableWidget* _tableauMaillages;
};


#endif // __TY_CALCUL_WIDGET__

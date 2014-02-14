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
 * \file TYAcousticLineWidget.h
 * \brief Outil IHM pour une ligne acoustique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTICLINE_WIDGET__
#define __TY_ACOUSTICLINE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticLine;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class QComboBox;
class QCheckBox;
class TYSourceWidget;
class QButtonGroup;
class QRadioButton;

/**
 * \class TYAcousticLineWidget
 * \brief classe de l'objet IHM pour une ligne acoustique
 */
class TYAcousticLineWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYAcousticLine)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticLineWidget(TYAcousticLine* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticLineWidget();

    /**
     * Mise a jour du comboBox des regimes
     */
    void updateComboRegime();


    /**
     * Mise a jour de l'etat a partir du regime courant
     */
    void updateFromCurrentRegime();



public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget de la source lineique.
     */
    void editSrcLin();
    /**
     * Creation d'un nouveau regime.
     */
    void createNewRegime();

    /**
     * Suppression du regime
     */
    void deleteRegime();

    /**
     * Changement de regime
     */
    void changeRegime(int regime);

	/*!
	 * \fn void saveCurrentRegime();
	 * \brief save current regime if changed
	 */
	void saveCurrentRegime();
	
	/**
     * Renomme le regime
     */
    void renameRegime(const QString& nom);

    /**
     * Visualise le spectre de l'etat courant de l'element
     */
    void showSpectre();

	/*!
	 * \fn void setSpectreToReadOnly();
	 * \brief Set Spectre to read only when in calculated mode
	 */
	void setSpectreToReadOnly();

    /**
     * Edite la widget de l'attenuateur.
     */
    void editAtt();

    /**
     * Active l'attenuateur
     */
    void useAttenuateur();



    // Membres
protected:

    // AcousticInterface
    QLabel* _labeUseAtt;
    QCheckBox* _checkBoxUseAtt;
    QLabel* _labelIsRayonnant;
    QCheckBox* _checkBoxIsRayonnant;

    QGroupBox* _groupBoxAtt;
    QLineEdit* _lineEditNomAtt;
    QPushButton* _pushButtonSpectreAtt;
    QGridLayout* _groupBoxAttLayout;

    // Gestion du type de distribution de la puissance
    QButtonGroup* _buttonGroupTypeDistrib;
    QRadioButton* _pRadioButtonCalculee;
    QRadioButton* _pRadioButtonImposee;

    QGroupBox* _groupBoxSpectre;
    QGridLayout* _groupBoxSpectreLayout;
    QPushButton* _pushButtonShowSpectre;

    QGroupBox* _groupBoxLine;
    QLabel* _labelLargeur;
    QLineEdit* _lineEditLargeur;
    QGroupBox* _groupBoxSrcLin;
    QLineEdit* _lineEditNomSrcLin;
    QPushButton* _pushButtonSrcLin;
    QGroupBox* _groupBoxListPt;

    QGridLayout* _acousticLineLayout;
    QGridLayout* _groupBoxLineLayout;
    QGridLayout* _groupBoxSrcLinLayout;
    QGridLayout* _groupBoxListPtLayout;

    // Choix du regime
    QComboBox* _comboBoxSelectRegime;
    QPushButton* _pushButtonNewRegime;
    QPushButton* _pushButtonRemRegime;

    // Choix de la densite de sources
    QGroupBox* _groupBoxDensite;
    QGridLayout* _groupBoxDensiteLayout;
    QLineEdit* _lineEditDensiteSrcs;

    TYSourceWidget* _elmW;
};


#endif // __TY_ACOUSTICLINE_WIDGET__

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
 * \file TYAcousticInterfaceWidget.h
 * \brief Outil IHM pour une interface acoustique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTICINTERFACE_WIDGET__
#define __TY_ACOUSTICINTERFACE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticInterface;
class QLineEdit;
class QGridLayout;
class QLabel;
class QCheckBox;
class QGroupBox;
class QPushButton;
class QButtonGroup;
class QWidget;
class QComboBox;
class QRadioButton;

/**
 * \class TYAcousticInterfaceWidget
 * \brief classe de l'objet IHM pour une interface acoustique
 */
class TYAcousticInterfaceWidget : public QWidget
{
    Q_OBJECT

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticInterfaceWidget(TYAcousticInterface* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticInterfaceWidget();

    /**
     * Mise a jour du comboBox des regimes
     */
    void updateComboRegime();

    /**
     * Mise a jour de l'etat a partir du regime courant
     */
    void updateFromCurrentRegime();

    /**
     * Sauvegarde des donnees du regime
     */
    void saveCurrentRegime();

    /**
     * Permet de desactiver la checkBox rayonnant
     */
    void setCheckBoxRayonnantState(const bool& active);

    /**
     * Permet de connaitre l'etat de la checkBox rayonnant
     */
    bool getCheckBoxRayonnantState();

    TYAcousticInterface* getElement() { return _pElement; }  //sm++ Correction de la macro TY_DECL_METIER_WIDGET

protected:
    bool eventFilter(QObject* obj, QEvent* ev);

public slots:
    virtual void updateContent();
    virtual void apply();
    virtual void reject();

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

    /**
     * Renomme le regime
     */
    void renameRegime();

    /**
     * Edite la widget de l'attenuateur.
     */
    void editAtt();

    void useAttenuateur();

    /// Note que la densite de source a ete modifiee
    void updateDensite();

    /**
     * Visualise le spectre de l'etat courant de l'element
     */
    void showSpectre();

    /**
     * Note que la case a cocher rayonnant a ete modifiee
     */
    void updateRayonnant();

signals:
    void regimeChanged(int);

    // Membres
protected:

    TYAcousticInterface* _pElement; //sm++ Correction de la macro TY_DECL_METIER_WIDGET

    QGroupBox* _groupBoxDensiteH;
    QLabel* _labelDensiteSrcsH;
    QLineEdit* _lineEditDensiteSrcsH;
    QGridLayout* _groupBoxDensiteHLayout;

    QGroupBox* _groupBoxDensiteV;
    QLabel* _labelDensiteSrcsV;
    QLineEdit* _lineEditDensiteSrcsV;
    QGridLayout* _groupBoxDensiteVLayout;

    QLabel* _labeUseAtt;
    QCheckBox* _checkBoxUseAtt;
    QLabel* _labelIsRayonnant;
    QCheckBox* _checkBoxIsRayonnant;

    QGroupBox* _groupBoxAtt;
    QLineEdit* _lineEditNomAtt;
    QPushButton* _pushButtonSpectreAtt;
    QGroupBox* _groupBox;
    QButtonGroup* _buttonGroupTypeDistrib;
    QRadioButton* _pRadioButtonCalculee;
    QRadioButton* _pRadioButtonImposee;

    QGroupBox* _groupBoxSpectre;
    QGridLayout* _groupBoxSpectreLayout;
    QPushButton* _pushButtonShowSpectre;

    QGridLayout* _acousticInterfaceLayout;
    QGridLayout* _groupBoxListSpLayout;
    QGridLayout* _groupBoxAttLayout;
    QGridLayout* _groupBoxLayout;

    // Choix du regime
    QGroupBox* _buttonGroupBox;
    QGridLayout* _buttonGroupBoxLayout;
    QComboBox* _comboBoxSelectRegime;
    QPushButton* _pushButtonNewRegime;
    QPushButton* _pushButtonRemRegime;


    ///Autorise ou non l'ajout et la suppression de regime.
    bool _bAddRemRegimeOk;

    ///Signale une modification des parametres
    bool _modified;
};


#endif // __TY_ACOUSTICINTERFACE_WIDGET__

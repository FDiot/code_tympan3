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
 * \file TYUserSourcePonctuelleWidget.h
 * \brief outil IHM pour une source ponctuelle (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_USERSOURCEPONCTUELLE_WIDGET__
#define __TY_USERSOURCEPONCTUELLE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYUserSourcePonctuelle;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class QCheckBox;
class QComboBox;
class TYSourcePonctuelleWidget;


/**
 * \class TYUserSourcePonctuelleWidget
 * \brief classe de l'objet IHM pour une source ponctuelle
 */
class TYUserSourcePonctuelleWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYUserSourcePonctuelle)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYUserSourcePonctuelleWidget(TYUserSourcePonctuelle* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYUserSourcePonctuelleWidget();

    /**
     * Sauvegarde des donnees du regime
     */
    void saveCurrentRegime();

protected:
    bool eventFilter(QObject* obj, QEvent* ev);

public slots:
    virtual void updateContent();
    virtual void apply();

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
    void editAttenuateur();

    /**
     * Activation/desactivation d'une attenuateur
     */
    void useAttenuateur();
    /**
     * Activation/desactivation d'une attenuateur
     */

    /**
     * Changement du type de rayonnement de la source
     */
//    void changeTypeRaynt(int typeRaynt);

public:
    /**
     * Edite la widget de la directivite selectionnee.
     */
//    void editDirectivite();

    /**
     * Mise a jour du comboBox des regimes
     */
    void updateComboRegime();

    /**
     * Mise a jour de l'etat a partir du regime courant
     */
    void updateFromCurrentRegime();

    // Membres
protected:
    QGridLayout* _sourcePonctuelleLayout;

    /// Gestion de l'attenuateur
    QGroupBox* _groupBoxAtt;
    QGridLayout* _groupBoxAttLayout;
    QPushButton* _pushButtonAtt;
    QLineEdit* _lineEditNomAtt;

    /// Gestion de la directivite
    //QGroupBox* _groupBoxDir;
    //QGridLayout* _groupBoxDirLayout;
    //QPushButton* _pushButtonDir;
    //QLineEdit* _lineEditNomDir;


    // Choix du regime
    QComboBox* _comboBoxSelectRegime;
    QPushButton* _pushButtonNewRegime;
    QPushButton* _pushButtonRemRegime;

    // Traitement de l'etat de fonctionnement courant
    QLabel* _labeUseAtt;
    QCheckBox* _checkBoxUseAtt;
    QLabel* _labelIsRayonnant;
    QCheckBox* _checkBoxIsRayonnant;


    TYSourcePonctuelleWidget* _elmW;
};


#endif // __TY_USERSOURCEPONCTUELLE_WIDGET__

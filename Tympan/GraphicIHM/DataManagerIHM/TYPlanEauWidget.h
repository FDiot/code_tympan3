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
 * \file TYPlanEauWidget.h
 * \brief outil IHM pour un plan d'eau (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_PLAN_EAU_WIDGET__
#define __TY_PLAN_EAU_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYPlanEau;
class QLabel;
class QLineEdit;
class QGridLayout;
class QGroupBox;
class QPushButton;
class TYTerrainWidget;
class QCheckBox;


/**
 * \class TYPlanEauWidget
 * \brief Classe de l'objet IHM pour un plan d'eau
 */
class TYPlanEauWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYPlanEau)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYPlanEauWidget(TYPlanEau* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYPlanEauWidget();


public slots:
    virtual void updateContent();
    virtual void apply();
    virtual void updateUseDefault();
    virtual void setDefaultValue();

    // Membres
protected:
    QGroupBox* _groupBoxAlt;
    QLabel* _labelAltitude;
    QLineEdit* _lineEditAltitude;

    QGroupBox* _groupBoxDistMax;
    QLabel* _labelDistMax;
    QLineEdit* _lineEditDistMax;
    QGridLayout* _groupBoxDistLayout;

    QPushButton* _pushButtonUseDefault;
    QLabel* _labelUseDefault;

    QGridLayout* _planEauLayout;
    QGridLayout* _groupBoxAltLayout;

    TYTerrainWidget* _terrainW;
};


#endif // __TY_PLAN_EAU_WIDGET__

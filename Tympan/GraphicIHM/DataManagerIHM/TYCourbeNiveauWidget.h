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
 * \file TYCourbeNiveauWidget.h
 * \brief Outil IHM pour une courbe de niveau (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_COURBENIVEAU_WIDGET__
#define __TY_COURBENIVEAU_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYCourbeNiveau;
class QLineEdit;
class QGridLayout;
class QLabel;
class QTreeWidget;
class QGroupBox;
class QPushButton;
class QTreeWidgetItem;
class TYElementWidget;
class TYColorInterfaceWidget;
class QCheckBox;

/**
 * \class TYCourbeNiveauWidget
 * \brief classe d'un objet IHM pour une courbe de niveau
 */
class TYCourbeNiveauWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYCourbeNiveau)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYCourbeNiveauWidget(TYCourbeNiveau* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYCourbeNiveauWidget();


public slots:
    virtual void updateContent();
    virtual void apply();
    virtual void updateUseDefault();
    virtual void setDefaultValue();

    // Membres
protected:

    QGroupBox* _groupBox;
    QTreeWidget* _listViewTabPt;
    QGroupBox* _groupBoxAlt;
    QLabel* _labelAltitude;
    QLineEdit* _lineEditAltitude;

    QGroupBox* _groupBoxDistMax;
    QLabel* _labelDistMax;
    QLineEdit* _lineEditDistMax;
    QGridLayout* _groupBoxDistLayout;

    QPushButton* _pushButtonUseDefault;
    QLabel* _labelUseDefault;

    QGridLayout* _courbeNiveauLayout;
    QGridLayout* _groupBoxLayout;
    QGridLayout* _groupBoxAltLayout;

    TYElementWidget* _elmW;
    TYColorInterfaceWidget* _colorW;

    bool _statusDMax;
};


#endif // __TY_COURBENIVEAU_WIDGET__

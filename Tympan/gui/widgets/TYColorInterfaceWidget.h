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
 * \file TYColorInterfaceWidget.h
 * \brief outil IHM pour l'objet ColorInterface (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TYCOLORINTERFACEWIDGET__
#define __TYCOLORINTERFACEWIDGET__

#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYColorInterface;
class QLabel;
class QGridLayout;
class QGroupBox;
class TYToolButton;

/**
 * \class TYColorInterfaceWidget
 * \brief classe de l'objet IHM pour l'objet ColorInterface
 */
class TYColorInterfaceWidget : public QWidget
{
    Q_OBJECT

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYColorInterfaceWidget(TYColorInterface* pElement, QWidget* _pParent = 0);
    /**
     * Destructeur.
     */
    virtual ~TYColorInterfaceWidget();

    TYColorInterface* getElement() { return _pElement; }

public slots:
    virtual void updateContent();
    virtual void apply();
    virtual void reject();

    void colorAccepted();

signals:
    void colorModified();

    // Membres
protected:
    TYColorInterface* _pElement; //sm++ Correction de la macro TY_DECL_METIER_WIDGET

    QGroupBox* _groupBox;
    QLabel* _labelColor;
    TYToolButton* _pushButtonColor;

    QGridLayout* _groupBoxLayout;
};

#endif // __TYCOLORINTERFACEWIDGET__

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
 * \file TYReseauTransportWidget.h
 * \brief Outil IHM pour un reseau de transport (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_RESEAUTRANSPORT_WIDGET__
#define __TY_RESEAUTRANSPORT_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYReseauTransport;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class TYAcousticLineWidget;

/**
 * \class TYReseauTransportWidget
 * \brief classe de l'objet IHM pour un reseau de transport
 */
class TYReseauTransportWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYReseauTransport)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYReseauTransportWidget(TYReseauTransport* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYReseauTransportWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    // Membres
protected:
    QGroupBox* _groupBox;
    QLineEdit* _lineEditNbBrins;
    QLabel* _labelNbBrins;
    QLabel* _labelTension;
    QLineEdit* _lineEditTension;
    QLabel* _labelPuissance;
    QLineEdit* _lineEditPuissance;
    QLineEdit* _lineEditHautMoy;
    QLabel* _labelHautMoy;

    QGridLayout* _reseauTransportLayout;
    QGridLayout* _groupBoxLayout;

    TYAcousticLineWidget* _elmW;
};


#endif // __TY_RESEAUTRANSPORT_WIDGET__

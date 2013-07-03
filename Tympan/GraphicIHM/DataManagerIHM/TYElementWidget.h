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
 * \file TYElementWidget.h
 * \brief outil IHM pour un element (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ELEMENT_WIDGET__
#define __TY_ELEMENT_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class QLineEdit;
class QLabel;
class QGridLayout;
class QGroupBox;


/**
 * \class TYElementWidget
 * \brief classe de l'objet IHM pour un element
 */
class TYElementWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYElement)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYElementWidget(TYElement* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYElementWidget();

    /**
     * Retourne le nom
     */
    QString getEditName() const;

public slots:
    virtual void updateContent();
    virtual void apply();
    virtual void reject();
    virtual void updateEditName(const QString& name);

    // Membres
protected:

    QGroupBox* _groupBox;
    QLineEdit* _lineEditName;
    QLabel* _labelName;


protected:
    QGridLayout* _groupBoxLayout;
};


#endif // __TY_ELEMENT_WIDGET__

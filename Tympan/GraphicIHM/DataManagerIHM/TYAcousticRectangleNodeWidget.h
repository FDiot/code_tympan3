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
 * \file TYAcousticRectangleNodeWidget.h
 * \brief outil IHM pour un ensemble de rectangles acoustiques (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTICRECTANGLENODE_WIDGET__
#define __TY_ACOUSTICRECTANGLENODE_WIDGET__


#include "TYWidget.h"
#include <qlayout.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticRectangleNode;
class TYAcousticSurfaceNodeWidget;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;

/**
 * \class TYAcousticRectangleNodeWidget
 * \brief Classe de l'objet IHM pour un ensemble de rectangles acoustiques
 */
class TYAcousticRectangleNodeWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYAcousticRectangleNode)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticRectangleNodeWidget(TYAcousticRectangleNode* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticRectangleNodeWidget();

    /**
     * Permet de modifier les parametres du layout.
     *
     * @param margin La valeur de la marge.
     * @param spacing La valeur de l'espacement.
     */
    void setLayoutSpacing(int margin, int spacing)
    {
        _acousticRectangleNodeLayout->setMargin(margin);
        _acousticRectangleNodeLayout->setSpacing(spacing);
    }


public slots:
    virtual void updateContent();
    virtual void apply();

    // Membres
protected:

    TYAcousticSurfaceNodeWidget* _elmW;
    QGridLayout* _acousticRectangleNodeLayout;
};


#endif // __TY_ACOUSTICRECTANGLENODE_WIDGET__

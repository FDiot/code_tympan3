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
 * \file TYAcousticSurfaceNodeWidget.h
 * \brief outil IHM pour un ensemble de surfaces acoustiques (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTICSURFACENODE_WIDGET__
#define __TY_ACOUSTICSURFACENODE_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class TYAcousticSurfaceNode;
class TYAcousticInterfaceWidget;
class TYElementWidget;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class TYSurfaceInterfaceWidget;
class TYColorInterfaceWidget;
class QTableWidget;



/**
 * \class TYAcousticSurfaceNodeWidget
 * \brief classe de l'objet IHM pour ensemble de surfaces acoustiques
 */
class TYAcousticSurfaceNodeWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYAcousticSurfaceNode)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticSurfaceNodeWidget(TYAcousticSurfaceNode* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticSurfaceNodeWidget();


public slots:
    virtual void updateContent();
    virtual void apply();

    /**
     * Edite la widget de l'acoustic surface selectionne.
     *
     * @param item Element de la liste correspondant a l'acoustic surface selectionne.
     */
    void editAccSurf(const int& item);

    /**
     * Supprime l'acoustic surface selectionnee
     * @param item Element de la liste correspondant a l'acoustic surface selectionne
     */
    void delAccSurf(const int&  item);

    /**
     * Edite la widget du bounding rect.
     */
    //  void editRect();

    //  void selectionChanged(int index);

    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuEvent(QContextMenuEvent* e);


    // Membres
protected:

    //  QGroupBox* _groupBoxRect;
    //    QLineEdit* _lineEditNomRect;
    //    QPushButton* _pushButtonRect;
    QGroupBox* _groupBox;

    QTableWidget* _tableFaces;


    QGridLayout* _acousticSurfacenodeLayout;
    //    QGridLayout* _groupBoxRectLayout;
    QGridLayout* _groupBoxLayout;

    TYAcousticInterfaceWidget* _interW;
    TYSurfaceInterfaceWidget* _surfaceW;
    TYColorInterfaceWidget* _colorW;
    TYElementWidget* _elmW;
};


#endif // __TY_ACOUSTICSURFACENODE_WIDGET__

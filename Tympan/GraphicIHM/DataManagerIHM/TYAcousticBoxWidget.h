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
 * \file TYAcousticBoxWidget.h
 * \brief outil IHM pour une boite acoustique (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ACOUSTICBOX_WIDGET__
#define __TY_ACOUSTICBOX_WIDGET__


#include "TYWidget.h"
//Added by qt3to4:
#include <QGridLayout>

class TYAcousticBox;
class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class TYAcousticVolumeWidget;
class QTableWidget;

/**
 * \class TYAcousticBoxWidget
 * \brief classe de l'objet IHM pour une boite acoustique
 */
class TYAcousticBoxWidget : public TYWidget
{
    Q_OBJECT

    TY_DECL_METIER_WIDGET(TYAcousticBox)

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYAcousticBoxWidget(TYAcousticBox* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYAcousticBoxWidget();


public slots:
    virtual void updateContent();
    virtual void apply();
    virtual void reject();

    /**
     * Affiche un menu contextuel.
     */
    virtual void contextMenuEvent(QContextMenuEvent* e);

    /**
     * Edite la widget de la face selectionnee.
     *
     * @param item Element de la liste correspondant a la face selectionnee.
     */
    void editFace(const int& item);

    /**
     * Indique que la couleur a ete modifiee.
     */
    void modifyColor();

    // Membres
protected:

    bool _isColorModified;

    QGroupBox* _groupBox;

    QGridLayout* _acousticBoxLayout;
    QGridLayout* _groupBoxLayout;

    QTableWidget* _tableFaces;

    QGroupBox* _groupBoxDimensions;
    QGridLayout* _groupBoxDimensionsLayout;
    QLabel*     _dimensionsXLabel;
    QLineEdit*  _dimensionsXLineEdit;
    QLabel*     _dimensionsYLabel;
    QLineEdit*  _dimensionsYLineEdit;
    QLabel*     _dimensionsZLabel;
    QLineEdit*  _dimensionsZLineEdit;

    TYAcousticVolumeWidget* _elmW;
};


#endif // __TY_ACOUSTICBOX_WIDGET__

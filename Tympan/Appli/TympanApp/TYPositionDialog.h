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
 * \file TYPositionDialog.h
 * \brief boite de dialogue pour la gestion de la position selon les modes 'moving', 'rotation', 'edition' (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_POSITION_DIALOG__
#define __TY_POSITION_DIALOG__


#include "TYAppDefines.h"
#include <qdialog.h>
#include <qlayout.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class OPoint3D;
class TYGeometryNode;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class TYElementWidget;

/**
 * \class TYPositionDialog
 * \brief boite de dialogue pour la gestion de la position selon les modes 'moving', 'rotation', 'edition'
 */
class TYPositionDialog : public QDialog
{
    Q_OBJECT

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYPositionDialog(TYGeometryNode* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYPositionDialog();

    /**
     * Permet de modifier les parametres du layout.
     *
     * @param margin La valeur de la marge.
     * @param spacing La valeur de l'espacement.
     */
    void setLayoutSpacing(int margin, int spacing)
    {
        _pointLayout->setMargin(margin);
        _pointLayout->setSpacing(spacing);
    }

    /// Get/Set de l'etat de la zone de saisie des hauteurs
    bool getIsHauteurEnabled() { return _bHauteurEnable; }
    const bool getIsHauteurEnabled() const { return _bHauteurEnable; }

    /// Get/Set de l'etat de la zone de saisie des hauteurs
    void setHauteurEnabled(const bool& enable) { _bHauteurEnable = enable; updateContent(); }



public slots:
    virtual void updateContent();
    virtual void apply();

    // Membres
protected:
    QGroupBox* _groupBox;
    QLabel* _labelX;
    QLabel* _labelZ;
    QLabel* _labelY;
    QLabel* _labelHauteur;
    QLineEdit* _lineEditX;
    QLineEdit* _lineEditY;
    QLineEdit* _lineEditZ;
    QLineEdit* _lineEditH;

    bool _bHauteurEnable;
    QGridLayout* _pointLayout;
    QGridLayout* _groupBoxLayout;

    TYGeometryNode* _pElement;
};


#endif // __TY_POSITION_DIALOG__

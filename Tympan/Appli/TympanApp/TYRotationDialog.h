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
 * \file TYRotationDialog.h
 * \brief Boite de dialogue pour la saisie de la l'angle des objets (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ROTATION_DIALOG__
#define __TY_ROTATION_DIALOG__


#include "TYAppDefines.h"
#include <qdialog.h>
#include <qlayout.h>
#include <qcheckbox.h>
//Added by qt3to4:
#include <QGridLayout>
#include <QLabel>

class OPoint3D;
class QLineEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class TYElementWidget;

/**
 * \class TYRotationDialog
 * \brief Boite de dialogue pour la saisie de la l'angle des objets
 */
class TYRotationDialog : public QDialog
{
    Q_OBJECT

    // Methodes
public:
    /**
     * Constructeur.
     */
    TYRotationDialog(OPoint3D* pElement, QWidget* _pParent = NULL);
    /**
     * Destructeur.
     */
    virtual ~TYRotationDialog();

    /**
     * \fn void setLayoutSpacing(int margin, int spacing)
     * \brief Permet de modifier les parametres du layout
     * \param margin La valeur de la marge.
     * \param spacing La valeur de l'espacement.
     */
    void setLayoutSpacing(int margin, int spacing)
    {
        _pointLayout->setMargin(margin);
        _pointLayout->setSpacing(spacing);
    }

    /// Retourne l'etat du checkbox de concatenation des rotations
    bool getConcatenateStatus() { return _pConcatenateCheckBox->isChecked(); }

    /// Empeche les rotations selon X et Y (utile pour les sites ...)
    void lockXY();

public slots:
    virtual void updateContent();
    virtual void apply();
    virtual void switchConcatenate();

    // Membres
protected:
    QGroupBox* _groupBox;
    QLabel* _labelX;
    QLabel* _labelZ;
    QLabel* _labelY;
    QLineEdit* _lineEditX;
    QLineEdit* _lineEditY;
    QLineEdit* _lineEditZ;

    QGridLayout* _pointLayout;
    QGridLayout* _groupBoxLayout;
    QCheckBox* _pConcatenateCheckBox;

    OPoint3D* _pElement;
};


#endif // __TY_ROTATION_DIALOG__

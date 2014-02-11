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
 * \file TYAddLibraryDialog.h
 * \brief Boite de dialogue pour l'ajout d'un element dans la bibliotheque (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_ADD_LIBRARY_DIALOG__
#define __TY_ADD_LIBRARY_DIALOG__


#include "TYAppDefines.h"
#include "Tympan/MetierSolver/DataManagerCore/TYElement.h"

#include <qdialog.h>
//Added by qt3to4:
#include <QLabel>

class QPushButton;
class QLabel;
class QLineEdit;

/**
 * \class TYAddLibraryDialog
 * \brief Boite de dialogue pour l'ajout d'un element dans la bibliotheque.
 */
class TYAddLibraryDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * \fn TYAddLibraryDialog(QWidget * parent = 0, const char * name = 0, Qt::WFlags f = 0)
     * \brief Constructeur par defaut.
     */
    TYAddLibraryDialog(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * \fn virtual ~TYAddLibraryDialog()
     * \brief Destructeur.
     */
    virtual ~TYAddLibraryDialog();

    /**
     * \fn void setElement(LPTYElement element)
     * \brief Acces a l'element
     */
    void setElement(LPTYElement element);

protected slots:
    /// Sauvegarde
    void save();

    /// Active ou non le bouton _pSaveBtn
    void setEnableButton(const QString&);

protected:
    /// Element a sauvegarder
    LPTYElement _element;

    /// La librarie
    TYLibraryWidget* _pLibrary;

    /// Le nom de sauvegarde
    QLineEdit* _lineEditName;
    QLabel* _labelName;

    /// Le bouton de sauvegarde
    QPushButton* _pSaveBtn;
};


#endif // __TY_ADD_LIBRARY_DIALOG__

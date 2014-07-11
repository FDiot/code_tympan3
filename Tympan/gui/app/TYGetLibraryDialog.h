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
 * \file TYGetLibraryDialog.h
 * \brief Boite de dialogue pour la lecture d'un element dans la bibliotheque (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_GET_LIBRARY_DIALOG__
#define __TY_GET_LIBRARY_DIALOG__


#include "TYAppDefines.h"
#include "Tympan/models/business/TYElement.h"

#include <qdialog.h>

class QPushButton;

/**
 * \class TYGetLibraryDialog
 * \brief Boite de dialogue pour la lecture d'un element dans la bibliotheque.
 */
class TYGetLibraryDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYGetLibraryDialog(int filter, QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYGetLibraryDialog();

    /**
     * Accesseur a la collection d'elements
     */
    LPTYElementArray getElements();

protected slots:
    /// Lit la selection
    void openSelected();

    /// Active ou non le bouton _pOpenBtn
    void setEnableButton(const QString&);

protected:
    /// La collection d'element
    LPTYElementArray _elements;

    /// Le bouton open
    QPushButton* _pOpenBtn;

    ///La librarie
    TYLibraryWidget* _pLibrary;
};


#endif // __TY_GET_LIBRARY_DIALOG__

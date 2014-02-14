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
 * \file TYPluginDialog.h
 * \brief Boite de dialogue pour la selection du plugin de calcul (fichier header)
 * \author Projet_Tympan
 *
 *
 *
 *
 *
 */

#ifndef __TY_PLUGIN_DIALOG__
#define __TY_PLUGIN_DIALOG__

#include "TYAppDefines.h"

#include <qdialog.h>

class QTreeWidget;
class QPushButton;

/**
 * \class TYPluginDialog
 * \brief Boite de dialogue pour la selection du plugin de calcul
 */
class TYPluginDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Constructeur par defaut.
     */
    TYPluginDialog(QWidget* parent = 0, const char* name = 0, Qt::WFlags f = 0);
    /**
     * Destructeur.
     */
    virtual ~TYPluginDialog();

protected slots:
    /// Mets a jour le TYSolverInterface courant
    void updateCurrentPlugin();

    /// Active ou non le bouton _pOkBtn
    void setEnableButton();

protected:
    // List view
    QTreeWidget* _pPluginListView;

    // Le bouton d'application
    QPushButton* _pOkBtn;
};

#endif // __TY_PLUGIN_DIALOG__

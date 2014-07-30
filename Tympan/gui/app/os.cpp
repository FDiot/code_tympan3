/*
 * Copyright (C) <2014> <EDF-R&D> <FRANCE>
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
 * \file Tympan/gui/app/os.cpp
 * \brief Utilitaires pour les interactions entre l'interface graphique et le système
 */


#include <QCoreApplication>
#include <qmessagebox.h>

#include "Tympan/models/business/subprocess_util.h"
#include "Tympan/gui/app/TYApplication.h"
#include "os.h"

bool python_gui(QStringList args)
{
    // Disable GUI
    getTYMainWnd()->setEnabled(false);
    TYApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    string error_msg;
    bool computation_ok = python(args, error_msg);
    if (!computation_ok && !error_msg.empty())
    {
        QMessageBox msgBox;
        msgBox.setText(error_msg.c_str());
        msgBox.exec();
    }
    // Enable GUI
    TYApplication::restoreOverrideCursor();
    getTYMainWnd()->setEnabled(true);
    return computation_ok;
}


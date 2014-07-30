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
 * \file Tympan/gui/app/os.h
 * \brief Utilitaires pour les interactions entre l'interface graphique et le système (headers)
 */


#ifndef TY_G_A_OS
#define TY_G_A_OS

#include <qstring.h>
#include <QTemporaryFile>

bool python(QStringList args);
bool must_keep_tmp_files();
bool init_tmp_file(QTemporaryFile& tmp_file, bool keep_file=false);

#endif // TY_G_A_OS

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

/*
 *
 */

#ifndef __TY_CONFIGURE__
#define __TY_CONFIGURE__

#ifdef _MSC_VER
#pragma warning( disable : 4275 4800 4355 4273 4786 4018 4081)
#endif

#pragma warning(disable: 4081)

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

/**
 * Configuration du projet Tympan.
 * Ce fichier permet de preciser quelle API systeme est utilisee ou pas
 * pour l'IHM, le graphique (3D et/ou 2D) et l'impression.
 */

///Nom de la categorie pour la sauvegarde des preferences.
#define TYDIRPREFERENCEMANAGER "UserPreferences"

#endif // __TY_CONFIGURE__

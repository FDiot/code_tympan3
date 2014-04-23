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

/* Initial platform/compiler-related stuff to set.
*/
#define TY_PLATFORM_WIN32 1
#define TY_PLATFORM_WIN64 2
#define TY_PLATFORM_LINUX 3

#define TY_COMPILER_MSVC 1
#define TY_COMPILER_GNUC 2

#define TY_ARCHITECTURE_32 1
#define TY_ARCHITECTURE_64 2

/* Finds the compiler type and version.
*/
#if defined( _MSC_VER )
#   define TY_COMPILER TY_COMPILER_MSVC
#   define TY_COMP_VER _MSC_VER

#elif defined( __GNUC__ )
#   define TY_COMPILER TY_COMPILER_GNUC
#   define TY_COMP_VER (((__GNUC__)*100) + \
                        (__GNUC_MINOR__*10) + \
                        __GNUC_PATCHLEVEL__)

#else
#   pragma error "No known compiler. Abort! Abort!"

#endif

/* Finds the current platform */

#if defined( __WIN32__ ) || defined( _WIN32 )
#   define TY_PLATFORM TY_PLATFORM_WIN32

#elif defined( _WIN64_ ) || defined( _WIN64 )
#   define TY_PLATFORM TY_PLATFORM_WIN64

#else
#   define TY_PLATFORM TY_PLATFORM_LINUX
#endif

/* Find the arch type */
#if defined(__x86_64__) || defined(_M_X64) || defined(__powerpc64__) || defined(__alpha__) || defined(__ia64__) || defined(__s390__) || defined(__s390x__)
#   define TY_ARCH_TYPE TY_ARCHITECTURE_64
#else
#   define TY_ARCH_TYPE TY_ARCHITECTURE_32
#endif

// Integer formats of fixed bit width
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
// define uint64 type
#if TY_COMPILER == TY_COMPILER_MSVC
typedef unsigned __int64 uint64;
#else
typedef unsigned long long uint64;
#endif

/**
 * Configuration du projet Tympan.
 * Ce fichier permet de preciser quelle API systeme est utilisee ou pas
 * pour l'IHM, le graphique (3D et/ou 2D) et l'impression.
 */

///Nom de la categorie pour la sauvegarde des preferences.
#define TYDIRPREFERENCEMANAGER "UserPreferences"

/// Permet l'execution en version "RECHERCHE"
//#define TY_RECHERCHE

#endif // __TY_CONFIGURE__

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
 * \file TYMessageManager.cpp
 * \brief utilitaire pour la gestion des messages dans Tympan
 *
 *
 */




#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanApp.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER

#include "TYMessageManager.h"


#if TY_USE_IHM


#include <stdarg.h>

#include "TYApplication.h"
#include "Tympan/Tools/OLocalizator.h"


#define TR(id) OLocalizator::getString("TYMessageManager", (id))

#ifdef _DEBUG
static bool bDebug = true;
#else
static bool bDebug = false;
#endif

TYMessageManager::TYMessageManager()
{
}

TYMessageManager::~TYMessageManager()
{
}

void TYMessageManager::format(int level, const char* message, ...)
{
    if ((!bDebug) && (level == MSG_DEBUG)) { return; }   // on n'affiche les msg debug qu'en mode debug...

    va_list args;
    char    strLevel[1024];
    char    tmp[1024];
    char    msg[2048];

    switch (level)
    {
        case MSG_DEBUG:
            sprintf(strLevel, "%s", TR("id_level_debug").toAscii().data());
            break;
        case MSG_BENCH:
            sprintf(strLevel, "%s", TR("id_level_bench").toAscii().data());
            break;
        case MSG_INFO:
            sprintf(strLevel, "%s", TR("id_level_info").toAscii().data());
            break;
        case MSG_WARNING:
            sprintf(strLevel, "%s", TR("id_level_warning").toAscii().data());
            break;
        case MSG_ERROR:
            sprintf(strLevel, "%s", TR("id_level_error").toAscii().data());
            break;
        case MSG_FATAL:
            sprintf(strLevel, "%s", TR("id_level_fatal").toAscii().data());
            break;
        default:
            strcpy(strLevel, "");
            break;
    }


    va_start(args, message);

    // Arguments
    vsprintf(tmp, message, args);
    // On construit le msg de la facon suivante :
    // JJ/MM/AAAA - HH:MM:SS <niveau><message><retour a la ligne>
    sprintf(msg, "%s %s%s", getStrDate(), strLevel, tmp);

    va_end(args);

    // Affichage/ecriture du message
    output(msg, level);

}

void TYMessageManager::output(const char* message, int level)
{
    if (level > MSG_DEBUG)   // on n'affiche les msg dans l'ihm que s'ils ne sont pas debug de toute facon...
    {
        writeOutputMsg(message);
        if (qApp->hasPendingEvents())
        {
            qApp->processEvents();
        }
    }
    else
    {
        // pour les msg de debug, on utilise la console...
        OMessageManager::output(message, level);
    }
}


#else // TY_USE_IHM


TYMessageManager::TYMessageManager()
{
}

TYMessageManager::~TYMessageManager()
{
}

void TYMessageManager::format(int level, const char* message, ...)
{
    OMessageManager::format(level, message);
}

void TYMessageManager::output(const char* message, int level)
{
    OMessageManager::output(message, level);
}


#endif // TY_USE_IHM


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

#ifndef __O_MSG__
#define __O_MSG__

#include <time.h>

#define MSG_DEBUG   0x0001
#define MSG_BENCH   0x0002
#define MSG_INFO    0x0004
#define MSG_WARNING 0x0008
#define MSG_ERROR   0x0010
#define MSG_ALL     MSG_DEBUG|MSG_BENCH|MSG_INFO|MSG_WARNING|MSG_ERROR
#define MSG_DEFAUT  MSG_INFO|MSG_WARNING|MSG_ERROR


/**
 * Classe utilitaire pour la gestion des messages.
 */
class OMsg
{
    // Methods
public:
    /**
     * Trace dans un fichier.
     */
    static void msgTrace(const char* message, ...);

    /**
     * Affichage d'un message de type erreur.
     */
    static void msgErr(const char* message, ...);

    /**
     * Affichage d'un message de type warning.
     */
    static void msgWarning(const char* message, ...);

    /**
     * Affichage d'un message de type information.
     */
    static void msgInfo(const char* message, ...);

    /**
     * Affichage d'un message de type debug.
     */
    static void msgDebug(const char* message, ...);

    /**
     * Message d'erreur general pour un fichier absent.
     */
    static void msgErrFicAbsent(const char* nomFic);

    /**
     * Affiche un message informant que la fonctionnalite n'est pas encore implementee.
     */
    static void msgToDo(const char* message);

private:
    /**
     * Fonction generique d'affichage des messages.
     *
     * Remarque :
     * N'a pas de raison d'etre utilisee directement, utiliser plutot
     * MsgErr(), MsgWarning(), ...
     *
     * @param level Niveau du message (MSG_DEBUG, MSG_INFO, MSG_WARNING, ...).
     * @param message Contenu du message (style printf).
     */
    static void msgXxx1(int level, const char* message, ...);

    static void msgXxx(int level, const char* message, ...);

    static int  initTrace();
    static char* getStrDate();
#ifdef _WIN32
    static void checkFile(struct _finddata_t* c_file, time_t theTime);
#else
    static void checkFile(const char* c_file, time_t theTime);
#endif

    // Members
private:
    static FILE*    _ficTrace;
};

#endif // __O_MSG__

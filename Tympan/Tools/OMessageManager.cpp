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




#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef _WIN32
#include <io.h>
#else
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif // _WIN32
#include "OMessageManager.h"


// Initialise a NULL.
LPOMessageManager OMessageManager::_pInstance = 0;

#ifdef _DEBUG
static bool bDebug = true;
#else
static bool bDebug = false;
#endif

OMessageManager::OMessageManager()
{
    _ficTrace = NULL;
}

OMessageManager::~OMessageManager()
{
}

bool OMessageManager::setAsSingleton()
{
    bool ret = _pInstance ? true : false;

    _pInstance = this;

    return ret;
}

OMessageManager* OMessageManager::get()
{
    if (!_pInstance)
    {
        // Si pas d'instance definie comme singleton on en cree une
        _pInstance = new OMessageManager();
    }

    return _pInstance;
}

void OMessageManager::warning(const char* message, ...)
{
    va_list args;
    char    mess[512];

    va_start(args, message);
    vsprintf(mess, message, args);
    format(MSG_WARNING, mess);
    va_end(args);
}

void OMessageManager::error(const char* message, ...)
{
    va_list args;
    char    mess[512];

    va_start(args, message);
    vsprintf(mess, message, args);
    format(MSG_ERROR, mess);
    va_end(args);
}

void OMessageManager::fatal(const char* message, ...)
{
    va_list args;
    char    mess[512];

    va_start(args, message);
    vsprintf(mess, message, args);
    format(MSG_FATAL, mess);
    va_end(args);
}

void OMessageManager::info(const char* message, ...)
{
    va_list args;
    char    mess[512];

    va_start(args, message);
    vsprintf(mess, message, args);
    va_end(args);

    format(MSG_INFO, mess);
}

void OMessageManager::debug(const char* message, ...)
{
    if (bDebug)
    {
        va_list args;
        char    mess[512];

        va_start(args, message);
        vsprintf(mess, message, args);
        format(MSG_DEBUG, mess);
        va_end(args);
    }
}

void OMessageManager::missingFile(const char* nomFic)
{
    error("Fichier %s introuvable", nomFic);
}

void OMessageManager::toDo(const char* message)
{
    printf("%s non implemente.\n", message);
}

void OMessageManager::trace(const char* message, ...)
{
    va_list args;
    va_start(args, message);

    if (initTrace())
    {
        fprintf(_ficTrace, "TRACE %s : ", getStrDate());
        vfprintf(_ficTrace, message, args);
        fprintf(_ficTrace, "\n");
        fflush(_ficTrace);
    }

    va_end(args);
}

void OMessageManager::warning(const QString& message, ...)
{
    va_list args;
    char    mess[512];

#ifdef _WIN32
    // CLM-NT35: Pb en debug avec string cast
    std::string mess_tmp = message.toAscii();
    char* mymessage = (char*)mess_tmp.data();
#else
    char* mymessage =  message.toAscii().data();
#endif

    va_start(args, mymessage);
    vsprintf(mess, mymessage, args);
    format(MSG_WARNING, mess);
    va_end(args);
}

void OMessageManager::error(const QString& message, ...)
{
    va_list args;
    char    mess[512];

#ifdef _WIN32
    // CLM-NT35: Pb en debug avec string cast
    std::string mess_tmp = message.toAscii();
    char* mymessage = (char*)mess_tmp.data();
#else
    char* mymessage =  message.toAscii().data();
#endif

    va_start(args, mymessage);
    vsprintf(mess, mymessage, args);
    format(MSG_ERROR, mess);
    va_end(args);
}

void OMessageManager::fatal(const QString& message, ...)
{
    va_list args;
    char    mess[512];

#ifdef _WIN32
    // CLM-NT35: Pb en debug avec string cast
    std::string mess_tmp = message.toAscii();
    char* mymessage = (char*)mess_tmp.data();
#else
    char* mymessage =  message.toAscii().data();
#endif

    va_start(args, mymessage);
    vsprintf(mess, mymessage, args);
    format(MSG_FATAL, mess);
    va_end(args);
}

void OMessageManager::info(const QString& message, ...)
{
    va_list args;
    char    mess[512];

#ifdef _WIN32
    // CLM-NT35: Pb en debug avec string cast
    std::string mess_tmp = message.toAscii();
    char* mymessage = (char*)mess_tmp.data();
#else
    char* mymessage =  message.toAscii().data();
#endif

    va_start(args, mymessage);
    vsprintf(mess, mymessage, args);
    va_end(args);

    format(MSG_INFO, mess);
}

void OMessageManager::debug(const QString& message, ...)
{
    if (bDebug)
    {
        va_list args;
        char    mess[512];

#ifdef _WIN32
        // CLM-NT35: Pb en debug avec string cast
        std::string mess_tmp = message.toAscii();
        char* mymessage = (char*)mess_tmp.data();
#else
        char* mymessage =  message.toAscii().data();
#endif
        va_start(args, mymessage);
        vsprintf(mess, mymessage, args);
        format(MSG_DEBUG, mess);
        va_end(args);
    }
}

void OMessageManager::missingFile(const QString& nomFic)
{
    error("Fichier %s introuvable", nomFic.toAscii().data());
}

void OMessageManager::toDo(const QString& message)
{
    printf("%s non implemente.\n", message.toAscii().data());
}

void OMessageManager::trace(const QString& message, ...)
{
    va_list args;

#ifdef _WIN32
    // CLM-NT35: Pb en debug avec string cast
    std::string mess_tmp = message.toAscii();
    char* mymessage = (char*)mess_tmp.data();
#else
    char* mymessage =  message.toAscii().data();
#endif

    va_start(args, mymessage);

    if (initTrace())
    {
        fprintf(_ficTrace, "TRACE %s : ", getStrDate());
        vfprintf(_ficTrace, mymessage, args);
        fprintf(_ficTrace, "\n");
        fflush(_ficTrace);
    }

    va_end(args);
}

void OMessageManager::format(int level, const char* message, ...)
{
    if ((!bDebug) && (level == MSG_DEBUG)) { return; }   // on n'affiche les msg debug qu'en mode debug...

    va_list args;
    char    strLevel[64];
    char    tmp[256];
    char    msg[512];

    switch (level)
    {
        case MSG_DEBUG:
            sprintf(strLevel, "%s : ", "Debug");
            break;
        case MSG_BENCH:
            sprintf(strLevel, "%s : ", "Bench");
            break;
        case MSG_INFO:
            sprintf(strLevel, "%s : ", "Info");
            break;
        case MSG_WARNING:
            sprintf(strLevel, "%s : ", "Warning");
            break;
        case MSG_ERROR:
            sprintf(strLevel, "%s : ", "Erreur");
            break;
        case MSG_FATAL:
            sprintf(strLevel, "%s : ", "Erreur Fatale");
            break;
        default:
            strcpy(strLevel, "");
            break;
    }

    va_start(args, message);

    // Arguments
    vsprintf(tmp, message, args);
    // On construit le msg de la facon suivante :
    // JJ/MM/AAAA HH:MM:SS <niveau> <message> <retour a la ligne>
    sprintf(msg, "%s %s %s %s", getStrDate(), strLevel, tmp, "\n");

    va_end(args);

    // Affichage/ecriture du message
    output(msg, level);
}

void OMessageManager::output(const char* message, int level)
{
    if ((!bDebug) && (level == MSG_DEBUG)) { return; }

    FILE* outputStream = 0;

    switch (level)
    {
        case MSG_WARNING:
        case MSG_ERROR:
        case MSG_FATAL:
            outputStream = stderr;
            break;
        case MSG_DEBUG:
        case MSG_BENCH:
        case MSG_INFO:
        default:
            outputStream = stdout;
            break;
    }

    fprintf(outputStream, "%s", message);
}

int OMessageManager::initTrace()
{
    time_t thetime;
    char fileName[100];
    time(&thetime);
#ifdef _WIN32
    intptr_t hFile;
    struct _finddata_t c_file;
    if ((hFile = _findfirst("c:\\Trace*.*", &c_file)) != -1L)      //Attention: 20/02/01 LM : change le nom du fichier mais faudra mettre un truc pour parametrer ce nom
    {
        checkFile(&c_file, thetime);
        while (_findnext(hFile, &c_file) == 0)
        {
            checkFile(&c_file, thetime);
        }
        _findclose(hFile);
    }
#else
    glob_t globbuf;
    if (0 == glob("/Trace*.*", 0, NULL, &globbuf))
    {
        for (int i = 0; i < globbuf.gl_pathc; i++)
        {
            checkFile(globbuf.gl_pathv[i], thetime);
        }
        globfree(&globbuf);
    }

#endif

#ifdef _WIN32
    sprintf(fileName, "c:\\Trace%.3d.txt", gmtime(&thetime)->tm_yday); //Attention: 20/02/01 LM : change le nom du fichier mais faudra mettre un truc pour parametrer ce nom
#else
    sprintf(fileName, "/Trace%.3d.txt", gmtime(&thetime)->tm_yday); //Attention: 20/02/01 LM : change le nom du fichier mais faudra mettre un truc pour parametrer ce nom
#endif
    if (_ficTrace == NULL)
    {
        _ficTrace = fopen(fileName, "at");
        if (_ficTrace != NULL)
        {
            fprintf(_ficTrace, "-------------------------------------------------------------------------\n");
        }
    }

    return (_ficTrace != NULL);
}

char* OMessageManager::getStrDate()
{
    static char sDate[100];
    time_t      Date;

    time(&Date);
    strftime(sDate, sizeof(sDate), "%d/%m/%Y - %H:%M:%S", localtime(&Date));

    return (sDate);
}


#ifdef _WIN32
void OMessageManager::checkFile(struct _finddata_t* c_file, time_t theTime)
{
    if (difftime(theTime, c_file->time_create) > 604800) // Une semaine
    {
        remove(c_file->name);
    }
}
#else
void OMessageManager::checkFile(const char* c_file, time_t theTime)
{
    struct stat buf;

    stat(c_file, &buf);
    if (difftime(theTime, buf.st_ctime) > 604800) // Une semaine
    {
        remove(c_file);
    }
}
#endif

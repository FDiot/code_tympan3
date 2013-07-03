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

#include "OMsg.h"


FILE* OMsg::_ficTrace = NULL;


char* OMsg::getStrDate()
{
    static char sDate[100];
    time_t      Date;

    time(&Date);
    strftime(sDate, sizeof(sDate), "%d/%m/%Y %H:%M:%S", localtime(&Date));

    return (sDate);
}


#ifdef _WIN32
void OMsg::checkFile(struct _finddata_t* c_file, time_t theTime)
{
    if (difftime(theTime, c_file->time_create) > 604800) // Une semaine
    {
        remove(c_file->name);
    }
}
#else
void OMsg::checkFile(const char* c_file, time_t theTime)
{
    struct stat buf;

    stat(c_file, &buf);
    if (difftime(theTime, buf.st_ctime) > 604800) // Une semaine
    {
        remove(c_file);
    }
}
#endif

int OMsg::initTrace()
{
    intptr_t hFile;
    time_t thetime;
    char fileName[100];
    time(&thetime);

#ifdef _WIN32
    struct _finddata_t c_file;

    if ((hFile = _findfirst("Trace*.*", &c_file)) != -1L)      //Attention: 20/02/01 LM : change le nom du fichier mais faudra mettre un truc pour parametrer ce nom
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
    if (0 == glob("Trace*.*", 0, NULL, &globbuf))
    {
        for (int i = 0; i < globbuf.gl_pathc; i++)
        {
            checkFile(globbuf.gl_pathv[i], thetime);
        }
        globfree(&globbuf);
    }

#endif

    sprintf(fileName, "Trace%.3d.txt", gmtime(&thetime)->tm_yday); //Attention: 20/02/01 LM : change le nom du fichier mais faudra mettre un truc pour parametrer ce nom
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

void OMsg::msgTrace(const char* message, ...)
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

void OMsg::msgXxx1(int level, const char* message, ...)
{
    va_list args;
    char    strLevel[256];
    FILE*   fic1;

    switch (level)
    {
        case MSG_DEBUG:
            fic1 = stdout;
            sprintf(strLevel, "%s : ", "Debug");
            break;

        case MSG_BENCH:
            fic1 = stdout;
            sprintf(strLevel, "%s : ", "Bench");
            break;

        case MSG_INFO:
            fic1 = stdout;
            sprintf(strLevel, "%s : ", "Info");
            break;

        case MSG_WARNING:
            fic1 = stderr;
            sprintf(strLevel, "%s : ", "Warning");
            break;

        case MSG_ERROR:
            fic1 = stderr;
            sprintf(strLevel, "%s : ", "Erreur");
            break;

        default:
            fic1 = stdout;
            strcpy(strLevel, "");
            break;
    }

    if (fic1)
    {
        va_start(args, message);

        fprintf(fic1, "%s %s", getStrDate(), strLevel);
        vfprintf(fic1, message, args);
        fprintf(fic1, "\n");
        fflush(fic1);

        if ((level == MSG_WARNING) || (level == MSG_ERROR))
            if (initTrace())
            {

                fprintf(_ficTrace, "TRACE %s %s ", getStrDate(), strLevel);
                vfprintf(_ficTrace, message, args);
                fprintf(_ficTrace, "\n");
                fflush(_ficTrace);
            }

        va_end(args);
    }
}

void OMsg::msgXxx(int level, const char* message, ...)
{
    va_list args;
    char    mess[512];

    va_start(args, message);
    vsprintf(mess, message, args);
    msgXxx1(level, mess);
}

void OMsg::msgErr(const char* message, ...)
{
    va_list args;
    char    mess[512];

    va_start(args, message);
    vsprintf(mess, message, args);
    msgXxx(MSG_ERROR, mess);
    va_end(args);
}

void OMsg::msgWarning(const char* message, ...)
{
    va_list args;
    char    mess[512];

    va_start(args, message);
    vsprintf(mess, message, args);
    msgXxx(MSG_WARNING, mess);
    va_end(args);
}

void OMsg::msgInfo(const char* message, ...)
{
    va_list args;
    char    mess[512];

    va_start(args, message);
    vsprintf(mess, message, args);
    msgXxx(MSG_INFO, mess);
    va_end(args);
}

void OMsg::msgDebug(const char* message, ...)
{
#if _DEBUG
    va_list args;
    char    mess[512];

    va_start(args, message);
    vsprintf(mess, message, args);
    msgXxx(MSG_DEBUG, mess);
    va_end(args);
#endif //_DEBUG
}

void OMsg::msgErrFicAbsent(const char* nomFic)
{
    msgErr("Fichier %s introuvable", nomFic);
}

void OMsg::msgToDo(const char* message)
{
    printf("%s non implemente.\n", message);
}

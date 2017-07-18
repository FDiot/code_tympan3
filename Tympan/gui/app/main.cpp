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
 * \file main.cpp
 * \brief fichier principal de lancement de l'application Tympan en mode IHM
 */



#include <qmessagebox.h>

#include "Tympan/core/logging.h"
#include "Tympan/models/business/init_registry.h"
#include "TYApplication.h"

static QtMsgHandler old_handler;

static void QTMessageHandler(QtMsgType type, const char* message)
{
    if (old_handler != NULL) { old_handler(type, message); }

    switch (type)
    {
        case QtDebugMsg:
            break;
        case QtWarningMsg:
            break;
        case QtCriticalMsg:
        case QtFatalMsg:
            int selected;

            selected = QMessageBox::critical(NULL, "Attention !", message, "Debug", "Continue", "Quit");

            if (selected == 0)
            {
                // Debug
#if defined(WIN32)
                _CrtDbgBreak();
#endif
            }
            else if (selected == 1)
            {
                // Continue
            }
            else if (selected == 2)
            {
                // Quit
                exit(1);
            }
            break;
    }
}


static int tyMain(int argc, char** argv)
{
    bool success = false;

    old_handler = qInstallMsgHandler(QTMessageHandler);

    TYApplication tyApp(argc, argv);
    if (tyApp.init())
    {
        // Lance la boucle principal
        success = tyApp.run();

        // Termine correctement l'appli
        tyApp.close();
    }
    OMessageManager::get()->debug("Counters : TYElements created %u, deleted %u and ID generated %u",
                                  TYElement::getConstructorCount(), TYElement::getDestructorCount(), TYElement::getIdGenerationCount());
    qInstallMsgHandler(old_handler);

    // return code
    int ret;
    success ? ret = 0 : ret = 1;

    return ret;
}

int main(int argc, char** argv)
{
    int ret;
    // Register TY* classes before starting the application
    tympan::init_registry();
    // Appel le main de Tympan
    ret = tyMain(argc, argv);
    return ret;
}




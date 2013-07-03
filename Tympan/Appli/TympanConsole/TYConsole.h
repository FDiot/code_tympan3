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
 *
 *
 *
 */

#ifndef __TY_CONSOLE__
#define __TY_CONSOLE__

#include <QString>
#include "Tympan/Tools/OGenID.h"
#include "Tympan/MetierSolver/DataManagerCore/TYElementCollection.h"

/**
 * Classe pour la gestion des arguments passe en mode console
 *
 * @author Projet_Tympan
 * @author Projet_Tympan
 *
 */
class TYConsole
{
public:
    /**
     * Constructeur par default
     */
    TYConsole(QString dirPath);

    /**
     * Destructeur
     */
    ~TYConsole();

    /**
     * Lancement de l'application en mode console
     */
    void run(int argc, char** argv);

private:
    /**
     * Structure definissant une option
     */
    typedef struct
    {
        const char* name;
        const char* help;
        const char* argname;
        const char* arg;
        unsigned int number;
    } TYOptionDef;

    /**
     * Permet de parser les arguments
     */
    bool parseOptions(int argc, char** argv);

    /**
     * Fonction permettant de retrouver l'option correspondante au nom
     */
    const TYOptionDef* findOption(const char* name);

    /**
     * Fonction permettant de traiter l'option correspondant au numero
     */
    void processOption(const TYOptionDef* option);

    // Option setting function
    void optShowHelp();
    void optInputFile(const char* filename);
    void optOutputFile(const char* filename);
    void optSolverName(const char* name);
    void optResultFile(const char* filename);

    // Tableau d'options
    static const TYOptionDef _options[];

    // Input file
    QString _input;

    // Output file
    QString _output;

    // Result output file
    QString _result_output;

    // Solver name
    QString _solverName;

    //dir path
    QString _dirPath;

    // La collection d'elements dans la liste.
    TYElementCollection _elements;
};

#endif // __TY_CONSOLE__

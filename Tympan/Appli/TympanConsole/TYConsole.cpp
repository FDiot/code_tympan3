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


#ifdef TYMPAN_USE_PRECOMPILED_HEADER
#include "TYPHTympanConsole.h"
#endif // TYMPAN_USE_PRECOMPILED_HEADER
#include "Tympan/Tools/OChrono.h"
#include "Tympan/Tools/OLocalizator.h"
#include "TYConsole.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPluginManager.h"
#include "Tympan/Tools/TYProgressManager.h"
#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"
#include <iomanip>

#define TR(id) OLocalizator::getString("TYConsole", (id))

/*static*/ const TYConsole::TYOptionDef TYConsole::_options[] =
{
    { "h", "id_console_help_help", 0, 0, 0 },
    { "-help", "id_console_help_help", 0, 0, 0 },
    { "i", "id_console_input_help", "filename", 0, 1 },
    { "-input", "id_console_input_help", "filename", 0, 1 },
    { "o", "id_console_output_help", "filename", 0, 2 },
    { "-output", "id_console_output_help", "filename", 0, 2 },
    { "s", "id_console_solver_help", "name", 0, 3 },
    { "-solver", "id_console_solver_help", "name", 0, 3 },
    { "r", "id_console_output_help", "csv_filename", 0, 4 },
    { "-result", "id_console_output_help", "csv_filename", 0, 4 },
    { NULL, }
};

TYConsole::TYConsole(QString dirPath)
    : _input(""),
      _output(""),
      _solverName("DefaultSolver"),
      _dirPath(dirPath),
      _result_output("")
{
}

TYConsole::~TYConsole()
{

}

void TYConsole::run(int argc, char** argv)
{
    // Parse option
    if (!parseOptions(argc, argv))
    {
        OMessageManager::get()->info(TR("id_arg_error"));
        optShowHelp();
        return;
    }

    // Create plugin manager
    TYPluginManager* pluginManager = new TYPluginManager;
    pluginManager->setAsSingleton();

    QString pluginsPath;
#ifndef _DEBUG
    pluginsPath = _dirPath + "/pluginsConsole/";
#else
    pluginsPath = _dirPath + "/pluginsConsoled/";
#endif

    if (!pluginManager->loadPlugins(pluginsPath))
    {
        OMessageManager::get()->error(TR("id_plugins_error"));
        exit(1);
    }

    // Test plugin to use
    if (!_solverName.isEmpty())
    {
        if (!pluginManager->exist(_solverName))
        {
            OMessageManager::get()->error(TR("id_plugin_error"));
            exit(1);
        }
    }

    // Check input file
    if (_input.isEmpty())
    {
        OMessageManager::get()->error(TR("id_input_error"));
        exit(1);
    }

    // Check output file
    if (_output.isEmpty())
    {
        _output = _input;
    }

    if (_result_output.isEmpty())
    {
        _result_output = _output;
        _result_output.replace(".xml", "");
        _result_output += "_" + _solverName;
    }

    // Create XMLManager
    TYXMLManager xmlManager;

    // Load input file
    int isLoaded = xmlManager.load(_input, _elements);
    if (isLoaded != 1)
    {
        OMessageManager::get()->error(TR("id_open_error") + " " + _input);
        exit(1);
    }

    // Retrieve project
    unsigned int i;
    LPTYProjet project = 0;
    for (i = 0; i < _elements.size(); ++i)
    {
        LPTYElement elt = _elements[i];
        if (strcmp(elt->getClassName(), "TYProjet") == 0)
        {
            project = static_cast<TYProjet*>(elt.getRealPointer());
            break;
        }
    }

    // Check if project exists
    if (!project)
    {
        OMessageManager::get()->error(TR("id_project_error"));
        exit(1);
    }

    //update site node
    project->getSite()->update();

    // Retrieve calcul
    const TYTabLPCalcul calculs = project->getListCalcul();

    // Check if it exist at least one calcul
    if (calculs.empty())
    {
        OMessageManager::get()->error(TR("id_calcul_error"));
        exit(1);
    }

    // Calcul
    for (i = 0; i < calculs.size(); ++i)
    {
        std::cout << std::endl;

        OMessageManager::get()->info(TR("id_go_calcul"));

        LPTYCalcul calcul = calculs[i];
        project->setCurrentCalcul(calcul);
        pluginManager->setCurrent(_solverName);

        // Calcul run
        OChronoTime startTime;
        bool ret = calcul->go();
        OChronoTime endTime;
        OChronoTime duration = endTime - startTime;
        unsigned long second = duration.getTime() / 1000;
        unsigned long millisecond = duration.getTime() - second * 1000;
        QString msg = QString("Temps de calcul : %1,%2 sec.  (%3 msec.)").arg(second).arg(millisecond).arg(duration.getTime());
        OMessageManager::get()->info(msg);

        if (ret)
        {
            TYXMLManager xmlManagerOutput;

            xmlManagerOutput.createDoc(TY_PRODUCT_XMLTAG_, TY_PRODUCT_VERSION_);
            xmlManagerOutput.addElement(project);

            xmlManagerOutput.save(_output);

            // CLM-NT35 : Sauvegarde du resultat pour Excel
            LPTYResultat resultat = calcul->getResultat();
            int aff = 0;
            QString result_fn = "";
            result_fn = _result_output + "_" + QString::number(i) + ".csv";
            std::string res_filename = result_fn.toAscii();
            resultat->saveValue(res_filename, aff);

            // Calcul done
            OMessageManager::get()->info(TR("id_calcul_done"));
        }
        else
        {
            // Calcul failed
            OMessageManager::get()->info(TR("id_calcul_failed"));
        }
    }

    exit(0);
}

bool TYConsole::parseOptions(int argc, char** argv)
{
    if (argc == 1)
    {
        return false;
    }

    const char* arg;
    TYOptionDef option;
    bool handleoptions = true;

    int index = 1;
    while (index < argc)
    {
        arg = argv[index++];

        if (handleoptions && arg[0] == '-' && arg[1] != '\0')
        {
            if (arg[1] == '-' && arg[2] == '\0')
            {
                handleoptions = false;
                continue;
            }

            option = *findOption(arg + 1);
            if (!option.name)
            {
                OMessageManager::get()->error(TR("id_option_error") + " '" + QString(arg) + "'");
                exit(1);
            }

            arg = 0;
            if (option.argname != 0)
            {
                arg = argv[index++];
                if (!arg || arg[0] == '-')
                {
                    OMessageManager::get()->error(TR("id_arg_error") + " '" + QString(option.argname) + "'");
                    exit(1);
                }
                option.arg = arg;
            }

            processOption(&option);
        }
    }

    return true;
}

const TYConsole::TYOptionDef* TYConsole::findOption(const char* name)
{
    const TYOptionDef* po = _options;
    while (po->name != NULL)
    {
        if (!strcmp(name, po->name))
        {
            break;
        }
        po++;
    }
    return po;
}

void TYConsole::processOption(const TYOptionDef* option)
{
    switch (option->number)
    {
        case 0:
            optShowHelp();
            exit(0);
            break;
        case 1:
            optInputFile(option->arg);
            break;
        case 2:
            optOutputFile(option->arg);
            break;
        case 3:
            optSolverName(option->arg);
            break;
        case 4:
            optResultFile(option->arg);
            break;
        default:
            break;
    }
}

void TYConsole::optShowHelp()
{
    OMessageManager::get()->info(TR("id_console_help0"));
    OMessageManager::get()->info(TR("id_console_help1"));
    OMessageManager::get()->info(TR("id_console_help2"));
    OMessageManager::get()->info(TR("id_console_help3"));
    OMessageManager::get()->info(TR("id_console_help4"));
    OMessageManager::get()->info(TR("id_console_help5"));
    OMessageManager::get()->info(TR("id_console_help6"));
}

void TYConsole::optInputFile(const char* filename)
{
    _input = filename;
}

void TYConsole::optOutputFile(const char* filename)
{
    _output = filename;
}

void TYConsole::optSolverName(const char* name)
{
    _solverName = name;
}

void TYConsole::optResultFile(const char* filename)
{
    _result_output = filename;
}

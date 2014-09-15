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
 * \file Tympan/models/business/subprocess_util.cpp
 * \brief Utilitaires pour les interactions entre l'application tympan et des
 * sous-processus python
 */


#include <QFile>
#include <QCoreApplication>
#include <QProcess>
#include <QDir>

#include "Tympan/core/defines.h"
#include "Tympan/core/chrono.h"
#include "Tympan/core/logging.h"
#include "Tympan/core/exceptions.h"
#include "subprocess_util.h"

#if defined _DEBUG
  #define TYMPAN_REL_CYTHON_PATH "cython_d"
#else // undefined _DEBUG (release install)
  #define TYMPAN_REL_CYTHON_PATH "cython"
#endif // defined _DEBUG

#define COMPUTATION_TIMEOUT 10000 // In ms


QStringList _python_qprocess_environment()
{
    QStringList env(QProcess::systemEnvironment());
    // Absolute path to Tympan install directory
    QString cythonlibs_path(QCoreApplication::applicationDirPath());
    // Relative path from Tympan install directory to cython modules
    cythonlibs_path.append("/");
    cythonlibs_path.append(TYMPAN_REL_CYTHON_PATH);
    cythonlibs_path =QDir::toNativeSeparators(cythonlibs_path);
    // Set new PYTHONPATH
    QRegExp pythonpath_regexp("^PYTHONPATH=(.*)", Qt::CaseInsensitive);
    int pythonpath_index = env.indexOf(pythonpath_regexp);
    QString pythonpath;
    if (pythonpath_index > 0)
    {
        pythonpath = env[pythonpath_index];
        // Check the presence of cython libs in the PYTHONPATH
        if (pythonpath != "PYTHONPATH=")
        {
#if TY_PLATFORM == TY_PLATFORM_WIN32 || TY_PLATFORM == TY_PLATFORM_WIN64
            pythonpath.append(";");
#else
            pythonpath.append(":");
#endif
        }
        pythonpath.append(cythonlibs_path);
        env.removeAt(pythonpath_index);
    }
    else
    {
        pythonpath = "PYTHONPATH=";
        pythonpath.append(cythonlibs_path);
    }
    env.append(pythonpath);
#if TY_PLATFORM == TY_PLATFORM_WIN32 || TY_PLATFORM == TY_PLATFORM_WIN64
    // Add path to dynamic libraries needed by Tympan and thus needed by pytam (cython library)
    QRegExp path_regexp("^Path=(.*)", Qt::CaseInsensitive);
    int path_index = env.indexOf(path_regexp);
    QString path = env[path_index];
    path.append(";");
    path.append(QCoreApplication::applicationDirPath());
    env.removeAt(path_index);
    env.append(path);
#endif
    return env;
}


QString _get_python_interp()
{
#if TY_PLATFORM == TY_PLATFORM_WIN32 || TY_PLATFORM == TY_PLATFORM_WIN64
    QStringList env(QProcess::systemEnvironment());
    // TYMPAN_PYTHON_INTERP environment variable must be set to the path to
    // python interpreter (ex: "C:\Python27\python.exe")
    int python_interp_idx = env.indexOf(QRegExp("^TYMPAN_PYTHON_INTERP=(.*)"));
    if (python_interp_idx < 0)
    {
        throw tympan::invalid_data(
                "Can't access python interpreter. TYMPAN_PYTHON_INTERP environment variable is not set.");
    }
    QString python_interp_path = env.at(python_interp_idx).split('=')[1].remove("\"");
    QFile python_interp (python_interp_path);
    if (!python_interp.exists())
    {
        throw tympan::invalid_data("Can't access python interpreter. TYMPAN_PYTHON_INTERP environment variable is not correctly set.");
    }
    return python_interp_path;
#else
    return QString("python");
#endif
}

bool must_keep_tmp_files()
{
    QStringList appli_env(QProcess::systemEnvironment());
    int tympan_debug_idx = appli_env.indexOf(QRegExp("^TYMPAN_DEBUG=(.*)"));
    if (tympan_debug_idx >= 0)
    {
        QString debug_option = appli_env[tympan_debug_idx].split('=')[1];
        if (debug_option.contains("keep_tmp_files", Qt::CaseInsensitive))
        {
            return true;
        }
    }
    return false;
}

bool init_tmp_file(QTemporaryFile& tmp_file, bool keep_file)
{
    if (!tmp_file.open())
    {
        return false;
    }
    tmp_file.close();
    // Prevent from automatic file removal
    if (keep_file)
        tmp_file.setAutoRemove(false);
    return true;
}

bool python(QStringList args, std::string& error_msg)
{
    OMessageManager& logger =  *OMessageManager::get();
    // Start chrono
    OChronoTime startTime;
    logger.debug("Lancement du script python...");
    QProcess python;
    float comp_duration (0.);
    bool comp_finished (false);
    // Set PYTHONPATH to python subprocess
    QStringList env(_python_qprocess_environment());
    python.setEnvironment(env);
    // Since scripts passed to QProcess are not launched through cmd.exe
    // under windows, we have to give QProcess the path to the python interpreter
    QString python_interp;
    try
    {
        python_interp = _get_python_interp();
    }
    catch(const tympan::invalid_data& exc)
    {
        error_msg = "L'interpreteur python n'a pas pu etre trouve.\nVeuillez verifier que la variable d'environnement TYMPAN_PYTHON_INTERP est correctement positionnee\n";
        return false;
    }
    python.start(python_interp, args);
    do
    {
        comp_finished = python.waitForFinished(COMPUTATION_TIMEOUT);
        if (comp_finished)
        {
            break;
        }
        if (python.error() == QProcess::Timedout)
        {
            // Computation still running
            comp_duration += (COMPUTATION_TIMEOUT/1000);
            logger.info("Le script python s'execute encore apres %.3f secondes", comp_duration);
            continue;
        }
        else
        {
            // Will have a bad exit status and will be handled below
            break;
        }
    }
    while(!comp_finished);

    QString err_output (python.readAllStandardError());
    if(!err_output.isEmpty())
    {
        error_msg = err_output.toStdString();
        // If there is an error among the possible warnings
        if (err_output.contains("Error"))
        {
            error_msg.append("\nVeuillez lire tympan.log pour plus d'information.");
            return false;
        }
        else
        {
            logger.warning(error_msg.c_str());
        }
    }

    int pystatus = python.exitStatus();
    if (pystatus == 1)
    {
        error_msg = "Le sous-process python a termine avec le code d'erreur ";
        error_msg.append(std::to_string(static_cast<unsigned long long>(python.error())));
        return false;
    }
    // Compute and display computation time
    OChronoTime endTime;
    OChronoTime duration = endTime - startTime;
    unsigned long second  = duration.getTime() / 1000;
    unsigned long millisecond = duration.getTime() - second * 1000;
    logger.info("Temps de calcul : %02ld,%03ld sec.  (%ld msec.)", second,
            millisecond, duration.getTime());
    return true;
}

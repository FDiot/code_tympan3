#include "EnvironmentUtils.hpp"

#include <QFile>
#include <QCoreApplication>
#include <QProcess>
#include <QDir>

#include "Tympan/MetierSolver/CommonTools/Defines.h"
#include "Tympan/MetierSolver/CommonTools/exceptions.hpp"

namespace tympan
{

QString get_python_interp()
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

QStringList python_qprocess_environment()
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

}


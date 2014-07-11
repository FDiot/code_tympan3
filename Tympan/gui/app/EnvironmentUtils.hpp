#ifndef __ENVIRONMENT_UTILS_HPP
#define __ENVIRONMENT_UTILS_HPP

#include <QString>
#include <QStringList>

//! Relative path to the cython modules depending on the build/install type (debug/release)
#if defined _DEBUG
#define TYMPAN_REL_CYTHON_PATH "cython_d"
#else // defined _DEBUG (release install)
#define TYMPAN_REL_CYTHON_PATH "cython"
#endif // defined _DEBUG 


namespace tympan
{
/*!
 * \brief Retrieve the absolute path to the python interpreter (used under Windows)
 *
 * Retrieves the value of TYMPAN_PYTHON_INTERP environment variable.
 * This environment variable contains the absolute path to the python interpreter.
 *
 * If it is not defined or not correctly defined (python.exe doesn't exist at
 * the path set) it throws a tympan::invalid_data exception.
 */
QString get_python_interp();

/**
 * \brief Builds and returns an environment containing the PYTHONPATH set to the
 *          directory containing cython modules.
 *
 * Under Windows, also adds the path to Tympan install directory to the PATH
 * environment variable.
 *
 */
QStringList python_qprocess_environment();
}

#endif // __ENVIRONMENT_UTILS_HPP

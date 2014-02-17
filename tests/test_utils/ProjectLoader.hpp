/**
 * @file ProjectLoader.hpp
 *
 * @brief Utilities to load and access a project for testing.
 *
 */

#ifndef TYMPAN__PROJECTLOADER_HPP__INCLUDED
#define TYMPAN__PROJECTLOADER_HPP__INCLUDED

#include <cstring>
#include <QString>

#include "gtest/gtest.h"

#include "Tympan/MetierSolver/DataManagerCore/TYElementCollection.h"
#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"


namespace tympan
{

/**
 * @brief load an XML project file
 * @param filename the filename of the project to be open.
 * @param project Output argument : this pointer (expected to be NULL) will be
 *                make to refer to the loaded \c TYProjet.
 */
void load_project_from_file(const char* filename, LPTYProjet& project);

/**
 * @brief Assert some assumptions on a loaded project (like no subsites...)
 * @param project the project to be asserted.
 */
void assert_loaded_project(const LPTYProjet& project);

} /* namespace tympan */
#endif /* TYMPAN__PROJECTLOADER_HPP__INCLUDED */

/**
 * @file xml_project_util.hpp
 *
 * @brief Utilities to load a project and a solver.
 *
 */

#ifndef TYMPAN__XML_PROJECT_UTIL_HPP__INCLUDED
#define TYMPAN__XML_PROJECT_UTIL_HPP__INCLUDED

#include "Tympan/models/business/TYProjet.h"

namespace tympan
{
    /**
     * @brief load an XML project file
     * @param filename the file containing the project to load
     * @param project Output argument : this pointer (expected to be NULL) will be
     *                made to refer to the loaded \c TYProjet.
     * @return true if the project could be loader, false otherwise
     */
    LPTYProjet load_project(const char * filename);

    /**
     * @brief save a project into an XML file
     * @param filename the file where to save the project
     * @param project : pointer to the project to save
     * @return true if the project could be saved, false otherwise
     */
    void save_project(const char * filename, const LPTYProjet& project);

} /* namespace tympan */


#endif /* TYMPAN__XML_PROJECT_UTIL_HPP__INCLUDED */

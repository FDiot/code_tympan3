/**
 * @file Loader.hpp
 *
 * @brief Utilities to load a project and a solver.
 *
 */

#ifndef TYMPAN__LOADER_HPP__INCLUDED
#define TYMPAN__LOADER_HPP__INCLUDED

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"

namespace tympan
{

    void load_project_from_file(const char *, LPTYProjet&);

    void save_project(const char *, const LPTYProjet&);

    void load_solver(const char *, TYCalcul *);

} /* namespace tympan */


#endif /* TYMPAN__LOADER_HPP__INCLUDED */

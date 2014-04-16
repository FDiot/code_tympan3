/*! \file ComputationLauncher.hpp

  \brief Class and utilities which manage and launch computations without GUI.

  We would like to have something which can:

   - load a projet and get a TYCalcul ;
   - change & store different TYCalcul ;
   - change the solver ;
   - launch a computation ;
   - get and export results of receptors ;
   - change the regime of different elements: sources, building, roads, etc.
 */

#ifndef TYMPAN__COMPUTATIONLAUNCHER_HPP__INCLUDED
#define TYMPAN__COMPUTATIONLAUNCHER_HPP__INCLUDED

#include <cstring>
#include <QString>

#include "gtest/gtest.h"

#include "ProjectLoader.hpp"

#include "Tympan/MetierSolver/DataManagerCore/TYXMLManager.h"
#include "Tympan/MetierSolver/DataManagerCore/TYPluginManager.h"

#include "Tympan/MetierSolver/DataManagerMetier/Commun/TYProjet.h"
#include "Tympan/MetierSolver/DataManagerMetier/Site/TYSiteNode.h"

#define DEFAULT_SOLVER_UUID "{A98B320C-44C4-47a9-B689-1DD352DAA8B2}"
#define SIMPLERAY_SOLVER_UUID "{EDA31C0D-5001-40e2-B36E-C29B66950197}"

namespace tympan
{

class ComputationLauncher
{

public:
    //! Constructor.
    /*! By default, the current computation is the first one.
     \param filename The XML project filename to load.
     */
    ComputationLauncher(const char* filename);

    ///////////////
    //  Getters  //
    ///////////////

    //! Get the pointer to the current project.
    LPTYProjet getProject() const { return _project; }

    //! Get the list of available computations (i.e. \c TYCalcul).
    TYTabLPCalcul getComputationList() const { return _project->getListCalcul(); }

    //! Get the number of available computations in the current project.
    /*!
     \return The number of computations.
     */
    size_t getComputationNumber() { return getComputationList().size(); }

    //! Get current computation.
    /*! Get current computation. First one in the computation list by default.
      \return A pointer to the current \c TYCalcul.
     */
    LPTYCalcul getCurrentComputation() const
    {
        // First one in the computation list by default.
        return _current_computation;
    }

    //! Launch the computation with the DefaultSolver.
    /*!
      \param directory Directory where there is the plugin related to the
       Default Solver.
     */
    void launchDefault(const char* directory) const;

    //! Launch the computation with the SimpleRaySolver.
    /*!
      \param directory Directory where there is the plugin related to the
       SimpleRay solver.
     */
    void launchSimpleRay(const char* directory) const;

    ///////////////
    //  Setters  //
    ///////////////

    //! Set the current computation according to its index.
    void setCurrentComputation(size_t index);

private:
    //////////////////
    //  Attributes  //
    //////////////////

    //! XML project file.
    std::string _filename;
    //! Pointer to a Tympan project.
    LPTYProjet _project;
    //! Pointer to the current computation (i.e. \c TYCalcul).
    LPTYCalcul _current_computation;
};


} /* namespace tympan */


#endif /* TYMPAN__COMPUTATIONLAUNCHER_HPP__INCLUDED */

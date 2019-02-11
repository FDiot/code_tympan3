/**
 * @file testutils.h
 *
 * @brief Misc utilities for testing.
 */

#ifndef TY_TESTUTILS
#define TY_TESTUTILS

#include<iostream>
#include<deque>
#include<string>

/// @brief Reads a CVS as a table of trimmed strings
std::deque<std::deque<std::string> > readCsvAsStringTable(std::istream& in);

/// @brief Reads a CVS as a table of \c T assuming that T can be read from a string
template<class T>
std::deque<std::deque<T> > readCsvAsTableOf(std::istream& in);


/* ********** Implementation of templates starts here ********** */

#include<boost/lexical_cast.hpp>
#include<boost/tokenizer.hpp>
#include<boost/algorithm/string/trim.hpp>
#include<boost/foreach.hpp>

template<class T>
std::deque<std::deque<T> > readCsvAsTableOf(std::istream& in)
{
    using namespace std;
    using namespace boost;

    typedef tokenizer< escaped_list_separator<char> > Tokenizer;
    deque<deque<T> > table;
    string line;
    while (getline(in, line))
    {
        deque<T> line_array;
        Tokenizer tok(line);
        BOOST_FOREACH(string str_rec, tok)
        {
            trim(str_rec);
            line_array.push_back(boost::lexical_cast<T>(str_rec));
        }
        table.push_back(line_array);
    }
    return table;
}


/* ********** loads a project from an XML project file ********** */

#include <cstring>
#include <QString>

#include "gtest/gtest.h"

#include "Tympan/models/business/xml_project_util.h"
#include "Tympan/models/business/TYProjet.h"
#include "Tympan/models/business/infrastructure/TYSiteNode.h"


class BuildingFromSiteFixture: public ::testing::Test
{

public:
    // static void SetUpTestCase()

    // Load the project file `filename` into the `project` member
    virtual void load_file(const char filename[])
    {
        using namespace tympan;
        project = load_project(filename); // This output to `project`
        LPTYSiteNode site = project->getSite();
        ASSERT_TRUE(site) << "Invalid root site";
    }

    // virtual void SetUp() {}

    LPTYProjet project;
};

#include "Tympan/models/solver/entities.hpp"
#include "Tympan/solvers/DefaultSolver/TYSolver.h"
#include "Tympan/models/solver/acoustic_problem_model.hpp"
#include "Tympan/models/solver/data_model_common.hpp"
#include "Tympan/solvers/DefaultSolver/TYAcousticModel.h"
#include <iostream>

class TYInitUserCase
{
public:


    bool m_isInitGlobal;
    double m_spectrPrecision;

    //constantes globales
    double m_gamma;
    double m_perfectGaz;
    double m_molareMass;
    double m_temperature;
    double m_absoluteZero;
    double m_pression;
    double m_hydromzetry;
    double m_distance;
    double m_tk, m_soundSpeed;
    OSpectre m_spectreLambda;

    //objets chemins
    TYChemin m_chemin_direct;
    TYChemin m_chemin_sol;
    TYChemin m_chemin_ecran;
    TYChemin m_chemin_relex;

    //tableau d'�tapes
    TYTabEtape m_tabEtape01;

    //spectre source
    OSpectreComplex m_spectre_source;

    //tableau d'intersection
    std::deque<TYSIntersection> m_tabIntersect;

    //directivit�
    OVector3D m_directivityVector;
    tympan::SourceDirectivityInterface* m_directivity;

    //points source et reception
    OPoint3D m_pointSource;
    OPoint3D m_pintReception;
    OSegment3D rayonSR;
    std::vector<tympan::AcousticSource> m_acousticSourceList;
    std::vector<tympan::AcousticReceptor> m_acousticReceptionList;

    TabPoint3D m_tabPoints;

public:
    void initGlobal(double p_distance);

    OSpectre initDirectPathWay(bool p_isFlate=false);

    OSpectre initGroundPathWay();

    OSpectre initBlockPathWay();

    OSpectre initReflexionPathWay();
};

#endif /* TY_TESTUTILS */

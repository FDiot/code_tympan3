//! \test test_bdtopo_import.cpp
/*! \note Test BDTOPO DXF files import. Have to test some protected/private
    methods from \c TYDXFImporter and \c TYDXFBDTopoImporter. Need to write \c
    FRIEND_TEST class from GTest in \file Tympan/TympanApp/Appli/TYDXFImporter.h

    \sa http://code.google.com/p/googletest/wiki/AdvancedGuide#Private_Class_Members
 */

#include <iostream>

#include "gtest/gtest.h"

#include <dime/Model.h>

#include "Tympan/Appli/TympanApp/TYDXFImporter.h"
#include "Tympan/Appli/TympanApp/TYDXFBDTOPOImporter.h"

#include "test_utils/ProjectLoader.hpp"

using std::cout;
using std::endl;


// Just test the 'importIsoline'.  Take into account 'ALT*.dxf' & 'ORO*.dxf'
// files with contents called 'CUVETTE', 'COURBE' & 'LIG_DES'.
TEST(TestBDTOPOImport, topography_import) {

// DXF files to read.
const QDir data_dir("../data/dxf");
const QString alti_file = data_dir.absoluteFilePath("ALT_TEST.dxf");
const QString oro_file = data_dir.absoluteFilePath("ORO_TEST.dxf");
ASSERT_TRUE(QFile::exists(alti_file));
ASSERT_TRUE(QFile::exists(oro_file));

// Max number of isolines to import.
const int MAX = 1000;
LPTYSiteNode site_ptr = new TYSiteNode;
LPTYTopographie pTYTopographie = site_ptr->getTopographie();

// filter to get points from a polyline. See TYDXFImporter::DecimatePolyline
// method.
const double min_point_distance = 0.;
const double min_height_building = 4.; // in meter
TYDXFBDTopoImporter importer = TYDXFBDTopoImporter(min_point_distance,
                                                   min_height_building);

// Copy/paste from 'TYDXFBDTopoImporter::import'.
DXFLimites LimiteMin, LimiteMax;
LimiteMin.x = LimiteMin.y = 0;
LimiteMax.x = LimiteMax.y = 0;
DXFLimites offset;
offset.x = (LimiteMax.x + LimiteMin.x) / 2;
offset.y = (LimiteMax.y + LimiteMin.y) / 2;

// Copy/paste bis repetita (suppose we have 8 DXF files).
const char* DXFPrefixFileName[] =
    {
        "ALT",
        "VCR",
        "FTE",
        "HYD",
        "LID",
        "BAT",
        "VEG",
        "ORO"
    };
const int nDXFFileCount = sizeof(DXFPrefixFileName) / sizeof(const char*);//8;
dimeModel model[nDXFFileCount];

// Read just two files.
importer.ReadDXF(&(model[0]), alti_file);
importer.ReadDXF(&(model[7]), oro_file);

// Import isolines from the two previous DXF files.
importer.importIsoline(MAX, model, offset, pTYTopographie);

// Should have 7 polylines.
const size_t nb_polyline = 7;
const TYTabCourbeNiveauGeoNode& isoline_list = pTYTopographie->getListCrbNiv();
ASSERT_EQ(nb_polyline, isoline_list.size());

// Compute the number of points for each isoline.
int count_nb_point = 0;
for (int i = 0; i < nb_polyline; ++i)
{
    const TYCourbeNiveau* isoline_ptr =
        TYCourbeNiveau::safeDownCast(isoline_list[i]->getElement());
    count_nb_point += isoline_ptr->getListPoints().size();
}

// Should have 495 points.
ASSERT_EQ(495, count_nb_point);

} // End 'topography_import' test.


/**
 * \file test_environment.cpp
 * \test Some debuging of the testing environment
 *
 *  Created on: 6 nov. 2012
 *      Author: Anthony Truchet <anthony.truchet@logilab.fr>
 */

#include <cstdlib>
#include <string>

#include "gtest/gtest.h"

#include <QDir>

#include "TympanTestsConfig.h"

using std::cout;
using std::cerr;
using std::endl;

TEST(TestEnvironment, Path)
{
    char* path = std::getenv("PATH");
    ASSERT_FALSE(path == NULL);
    cout << "PATH = " << path << endl;
}

TEST(TestEnvironment, TestDataPath)
{
    QString expected_data_path = QDir::toNativeSeparators(QDir::current().absoluteFilePath("data"));
    EXPECT_EQ(expected_data_path.toStdString(), std::string(TYMPAN_TESTS_DATA_DIR));
}

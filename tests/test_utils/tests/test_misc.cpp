//! \test test_misc.cpp

#include "gtest/gtest.h"

#include<string>
#include<sstream>
#include<algorithm>    // copy
#include<iterator>     // ostream_operator

#include "test_utils/misc.hpp"

#include <boost/foreach.hpp>

using namespace std;

// to ease debuging of the test
#include<iostream>
template<class T>
void print_table_of(const deque<deque<T> >& table)
{
    BOOST_FOREACH(const deque<T>& rec, table)
    {
        copy(rec.begin(), rec.end(),
             ostream_iterator<T>(cout, "|"));
        cout << endl;
    }
}

TEST(TestTestUtils, csvAsString) {
    string s = "Field 1,  \"putting quotes around fields, allows commas\", Field 3\n"
               "1.0, 2.1, 3e-2\n";

    istringstream ss(s);
    deque<deque<string> > table = readCsvAsStringTable(ss);

    ASSERT_EQ(2, table.size());

    EXPECT_EQ("putting quotes around fields, allows commas", table[0][1]);
    EXPECT_EQ("3e-2", table[1][2]);

    //print_table_of<string>(table);
} // TEST(TestTestUtils, csvAsString)

TEST(TestTestUtils, readCsvAsTableOf) {
    string s = "1.0, 2.1, .3e-2\n"
               "1, 0  , -4 ";

    istringstream ss(s);
    deque<deque<double> > table = readCsvAsTableOf<double>(ss);

    ASSERT_EQ(2, table.size());

    EXPECT_EQ(2.1, table[0][1]);
    EXPECT_EQ(0.003, table[0][2]);

    //print_table_of<double>(table);
} // TEST(TestTestUtils, csvAsDouble)

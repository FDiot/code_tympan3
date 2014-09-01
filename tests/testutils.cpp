/**
 * @file misc.cpp
 *
 * @brief Implementation of misc utilities for testing.
 */

#include<iostream>
#include<string>
#include<deque>

#include<boost/tokenizer.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>

#include "testutils.h"

using namespace std;
using namespace boost;

deque<deque<string> > readCsvAsStringTable(istream& in)
{
    typedef tokenizer< escaped_list_separator<char> > Tokenizer;
    deque<deque<string> > table;
    string line;
    while (getline(in, line))
    {
        deque<string> line_array;
        Tokenizer tok(line);
        BOOST_FOREACH(const string & str_rec, tok)
        {
            line_array.push_back(trim_copy(str_rec));
        }
        table.push_back(line_array);
    }
    return table;
}

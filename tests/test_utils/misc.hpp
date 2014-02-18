/**
 * @file misc.hpp
 *
 * @brief Misc utilities for testing.
 */

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

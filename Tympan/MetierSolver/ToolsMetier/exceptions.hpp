/**
 * @file exception.hpp
 *
 * @brief Utilities to handle exceptions
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#ifndef TYMPAN__EXCEPTIONS_HPP__INCLUDED
#define TYMPAN__EXCEPTIONS_HPP__INCLUDED

#include <cassert>
#include <exception>
#include <ostream>

#include <boost/exception/all.hpp>
#include <boost/tuple/tuple.hpp>

namespace tympan
{

// We use the boost::exception to add information to exception classes.
// Cf. http://www.boost.org/doc/libs/1_54_0/libs/exception/doc/tutorial_transporting_data.html

/// The base exception class for all exceptions specific to Code_TYMPAN
struct exception: virtual boost::exception, virtual std::exception {
    virtual const char* what() const throw()
    { return "Code_TYMPAN (unspecified) base exception"; }
};

/// The base exception class for internal logic / algorithmic errors
struct logic_error: virtual tympan::exception, virtual std::logic_error {
    logic_error() : std::logic_error("Code_TYMPAN internal logic error") {};
    logic_error(const std::string& desc) : std::logic_error(desc) {};
};

/// The base exception class for errors due to invalid data
struct invalid_data: virtual tympan::exception, virtual std::runtime_error {
    invalid_data() : std::runtime_error("Code_TYMPAN invalid data encountered") {};
    invalid_data(const std::string& desc) : std::runtime_error(desc) {};
};

/// The tag for retrieving the source file name
typedef boost::error_info<struct tag_source_file_name, const char*> source_file_name;
/// The tag for retrieving the source line number
typedef boost::error_info<struct tag_source_line_num, unsigned> source_line_num;
/// The tuple packing source file name and line num into a source_loc
typedef boost::tuple<source_file_name, source_line_num> source_loc;

} //namespace tympan

/// This macro build a \c source_loc object to be attached to a \c tympan::Exception
#define tympan_source_loc  (tympan::source_loc( __FILE__, __LINE__))

#endif // TYMPAN__EXCEPTIONS_HPP__INCLUDED

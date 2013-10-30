/**
 * @file test_exceptions.hpp
 *
 * @brief Test the exception utilities
 *
 * @author Anthony Truchet <anthony.truchet@logilab.fr>
 *
 */

#include <cstdlib>

#include "gtest/gtest.h"
// #include "test_utils/prettyprint.hpp"

#include "Tympan/MetierSolver/ToolsMetier/exceptions.hpp"

static unsigned test_exceptions_source_line_no;

void throw_logic_default_constructed()
{
    throw tympan::logic_error();
}

void throw_invalid_data_localized()
{
    test_exceptions_source_line_no = __LINE__;
    throw tympan::invalid_data("This is bad") << tympan_source_loc;
}


TEST(exceptions, simple_logic) {
    ASSERT_THROW(throw_logic_default_constructed(), tympan::exception);
    try {
        throw_logic_default_constructed();
    }
    catch(const std::logic_error& exc)  {
        EXPECT_STREQ("Code_TYMPAN internal logic error", exc.what());
    }
    catch(...) {
        FAIL() << "a tympan::exception which is not, also, a std::logic_error was thrown";
    }
    // Check that std::exception is NOT an ambiguous base
    ASSERT_THROW(throw_logic_default_constructed(), std::exception);
}

TEST(exceptions, source_localized) {
    using tympan::source_file_name_errinfo;
    using tympan::source_line_num_errinfo;
    using boost::get_error_info;

    ASSERT_THROW(throw_invalid_data_localized(), tympan::exception);
    try {
        throw_invalid_data_localized();
    }
    catch(const std::runtime_error& exc)  {
        EXPECT_STREQ("This is bad", exc.what());
        if( const char * const* p_filename = get_error_info<source_file_name_errinfo>(exc) )
        {
            EXPECT_STREQ(__FILE__, *p_filename);
        }
        else
            FAIL() << "We could not extract 'source_file_name' from the exception";
        if( unsigned const* p_linenum = get_error_info<source_line_num_errinfo>(exc) )
        {
            EXPECT_EQ(test_exceptions_source_line_no+1, *p_linenum);
        }
        else
            FAIL() << "We could not extract 'source_line_num' from the exception";
    }
    catch(...) {
        FAIL() << "a tympan::exception which is not, also, a std::runtime_error was thrown";
    }
}

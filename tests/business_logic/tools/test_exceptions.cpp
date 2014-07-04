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

#include "Tympan/MetierSolver/CommonTools/exceptions.hpp"

static unsigned test_exceptions_source_line_no;
static const char* test_exceptions_func_name;

void throw_logic_default_constructed()
{
    throw tympan::logic_error();
}

void throw_invalid_data_localized()
{
    test_exceptions_func_name = BOOST_CURRENT_FUNCTION;
    test_exceptions_source_line_no = __LINE__;
    throw tympan::invalid_data("This is bad") << tympan_source_loc;
}


TEST(exceptions, simple_logic)
{
    ASSERT_THROW(throw_logic_default_constructed(), tympan::exception);
    try
    {
        throw_logic_default_constructed();
    }
    catch (const std::logic_error& exc)
    {
        EXPECT_STREQ("Code_TYMPAN internal logic error", exc.what());
    }
    catch (...)
    {
        FAIL() << "a tympan::exception which is not, also, a std::logic_error was thrown";
    }
    // Check that std::exception is NOT an ambiguous base
    ASSERT_THROW(throw_logic_default_constructed(), std::exception);
}

TEST(exceptions, source_localized)
{
    using ::boost::throw_file;
    using ::boost::throw_line;
    using ::boost::throw_function;
    using boost::get_error_info;

    ASSERT_THROW(throw_invalid_data_localized(), tympan::exception);
    try
    {
        throw_invalid_data_localized();
    }
    catch (const std::runtime_error& exc)
    {
        EXPECT_STREQ("This is bad", exc.what());
        if (const char* const* p_filename = get_error_info<throw_file>(exc))
        {
            EXPECT_STREQ(__FILE__, *p_filename);
        }
        else
        {
            FAIL() << "We could not extract 'throw_file' from the exception";
        }
        if (const char* const* p_funcname = get_error_info<throw_function>(exc))
        {
            EXPECT_STREQ(test_exceptions_func_name, *p_funcname);
        }
        else
        {
            FAIL() << "We could not extract 'throw_function' from the exception";
        }
        if (const int* const p_linenum = get_error_info<throw_line>(exc))
        {
            EXPECT_EQ(test_exceptions_source_line_no + 1, *p_linenum);
        }
        else
        {
            FAIL() << "We could not extract 'throw_line' from the exception";
        }
    }
    catch (...)
    {
        FAIL() << "a tympan::exception which is not, also, a std::runtime_error was thrown";
    }
}

//----------------------------------------------------------------------------
/** @file UnitTestMain.cpp
    Main function for running unit tests from all modules in project Fuego. */
//----------------------------------------------------------------------------

#include "SgSystem.h"

#include "GoInit.h"
#include "SgInit.h"

//----------------------------------------------------------------------------

namespace {

void Init()
{
    SgInit();
    GoInit();
}

void Fini()
{
    GoFini();
    SgFini();
}

} // namespace

//----------------------------------------------------------------------------

// Handling of unit testing framework initialization is messy and not
// documented in the Boost 1.34 documentation. See also:
// http://lists.boost.org/Archives/boost/2006/11/112946.php

#include <cstdlib>
#define BOOST_TEST_ALTERNATIVE_INIT_API
#define BOOST_TEST_NO_MAIN

#include <boost/test/included/unit_test.hpp>

bool init_unit_test()
{
    try
    {
        Init();
    }
    catch (const std::exception&)
    {
        return false;
    }
    if (std::atexit(Fini) != 0)
        return false;
    return true;
}

int main(int argc, char* argv[])
{
    return boost::unit_test::unit_test_main(&init_unit_test, argc, argv);
}

//----------------------------------------------------------------------------

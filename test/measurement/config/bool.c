/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen University, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */



/**
 * @file       test/measurement/config/bool.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @status alpha
 *
 */


#include <config.h>


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>


#include <CuTest.h>


#include <SCOREP_Config.h>


static bool bool_default_true;
static bool bool_default_yes;
static bool bool_default_on;
static bool bool_default_1;
static bool bool_default_42;
static bool bool_default_false;
static bool bool_default_no;
static bool bool_default_off;
static bool bool_default_0;
static bool bool_default_garbage;


static SCOREP_ConfigVariable bool_config_variables[] =
{
    {
        "bool_default_true",
        SCOREP_CONFIG_TYPE_BOOL,
        &bool_default_true,
        NULL,
        "true",
        "",
        ""
    },
    {
        "bool_default_yes",
        SCOREP_CONFIG_TYPE_BOOL,
        &bool_default_yes,
        NULL,
        "yes",
        "",
        ""
    },
    {
        "bool_default_on",
        SCOREP_CONFIG_TYPE_BOOL,
        &bool_default_on,
        NULL,
        "on",
        "",
        ""
    },
    {
        "bool_default_1",
        SCOREP_CONFIG_TYPE_BOOL,
        &bool_default_1,
        NULL,
        "1",
        "",
        ""
    },
    {
        "bool_default_42",
        SCOREP_CONFIG_TYPE_BOOL,
        &bool_default_42,
        NULL,
        "42",
        "",
        ""
    },
    {
        "bool_default_false",
        SCOREP_CONFIG_TYPE_BOOL,
        &bool_default_false,
        NULL,
        "false",
        "",
        ""
    },
    {
        "bool_default_no",
        SCOREP_CONFIG_TYPE_BOOL,
        &bool_default_no,
        NULL,
        "no",
        "",
        ""
    },
    {
        "bool_default_off",
        SCOREP_CONFIG_TYPE_BOOL,
        &bool_default_off,
        NULL,
        "off",
        "",
        ""
    },
    {
        "bool_default_0",
        SCOREP_CONFIG_TYPE_BOOL,
        &bool_default_0,
        NULL,
        "0",
        "",
        ""
    },
    {
        "bool_default_garbage",
        SCOREP_CONFIG_TYPE_BOOL,
        &bool_default_garbage,
        NULL,
        "garbage",
        "",
        ""
    },
    SCOREP_CONFIG_TERMINATOR
};


static void
test_bool_default_true( CuTest* tc )
{
    CuAssert( tc, "default \"true\"", bool_default_true == true );
}


static void
test_bool_default_yes( CuTest* tc )
{
    CuAssert( tc, "default \"yes\"", bool_default_yes == true );
}


static void
test_bool_default_on( CuTest* tc )
{
    CuAssert( tc, "default \"on\"", bool_default_on == true );
}


static void
test_bool_default_1( CuTest* tc )
{
    CuAssert( tc, "default \"1\"", bool_default_1 == true );
}


static void
test_bool_default_42( CuTest* tc )
{
    CuAssert( tc, "default \"42\"", bool_default_42 == true );
}


static void
test_bool_default_false( CuTest* tc )
{
    CuAssert( tc, "default \"false\"", bool_default_false == false );
}


static void
test_bool_default_no( CuTest* tc )
{
    CuAssert( tc, "default \"no\"", bool_default_no == false );
}


static void
test_bool_default_off( CuTest* tc )
{
    CuAssert( tc, "default \"off\"", bool_default_off == false );
}


static void
test_bool_default_0( CuTest* tc )
{
    CuAssert( tc, "default \"0\"", bool_default_0 == false );
}


static void
test_bool_default_garbage( CuTest* tc )
{
    CuAssert( tc, "default \"garbage\"", bool_default_garbage == false );
}


int
main()
{
    CuString* output = CuStringNew();
    CuSuite*  suite  = CuSuiteNew( "default values for config type bool" );

    SCOREP_ConfigInit();

    SCOREP_ConfigRegister( "", bool_config_variables );

    SUITE_ADD_TEST_NAME( suite, "bool with default \"true\"",
                         test_bool_default_true );
    SUITE_ADD_TEST_NAME( suite, "bool with default \"yes\"",
                         test_bool_default_yes );
    SUITE_ADD_TEST_NAME( suite, "bool with default \"on\"",
                         test_bool_default_on );
    SUITE_ADD_TEST_NAME( suite, "bool with default \"1\"",
                         test_bool_default_1 );
    SUITE_ADD_TEST_NAME( suite, "bool with default \"42\"",
                         test_bool_default_42 );
    SUITE_ADD_TEST_NAME( suite, "bool with default \"false\"",
                         test_bool_default_false );
    SUITE_ADD_TEST_NAME( suite, "bool with default \"no\"",
                         test_bool_default_no );
    SUITE_ADD_TEST_NAME( suite, "bool with default \"off\"",
                         test_bool_default_off );
    SUITE_ADD_TEST_NAME( suite, "bool with default \"0\"",
                         test_bool_default_0 );
    SUITE_ADD_TEST_NAME( suite, "bool with default \"garbage\"",
                         test_bool_default_garbage );

    CuSuiteRun( suite );
    CuSuiteSummary( suite, output );
    printf( "%s", output->buffer );

    int failCount = suite->failCount;
    CuSuiteFree( suite );
    CuStringFree( output );

    SCOREP_ConfigFini();

    return failCount;
}

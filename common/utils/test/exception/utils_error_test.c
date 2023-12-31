/*
 * This file is part of the Score-P software ecosystem (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2012,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2012,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2012, 2022,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2012,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2012,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */



/**
 * @file
 *
 *
 */

#include <config.h>
#include <UTILS_Error.h>

#include <stdio.h>

#include <utils_package.h>


int
main()
{
    printf( "================================================================\n"
            "The following error test will print error messages in order\n"
            "to test whether the error system works correctly. If we suppress\n"
            "this messages we would no longer see, if a segmentation fault\n"
            "occurs.\n"
            "================================================================\n"
            );
    errno = 0;
    UTILS_ERROR_POSIX( "This error message is no real error: Success" );
    errno = ENOMEM;
    UTILS_ERROR_POSIX( "This error message is no real error: Valid errno" );
    errno = 1024;
    UTILS_ERROR_POSIX( "This error message is no real error: Unknown POSIX" );
    errno = -1024;
    UTILS_ERROR_POSIX( "This error message is no real error: Unknown POSIX II" );
    UTILS_ERROR( PACKAGE_SUCCESS,
                 "This error message is no real error: "
                 UTILS_STRINGIFY( PACKAGE_SUCCESS ) );
    UTILS_ERROR( PACKAGE_ERROR_END_OF_FUNCTION,
                 "This error message is no real error: "
                 UTILS_STRINGIFY( PACKAGE_ERROR_END_OF_FUNCTION ) );
    UTILS_ERROR( PACKAGE_ERROR_END_OF_FUNCTION, "Just a test of the error system." );
    UTILS_ERROR( 1024,
                 "This error message is no real error: error invalid positive code" );
    UTILS_ERROR( -1024,
                 "This error message is no real error: error invalid negative code" );

    UTILS_WARNING( "This is just a warning from the exception module." );

    UTILS_DEPRECATED( "This is just a deprecation warning from the exception module." );

    printf( "================================================================\n"
            "End of error tests.\n"
            "================================================================\n"
            );
    return 0;
}

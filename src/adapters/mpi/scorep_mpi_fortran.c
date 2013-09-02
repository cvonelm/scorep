/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2013
 *    Forschungszentrum Juelich GmbH, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */

/**
 * @file       scorep_mpi_fortran.c
 * @ingroup    MPI_Wrapper
 *
 * @brief Implements Fortran <-> C data converter functions.
 */

#include <config.h>
#include "scorep_mpi_fortran.h"
#include <UTILS_Error.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

char*
scorep_f2c_string( const char* f_string, uint32_t length )
{
    char* c_string = ( char* )malloc( ( length + 1 ) * sizeof( char ) );
    if ( !c_string )
    {
        UTILS_ERROR_POSIX( "Failed to allocate memory for string" );
        exit( EXIT_FAILURE );
    }
    strncpy( c_string, f_string, length );
    c_string[ length ] = '\0';
    if ( length > 0 )
    {
        char* cur = c_string + length - 1;
        while ( ( cur != c_string ) && isspace( *cur ) )
        {
            cur--;
        }
        cur[ isspace( *cur ) ? 0 : 1 ] = '\0';
    }
    return c_string;
}

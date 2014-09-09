/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief C interface wrappers for error handling
 */

#include <config.h>
#include "SCOREP_Mpi.h"
#include <SCOREP_Events.h>

/**
 * @name C wrappers
 * @{
 */

#if HAVE( DECL_PMPI_ADD_ERROR_CLASS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Add_error_class )
/**
 * Measurement wrapper for MPI_Add_error_class
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup err
 * Triggers an enter and exit event.
 * It wraps the MPI_Add_error_class call with enter and exit events.
 */
int
MPI_Add_error_class( int* errorclass )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ERR ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_ADD_ERROR_CLASS ] );

        return_val = PMPI_Add_error_class( errorclass );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_ADD_ERROR_CLASS ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Add_error_class( errorclass );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_ADD_ERROR_CODE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Add_error_code )
/**
 * Measurement wrapper for MPI_Add_error_code
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup err
 * Triggers an enter and exit event.
 * It wraps the MPI_Add_error_code call with enter and exit events.
 */
int
MPI_Add_error_code( int errorclass, int* errorcode )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ERR ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_ADD_ERROR_CODE ] );

        return_val = PMPI_Add_error_code( errorclass, errorcode );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_ADD_ERROR_CODE ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Add_error_code( errorclass, errorcode );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_ERRHANDLER_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Errhandler_create )
/**
 * Measurement wrapper for MPI_Errhandler_create
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup err
 * Triggers an enter and exit event.
 * It wraps the MPI_Errhandler_create call with enter and exit events.
 */
int
MPI_Errhandler_create( MPI_Handler_function* function, MPI_Errhandler* errhandler )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ERR ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_ERRHANDLER_CREATE ] );

        return_val = PMPI_Errhandler_create( function, errhandler );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_ERRHANDLER_CREATE ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Errhandler_create( function, errhandler );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_ERRHANDLER_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Errhandler_free )
/**
 * Measurement wrapper for MPI_Errhandler_free
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup err
 * Triggers an enter and exit event.
 * It wraps the MPI_Errhandler_free call with enter and exit events.
 */
int
MPI_Errhandler_free( MPI_Errhandler* errhandler )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ERR ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_ERRHANDLER_FREE ] );

        return_val = PMPI_Errhandler_free( errhandler );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_ERRHANDLER_FREE ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Errhandler_free( errhandler );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_ERRHANDLER_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Errhandler_get )
/**
 * Measurement wrapper for MPI_Errhandler_get
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup err
 * Triggers an enter and exit event.
 * It wraps the MPI_Errhandler_get call with enter and exit events.
 */
int
MPI_Errhandler_get( MPI_Comm comm, MPI_Errhandler* errhandler )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ERR ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_ERRHANDLER_GET ] );

        return_val = PMPI_Errhandler_get( comm, errhandler );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_ERRHANDLER_GET ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Errhandler_get( comm, errhandler );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_ERRHANDLER_SET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Errhandler_set )
/**
 * Measurement wrapper for MPI_Errhandler_set
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup err
 * Triggers an enter and exit event.
 * It wraps the MPI_Errhandler_set call with enter and exit events.
 */
int
MPI_Errhandler_set( MPI_Comm comm, MPI_Errhandler errhandler )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ERR ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_ERRHANDLER_SET ] );

        return_val = PMPI_Errhandler_set( comm, errhandler );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_ERRHANDLER_SET ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Errhandler_set( comm, errhandler );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_ERROR_CLASS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Error_class )
/**
 * Measurement wrapper for MPI_Error_class
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup err
 * Triggers an enter and exit event.
 * It wraps the MPI_Error_class call with enter and exit events.
 */
int
MPI_Error_class( int errorcode, int* errorclass )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ERR ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_ERROR_CLASS ] );

        return_val = PMPI_Error_class( errorcode, errorclass );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_ERROR_CLASS ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Error_class( errorcode, errorclass );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_ERROR_STRING ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Error_string )
/**
 * Measurement wrapper for MPI_Error_string
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup err
 * Triggers an enter and exit event.
 * It wraps the MPI_Error_string call with enter and exit events.
 */
int
MPI_Error_string( int errorcode, char* string, int* resultlen )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ERR ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_ERROR_STRING ] );

        return_val = PMPI_Error_string( errorcode, string, resultlen );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_ERROR_STRING ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Error_string( errorcode, string, resultlen );
    }

    return return_val;
}
#endif


#if HAVE( DECL_PMPI_ADD_ERROR_STRING ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Add_error_string ) && defined( SCOREP_MPI_ADD_ERROR_STRING_PROTO_ARGS )
/**
 * Measurement wrapper for MPI_Add_error_string
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_StdWithProto.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup err
 * Triggers an enter and exit event.
 * It wraps the MPI_Add_error_string call with enter and exit events.
 */
int
MPI_Add_error_string SCOREP_MPI_ADD_ERROR_STRING_PROTO_ARGS
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ERR ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_ADD_ERROR_STRING ] );

        return_val = PMPI_Add_error_string( errorcode, string );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_ADD_ERROR_STRING ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Add_error_string( errorcode, string );
    }

    return return_val;
}
#endif


/**
 * @}
 */

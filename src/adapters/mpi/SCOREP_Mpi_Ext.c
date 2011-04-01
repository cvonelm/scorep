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
 * @file       SCOREP_Mpi_Ext.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    MPI_Wrapper
 *
 * @brief C interface wrappers for external interface functions
 */

#include <config.h>
#include "SCOREP_Mpi.h"

/**
 * @name C wrappers
 * @{
 */

#if HAVE( DECL_PMPI_ABORT )
/**
 * Measurement wrapper for MPI_Abort
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI 1.0
 * @ingroup ext
 */
int
MPI_Abort( MPI_Comm comm,
           int      errorcode )
{
    int return_val;

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_WARNING | SCOREP_DEBUG_MPI,
                         "Explicit MPI_Abort call abandoning the SCOREP measurement." );

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_EXT ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_ABORT ] );

        return_val = PMPI_Abort( comm, errorcode );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_ABORT ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Abort( comm, errorcode );
    }

    return return_val;
}
#endif

#if HAVE( MPI_GREQUEST_COMPLETE_COMPLIANT )
#if HAVE( DECL_PMPI_GREQUEST_COMPLETE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Grequest_complete )
/**
 * Measurement wrapper for MPI_Grequest_complete
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Grequest_complete( MPI_Request request )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_EXT ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_GREQUEST_COMPLETE ] );

        return_val = PMPI_Grequest_complete( request );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_GREQUEST_COMPLETE ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Grequest_complete( request );
    }

    return return_val;
}
#endif
#endif

#if HAVE( DECL_PMPI_GET_COUNT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Get_count )
/**
 * Measurement wrapper for MPI_Get_count
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Get_count( MPI_Status* status, MPI_Datatype datatype, int* count )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_EXT ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_GET_COUNT ] );

        return_val = PMPI_Get_count( status, datatype, count );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_GET_COUNT ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Get_count( status, datatype, count );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_GET_ELEMENTS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Get_elements )
/**
 * Measurement wrapper for MPI_Get_elements
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Get_elements( MPI_Status* status, MPI_Datatype datatype, int* count )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_EXT ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_GET_ELEMENTS ] );

        return_val = PMPI_Get_elements( status, datatype, count );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_GET_ELEMENTS ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Get_elements( status, datatype, count );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_GET_PROCESSOR_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Get_processor_name )
/**
 * Measurement wrapper for MPI_Get_processor_name
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Get_processor_name( char* name, int* resultlen )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_EXT ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_GET_PROCESSOR_NAME ] );

        return_val = PMPI_Get_processor_name( name, resultlen );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_GET_PROCESSOR_NAME ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Get_processor_name( name, resultlen );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_GREQUEST_START ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Grequest_start )
/**
 * Measurement wrapper for MPI_Grequest_start
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Grequest_start( MPI_Grequest_query_function* query_fn, MPI_Grequest_free_function* free_fn, MPI_Grequest_cancel_function* cancel_fn, void* extra_state, MPI_Request* request )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_EXT ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_GREQUEST_START ] );

        return_val = PMPI_Grequest_start( query_fn, free_fn, cancel_fn, extra_state, request );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_GREQUEST_START ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Grequest_start( query_fn, free_fn, cancel_fn, extra_state, request );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_STATUS_SET_CANCELLED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Status_set_cancelled )
/**
 * Measurement wrapper for MPI_Status_set_cancelled
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Status_set_cancelled( MPI_Status* status, int flag )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_EXT ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_STATUS_SET_CANCELLED ] );

        return_val = PMPI_Status_set_cancelled( status, flag );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_STATUS_SET_CANCELLED ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Status_set_cancelled( status, flag );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_STATUS_SET_ELEMENTS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Status_set_elements )
/**
 * Measurement wrapper for MPI_Status_set_elements
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup ext
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Status_set_elements( MPI_Status* status, MPI_Datatype datatype, int count )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_EXT ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_STATUS_SET_ELEMENTS ] );

        return_val = PMPI_Status_set_elements( status, datatype, count );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_STATUS_SET_ELEMENTS ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Status_set_elements( status, datatype, count );
    }

    return return_val;
}
#endif


/**
 * @}
 */

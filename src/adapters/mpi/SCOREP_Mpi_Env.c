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
 * @file       SCOREP_Mpi_Env.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     alpha
 * @ingroup    MPI_Wrapper
 *
 * @brief C interface wrappers for environmental management.
 */

#include <config.h>
#include "SCOREP_Mpi.h"

#if defined( SCOREP_WITH_OA )
#include <SCOREP_OA_Init.h>
#endif

/** Flag set if the measurement sysem was already opened by another adapter.
    If the measurement system is not already initilized, it is assumed that
    the mpi adapter is the only active adapter. In this case, at first an
    additional region is entered MPI_Init. Thus, all regions appear as
    children of this region.
 */
static int scorep_mpi_parallel_entered = 0;

/**
 * @name C wrappers
 * @{
 */

/**
 * Measurement wrapper for MPI_Init.
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup env
 * If the measurement system is not initialized, it will iniialize the measurement
 * system and enter a special region named "parallel" which is exited when MPI is
 * finalized.
 * The sequence of events generated by this wrapper is:
 * @li enter region 'PARALLEL': Only if this adapter initializes the measurement system.
 * @li enter region 'MPI_Init'
 * @li define communicator 'COMM_WORLD'
 * @li exit region 'MPI_Init'
 */
int
MPI_Init( int* argc, char*** argv )
{
    int event_gen_active = 0;          /* init is deferred to later */
    int return_val;
    int fflag, rank;

    if ( !SCOREP_IsInitialized() )
    {
        /* Initialize the measurement system */
        SCOREP_InitMeasurement();

        /* Enter global MPI region */
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP_PARALLEL__MPI ] );

        /* Remember that SCOREP_PARALLEL__MPI was entered */
        scorep_mpi_parallel_entered = 1;
    }

    event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ENV );

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        /* Enter the init region */
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_INIT ] );
    }

    return_val = PMPI_Init( argc, argv );

    /* XXXX should only continue if MPI initialised OK! */

    if ( ( PMPI_Finalized( &fflag ) == MPI_SUCCESS ) && ( fflag == 0 ) )
    {
        SCOREP_OnPMPI_Init();

        /* initialize communicator management and register MPI_COMM_WORLD*/
        scorep_mpi_comm_init();

        /* Obtain rank */
        PMPI_Comm_rank( MPI_COMM_WORLD, &rank );

        /* complete initialization of measurement core and MPI event handling */
        SCOREP_InitMeasurementMPI( rank );
    }

    if ( event_gen_active )
    {
        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_INIT ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }

#if defined( SCOREP_WITH_OA )
    SCOREP_OA_Init();
#endif

    return return_val;
}

#if HAVE( DECL_PMPI_INIT_THREAD )
/**
 * Measurement wrapper for MPI_Init_thread, the thread-capable
 * alternative to MPI_Init.
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup env
 * If the measurement system is not initialized, it will iniialize the measurement
 * system and enter a special region named "parallel" which is exited when MPI is
 * finalized.
 * The sequence of events generated by this wrapper is:
 * @li enter region 'PARALLEL': Only if this adapter initializes the measurement system.
 * @li enter region 'MPI_Init_thread'
 * @li define communicator 'COMM_WORLD'
 * @li exit region 'MPI_Init_thread'
 */
int
MPI_Init_thread( int* argc, char*** argv, int required, int* provided )
{
    int event_gen_active = 0;
    int return_val;
    int fflag, rank;

    if ( !SCOREP_IsInitialized() )
    {
        /* Initialize the measurement system */
        SCOREP_InitMeasurement();

        /* Enter global MPI region */
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP_PARALLEL__MPI ] );

        /* Remember that SCOREP_PARALLEL__MPI was entered */
        scorep_mpi_parallel_entered = 1;
    }

    event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ENV );

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();

        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_INIT_THREAD ] );
    }

    return_val = PMPI_Init_thread( argc, argv, required, provided );

    /* XXXX should only continue if MPI initialised OK! */

    if ( ( return_val == MPI_SUCCESS ) && ( *provided > MPI_THREAD_FUNNELED ) )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_WARNING | SCOREP_DEBUG_MPI,
                             "MPI environment initialized with level exceeding MPI_THREAD_FUNNELED!" );
        /* XXXX continue even though not supported by analysis */
    }

    if ( ( PMPI_Finalized( &fflag ) == MPI_SUCCESS ) && ( fflag == 0 ) )
    {
        SCOREP_OnPMPI_Init();

        /* initialize communicator management and register MPI_COMM_WORLD */
        scorep_mpi_comm_init();

        /* Obtain rank */
        PMPI_Comm_rank( MPI_COMM_WORLD, &rank );

        /* complete initialization of measurement core and MPI event handling */
        SCOREP_InitMeasurementMPI( rank );
    }

    if ( event_gen_active )
    {
        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_INIT_THREAD ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif

/**
 * Measurement wrapper for MPI_Finalize
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup env
 * Generates an enter event at function start and an exit event at function end.
 * If the "parallel" region was entered in MPI_Init, it exits the "parallel" region.
 * It does not perform the MPI finalization, because MPI is still needed by the
 * measurement system, but substituts it with a barrier. The MPI finalization
 * will be done from the measurement system.
 */
int
MPI_Finalize()
{
    const int event_gen_active = SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ENV );
    int       return_val;

    if ( event_gen_active )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_FINALIZE ] );
    }

    /* finalize communicator and request management */
    scorep_mpi_comm_finalize();
/* Asynchroneous communication not supported
   scorep_mpi_request_finalize();
 */

    /* finalize MPI event handling */
    SCOREP_FinalizeMeasurementMPI();

    /* fake finalization, so that MPI can be used during SCOREP finalization */
    return_val = PMPI_Barrier( MPI_COMM_WORLD );

    if ( event_gen_active )
    {
        /* Exit MPI_Finalize region */
        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_FINALIZE ] );

        /* Exit the extra parallel region in case it was entered in MPI_Init */
        if ( scorep_mpi_parallel_entered )
        {
            SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP_PARALLEL__MPI ] );
        }

        SCOREP_MPI_EVENT_GEN_ON();
    }

    return return_val;
}

#if HAVE( DECL_PMPI_IS_THREAD_MAIN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Is_thread_main )
/**
 * Measurement wrapper for MPI_Is_thread_main
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup env
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Is_thread_main( int* flag )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ENV ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_IS_THREAD_MAIN ] );

        return_val = PMPI_Is_thread_main( flag );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_IS_THREAD_MAIN ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Is_thread_main( flag );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_QUERY_THREAD ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Query_thread )
/**
 * Measurement wrapper for MPI_Query_thread
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup env
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Query_thread( int* provided )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ENV ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_QUERY_THREAD ] );

        return_val = PMPI_Query_thread( provided );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_QUERY_THREAD ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Query_thread( provided );
    }

    return return_val;
}
#endif


#if HAVE( DECL_PMPI_FINALIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Finalized )
/**
 * Measurement wrapper for MPI_Finalized
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-2
 * @ingroup env
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Finalized( int* flag )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ENV ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_FINALIZED ] );

        return_val = PMPI_Finalized( flag );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_FINALIZED ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Finalized( flag );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_INITIALIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Initialized )
/**
 * Measurement wrapper for MPI_Initialized
 * @note Auto-generated by wrapgen from template: SCOREP_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup env
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Initialized( int* flag )
{
    int return_val;

    if ( SCOREP_MPI_IS_EVENT_GEN_ON_FOR( SCOREP_MPI_ENABLED_ENV ) )
    {
        SCOREP_MPI_EVENT_GEN_OFF();
        SCOREP_EnterRegion( scorep_mpi_regid[ SCOREP__MPI_INITIALIZED ] );

        return_val = PMPI_Initialized( flag );

        SCOREP_ExitRegion( scorep_mpi_regid[ SCOREP__MPI_INITIALIZED ] );
        SCOREP_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Initialized( flag );
    }

    return return_val;
}
#endif


/**
 * @}
 */

/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       silc_mpi_enabled.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status ALPHA
 *
 *
 */


#include "silc_mpi.h"

#include <SILC_Debug.h>
#include <silc_thread.h>
#include <mpi.h>
#include <assert.h>


static MPI_Comm silc_mpi_comm_world;


extern void
silc_status_initialize_mpi();


void
SILC_Status_Initialize()
{
    silc_status_initialize_mpi();
    // it is too early to call PMPI from here.
}


bool
silc_create_experiment_dir( char* dirName,
                            int   dirNameSize,
                            void  ( * createDir )( const char* ) )
{
    if ( !SILC_Mpi_IsInitialized() )
    {
        SILC_DEBUG_PRINTF( SILC_DEBUG_MPI, "MPI not initialized, experiment directory creation deferred." );
        return false;
    }

    if ( SILC_Mpi_GetRank() == 0 )
    {
        createDir( dirName );
    }
    //MPI_Bcast( dirName, dirNameSize, MPI_CHAR, 0, MPI_COMM_WORLD );

    return true;
}


bool
SILC_Mpi_HasMpi()
{
    return true;
}


void
SILC_Mpi_GlobalBarrier()
{
    assert( SILC_Mpi_IsInitialized() );
    assert( !SILC_Mpi_IsFinalized() );
    int status = PMPI_Barrier( silc_mpi_comm_world );
    assert( status == MPI_SUCCESS );
}


void
SILC_Mpi_DuplicateCommWorld()
{
    assert( SILC_Mpi_IsInitialized() );
    assert( !SILC_Mpi_IsFinalized() );
    int status = PMPI_Comm_dup( MPI_COMM_WORLD, &silc_mpi_comm_world );
    assert( status == MPI_SUCCESS );
}


uint32_t
SILC_Mpi_GetGlobalNumberOfLocations()
{
    assert( SILC_Mpi_IsInitialized() );
    assert( !SILC_Mpi_IsFinalized() );
    int n_local_locations = SILC_Thread_GetNumberOfLocations();
    int sum_of_locations  = 0;
    int root_rank         = 0;
    int n_elements        = 1;
    PMPI_Reduce( &n_local_locations,
                 &sum_of_locations,
                 n_elements,
                 MPI_INT,
                 MPI_SUM,
                 root_rank,
                 silc_mpi_comm_world );
    if ( SILC_Mpi_GetRank() == 0 )
    {
        assert( sum_of_locations > 0 );
    }
    return sum_of_locations;
}

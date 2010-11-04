/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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
 * @file       scorep_mpi_enabled.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */

#include <config.h>
#include "scorep_mpi.h"

#include <scorep_utility/SCOREP_Debug.h>
#include <scorep_thread.h>
#include <scorep_definitions.h>
#include <mpi.h>
#include <assert.h>


static MPI_Comm scorep_mpi_comm_world;


extern void
scorep_status_initialize_mpi();


extern void
scorep_status_initialize_common();


void
SCOREP_Status_Initialize()
{
    scorep_status_initialize_common();
    scorep_status_initialize_mpi();
    // it is too early to call PMPI from here.
}


bool
scorep_create_experiment_dir( char* dirName,
                              int   dirNameSize,
                              void  ( * createDir )( const char* ) )
{
    if ( !SCOREP_Mpi_IsInitialized() )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_MPI, "MPI not initialized, experiment directory creation deferred." );
        return false;
    }

    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        createDir( dirName );
    }
    //MPI_Bcast( dirName, dirNameSize, MPI_CHAR, 0, MPI_COMM_WORLD );

    return true;
}


bool
SCOREP_Mpi_HasMpi()
{
    return true;
}


void
SCOREP_Mpi_GlobalBarrier()
{
    assert( SCOREP_Mpi_IsInitialized() );
    assert( !SCOREP_Mpi_IsFinalized() );
    int status = PMPI_Barrier( scorep_mpi_comm_world );
    assert( status == MPI_SUCCESS );
}


void
SCOREP_Mpi_DuplicateCommWorld()
{
    assert( SCOREP_Mpi_IsInitialized() );
    assert( !SCOREP_Mpi_IsFinalized() );
    int status = PMPI_Comm_dup( MPI_COMM_WORLD, &scorep_mpi_comm_world );
    assert( status == MPI_SUCCESS );
}


int
SCOREP_Mpi_CalculateCommWorldSize()
{
    int size;
    PMPI_Comm_size( scorep_mpi_comm_world, &size );
    return size;
}


int*
SCOREP_Mpi_GatherNumberOfLocationsPerRank()
{
    int* n_locations_per_rank = 0;
    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        n_locations_per_rank = calloc( SCOREP_Mpi_GetCommWorldSize(), sizeof( int ) );
        assert( n_locations_per_rank );
    }
    int n_local_locations = SCOREP_Thread_GetNumberOfLocations();
    PMPI_Gather( &n_local_locations,
                 1,
                 MPI_INT,
                 n_locations_per_rank,
                 1,
                 MPI_INT,
                 0,
                 scorep_mpi_comm_world );
    return n_locations_per_rank;
}


int*
SCOREP_Mpi_GatherNumberOfDefinitionsPerLocation( int* nLocationsPerRank,
                                                 int  nGlobalLocations )
{
    /// @todo refactor
    int* n_definitions_per_location = 0;
    int* diplacements               = 0;
    int  rank                       = 0;
    if ( SCOREP_Mpi_GetRank() == 0 )
    {
        // recv buf
        n_definitions_per_location = calloc( nGlobalLocations, sizeof( int ) );
        assert( n_definitions_per_location );

        // displacements
        diplacements = calloc( SCOREP_Mpi_GetCommWorldSize(), sizeof( int ) );
        assert( diplacements );
        int displacement = 0;
        for ( rank = 0; rank < SCOREP_Mpi_GetCommWorldSize(); ++rank )
        {
            diplacements[ rank ] = displacement;
            displacement        += nLocationsPerRank[ rank ];
        }
    }

    // send buf
    int* n_local_definitions = calloc( SCOREP_Thread_GetNumberOfLocations(), sizeof( int ) );
    assert( n_local_definitions );
    int  number_of_locations = SCOREP_GetNumberOfDefinitions();
    for ( int i = 0; i < SCOREP_Thread_GetNumberOfLocations(); ++i )
    {
        // assign all locations the same number of definitions. This is a temporary solution
        // as we need to duplicate the definitions for every location until OTF2 is able
        // to handle pre-process definitions.
        n_local_definitions[ i ] = number_of_locations;
    }

    PMPI_Gatherv( n_local_definitions,
                  SCOREP_Thread_GetNumberOfLocations(),
                  MPI_INT,
                  n_definitions_per_location,
                  nLocationsPerRank,
                  diplacements,
                  MPI_INT,
                  0,
                  scorep_mpi_comm_world );

    free( n_local_definitions );
    free( diplacements );

    return n_definitions_per_location;
}

static MPI_Datatype
scorep_mpi_to_mpi_datatype( enum SCOREP_Mpi_Datatype scorep_datatype )
{
    switch ( scorep_datatype )
    {
#define SCOREP_MPI_DATATYPE( datatype ) \
    case SCOREP_ ## datatype: \
        return datatype;
        SCOREP_MPI_DATATYPES
#undef SCOREP_MPI_DATATYPE
        default:
            assert( !"Unknown mpi datatype" );
    }
}


int
SCOREP_Mpi_Send( void*               buf,
                 int                 count,
                 SCOREP_Mpi_Datatype scorep_datatype,
                 int                 dest )
{
    return PMPI_Send( buf,
                      count,
                      scorep_mpi_to_mpi_datatype( scorep_datatype ),
                      dest,
                      0,
                      scorep_mpi_comm_world );
}


int
SCOREP_Mpi_Recv( void*               buf,
                 int                 count,
                 SCOREP_Mpi_Datatype scorep_datatype,
                 int                 source,
                 SCOREP_Mpi_Status   status )
{
    return PMPI_Recv( buf,
                      count,
                      scorep_mpi_to_mpi_datatype( scorep_datatype ),
                      source,
                      0,
                      scorep_mpi_comm_world,
                      status == SCOREP_MPI_STATUS_IGNORE
                      ? MPI_STATUS_IGNORE
                      : ( MPI_Status* )status );
}

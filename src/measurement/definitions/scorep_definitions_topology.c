/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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
 * @status     alpha
 * @file       src/measurement/definitions/scorep_definitions_topology.c
 * @maintainer Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Declaration of definition functions to be used by the adapter layer.
 *
 */


#include <config.h>


#include <definitions/SCOREP_Definitions.h>


#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>
#include <inttypes.h>


#include <UTILS_Error.h>
#define SCOREP_DEBUG_MODULE_NAME DEFINITIONS
#include <UTILS_Debug.h>


#include <jenkins_hash.h>


#include <SCOREP_DefinitionHandles.h>
#include <scorep_types.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_Memory.h>


/**
 * Associate a MPI cartesian topology with a process unique topology handle.
 */
SCOREP_MPICartesianTopologyHandle
SCOREP_Definitions_NewMPICartesianTopology( const char*                      topologyName,
                                            SCOREP_InterimCommunicatorHandle communicatorHandle,
                                            uint32_t                         nDimensions,
                                            const uint32_t                   nProcessesPerDimension[],
                                            const uint8_t                    periodicityPerDimension[] )
{
    UTILS_DEBUG_ENTRY( "%s", topologyName );

    SCOREP_Definitions_Lock();

    SCOREP_MPICartesianTopologyDef*   new_definition = NULL;
    SCOREP_MPICartesianTopologyHandle new_handle     = SCOREP_INVALID_CART_TOPOLOGY;

    // Init new_definition
    // see ticket:423
    //UTILS_NOT_YET_IMPLEMENTED();

#ifdef SCOREP_DEBUG
    UTILS_DEBUG( "    Handle ID:  %x", new_definition->sequence_number );
    UTILS_DEBUG( "    Dimensions: %u", nDimensions );

    for ( uint32_t i = 0; i < nDimensions; ++i )
    {
        UTILS_DEBUG( "    Dimension %u:", i );
        UTILS_DEBUG( "        #processes  %u:", nProcessesPerDimension[ i ] );
        UTILS_DEBUG( "        periodicity %hhu:", periodicityPerDimension[ i ] );
    }
#endif

    SCOREP_Definitions_Unlock();

    return new_handle;
}


/////////////////////////////////////////////////////////////////////////////
// MPICartesianCoordsDefinitions ////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


/**
 * Define the coordinates of the current rank in the cartesian topology
 * referenced by @a cartesianTopologyHandle.
 */
void
SCOREP_Definitions_NewMPICartesianCoords( SCOREP_MPICartesianTopologyHandle cartesianTopologyHandle,
                                          uint32_t                          nCoords,
                                          const uint32_t                    coordsOfCurrentRank[] )
{
    UTILS_DEBUG_ENTRY();

    SCOREP_Definitions_Lock();

    SCOREP_MPICartesianCoordsDef*   new_definition = NULL;
    SCOREP_MPICartesianCoordsHandle new_handle     = SCOREP_INVALID_CART_COORDS;

    // Init new_definition
    // see ticket:423
    //UTILS_NOT_YET_IMPLEMENTED();

#ifdef SCOREP_DEBUG
    char stringBuffer[ 16 ];

    UTILS_DEBUG_PREFIX( SCOREP_DEBUG_DEFINITIONS );
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_DEFINITIONS, "    Coordinates:" );
    for ( uint32_t i = 0; i < nCoords; ++i )
    {
        UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_DEFINITIONS,
                                " %u", coordsOfCurrentRank[ i ] );
    }
    UTILS_DEBUG_RAW_PRINTF( SCOREP_DEBUG_DEFINITIONS, "\n" );
#endif

    SCOREP_Definitions_Unlock();
}
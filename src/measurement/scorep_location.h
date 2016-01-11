/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014, 2016,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2015,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013, 2015,
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

#ifndef SCOREP_INTERNAL_LOCATION_H
#define SCOREP_INTERNAL_LOCATION_H


#include <stdint.h>
#include <stdbool.h>
#include <SCOREP_Location.h>
#include <SCOREP_Memory.h>

void
SCOREP_Location_Initialize( void );


/**
 * Call from master thread, e.g. SCOREP_FinalizeMeasurement(). Cleans up
 * locations data structures.
 *
 * Must be called before SCOREP_Thread_Finalize().
 */
void
SCOREP_Location_Finalize( void );


SCOREP_Location*
SCOREP_Location_CreateCPULocation( const char* name );

/**
 * Returns the current number of locations.
 */
uint32_t
SCOREP_Location_GetCountOfLocations( void );


SCOREP_Allocator_PageManager*
SCOREP_Location_GetMemoryPageManager( SCOREP_Location*  locationData,
                                      SCOREP_MemoryType type );


void*
SCOREP_Location_GetSubstrateData( SCOREP_Location* location,
                                  size_t           substrateId );


void
SCOREP_Location_SetSubstrateData( SCOREP_Location* location,
                                  void*            substrateData,
                                  size_t           substrateId );


/*
 * The caller must ensure thread safety. Currently called at measurement
 * time for the current location at flush time, and at finalization time.
 */
void
SCOREP_Location_EnsureGlobalId( SCOREP_Location* location );


void
SCOREP_Location_FinalizeDefinitions( void );


void
SCOREP_Location_FinalizeLocations( void );


#endif /* SCOREP_INTERNAL_LOCATION_H */

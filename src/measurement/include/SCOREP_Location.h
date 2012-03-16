/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2012,
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

#ifndef SCOREP_LOCATION_H
#define SCOREP_LOCATION_H


#include <stddef.h>
#include <stdint.h>

#include "SCOREP_Types.h"
#include "SCOREP_Definitions.h"

/**
 * @file       SCOREP_Location.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 *
 */


typedef struct SCOREP_Location SCOREP_Location;


/**
 * This function can be used by subsystems to create new locations.
 *
 * @param parent            Handle of parent location.
 * @param type              Type of new location.
 * @param name              Name of new location.
 *
 * @return Returns handle for new location.
 */
SCOREP_Location*
SCOREP_Location_CreateNonCPUThreadLocation( SCOREP_Location*    parent,
                                            SCOREP_LocationType type,
                                            const char*         name );


/**
 *
 *
 * @return
 */
SCOREP_Location*
SCOREP_Location_GetCurrentCPUThreadData();

/**
 *  Returns the local id of the location (ie. thread id).
 *
 * @return
 */
uint32_t
SCOREP_Location_GetLocationId( SCOREP_Location* locationData );


/**
 *  Returns the type of the location.
 *
 * @return
 */
SCOREP_LocationType
SCOREP_Location_GetLocationType( SCOREP_Location* locationData );


void*
SCOREP_Location_GetSubsystemLocationData( SCOREP_Location* locationData,
                                          size_t           subsystem_id );


void
SCOREP_Location_SetSubsystemLocationData( SCOREP_Location* locationData,
                                          size_t           subsystem_id,
                                          void*            subsystem_data );


void
SCOREP_Location_ForAllLocations( void ( * cb )( SCOREP_Location*,
                                                void* ),
                                 void* data );


SCOREP_LocationHandle
SCOREP_Location_GetLocationHandle( SCOREP_Location* locationData );

uint64_t
SCOREP_Location_GetLastTimestamp( SCOREP_Location* locationData );


#endif /* SCOREP_LOCATION_H */

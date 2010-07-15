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
#ifndef SILC_DEFINITION_CUBE4_H
#define SILC_DEFINITION_CUBE4_H

/**
 * @file silc_definition_cube4.h
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status ALPHA
 * Declares functions and types needed for writing definitions in Cube 4
 * format.
 */

#include "SILC_Utils.h"

#include <cubew.h>
#include <cube.h>

/**
   Contains the mapping structs for Cube 4 handles to SILC handles.
 */
typedef struct
{
    SILC_Hashtab* region_table_cube;
    SILC_Hashtab* metric_table_cube;
    SILC_Hashtab* callpath_table_cube;
    SILC_Hashtab* region_table_silc;
    SILC_Hashtab* metric_table_silc;
    SILC_Hashtab* callpath_table_silc;
} silc_cube4_definitions_map;

/**
   Writes the definitions to the Cube 4 struct, given by @a my_cube. Fills
   the mapping table given by map.
   @param my_cube Pointer to the cube struct to which the data is written.
   @param map     Pointer to an already inititialized mapping structure.
 */
void
silc_write_definitions_to_cube4( cube_t*                     my_cube,
                                 silc_cube4_definitions_map* map );

/**
   Creates an instance of @ref silc_cube4_definitions_map.
   @param returns a pointer to the newly created instance.
 */
silc_cube4_definitions_map*
silc_cube4_create_definitions_map();

/**
   Deletes the silc_cube4_definitions_map given by @a map.
   @param map Pointer to the map that will be deleted. If it is NULL.
              nothing happens.
 */
void
silc_cube4_delete_definitions_map( silc_cube4_definitions_map* map );

/**
   Adds a region definition pair to the mapping.
   @param map         Pointer to the mapping structure, to which the mapping
                      is added.
   @param cube_handle Pointer to the cube region definition structure.
   @param silc_handle SILC handle for the region.
 */
void
silc_cube4_add_region_mapping( silc_cube4_definitions_map* map,
                               cube_region*                cube_handle,
                               SILC_RegionHandle           silc_handle );

/**
   Adds a callpath definition pair to the mapping.
   @param map         Pointer to the mapping structure, to which the mapping
                      is added.
   @param cube_handle Pointer to the cube cnode defintion structure.
   @param silc_handle SILC handle for the callpath.
 */
void
silc_cube4_add_callpath_mapping( silc_cube4_definitions_map* map,
                                 cube_cnode*                 cube_handle,
                                 SILC_CallpathHandle         silc_handle );

/**
   Adds a metric definition pair to the mapping.
   @param map         Pointer to the mapping structure, to which the mapping
                      is added.
   @param cube_handle Pointer to the cube metric defintion structure.
   @param silc_handle SILC handle for the metric.
 */
void
silc_cube4_add_metric_mapping( silc_cube4_definitions_map* map,
                               cube_metric*                cube_handle,
                               SILC_CounterHandle          silc_handle );

#endif /* SILC_DEFINITION_CUBE4_H */

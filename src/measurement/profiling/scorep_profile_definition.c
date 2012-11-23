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

/**
 * @file scorep_profile_definition.c
 * Implementation of the profile definition.
 *
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <config.h>
#include <stddef.h>
#include <stdio.h>

#include "SCOREP_Memory.h"
#include "SCOREP_Definitions.h"
#include <SCOREP_Config.h>

#include "scorep_profile_definition.h"
#include "scorep_definitions.h"

/*----------------------------------------------------------------------------------------
   Global variables
   -------------------------------------------------------------------------------------*/

/**
    Global profile definition instance
 */
scorep_profile_definition scorep_profile =
{
    NULL,      /* first_root_node      */
    false,     /* has_collapse_node    */
    0,         /* reached_depth        */
    false,     /* is_initialized       */
    false,     /* reinitialize         */
    0,         /* max_callpath_depth   */
    UINT64_MAX /* max_callpath_num     */
};

/*----------------------------------------------------------------------------------------
   Constructors / destructors
   -------------------------------------------------------------------------------------*/

/** Initializes the profile definition struct
 */
void
scorep_profile_init_definition( void )
{
    scorep_profile.is_initialized = true;

    /* Store configuration */
    scorep_profile.has_collapse_node  = false;
    scorep_profile.reached_depth      = 0;
    scorep_profile.max_callpath_depth = scorep_profile_max_callpath_depth;
}

/** Resets the profile definition struct
 */
void
scorep_profile_delete_definition( void )
{
    scorep_profile.is_initialized = false;
    scorep_profile.reinitialize   = true;

    /* Do not reset first_root_node, because in Periscope phases the list of root nodes
       stay alive.
     */
}

/*----------------------------------------------------------------------------------------
   Other functions
   -------------------------------------------------------------------------------------*/

uint64_t
scorep_profile_get_number_of_threads( void )
{
    uint64_t             no_of_threads = 0;
    scorep_profile_node* current       = scorep_profile.first_root_node;

    while ( current != NULL )
    {
        if ( current->node_type == scorep_profile_node_thread_root )
        {
            no_of_threads++;
        }
        current = current->next_sibling;
    }

    return no_of_threads;
}

/*----------------------------------------------------------------------------------------
   Debug
   -------------------------------------------------------------------------------------*/
void
scorep_profile_dump_subtree( scorep_profile_node* node,
                             uint32_t             level )
{
    static char* type_name_map[] = {
        "regular region",
        "paramater string",
        "parameter integer",
        "thread root",
        "thread start"
    };

    if ( node == NULL )
    {
        return;
    }

    printf( "%p ", node );
    for ( int i = 0; i < level; i++ )
    {
        printf( "| " );
    }
    printf( "+ type: %s", type_name_map[ node->node_type ] );
    if ( node->node_type == scorep_profile_node_regular_region )
    {
        printf( "  name: %s", SCOREP_Region_GetName( scorep_profile_type_get_region_handle( node->type_specific_data ) ) );
    }
    printf( "\n" );
    if ( node->first_child != NULL )
    {
        scorep_profile_dump_subtree( node->first_child, level + 1 );
    }
    if ( node->next_sibling != NULL )
    {
        scorep_profile_dump_subtree( node->next_sibling, level );
    }
}

void
scorep_profile_dump( void )
{
    printf( "\n" );
    scorep_profile_dump_subtree( scorep_profile.first_root_node, 0 );
    printf( "\n" );
}

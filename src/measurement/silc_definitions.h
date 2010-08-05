#ifndef SILC_INTERNAL_DEFINITIONS_H
#define SILC_INTERNAL_DEFINITIONS_H

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
 * @file       silc_definitions.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <SILC_DefinitionHandles.h>
#include <SILC_PublicTypes.h>
#include "silc_definition_structs.h"
#include <SILC_Memory.h>

#include <jenkins_hash.h>


/**
 * Allocate, assign the sequence number, and store in manager list a new
 * definition of type @type
 */
/* *INDENT-OFF* */
#define SILC_ALLOC_NEW_DEFINITION( Type, type ) \
    do { \
        new_handle = SILC_Memory_AllocForDefinitions( \
            sizeof( SILC_ ## Type ## _Definition ) ); \
        new_definition = \
            SILC_MEMORY_DEREF_MOVABLE( new_handle, \
                                       SILC_ ## Type ## _Definition* ); \
        new_definition->next = SILC_MOVABLE_NULL; \
        new_definition->hash_value = 0; \
        *silc_definition_manager.type ## _definition_tail_pointer = \
            new_handle; \
        silc_definition_manager.type ## _definition_tail_pointer = \
            &new_definition->next; \
        new_definition->sequence_number = \
            silc_definition_manager.type ## _definition_counter++; \
    } while ( 0 )
/* *INDENT-ON* */

/**
 * Allocate, assign the sequence number, and store in manager list a new
 * definition of type @type with a variable array member of type @array_type
 * and a total number of members of @number_of_members
 */
/* *INDENT-OFF* */
#define SILC_ALLOC_NEW_DEFINITION_VARIABLE_ARRAY( Type, \
                                                  type, \
                                                  array_type, \
                                                  number_of_members ) \
    do { \
        new_handle = SILC_Memory_AllocForDefinitions( \
            sizeof( SILC_ ## Type ## _Definition ) + \
            ( ( number_of_members ) - 1 ) * sizeof( array_type ) ); \
        new_definition = \
            SILC_MEMORY_DEREF_MOVABLE( new_handle, \
                                       SILC_ ## Type ## _Definition* ); \
        new_definition->next = SILC_MOVABLE_NULL; \
        new_definition->hash_value = 0; \
        *silc_definition_manager.type ## _definition_tail_pointer = \
            new_handle; \
        silc_definition_manager.type ## _definition_tail_pointer = \
            &new_definition->next; \
        new_definition->sequence_number = \
            silc_definition_manager.type ## _definition_counter++; \
    } while ( 0 )
/* *INDENT-ON* */

/* this size is temporary */
#define SILC_DEFINITION_HASH_TABLE_SIZE hashsize( 8 )
#define SILC_DEFINITION_HASH_TABLE_MASK hashmask( 8 )

/**
 * Holds all definitions.
 *
 * Not all members of this struct needs to be valid, if it will be moved
 * to a remote process.
 */
/* *INDENT-OFF* */
typedef struct SILC_DefinitionManager SILC_DefinitionManager;
struct SILC_DefinitionManager
{
    /* note: no ';' */
    #define SILC_DEFINE_DEFINITION_LIST( Type, type ) \
        SILC_ ## Type ## Handle  type ## _definition_head; \
        SILC_ ## Type ## Handle* type ## _definition_tail_pointer; \
        uint32_t                              type ## _definition_counter;

    SILC_DEFINE_DEFINITION_LIST( String, string )
    SILC_StringHandle* string_definition_hash_table;

    SILC_DEFINE_DEFINITION_LIST( Location, location )
    SILC_DEFINE_DEFINITION_LIST( SourceFile, source_file )
    SILC_DEFINE_DEFINITION_LIST( Region, region )
    SILC_DEFINE_DEFINITION_LIST( Group, group )
    SILC_DEFINE_DEFINITION_LIST( MPIWindow, mpi_window )
    SILC_DEFINE_DEFINITION_LIST( MPICartesianTopology, mpi_cartesian_topology )
    SILC_DEFINE_DEFINITION_LIST( MPICartesianCoords, mpi_cartesian_coords )
    SILC_DEFINE_DEFINITION_LIST( CounterGroup, counter_group )
    SILC_DEFINE_DEFINITION_LIST( Counter, counter )
    SILC_DEFINE_DEFINITION_LIST( IOFileGroup, io_file_group )
    SILC_DEFINE_DEFINITION_LIST( IOFile, io_file )
    SILC_DEFINE_DEFINITION_LIST( MarkerGroup, marker_group )
    SILC_DEFINE_DEFINITION_LIST( Marker, marker )
    SILC_DEFINE_DEFINITION_LIST( Parameter, parameter )
    SILC_DEFINE_DEFINITION_LIST( Callpath, callpath )

    #undef SILC_DEFINE_DEFINITION_LIST
};
/* *INDENT-ON* */

/**
 * Iterator functions for definition. The iterator variable is named
 * @definition.
 *
 * Example:
 * @code
 *  SILC_DEFINITION_FOREACH_DO( &silc_definition_manager, String, string )
 *  {
 *      :
 *      definition->member = ...
 *      :
 *  }
 *  SILC_DEFINITION_FOREACH_WHILE();
 * @endcode
 */
/* *INDENT-OFF* */
#define SILC_DEFINITION_FOREACH_DO( manager_pointer, Type, type ) \
    do { \
        SILC_ ## Type ## _Definition* definition; \
        SILC_ ## Type ## Handle handle; \
        for ( handle = ( manager_pointer )->type ## _definition_head; \
              handle != SILC_MOVABLE_NULL; \
              handle = definition->next ) \
        { \
            definition = SILC_MEMORY_DEREF_MOVABLE( \
                handle, SILC_ ## Type ## _Definition* ); \
            {

#define SILC_DEFINITION_FOREACH_WHILE() \
            } \
        } \
    } while ( 0 )
/* *INDENT-ON* */


/* Some convenient macros to add members or sub-hashes to the hash value */
#define HASH_ADD_POD( pod_member ) \
    new_definition->hash_value = hash( &new_definition->pod_member, \
                                       sizeof( new_definition->pod_member ), \
                                       new_definition->hash_value )

#define HASH_ADD_HANDLE( handle_member, Type ) \
    new_definition->hash_value = hashword( \
        &SILC_HANDLE_GET_HASH( new_definition->handle_member, Type ), \
        1, new_definition->hash_value )

#define HASH_ADD_ARRAY( array_member, number_member ) \
    new_definition->hash_value = hash( \
        new_definition->array_member, \
        sizeof( new_definition->array_member[ 0 ] ) \
        * new_definition->number_member, \
        new_definition->hash_value )


void
SILC_Definitions_Initialize();


void
SILC_Definitions_Finalize();


void
SILC_Definitions_Write();


SILC_StringHandle
SILC_DefineString( const char* str );


SILC_LocationHandle
SILC_DefineLocation( uint64_t    globalLocationId,
                     const char* name );


SILC_CallpathHandle
SILC_DefineCallpath( SILC_CallpathHandle parent,
                     SILC_RegionHandle   region );


SILC_CallpathHandle
SILC_DefineCallpathParameterInteger( SILC_CallpathHandle  parent,
                                     SILC_ParameterHandle param,
                                     int64_t              value );


SILC_CallpathHandle
SILC_DefineCallpathParameterString( SILC_CallpathHandle  parent,
                                    SILC_ParameterHandle param,
                                    SILC_StringHandle    value );


int
SILC_GetNumberOfDefinitions();


#endif /* SILC_INTERNAL_DEFINITIONS_H */

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

#ifndef SCOREP_MEMORY_H
#define SCOREP_MEMORY_H




/**
 * @file SCOREP_Memory.h Memory management interface for measurement and
 * adapters.
 *
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 * @todo add comments about synchronization needs
 */


#include <scorep_utility/SCOREP_Allocator.h>
#include <stddef.h>


/**
 * Types of memory pages a location has.
 *
 */
typedef enum SCOREP_MemoryType
{
    SCOREP_MEMORY_TYPE_MISC = 0,

    /** separate because we might clear them for periscope from time to time */
    SCOREP_MEMORY_TYPE_PROFILING,

    /** For the OTF2 chunks, will be cleared after a flush */
    SCOREP_MEMORY_TYPE_TRACING,

    SCOREP_NUMBER_OF_MEMORY_TYPES
} SCOREP_MemoryType;


/**
 * Initialize the memory system for the measurement core, the adapters and the
 * online interface, i.e. everything except otf2.
 *
 * @param totalMemory total amount of memory in bytes the measurement system
 * should use.
 * @param pageSize size of a single page in bytes, needs to be <= @a totalMemory
 */
void
SCOREP_Memory_Initialize( size_t totalMemory,
                          size_t pageSize );


/**
 * Informs the user of an out of memory situation and aborts the measurement.
 */
void
SCOREP_Memory_HandleOutOfMemory( void );

/**
 * Clean up the memory system.
 */
void
SCOREP_Memory_Finalize();


/**
 * If we encounter a new thread, we need to add page managers for all page
 * types to the newly created thread local data structures. But only here we
 * know how many and from which allocator. The page manager array needs to have
 * a length of SCOREP_NUMBER_OF_MEMORY_TYPES.
 *
 * Aborts, if creation fails.
 *
 * @see SCOREP_Thread.h
 *
 * @return
 */
void
SCOREP_Memory_CreatePageManagers( SCOREP_Allocator_PageManager** pageManagers );


void
SCOREP_Memory_DeletePageManagers( SCOREP_Allocator_PageManager** pageManagers );


/**
 * @name Memory allocation functions for the measurement core and the adapters.
 *
 * These functions are the replacement of malloc and free. Note that there is
 * no possibility to free a single allocation but only to free the entire
 * allocated memory of a specific type. Due to the usual memory access
 * patterns during measurement this design is hopefully justified.
 *
 * We provide specific functions for different types of memory (like summary,
 * definitions, etc.) instead of a generic interface (that accepts e.g. a enum
 * to specify the type) to be more flexible and performant during
 * implementation (e.g. the summary needs a multithreaded implementation
 * whereas the definition memory is per process).
 *
 * We do not provide a memory interface for otf2 as it may not be possible to
 * harmonize the parameters for the paged allocator.
 *
 */
/*@{*/

/**
 * The malloc function for miscellaneous data. It reserves a
 * contiguous memory block whose size in bytes it at least @a size. The
 * contents of the memory block is undetermined.
 *
 * @param size The size of the requested memory block in bytes. @a size == 0
 * leads to undefined behaviour.
 *
 * @return The address of the first byte in the memory block allocated, or
 *         aborts if the request could not handled.
 *
 * @see SCOREP_Memory_FreeMiscMem()
 */
void*
SCOREP_Memory_AllocForMisc( size_t size );


/**
 * Release the entire allocated memory for miscellaneous stuff.
 *
 * @see SCOREP_Memory_AllocForMisc()
 */
void
SCOREP_Memory_FreeMiscMem();


/**
 * The malloc function for runtime summarization data (e.g. calltree nodes
 * etc.). It reserves a contiguous memory block whose size in bytes it at
 * least @a size. The contents of the memory block is undetermined.
 *
 * @param size The size of the requested memory block in bytes. @a size == 0
 * leads to undefined behaviour.
 *
 * @return The address of the first byte in the memory block allocated, or
 *         aborts if the request could not handled.
 *
 * @see SCOREP_Memory_FreeSummaryMem()
 */
void*
SCOREP_Memory_AllocForProfile( size_t size );


/**
 * Release the entire allocated runtime summarization data memory.
 *
 * @see SCOREP_Memory_AllocForSummary()
 */
void
SCOREP_Memory_FreeProfileMem();


/**
 * The malloc function for tracing data ie trace buffers.
 *
 * @param size The size of the requested memory block in bytes.
 *
 * @return The address of the first byte in the memory block allocated, or a
 * null pointer if the requested memory is not available.
 *
 * @see SCOREP_Memory_FreeTracingMem()
 */
void*
SCOREP_Memory_AllocForTracing( size_t size );


/**
 * Release the entire allocated memory for tracing stuff.
 *
 * @see SCOREP_Memory_AllocForTracing()
 */
void
SCOREP_Memory_FreeTracingMem();


/**
 * The malloc function for definition data. It reserves a contiguous memory
 * block whose size in bytes it at least @a size. The contents of the memory
 * block is undetermined.
 *
 * @param size The size of the requested memory block in bytes. @a size == 0
 * leads to undefined behaviour.
 *
 * @return A pseudo pointer of type SCOREP_Memory_DefinitionMemory object or a null
 * pointer if the memory requested is not available.
 *
 * @note You can obtain the real address of the memory and cast it to the
 * desired type with the help of the macro SCOREP_MEMORY_DEREF_MOVABLE.
 *
 * @see SCOREP_Memory_FreeDefinitionMem()
 */
SCOREP_Allocator_MovableMemory
SCOREP_Memory_AllocForDefinitions( size_t size );

/**
 * Release the entire allocated definition memory.
 *
 * @see SCOREP_Memory_AllocForDefinitions()
 */
void
SCOREP_Memory_FreeDefinitionMem();


void*
SCOREP_Memory_GetAddressFromMovableMemory( SCOREP_Allocator_MovableMemory movableMemory,
                                           SCOREP_Allocator_PageManager*  movablePageManager );

SCOREP_Allocator_PageManager*
SCOREP_Memory_CreateMovedPagedMemory( void );


SCOREP_Allocator_PageManager*
SCOREP_Memory_GetLocalDefinitionPageManager();


/*@}*/

#endif /* SCOREP_MEMORY_H */

/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2019-2021,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license. See the COPYING file in the package base
 * directory for details.
 *
 */

#include <SCOREP_Atomic.h>
#include <SCOREP_Mutex.h>
#include <SCOREP_ReaderWriterLock.h>

#include <stdint.h>
#include <stdbool.h>
#if HAVE( STDALIGN_H )
#include <stdalign.h>
#endif


/*
   SCOREP_HASH_TABLE_MONOTONIC( prefix, nPairsPerChunk, hashTableSize ) and
   SCOREP_HASH_TABLE_NON_MONOTONIC( prefix, nPairsPerChunk, hashTableSize )
   are thread-safe hash table templates that lock per bucket, if necessary.
   Locks for distinct buckets reside in different cachelines. A bucket
   uses chained chunks of nPairsPerChunk keys and values for conflict
   resolution. An instantiation requires the user to provide:

   @a prefix, a compilation-unit unique name that is used to prefix
   functions, variables and types.
   @a nPairsPerChunk, an positive integer specifying the size of the
   key-array and the value-array a chunk is composed of. In addition a
   chunk, holds a next pointer.
   @a hashTableSize, the number of buckets the table should hold.

   In addition the user needs to provide helper functions and
   typedefs. Care was taken to minimize locking: an arbitrary number
   of 'getters' can run concurrently, even if an insert operation
   takes place. Remove operations block until pending getters and
   inserters have finished.

   The monotonic version has no remove operation.

   Before instantiating a template, key and value types need to be provided:
   typedef <existing_type> <prefix>_key_t;
   typedef <existing_type> <prefix>_value_t;
   where <existing_type> is either a pointer, an integral type or a
   struct; if you use a struct, beware that objects get copied.

   For performance reasons, insert operations are modeled using
   a get_and_insert function that is provided by an
   instantiation: <prefix>_get_and_insert().

   The bucket is determined by calling the user-provided
   <prefix>_hash(). For searching, the user-provided <prefix>_equals()
   is called. If an item wasn't found, it will be inserted utilizing
   the the user-provided function <prefix>_value_ctor() and
   potentially the user-provided <prefix>_allocate_chunk().
   The provided remove operations <prefix>_remove() and
   <prefix>_remove_if() call the user-provided <prefix>_value_dtor()
   if a key-value pair is to be removed.

   A template instantiation provides the functions
   <prefix>_iterate_key_value_pairs() and <prefix>_free_chunks() that
   require user-provided functions <prefix>_iterate_key_value_pair()
   and <prefix>_free_chunk(). These functions are supposed to be used
   in a serial context only.

   All types and functions names are prefixed with <prefix>, so you
   might instantiate more than one table per compilation unit. All
   locking is done using atomic operations.

   With nPairsPerChunk you determine the size of a chunk which is
   nPairsPerChunk * ( sizeof(<prefix>_key_t) +
   sizeof(<prefix>_value_t) ) + sizeof( void* ). You might consider
   aligned storage and filling at least one cacheline.


   SCOREP_HASH_TABLE_MONOTONIC( prefix, nPairsPerChunk, hashTableSize )
   --------------------------------------------------------------------

   A hash table without remove operation.

   An instantiation of SCOREP_HASH_TABLE_MONOTONIC provides following
   functions:

   // <prefix>_get() will return true if @a key was found in the hash
   // table, which is checked using the function <prefix>_equals().
   // In this case @a *value will be assigned the hash table's value
   // corresponding to key. The function <prefix>_hash() determines
   // the table-bucket used in searching @a key. If @a key is not in
   // the table, false is returned and @a *value stays untouched. An
   // arbitrary number of get operations can run concurrently, even
   // together with get_and_insert operations.
   static inline bool
   <prefix>_get( <prefix>_key_t key,
                 <prefix>_value_t* value );

   // <prefix>_get_and_insert() will return the <prefix>_value_t that
   // corresponds to @a key. The function <prefix>_hash() determines
   // the table-bucket used in searching key. If @a key is not already
   // in the table, which is checked using the function
   // <prefix>_equals(), a new key-value pair will be added where the
   // value is provided by the function <prefix>_value_ctor(), using
   // @a ctorData.  If the table needs to allocate new internal data
   // during insert, it calls <prefix>_allocate_chunk(). @a inserted
   // indicates if the a new key-values pair was inserted.
   static inline <prefix>_value_t
   <prefix>_get_and_insert( <prefix>_key_t key,
                            const void* ctorData,
                            bool* inserted );

   // <prefix>_iterate_key_value_pairs() will iterate over the entire
   // table and call <prefix>_iterate_key_value_pair() for each
   // key-value pair. This function is supposed to be used in a serial
   // context only.
   static void
   <prefix>_iterate_key_value_pairs( void );

   // <prefix>_free_chunks() iterates over all chunks of the table and
   // calls <prefix>_free_chunk() for each chunk. Afterwards, the
   // table will be empty. This function is supposed to be used in a
   // serial context only.
   static void
   <prefix>_free_chunks( void );

   The functions mentioned in the documentation above need to be
   provided by the user, in addition to the key and value type
   typedefs (see complete list below). Note that the dynamic memory
   allocation and deallocation of chunks and values is in the
   responsibility of the user. Deallocate values, if necessary, using
   <prefix>_iterate_key_value_pairs().  Deallocate chunks, if
   necessary, after deallocating values by calling
   <prefix>_free_chunks().

   The table itself is declared static.

   Here the list of typedefs and functions that need to be
   declared/defined before instantiating SCOREP_HASH_TABLE_MONOTONIC:

   typedef <existing_type> <prefix>_key_t;
   typedef <existing_type> <prefix>_value_t;
   uint32_t <prefix>_hash( <prefix>_key_t key ); // consider inlining
   bool <prefix>_equals( <prefix>_key_t key1, <prefix>_key_t key2 ); // consider inlining
   void* <prefix>_allocate_chunk( size_t chunkSize ); // consider cacheline-size alignment
   <prefix>_value_t <prefix>_value_ctor( <prefix>_key_t key, void* ctorData );
   void <prefix>_iterate_key_value_pair( <prefix>_key_t key, <prefix>_value_t value );
   void <prefix>_free_chunk( void* chunk );



   SCOREP_HASH_TABLE_NON_MONOTONIC( prefix, nPairsPerChunk, hashTableSize )
   ------------------------------------------------------------------------

   Similar to SCOREP_HASH_TABLE_MONOTONIC, but in addition, provides
   remove operations.  The remove operations will wait for
   get_and_insert operations to finish. When removal takes place, new
   get_and_insert operations will wait until removal is complete. When
   a remove operation leads to an empty chunk, store this chunk in a
   hash table specific free-list for later reuse.

   An instantiation of SCOREP_HASH_TABLE_NON_MONOTONIC provides
   everything from SCOREP_HASH_TABLE_MONOTONIC and in addition:

   // <prefix>_remove() will remove the key-value pair (if any)
   // corresponding to @a key from the table after calling
   // <prefix>_value_dtor(). If key is found, return true.
   static inline bool
   <prefix>_remove( <prefix>_key_t key );

   // A function of following type needs to be provided to
   // <prefix>_remove_if().
   typedef bool ( *<prefix>_condition_t )( <prefix>_key_t,
                                           <prefix>_value_t,
                                           void* data );

   // <prefix>_remove_if() will iterate over the entire hash table and
   // call @a condition() for every key-value pair, providing @a data
   // as third parameter. If condition() returns true, the
   // corresponding key-value pair is removed and
   // <prefix>_value_dtor() is called.
   static inline void
   <prefix>_remove_if( <prefix>_condition_t condition, void* data );

   In addition to the functions and types required for
   SCOREP_HASH_TABLE_MONOTONIC, the following needs to be provided
   before instantiation:

   static void <prefix>_value_dtor( <prefix>_key_t key, <prefix>_value_t value );

 */

/* *INDENT-OFF* */


/* implementation detail, do not use directly */
#define SCOREP_HASH_TABLE_MONOTONIC_BUCKET( prefix ) \
    typedef struct prefix ## _chunk_t prefix ##  _chunk_t; \
    typedef struct prefix ## _bucket_t prefix ## _bucket_t; \
    struct prefix ## _bucket_t \
    { \
        SCOREP_ALIGNAS( SCOREP_CACHELINESIZE ) uint32_t size;	\
        SCOREP_Mutex                                    insert_lock; \
        prefix ## _chunk_t*                             chunk; \
    };


/* implementation detail, do not use directly */
#define SCOREP_HASH_TABLE_NON_MONOTONIC_BUCKET( prefix ) \
    typedef struct prefix ## _chunk_t prefix ##  _chunk_t; \
    typedef struct prefix ## _bucket_t prefix ## _bucket_t; \
    struct prefix ## _bucket_t \
    { \
        SCOREP_ALIGNAS( SCOREP_CACHELINESIZE ) uint32_t size; \
        SCOREP_Mutex                                    insert_lock; \
        SCOREP_Mutex                                    remove_lock; \
        int16_t                                         pending; \
        int16_t                                         departing; \
        int16_t                                         release_n_readers; \
        int16_t                                         release_writer; \
        prefix ## _chunk_t*                             chunk; \
    };


/* implementation detail, do not use directly */
#define SCOREP_HASH_TABLE_GET( prefix, nPairsPerChunk ) \
    uint32_t i                 = 0; \
    uint32_t j                 = 0; \
    uint32_t current_size      = SCOREP_Atomic_LoadN_uint32( &( bucket->size ), SCOREP_ATOMIC_SEQUENTIAL_CONSISTENT ); \
    prefix ## _chunk_t** chunk = &( bucket->chunk ); \
    uint32_t old_size; \
    /* search until end of chunks */ \
    do \
    { \
        for (; i < current_size; ++i, ++j ) \
        { \
            if ( j == ( nPairsPerChunk ) ) \
            { \
                chunk = &( ( *chunk )->next ); \
                j     = 0; \
            } \
            if ( prefix ## _equals( key, ( *chunk )->keys[ j ] ) ) \
            { \
                *inserted = false; \
                return ( *chunk )->values[ j ]; \
            } \
        } \
        old_size     = current_size; \
        current_size = SCOREP_Atomic_LoadN_uint32( &( bucket->size ), SCOREP_ATOMIC_SEQUENTIAL_CONSISTENT ); \
    } \
    while ( current_size > old_size );  \
    /* not found, search again while waiting for 'insert_lock' */ \
    while ( true ) \
    { \
        if ( SCOREP_MutexTrylock( &( bucket->insert_lock ) ) == true ) \
        { \
            break; \
        } \
        else \
        { \
            current_size = SCOREP_Atomic_LoadN_uint32( &( bucket->size ), SCOREP_ATOMIC_SEQUENTIAL_CONSISTENT ); \
            if ( current_size > old_size ) \
            { \
                for (; i < current_size; ++i, ++j ) \
                { \
                    if ( j == ( nPairsPerChunk ) ) \
                    { \
                        chunk = &( ( *chunk )->next ); \
                        j     = 0; \
                    } \
                    if ( prefix ## _equals( key, ( *chunk )->keys[ j ] ) ) \
                    { \
                        *inserted = false; \
                        return ( *chunk )->values[ j ]; \
                    } \
                } \
                old_size = current_size; \
            } \
        } \
    }


/* implementation detail, do not use directly */
#define SCOREP_HASH_TABLE_INSERT_1( prefix, nPairsPerChunk ) \
    /* 'insert_lock' acquired: search again, inserts might have taken place in between */ \
    current_size = SCOREP_Atomic_LoadN_uint32( &( bucket->size ), SCOREP_ATOMIC_SEQUENTIAL_CONSISTENT ); \
    for (; i < current_size; ++i, ++j ) \
    { \
        if ( j == ( nPairsPerChunk ) ) \
        { \
            chunk = &( ( *chunk )->next ); \
            j     = 0; \
        } \
        if ( prefix ## _equals( key, ( *chunk )->keys[ j ] ) ) \
        { \
            SCOREP_MutexUnlock( &( bucket->insert_lock ) ); \
            *inserted = false; \
            return ( *chunk )->values[ j ]; \
        } \
    }


/* implementation detail, do not use directly */
#define SCOREP_HASH_TABLE_NEW_CHUNK_MONOTONIC( prefix, nPairsPerChunk ) \
    if ( current_size == 0 ) \
    { \
        bucket->chunk       = prefix ## _allocate_chunk( sizeof( prefix ## _chunk_t ) ); \
        bucket->chunk->next = NULL; \
        chunk               = &( bucket->chunk ); \
    } \
    else if ( j == ( nPairsPerChunk ) ) \
    { \
        ( *chunk )->next       = prefix ## _allocate_chunk( sizeof( prefix ## _chunk_t ) ); \
        ( *chunk )->next->next = NULL; \
        chunk                  = &( ( *chunk )->next ); \
        j                      = 0; \
    }


/* implementation detail, do not use directly */
#define SCOREP_HASH_TABLE_NEW_CHUNK_NON_MONOTONIC( prefix, nPairsPerChunk ) \
    if ( current_size == 0 ) \
    { \
        bucket->chunk       = prefix ## _allocate_chunk( sizeof( prefix ## _chunk_t ) ); \
        bucket->chunk->next = NULL; \
        chunk               = &( bucket->chunk ); \
    } \
    else if ( j == ( nPairsPerChunk ) ) \
    { \
        SCOREP_MutexLock( &( prefix ## _chunk_free_list_lock ) ); \
        if ( prefix ## _chunk_free_list != NULL ) \
        { \
            ( *chunk )->next = prefix ## _chunk_free_list; \
            prefix ## _chunk_free_list = prefix ## _chunk_free_list->next; \
            SCOREP_MutexUnlock( &( prefix ## _chunk_free_list_lock ) ); \
        } \
        else \
        { \
            SCOREP_MutexUnlock( &( prefix ## _chunk_free_list_lock ) ); \
            ( *chunk )->next = prefix ## _allocate_chunk( sizeof( prefix ## _chunk_t ) ); \
        } \
        ( *chunk )->next->next = NULL; \
        chunk                  = &( ( *chunk )->next ); \
        j                      = 0; \
    }


/* implementation detail, do not use directly */
#define SCOREP_HASH_TABLE_INSERT_2( prefix ) \
    /* create new value_t and insert */ \
    ( *chunk )->keys[ j ]   = key; \
    ( *chunk )->values[ j ] = prefix ## _value_ctor( key, ctorData ); \
    SCOREP_Atomic_StoreN_uint32( &( bucket->size ), current_size + 1, SCOREP_ATOMIC_SEQUENTIAL_CONSISTENT ); \
    /* unlock */ \
    SCOREP_MutexUnlock( &( bucket->insert_lock ) ); \
    *inserted = true; \
    return ( *chunk )->values[ j ];


/* implementation detail, do not use directly */
#define SCOREP_HASH_TABLE_COMMON( prefix, nPairsPerChunk, hashTableSize ) \
\
    struct prefix ## _chunk_t \
    { \
        prefix ## _key_t    keys[ ( nPairsPerChunk ) ]; \
        prefix ## _value_t  values[ ( nPairsPerChunk ) ]; \
        prefix ## _chunk_t* next; \
    }; \
\
    static prefix ## _bucket_t prefix ## _hash_table[ ( hashTableSize ) ]; \
\
    /* Do not call concurrently */ \
    static void \
    prefix ## _iterate_key_value_pairs( void ) \
    { \
        for ( uint32_t b = 0; b < ( hashTableSize ); ++b ) \
        { \
            prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ b ] ); \
            prefix ## _chunk_t* chunk   = bucket->chunk; \
            uint32_t i                  = 0; \
            uint32_t current_size       = bucket->size; \
            while ( chunk != NULL ) \
            { \
                for ( int j = 0; i < current_size && j < ( nPairsPerChunk ); ++i, ++j ) \
                { \
                    prefix ## _iterate_key_value_pair( chunk->keys[ j ], \
                                                       chunk->values[ j ] ); \
                } \
                chunk = chunk->next; \
            } \
        } \
    } \
\
    /* implementation detail, do not use directly */ \
    static inline prefix ## _value_t \
    prefix ## _get_impl( prefix ## _key_t key, \
                         bool* inserted, \
                         prefix ## _bucket_t* bucket ) \
    { \
        SCOREP_HASH_TABLE_GET( prefix, nPairsPerChunk ) /* might return */ \
        *inserted = true; /* i.e. not found */ \
        prefix ## _value_t dummy; \
        return dummy; \
    }

/* implementation detail, do not use directly */
#define SCOREP_HASH_TABLE_FREE_CHUNKS( prefix, hashTableSize ) \
    for ( uint32_t b = 0; b < ( hashTableSize ); ++b ) \
    { \
        prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ b ] ); \
        prefix ## _chunk_t* chunk   = bucket->chunk; \
        while ( chunk != NULL ) \
        { \
            prefix ## _chunk_t* next = chunk->next; \
            prefix ## _free_chunk( chunk ); \
            chunk = next; \
        } \
        bucket->chunk = NULL; \
        bucket->size  = 0; \
    }


/*
   SCOREP_HASH_TABLE_MONOTONIC( prefix, nPairsPerChunk, hashTableSize )
   see documentation above
 */
#define SCOREP_HASH_TABLE_MONOTONIC( prefix, nPairsPerChunk, hashTableSize ) \
    SCOREP_HASH_TABLE_MONOTONIC_BUCKET( prefix ) \
    SCOREP_HASH_TABLE_COMMON( prefix, nPairsPerChunk, hashTableSize ) \
\
    /* implementation detail, do not use directly */ \
    static inline prefix ## _value_t \
    prefix ## _get_and_insert_impl( prefix ## _key_t key, \
                                    const void* ctorData, \
                                    bool* inserted, \
                                    prefix ## _bucket_t* bucket ) \
    { \
        SCOREP_HASH_TABLE_GET( prefix, nPairsPerChunk ) \
        SCOREP_HASH_TABLE_INSERT_1( prefix, nPairsPerChunk ) \
        SCOREP_HASH_TABLE_NEW_CHUNK_MONOTONIC( prefix, nPairsPerChunk ) \
        SCOREP_HASH_TABLE_INSERT_2( prefix ) \
    } \
\
    static inline bool /* found */ \
    prefix ## _get( prefix ## _key_t key, \
                    prefix ## _value_t* value ) \
    { \
        prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ prefix ## _get_hash( key ) ] ); \
        bool inserted; \
        prefix ## _value_t ret_val = prefix ## _get_impl( key, &inserted, bucket ); \
        if ( !inserted ) \
        { \
            *value = ret_val; \
            return true; \
        } \
        return false; \
    } \
\
    static inline prefix ## _value_t \
    prefix ## _get_and_insert( prefix ## _key_t key, \
                               const void* ctorData, \
                               bool* inserted ) \
    { \
        prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ prefix ## _get_hash( key ) ] ); \
        return prefix ## _get_and_insert_impl( key, ctorData, inserted, bucket ); \
    } \
\
    /* Do not call concurrently */ \
    static void \
    prefix ## _free_chunks( void ) \
    { \
        SCOREP_HASH_TABLE_FREE_CHUNKS( prefix, hashTableSize ) \
    }


/* implementation detail, do not use directly */
#define SCOREP_HASH_TABLE_MOVE_LAST_TO_REMOVED( prefix, nPairsPerChunk ) \
    /* move last key-value pair to removed key-value pair's slot, decrement size: */ \
    /*   go to first element of current chunk */ \
    i -= j; \
    /*   traverse to last chunk; by then, (i,j=0) refers to the first element of the last chunk. */ \
    while ( chunk->next != NULL ) \
    { \
        previous_chunk = chunk; \
        chunk          = chunk->next; \
        i             += ( nPairsPerChunk ); \
    } \
    /*   go to last used element of last chunk */ \
    j = current_size - i - 1; \
    /*   move key-value pair, decrement size, release empty chunk to free list */ \
    *free_key   = chunk->keys[ j ]; \
    *free_value = chunk->values[ j ]; \
    if ( j == 0 ) /* sole pair of last chunk was moved, chunk is now empty */ \
    { \
        if ( previous_chunk == NULL ) \
        { \
            bucket->chunk = NULL; \
        } \
        else \
        { \
            previous_chunk->next = NULL; \
        } \
        /* lock chunk_free_list as it is per hash table, not per bucket */ \
        SCOREP_MutexLock( &( prefix ## _chunk_free_list_lock ) ); \
        chunk->next = prefix ## _chunk_free_list; \
        prefix ## _chunk_free_list = chunk; \
        SCOREP_MutexUnlock( &( prefix ## _chunk_free_list_lock ) ); \
    } \
    SCOREP_Atomic_StoreN_uint32( &( bucket->size ), --current_size, SCOREP_ATOMIC_SEQUENTIAL_CONSISTENT );

/*
   SCOREP_HASH_TABLE_NON_MONOTONIC( prefix, nPairsPerChunk, hashTableSize )
   see documentation above
 */
#define SCOREP_HASH_TABLE_NON_MONOTONIC( prefix, nPairsPerChunk, hashTableSize ) \
    struct prefix ## _chunk_t; \
    /* _chunk_free_list handling could be done with a lock-free stack implementation. */ \
    /* This implementation needs to deal with the ABA problem and a potential counter */ \
    /* overflow. As an implementation doesn't exist and there is no evidence that it */ \
    /* performs better than a SCOREP_Mutex in real world scenarios, use a mutex for now. */ \
    static struct prefix ## _chunk_t* prefix ## _chunk_free_list; \
    static SCOREP_Mutex               prefix ## _chunk_free_list_lock; \
    SCOREP_HASH_TABLE_NON_MONOTONIC_BUCKET( prefix ) \
    SCOREP_HASH_TABLE_COMMON( prefix, nPairsPerChunk, hashTableSize ) \
\
    /* implementation detail, do not use directly */ \
    static inline prefix ## _value_t \
    prefix ## _get_and_insert_impl( prefix ## _key_t key, \
                                    const void* ctorData, \
                                    bool* inserted, \
                                    prefix ## _bucket_t* bucket ) \
    { \
        SCOREP_HASH_TABLE_GET( prefix, nPairsPerChunk ) \
        SCOREP_HASH_TABLE_INSERT_1( prefix, nPairsPerChunk ) \
        SCOREP_HASH_TABLE_NEW_CHUNK_NON_MONOTONIC( prefix, nPairsPerChunk ) \
        SCOREP_HASH_TABLE_INSERT_2( prefix ) \
    } \
\
    static inline bool /* found */ \
    prefix ## _get( prefix ## _key_t key, \
                    prefix ## _value_t* value ) \
    { \
        prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ prefix ## _get_hash( key ) ] ); \
        bool inserted; \
        SCOREP_RWLock_ReaderLock( &( bucket->pending ), &( bucket->release_n_readers ) ); \
        prefix ## _value_t ret_val = prefix ## _get_impl( key, &inserted, bucket ); \
        SCOREP_RWLock_ReaderUnlock( &( bucket->pending ), &( bucket->departing ), &( bucket->release_writer ) ); \
        if ( !inserted ) \
        { \
            *value = ret_val; \
            return true; \
        } \
        return false; \
    } \
\
    static inline prefix ## _value_t \
    prefix ## _get_and_insert( prefix ## _key_t key, \
                               const void* ctorData, \
                               bool* inserted ) \
    { \
        prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ prefix ## _get_hash( key ) ] ); \
        SCOREP_RWLock_ReaderLock( &( bucket->pending ), &( bucket->release_n_readers ) ); \
        prefix ## _value_t value = prefix ## _get_and_insert_impl( key, ctorData, inserted, bucket ); \
        SCOREP_RWLock_ReaderUnlock( &( bucket->pending ), &( bucket->departing ), &( bucket->release_writer ) ); \
        return value; \
    } \
\
    /* Do not call concurrently */ \
    static void \
    prefix ## _free_chunks( void ) \
    { \
        SCOREP_HASH_TABLE_FREE_CHUNKS( prefix, hashTableSize ) \
        /* in addition, deallocate free_list elements */ \
        while ( prefix ## _chunk_free_list ) \
        { \
            prefix ## _chunk_t* next = prefix ## _chunk_free_list->next; \
            prefix ## _free_chunk( prefix ## _chunk_free_list ); \
            prefix ## _chunk_free_list = next; \
        } \
    } \
\
    static inline bool \
    prefix ## _remove( prefix ## _key_t key ) \
    { \
        prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ prefix ## _get_hash( key ) ] ); \
        /* search and remove, if found, reduce bucket->size, call <prefix>_value_dtor if found */ \
        uint32_t i                         = 0; \
        uint32_t j                         = 0; \
        prefix ## _chunk_t* previous_chunk = NULL; \
        bool found                         = false; \
        prefix ## _key_t*   free_key; \
        prefix ## _value_t* free_value; \
        SCOREP_RWLock_WriterLock( &( bucket->remove_lock ), &( bucket->pending ), \
                                  &( bucket->departing ), &( bucket->release_writer ) ); \
        uint32_t current_size     = SCOREP_Atomic_LoadN_uint32( &( bucket->size ), SCOREP_ATOMIC_SEQUENTIAL_CONSISTENT ); \
        prefix ## _chunk_t* chunk = bucket->chunk; \
        /* search until end of chunks */ \
        for (; i < current_size; ++i, ++j ) \
        { \
            if ( j == ( nPairsPerChunk ) ) \
            { \
                previous_chunk = chunk; \
                chunk          = chunk->next; \
                j              = 0; \
            } \
            if ( prefix ## _equals( key, chunk->keys[ j ] ) ) \
            { \
                /* release element */ \
                prefix ## _value_dtor( chunk->keys[ j ], chunk->values[ j ] ); \
                free_key   = &( chunk->keys[ j ] ); \
                free_value = &( chunk->values[ j ] ); \
                found      = true; \
                break; \
            } \
        } \
        if ( !found ) \
        { \
            SCOREP_RWLock_WriterUnlock( &( bucket->remove_lock ), &( bucket->pending ), \
                                        &( bucket->release_n_readers ) );  \
            return false; \
        } \
        SCOREP_HASH_TABLE_MOVE_LAST_TO_REMOVED( prefix, nPairsPerChunk ) \
        SCOREP_RWLock_WriterUnlock( &( bucket->remove_lock ), &( bucket->pending ), \
                                    &( bucket->release_n_readers ) ); \
        return true; \
    } \
\
    typedef bool ( *prefix ## _condition_t )( prefix ## _key_t, prefix ## _value_t, void* data ); \
\
    static inline void \
    prefix ## _remove_if( prefix ## _condition_t condition, void* data ) \
    { \
        for ( uint32_t b = 0; b < ( hashTableSize ); ++b ) \
        { \
            prefix ## _bucket_t* bucket = &( prefix ## _hash_table[ b ] ); \
            SCOREP_RWLock_WriterLock( &( bucket->remove_lock ), &( bucket->pending ), \
                                      &( bucket->departing ), &( bucket->release_writer ) ); \
            prefix ## _chunk_t* outer_chunk = bucket->chunk; \
            int32_t outer_i                 = 0; \
            uint32_t current_size           = SCOREP_Atomic_LoadN_uint32( &( bucket->size ), SCOREP_ATOMIC_SEQUENTIAL_CONSISTENT ); \
            while ( outer_chunk != NULL ) \
            { \
                for ( int32_t outer_j = 0; outer_i < current_size && outer_j < ( nPairsPerChunk ); ++outer_i, ++outer_j ) \
                { \
                    if ( condition( outer_chunk->keys[ outer_j ], outer_chunk->values[ outer_j ], data ) ) \
                    { \
                        uint32_t i = outer_i; \
                        uint32_t j = outer_j; \
                        prefix ## _chunk_t* chunk = outer_chunk; \
                        prefix ## _key_t*   free_key; \
                        prefix ## _value_t* free_value; \
                        prefix ## _chunk_t* previous_chunk = NULL; \
                        /* release element */ \
                        prefix ## _value_dtor( chunk->keys[ j ], chunk->values[ j ] ); \
                        free_key   = &( chunk->keys[ j ] ); \
                        free_value = &( chunk->values[ j ] ); \
                        SCOREP_HASH_TABLE_MOVE_LAST_TO_REMOVED( prefix, nPairsPerChunk ) \
                        --outer_i; \
                        --outer_j; \
                    } \
                } \
                outer_chunk = outer_chunk->next; \
            } \
            SCOREP_RWLock_WriterUnlock( &( bucket->remove_lock ), &( bucket->pending ), \
                                        &( bucket->release_n_readers ) ); \
        } \
    }

/* *INDENT-ON*  */

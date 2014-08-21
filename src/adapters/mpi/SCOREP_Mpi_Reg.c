/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
 * RWTH Aachen University, Germany
 *
 * Copyright (c) 2009-2013,
 * Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *
 * Copyright (c) 2009-2014,
 * Technische Universitaet Dresden, Germany
 *
 * Copyright (c) 2009-2013,
 * University of Oregon, Eugene, USA
 *
 * Copyright (c) 2009-2013,
 * Forschungszentrum Juelich GmbH, Germany
 *
 * Copyright (c) 2009-2013,
 * German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
 *
 * Copyright (c) 2009-2013,
 * Technische Universitaet Muenchen, Germany
 *
 * This software may be modified and distributed under the terms of
 * a BSD-style license.  See the COPYING file in the package base
 * directory for details.
 *
 */

/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2011                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2010-2011                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/


/**
 * @file
 * @ingroup    MPI_Wrapper
 *
 * @brief Registration of MPI regions
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "SCOREP_Mpi.h"

/* Placeholder for MPI functions which are not communicative */
#define SCOREP_REGION_NONE SCOREP_REGION_FUNCTION

/**
 * MPI operation event type
 */
typedef struct
{
    char*    name;
    uint32_t type;
} scorep_mpi_type;

/** MPI point-to-point function types
 * @note contents must be alphabetically sorted */
static const scorep_mpi_type scorep_mpi_pt2pt[] = {
    { "MPI_Bsend",            SCOREP_MPI_TYPE__SEND                                 },
    { "MPI_Ibsend",           SCOREP_MPI_TYPE__SEND                                 },
    { "MPI_Irsend",           SCOREP_MPI_TYPE__SEND                                 },
    { "MPI_Isend",            SCOREP_MPI_TYPE__SEND                                 },
    { "MPI_Issend",           SCOREP_MPI_TYPE__SEND                                 },
    { "MPI_Recv",             SCOREP_MPI_TYPE__RECV                                 },
    { "MPI_Rsend",            SCOREP_MPI_TYPE__SEND                                 },
    { "MPI_Send",             SCOREP_MPI_TYPE__SEND                                 },
    { "MPI_Sendrecv",         SCOREP_MPI_TYPE__SEND | SCOREP_MPI_TYPE__RECV         },
    { "MPI_Sendrecv_replace", SCOREP_MPI_TYPE__SEND | SCOREP_MPI_TYPE__RECV         },
    { "MPI_Ssend",            SCOREP_MPI_TYPE__SEND                                 },
    { "MPI_Start",            SCOREP_MPI_TYPE__SEND                                 },
    { "MPI_Startall",         SCOREP_MPI_TYPE__SEND                                 },
    { "MPI_Test",             SCOREP_MPI_TYPE__RECV                                 },
    { "MPI_Testall",          SCOREP_MPI_TYPE__RECV                                 },
    { "MPI_Testany",          SCOREP_MPI_TYPE__RECV                                 },
    { "MPI_Testsome",         SCOREP_MPI_TYPE__RECV                                 },
    { "MPI_Wait",             SCOREP_MPI_TYPE__RECV                                 },
    { "MPI_Waitall",          SCOREP_MPI_TYPE__RECV                                 },
    { "MPI_Waitany",          SCOREP_MPI_TYPE__RECV                                 },
    { "MPI_Waitsome",         SCOREP_MPI_TYPE__RECV                                 },
};

/** MPI collective function types
 * @note contents must be alphabetically sorted */
static const scorep_mpi_type scorep_mpi_colls[] = {
    { "MPI_Allgather",      SCOREP_COLL_TYPE__ALL2ALL           },
    { "MPI_Allgatherv",     SCOREP_COLL_TYPE__ALL2ALL           },
    { "MPI_Allreduce",      SCOREP_COLL_TYPE__ALL2ALL           },
    { "MPI_Alltoall",       SCOREP_COLL_TYPE__ALL2ALL           },
    { "MPI_Alltoallv",      SCOREP_COLL_TYPE__ALL2ALL           },
    { "MPI_Alltoallw",      SCOREP_COLL_TYPE__ALL2ALL           },
    { "MPI_Barrier",        SCOREP_COLL_TYPE__BARRIER           },
    { "MPI_Bcast",          SCOREP_COLL_TYPE__ONE2ALL           },
    { "MPI_Cart_create",    SCOREP_COLL_TYPE__IMPLIED           },
    { "MPI_Cart_sub",       SCOREP_COLL_TYPE__IMPLIED           },
    { "MPI_Comm_create",    SCOREP_COLL_TYPE__IMPLIED           },
    { "MPI_Comm_dup",       SCOREP_COLL_TYPE__IMPLIED           },
    { "MPI_Comm_free",      SCOREP_COLL_TYPE__IMPLIED           },
    { "MPI_Comm_split",     SCOREP_COLL_TYPE__IMPLIED           },
    { "MPI_Exscan",         SCOREP_COLL_TYPE__PARTIAL           },
    { "MPI_Finalize",       SCOREP_COLL_TYPE__IMPLIED           },
    { "MPI_Gather",         SCOREP_COLL_TYPE__ALL2ONE           },
    { "MPI_Gatherv",        SCOREP_COLL_TYPE__ALL2ONE           },
    { "MPI_Init",           SCOREP_COLL_TYPE__IMPLIED           },
    { "MPI_Init_thread",    SCOREP_COLL_TYPE__IMPLIED           },
    { "MPI_Reduce",         SCOREP_COLL_TYPE__ALL2ONE           },
    { "MPI_Reduce_scatter", SCOREP_COLL_TYPE__ALL2ALL           },
    { "MPI_Scan",           SCOREP_COLL_TYPE__PARTIAL           },
    { "MPI_Scatter",        SCOREP_COLL_TYPE__ONE2ALL           },
    { "MPI_Scatterv",       SCOREP_COLL_TYPE__ONE2ALL           },
    { "MPI_Win_create",     SCOREP_COLL_TYPE__IMPLIED           },
    { "MPI_Win_fence",      SCOREP_COLL_TYPE__IMPLIED           },
    { "MPI_Win_free",       SCOREP_COLL_TYPE__IMPLIED           }
};

/** Region IDs of MPI functions */
SCOREP_RegionHandle scorep_mpi_regid[ SCOREP__MPI_NUMFUNCS + 1 ];

/**
 * Register MPI functions and initialize data structures
 */
void
scorep_mpi_register_regions()
{
    /* Define source file handle for MPI functions */
    SCOREP_SourceFileHandle file_id = SCOREP_Definitions_NewSourceFile( "MPI" );

#if HAVE( DECL_PMPI_ABORT )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_ABORT ] =
            SCOREP_Definitions_NewRegion( "MPI_Abort",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ACCUMULATE ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Accumulate )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_ACCUMULATE ] =
            SCOREP_Definitions_NewRegion( "MPI_Accumulate",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ADD_ERROR_CLASS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Add_error_class )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_ADD_ERROR_CLASS ] =
            SCOREP_Definitions_NewRegion( "MPI_Add_error_class",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ADD_ERROR_CODE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Add_error_code )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_ADD_ERROR_CODE ] =
            SCOREP_Definitions_NewRegion( "MPI_Add_error_code",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ADD_ERROR_STRING ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Add_error_string )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_ADD_ERROR_STRING ] =
            SCOREP_Definitions_NewRegion( "MPI_Add_error_string",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ADDRESS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Address )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_ADDRESS ] =
            SCOREP_Definitions_NewRegion( "MPI_Address",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ALLGATHER )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_ALLGATHER ] =
            SCOREP_Definitions_NewRegion( "MPI_Allgather",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_ALLGATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allgatherv )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_ALLGATHERV ] =
            SCOREP_Definitions_NewRegion( "MPI_Allgatherv",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_ALLOC_MEM ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Alloc_mem )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_ALLOC_MEM ] =
            SCOREP_Definitions_NewRegion( "MPI_Alloc_mem",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ALLREDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Allreduce )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_ALLREDUCE ] =
            SCOREP_Definitions_NewRegion( "MPI_Allreduce",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_ALLTOALL ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoall )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_ALLTOALL ] =
            SCOREP_Definitions_NewRegion( "MPI_Alltoall",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_ALLTOALLV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoallv )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_ALLTOALLV ] =
            SCOREP_Definitions_NewRegion( "MPI_Alltoallv",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_ALLTOALLW ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Alltoallw )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_ALLTOALLW ] =
            SCOREP_Definitions_NewRegion( "MPI_Alltoallw",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_ATTR_DELETE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Attr_delete )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_ATTR_DELETE ] =
            SCOREP_Definitions_NewRegion( "MPI_Attr_delete",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ATTR_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Attr_get )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_ATTR_GET ] =
            SCOREP_Definitions_NewRegion( "MPI_Attr_get",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ATTR_PUT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Attr_put )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_ATTR_PUT ] =
            SCOREP_Definitions_NewRegion( "MPI_Attr_put",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_BARRIER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Barrier )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_BARRIER ] =
            SCOREP_Definitions_NewRegion( "MPI_Barrier",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_BARRIER );
    }
#endif
#if HAVE( DECL_PMPI_BCAST ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Bcast )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_BCAST ] =
            SCOREP_Definitions_NewRegion( "MPI_Bcast",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ONE2ALL );
    }
#endif
#if HAVE( DECL_PMPI_BSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Bsend )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_BSEND ] =
            SCOREP_Definitions_NewRegion( "MPI_Bsend",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_BSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Bsend_init )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_BSEND_INIT ] =
            SCOREP_Definitions_NewRegion( "MPI_Bsend_init",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_BUFFER_ATTACH ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Buffer_attach )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_BUFFER_ATTACH ] =
            SCOREP_Definitions_NewRegion( "MPI_Buffer_attach",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_BUFFER_DETACH ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Buffer_detach )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_BUFFER_DETACH ] =
            SCOREP_Definitions_NewRegion( "MPI_Buffer_detach",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_CANCEL ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Cancel )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_CANCEL ] =
            SCOREP_Definitions_NewRegion( "MPI_Cancel",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_CART_COORDS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_coords )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_CART_COORDS ] =
            SCOREP_Definitions_NewRegion( "MPI_Cart_coords",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_CART_CREATE )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_CART_CREATE ] =
            SCOREP_Definitions_NewRegion( "MPI_Cart_create",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_CART_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_get )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_CART_GET ] =
            SCOREP_Definitions_NewRegion( "MPI_Cart_get",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_CART_MAP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_map )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_CART_MAP ] =
            SCOREP_Definitions_NewRegion( "MPI_Cart_map",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_CART_RANK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_rank )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_CART_RANK ] =
            SCOREP_Definitions_NewRegion( "MPI_Cart_rank",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_CART_SHIFT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cart_shift )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_CART_SHIFT ] =
            SCOREP_Definitions_NewRegion( "MPI_Cart_shift",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_CART_SUB )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_CART_SUB ] =
            SCOREP_Definitions_NewRegion( "MPI_Cart_sub",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_CARTDIM_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Cartdim_get )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_CARTDIM_GET ] =
            SCOREP_Definitions_NewRegion( "MPI_Cartdim_get",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_CLOSE_PORT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Close_port )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_SPAWN )
    {
        scorep_mpi_regid[ SCOREP__MPI_CLOSE_PORT ] =
            SCOREP_Definitions_NewRegion( "MPI_Close_port",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_ACCEPT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_accept )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_SPAWN )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_ACCEPT ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_accept",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_C2F ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Comm_c2f )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_C2F ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_c2f",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_CALL_ERRHANDLER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Comm_call_errhandler )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_CALL_ERRHANDLER ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_call_errhandler",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_COMPARE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_compare )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_COMPARE ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_compare",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_CONNECT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_connect )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_SPAWN )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_CONNECT ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_connect",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_CREATE )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_CREATE ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_create",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_CREATE_ERRHANDLER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Comm_create_errhandler )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_CREATE_ERRHANDLER ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_create_errhandler",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_CREATE_KEYVAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_create_keyval )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_CREATE_KEYVAL ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_create_keyval",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_DELETE_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_delete_attr )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_DELETE_ATTR ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_delete_attr",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_DISCONNECT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_disconnect )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_SPAWN )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_DISCONNECT ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_disconnect",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_DUP )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_DUP ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_dup",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_F2C ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Comm_f2c )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_F2C ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_f2c",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_FREE )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_FREE ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_free",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_FREE_KEYVAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_free_keyval )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_FREE_KEYVAL ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_free_keyval",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_GET_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_get_attr )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_GET_ATTR ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_get_attr",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_GET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Comm_get_errhandler )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_GET_ERRHANDLER ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_get_errhandler",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_GET_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_get_name )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_GET_NAME ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_get_name",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_GET_PARENT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_get_parent )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_SPAWN )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_GET_PARENT ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_get_parent",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_GROUP )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_GROUP ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_group",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_JOIN ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_join )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_SPAWN )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_JOIN ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_join",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_RANK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MINI ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_rank )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_RANK ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_rank",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_REMOTE_GROUP )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_REMOTE_GROUP ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_remote_group",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_REMOTE_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MINI ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_remote_size )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_REMOTE_SIZE ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_remote_size",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SET_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_set_attr )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_SET_ATTR ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_set_attr",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Comm_set_errhandler )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_SET_ERRHANDLER ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_set_errhandler",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SET_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Comm_set_name )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_SET_NAME ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_set_name",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MINI ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_size )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_SIZE ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_size",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SPAWN ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_spawn )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_SPAWN )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_SPAWN ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_spawn",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SPAWN_MULTIPLE ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Comm_spawn_multiple )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_SPAWN )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_SPAWN_MULTIPLE ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_spawn_multiple",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_SPLIT )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_SPLIT ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_split",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_COMM_TEST_INTER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Comm_test_inter )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_COMM_TEST_INTER ] =
            SCOREP_Definitions_NewRegion( "MPI_Comm_test_inter",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_DIMS_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dims_create )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_DIMS_CREATE ] =
            SCOREP_Definitions_NewRegion( "MPI_Dims_create",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_CREATE ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_create )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_DIST_GRAPH_CREATE ] =
            SCOREP_Definitions_NewRegion( "MPI_Dist_graph_create",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_CREATE_ADJACENT ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_create_adjacent )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_DIST_GRAPH_CREATE_ADJACENT ] =
            SCOREP_Definitions_NewRegion( "MPI_Dist_graph_create_adjacent",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_NEIGHBORS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_neighbors )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_DIST_GRAPH_NEIGHBORS ] =
            SCOREP_Definitions_NewRegion( "MPI_Dist_graph_neighbors",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_DIST_GRAPH_NEIGHBORS_COUNT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Dist_graph_neighbors_count )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_DIST_GRAPH_NEIGHBORS_COUNT ] =
            SCOREP_Definitions_NewRegion( "MPI_Dist_graph_neighbors_count",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ERRHANDLER_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Errhandler_create )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_ERRHANDLER_CREATE ] =
            SCOREP_Definitions_NewRegion( "MPI_Errhandler_create",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ERRHANDLER_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Errhandler_free )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_ERRHANDLER_FREE ] =
            SCOREP_Definitions_NewRegion( "MPI_Errhandler_free",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ERRHANDLER_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Errhandler_get )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_ERRHANDLER_GET ] =
            SCOREP_Definitions_NewRegion( "MPI_Errhandler_get",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ERRHANDLER_SET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Errhandler_set )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_ERRHANDLER_SET ] =
            SCOREP_Definitions_NewRegion( "MPI_Errhandler_set",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ERROR_CLASS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Error_class )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_ERROR_CLASS ] =
            SCOREP_Definitions_NewRegion( "MPI_Error_class",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ERROR_STRING ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Error_string )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_ERROR_STRING ] =
            SCOREP_Definitions_NewRegion( "MPI_Error_string",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_EXSCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Exscan )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_EXSCAN ] =
            SCOREP_Definitions_NewRegion( "MPI_Exscan",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_OTHER );
    }
#endif
#if HAVE( DECL_PMPI_FILE_C2F ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_File_c2f )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_C2F ] =
            SCOREP_Definitions_NewRegion( "MPI_File_c2f",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_CALL_ERRHANDLER ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_File_call_errhandler )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_CALL_ERRHANDLER ] =
            SCOREP_Definitions_NewRegion( "MPI_File_call_errhandler",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_CLOSE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_close )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_CLOSE ] =
            SCOREP_Definitions_NewRegion( "MPI_File_close",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_CREATE_ERRHANDLER ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_File_create_errhandler )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_CREATE_ERRHANDLER ] =
            SCOREP_Definitions_NewRegion( "MPI_File_create_errhandler",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_DELETE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_delete )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_DELETE ] =
            SCOREP_Definitions_NewRegion( "MPI_File_delete",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_F2C ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_File_f2c )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_F2C ] =
            SCOREP_Definitions_NewRegion( "MPI_File_f2c",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_AMODE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_amode )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_GET_AMODE ] =
            SCOREP_Definitions_NewRegion( "MPI_File_get_amode",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_ATOMICITY ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_atomicity )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_GET_ATOMICITY ] =
            SCOREP_Definitions_NewRegion( "MPI_File_get_atomicity",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_BYTE_OFFSET ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_byte_offset )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_GET_BYTE_OFFSET ] =
            SCOREP_Definitions_NewRegion( "MPI_File_get_byte_offset",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_File_get_errhandler )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_GET_ERRHANDLER ] =
            SCOREP_Definitions_NewRegion( "MPI_File_get_errhandler",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_GROUP ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_group )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_GET_GROUP ] =
            SCOREP_Definitions_NewRegion( "MPI_File_get_group",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_INFO ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_info )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_GET_INFO ] =
            SCOREP_Definitions_NewRegion( "MPI_File_get_info",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_POSITION ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_position )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_GET_POSITION ] =
            SCOREP_Definitions_NewRegion( "MPI_File_get_position",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_POSITION_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_position_shared )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_GET_POSITION_SHARED ] =
            SCOREP_Definitions_NewRegion( "MPI_File_get_position_shared",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_SIZE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_size )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_GET_SIZE ] =
            SCOREP_Definitions_NewRegion( "MPI_File_get_size",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_TYPE_EXTENT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_type_extent )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_GET_TYPE_EXTENT ] =
            SCOREP_Definitions_NewRegion( "MPI_File_get_type_extent",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_GET_VIEW ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_get_view )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_GET_VIEW ] =
            SCOREP_Definitions_NewRegion( "MPI_File_get_view",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_IREAD ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_IREAD ] =
            SCOREP_Definitions_NewRegion( "MPI_File_iread",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_AT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread_at )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_IREAD_AT ] =
            SCOREP_Definitions_NewRegion( "MPI_File_iread_at",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_IREAD_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iread_shared )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_IREAD_SHARED ] =
            SCOREP_Definitions_NewRegion( "MPI_File_iread_shared",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_IWRITE ] =
            SCOREP_Definitions_NewRegion( "MPI_File_iwrite",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_AT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite_at )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_IWRITE_AT ] =
            SCOREP_Definitions_NewRegion( "MPI_File_iwrite_at",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_IWRITE_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_iwrite_shared )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_IWRITE_SHARED ] =
            SCOREP_Definitions_NewRegion( "MPI_File_iwrite_shared",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_OPEN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_open )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_OPEN ] =
            SCOREP_Definitions_NewRegion( "MPI_File_open",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_PREALLOCATE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_preallocate )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_PREALLOCATE ] =
            SCOREP_Definitions_NewRegion( "MPI_File_preallocate",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_READ ] =
            SCOREP_Definitions_NewRegion( "MPI_File_read",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_all )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_READ_ALL ] =
            SCOREP_Definitions_NewRegion( "MPI_File_read_all",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_all_begin )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_READ_ALL_BEGIN ] =
            SCOREP_Definitions_NewRegion( "MPI_File_read_all_begin",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_ALL_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_all_end )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_READ_ALL_END ] =
            SCOREP_Definitions_NewRegion( "MPI_File_read_all_end",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_at )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_READ_AT ] =
            SCOREP_Definitions_NewRegion( "MPI_File_read_at",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_at_all )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_READ_AT_ALL ] =
            SCOREP_Definitions_NewRegion( "MPI_File_read_at_all",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_at_all_begin )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_READ_AT_ALL_BEGIN ] =
            SCOREP_Definitions_NewRegion( "MPI_File_read_at_all_begin",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_AT_ALL_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_at_all_end )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_READ_AT_ALL_END ] =
            SCOREP_Definitions_NewRegion( "MPI_File_read_at_all_end",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_ordered )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_READ_ORDERED ] =
            SCOREP_Definitions_NewRegion( "MPI_File_read_ordered",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_ordered_begin )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_READ_ORDERED_BEGIN ] =
            SCOREP_Definitions_NewRegion( "MPI_File_read_ordered_begin",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_ORDERED_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_ordered_end )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_READ_ORDERED_END ] =
            SCOREP_Definitions_NewRegion( "MPI_File_read_ordered_end",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_READ_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_read_shared )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_READ_SHARED ] =
            SCOREP_Definitions_NewRegion( "MPI_File_read_shared",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SEEK ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_seek )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_SEEK ] =
            SCOREP_Definitions_NewRegion( "MPI_File_seek",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SEEK_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_seek_shared )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_SEEK_SHARED ] =
            SCOREP_Definitions_NewRegion( "MPI_File_seek_shared",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SET_ATOMICITY ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_set_atomicity )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_SET_ATOMICITY ] =
            SCOREP_Definitions_NewRegion( "MPI_File_set_atomicity",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_File_set_errhandler )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_SET_ERRHANDLER ] =
            SCOREP_Definitions_NewRegion( "MPI_File_set_errhandler",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SET_INFO ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_set_info )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_SET_INFO ] =
            SCOREP_Definitions_NewRegion( "MPI_File_set_info",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SET_SIZE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_set_size )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_SET_SIZE ] =
            SCOREP_Definitions_NewRegion( "MPI_File_set_size",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SET_VIEW ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_set_view )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_SET_VIEW ] =
            SCOREP_Definitions_NewRegion( "MPI_File_set_view",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_SYNC ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_sync )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_SYNC ] =
            SCOREP_Definitions_NewRegion( "MPI_File_sync",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_WRITE ] =
            SCOREP_Definitions_NewRegion( "MPI_File_write",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_all )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_WRITE_ALL ] =
            SCOREP_Definitions_NewRegion( "MPI_File_write_all",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_all_begin )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_WRITE_ALL_BEGIN ] =
            SCOREP_Definitions_NewRegion( "MPI_File_write_all_begin",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ALL_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_all_end )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_WRITE_ALL_END ] =
            SCOREP_Definitions_NewRegion( "MPI_File_write_all_end",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_at )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_WRITE_AT ] =
            SCOREP_Definitions_NewRegion( "MPI_File_write_at",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_at_all )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_WRITE_AT_ALL ] =
            SCOREP_Definitions_NewRegion( "MPI_File_write_at_all",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_at_all_begin )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_WRITE_AT_ALL_BEGIN ] =
            SCOREP_Definitions_NewRegion( "MPI_File_write_at_all_begin",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_AT_ALL_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_at_all_end )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_WRITE_AT_ALL_END ] =
            SCOREP_Definitions_NewRegion( "MPI_File_write_at_all_end",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_ordered )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_WRITE_ORDERED ] =
            SCOREP_Definitions_NewRegion( "MPI_File_write_ordered",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED_BEGIN ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_ordered_begin )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_WRITE_ORDERED_BEGIN ] =
            SCOREP_Definitions_NewRegion( "MPI_File_write_ordered_begin",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_ORDERED_END ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_ordered_end )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_WRITE_ORDERED_END ] =
            SCOREP_Definitions_NewRegion( "MPI_File_write_ordered_end",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FILE_WRITE_SHARED ) && !defined( SCOREP_MPI_NO_IO ) && !defined( MPI_File_write_shared )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_FILE_WRITE_SHARED ] =
            SCOREP_Definitions_NewRegion( "MPI_File_write_shared",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FINALIZE )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ENV )
    {
        scorep_mpi_regid[ SCOREP__MPI_FINALIZE ] =
            SCOREP_Definitions_NewRegion( "MPI_Finalize",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FINALIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Finalized )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ENV )
    {
        scorep_mpi_regid[ SCOREP__MPI_FINALIZED ] =
            SCOREP_Definitions_NewRegion( "MPI_Finalized",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_FREE_MEM ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Free_mem )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_FREE_MEM ] =
            SCOREP_Definitions_NewRegion( "MPI_Free_mem",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GATHER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gather )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_GATHER ] =
            SCOREP_Definitions_NewRegion( "MPI_Gather",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ALL2ONE );
    }
#endif
#if HAVE( DECL_PMPI_GATHERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Gatherv )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_GATHERV ] =
            SCOREP_Definitions_NewRegion( "MPI_Gatherv",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ALL2ONE );
    }
#endif
#if HAVE( DECL_PMPI_GET ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Get )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_GET ] =
            SCOREP_Definitions_NewRegion( "MPI_Get",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GET_ADDRESS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Get_address )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_GET_ADDRESS ] =
            SCOREP_Definitions_NewRegion( "MPI_Get_address",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GET_COUNT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Get_count )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_GET_COUNT ] =
            SCOREP_Definitions_NewRegion( "MPI_Get_count",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GET_ELEMENTS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Get_elements )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_GET_ELEMENTS ] =
            SCOREP_Definitions_NewRegion( "MPI_Get_elements",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GET_PROCESSOR_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Get_processor_name )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_GET_PROCESSOR_NAME ] =
            SCOREP_Definitions_NewRegion( "MPI_Get_processor_name",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GET_VERSION ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Get_version )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_GET_VERSION ] =
            SCOREP_Definitions_NewRegion( "MPI_Get_version",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GRAPH_CREATE )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_GRAPH_CREATE ] =
            SCOREP_Definitions_NewRegion( "MPI_Graph_create",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GRAPH_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_get )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_GRAPH_GET ] =
            SCOREP_Definitions_NewRegion( "MPI_Graph_get",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GRAPH_MAP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_map )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_GRAPH_MAP ] =
            SCOREP_Definitions_NewRegion( "MPI_Graph_map",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_GRAPH_NEIGHBORS ] =
            SCOREP_Definitions_NewRegion( "MPI_Graph_neighbors",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS_COUNT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors_count )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_GRAPH_NEIGHBORS_COUNT ] =
            SCOREP_Definitions_NewRegion( "MPI_Graph_neighbors_count",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GRAPHDIMS_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Graphdims_get )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_GRAPHDIMS_GET ] =
            SCOREP_Definitions_NewRegion( "MPI_Graphdims_get",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GREQUEST_COMPLETE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Grequest_complete )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_GREQUEST_COMPLETE ] =
            SCOREP_Definitions_NewRegion( "MPI_Grequest_complete",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GREQUEST_START ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Grequest_start )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_GREQUEST_START ] =
            SCOREP_Definitions_NewRegion( "MPI_Grequest_start",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_C2F ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Group_c2f )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_C2F ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_c2f",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_COMPARE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_compare )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_COMPARE ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_compare",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_DIFFERENCE ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_difference )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_DIFFERENCE ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_difference",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_EXCL ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_excl )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_EXCL ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_excl",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_F2C ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Group_f2c )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_F2C ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_f2c",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_FREE ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_free )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_FREE ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_free",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_INCL ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_incl )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_INCL ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_incl",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_INTERSECTION ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_intersection )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_INTERSECTION ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_intersection",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_RANGE_EXCL ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_range_excl )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_RANGE_EXCL ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_range_excl",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_RANGE_INCL ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_range_incl )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_RANGE_INCL ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_range_incl",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_RANK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_rank )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_RANK ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_rank",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_size )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_SIZE ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_size",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_TRANSLATE_RANKS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_translate_ranks )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_TRANSLATE_RANKS ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_translate_ranks",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_GROUP_UNION ) && !defined( SCOREP_MPI_NO_CG ) && !defined( MPI_Group_union )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_GROUP_UNION ] =
            SCOREP_Definitions_NewRegion( "MPI_Group_union",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_IBSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ibsend )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_IBSEND ] =
            SCOREP_Definitions_NewRegion( "MPI_Ibsend",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_INFO_C2F ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_c2f )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_INFO_C2F ] =
            SCOREP_Definitions_NewRegion( "MPI_Info_c2f",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INFO_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_create )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_INFO_CREATE ] =
            SCOREP_Definitions_NewRegion( "MPI_Info_create",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INFO_DELETE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_delete )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_INFO_DELETE ] =
            SCOREP_Definitions_NewRegion( "MPI_Info_delete",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INFO_DUP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_dup )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_INFO_DUP ] =
            SCOREP_Definitions_NewRegion( "MPI_Info_dup",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INFO_F2C ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_f2c )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_INFO_F2C ] =
            SCOREP_Definitions_NewRegion( "MPI_Info_f2c",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INFO_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_free )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_INFO_FREE ] =
            SCOREP_Definitions_NewRegion( "MPI_Info_free",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INFO_GET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_get )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_INFO_GET ] =
            SCOREP_Definitions_NewRegion( "MPI_Info_get",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INFO_GET_NKEYS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_get_nkeys )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_INFO_GET_NKEYS ] =
            SCOREP_Definitions_NewRegion( "MPI_Info_get_nkeys",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INFO_GET_NTHKEY ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_get_nthkey )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_INFO_GET_NTHKEY ] =
            SCOREP_Definitions_NewRegion( "MPI_Info_get_nthkey",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INFO_GET_VALUELEN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_get_valuelen )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_INFO_GET_VALUELEN ] =
            SCOREP_Definitions_NewRegion( "MPI_Info_get_valuelen",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INFO_SET ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Info_set )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_INFO_SET ] =
            SCOREP_Definitions_NewRegion( "MPI_Info_set",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INIT )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ENV )
    {
        scorep_mpi_regid[ SCOREP__MPI_INIT ] =
            SCOREP_Definitions_NewRegion( "MPI_Init",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INIT_THREAD )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ENV )
    {
        scorep_mpi_regid[ SCOREP__MPI_INIT_THREAD ] =
            SCOREP_Definitions_NewRegion( "MPI_Init_thread",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INITIALIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Initialized )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ENV )
    {
        scorep_mpi_regid[ SCOREP__MPI_INITIALIZED ] =
            SCOREP_Definitions_NewRegion( "MPI_Initialized",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INTERCOMM_CREATE )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_INTERCOMM_CREATE ] =
            SCOREP_Definitions_NewRegion( "MPI_Intercomm_create",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_INTERCOMM_MERGE )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG )
    {
        scorep_mpi_regid[ SCOREP__MPI_INTERCOMM_MERGE ] =
            SCOREP_Definitions_NewRegion( "MPI_Intercomm_merge",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_IPROBE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Iprobe )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_IPROBE ] =
            SCOREP_Definitions_NewRegion( "MPI_Iprobe",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_IRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Irecv )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_IRECV ] =
            SCOREP_Definitions_NewRegion( "MPI_Irecv",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_IRSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Irsend )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_IRSEND ] =
            SCOREP_Definitions_NewRegion( "MPI_Irsend",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_IS_THREAD_MAIN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Is_thread_main )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ENV )
    {
        scorep_mpi_regid[ SCOREP__MPI_IS_THREAD_MAIN ] =
            SCOREP_Definitions_NewRegion( "MPI_Is_thread_main",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_ISEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Isend )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_ISEND ] =
            SCOREP_Definitions_NewRegion( "MPI_Isend",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_ISSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Issend )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_ISSEND ] =
            SCOREP_Definitions_NewRegion( "MPI_Issend",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_KEYVAL_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Keyval_create )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_KEYVAL_CREATE ] =
            SCOREP_Definitions_NewRegion( "MPI_Keyval_create",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_KEYVAL_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_CG ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Keyval_free )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_CG_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_KEYVAL_FREE ] =
            SCOREP_Definitions_NewRegion( "MPI_Keyval_free",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_LOOKUP_NAME ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Lookup_name )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_SPAWN )
    {
        scorep_mpi_regid[ SCOREP__MPI_LOOKUP_NAME ] =
            SCOREP_Definitions_NewRegion( "MPI_Lookup_name",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_OP_C2F ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Op_c2f )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_OP_C2F ] =
            SCOREP_Definitions_NewRegion( "MPI_Op_c2f",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_OP_COMMUTATIVE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Op_commutative )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_OP_COMMUTATIVE ] =
            SCOREP_Definitions_NewRegion( "MPI_Op_commutative",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_OP_CREATE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Op_create )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_OP_CREATE ] =
            SCOREP_Definitions_NewRegion( "MPI_Op_create",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_OP_F2C ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Op_f2c )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_OP_F2C ] =
            SCOREP_Definitions_NewRegion( "MPI_Op_f2c",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_OP_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Op_free )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_OP_FREE ] =
            SCOREP_Definitions_NewRegion( "MPI_Op_free",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_OPEN_PORT ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Open_port )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_SPAWN )
    {
        scorep_mpi_regid[ SCOREP__MPI_OPEN_PORT ] =
            SCOREP_Definitions_NewRegion( "MPI_Open_port",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_PACK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Pack )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_PACK ] =
            SCOREP_Definitions_NewRegion( "MPI_Pack",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_PACK_EXTERNAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Pack_external )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_PACK_EXTERNAL ] =
            SCOREP_Definitions_NewRegion( "MPI_Pack_external",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_PACK_EXTERNAL_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Pack_external_size )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_PACK_EXTERNAL_SIZE ] =
            SCOREP_Definitions_NewRegion( "MPI_Pack_external_size",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_PACK_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Pack_size )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_PACK_SIZE ] =
            SCOREP_Definitions_NewRegion( "MPI_Pack_size",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_PCONTROL ) && !defined( SCOREP_MPI_NO_PERF ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Pcontrol )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_PERF )
    {
        scorep_mpi_regid[ SCOREP__MPI_PCONTROL ] =
            SCOREP_Definitions_NewRegion( "MPI_Pcontrol",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_PROBE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Probe )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_PROBE ] =
            SCOREP_Definitions_NewRegion( "MPI_Probe",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_PUBLISH_NAME ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Publish_name )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_SPAWN )
    {
        scorep_mpi_regid[ SCOREP__MPI_PUBLISH_NAME ] =
            SCOREP_Definitions_NewRegion( "MPI_Publish_name",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_PUT ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Put )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_PUT ] =
            SCOREP_Definitions_NewRegion( "MPI_Put",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_QUERY_THREAD ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ENV ) && !defined( MPI_Query_thread )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_ENV )
    {
        scorep_mpi_regid[ SCOREP__MPI_QUERY_THREAD ] =
            SCOREP_Definitions_NewRegion( "MPI_Query_thread",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_RECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Recv )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_RECV ] =
            SCOREP_Definitions_NewRegion( "MPI_Recv",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_RECV_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Recv_init )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_RECV_INIT ] =
            SCOREP_Definitions_NewRegion( "MPI_Recv_init",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_REDUCE ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_REDUCE ] =
            SCOREP_Definitions_NewRegion( "MPI_Reduce",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ALL2ONE );
    }
#endif
#if HAVE( DECL_PMPI_REDUCE_LOCAL ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_local )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_REDUCE_LOCAL ] =
            SCOREP_Definitions_NewRegion( "MPI_Reduce_local",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_REDUCE_SCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_REDUCE_SCATTER ] =
            SCOREP_Definitions_NewRegion( "MPI_Reduce_scatter",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_REDUCE_SCATTER_BLOCK ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Reduce_scatter_block )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_REDUCE_SCATTER_BLOCK ] =
            SCOREP_Definitions_NewRegion( "MPI_Reduce_scatter_block",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ALL2ALL );
    }
#endif
#if HAVE( DECL_PMPI_REGISTER_DATAREP ) && !defined( SCOREP_MPI_NO_IO ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Register_datarep )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_IO )
    {
        scorep_mpi_regid[ SCOREP__MPI_REGISTER_DATAREP ] =
            SCOREP_Definitions_NewRegion( "MPI_Register_datarep",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_REQUEST_C2F ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Request_c2f )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_REQUEST_C2F ] =
            SCOREP_Definitions_NewRegion( "MPI_Request_c2f",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_REQUEST_F2C ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Request_f2c )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_REQUEST_F2C ] =
            SCOREP_Definitions_NewRegion( "MPI_Request_f2c",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_REQUEST_FREE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Request_free )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_REQUEST_FREE ] =
            SCOREP_Definitions_NewRegion( "MPI_Request_free",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_REQUEST_GET_STATUS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Request_get_status )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_REQUEST_GET_STATUS ] =
            SCOREP_Definitions_NewRegion( "MPI_Request_get_status",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_RSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Rsend )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_RSEND ] =
            SCOREP_Definitions_NewRegion( "MPI_Rsend",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_RSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Rsend_init )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_RSEND_INIT ] =
            SCOREP_Definitions_NewRegion( "MPI_Rsend_init",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_SCAN ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scan )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_SCAN ] =
            SCOREP_Definitions_NewRegion( "MPI_Scan",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_OTHER );
    }
#endif
#if HAVE( DECL_PMPI_SCATTER ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatter )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_SCATTER ] =
            SCOREP_Definitions_NewRegion( "MPI_Scatter",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ONE2ALL );
    }
#endif
#if HAVE( DECL_PMPI_SCATTERV ) && !defined( SCOREP_MPI_NO_COLL ) && !defined( MPI_Scatterv )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_COLL )
    {
        scorep_mpi_regid[ SCOREP__MPI_SCATTERV ] =
            SCOREP_Definitions_NewRegion( "MPI_Scatterv",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_COLL_ONE2ALL );
    }
#endif
#if HAVE( DECL_PMPI_SEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Send )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_SEND ] =
            SCOREP_Definitions_NewRegion( "MPI_Send",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_SEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Send_init )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_SEND_INIT ] =
            SCOREP_Definitions_NewRegion( "MPI_Send_init",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_SENDRECV ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Sendrecv )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_SENDRECV ] =
            SCOREP_Definitions_NewRegion( "MPI_Sendrecv",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_SENDRECV_REPLACE ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Sendrecv_replace )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_SENDRECV_REPLACE ] =
            SCOREP_Definitions_NewRegion( "MPI_Sendrecv_replace",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_SIZEOF ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Sizeof )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_SIZEOF ] =
            SCOREP_Definitions_NewRegion( "MPI_Sizeof",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_SSEND ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ssend )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_SSEND ] =
            SCOREP_Definitions_NewRegion( "MPI_Ssend",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_SSEND_INIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Ssend_init )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_SSEND_INIT ] =
            SCOREP_Definitions_NewRegion( "MPI_Ssend_init",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_START ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Start )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_START ] =
            SCOREP_Definitions_NewRegion( "MPI_Start",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_STARTALL ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Startall )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_STARTALL ] =
            SCOREP_Definitions_NewRegion( "MPI_Startall",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_STATUS_C2F ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Status_c2f )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_STATUS_C2F ] =
            SCOREP_Definitions_NewRegion( "MPI_Status_c2f",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_STATUS_F2C ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Status_f2c )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_STATUS_F2C ] =
            SCOREP_Definitions_NewRegion( "MPI_Status_f2c",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_STATUS_SET_CANCELLED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Status_set_cancelled )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_STATUS_SET_CANCELLED ] =
            SCOREP_Definitions_NewRegion( "MPI_Status_set_cancelled",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_STATUS_SET_ELEMENTS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Status_set_elements )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_STATUS_SET_ELEMENTS ] =
            SCOREP_Definitions_NewRegion( "MPI_Status_set_elements",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TEST ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Test )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_TEST ] =
            SCOREP_Definitions_NewRegion( "MPI_Test",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_TEST_CANCELLED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Test_cancelled )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_TEST_CANCELLED ] =
            SCOREP_Definitions_NewRegion( "MPI_Test_cancelled",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TESTALL ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Testall )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_TESTALL ] =
            SCOREP_Definitions_NewRegion( "MPI_Testall",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_TESTANY ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Testany )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_TESTANY ] =
            SCOREP_Definitions_NewRegion( "MPI_Testany",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_TESTSOME ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Testsome )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_TESTSOME ] =
            SCOREP_Definitions_NewRegion( "MPI_Testsome",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_TOPO_TEST ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TOPO ) && !defined( MPI_Topo_test )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TOPO )
    {
        scorep_mpi_regid[ SCOREP__MPI_TOPO_TEST ] =
            SCOREP_Definitions_NewRegion( "MPI_Topo_test",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_C2F ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Type_c2f )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_C2F ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_c2f",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_COMMIT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_commit )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_COMMIT ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_commit",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CONTIGUOUS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_contiguous )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_CONTIGUOUS ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_contiguous",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_DARRAY ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_darray )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_CREATE_DARRAY ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_create_darray",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_F90_COMPLEX ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_f90_complex )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_CREATE_F90_COMPLEX ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_create_f90_complex",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_F90_INTEGER ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_f90_integer )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_CREATE_F90_INTEGER ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_create_f90_integer",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_F90_REAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_f90_real )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_CREATE_F90_REAL ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_create_f90_real",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_HINDEXED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_hindexed )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_CREATE_HINDEXED ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_create_hindexed",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_HVECTOR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_hvector )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_CREATE_HVECTOR ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_create_hvector",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_INDEXED_BLOCK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_indexed_block )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_CREATE_INDEXED_BLOCK ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_create_indexed_block",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_KEYVAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_create_keyval )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_CREATE_KEYVAL ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_create_keyval",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_RESIZED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_resized )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_CREATE_RESIZED ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_create_resized",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_STRUCT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_struct )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_CREATE_STRUCT ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_create_struct",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_CREATE_SUBARRAY ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_create_subarray )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_CREATE_SUBARRAY ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_create_subarray",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_DELETE_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_delete_attr )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_DELETE_ATTR ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_delete_attr",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_DUP ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_dup )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_DUP ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_dup",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_EXTENT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_extent )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_EXTENT ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_extent",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_F2C ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( MPI_Type_f2c )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_F2C ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_f2c",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_FREE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_free )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_FREE ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_free",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_FREE_KEYVAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_free_keyval )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_FREE_KEYVAL ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_free_keyval",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_GET_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_get_attr )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_GET_ATTR ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_get_attr",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_GET_CONTENTS ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_contents )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_GET_CONTENTS ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_get_contents",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_GET_ENVELOPE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_envelope )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_GET_ENVELOPE ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_get_envelope",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_GET_EXTENT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_extent )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_GET_EXTENT ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_get_extent",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_GET_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_get_name )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_GET_NAME ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_get_name",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_GET_TRUE_EXTENT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_get_true_extent )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_GET_TRUE_EXTENT ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_get_true_extent",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_HINDEXED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_hindexed )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_HINDEXED ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_hindexed",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_HVECTOR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_hvector )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_HVECTOR ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_hvector",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_INDEXED ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_indexed )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_INDEXED ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_indexed",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_LB ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_lb )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_LB ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_lb",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_MATCH_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_match_size )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_MATCH_SIZE ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_match_size",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_SET_ATTR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_set_attr )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_SET_ATTR ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_set_attr",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_SET_NAME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Type_set_name )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_SET_NAME ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_set_name",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_SIZE ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_size )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_SIZE ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_size",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_STRUCT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_struct )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_STRUCT ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_struct",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_UB ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_ub )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_UB ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_ub",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_TYPE_VECTOR ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Type_vector )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_TYPE_VECTOR ] =
            SCOREP_Definitions_NewRegion( "MPI_Type_vector",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_UNPACK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Unpack )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_UNPACK ] =
            SCOREP_Definitions_NewRegion( "MPI_Unpack",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_UNPACK_EXTERNAL ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_TYPE ) && !defined( MPI_Unpack_external )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_TYPE )
    {
        scorep_mpi_regid[ SCOREP__MPI_UNPACK_EXTERNAL ] =
            SCOREP_Definitions_NewRegion( "MPI_Unpack_external",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_UNPUBLISH_NAME ) && !defined( SCOREP_MPI_NO_SPAWN ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Unpublish_name )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_SPAWN )
    {
        scorep_mpi_regid[ SCOREP__MPI_UNPUBLISH_NAME ] =
            SCOREP_Definitions_NewRegion( "MPI_Unpublish_name",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WAIT ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Wait )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_WAIT ] =
            SCOREP_Definitions_NewRegion( "MPI_Wait",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_WAITALL ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Waitall )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_WAITALL ] =
            SCOREP_Definitions_NewRegion( "MPI_Waitall",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_WAITANY ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Waitany )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_WAITANY ] =
            SCOREP_Definitions_NewRegion( "MPI_Waitany",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_WAITSOME ) && !defined( SCOREP_MPI_NO_P2P ) && !defined( MPI_Waitsome )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_P2P )
    {
        scorep_mpi_regid[ SCOREP__MPI_WAITSOME ] =
            SCOREP_Definitions_NewRegion( "MPI_Waitsome",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_POINT2POINT );
    }
#endif
#if HAVE( DECL_PMPI_WIN_C2F ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Win_c2f )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_C2F ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_c2f",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_CALL_ERRHANDLER ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Win_call_errhandler )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_CALL_ERRHANDLER ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_call_errhandler",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_COMPLETE ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Win_complete )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_COMPLETE ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_complete",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_CREATE ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Win_create )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_CREATE ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_create",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_CREATE_ERRHANDLER ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Win_create_errhandler )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_CREATE_ERRHANDLER ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_create_errhandler",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_CREATE_KEYVAL ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Win_create_keyval )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_CREATE_KEYVAL ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_create_keyval",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_DELETE_ATTR ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Win_delete_attr )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_DELETE_ATTR ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_delete_attr",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_F2C ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_MISC ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Win_f2c )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_MISC )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_F2C ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_f2c",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_FENCE ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Win_fence )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_FENCE ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_fence",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_FREE ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Win_free )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_FREE ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_free",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_FREE_KEYVAL ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Win_free_keyval )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_FREE_KEYVAL ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_free_keyval",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_GET_ATTR ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Win_get_attr )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_GET_ATTR ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_get_attr",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_GET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Win_get_errhandler )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_GET_ERRHANDLER ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_get_errhandler",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_GET_GROUP ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Win_get_group )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_GET_GROUP ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_get_group",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_GET_NAME ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Win_get_name )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_GET_NAME ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_get_name",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_LOCK ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Win_lock )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_LOCK ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_lock",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_POST ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Win_post )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_POST ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_post",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_SET_ATTR ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Win_set_attr )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_SET_ATTR ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_set_attr",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_SET_ERRHANDLER ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_ERR ) && !defined( MPI_Win_set_errhandler )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_ERR )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_SET_ERRHANDLER ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_set_errhandler",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_SET_NAME ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( MPI_Win_set_name )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_SET_NAME ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_set_name",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_START ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Win_start )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_START ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_start",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_TEST ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Win_test )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_TEST ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_test",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_UNLOCK ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Win_unlock )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_UNLOCK ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_unlock",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WIN_WAIT ) && !defined( SCOREP_MPI_NO_RMA ) && !defined( MPI_Win_wait )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_RMA )
    {
        scorep_mpi_regid[ SCOREP__MPI_WIN_WAIT ] =
            SCOREP_Definitions_NewRegion( "MPI_Win_wait",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WTICK ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MINI ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Wtick )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_WTICK ] =
            SCOREP_Definitions_NewRegion( "MPI_Wtick",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif
#if HAVE( DECL_PMPI_WTIME ) && !defined( SCOREP_MPI_NO_EXTRA ) && !defined( SCOREP_MPI_NO_MINI ) && !defined( SCOREP_MPI_NO_EXT ) && !defined( MPI_Wtime )
    if ( scorep_mpi_enabled & SCOREP_MPI_ENABLED_EXT )
    {
        scorep_mpi_regid[ SCOREP__MPI_WTIME ] =
            SCOREP_Definitions_NewRegion( "MPI_Wtime",
                                          NULL,
                                          file_id,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_INVALID_LINE_NO,
                                          SCOREP_PARADIGM_MPI,
                                          SCOREP_REGION_NONE );
    }
#endif

    /* Artificial root for MPI-only experiments when no user-code
     * instrumenation is available */
    scorep_mpi_regid[ SCOREP__MPI_ARTIFICIAL_ROOT ] =
        SCOREP_Definitions_NewRegion( "MPI",
                                      NULL,
                                      SCOREP_INVALID_SOURCE_FILE,
                                      SCOREP_INVALID_LINE_NO,
                                      SCOREP_INVALID_LINE_NO,
                                      SCOREP_PARADIGM_MPI,
                                      SCOREP_REGION_ARTIFICIAL );
}

/**
 * Compare function for binary search
 * @param v1 string to checked
 * @param v2 pointer to \p scorep_mpi_type structure
 * @return result is equivalent to \p strcasecmp result of two strings
 */
static int
scorep_mpi_mycmp( const void* v1, const void* v2 )
{
    return strcasecmp( ( char* )v1, ( ( scorep_mpi_type* )v2 )->name );
}

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


#include "SILC_Mpi.h"
#include "SILC_DefinitionLocking.h"
#include "config.h"

/**
 * @file       SILC_Mpi_Topo.c
 * @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 * @status     ALPHA
 * @ingroup    MPI_Wrapper
 *
 * @brief C interface wrappers for topologies
 */

/**
 * @name C wrappers
 * @{
 */

#if HAVE( DECL_PMPI_CART_CREATE )
/**
 * Measurement wrapper for MPI_Cart_create
 * @note Manually adapted wrapper
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Sequence of events:
 * @li enter region 'MPI_Cart_create'
 * @li define communicator
 * @li define topology
 * @li define coordinates
 * @li exit region 'MPI_Cart_create'
 */
int
MPI_Cart_create( MPI_Comm  comm_old,
                 int       ndims,
                 int*      dims,
                 int*      periodv,
                 int       reorder,
                 MPI_Comm* comm_cart )
{
    const int32_t event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO );
    int32_t       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_CART_CREATE ] );
    }

    return_val = PMPI_Cart_create( comm_old, ndims, dims, periodv, reorder, comm_cart );

    if ( *comm_cart != MPI_COMM_NULL )
    {
        SILC_MPICartTopolHandle topid = SILC_INVALID_CART_TOPOLOGY;
        int32_t                 cid, my_rank, i;
        int32_t*                coordv;
        uint8_t*                uperiodv;
        uint32_t*               udimv;
        uint32_t*               ucoordv;

        /* register the new topology communicator */
        silc_mpi_comm_create( *comm_cart );

        /* get the internal comminicator id for the communicator of the new topology */
        cid = silc_mpi_comm_id( *comm_cart );

        /* find the rank of the calling process */
        PMPI_Comm_rank( *comm_cart, &my_rank );

        /* assign the cartesian topology dimension parameters */
        udimv = calloc( ndims, sizeof( uint32_t ) );
        if ( !udimv )
        {
            SILC_ERROR_POSIX();
        }

        uperiodv = calloc( ndims, sizeof( uint8_t ) );
        if ( !uperiodv )
        {
            SILC_ERROR_POSIX();
        }

        for ( i = 0; i < ndims; i++ )
        {
            udimv[ i ]    =  ( uint32_t )dims[ i ];
            uperiodv[ i ] =  ( uint8_t )periodv[ i ];
        }

        /* create the cartesian topology definition record */
        SILC_LockMPICartesianTopologyDefinition();
        topid = SILC_DefineMPICartesianTopology( "", cid, ndims, udimv, uperiodv );
        SILC_UnlockMPICartesianTopologyDefinition();

        /* allocate space for coordv and ucoordv */
        coordv = calloc( ndims, sizeof( int ) );
        if ( !coordv )
        {
            SILC_ERROR_POSIX();
        }

        ucoordv = calloc( ndims, sizeof( uint32_t ) );
        if ( !ucoordv )
        {
            SILC_ERROR_POSIX();
        }

        /* get the coordinates of the calling process in coordv */
        PMPI_Cart_coords( *comm_cart, my_rank, ndims, coordv );

        /* assign the coordinates */
        for ( i = 0; i < ndims; i++ )
        {
            ucoordv[ i ] = ( uint32_t )coordv[ i ];
        }

        /* create the coordinates definition record */
        SILC_LockMPICartesianCoordsDefinition();
        SILC_DefineMPICartesianCoords( topid, ndims, ucoordv );
        SILC_UnlockMPICartesianCoordsDefinition();

        free( udimv );
        free( uperiodv );
        free( ucoordv );
    }

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_CART_CREATE ] );
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_CART_SUB )
/**
 * Measurement wrapper for MPI_Cart_sub
 * @note Auto-generated by wrapgen from template: SILC_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SILC_DefineMPICommunicator is called.
 */
int
MPI_Cart_sub( MPI_Comm  comm,
              int*      remain_dims,
              MPI_Comm* newcomm )
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_CART_SUB ] );
    }

    return_val = PMPI_Cart_sub( comm, remain_dims, newcomm );
    if ( *newcomm != MPI_COMM_NULL )
    {
        silc_mpi_comm_create( *newcomm );
    }

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_CART_SUB ] );
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif
#if HAVE( DECL_PMPI_GRAPH_CREATE )
/**
 * Measurement wrapper for MPI_Graph_create
 * @note Auto-generated by wrapgen from template: SILC_Mpi_CommMgnt.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * It triggers an enter and an exit event. Additionally, between enter event and exit
 * event, @ref SILC_DefineMPICommunicator is called.
 */
int
MPI_Graph_create( MPI_Comm  comm_old,
                  int       nnodes,
                  int*      index,
                  int*      edges,
                  int       reorder,
                  MPI_Comm* newcomm )
{
    const int event_gen_active = SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO );
    int       return_val;

    if ( event_gen_active )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_GRAPH_CREATE ] );
    }

    return_val = PMPI_Graph_create( comm_old, nnodes, index, edges, reorder, newcomm );
    if ( *newcomm != MPI_COMM_NULL )
    {
        silc_mpi_comm_create( *newcomm );
    }

    if ( event_gen_active )
    {
        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_GRAPH_CREATE ] );
        SILC_MPI_EVENT_GEN_ON();
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_CART_COORDS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Cart_coords )
/**
 * Measurement wrapper for MPI_Cart_coords
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Cart_coords( MPI_Comm comm,
                 int      rank,
                 int      maxdims,
                 int*     coords )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_CART_COORDS ] );

        return_val = PMPI_Cart_coords( comm, rank, maxdims, coords );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_CART_COORDS ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Cart_coords( comm, rank, maxdims, coords );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_CART_GET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Cart_get )
/**
 * Measurement wrapper for MPI_Cart_get
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Cart_get( MPI_Comm comm,
              int      maxdims,
              int*     dims,
              int*     periods,
              int*     coords )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_CART_GET ] );

        return_val = PMPI_Cart_get( comm, maxdims, dims, periods, coords );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_CART_GET ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Cart_get( comm, maxdims, dims, periods, coords );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_CART_MAP ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Cart_map )
/**
 * Measurement wrapper for MPI_Cart_map
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Cart_map( MPI_Comm comm,
              int      ndims,
              int*     dims,
              int*     periods,
              int*     newrank )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_CART_MAP ] );

        return_val = PMPI_Cart_map( comm, ndims, dims, periods, newrank );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_CART_MAP ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Cart_map( comm, ndims, dims, periods, newrank );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_CART_RANK ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Cart_rank )
/**
 * Measurement wrapper for MPI_Cart_rank
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Cart_rank( MPI_Comm comm,
               int*     coords,
               int*     rank )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_CART_RANK ] );

        return_val = PMPI_Cart_rank( comm, coords, rank );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_CART_RANK ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Cart_rank( comm, coords, rank );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_CART_SHIFT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Cart_shift )
/**
 * Measurement wrapper for MPI_Cart_shift
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Cart_shift( MPI_Comm comm,
                int      direction,
                int      disp,
                int*     rank_source,
                int*     rank_dest )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_CART_SHIFT ] );

        return_val = PMPI_Cart_shift( comm, direction, disp, rank_source, rank_dest );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_CART_SHIFT ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Cart_shift( comm, direction, disp, rank_source, rank_dest );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_CARTDIM_GET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Cartdim_get )
/**
 * Measurement wrapper for MPI_Cartdim_get
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Cartdim_get( MPI_Comm comm,
                 int*     ndims )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_CARTDIM_GET ] );

        return_val = PMPI_Cartdim_get( comm, ndims );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_CARTDIM_GET ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Cartdim_get( comm, ndims );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_GRAPH_GET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Graph_get )
/**
 * Measurement wrapper for MPI_Graph_get
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Graph_get( MPI_Comm comm,
               int      maxindex,
               int      maxedges,
               int*     index,
               int*     edges )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_GRAPH_GET ] );

        return_val = PMPI_Graph_get( comm, maxindex, maxedges, index, edges );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_GRAPH_GET ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Graph_get( comm, maxindex, maxedges, index, edges );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_GRAPH_MAP ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Graph_map )
/**
 * Measurement wrapper for MPI_Graph_map
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Graph_map( MPI_Comm comm,
               int      nnodes,
               int*     index,
               int*     edges,
               int*     newrank )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_GRAPH_MAP ] );

        return_val = PMPI_Graph_map( comm, nnodes, index, edges, newrank );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_GRAPH_MAP ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Graph_map( comm, nnodes, index, edges, newrank );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors )
/**
 * Measurement wrapper for MPI_Graph_neighbors
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Graph_neighbors( MPI_Comm comm,
                     int      rank,
                     int      maxneighbors,
                     int*     neighbors )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_GRAPH_NEIGHBORS ] );

        return_val = PMPI_Graph_neighbors( comm, rank, maxneighbors, neighbors );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_GRAPH_NEIGHBORS ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Graph_neighbors( comm, rank, maxneighbors, neighbors );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_GRAPH_NEIGHBORS_COUNT ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Graph_neighbors_count )
/**
 * Measurement wrapper for MPI_Graph_neighbors_count
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Graph_neighbors_count( MPI_Comm comm,
                           int      rank,
                           int*     nneighbors )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_GRAPH_NEIGHBORS_COUNT ] );

        return_val = PMPI_Graph_neighbors_count( comm, rank, nneighbors );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_GRAPH_NEIGHBORS_COUNT ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Graph_neighbors_count( comm, rank, nneighbors );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_GRAPHDIMS_GET ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Graphdims_get )
/**
 * Measurement wrapper for MPI_Graphdims_get
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Graphdims_get( MPI_Comm comm,
                   int*     nnodes,
                   int*     nedges )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_GRAPHDIMS_GET ] );

        return_val = PMPI_Graphdims_get( comm, nnodes, nedges );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_GRAPHDIMS_GET ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Graphdims_get( comm, nnodes, nedges );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_TOPO_TEST ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Topo_test )
/**
 * Measurement wrapper for MPI_Topo_test
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Topo_test( MPI_Comm comm,
               int*     status )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_TOPO_TEST ] );

        return_val = PMPI_Topo_test( comm, status );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_TOPO_TEST ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Topo_test( comm, status );
    }

    return return_val;
}
#endif

#if HAVE( DECL_PMPI_DIMS_CREATE ) && !defined( SILC_MPI_NO_EXTRA ) && !defined( SILC_MPI_NO_TOPO ) && !defined( MPI_Dims_create )
/**
 * Measurement wrapper for MPI_Dims_create
 * @note Auto-generated by wrapgen from template: SILC_Mpi_Std.w
 * @note C interface
 * @note Introduced with MPI-1
 * @ingroup topo
 * Triggers an enter and exit event.
 * It wraps the me) call with enter and exit events.
 */
int
MPI_Dims_create( int  nnodes,
                 int  ndims,
                 int* dims )
{
    int return_val;

    if ( SILC_MPI_IS_EVENT_GEN_ON_FOR( SILC_MPI_ENABLED_TOPO ) )
    {
        SILC_MPI_EVENT_GEN_OFF();
        SILC_EnterRegion( silc_mpi_regid[ SILC__MPI_DIMS_CREATE ] );

        return_val = PMPI_Dims_create( nnodes, ndims, dims );

        SILC_ExitRegion( silc_mpi_regid[ SILC__MPI_DIMS_CREATE ] );
        SILC_MPI_EVENT_GEN_ON();
    }
    else
    {
        return_val = PMPI_Dims_create( nnodes, ndims, dims );
    }

    return return_val;
}
#endif


/**
 * @}
 */

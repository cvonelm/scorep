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
 * @file       SCOREP_Score_EventList.hpp
 * @maintainer Daniel Lorenz  <d.lorenz@fz-juelich.de>
 *
 * @brief      Defines name lists for determine which events are created by which region.
 */

#ifndef SCOREP_SCORE_EVENTLIST_H
#define SCOREP_SCORE_EVENTLIST_H

#define SCOREP_SCORE_EVENT_SEND                \
    SCOREP_SCORE_EVENT( "MPI_Bsend" )            \
    SCOREP_SCORE_EVENT( "MPI_Rsend" )            \
    SCOREP_SCORE_EVENT( "MPI_Ssend" )            \
    SCOREP_SCORE_EVENT( "MPI_Sendrecv" )         \
    SCOREP_SCORE_EVENT( "MPI_Sendrecv_replace" ) \
    SCOREP_SCORE_EVENT( "MPI_Send" )

#define SCOREP_SCORE_EVENT_ISEND         \
    SCOREP_SCORE_EVENT( "MPI_Ibsend" )     \
    SCOREP_SCORE_EVENT( "MPI_Irsend" )     \
    SCOREP_SCORE_EVENT( "MPI_Issend" )     \
    SCOREP_SCORE_EVENT( "MPI_Isend" )      \
    SCOREP_SCORE_EVENT( "MPI_Bsend_init" ) \
    SCOREP_SCORE_EVENT( "MPI_Rsend_init" ) \
    SCOREP_SCORE_EVENT( "MPI_Send_init" )  \
    SCOREP_SCORE_EVENT( "MPI_Ssend_init" )

#define SCOREP_SCORE_EVENT_ISENDCOMPLETE \
    SCOREP_SCORE_EVENT( "MPI_Ibsend" )     \
    SCOREP_SCORE_EVENT( "MPI_Irsend" )     \
    SCOREP_SCORE_EVENT( "MPI_Issend" )     \
    SCOREP_SCORE_EVENT( "MPI_Isend" )      \
    SCOREP_SCORE_EVENT( "MPI_Bsend_init" ) \
    SCOREP_SCORE_EVENT( "MPI_Rsend_init" ) \
    SCOREP_SCORE_EVENT( "MPI_Send_init" )  \
    SCOREP_SCORE_EVENT( "MPI_Ssend_init" )

#define SCOREP_SCORE_EVENT_IRECVREQUEST  \
    SCOREP_SCORE_EVENT( "MPI_Irecv" )      \
    SCOREP_SCORE_EVENT( "MPI_Irecv_init" )

#define SCOREP_SCORE_EVENT_RECV                \
    SCOREP_SCORE_EVENT( "MPI_Sendrecv" )         \
    SCOREP_SCORE_EVENT( "MPI_Sendrecv_replace" ) \
    SCOREP_SCORE_EVENT( "MPI_Recv" )

#define SCOREP_SCORE_EVENT_IRECV         \
    SCOREP_SCORE_EVENT( "MPI_Irecv" )      \
    SCOREP_SCORE_EVENT( "MPI_Irecv_init" )

#define SCOREP_SCORE_EVENT_COLLECTIVE              \
    SCOREP_SCORE_EVENT( "MPI_Allgather" )            \
    SCOREP_SCORE_EVENT( "MPI_Allgatherv" )           \
    SCOREP_SCORE_EVENT( "MPI_Allreduce" )            \
    SCOREP_SCORE_EVENT( "MPI_Alltoall" )             \
    SCOREP_SCORE_EVENT( "MPI_Alltoallv" )            \
    SCOREP_SCORE_EVENT( "MPI_Alltoallw" )            \
    SCOREP_SCORE_EVENT( "MPI_Barrier" )              \
    SCOREP_SCORE_EVENT( "MPI_Bcast" )                \
    SCOREP_SCORE_EVENT( "MPI_Exscan" )               \
    SCOREP_SCORE_EVENT( "MPI_Gather" )               \
    SCOREP_SCORE_EVENT( "MPI_Gatherv" )              \
    SCOREP_SCORE_EVENT( "MPI_Reduce" )               \
    SCOREP_SCORE_EVENT( "MPI_Reduce_scatter" )       \
    SCOREP_SCORE_EVENT( "MPI_Reduce_scatter_block" ) \
    SCOREP_SCORE_EVENT( "MPI_Scan" )                 \
    SCOREP_SCORE_EVENT( "MPI_Scatter" )              \
    SCOREP_SCORE_EVENT( "MPI_Scatterv" )


#define SCOREP_SCORE_EVENT_FORK \
    SCOREP_SCORE_EVENT( "!$omp parallel" )

#define SCOREP_SCORE_EVENT_JOIN \
    SCOREP_SCORE_EVENT( "!$omp parallel" )

#define SCOREP_SCORE_EVENT_ACQUIRELOCK \
    SCOREP_SCORE_EVENT( "omp_set_lock" ) \
    SCOREP_SCORE_EVENT( "omp_set_nest_lock" )

#define SCOREP_SCORE_EVENT_RELEASELOCK \
    SCOREP_SCORE_EVENT( "omp_unset_lock" ) \
    SCOREP_SCORE_EVENT( "omp_unset_nest_lock" )

#endif

/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2013,
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

#ifndef SCOREP_INTERNAL_IPC_H
#define SCOREP_INTERNAL_IPC_H



/**
 * @file       src/measurement/scorep_ipc.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include <stdbool.h>
#include <stdint.h>

/**
 *  Is a multi program paradigm possible at all.
 */
bool
SCOREP_Ipc_Possible( void );

/**
 *  Get the number of processes in this parallel program.
 */
void
SCOREP_Ipc_Init( void );


/**
 *  Get the number of processes in this parallel program.
 */
void
SCOREP_Ipc_Finalize( void );


/**
 *  Get the number of processes in this parallel program.
 */
int
SCOREP_Ipc_GetSize( void );


/**
 * Get the rank of the process.
 *
 * @return In MPI mode we will get the rank of the process in MPI_COMM_WORLD's
 * group, i.e. MPI_Comm_rank(MPI_COMM_WORLD, &rank). In any other mode we will
 * get 0. Calls to this function before MPI_Init() will fail.
 */
int
SCOREP_Ipc_GetRank( void );


// list of used ipc datatypes
#define SCOREP_IPC_DATATYPES \
    SCOREP_IPC_DATATYPE( BYTE ) \
    SCOREP_IPC_DATATYPE( CHAR ) \
    SCOREP_IPC_DATATYPE( UNSIGNED_CHAR ) \
    SCOREP_IPC_DATATYPE( INT ) \
    SCOREP_IPC_DATATYPE( UNSIGNED ) \
    SCOREP_IPC_DATATYPE( INT32 ) \
    SCOREP_IPC_DATATYPE( UINT32 ) \
    SCOREP_IPC_DATATYPE( INT64 ) \
    SCOREP_IPC_DATATYPE( UINT64 ) \
    SCOREP_IPC_DATATYPE( DOUBLE )

typedef enum SCOREP_Ipc_Datatype
{
    #define SCOREP_IPC_DATATYPE( datatype ) \
    SCOREP_IPC_ ## datatype,
    SCOREP_IPC_DATATYPES
    #undef SCOREP_IPC_DATATYPE
    SCOREP_IPC_NUMBER_OF_DATATYPES
} SCOREP_Ipc_Datatype;


// list of used ipc operations
#define SCOREP_IPC_OPERATIONS \
    SCOREP_IPC_OPERATION( BAND ) \
    SCOREP_IPC_OPERATION( BOR ) \
    SCOREP_IPC_OPERATION( MIN ) \
    SCOREP_IPC_OPERATION( MAX ) \
    SCOREP_IPC_OPERATION( SUM )

typedef enum SCOREP_Ipc_Operation
{
    #define SCOREP_IPC_OPERATION( op ) \
    SCOREP_IPC_ ## op,
    SCOREP_IPC_OPERATIONS
    #undef SCOREP_IPC_OPERATION
    SCOREP_IPC_NUMBER_OF_OPERATIONS
} SCOREP_Ipc_Operation;


int
SCOREP_Ipc_Send( void*               buf,
                 int                 count,
                 SCOREP_Ipc_Datatype datatype,
                 int                 dest );

int
SCOREP_Ipc_Recv( void*               buf,
                 int                 count,
                 SCOREP_Ipc_Datatype datatype,
                 int                 source );

int
SCOREP_Ipc_Barrier( void );

int
SCOREP_Ipc_Bcast( void*               buf,
                  int                 count,
                  SCOREP_Ipc_Datatype datatype,
                  int                 root );

int
SCOREP_Ipc_Gather( void*               sendbuf,
                   void*               recvbuf,
                   int                 count,
                   SCOREP_Ipc_Datatype datatype,
                   int                 root );

int
SCOREP_Ipc_Gatherv( void*               sendbuf,
                    int                 sendcount,
                    void*               recvbuf,
                    int*                recvcnts,
                    int*                displs,
                    SCOREP_Ipc_Datatype datatype,
                    int                 root );

int
SCOREP_Ipc_Allgather( void*               sendbuf,
                      void*               recvbuf,
                      int                 count,
                      SCOREP_Ipc_Datatype datatype );

int
SCOREP_Ipc_Reduce( void*                sendbuf,
                   void*                recvbuf,
                   int                  count,
                   SCOREP_Ipc_Datatype  datatype,
                   SCOREP_Ipc_Operation operation,
                   int                  root );

int
SCOREP_Ipc_Allreduce( void*                sendbuf,
                      void*                recvbuf,
                      int                  count,
                      SCOREP_Ipc_Datatype  datatype,
                      SCOREP_Ipc_Operation operation );

int
SCOREP_Ipc_Scan( void*                sendbuf,
                 void*                recvbuf,
                 int                  count,
                 SCOREP_Ipc_Datatype  datatype,
                 SCOREP_Ipc_Operation operation );

#endif /* SCOREP_INTERNAL_IPC_H */
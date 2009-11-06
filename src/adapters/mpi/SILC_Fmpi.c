/****************************************************************************
**  SCALASCA    http://www.scalasca.org/                                   **
**  KOJAK       http://www.fz-juelich.de/jsc/kojak/                        **
*****************************************************************************
**  Copyright (c) 1998-2009                                                **
**  Forschungszentrum Juelich, Juelich Supercomputing Centre               **
**                                                                         **
**  Copyright (c) 2003-2008                                                **
**  University of Tennessee, Innovative Computing Laboratory               **
**                                                                         **
**  See the file COPYRIGHT in the package base directory for details       **
****************************************************************************/

/** @file  SILC_Fmpi.c
    @brief Main file for Fortran interface measurement wrappers.

    Includes all Fortran wrappers for MPI functions. Fortran compilers decorate the
    C-calls in different ways. Thus, the macro FSUB must be set to SILC_FORTRAN_SUB1,
    SILC_FORTRAN_SUB2, SILC_FORTRAN_SUB3, or SILC_FORTRAN_SUB4 to obtain correctly
    decorated functions. The form of the decoration depends on the used compiler.

    You can compile this wrappers four times with different decorations, to have all
    possible names for the Fortran wrappers available.

    The wrapped decorated functions call the C-wrappers for MPI. Thus, the same set of
    events are generated as with the C functions.
 */

#include "SILC_Fortran_Wrapper.h"
#include "SILC_Error.h"
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#if defined( __sun ) || defined( _SX ) || defined( OPEN_MPI ) || defined( HP_MPI )
#  define NEED_F2C_CONV
#endif

/*
 *-----------------------------------------------------------------------------
 *
 * name Environmental Management
 *
 *-----------------------------------------------------------------------------
 */

#include "SILC_Fmpi_Env.c"

/*
 *-----------------------------------------------------------------------------
 *
 * External interfaces
 *
 *-----------------------------------------------------------------------------
 */

#include "SILC_Fmpi_Ext.c"

/*
 *-----------------------------------------------------------------------------
 *
 * Datatypes
 *
 *-----------------------------------------------------------------------------
 */

#include "SILC_Fmpi_Type.c"

/*
 *-----------------------------------------------------------------------------
 *
 * Miscelaneous
 *
 *-----------------------------------------------------------------------------
 */

#include "SILC_Fmpi_Misc.c"

/*
 *-----------------------------------------------------------------------------
 *
 * Communicator management
 *
 *-----------------------------------------------------------------------------
 */

#include "SILC_Fmpi_Cg.c"

/*
 *-----------------------------------------------------------------------------
 *
 * Cartesian Toplogy functions
 *
 *-----------------------------------------------------------------------------
 */

#include "SILC_Fmpi_Topo.c"

/*
 *-----------------------------------------------------------------------------
 *
 * Point-to-point communication
 *
 *-----------------------------------------------------------------------------
 */

#include "SILC_Fmpi_P2p.c"

/*
 *-----------------------------------------------------------------------------
 *
 * Collective communication
 *
 *-----------------------------------------------------------------------------
 */

#include "SILC_Fmpi_Coll.c"

/*
 *-----------------------------------------------------------------------------
 *
 * One-sided Communication
 *
 *-----------------------------------------------------------------------------
 */

#ifdef HAS_MPI2_1SIDED
#include "SILC_Fmpi_Rma.c"
#endif

/*
 *-----------------------------------------------------------------------------
 *
 * Parallel I/O
 *
 *-----------------------------------------------------------------------------
 */
#ifdef HAS_MPI2_IO
#include "SILC_Fmpi_Io.c"
#endif

/*
 *-----------------------------------------------------------------------------
 *
 * Process Creation and Management
 *
 *-----------------------------------------------------------------------------
 */

#ifdef HAS_MPI2_SPAWN
#include "SILC_Fmpi_Spawn.c"
#endif

/*
 *-----------------------------------------------------------------------------
 *
 * Error Handlers
 *
 *-----------------------------------------------------------------------------
 */

#include "SILC_Fmpi_Err.c"

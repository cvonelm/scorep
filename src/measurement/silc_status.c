/*
 * This file is part of the SILC project (http://www.silc.de)
 *
 * Copyright (c) 2009-2011,
 *    RWTH Aachen, Germany
 *    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
 *    Technische Universitaet Dresden, Germany
 *    University of Oregon, Eugene, USA
 *    Forschungszentrum Juelich GmbH, Germany
 *    Technische Universitaet Muenchen, Germany
 *
 * See the COPYING file in the package base directory for details.
 *
 */


/**
 * @file       silc_status.c
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#include "silc_status.h"

#include <limits.h>
#include <assert.h>


/* *INDENT-OFF* */
/* *INDENT-ON*  */


typedef struct silc_status silc_status;
struct silc_status
{
    int  mpi_rank;
    bool mpi_is_initialized;
    bool mpi_is_finalized;
    bool is_experiment_dir_created;
    bool is_profiling_enabled;
    bool is_tracing_enabled;
    bool otf2_has_flushed;
};


static silc_status status = {
    INT_MAX,             // mpi_rank
    false,               // mpi_is_initialized
    false,               // mpi_is_finalized
    false,               // is_experiment_dir_created
    false,               // is_profiling_enabled
    true,                // is_tracing_enabled
    false                // otf2_has_flushed
};


void
silc_status_initialize_mpi()
{
    // nothing to to here
}


void
silc_status_initialize_non_mpi()
{
    status.mpi_is_initialized = true;
    status.mpi_is_finalized   = true;
    status.mpi_rank           = 0;
}


void
SILC_Mpi_SetIsInitialized()
{
    assert( !status.mpi_is_initialized );
    assert( !status.mpi_is_finalized );
    status.mpi_is_initialized = true;
}

void
SILC_Mpi_SetIsFinalized()
{
    assert( status.mpi_is_initialized );
    assert( !status.mpi_is_finalized );
    status.mpi_is_finalized = true;
}


void
SILC_Mpi_SetRankTo( int rank )
{
    assert( status.mpi_is_initialized );
    assert( !status.mpi_is_finalized );
    assert( rank >= 0 );
    status.mpi_rank = rank;
}


int
SILC_Mpi_GetRank()
{
    assert( status.mpi_is_initialized );
    return status.mpi_rank;
}


bool
SILC_Mpi_IsInitialized()
{
    return status.mpi_is_initialized;
}


bool
SILC_Mpi_IsFinalized()
{
    return status.mpi_is_finalized;
}


bool
SILC_IsTracingEnabled()
{
    return status.is_tracing_enabled;
}


bool
SILC_IsProfilingEnabled()
{
    return status.is_profiling_enabled;
}


void
SILC_Otf2_SetHasFlushed()
{
    status.otf2_has_flushed = true;
}


bool
SILC_Otf2_HasFlushed()
{
    return status.otf2_has_flushed;
}


bool
SILC_IsExperimentDirCreated()
{
    return status.is_experiment_dir_created;
}


void
SILC_SetExperimentDirIsCreated()
{
    assert( !status.is_experiment_dir_created );
    status.is_experiment_dir_created = true;
}

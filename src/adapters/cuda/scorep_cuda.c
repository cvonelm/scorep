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
 *  @status     alpha
 *  @file       scorep_cuda.c
 *  @maintainer Robert Dietrich <robert.dietrich@zih.tu-dresden.de>
 *
 *  Implementation of commonly used functionality within the CUDA adapter.
 */

#include <config.h>

#include <UTILS_Error.h>
#include <UTILS_Debug.h>

#include "scorep_cuda.h"
#include "scorep_cupti.h"


uint8_t scorep_cuda_record_kernels = 0;

uint8_t scorep_cuda_record_idle = 0;

bool scorep_cuda_record_memcpy = false;

uint8_t scorep_cuda_sync_level = 0;

bool scorep_cuda_stream_reuse = false;

bool scorep_cuda_device_reuse = false;

uint8_t scorep_cuda_record_gpumemusage = 0;

/* handles for CUDA communication unification */
SCOREP_InterimCommunicatorHandle scorep_cuda_interim_communicator_handle =
    SCOREP_INVALID_INTERIM_COMMUNICATOR;
SCOREP_InterimRmaWindowHandle scorep_cuda_interim_window_handle =
    SCOREP_INVALID_INTERIM_RMA_WINDOW;

size_t scorep_cuda_global_location_number = 0;

uint64_t* scorep_cuda_global_location_ids = NULL;

void
scorep_cuda_set_features()
{
    /* check for CUDA kernels */
    if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_KERNEL ) == SCOREP_CUDA_FEATURE_KERNEL )
    {
        scorep_cuda_record_kernels = SCOREP_CUDA_KERNEL;

        /* check for idle */
        if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_IDLE ) == SCOREP_CUDA_FEATURE_IDLE )
        {
            scorep_cuda_record_idle = SCOREP_CUDA_COMPUTE_IDLE;
        }

        if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_KERNEL_COUNTER ) == SCOREP_CUDA_FEATURE_KERNEL_COUNTER )
        {
            scorep_cuda_record_kernels = SCOREP_CUDA_KERNEL_AND_COUNTER;
        }
    }

    /* check for CUDA memcpy */
    if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_MEMCPY ) == SCOREP_CUDA_FEATURE_MEMCPY )
    {
        scorep_cuda_record_memcpy = true;

        if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_PURE_IDLE ) == SCOREP_CUDA_FEATURE_PURE_IDLE )
        {
            scorep_cuda_record_idle = SCOREP_CUDA_PURE_IDLE;
        }
    }

    /* check for CUDA GPU memory usage */
    if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_GPUMEMUSAGE ) == SCOREP_CUDA_FEATURE_GPUMEMUSAGE )
    {
        scorep_cuda_record_gpumemusage = SCOREP_CUDA_GPUMEMUSAGE;
    }

    /* check for features, which are only available, when something is recorded on the device */
    if ( scorep_cuda_record_kernels || scorep_cuda_record_memcpy || scorep_cuda_record_gpumemusage )
    {
        if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_STREAM_REUSE )
             == SCOREP_CUDA_FEATURE_STREAM_REUSE )
        {
            scorep_cuda_stream_reuse = true;
        }

        if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_DEVICE_REUSE )
             == SCOREP_CUDA_FEATURE_DEVICE_REUSE )
        {
            scorep_cuda_device_reuse = true;
        }

        if ( ( scorep_cuda_features & SCOREP_CUDA_FEATURE_SYNC )
             == SCOREP_CUDA_FEATURE_SYNC )
        {
            scorep_cuda_sync_level = SCOREP_CUDA_RECORD_SYNC_FULL;
        }
    }
}

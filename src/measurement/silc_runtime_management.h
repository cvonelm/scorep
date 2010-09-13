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

#ifndef SILC_INTERNAL_RUNTIME_MANAGEMENT_H
#define SILC_INTERNAL_RUNTIME_MANAGEMENT_H



/**
 * @file       silc_runtime_management.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */

#include <otf2/OTF2_Archive.h>
#include <SILC_Thread_Types.h>

/**
 * Toplevel relative experiment directory. In the non MPI case a valid name is
 * available after the first call to SILC_CreateExperimentDir(). In the MPI
 * case a valid and unique name is available on all processes after the call
 * to SILC_CreateExperimentDir() from within SILC_InitMeasurementMPI().
 *
 * @note The name is a temporary name used during measurement. At
 * silc_finalize() we rename the temporary directory to something like
 * silc_<prgname>_nProcs_x_nLocations.
 *
 * @todo rename directory in silc_finalize().
 */
char*
SILC_GetExperimentDirName();


/**
 * Create a directory with a temporary name (accessible via
 * SILC_GetExperimentDirName()) to store all measurment data inside.
 *
 * @note There is a MPI implementation and a non-MPI implementation.
 *
 */
void
SILC_CreateExperimentDir();


void
SILC_RenameExperimentDir();


uint64_t
SILC_CalculateGlobalLocationId( SILC_Thread_LocationData* locationData );


void
SILC_DeferLocationInitialization( SILC_Thread_LocationData* locationData );


void
SILC_ProcessDeferredLocations();


/**
 * Archive for trace data. One per process, can contain multiple "location"
 * writers.
 */
extern OTF2_Archive* silc_otf2_archive;


/** @brief Did the first buffer flush happened, of so we can't switch to MPI
 *  anymore.
 */
extern bool flush_done;


/**
 * Called per location by OTF2 after the location buffer has been flushed.
 *
 * @return The clock ticks of this event.
 */
uint64_t
SILC_OnTraceAndDefinitionPostFlush( void );


/**
 * Called per location by OTF2 before the location buffer will be flushed.
 *
 */
OTF2_FlushType
SILC_OnTracePreFlush( void* evtWriter,
                      void* evtReader );


#endif /* SILC_INTERNAL_RUNTIME_MANAGEMENT_H */

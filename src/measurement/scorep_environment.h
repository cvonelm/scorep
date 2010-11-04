/*
 * This file is part of the SCOREP project (http://www.scorep.de)
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


/**
 * @file       scorep_environment.h
 * @maintainer Christian R&ouml;ssel <c.roessel@fz-juelich.de>
 *
 * @status alpha
 *
 */


#ifndef SCOREP_INTERNAL_ENVIRONMENT_H
#define SCOREP_INTERNAL_ENVIRONMENT_H


#include <stdbool.h>
#include <stdint.h>


void
SCOREP_Env_InitializeCoreEnvironmentVariables();


//bool
//SCOREP_Env_CoreEnvironmentVariablesInitialized();


bool
SCOREP_Env_RunVerbose();


bool
SCOREP_Env_DoTracing();


bool
SCOREP_Env_DoProfiling();


uint64_t
SCOREP_Env_GetTotalMemory();


uint64_t
SCOREP_Env_GetPageSize();


#endif /* SCOREP_INTERNAL_ENVIRONMENT_H */

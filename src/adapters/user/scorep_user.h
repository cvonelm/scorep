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
 *  @file       src/adapters/users/scorep_user.h
 *  @maintainer Daniel Lorenz <d.lorenz@fz-juelich.de>
 *  @status     alpha
 *
 */

#ifndef SCOREP_INTERNAL_USER_H
#define SCOREP_INTERNAL_USER_H

void
scorep_user_init_regions();

void
scorep_user_final_regions();

void
scorep_user_init_metric();

void
scorep_user_final_metric();

void
scorep_selective_init();

SCOREP_Error_Code
scorep_selective_register();

void
scorep_selective_finalize();

extern size_t scorep_user_subsystem_id;

#endif /* SCOREP_INTERNAL_USER_H */

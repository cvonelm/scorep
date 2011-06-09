/*
 * This file is part of the Score-P software (http://www.score-p.org)
 *
 * Copyright (c) 2009-2011,
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

#ifndef SCOREP_CONFIG_H
#define SCOREP_CONFIG_H


#include <stdio.h>


/**
 * @file        SCOREP_Config.h
 * @maintainer  Bert Wesarg <Bert.Wesarg@tu-dresden.de>
 *
 * @brief Runtime configuration subsystem.
 *
 */


/**
 * @defgroup SCOREP_Config SCOREP Configuration

 * To centralize the reading and parsing of the configuration the adapters
   need to announce their configuration options to the measurement system.

 * The configuration will, beside environment variables, also be read from
   configuration files. All these sources will have a defined priority. With
   the environment variables as the highest one (this may change, if we can
   pass parameters from the tool wrapper to the measurement system, but
   the tool could also pass the parameters per environment variables to the
   measurement system).

 * With this centralizing the measurement system has also the ability to write
   the configuration of the measurement into the experiment directory.

 * The tools can also provide a way to show all known configuration options
   with their current and default value and a appropriate help description to
   the user.

 * There is a limitation of 32 characters, per namespace, per variable name.

 * Namespaces and variables can consists only of alpha numeric characters
   and varialbes can also have "_" - underscore ( but not at the beginning
   or end).

 * @{
 */


#include <SCOREP_Types.h>
#include <scorep_utility/SCOREP_Error.h>
#include <stddef.h>
#include <stdbool.h>


SCOREP_Error_Code
SCOREP_ConfigInit( void );


void
SCOREP_ConfigFini( void );


/**
 * Register a set of configure variables to the measurement system.
 *
 * @param nameSpace The namespace in which these variables reside in.
 *                  (Can be the empty string, for the global namespace.)
 * @param variables Array of type SCOREP_ConfigVariable which will be registered
 *                  to the measurement system. Terminated by
 *                  @a SCOREP_CONFIG_TERMINATOR.

 * Example:
 * @code
 *      bool unify;
 *      SCOREP_ConfigVariable unify_vars[] = {
 *          {
 *              "unify",
 *              SCOREP_CONFIG_TYPE_BOOL,
 *              &unify,
 *              NULL,
 *              "true", // default value as string
 *              "Unify trace files after the measurement",
 *              "long help\nwith line breaks\n\nwill be nicely aligned."
 *          },
 *          SCOREP_CONFIG_TERMINATOR
 *      };
 *      :
 *      SCOREP_ConfigRegister( "", unify_vars ); // in global namespace
 * @endcode

 * @note the @a variables array will not be referenced from the measurement
 *       system after the call. But most of the members of the variables need
 *       to be valid after the call.
 *       These are:
 *        @li @a SCOREP_ConfigVariable::variableReference
 *            (reason: obvious)
 *        @li @a SCOREP_ConfigVariable::variableContext
 *            (reason: obvious)
 *        @li @a SCOREP_ConfigVariable::defaultValue
 *            (reason: for resetting to the default value)

 * @return Successful registration or failure
 */
SCOREP_Error_Code
SCOREP_ConfigRegister( const char*            nameSpace,
                       SCOREP_ConfigVariable* variables );


SCOREP_Error_Code
SCOREP_ConfigApplyEnv( void );


SCOREP_Error_Code
SCOREP_ConfigDump( FILE* dumpFile );


/**
 * @brief Terminates an array of SCOREP_ConfigVariable.
 */
#define SCOREP_CONFIG_TERMINATOR { \
        NULL, \
        SCOREP_INVALID_CONFIG_TYPE, \
        NULL, \
        NULL, \
        NULL, \
        NULL, \
        NULL  \
}

/**
 * @ToDo how can the online measurement system change configs?
 * This interface will be private to the measurement system.
 */


/*
 * @}
 */


#endif /* SCOREP_CONFIG_H */

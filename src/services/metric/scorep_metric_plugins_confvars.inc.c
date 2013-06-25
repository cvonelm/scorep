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


/**
 * @file       src/services/metric/scorep_metric_plugins_confvars.inc.c
 * @maintainer Ronny Tschueter <ronny.tschueter@tu-dresden.de>
 *
 * @status alpha
 *
 */


/** Contains the name of requested metrics. */
static char* scorep_metrics_plugins = NULL;

/** Contains the separator of metric names. */
static char* scorep_metrics_plugins_separator = NULL;

/** List of configuration variables for the resource usage metric source.
 *
 *  This list contains variables to specify 'strictly synchronous' and
 *  per-process metrics. Furthermore, a variable for the character
 *  that separates single metric names is defined.
 *
 *  Configuration variables for the resource usage metric source.
 *  Current configuration variables are:
 *  @li @c SCOREP_METRIC_PLUGINS list of requested metric names.
 */
static SCOREP_ConfigVariable scorep_metric_plugins_configs[] = {
    {
        "plugins",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metrics_plugins,
        NULL,
        "",
        "Specify list of used plugins",
        "List of requested metric plugin names that will be used during program run."
    },
    {
        "plugins_sep",
        SCOREP_CONFIG_TYPE_STRING,
        &scorep_metrics_plugins_separator,
        NULL,
        ",",
        "Separator of plugin names",
        "Character that separates plugin names in SCOREP_METRIC_PLUGINS."
    },
    SCOREP_CONFIG_TERMINATOR
};

/** Maximum number of plugins in use */
#define PLUGINS_MAX 20

/** Container for additional environment variables. During runtime these
 *  variables are provided to plugins. Plugin can use the first item of
 *  @ config_variables to specify selected metrics. The last item of
 *  @ config_variables must be used for @ SCOREP_CONFIG_TERMINATOR. */
typedef struct additional_environment_variables_container_struct
{
    /** First item: variable to select metrics of an individual plugin, last item: @ SCOREP_CONFIG_TERMINATOR */
    SCOREP_ConfigVariable config_variables[ 2 ];
    /** String containing all select metrics of an individual plugin */
    char*                 event_variable;
} additional_environment_variables_container;


/** Array of additional environment variables data. Each used plugin
 *  will get an entry. Plugins should save their index to access
 *  corresponding variables. */
static additional_environment_variables_container additional_environment_variables[ PLUGINS_MAX ];
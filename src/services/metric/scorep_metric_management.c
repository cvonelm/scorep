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
 *  @file       scorep_metric_management.c
 *  @author     Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *  @maintainer Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *
 *  @brief This module handles implementations of different metric sources.
 */

#include <config.h>

#include <stdlib.h>
#include <assert.h>

#include "SCOREP_Config.h"
#include <SCOREP_Memory.h>
#include <scorep_utility/SCOREP_Debug.h>

/* Include to implement metric service */
#include "SCOREP_Metric.h"
/* Include to manage different metric sources */
#include "SCOREP_Metric_Source.h"
/* Include to write definitions */
#include "SCOREP_Definitions.h"

#include <SCOREP_Location.h>

/* Include header files of supported metric sources */
#if HAVE( PAPI )
#include "scorep_metric_papi.h"
#endif
#if HAVE( GETRUSAGE )
#include "scorep_metric_rusage.h"
#endif

#include <unistd.h>
#include <sys/types.h>


/** Thread local data related to metrics */
typedef struct SCOREP_Metric_LocationData SCOREP_Metric_LocationData;

/** Thread local data related to metrics */
struct SCOREP_Metric_LocationData
{
    SCOREP_Metric_EventSet** event_set;
    uint64_t*                values;
};

/** List of metric sources. */
static const SCOREP_MetricSource* scorep_metric_sources[] = {
#if HAVE( PAPI )
    &SCOREP_Metric_Papi,
#endif
#if HAVE( GETRUSAGE )
    &SCOREP_Metric_Rusage
#endif
};

/** Number of registered metric sources */
#define SCOREP_NUMBER_OF_METRIC_SOURCES sizeof( scorep_metric_sources ) / sizeof( scorep_metric_sources[ 0 ] )

/** Data structure needed to manage metric sources */
typedef struct scorep_metric_sources_management_data
{
    /** Number of overall active metrics */
    uint64_t overall_number_of_metrics;
    /** Vector that contains number of requested metrics in each metric source */
    uint32_t number_of_metrics_vector[ SCOREP_NUMBER_OF_METRIC_SOURCES ];
    /** Vector of offsets in scorep_number_of_metrics_vector for each metric */
    uint32_t metrics_offset_vector[ SCOREP_NUMBER_OF_METRIC_SOURCES ];
} scorep_metric_sources_management_data;


/** Flag indicating status of metric management */
static bool scorep_metric_management_initialized;

/** Metric sources management data */
static scorep_metric_sources_management_data metric_sources_management_data;

/** Array of all metric handles */
static SCOREP_MetricHandle* all_metric_handles;

/** Handle of sampling set */
static SCOREP_SamplingSetHandle sampling_set_handle;

/** Our subsystem id, used to address our per-location metric datas */
static size_t scorep_metric_subsystem_id;


/* *********************************************************************
 * Service management
 **********************************************************************/

/** @brief Registers configuration variables for the metric services.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_Error_Code
scorep_metric_register( size_t subsystem_id )
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " register metric management." );

    scorep_metric_subsystem_id = subsystem_id;

    /* Register metric sources */
    for ( size_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; i++ )
    {
        scorep_metric_sources[ i ]->metric_source_register();
    }

    return SCOREP_SUCCESS;
}

/** @brief Called on deregistration of the metric service.
 */
static void
scorep_metric_deregister()
{
    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " deregister metric management." );

    /* Deregister metric sources */
    for ( size_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; i++ )
    {
        scorep_metric_sources[ i ]->metric_source_deregister();
    }
}

/** @brief Called on initialization of the metric service.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_Error_Code
scorep_metric_initialize_service()
{
    /* Call only, if not previously initialized */
    if ( !scorep_metric_management_initialized )
    {
        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialize metric management." );

        uint32_t num_of_metrics;
        for ( size_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; i++ )
        {
            /* Initialize metric source and get number of active metrics for this source */
            metric_sources_management_data.number_of_metrics_vector[ i ] = scorep_metric_sources[ i ]->metric_source_initialize();

            /* Number of active metrics (valid up to now) indicates the required offset we have to use,
            * if we want to store values from this source in the common values array at a later time */
            metric_sources_management_data.metrics_offset_vector[ i ] = metric_sources_management_data.overall_number_of_metrics;

            /* Update number of active metrics */
            metric_sources_management_data.overall_number_of_metrics += metric_sources_management_data.number_of_metrics_vector[ i ];
        }

        sampling_set_handle = SCOREP_INVALID_SAMPLING_SET;
        if ( metric_sources_management_data.overall_number_of_metrics != 0 )
        {
            /* Now we know how many metrics are used, so we can allocate memory to store their handles */
            all_metric_handles = malloc( metric_sources_management_data.overall_number_of_metrics * sizeof( SCOREP_MetricHandle ) );
            assert( all_metric_handles );

            uint64_t recent_metric_index = 0;
            for ( size_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; i++ )
            {
                for ( uint32_t j = 0; j < metric_sources_management_data.number_of_metrics_vector[ i ]; j++ )
                {
                    /* Get properties of used metrics */
                    SCOREP_Metric_Properties props = scorep_metric_sources[ i ]->metric_source_props( j );

                    /* Write metric member definition */
                    SCOREP_MetricHandle metric_handle = SCOREP_DefineMetric( props.name,
                                                                             props.description,
                                                                             props.source_type,
                                                                             props.mode,
                                                                             props.value_type,
                                                                             props.base,
                                                                             props.exponent,
                                                                             props.unit,
                                                                             props.profiling_type );
                    all_metric_handles[ recent_metric_index++ ] = metric_handle;
                }
            }

            /* Write definition of sampling set */
            sampling_set_handle = SCOREP_DefineSamplingSet( ( uint8_t )metric_sources_management_data.overall_number_of_metrics,
                                                            all_metric_handles,
                                                            SCOREP_METRIC_OCCURRENCE_SYNCHRONOUS_STRICT );
        }

        /* Set initialization flag */
        scorep_metric_management_initialized = true;

        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " initialization of metric management done." );
    }

    return SCOREP_SUCCESS;
}

/** @brief Service finalization.
 */
static void
scorep_metric_finalize_service()
{
    /* Call only, if previously initialized */
    if ( scorep_metric_management_initialized )
    {
        for ( size_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; i++ )
        {
            /* Finalize metric source */
            scorep_metric_sources[ i ]->metric_source_finalize();
            /* Reset number of requested metrics for recent source */
            metric_sources_management_data.number_of_metrics_vector[ i ] = 0;
        }
        metric_sources_management_data.overall_number_of_metrics = 0;
        sampling_set_handle                                      = SCOREP_INVALID_SAMPLING_SET;

        free( all_metric_handles );

        /* Set initialization flag */
        scorep_metric_management_initialized = false;

        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " finalization of metric management done." );
    }
}

static void
initialize_location_metric_cb( SCOREP_Location* locationData,
                               void*            data )
{
    /* Call only, if previously initialized */
    if ( scorep_metric_management_initialized )
    {
        /* Get the thread local data related to metrics */
        SCOREP_Metric_LocationData* metric_data = SCOREP_Location_GetSubsystemLocationData(
            locationData,
            scorep_metric_subsystem_id );
        SCOREP_ASSERT( metric_data != NULL );

        /* Allocate memory for array of event sets in thread local storage */
        metric_data->event_set = malloc( SCOREP_NUMBER_OF_METRIC_SOURCES * sizeof( SCOREP_Metric_EventSet* ) );
        assert( metric_data->event_set );

        /* Initialize metric sources for this thread */
        for ( size_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; i++ )
        {
            if ( metric_sources_management_data.number_of_metrics_vector[ i ] > 0 )
            {
                /* Set up event sets for this thread if the metric source provide some metrics */
                metric_data->event_set[ i ] = scorep_metric_sources[ i ]->metric_source_initialize_location();
            }
            else
            {
                /* Metric source does not provide any metrics, invalidate corresponding event_set pointer */
                metric_data->event_set[ i ] = NULL;
            }
        }

        if ( metric_sources_management_data.overall_number_of_metrics > 0 )
        {
            /* Allocate thread local result buffer */
            metric_data->values = malloc( metric_sources_management_data.overall_number_of_metrics * sizeof( uint64_t ) );
            SCOREP_ASSERT( metric_data->values );
        }

        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric management has initialized location." );
    }
}

/** @brief  Location specific initialization function for metric services.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
static SCOREP_Error_Code
scorep_metric_initialize_location()
{
    /* Get the thread local data */
    SCOREP_Location* thread_data = SCOREP_Location_GetCurrentCPUThreadData();
    SCOREP_ASSERT( thread_data != NULL );

    SCOREP_Metric_LocationData* metric_data =
        SCOREP_Memory_AllocForMisc( sizeof( *metric_data ) );

    SCOREP_Location_SetSubsystemLocationData( thread_data,
                                              scorep_metric_subsystem_id,
                                              metric_data );

    initialize_location_metric_cb( thread_data, NULL );

    return SCOREP_SUCCESS;
}

static void
finalize_location_metric_cb( SCOREP_Location* locationData,
                             void*            data )
{
    /* Call only, if previously initialized */
    if ( scorep_metric_management_initialized )
    {
        for ( size_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; i++ )
        {
            scorep_metric_sources[ i ]->metric_source_finalize_location( NULL );
        }

        /* Get the thread local data related to metrics */
        SCOREP_Metric_LocationData* metric_data = SCOREP_Location_GetSubsystemLocationData(
            locationData,
            scorep_metric_subsystem_id );
        SCOREP_ASSERT( metric_data != NULL );

        free( metric_data->event_set );
        free( metric_data->values );

        SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric management has finalized location." );
    }
}

/** @brief Location specific finalization function for metric services.
 *
 *  @param location Reference to location that will finalize its metric related data structures.
 */
static void
scorep_metric_finalize_location( SCOREP_Location* locationData )
{
    finalize_location_metric_cb( locationData, NULL );
}


/* *********************************************************************
 * Functions called directly by measurement environment
 **********************************************************************/

/** @brief  Get recent values of all metrics.
 *
 *  @return Returns pointer to value array filled with recent metric values,
 *          or NULL if we don't have metrics to read from.
 */
uint64_t*
SCOREP_Metric_read( SCOREP_Location* locationData )
{
    /* Call only, if previously initialized, and we have at least one metric
     * to read
     */
    if ( !scorep_metric_management_initialized
         || 0 == metric_sources_management_data.overall_number_of_metrics )
    {
        return NULL;
    }

    /* Get the thread local data related to metrics */
    SCOREP_Metric_LocationData* metric_data = SCOREP_Location_GetSubsystemLocationData(
        locationData,
        scorep_metric_subsystem_id );
    SCOREP_ASSERT( metric_data != NULL );

    assert( metric_data->event_set != NULL );
    assert( metric_data->values != NULL );

    for ( size_t i = 0; i < SCOREP_NUMBER_OF_METRIC_SOURCES; i++ )
    {
        if ( metric_sources_management_data.number_of_metrics_vector[ i ] > 0 )
        {
            /* Read values of selected metric */
            scorep_metric_sources[ i ]->metric_source_read( metric_data->event_set[ i ], &metric_data->values[ metric_sources_management_data.metrics_offset_vector[ i ] ] );
        }
    }

    SCOREP_DEBUG_PRINTF( SCOREP_DEBUG_METRIC, " metric management has read metric values." );

    return metric_data->values;
}

/** @brief  Reinitialize metric management. This functionality is used by
 *          Score-P Online Access to change recorded metrics between
 *          separate phases of program execution.
 *
 *  @return It returns SCOREP_SUCCESS if successful,
 *          otherwise an error code will be reported.
 */
SCOREP_Error_Code
SCOREP_Metric_reinitialize()
{
    SCOREP_Error_Code return_value;

    /* Read environment variables to get their recent values. In the usual case the names of recorded metrics will have changed. */
    return_value = SCOREP_ConfigApplyEnv();
    if ( return_value != SCOREP_SUCCESS )
    {
        return return_value;
    }

    /* Finalize each location (frees internal buffers) */
    SCOREP_Location_ForAllLocations( finalize_location_metric_cb, NULL );

    /* Finalize metric service */
    scorep_metric_finalize_service();

    /* Reinitialize metric service */
    scorep_metric_initialize_service();

    /* Reinitialize each location */
    SCOREP_Location_ForAllLocations( initialize_location_metric_cb, NULL );

    return SCOREP_SUCCESS;
}

/** @brief  Returns the sampling set handle to the measurement system.
 *
 *  @return Returns the sampling set handle to the measurement system.
 */
SCOREP_SamplingSetHandle
SCOREP_Metric_GetSamplingSet()
{
    return sampling_set_handle;
}


/* *********************************************************************
 * Subsytem declaration
 **********************************************************************/

/**
   Implementation of the metric service initialization/finalization struct
 */
const SCOREP_Subsystem SCOREP_Metric_Service =
{
    "METRIC",
    &scorep_metric_register,
    &scorep_metric_initialize_service,
    &scorep_metric_initialize_location,
    &scorep_metric_finalize_location,
    &scorep_metric_finalize_service,
    &scorep_metric_deregister
};

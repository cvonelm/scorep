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
 * @file       SCOREP_MetricTypes.h
 * @maintainer Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>
 *
 * @brief Types used by metric service.
 */


#ifndef SCOREP_METRIC_TYPES_H_
#define SCOREP_METRIC_TYPES_H_

#include <stdint.h>

/* *********************************************************************
 * Type definitions
 **********************************************************************/



/**
 * Metric sources to be used in defining a metric member (SCOREP_Definitions_NewMetric()).
 *
 */
typedef enum SCOREP_MetricSourceType
{
    /** PAPI counter. */
    SCOREP_METRIC_SOURCE_TYPE_PAPI   = 0,
    /** Resource usage counter. */
    SCOREP_METRIC_SOURCE_TYPE_RUSAGE = 1,
    /** User metrics. */
    SCOREP_METRIC_SOURCE_TYPE_USER   = 2,
    /** Any other metrics. */
    SCOREP_METRIC_SOURCE_TYPE_OTHER  = 3,
    /** Generated by task profiling. */
    SCOREP_METRIC_SOURCE_TYPE_TASK   = 4,
    /* RonnyT: new type for plugin metrics */
    /** Plugin metrics. */
    SCOREP_METRIC_SOURCE_TYPE_PLUGIN = 5,

    SCOREP_INVALID_METRIC_SOURCE_TYPE /**< For internal use only. */
} SCOREP_MetricSourceType;

/**
 * Types used to define type of profiling.
 */
typedef enum SCOREP_MetricProfilingType
{
    /** Exclusive values (excludes values from subordinated items) */
    SCOREP_METRIC_PROFILING_TYPE_EXCLUSIVE = 0,
    /** Inclusive values (sum including values from subordinated items ) */
    SCOREP_METRIC_PROFILING_TYPE_INCLUSIVE = 1,
    /** Single value */
    SCOREP_METRIC_PROFILING_TYPE_SIMPLE    = 2,
    /** Maximum values */
    SCOREP_METRIC_PROFILING_TYPE_MAX       = 3,
    /** Minimum values */
    SCOREP_METRIC_PROFILING_TYPE_MIN       = 4,

    SCOREP_INVALID_METRIC_PROFILING_TYPE /**< For internal use only. */
} SCOREP_MetricProfilingType;

/**
 * Types to be used in defining type of metric values (SCOREP_Definitions_NewMetric()).
 *
 */
typedef enum SCOREP_MetricValueType
{
    SCOREP_METRIC_VALUE_INT64,
    SCOREP_METRIC_VALUE_UINT64,
    SCOREP_METRIC_VALUE_DOUBLE,

    SCOREP_INVALID_METRIC_VALUE_TYPE /**< For internal use only. */
} SCOREP_MetricValueType;

/**
 * Types to be used in defining metric mode (SCOREP_Definitions_NewMetric()).
 *
 */
typedef enum SCOREP_MetricMode
{
    /** Accumulated metric, 'START' timing. */
    SCOREP_METRIC_MODE_ACCUMULATED_START = 0,
    /** Accumulated metric, 'POINT' timing. */
    SCOREP_METRIC_MODE_ACCUMULATED_POINT = 1,
    /** Accumulated metric, 'LAST' timing. */
    SCOREP_METRIC_MODE_ACCUMULATED_LAST  = 2,
    /** Accumulated metric, 'NEXT' timing. */
    SCOREP_METRIC_MODE_ACCUMULATED_NEXT  = 3,

    /** Absolute metric, 'POINT' timing. */
    SCOREP_METRIC_MODE_ABSOLUTE_POINT    = 4,
    /** Absolute metric, 'LAST' timing. */
    SCOREP_METRIC_MODE_ABSOLUTE_LAST     = 5,
    /** Absolute metric, 'NEXT' timing. */
    SCOREP_METRIC_MODE_ABSOLUTE_NEXT     = 6,

    /** Relative metric, 'POINT' timing. */
    SCOREP_METRIC_MODE_RELATIVE_POINT    = 7,
    /** Relative metric, 'LAST' timing. */
    SCOREP_METRIC_MODE_RELATIVE_LAST     = 8,
    /** Relative metric, 'NEXT' timing. */
    SCOREP_METRIC_MODE_RELATIVE_NEXT     = 9,

    SCOREP_INVALID_METRIC_MODE /**< For internal use only. */
} SCOREP_MetricMode;

/**
 * Types to be used in defining metric base (SCOREP_Definitions_NewMetric()).
 *
 */
typedef enum SCOREP_MetricBase
{
    /** Binary base. */
    SCOREP_METRIC_BASE_BINARY  = 0,
    /** Decimal base. */
    SCOREP_METRIC_BASE_DECIMAL = 1,

    SCOREP_INVALID_METRIC_BASE /**< For internal use only. */
} SCOREP_MetricBase;

/**
 * Properties desribing a metric.
 *
 */
typedef struct SCOREP_Metric_Properties
{
    /** Name of the metric. */
    const char*                name;
    /** Long description of the metric. */
    const char*                description;
    /** Type of the metric source (e.g. PAPI). */
    SCOREP_MetricSourceType    source_type;
    /** Metric mode (valid combination of ACCUMULATED|ABSOLUTE|RELATIVE and POINT|START|LAST|NEXT). */
    SCOREP_MetricMode          mode;
    /** Type of the metric value (INT64, UINT64, or DOUBLE). */
    SCOREP_MetricValueType     value_type;
    /** Base of metric values (DECIMAL or BINARY). */
    SCOREP_MetricBase          base;
    /** Exponent to scale metric values (metric_value = value * base^exponent). */
    int64_t                    exponent;
    /** Unit of the metric. */
    const char*                unit;
    /** Profiling type of the metric. */
    SCOREP_MetricProfilingType profiling_type;
} SCOREP_Metric_Properties;

/**
 * Enumeration to define how often a metric should be measured.
 *
 */
typedef enum SCOREP_MetricPer
{
    /** Metric values recorded per thread */
    SCOREP_METRIC_PER_THREAD = 0,
    /** Metric values recorded per process */
    SCOREP_METRIC_PER_PROCESS,
    /** Metric values recorded per host */
    SCOREP_METRIC_PER_HOST,
    /** Metric values recorded once for the program */
    SCOREP_METRIC_ONCE,

    SCOREP_METRIC_PER_MAX /**< NON-ABI, for internal use only. */
} SCOREP_MetricPer;

/**
 * Enumeration to define the synchronicity type of a metric.
 *
 */
typedef enum SCOREP_MetricSynchronicity
{
    /** Values recorded at every enter/leave event */
    SCOREP_METRIC_STRICTLY_SYNC = 0,
    /** Values recorded at enter/leave events, but no need
     *  to be recorded at every  */
    SCOREP_METRIC_SYNC,
    /** Values recorded at arbitrary points in time, but written
     *  at enter/leave events in the trace */
    SCOREP_METRIC_ASYNC_EVENT,
    /** Values recorded at arbitrary points in time and written
     *  at any position in event stream of trace */
    SCOREP_METRIC_ASYNC,

    SCOREP_METRIC_SYNC_TYPE_MAX /**< NON-ABI, for internal use only. */
} SCOREP_MetricSynchronicity;

/**
 * Pair of Score-P timestamp and corresponding metric value (used by
 * asynchronous metrics).
 *
 */
typedef struct SCOREP_MetricTimeValuePair
{
    /** Timestamp in Score-P time! */
    uint64_t timestamp;
    /** Current metric value */
    uint64_t value;
} SCOREP_MetricTimeValuePair;


#endif /* SCOREP_METRIC_TYPES_H_ */
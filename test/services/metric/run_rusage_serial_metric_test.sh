#!/bin/bash

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2011, 
##    RWTH Aachen University, Germany
##    Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##    Technische Universitaet Dresden, Germany
##    University of Oregon, Eugene, USA
##    Forschungszentrum Juelich GmbH, Germany
##    German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##    Technische Universitaet Muenchen, Germany
##
## See the COPYING file in the package base directory for details.
##

## file       run_rusage_serial_metric_test.sh
## maintainer Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>

# Get SRC_ROOT and OTF2_PRINT
make clean-local-scorep-config-tool
make scorep-config-tool-local
. ./scorep_config.dat

TEST_DATA_DIR=$SRC_ROOT/test/services/metric/data

# Set up directory that will contain experiment results
RESULT_DIR=$(pwd)/scorep-serial-rusage-metric-test-dir
rm -rf $RESULT_DIR

# Run test
SCOREP_EXPERIMENT_DIRECTORY=$RESULT_DIR SCOREP_ENABLE_PROFILING=false SCOREP_ENABLE_TRACING=true SCOREP_METRIC_RUSAGE_SEP=, SCOREP_METRIC_RUSAGE=ru_utime,ru_stime ./jacobi_serial_c_metric_test
if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp
    exit 1
fi
echo "Output of metric test can be found in $RESULT_DIR"

# Check metric definitions
$OTF2_PRINT -G $RESULT_DIR/traces.otf2 |
    grep ^METRIC |
    LC_ALL=C sed -e 's/^METRIC \(MEMBER\|CLASS\|INSTANCE\) \+/METRIC \1 /g' \
                 -e 's/Name: [[:digit:]]\+/Name: <id>/g' \
                 -e 's/Descr\.: [[:digit:]]\+ (.*), Type/Descr.: <id> (<string>), Type/g' \
                 -e 's/Unit: [[:digit:]]\+/Unit: <id>/g' > trace.txt
if diff $TEST_DATA_DIR/jacobi_c_serial_rusage_metric_definitions.out trace.txt > /dev/null
  then
    true
  else
    echo "-------- ERROR: in metric definition output --------" >&2
    diff -u $TEST_DATA_DIR/jacobi_c_serial_rusage_metric_definitions.out trace.txt >&2
    rm trace.txt
    exit 1
fi

# Check metric events
$OTF2_PRINT $RESULT_DIR/traces.otf2 | grep METRIC > trace.txt
if [ x`grep -c METRIC trace.txt` = x`grep -c METRIC $TEST_DATA_DIR/jacobi_c_serial_rusage_metric_events.out` ]; then
    rm trace.txt
    exit 0
  else
    echo "-------- ERROR: in metric event output --------" >&2
    diff -u $TEST_DATA_DIR/jacobi_c_serial_rusage_metric_events.out trace.txt >&2
    rm trace.txt
    exit 1
fi

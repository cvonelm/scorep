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

## file       run_rusage_mpi_metric_test.sh
## maintainer Ronny Tschueter <ronny.tschueter@zih.tu-dresden.de>

## At the moment there is no appropriate make target that allows executing
## shell scripts in MPI tests. Therefore run this script manually from
## inside your build-mpi directory. Furthermore some helpful make targets
## are not available in MPI build mode. As a work-around we define needed
## variables manually.
# Get SRC_ROOT and OTF2_PRINT
OTF2_PRINT=../vendor/otf2/build-backend/otf2-print
SRC_ROOT=../..
## The correct way to access these variables would be:
##make clean-local-scorep-config-tool
##make scorep-config-tool-local
##. ./scorep_config.dat

MPIRUN=mpiexec 

TEST_DATA_DIR=$SRC_ROOT/test/services/metric/data

# Set up directory that will contain experiment results
RESULT_DIR=$PWD/scorep-mpi-rusage-metric-test-dir
rm -rf $RESULT_DIR

# Run test
SCOREP_EXPERIMENT_DIRECTORY=$RESULT_DIR SCOREP_ENABLE_PROFILING=false SCOREP_ENABLE_TRACING=true SCOREP_METRIC_RUSAGE_SEP=, SCOREP_METRIC_RUSAGE=ru_utime,ru_stime $MPIRUN -np 4 ./jacobi_mpi_c_metric_test
if [ $? -ne 0 ]; then
    rm -rf scorep-measurement-tmp
    exit 1
fi
echo "Output of metric test can be found in $RESULT_DIR"

# Check metric definitions
$OTF2_PRINT -G $RESULT_DIR/traces.otf2 |
    grep '^METRIC_' |
    LC_ALL=C sed -e 's/^METRIC_\(MEMBER\|CLASS\|INSTANCE\) \+/METRIC_\1 /g' \
                 -e 's/Name: "\([^"]*\)" <[[:digit:]]\+>/Name: "\1" <id>/g' \
                 -e 's/Descr\.: "[^"]*" <[[:digit:]]\+>/Descr.: "<string>" <id>/g' \
                 -e 's/Unit: "\([^"]*\)" <[[:digit:]]\+>/Unit: "\1" <id>/g' > trace.txt
if diff $TEST_DATA_DIR/jacobi_c_mpi_rusage_metric_definitions.out trace.txt > /dev/null
  then
    true
  else
    echo "-------- ERROR: in metric definition output --------" >&2
    diff -u $TEST_DATA_DIR/jacobi_c_mpi_rusage_metric_definitions.out trace.txt >&2
    rm trace.txt
    exit 1
fi

# Check metric events
$OTF2_PRINT $RESULT_DIR/traces.otf2 | grep '^METRIC ' > trace.txt
if [ x`grep -c METRIC trace.txt` = x`grep -c METRIC $TEST_DATA_DIR/jacobi_c_mpi_rusage_metric_events.out` ]; then
    rm trace.txt
    exit 0
  else
    echo "-------- ERROR: in metric event output --------" >&2
    diff -u $TEST_DATA_DIR/jacobi_c_mpi_rusage_metric_events.out trace.txt >&2
    rm trace.txt
    exit 1
fi

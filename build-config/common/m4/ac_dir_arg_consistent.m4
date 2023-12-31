## -*- mode: autoconf -*-

##
## This file is part of the Score-P software ecosystem (http://www.score-p.org)
##
## Copyright (c) 2009-2011,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2011,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2011,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2011,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2011,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2011,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2011,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##


# AC_DIR_ARG_CONSISTENT(DIR_ARG [, FAILED_OPTION])
# ------------------------------
# Check directory argument DIR_ARG for consistency, i.e. remove trailing
# slashes and abort if argument is not an absolute directory name. Use
# FAILED_OPTION to show the user which option caused an error,  e.g.
# AC_DIR_ARG_CONSISTENT([doxygen_output_dir], [--with-doxygen-output-dir])
#
AC_DEFUN([AC_DIR_ARG_CONSISTENT],
[
for ac_var in $1
do
  eval ac_val=\$$ac_var
  # Remove trailing slashes.
  case $ac_val in
    */ )
      ac_val=`expr "X$ac_val" : 'X\(.*[[^/]]\)' \| "X$ac_val" : 'X\(.*\)'`
      eval $ac_var=\$ac_val;;
  esac
  # Be sure to have absolute directory names.
  case $ac_val in
    [[\\/$]]* | ?:[[\\/]]* )  continue;;
    NONE | '' ) case $ac_var in *prefix ) continue;; esac;;
  esac
  if test $# -gt 1; then
    AC_MSG_ERROR([expected an absolute directory name for $2: $ac_val])
  else
    AC_MSG_ERROR([expected an absolute directory name for --$ac_var: $ac_val])
  fi
done
])

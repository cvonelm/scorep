## -*- mode: autoconf -*-

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


AC_DEFUN([AC_SCOREP_REVISION],
[
    # When working with a svn checkout, write a REVISION file. The REVISION
    # file is updated during each configure call and also at make doxygen-user
    # and make dist.

    # When working with a make-dist-generated tarball, REVISION is already
    # there.

    scorep_revision="invalid"
    which svnversion > /dev/null; \
    if test $? -eq 0; then
        scorep_revision=`svnversion $srcdir`
        if test "x$scorep_revision" != "xexported"; then
            echo $scorep_revision > $srcdir/build-config/REVISION
        fi
    fi

    if grep -E [[A-Z]] $srcdir/build-config/REVISION > /dev/null || \
       grep ":" $srcdir/build-config/REVISION > /dev/null; then
        scorep_revision=`cat $srcdir/build-config/REVISION`
        AC_MSG_WARN([distribution does not match a single, unmodified revision, but $scorep_revision.])
    fi

    AC_SUBST([LIBRARY_CURRENT],  [`sed -n -e 's/library.current=\+// p' ${srcdir}/build-config/VERSION`])
    AC_SUBST([LIBRARY_REVISION], [`sed -n -e 's/library.revision=\+// p' ${srcdir}/build-config/VERSION`])
    AC_SUBST([LIBRARY_AGE],      [`sed -n -e 's/library.age=\+// p' ${srcdir}/build-config/VERSION`])

])

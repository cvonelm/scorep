# -*- mode: makefile -*-

## 
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2013-2014,
## Technische Universitaet Dresden, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file       build-shmem/Makefile.am 


## ACLOCAL_AMFLAGS contains options to pass to aclocal when aclocal.m4
## is to be rebuilt by make. This line is also used by autoreconf to
## run aclocal with suitable options, or by autopoint and gettextize
## to locate the place where Gettext's macros should be installed. So
## even if you do not really care about the rebuild rules, you should
## define ACLOCAL_AMFLAGS.
## For some reason this can't be moved to common.am.
ACLOCAL_AMFLAGS = -I ../vendor/common/build-config/m4 -I ../build-config/m4

include ../build-includes/common.am
include ../vendor/common/build-config/Makefile.tests-shmem.inc.am

AM_CPPFLAGS += -DBACKEND_BUILD_SHMEM

bindir = @bindir@@backend_suffix@
libdir = @libdir@@backend_suffix@

include ../build-includes/shmem-backend-only.am

if CROSS_BUILD
AM_CPPFLAGS += -DCROSS_BUILD
else !CROSS_BUILD
AM_CPPFLAGS += -DNOCROSS_BUILD
endif !CROSS_BUILD

pkgdata_DATA =
if HAVE_SHMEM_SUPPORT
if !HAVE_SHMEM_PROFILING_INTERFACE
pkgdata_DATA += ../share/shmem.wrap
endif !HAVE_SHMEM_PROFILING_INTERFACE
endif HAVE_SHMEM_SUPPORT

## Determine how to handle tests
if CROSS_BUILD
check-shmem:     check-file-shmem
else !CROSS_BUILD
if BACKEND_TEST_RUNS
check-shmem:     check-run-shmem
else !BACKEND_TEST_RUNS
check-shmem:     check-file-shmem
endif !BACKEND_TEST_RUNS
endif !CROSS_BUILD


## Here all libscorep_*.la are build
LIB_DIR_SCOREP = ../build-backend/

clean-local: clean-local-instrumenter-checks
	rm -f lex.yy.c scanner.h yacc.c yacc.h y.tab.h

INSTALLCHECK_SUBDIRS  =
if HAVE_SHMEM_SUPPORT
INSTALLCHECK_SUBDIRS +=
if HAVE_OPENMP_SUPPORT
INSTALLCHECK_SUBDIRS +=
endif HAVE_OPENMP_SUPPORT
endif HAVE_SHMEM_SUPPORT

installcheck-local: instrumenter-checks
	@fail= failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(INSTALLCHECK_SUBDIRS)'; for subdir in $$list; do \
	  echo "Making in $$subdir"; \
	  ($(am__cd) $$subdir && $(MAKE) $(AM_MAKEFLAGS)) \
	  || eval $$failcom; \
	done; test -z "$$fail"

PARADIGMS =
instrumenter_check_paradigm_makefiles =
instrumenter_configuration_files =
if HAVE_SHMEM_SUPPORT
PARADIGMS += shmem
instrumenter_check_paradigm_makefiles += \
    ../installcheck/instrumenter_checks/shmem/Makefile
instrumenter_configuration_files += \
    ../installcheck/instrumenter_checks/configurations_shmem
if HAVE_OPENMP_SUPPORT
PARADIGMS += shmem_omp
instrumenter_check_paradigm_makefiles += \
    ../installcheck/instrumenter_checks/shmem_omp/Makefile
instrumenter_configuration_files += \
    ../installcheck/instrumenter_checks/configurations_shmem_omp
endif HAVE_OPENMP_SUPPORT
endif HAVE_SHMEM_SUPPORT


# These prerequisites are controlled by build-backend/Makefile
foreign_instrumenter_check_prerequisites = \
    ../installcheck/instrumenter_checks/Makefile.common_rules \
    ../installcheck/instrumenter_checks/Makefile.instrument_configurations \
    ../installcheck/instrumenter_checks/instrumenter-configurations.sh \
    ../installcheck/instrumenter_checks/check-instrumentation.sh

$(foreign_instrumenter_check_prerequisites):
	@$(am__cd) ../build-backend && $(MAKE) $(AM_MAKEFLAGS) $@

$(instrumenter_configuration_files): ../installcheck/instrumenter_checks/instrumenter-configurations.sh
	@../installcheck/instrumenter_checks/instrumenter-configurations.sh

instrumenter-checks: $(instrumenter_configuration_files) \
                     $(instrumenter_check_paradigm_makefiles) \
                     $(foreign_instrumenter_check_prerequisites)
	@fail= failcom='exit 1'; \
	for f in x $$MAKEFLAGS; do \
	  case $$f in \
	    *=* | --[!k]*);; \
	    *k*) failcom='fail=yes';; \
	  esac; \
	done; \
	list='$(PARADIGMS)'; for paradigm in $$list; do \
	  echo "Making instrument-configurations in ../installcheck/instrumenter_checks/$$paradigm"; \
	  ($(am__cd) ../installcheck/instrumenter_checks/$$paradigm && $(MAKE) $(AM_MAKEFLAGS) instrument-configurations) \
	  || eval $$failcom; \
	done; test -z "$$fail"

clean-local-instrumenter-checks:
	@rm -f $(instrumenter_configuration_files)
	@for paradigm in $(PARADIGMS); do \
            rm -f ../installcheck/instrumenter_checks/bin/*-$${paradigm}-*; \
            rm -rf ../installcheck/instrumenter_checks/$$paradigm/build; \
        done
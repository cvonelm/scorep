# -*- mode: makefile -*-

##
## This file is part of the Score-P software (http://www.score-p.org)
##
## Copyright (c) 2009-2012,
## RWTH Aachen University, Germany
##
## Copyright (c) 2009-2012,
## Gesellschaft fuer numerische Simulation mbH Braunschweig, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Dresden, Germany
##
## Copyright (c) 2009-2012,
## University of Oregon, Eugene, USA
##
## Copyright (c) 2009-2013,
## Forschungszentrum Juelich GmbH, Germany
##
## Copyright (c) 2009-2012,
## German Research School for Simulation Sciences GmbH, Juelich/Aachen, Germany
##
## Copyright (c) 2009-2012,
## Technische Universitaet Muenchen, Germany
##
## This software may be modified and distributed under the terms of
## a BSD-style license.  See the COPYING file in the package base
## directory for details.
##

## file       test/instrumenter_checks/serial/src/jacobi/cxx/Makefile


#------------------------------------------------------------------------------
# Testcase specific variables and rules
#------------------------------------------------------------------------------

OBJECTS = \
	$(OBJDIR)/jacobi.o \
	$(OBJDIR)/main.o

#LIBS_INTERNAL =

all:


#------------------------------------------------------------------------------
# Dependencies
#------------------------------------------------------------------------------

$(OBJDIR)/jacobi.o: $(SRCDIR)/jacobi.cpp $(SRCDIR)/jacobi.h
$(OBJDIR)/main.o: $(SRCDIR)/main.cpp $(SRCDIR)/jacobi.h


#------------------------------------------------------------------------------
# Generic rules and variables, don't move above OBJECTS and LIBS
#------------------------------------------------------------------------------

include $(INSTRUMENTERCHECK_SRCDIR)/Makefile.common_rules

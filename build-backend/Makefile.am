##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2020                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  Copyright (c) 2009-2015                                                ##
##  German Research School for Simulation Sciences GmbH,                   ##
##  Laboratory for Parallel Programming                                    ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


# scorep--backend
## Options passed to aclocal, autoreconf, autopoint and gettextize.
## Specifies local macro directory.
## ACLOCAL_AMFLAGS contains options to pass to aclocal when aclocal.m4 is to be
## rebuilt by make. This line is also used by autoreconf to run aclocal with
## suitable options, or by autopoint and gettextize to locate the place where
## Gettext's macros should be installed. So even if you do not really care about
## the rebuild rules, you should define ACLOCAL_AMFLAGS.
## Can't be moved to common.am
ACLOCAL_AMFLAGS =  -I ../build-config/m4  -I ../build-config/common/m4


include ../src/Makefile.cubew.inc.am



bindir = @bindir@@backend_suffix@
libdir = @libdir@@backend_suffix@

## Convenience variable, used in common.am
BUILD_DIR=build-backend




#EXTRA_LOCAL_CLEAN_TARGETS = clean-local-cube-config-tool

clean-local:

.PHONY:

distclean-local:




 

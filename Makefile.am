##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2020                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
##                                                                         ##
##  This software may be modified and distributed under the terms of       ##
##  a BSD-style license.  See the COPYING file in the package base         ##
##  directory for details.                                                 ##
##*************************************************************************##


## The SUBDIRS variable holds a list of subdirectories automake (recursivly)
## must build.
SUBDIRS = . $(subdirs) $(ax_config_subdirs)

## ACLOCAL_AMFLAGS contains options to pass to aclocal when aclocal.m4 is to be
## rebuilt by make. This line is also used by autoreconf to run aclocal with
## suitable options, or by autopoint and gettextize to locate the place where
## Gettext's macros should be installed. So even if you do not really care about
## the rebuild rules, you should define ACLOCAL_AMFLAGS.
ACLOCAL_AMFLAGS = -I build-config/common/m4

PHONY_TARGETS =




include build-config/common/Makefile.inc.am
include build-config/platforms/platforms.am
include build-config/common/platforms.am


EXTRA_DIST+= \
	src/cube-config.in 



uninstall-local:
	rm -rf $(DESTDIR)$(bin)

if GIT_CONTROLLED	
install-data-local:
	@echo "Install compatibility script"
	@$(MKDIR_P) $(DESTDIR)$(bindir)
	@$(INSTALL) src/cube-config  $(DESTDIR)$(bindir)/cube-config
	@echo "=============================================================="
	@echo "No documentation is being installed. Documenation is available only "
	@echo "when tarball is used for the installation. Visit "
	@echo "            https://www.scalasca.org  "
	@echo "for the recent released tarball. "
	@echo "=============================================================="
else !GIT_CONTROLLED

	
install-data-local:
	@echo "Install compatibility script"
	@$(MKDIR_P) $(DESTDIR)$(bindir)
	@$(INSTALL) src/cube-config  $(DESTDIR)$(bindir)/cube-config
	@echo "=============================================================="
	@echo "Copy pdf files into components"
	@cp $(top_srcdir)/doc/CubeTool*.pdf $(top_srcdir)/vendor/cubew/doc/.
	@cp $(top_srcdir)/doc/CubeDerived*.pdf $(top_srcdir)/vendor/cubelib/doc/.
	@cp $(top_srcdir)/doc/CubeUser*.pdf $(top_srcdir)/vendor/cubegui/doc/.
	@cp $(top_srcdir)/doc/CubePlugin*.pdf $(top_srcdir)/vendor/cubegui/doc/.
	@cp $(top_srcdir)/doc/tools*.tar.gz $(top_srcdir)/vendor/cubew/doc/.
	@cp $(top_srcdir)/doc/derived*.tar.gz $(top_srcdir)/vendor/cubelib/doc/.
	@cp $(top_srcdir)/doc/guide*.tar.gz $(top_srcdir)/vendor/cubegui/doc/.
	@cp $(top_srcdir)/doc/plugins*.tar.gz $(top_srcdir)/vendor/cubegui/doc/.


endif !GIT_CONTROLLED

	
	
## To clean up configuration summary files
DISTCLEANFILES += \
    $(builddir)/src/cube-config
    




.PHONY: $(PHONY_TARGETS)


##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2019                                                ##
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





# scorep--frontend

## Options passed to aclocal, autoreconf, autopoint and gettextize.
## Specifies local macro directory.
## ACLOCAL_AMFLAGS contains options to pass  aclocal when aclocal.m4 is to be
## rebuilt by make. This line is also used by autoreconf to run aclocal with
## suitable options, or by autopoint and gettextize to locate the place where
## Gettext's macros should be installed. So even if you do not really care about
## the rebuild rules, you should define ACLOCAL_AMFLAGS.
## Can't be moved to common.am

ACLOCAL_AMFLAGS = -I ../build-config/m4 -I ../build-config/common/m4

include ../build-includes/common.am

AM_CPPFLAGS += -DFRONTEND_BUILD -DCROSS_BUILD $(FRONTEND_LIBZ_HEADERS)
AM_LDFLAGS += $(FRONTEND_LIBZ_LD)


include ../build-includes/frontend-only.am



TESTS+= $(CUBE_FRONTEND_SELF_TESTS)

#$(CUBE_CPPLIB_TESTS) $(CUBE_TOOLS_TEST) $(CUBE_GUI_TESTS)


## Convenience variable, used in common.am
BUILD_DIR=build-frontend



DISTCLEANFILES += $(BUILT_SOURCES)\
		$(builddir)/config.summary \
                $(builddir)/@PACKAGE@.summary

.PHONY: $(PHONY_TARGETS)


## Here we write only backend values.
create-config-tool-headers:
	(cd ../build-backend && $(MAKE) create-config-tool-headers)


install-data-local: create-frontend-examples-dir install-frontend-examples

create-frontend-examples-dir:
	@$(MKDIR_P) $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/gui/plugin-simple/
	@$(MKDIR_P) $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/gui/plugin-demo/
	@$(MKDIR_P) $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/gui/plugin-colormap/
	@$(MKDIR_P) $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/gui/plugin-context-free/
	@$(MKDIR_P) $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/gui/plugin-value-view/
	@$(MKDIR_P) $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/gui/plugin-parallel/


install-frontend-examples: $(INST_FRONTEND_EXAMPLES) $(INST_FRONTEND_PLUGIN_EXAMPLES)
	@echo "== Install frontend examples"; \
	for example in $(INST_FRONTEND_EXAMPLES) dummy; do \
	if test $$example != dummy; then \
	echo  "   Install example $$example"; \
	$(INSTALL_DATA) $$example $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/; \
	fi; \
	done
	@echo "== Install GUI plugin examples"; \
	for example in $(EXAMPLE_PLUGIN_SIMPLE) dummy; do \
	if test $$example != dummy; then \
	echo  "   Install example $$example"; \
	$(INSTALL_DATA) $$example $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/gui/plugin-simple/; \
	fi; \
	done
	@for example in $(EXAMPLE_PLUGIN_DEMO) dummy; do \
	if test $$example != dummy; then \
	echo  "   Install example $$example"; \
	$(INSTALL_DATA) $$example $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/gui/plugin-demo; \
	fi; \
	done
	@for example in $(EXAMPLE_PLUGIN_COLOR) dummy; do \
	if test $$example != dummy; then \
	echo  "   Install example $$example"; \
	$(INSTALL_DATA) $$example $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/gui/plugin-colormap; \
	fi; \
	done
	@for example in $(EXAMPLE_PLUGIN_CONTEXTFREE) dummy; do \
	if test $$example != dummy; then \
	echo  "   Install example $$example"; \
	$(INSTALL_DATA) $$example $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/gui/plugin-context-free; \
	fi; \
	done
	@for example in $(EXAMPLE_PLUGIN_VALUEVIEW) dummy; do \
	if test $$example != dummy; then \
	echo  "   Install example $$example"; \
	$(INSTALL_DATA) $$example $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/gui/plugin-value-view/; \
	fi; \
	done
	@for example in $(EXAMPLE_PLUGIN_PARALLEL) dummy; do \
	if test $$example != dummy; then \
	echo  "   Install example $$example"; \
	$(INSTALL_DATA) $$example $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/gui/plugin-parallel/; \
	fi; \
	done
	@echo "   Install a Makefile for building frontend examples using installed cube"
	@$(INSTALL_DATA) $(abs_builddir)/../examples/Makefile.frontend $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/
	@echo "   Install examples of real cube files"
	@$(INSTALL_DATA) $(srcdir)/../examples/GUI/*.cubex $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/


installcheck-local:  installcheck-frontend

installcheck-frontend:.
	cd $(DESTDIR)$(datadir)/doc/@PACKAGE@/example/; make -f Makefile.frontend; make -f Makefile.frontend run



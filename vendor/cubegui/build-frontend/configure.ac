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



AC_PREREQ([2.69])
AC_INIT([CubeGUI], m4_esyscmd([../build-config/common/generate-package-version.sh ../build-config/VERSION]), [scalasca@fz-juelich.de],[cubegui],[http://www.scalasca.org])
AFS_PACKAGE_BUILD_INIT([frontend])
AC_CONFIG_HEADERS([../src/config-frontend.h])
AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../build-config/common/generate-library-version.sh ../build-config/VERSION]))
AC_DEFINE_UNQUOTED([LIBRARY_INTERFACE_VERSION], ["$LIBRARY_INTERFACE_VERSION"], [Interface version number])
adl_RECURSIVE_EVAL([${libexecdir}/${PACKAGE}], [PKGLIBEXECDIR])
AC_DEFINE_UNQUOTED([PKGLIBEXECDIR],
                   ["${PKGLIBEXECDIR}"],
                   [Directory where ]AC_PACKAGE_NAME[ executables to be run by other programs rather than by users are installed])
AC_DEFINE_UNQUOTED([PKGLIBEXECDIR],
                   ["${PKGLIBEXECDIR}"],
                   [Directory where ]AC_PACKAGE_NAME[ executables to be run by other programs rather than by users are installed])
adl_RECURSIVE_EVAL([${bindir}], [BINDIR])
AC_SUBST([BINDIR])
adl_RECURSIVE_EVAL([${includedir}], [INCLUDEDIR])
AC_SUBST([INCLUDEDIR])
adl_RECURSIVE_EVAL([${libdir}/${PACKAGE}], [PKGLIBDIR])
AC_SUBST([PKGLIBDIR])
adl_RECURSIVE_EVAL([${libdir}${backend_suffix}/${PACKAGE}], [BACKEND_PKGLIBDIR])
AC_SUBST([BACKEND_PKGLIBDIR])
adl_RECURSIVE_EVAL([${datadir}/${PACKAGE}], [PKGDATADIR])
AC_SUBST([PKGDATADIR])
AC_DEFINE_UNQUOTED([PKGDATADIR],
                  ["${PKGDATADIR}"],
                   [Directory where ]AC_PACKAGE_NAME[ data is stored])
adl_RECURSIVE_EVAL([${docdir}], [DOCDIR])
AC_SUBST([DOCDIR])

## Looking for major requirement -> cube lib
AC_CUBE_FIND_CUBELIB

## Where to find auxiliary build tools & local macros
AC_CONFIG_AUX_DIR([../build-config])
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../build-config/common/m4])




## Define frontend-specific environment variables
AC_ARG_VAR(CXX_FOR_BUILD, [C++ compiler command for the frontend build])
AC_ARG_VAR(CPPFLAGS_FOR_BUILD, [(Objective) C/C++ preprocessor flags for the frontend build, e.g. -I<include dir> if you have headers in a nonstandard directory <include dir>])
AC_ARG_VAR(CXXFLAGS_FOR_BUILD, [C++ compiler flags for the frontend build])
AC_ARG_VAR(LDFLAGS_FOR_BUILD, [linker flags for the frontend build, e.g. -L<lib dir> if you have libraries in a nonstandard directory <lib dir>])
AC_ARG_VAR(LIBS_FOR_BUILD, [libraries to pass to the linker for the frontend build, e.g. -l<library>])

AS_IF([ test "x${ac_scorep_cross_compiling}" = "xyes" ],[
# Convert compilers & flags
AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([CC])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([CFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_COMPILER([CXX])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([CPPFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([CXXFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([LDFLAGS])
AC_SCOREP_CONVERT_FOR_BUILD_FLAGS([LIBS])
])

## Set up Automake
AM_INIT_AUTOMAKE([1.13.4 -Wall foreign color-tests tar-pax])
AM_SILENT_RULES([yes])
AM_PROG_AR

## Initialize configuration summary
AFS_SUMMARY_INIT


## Set up Libtool
LT_PREREQ([2.4.6])
LT_INIT([])
AC_SUBST([LIBTOOL_DEPS])



##--- Package-specific configuration ----------------------------------------

AC_SUBST([FRONTEND_CXX], [${CXX}])

## Checks for programs
AC_C_BIGENDIAN
AC_PROG_CXX
AM_PROG_CC_C_O
AC_CHECK_DECLS([basename], [], [], [[#include <libgen.h>]])

dnl Use the most recent C++ standard that is supported by the code.
dnl We can fall back to older versions.
AX_CXX_COMPILE_STDCXX_14([], optional)
if test "$HAVE_CXX14" -ne 1; then
    AX_CXX_COMPILE_STDCXX_11([], mandatory)
fi
	
	

AC_SCOREP_CUBE_FRONTEND_SELECTION
AC_SCOREP_CONFIG_SELECTION_REPORT([frontend])
AX_CXX_INTTYPE_MACROS

#check for examples
AC_CUBE_EXAMPLES



dnl Predicate to use HARDCODE_LIBDIR_FLAG_* only if linker needs it.
AS_IF([test "x${hardcode_into_libs}" = "xyes"],
      [ac_scorep_hardcode_into_libs=1],
      [ac_scorep_hardcode_into_libs=0])
AC_SUBST([HARDCODE_INTO_LIBS], [${ac_scorep_hardcode_into_libs}])
dnl Flag to hardcode libdir into a binary during linking, C version.
AC_SUBST([HARDCODE_LIBDIR_FLAG_CC], [${hardcode_libdir_flag_spec}])
dnl Flag to hardcode libdir into a binary during linking, C++ version.
AC_SUBST([HARDCODE_LIBDIR_FLAG_CXX], [${hardcode_libdir_flag_spec_CXX}])
dnl The ${wl} part of the HARDCODE_LIBDIR_FLAG_* defines.
AC_SUBST([HARDCODE_LIBDIR_WL], [${wl}])
dnl The ${aix_libpath} part of the HARDCODE_LIBDIR_FLAG_* defines. Only available on AIX.
AC_SUBST([HARDCODE_LIBDIR_AIX_LIBPATH], [${aix_libpath}])


AC_SUBST([SYS_LIB_DLSEARCH_PATH_SPEC], [${sys_lib_dlsearch_path_spec}])











CC="$CUBE_SCOREP $CC"
CXX="$CUBE_SCOREP $CXX"






AC_CONFIG_FILES([../src/tools/cubegui_config_data_]AFS_PACKAGE_BUILD_name[.cpp:../src/tools/cubegui_config_data_build.cpp.in])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([../examples/Makefile.frontend])
AC_CONFIG_FILES([../examples/GUI/plugins/simple/example-simple.pro
                 ../examples/GUI/plugins/demo/demo-plugin.pro
                 ../examples/GUI/plugins/colormap/colormap.pro
                 ../examples/GUI/plugins/context-free/context-free.pro
                 ../examples/GUI/plugins/value-view/tau-value.pro
                 ../examples/GUI/plugins/parallel/parallel-example.pro
                 ])
AC_CONFIG_FILES([../src/cubegui-concurrent.h:../src/cubegui-concurrent.h.in])


AC_OUTPUT

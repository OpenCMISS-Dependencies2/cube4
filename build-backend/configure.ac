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
AC_INIT([CubeWriter], m4_esyscmd([../build-config/common/generate-package-version.sh ../build-config/VERSION]), [scalasca@fz-juelich.de], [cubewriter],[http://www.scalasca.org])

## Save user provided arguments for use by sub-configures
AC_SCOREP_TOPLEVEL_ARGS

## Initialize configuration summary
AFS_PACKAGE_BUILD_INIT([backend])
#AFS_SUMMARY_INIT
## Where to find auxiliary build tools & local macros
AC_CONFIG_AUX_DIR([../build-config])
AC_CONFIG_MACRO_DIRS([../build-config/m4 ../build-config/common/m4])


##--- Package-specific configuration ----------------------------------------
## Initialize configuration summary
AFS_PACKAGE_INIT


##--- Package-specific configuration ----------------------------------------
AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])
AM_PROG_AR

## Set up Libtool
LT_PREREQ([2.4.6])
LT_INIT([disable-shared])
AC_SUBST([LIBTOOL_DEPS])

AC_SUBST([LIBRARY_INTERFACE_VERSION], m4_esyscmd([../build-config/common/generate-library-version.sh ../build-config/VERSION]))




#AC_SCOREP_CONSTUCT_LIST_OF_LIB_DIRS
#AC_SCOREP_CUBE_BACKEND_SELECTION

## Platform configuration




AC_SCOREP_CHECK_COMPILER_VAR_SET([CC])
## Checks for programs
AC_PROG_CC
AC_SCOREP_PROG_CC_C99
AC_C_BIGENDIAN
AM_PROG_CC_C_O





## trunc and ceil needed by the cube4 writer
AC_LANG_PUSH([C])
AC_SEARCH_LIBS([trunc], [m])
AC_SEARCH_LIBS([ceil], [m])
AC_LANG_POP([C])

#AC_LANG([C++])
AC_SCOREP_COMPILER_CHECKS

## Workaround for linking issues on Cray
AS_IF([test "x${ac_scorep_platform}" = "xcrayxt" && \
       test "x${enable_shared}" = "xno"],
      [AC_CONFIG_COMMANDS([libtool-cray-fix],
          [sed -e '1,/prefer_static_libs=/ s/prefer_static_libs=.*$/prefer_static_libs=yes/' \
               libtool > libtool.fix && mv libtool.fix libtool])])
               

# On crosscompile machines, install backend/mic stuff into
# $(bin|lib)dir/(backend|mic).
backend_suffix=""
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
        [AS_IF([test "x${ac_scorep_platform}" = "xmic"],
                 [backend_suffix="/mic"],
             [backend_suffix="/backend"])
        ])
AC_SUBST([backend_suffix])
               



## Generate config files
AC_CONFIG_HEADERS([../src/config-include.h])



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



AC_CONFIG_FILES([Makefile])



AC_OUTPUT

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


AC_PREREQ([2.69])
AC_INIT([CubeBundle], m4_esyscmd([build-config/common/generate-package-version.sh build-config/VERSION]), [scalasca@fz-juelich.de],[CubeBundle],[http://www.scalasca.org])

## Save user provided arguments for use by sub-configures
AC_SCOREP_TOPLEVEL_ARGS

## Initialize configuration summary
AFS_PACKAGE_INIT
AFS_SUMMARY_INIT

## Where to find auxiliary build tools & local macros
AC_CONFIG_AUX_DIR([build-config])
AC_CONFIG_MACRO_DIRS([build-config/m4 build-config/common/m4])

## Set the default installation prefix
AC_PREFIX_DEFAULT([/opt/cube])

## Set up Automake
AM_INIT_AUTOMAKE([foreign color-tests 1.13.4 -Wall tar-pax])
AM_SILENT_RULES([yes])



## Working from SVN? Get revision info
AC_SCOREP_GIT_CONTROLLED
AC_SCOREP_UNCRUSTIFY

## Platform detection
AC_SCOREP_DETECT_PLATFORMS
## Set up Automake
AC_SCOREP_PLATFORM_SETTINGS
AFS_CROSSCOMPILING	

## Check for cross-compilation
AM_CONDITIONAL([CROSS_BUILD], [test "x${ac_scorep_cross_compiling}" = "xyes"])
#On crosscompile machines, install backend/ /mic stuff into 
# $(bin|lib)dir/(backend|mic). 
backend_suffix=""
target=""
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
      [backend_suffix="/backend"
       AS_IF([test "x${ac_scorep_platform}" = "xmic"],
             [backend_suffix=""
              target="mic"])
      ])


## Don't detect and load defaults in nested configures
AS_IF([test ! -n "$ac_scorep_platform_data_provided" || \
       test "x${ac_scorep_platform_data_provided}" = "xno"],
      [AFS_CUSTOM_COMPILERS
       AS_IF([test "x${afs_custom_compilers_given}" = "xno"],
             [dnl AFS_COMPILER_BACKEND([CC, CXX, F77, FC])
              AC_SCOREP_WITH_COMPILER_SUITE
              AS_CASE([${afs_custom_compilers_given},${ac_scorep_platform}],
                  [no,mac64|no,mingw64],
                      [AC_MSG_ERROR([have no suitable defaults to build Scalasca on $afs_scorep_platform_name yet. Please use --with-custom-compilers and provide settings to <AC_PACKAGE_BUGREPORT> on success. Thanks.])],
                  [no,unknown],
                      [AC_MSG_ERROR([cannot detect platform to set suitable defaults. Please use --with-custom-compilers and provide settings and platform to <AC_PACKAGE_BUGREPORT> on success. Thanks.])
                  ])
             ])
       # Determine arguments for subdir configures
       args2="`$AWK -f $srcdir/build-config/common/process_arguments.awk \
              $ac_scorep_compilers_mpi                                         \
              $ac_scorep_compilers_frontend                                    \
              $ac_scorep_compilers_backend                                     \
              user_provided_configure_args`"
      ],
      [AC_MSG_NOTICE([platform data provided by toplevel configure.])
       AFS_SUMMARY([Platform], [$ac_scorep_platform (provided)])
       # Determine arguments for subdir configures
       args2="`$AWK -f $srcdir/build-config/common/process_arguments.awk \
              user_provided_configure_args`"
      ])

args="`$AWK -f $srcdir/build-config/common/process_arguments.awk \
              user_provided_configure_args`"

## used for EXTRA_DIST in Makefile.am
AM_CONDITIONAL([PLATFORM_MIC], [test "x${ac_scorep_platform}" = "xmic"])




AC_SUBST([backend_suffix])
AC_SUBST([target])

AX_CONFIG_SUBDIR([build-backend],
                        [${args2}  ac_scorep_platform=${ac_scorep_platform} ac_scorep_cross_compiling=${ac_scorep_cross_compiling} ])

AX_CONFIG_SUBDIR([vendor/cubew],
            	        [${args} afs_externally_triggered=yes])
AX_CONFIG_SUBDIR([vendor/cubelib],
                        [${args} afs_externally_triggered=yes])
AX_CONFIG_SUBDIR([vendor/cubegui],
                        [${args} --with-cubelib=intern afs_externally_triggered=yes])


##--- Generate output -------------------------------------------------------


AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/cube-config:src/cube-config.in])
AC_CONFIG_HEADERS([src/config-include.h])
AC_CONFIG_HEADERS([src/config.h])
AC_OUTPUT


AFS_SUMMARY_COLLECT

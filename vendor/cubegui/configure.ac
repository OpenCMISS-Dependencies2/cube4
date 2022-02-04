##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2021                                                ##
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
AC_INIT([CubeGUI], m4_esyscmd([build-config/common/generate-package-version.sh build-config/VERSION]), [scalasca@fz-juelich.de],[cubegui],[http://www.scalasca.org])
AFS_PACKAGE_INIT

## Save user provided arguments for use by sub-configures
AC_SCOREP_TOPLEVEL_ARGS

## Where to find auxiliary build tools & local macros
AC_CONFIG_AUX_DIR([build-config])
AC_CONFIG_MACRO_DIRS([build-config/m4 build-config/common/m4])

## Set the default installation prefix
AC_PREFIX_DEFAULT([/opt/cubegui])

## Set up Automake
AM_INIT_AUTOMAKE([foreign color-tests 1.11.1 -Wall tar-pax])
AM_SILENT_RULES([yes])
AFS_MAINTAINER_MODE 

## Initialize configuration summary
AFS_SUMMARY_INIT


##--- Package-specific configuration ----------------------------------------

## Working from git? Get revision info
AC_SCOREP_GIT_CONTROLLED
AC_SCOREP_UNCRUSTIFY

AC_SCOREP_IS_ANOTHER_CUBE_INSTALLED
##
## Detect whether there is an another cube installation and warn user about it but do not stop configuration
##
AM_CONDITIONAL([ANOTHER_CUBE], [test "x$ANOTHER_CUBE_PRESENT" = "xyes"])
AM_CONDITIONAL([OVERWRITE_CUBE], [test "x$ANOTHER_CUBE_OVERWRITE" = "xyes"])



if test "x$prefix" = "xNONE"; then
AFS_SUMMARY( [Cube installed into],[$ac_default_prefix])
else
AFS_SUMMARY( [Cube installed into],[$prefix])
fi



AS_IF([ test "x$ANOTHER_CUBE_PRESENT" = "xyes"],[
AFS_SUMMARY([ Another installation of CUBE was detected under ], [$anothercube])
])

AS_IF([ test "x$ANOTHER_CUBE_OVERWRITE" = "xyes"],[
AFS_SUMMARY([ This configuration will overwrite a present installation of cube under], [$overwritecube])
])




######################
AC_SCOREP_REVISION
AC_SCOREP_CUBEGUI_VARS

## Platform detection
AC_SCOREP_DETECT_PLATFORMS
AC_SCOREP_PLATFORM_SETTINGS 	
AFS_CROSSCOMPILING

## Don't detect and load defaults in nested configures
AS_IF([test ! -n "$ac_scorep_platform_data_provided" || \
       test "x${ac_scorep_platform_data_provided}" = "xno"],
      [AFS_CUSTOM_COMPILERS
       AS_IF([test "x${afs_custom_compilers_given}" = "xno"],
             [dnl AFS_COMPILER_BACKEND([CC, CXX, F77, FC])
                 AS_IF([test "x${ac_scorep_compilers_frontend}" = "x" ],
                 [	
	         #AC_CUBE_QT_COMPILER
                 # early invocation of Qmake detection in order to detect standard compiler
	            AT_WITH_QT([+network +widgets +printsupport], [+plugin], [WITH_GUI="yes"], [WITH_GUI="no"])
                 # detect Qt compiler
                    AS_IF([test "x${qt_compiler}" = "x"],[
			 ac_scorep_compilers_frontend="platform-frontend-${ac_scorep_platform}"
			],[
                    ac_scorep_compilers_frontend="compiler-frontend-${qt_compiler}"
	            ac_scorep_compilers_backend="compiler-nocross-${qt_compiler}" 
                    ac_compiler_suite_detected="yes"
                        ])
                ],
		[
		ac_scorep_compilers_frontend="platform-frontend-${ac_scorep_platform}"
		])
                AC_SCOREP_WITH_COMPILER_SUITE
                AS_CASE([${afs_custom_compilers_given},${ac_scorep_platform}],
                    [no,mac64|no,mingw64],
                        [AC_MSG_ERROR([have no suitable defaults to build Scalasca on $afs_scorep_platform_name yet. Please use --with-custom-compilers and provide settings to <AC_PACKAGE_BUGREPORT> on success. Thanks.])],
                    [no,unknown],
                        [AC_MSG_ERROR([cannot detect platform to set suitable defaults. Please use --with-custom-compilers and provide settings and platform to <AC_PACKAGE_BUGREPORT> on success. Thanks.])
                    ])
             ])
      # Determine arguments for subdir configures
       args="`$AWK -f $srcdir/build-config/common/process_arguments.awk \
              $ac_scorep_compilers_mpi                                         \
              $ac_scorep_compilers_frontend                                    \
              $ac_scorep_compilers_backend                                     \
              user_provided_configure_args`"
      ],
      [AC_MSG_NOTICE([platform data provided by toplevel configure.])
       AFS_SUMMARY([Platform], [$ac_scorep_platform (provided)])
       # Determine arguments for subdir configures
       args="`$AWK -f $srcdir/build-config/common/process_arguments.awk \
              user_provided_configure_args`"
      ])

## Check for programs
AC_SCOREP_DOXYGEN



## Handle own package's subdir configures
AS_IF([ test "x${with_cubelib}" != "xintern" ],
[
AC_SCOREP_HAVE_CONFIG_TOOL([cubelib], [10], [0])     
])
AX_CONFIG_SUBDIR([build-frontend],
                                    [${args} ac_scorep_platform=${ac_scorep_platform} ${scorep_cubelib_config_arg} ac_scorep_cross_compiling=${ac_scorep_cross_compiling}])

adl_RECURSIVE_EVAL([${libdir}], [LIBDIR])
cube_have_mic_support="no"
AS_IF([test -d "${LIBDIR}/mic"],
    [cube_have_mic_support="yes"])
backend_targets="backend"
AS_IF([test "x${ac_scorep_cross_compiling}" = "xyes"],
      [frontend_targets="frontend"
       AS_IF([test "x${ac_scorep_platform}" = "xmic"],
             [AS_UNSET([frontend_targets])
              mic_targets="mic"
             ])
      ],
      [AS_IF([test "x${cube_have_mic_support}" = "xyes"],
             [mic_targets="mic"])
      ])


AC_SUBST([backend_targets])
AC_SUBST([frontend_targets])
AC_SUBST([mic_targets])



##--- Generate output -------------------------------------------------------


AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([bin/cubegui-autocompletion.sh:bin/cubegui-autocompletion.sh.in])
AC_CONFIG_FILES([bin/maccubegui.sh:bin/maccubegui.sh.in])
AC_CONFIG_FILES([src/cubegui-version.h:src/cubegui-version.h.in])
AC_CONFIG_FILES([src/config.h:src/config.h.in])
AC_CONFIG_FILES([src/cubegui-config-common.h:build-config/common/config-common.h])
AC_CONFIG_FILES([vendor/macos/Info.plist:vendor/macos/Info.plist.in])

AM_COND_IF([HAVE_DOXYGEN],
           [AC_CONFIG_FILES([doc/doxygen-common/config.tex
                             doc/doxygen-common/doxygen-common.cfg
                             doc/doxygen-common/doxygen-cube-plugins-dev.cfg
                             doc/doxygen-common/doxygen-cubegui.cfg
                             doc/doxygen-dev/cube-plugins/doxygen-cube-plugins-dev.cfg
                             doc/doxygen-dev/cubegui/doxygen-cubegui-dev.cfg
                             doc/doxygen-user/guide/doxygen-guide.cfg
                             doc/doxygen-user/plugins-guide/doxygen-plugins-guide.cfg
                             ])])
CUBE_CORPORATE_STY

AC_OUTPUT


AFS_SUMMARY([cubegui-config], [$prefix/bin/cubegui-config ])
AFS_SUMMARY([For detailed information], ["$prefix/bin/cubegui-config --help"])

AFS_SUMMARY([Examples], [$prefix/share/doc/cubegui/example ])



AFS_SUMMARY_COLLECT([test "x${afs_externally_triggered}" != "xyes"])

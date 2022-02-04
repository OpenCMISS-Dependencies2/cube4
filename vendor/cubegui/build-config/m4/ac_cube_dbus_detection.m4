##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2016                                                ##
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




AC_DEFUN([AC_SCOREP_RESET_LIB_CACHE], [
    AS_FOR([AX_var], [ax_var], [$1], [
        dnl You can replace "ac_cv_lib_" with any prefix from http://www.gnu.org/software/autoconf/manual/html_node/Cache-Variable-Index.html
        AS_VAR_PUSHDEF([ax_Var], [ac_cv_lib_${ax_var}_$2])
        AS_UNSET([ax_Var])
        AS_VAR_POPDEF([ax_Var])
    ])
]) # AX_RESET_LIB_CACHE


# SYNOPSIS
#
# AC_SCOREP_RESET_HEADERS_CACHE(headers ...)
#
# DESCRIPTION
#
# This macro invalidates the headers cache variables created by previous AC_CHECK_HEADER/AC_CHECK_HEADERS checks.
#
AC_DEFUN([AC_SCOREP_RESET_HEADERS_CACHE], [
    AS_FOR([AX_var], [ax_var], [$1], [
        dnl You can replace "ac_cv_header_" with any prefix from http://www.gnu.org/software/autoconf/manual/html_node/Cache-Variable-Index.html
        AS_VAR_PUSHDEF([ax_Var], [ac_cv_header_${ax_var}])
        AS_UNSET([ax_Var])
        AS_VAR_POPDEF([ax_Var])
    ])
]) # AX_RESET_HEADERS_CACHE









AC_DEFUN([AC_DBUS_AVAILABLE],
[

#    if test "x$WITH_PARAVER" == "xyes"; then 
#	if test "x$PV_DEF_CFG" != "x"; then 
#		AC_MSG_NOTICE([Use $PV_DEF_CFG as a condifguration file for PARAVER. ])
#		CXXFLAGS="$CXXFLAGS -DPV_DEF_CFG='$PV_DEF_CFG' "
#	fi
#    fi

  AC_SCOREP_DBUS_DETECTION

  if test "x$DBUS_HEADER" != "xyes"; then
    AC_MSG_NOTICE([Cannot find DBUS headers. Please specify using option CXXFLAGS='-I[PATH to DBUS files dbus/dbus.h ] '.])
    DBUS_AVAILABLE=no
  fi

  if test "x$DBUS_LIB" != "xyes"; then
    AC_MSG_NOTICE([Cannot find DBUS library. Please specify using option LDFLAGS='-L[PATH to libdbus-1.a, libdbus-1.a or shared libraries libdbus.so or libdbus-1.so]'.])
    DBUS_AVAILABLE=no
  fi

  if test "x$DBUS_HEADER" = "xyes" && test "x$DBUS_LIB" = "xyes"; then  
    DBUS_AVAILABLE=yes
  fi

 AC_SUBST([DBUS_AVAILABLE])
])


AC_DEFUN([AC_SCOREP_DBUS_HEADERS_DETECTION],
[
  DBUS_CXXFLAGS=
  DBUS_CFLAGS=
  
    AC_CHECK_HEADERS([dbus/dbus.h], [DBUS_HEADER=yes],[DBUS_HEADER='try pkg-config...'])

    PKG_CONFIG_AVAILABLE=`which pkg-config`    
    if test "x${DBUS_HEADER}" != "xyes"; then 
        #here we try tu use pkg-config
        if test "x$PKG_CONFIG_AVAILABLE" != "x"; then
            DBUSCHECK=`pkg-config --cflags-only-I dbus-1 2>&1| grep "No package "`
            if test "x$DBUSCHECK" = "x";
            then
            DBUSVER=`pkg-config --modversion dbus-1`
            case "${DBUSVER}" in
                0.*)
                AC_MSG_NOTICE([DBUS API v${DBUSVER} is to old. Ignore dbus.])
                ;; # too old
                1.*) DBUSINC=`pkg-config --cflags-only-I dbus-1 `
                DBUS_CXXFLAGS="$DBUS_CXXFLAGS ${DBUSINC}"
                DBUS_CFLAGS="$DBUS_CFLAGS ${DBUSINC}"
                DBUS_HEADER=yes
                AC_MSG_NOTICE([Found dbus-1. Headers are included via ${DBUSINC}])
                ;;
                *)
                AC_MSG_NOTICE([ ignore dbus-1 version ${DBUSVER}])
                ;;
            esac
            fi
        fi
    fi
    AC_SUBST([DBUS_CXXFLAGS])
    AC_SUBST([DBUS_CFLAGS])
])

AC_DEFUN([AC_SCOREP_DBUS_LIB_DETECTION],
[
    DBUS_LDFLAGS=
    ax_probe_library_save_LDFLAGS=${LDFLAGS}
    ax_probe_library_save_LIBS=${LIBS}
    AC_SEARCH_LIBS(dbus_threads_init, dbus-1,  [DBUS_LIB=yes],[DBUS_LIB='try pkg-config...'],[])
    LDFLAGS=${ax_probe_library_save_LDFLAGS}
    LIBS=${ax_probe_library_save_LIBS}
    AC_SCOREP_RESET_LIB_CACHE([dbus-1], [dbus_threads_init])
    PKG_CONFIG_AVAILABLE=`which pkg-config`      
    if test "x$DBUS_LIB" != "xyes"; then
        #here we try tu use pkg-config
        if test "x$PKG_CONFIG_AVAILABLE" != "x"; then
            DBUSCHECK=`pkg-config --libs dbus-1 2>&1| grep "No package "`
            if test "x$DBUSCHECK" = "x";
            then
            DBUSVER=`pkg-config --modversion dbus-1`
            case "${DBUSVER}" in
                0.*)
                AC_MSG_NOTICE([DBUS API v${DBUSVER} is to old. Ignore dbus.])
                ;; # too old
                1.*) DBUSLIB=`pkg-config --libs dbus-1 `
                DBUS_LDFLAGS="$DBUS_LDFLAGS ${DBUSLIB}"
                DBUS_LIB=yes
                AC_MSG_NOTICE([Found libdbus-1. Library linked via ${DBUSLIB}])
                ;;
                *) 
                AC_MSG_NOTICE([ ignore dbus-1 version ${DBUSVER}])
                ;;
            esac
            fi
        fi
    else
    # we found libdbus without pkg-config
        DBUS_LDFLAGS="$DBUS_LDFLAGS -ldbus-1"
        DBUS_LIB=yes
    fi
    AC_SUBST([DBUS_LDFLAGS])
])


AC_DEFUN([AC_SCOREP_DBUS_DETECTION],
[
 AC_SCOREP_DBUS_HEADERS_DETECTION
 AC_SCOREP_DBUS_LIB_DETECTION

])



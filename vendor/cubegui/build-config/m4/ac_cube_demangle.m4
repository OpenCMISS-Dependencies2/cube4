##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2018                                                ##
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


#
# Simple check for demangling headers

AC_DEFUN([AC_CUBE_GUI_DEMANGLE],
[
AC_MSG_NOTICE([Check if Cube GUI can demangle region names ])
gui_demangle=no
AC_LANG_PUSH(C++)
AC_CHECK_HEADER(cxxabi.h, [
cxxabi_h_found=yes
gui_demangle=yes
AC_DEFINE_UNQUOTED(CXXABI_H_AVAILABLE, "yes", [Defined if cxxabi.h is found])
],
[cxxabi_h_found=no 
])
AM_CONDITIONAL([WITH_CXXABI_H], [test x$cxxabi_h_found == "xyes"])

AC_CHECK_HEADER(demangle.h, [
demangle_h_found=yes
gui_demangle=yes
DEMANGLE_LIB=-liberty
AC_DEFINE_UNQUOTED(DEMANGLE_H_AVAILABLE, "yes", [Defined if demangle.h is found])
],
[demangle_h_found=no 
], [ #include <string.h>])
AM_CONDITIONAL([WITH_DEMANGLE_H], [test x$demangle_h_found == "xyes"])
AC_SUBST(DEMANGLE_LIB)
AC_LANG_POP
])



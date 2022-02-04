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


#
# Simple check for shipped plugins

AC_DEFUN([AC_CUBE_WEB_ENGINE],
[


AS_IF( [ test x$with_web_engine == "xno" || test x$without_web_engine == "xyes"], 
[userwebengine="no"], [userwebengine="yes"])

webenginefound="no"
AS_IF([test x$userwebengine == "xyes"],
[
AC_MSG_CHECKING([Check if Qt's Web Engine is installed...])
found=`echo ${QT_DEFINES} | grep -o QT_WEBENGINE_LIB`
AS_IF([test "x$found" == "x"], [], 
[webenginefound="yes"
AC_DEFINE_UNQUOTED([WITH_WEB_ENGINE],
                  ["${webenginefound}"],
                    Defined if QWebEngine is enabled)
])
AC_MSG_RESULT($webenginefound)

MOC_UTILS_FLAGS="-DWITH_WEB_ENGINE"               
])
AM_CONDITIONAL([WITH_WEB_ENGINE], [test x$webenginefound == "xyes" ])
AC_SUBST([WITH_WEB_ENGINE])#
AC_SUBST([MOC_UTILS_FLAGS])

])



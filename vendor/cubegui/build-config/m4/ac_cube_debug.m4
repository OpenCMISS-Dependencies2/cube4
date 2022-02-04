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
# Simple check for mpirun to make backend examples and tests running under mpi. 

AC_DEFUN([AC_CUBE_DEBUG],
[

AC_ARG_ENABLE([debug], [AS_HELP_STRING([--enable-debug], [ Enables qDebug and qWarning outputs ])],[CUBE_DEBUG="yes"],[CUBE_DEBUG=])
CUBE_DEBUG_FLAGS="-DQT_NO_DEBUG_OUTPUT -DQT_NO_WARNING_OUTPUT"
AS_IF([ test "x$CUBE_DEBUG" != "x"], [
CUBE_DEBUG_FLAGS=
AC_MSG_NOTICE([Enable qDebug() and qWarning()])
],[
AC_MSG_NOTICE([Disable qDebug() and qWarning() with $CUBE_DEBUG_FLAGS])
])
AC_SUBST([CUBE_DEBUG_FLAGS])

])





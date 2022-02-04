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


AC_DEFUN([AC_CUBE_FIND_CUBELIB],
[
AS_IF([ test "x${with_cubelib}" == "xintern" ],
[
CUBE_CUBELIB=intern
])
AS_IF([ test "x${ac_scorep_platform}" != "xmingw" ], [

AS_IF([ test "x$CUBE_CUBELIB" == "xintern"], [
CUBELIB_CONFIG="intern"
CUBELIB_INCLUDEPATH='\
-I../../../vendor/cubelib/src \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/ \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/derivated \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions/calltree \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions/metric \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions/metric/cache \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions/metric/data \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions/metric/data/rows \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions/metric/index \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions/metric/matrix \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions/metric/strategies \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions/metric/value \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions/metric/value/trafo \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions/metric/value/trafo/single_value \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/dimensions/system \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/network \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/service/cubelayout \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/service/cubelayout/layout \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/service/cubelayout/readers \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/syntax \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/syntax/cubepl \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/syntax/cubepl/evaluators \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/syntax/cubepl/evaluators/binary \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/syntax/cubepl/evaluators/nullary \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/syntax/cubepl/evaluators/nullary/statements \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/syntax/cubepl/evaluators/unary \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/syntax/cubepl/evaluators/unary/functions/lambda_calcul \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/syntax/cubepl/evaluators/unary/functions/single_argument \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/syntax/cubepl/evaluators/unary/functions/two_arguments \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/topologies \
-I$(srcdir)/../../../vendor/cubelib/src/cube/include/service \
-I$(srcdir)/../../../vendor/cubelib/src/tools/common_inc \
-I$(srcdir)/../../../vendor/cubelib/src/tools/tools/0040.Tau2Cube'
CUBELIB_LIBADD=../../../vendor/cubelib/build-frontend/libcube4.la
CUBELIB_LDFLAGS="-L../../../vendor/cubelib/build-frontend/"
]
,
[
AS_IF([ test "x$CUBE_CUBELIB" == "x" || test "x$CUBE_CUBELIB" == "xyes"], [
CUBE_CUBELIB=$scorep_cubelib_bindir

AS_IF([ test "x$CUBE_CUBELIB" == "x" ], [
AC_MSG_ERROR([cubelib-config no found! Please specify its location using --with-cubelib=<location> command line option.])
])
])
AS_IF([ test -d $CUBE_CUBELIB ],[
CUBE_CUBELIB=$CUBE_CUBELIB/cubelib-config
])
AS_IF([! test -x $CUBE_CUBELIB ],[ 
AC_MSG_ERROR([No $CUBE_CUBELIB found!])
])
CUBELIB_CONFIG=$CUBE_CUBELIB
CUBELIB_INCLUDEPATH=`$CUBELIB_CONFIG --cppflags`
CUBELIB_LDFLAGS=`$CUBELIB_CONFIG --ldflags`
CUBELIB_LIBADD=`$CUBELIB_CONFIG --libs`
AC_DEFINE_UNQUOTED(CUBELIB_CONFIG, "$CUBELIB_CONFIG", [Stores path to the cubelib-config used to build Cube GUI])
])
])
AC_MSG_NOTICE([Using cubelib-config : $CUBELIB_CONFIG ])
AC_MSG_NOTICE([Using CubeLIB Include path : $CUBELIB_INCLUDEPATH ])
AC_MSG_NOTICE([Using CubeLIB ldflags : $CUBELIB_LDFLAGS ])
AC_MSG_NOTICE([Using CubeLIB libadd : $CUBELIB_LIBADD ])

AC_SUBST([CUBE_CUBELIB])
AC_SUBST([CUBELIB_CONFIG])
AC_SUBST([CUBELIB_LDFLAGS])
AC_SUBST([CUBELIB_LIBADD])
AC_SUBST([CUBELIB_INCLUDEPATH])
])


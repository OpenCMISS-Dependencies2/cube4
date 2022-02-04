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


# check if QtConcurrent is available
# sets WITH_QTCONCURRENT to "yes", if ${QT_DEFINES} contains -DQT_CONCURRENT_LIB
# autotroll.m4 parses qmake generated Makefiles and fills QT_DEFINES

AC_DEFUN([AC_CUBE_QTCONCURRENT],
[
                 
AS_IF( [ test x$with_concurrent == "xno" || test x$without_concurrent == "xyes"], 
[userconcurrent="no" qtconcurrentfound="no" ], [userconcurrent="yes" qtconcurrentfound="yes" ])

AS_IF([test x$userconcurrent == "xyes"],
[
AC_MSG_CHECKING([Check if Qt Concurrent is installed and we can use it...])
qtconcurrentfound="no"    
found=`echo ${QT_DEFINES} | grep -o QT_CONCURRENT_LIB`
AS_IF([test "x$found" == "x"], 
[
#if we do not find a flag, it is still possible that concurent is a part of thhe Qt Core.
# Hence more sophisticated test is required
AC_LANG_PUSH(C++)
cxxflags_SAVE=$CXXFLAGS
CXXFLAGS+=" $QT_INCPATH "
ldflags_SAVE=$LDFLAGS
LDFLAGS+=" $QT_LDFLAGS $QT_LIBS"

   AC_RUN_IFELSE([AC_LANG_PROGRAM([[
   #include <QFuture>
   #include <QtCore>
   int mapp(int& p)
   {
    return p*p;
   }
   ]],[[
      QVector<int> seq;
      QFuture<void> future =  QtConcurrent::map(seq.begin(), seq.end(), mapp);
   ]])],
    [ qtconcurrentfound="yes" ],[],[AC_MSG_FAILURE([test for QtConcurent failed])])
CXXFLAGS=$cxxflags_SAVE
LDFLAGS=$ldflags_SAVE
AC_LANG_POP
],[qtconcurrentfound="yes"])

AC_MSG_RESULT($qtconcurrentfound)
])

AS_IF([test "x$qtconcurrentfound" == "xyes"], 
[
# 526 is a prime smith number ...
    WITH_CONCURRENT=526
], [ 
# 0 in case of missing QtConcurrent
    WITH_CONCURRENT=0
])
AC_SUBST(WITH_CONCURRENT)
AM_CONDITIONAL([WITH_QTCONCURRENT], [test x$qtconcurrentfound == "xyes"])
])


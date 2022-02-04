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


CONFIG	+= qt thread rtti exceptions warn_on  release  plugin
TEMPLATE = lib
CUBE +=
CUBE_SRC = C:/Scalasca-Soft/Install/Cube2.0/$$(PREFIX)/install
GUI_SRC = ../../../src/GUI-qt
GUI_CONFIG = ../../../vpath/src
DEPENDPATH += $${GUI_SRC} $${GUI_SRC}/display
QT_MINGW = C:/Qt/Tools/mingw730_64
INCLUDEPATH += \
    	../include \
	$${GUI_SRC} \
	$${GUI_CONFIG}  \
	$${GUI_SRC}/display   \
	$${GUI_SRC}/display/plugins   \
	$${GUI_SRC}/display/utils   \
	$${CUBE_SRC}/include/cubelib \
	$${QT_MINGW}/opt/include \
	"C:/Program Files (x86)/GnuWin32/include" 
	 
LIBS += -L$${CUBE_SRC}/lib -lcube -L../GUI-qt-core/release -lcube4gui 
LIBS += -L"C:/Program Files (x86)/GnuWin32/lib" -lz

DEFINES+= CUBE_PREFIX=\\\"unknown\\\" 
DEFINES+= LIBDIR=\\\".\\\"


QMAKE_CXXFLAGS=-fexceptions

QT += widgets network printsupport concurrent

# Input{
HEADERS += \
$${GUI_SRC}/plugins/CubeDiff/CubeDiff.h 


SOURCES += \
$${GUI_SRC}/plugins/CubeDiff/CubeDiff.cpp



# Hack for IBM AIX
aix-g++ {
  QMAKE_CXXFLAGS_RELEASE += -O0
}
aix-g++-64 {
  QMAKE_CXXFLAGS_RELEASE += -O0
}

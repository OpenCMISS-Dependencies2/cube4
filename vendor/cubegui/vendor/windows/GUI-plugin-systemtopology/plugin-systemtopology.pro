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


CONFIG	+= qt thread rtti exceptions warn_on  release plugin
TEMPLATE = lib
CUBE +=
CUBE_SRC = C:/Scalasca-Soft/Install/Cube2.0/$$(PREFIX)/install
GUI_SRC = ../../../src/GUI-qt
GUI_CONFIG = ../../../vpath/src
DEPENDPATH += $${GUI_SRC} $${GUI_SRC}/display
INCLUDEPATH +=  \
    ../include \
	$${GUI_SRC} \
	$${GUI_CONFIG}  \
	$${GUI_SRC}/display   \
	$${GUI_SRC}/display/plugins   \
	$${GUI_SRC}/display/utils   \
	$${CUBE_SRC}/include/cubelib 
	 
LIBS += -L$${CUBE_SRC}/lib -lcube  -L../GUI-qt-core/release -lcube4gui 

DEFINES+= CUBE_PREFIX=\\\"unknown\\\" 
DEFINES+= LIBDIR=\\\".\\\"


QMAKE_CXXFLAGS=-fexceptions

QT += widgets network printsupport concurrent

# Input{
HEADERS += \
$${GUI_SRC}/plugins/SystemTopology/DimensionSelectionWidget.h\
$${GUI_SRC}/plugins/SystemTopology/OrderWidget.h\
$${GUI_SRC}/plugins/SystemTopology/Plane.h\
$${GUI_SRC}/plugins/SystemTopology/Point.h\
$${GUI_SRC}/plugins/SystemTopology/SystemTopology.h\
$${GUI_SRC}/plugins/SystemTopology/SystemTopologyData.h\
$${GUI_SRC}/plugins/SystemTopology/SystemTopologyDrawing.h\
$${GUI_SRC}/plugins/SystemTopology/SystemTopologyToolBar.h\
$${GUI_SRC}/plugins/SystemTopology/SystemTopologyView.h\
$${GUI_SRC}/plugins/SystemTopology/SystemTopologyViewTransform.h\
$${GUI_SRC}/plugins/SystemTopology/SystemTopologyWidget.h\
$${GUI_SRC}/plugins/SystemTopology/Tetragon.h\
$${GUI_SRC}/plugins/SystemTopology/TopologyDimensionBar.h

SOURCES += \
$${GUI_SRC}/plugins/SystemTopology/DimensionSelectionWidget.cpp\
$${GUI_SRC}/plugins/SystemTopology/OrderWidget.cpp\
$${GUI_SRC}/plugins/SystemTopology/Plane.cpp\
$${GUI_SRC}/plugins/SystemTopology/Point.cpp\
$${GUI_SRC}/plugins/SystemTopology/SystemTopology.cpp\
$${GUI_SRC}/plugins/SystemTopology/SystemTopologyData.cpp\
$${GUI_SRC}/plugins/SystemTopology/SystemTopologyDrawing.cpp\
$${GUI_SRC}/plugins/SystemTopology/SystemTopologyToolBar.cpp\
$${GUI_SRC}/plugins/SystemTopology/SystemTopologyViewTransform.cpp\
$${GUI_SRC}/plugins/SystemTopology/SystemTopologyWidget.cpp\
$${GUI_SRC}/plugins/SystemTopology/Tetragon.cpp\
$${GUI_SRC}/plugins/SystemTopology/TopologyDimensionBar.cpp



# Hack for IBM AIX
aix-g++ {
  QMAKE_CXXFLAGS_RELEASE += -O0
}
aix-g++-64 {
  QMAKE_CXXFLAGS_RELEASE += -O0
}

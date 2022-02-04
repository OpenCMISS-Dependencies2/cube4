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
DEPENDPATH += $${GUI_SRC} $${GUI_SRC}/display $${GUI_SRC}/plugins/Graphwidget-common
INCLUDEPATH += \
        	../include \
		$${GUI_SRC} \
		$${GUI_CONFIG} \
		$${GUI_SRC}/display \
		$${GUI_SRC}/display/plugins \
		$${GUI_SRC}/display/utils \
		$${GUI_SRC}/plugins/Graphwidget-common   \
		$${CUBE_SRC}/include/cubelib \
        "C:/Program Files (x86)/GnuWin32/include" 
	 
LIBS += -L$${CUBE_SRC}/lib -lcube  -L../GUI-qt-core/release -lcube4gui -L../GUI-plugin-graphwidgetcommon/release -lplugin-graphwidgetcommon 
LIBS += -L"C:/Program Files (x86)/GnuWin32/lib" -lz

DEFINES+= CUBE_PREFIX=\\\"unknown\\\" 
DEFINES+= LIBDIR=\\\".\\\"


QMAKE_CXXFLAGS=-fexceptions

QT += widgets network printsupport concurrent

# Input{
HEADERS += \
$${GUI_SRC}/plugins/Graphwidget-common/AbstractPlotArea.h  \
$${GUI_SRC}/plugins/Graphwidget-common/AbstractPlotAreaController.h  \
$${GUI_SRC}/plugins/Barplot/Barplot.h  \
$${GUI_SRC}/plugins/Barplot/BarplotArea.h \
$${GUI_SRC}/plugins/Barplot/BarplotController.h \
$${GUI_SRC}/plugins/Barplot/PlotsListController.h \
$${GUI_SRC}/plugins/Barplot/SettingsWidget.h

SOURCES += \
$${GUI_SRC}/plugins/Barplot/Barplot.cpp \
$${GUI_SRC}/plugins/Barplot/BarplotController.cpp \
$${GUI_SRC}/plugins/Barplot/BarplotArea.cpp\
$${GUI_SRC}/plugins/Barplot/PlotsListController.cpp\
$${GUI_SRC}/plugins/Barplot/SettingsWidget.cpp 



# Hack for IBM AIX
aix-g++ {
  QMAKE_CXXFLAGS_RELEASE += -O0
}
aix-g++-64 {
  QMAKE_CXXFLAGS_RELEASE += -O0
}

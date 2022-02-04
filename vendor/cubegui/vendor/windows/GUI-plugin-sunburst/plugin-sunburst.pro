##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2021                                                ##
##  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          ##
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
        $${GUI_SRC}/plugins/Sunburst/DataAccessFunctions.h \
        $${GUI_SRC}/plugins/Sunburst/InfoToolTip.h \
        $${GUI_SRC}/plugins/Sunburst/SunburstCursorData.h \
        $${GUI_SRC}/plugins/Sunburst/SunburstPainter.h \
        $${GUI_SRC}/plugins/Sunburst/SunburstSettingsHandler.h \
        $${GUI_SRC}/plugins/Sunburst/SunburstShapeData.h \
        $${GUI_SRC}/plugins/Sunburst/SystemSunburstPlugin.h \
        $${GUI_SRC}/plugins/Sunburst/TransformationData.h \
        $${GUI_SRC}/plugins/Sunburst/UIEventWidget.h


SOURCES += \
        $${GUI_SRC}/plugins/Sunburst/DataAccessFunctions.cpp \
        $${GUI_SRC}/plugins/Sunburst/InfoToolTip.cpp \
        $${GUI_SRC}/plugins/Sunburst/SunburstCursorData.cpp \
        $${GUI_SRC}/plugins/Sunburst/SunburstPainter.cpp \
        $${GUI_SRC}/plugins/Sunburst/SunburstSettingsHandler.cpp \
        $${GUI_SRC}/plugins/Sunburst/SunburstShapeData.cpp \
        $${GUI_SRC}/plugins/Sunburst/SystemSunburstPlugin.cpp \
        $${GUI_SRC}/plugins/Sunburst/TransformationData.cpp \
        $${GUI_SRC}/plugins/Sunburst/UIEventWidget.cpp 



# Hack for IBM AIX
aix-g++ {
  QMAKE_CXXFLAGS_RELEASE += -O0
}
aix-g++-64 {
  QMAKE_CXXFLAGS_RELEASE += -O0
}

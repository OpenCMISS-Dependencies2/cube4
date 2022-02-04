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
INCLUDEPATH += \
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
$${GUI_SRC}/plugins/MetricEditor/MetricEditor.h \
$${GUI_SRC}/plugins/MetricEditor/DerivedMetricEditor.h \
$${GUI_SRC}/plugins/MetricEditor/DerivedMetricsCollection.h \
$${GUI_SRC}/plugins/MetricEditor/CubePLSyntaxHighlighter.h \
$${GUI_SRC}/plugins/MetricEditor/MetricData.h \
$${GUI_SRC}/plugins/MetricEditor/HtmlHelpCollection.cpp  \
$${GUI_SRC}/plugins/MetricEditor/NewDerivatedMetricWidget.h


SOURCES += \
$${GUI_SRC}/plugins/MetricEditor/MetricEditor.cpp \
$${GUI_SRC}/plugins/MetricEditor/DerivedMetricEditor.cpp \
$${GUI_SRC}/plugins/MetricEditor/DerivedMetricsCollection.cpp \
$${GUI_SRC}/plugins/MetricEditor/CubePLSyntaxHighlighter.cpp \
$${GUI_SRC}/plugins/MetricEditor/NewDerivatedMetricWidget.cpp \
$${GUI_SRC}/plugins/MetricEditor/HtmlHelpCollection.cpp  \
$${GUI_SRC}/plugins/MetricEditor/MetricData.cpp 



# Hack for IBM AIX
aix-g++ {
  QMAKE_CXXFLAGS_RELEASE += -O0
}
aix-g++-64 {
  QMAKE_CXXFLAGS_RELEASE += -O0
}

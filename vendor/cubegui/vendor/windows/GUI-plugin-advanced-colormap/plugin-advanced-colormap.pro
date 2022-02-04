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
INCLUDEPATH +=  \
	../include \
	$${GUI_SRC} \
	$${GUI_CONFIG}  \
	$${GUI_SRC}/display   \
	$${GUI_SRC}/display/plugins   \
	$${GUI_SRC}/display/utils   \
	$${GUI_SRC}/plugins/AdvancedColorMaps/   \
	$${GUI_SRC}/plugins/AdvancedColorMaps/maps   \
	$${CUBE_SRC}/include/cubelib
	 
LIBS += -L$${CUBE_SRC}/lib -lcube  -L../GUI-qt-core/release -lcube4gui 

DEFINES+= CUBE_PREFIX=\\\"unknown\\\" 
DEFINES+= LIBDIR=\\\".\\\"


QMAKE_CXXFLAGS=-fexceptions
QMAKE_LDFLAGS="-Wl,--enable-auto-import" 


QT += widgets network printsupport concurrent

# Input{
HEADERS += \
$${GUI_SRC}/display/ColorMap.h \
$${GUI_SRC}/plugins/AdvancedColorMaps/AdvancedColorMaps.h \
$${GUI_SRC}/plugins/AdvancedColorMaps/AdvancedColorMapsSettings.h	\
$${GUI_SRC}/plugins/AdvancedColorMaps/ColorMapsFactory.h	\
$${GUI_SRC}/plugins/AdvancedColorMaps/ColorMapExtended.h	\
$${GUI_SRC}/plugins/AdvancedColorMaps/ColorMapPlot.h	\
$${GUI_SRC}/plugins/AdvancedColorMaps/RGBDefinerWidget.h	\
$${GUI_SRC}/plugins/AdvancedColorMaps/maps/SequentialColorMap.h	\
$${GUI_SRC}/plugins/AdvancedColorMaps/maps/DivergentColorMap.h	\
$${GUI_SRC}/plugins/AdvancedColorMaps/maps/CubehelixColorMap.h	\
$${GUI_SRC}/plugins/AdvancedColorMaps/maps/ImprovedRainbowColorMap.h


SOURCES += \
$${GUI_SRC}/plugins/AdvancedColorMaps/AdvancedColorMaps.cpp \
$${GUI_SRC}/plugins/AdvancedColorMaps/AdvancedColorMapsSettings.cpp \
$${GUI_SRC}/plugins/AdvancedColorMaps/ColorMapsFactory.cpp \
$${GUI_SRC}/plugins/AdvancedColorMaps/ColorMapExtended.cpp	\
$${GUI_SRC}/plugins/AdvancedColorMaps/ColorMapPlot.cpp \
$${GUI_SRC}/plugins/AdvancedColorMaps/RGBDefinerWidget.cpp \
$${GUI_SRC}/plugins/AdvancedColorMaps/maps/SequentialColorMap.cpp \
$${GUI_SRC}/plugins/AdvancedColorMaps/maps/DivergentColorMap.cpp \
$${GUI_SRC}/plugins/AdvancedColorMaps/maps/CubehelixColorMap.cpp \
$${GUI_SRC}/plugins/AdvancedColorMaps/maps/ImprovedRainbowColorMap.cpp




# Hack for IBM AIX
aix-g++ {
  QMAKE_CXXFLAGS_RELEASE += -O0
}
aix-g++-64 {
  QMAKE_CXXFLAGS_RELEASE += -O0
}

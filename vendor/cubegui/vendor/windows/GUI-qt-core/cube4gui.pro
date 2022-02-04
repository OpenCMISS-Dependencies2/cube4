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


CONFIG	+= qt thread rtti exceptions warn_on  release c++1z
TEMPLATE = lib
CUBE +=
CUBE_SRC = C:/Scalasca-Soft/Install/Cube2.0/$$(PREFIX)/install
GUI_SRC = ../../../src/GUI-qt
GUI_CONFIG = ../../../vpath/src
DEPENDPATH += $${GUI_SRC} \
              $${GUI_SRC}/display \
			  $${GUI_SRC}/display/plugins \
			  $${GUI_SRC}/display/utils 
			  
INCLUDEPATH += \
    ../include \
    $${GUI_SRC}  \
	$${GUI_CONFIG}  \
	$${GUI_SRC}/display  \
	$${GUI_SRC}/display/plugins  \
	$${GUI_SRC}/display/utils  \
	$${CUBE_SRC}/include/cubelib
	 
LIBS += -L$${CUBE_SRC}/lib -lcube 
RESOURCES = $${GUI_SRC}/cubeguires.qrc

DEFINES+= CUBE_PREFIX=\\\"unknown\\\" 
DEFINES+= LIBDIR=\\\".\\\"


QMAKE_CXXFLAGS=-fexceptions

QT += network widgets printsupport concurrent

# Input{
HEADERS += \
	$${GUI_SRC}/display/AggregatedTreeItem.h \
	$${GUI_SRC}/display/CallTree.h \
	$${GUI_SRC}/display/CallTreeLabelDialog.h \
	$${GUI_SRC}/display/CallTreeView.h \
	$${GUI_SRC}/display/ColorMap.h \
	$${GUI_SRC}/display/Constants.h \
	$${GUI_SRC}/display/CubeQtStreamSocket.h \
	$${GUI_SRC}/display/CubeWriter.h \
	$${GUI_SRC}/display/DefaultCallTree.h \
	$${GUI_SRC}/display/DefaultColorMap.h \
	$${GUI_SRC}/display/DefaultValueView.h \
	$${GUI_SRC}/display/DimensionOrderDialog.h \
	$${GUI_SRC}/display/FlatTree.h \
	$${GUI_SRC}/display/FlatTreeView.h \
	$${GUI_SRC}/display/Future.h \
	$${GUI_SRC}/display/Globals.h \
	$${GUI_SRC}/display/MainWidget.h \
	$${GUI_SRC}/display/MetricTree.h \
	$${GUI_SRC}/display/MetricTreeView.h \
	$${GUI_SRC}/display/PrecisionWidget.h \
	$${GUI_SRC}/display/Settings.h \
	$${GUI_SRC}/display/SettingsHandler.h \
	$${GUI_SRC}/display/SettingsToolBar.h \
	$${GUI_SRC}/display/SynchronizationToolBar.h \
	$${GUI_SRC}/display/SystemTree.h \
	$${GUI_SRC}/display/SystemTreeView.h \
	$${GUI_SRC}/display/TabInterface.h \
	$${GUI_SRC}/display/TabManager.h \
	$${GUI_SRC}/display/TabWidget.h \
    $${GUI_SRC}/display/TaskCallTree.h \
	$${GUI_SRC}/display/Task.h \
	$${GUI_SRC}/display/Tree.h \
	$${GUI_SRC}/display/TreeConfig.h \
	$${GUI_SRC}/display/TreeItem.h \
	$${GUI_SRC}/display/TreeItemMarkerDialog.h \
	$${GUI_SRC}/display/TreeModel.h \
	$${GUI_SRC}/display/TreeModelProxy.h \
	$${GUI_SRC}/display/TreeStatistics.h \
	$${GUI_SRC}/display/TreeView.h \
	$${GUI_SRC}/display/ValueWidget.h \
	$${GUI_SRC}/display/plugins/ContextFreePlugin.h \
	$${GUI_SRC}/display/plugins/ContextFreeServices.h \
	$${GUI_SRC}/display/plugins/CubePlugin.h \
	$${GUI_SRC}/display/plugins/PluginList.h \
	$${GUI_SRC}/display/plugins/PluginManager.h \
	$${GUI_SRC}/display/plugins/PluginServices.h \
	$${GUI_SRC}/display/plugins/PluginServiceExports.h \
	$${GUI_SRC}/display/plugins/PluginSettingDialog.h \
	$${GUI_SRC}/display/plugins/ValueView.h \
	$${GUI_SRC}/display/utils/BoxPlot.h \
	$${GUI_SRC}/display/utils/ColorScale.h \
	$${GUI_SRC}/display/utils/Compatibility.h \
	$${GUI_SRC}/display/utils/CubeApplication.h \
	$${GUI_SRC}/display/utils/DefaultMarkerLabel.h \
	$${GUI_SRC}/display/utils/DownloadThread.h \
	$${GUI_SRC}/display/utils/Environment.h \
	$${GUI_SRC}/display/utils/FindWidget.h \
	$${GUI_SRC}/display/utils/HelpBrowser.h \
	$${GUI_SRC}/display/utils/HmiInstrumentation.h \
	$${GUI_SRC}/display/utils/HtmlWidget.h \
	$${GUI_SRC}/display/utils/HtmlWidgetTextView.h \
	$${GUI_SRC}/display/utils/InfoWidget.h \
	$${GUI_SRC}/display/utils/Pretty.h \
	$${GUI_SRC}/display/utils/ProgressBar.h \
	$${GUI_SRC}/display/utils/PresentationCursor.h \
	$${GUI_SRC}/display/utils/RemoteFileDialog.h \
	$${GUI_SRC}/display/utils/RemoteFileSystemModel.h \
	$${GUI_SRC}/display/utils/ScrollArea.h \
	$${GUI_SRC}/display/utils/StatisticalInformation.h \
	$${GUI_SRC}/display/utils/StatusBar.h \
	$${GUI_SRC}/display/utils/StyleSheetEditor.h \
	$${GUI_SRC}/display/utils/TableWidget.h \
	$${GUI_SRC}/display/utils/ValueModusCombo.h \
	$${GUI_SRC}/display/utils/ValueViewConfig.h \
	$${GUI_SRC}/display/utils/VersionCheckWidget.h 

SOURCES += \
	$${GUI_SRC}/display/AggregatedTreeItem.cpp \
	$${GUI_SRC}/display/CallTree.cpp \
	$${GUI_SRC}/display/CallTreeLabelDialog.cpp \
	$${GUI_SRC}/display/CallTreeView.cpp \
	$${GUI_SRC}/display/CubeQtStreamSocket.cpp \
	$${GUI_SRC}/display/CubeWriter.cpp \
	$${GUI_SRC}/display/DefaultCallTree.cpp \
	$${GUI_SRC}/display/DefaultColorMap.cpp \
	$${GUI_SRC}/display/DefaultValueView.cpp \
	$${GUI_SRC}/display/DimensionOrderDialog.cpp \
	$${GUI_SRC}/display/FlatTree.cpp \
	$${GUI_SRC}/display/FlatTreeView.cpp \
	$${GUI_SRC}/display/Future.cpp \
	$${GUI_SRC}/display/Globals.cpp \
	$${GUI_SRC}/display/MainWidget.cpp \
	$${GUI_SRC}/display/MetricTree.cpp \
	$${GUI_SRC}/display/MetricTreeView.cpp \
	$${GUI_SRC}/display/PrecisionWidget.cpp \
	$${GUI_SRC}/display/Settings.cpp \
	$${GUI_SRC}/display/SettingsToolBar.cpp \
	$${GUI_SRC}/display/SynchronizationToolBar.cpp \
	$${GUI_SRC}/display/SystemTree.cpp \
	$${GUI_SRC}/display/SystemTreeView.cpp \
	$${GUI_SRC}/display/TabManager.cpp \
	$${GUI_SRC}/display/TabWidget.cpp \
    $${GUI_SRC}/display/TaskCallTree.cpp \
	$${GUI_SRC}/display/Tree.cpp \
	$${GUI_SRC}/display/TreeConfig.cpp \
	$${GUI_SRC}/display/TreeItem.cpp \
	$${GUI_SRC}/display/TreeItemMarker.cpp \
	$${GUI_SRC}/display/TreeItemMarkerDialog.cpp \
	$${GUI_SRC}/display/TreeModel.cpp \
	$${GUI_SRC}/display/TreeModelProxy.cpp \
	$${GUI_SRC}/display/TreeStatistics.cpp \
	$${GUI_SRC}/display/TreeView.cpp \
	$${GUI_SRC}/display/ValueWidget.cpp \
	$${GUI_SRC}/display/plugins/ContextFreeServices.cpp \
	$${GUI_SRC}/display/plugins/PluginList.cpp \
	$${GUI_SRC}/display/plugins/PluginManager.cpp \
	$${GUI_SRC}/display/plugins/PluginServices.cpp \
	$${GUI_SRC}/display/plugins/PluginSettingDialog.cpp \
	$${GUI_SRC}/display/utils/BoxPlot.cpp \
	$${GUI_SRC}/display/utils/DefaultMarkerLabel.cpp \
	$${GUI_SRC}/display/utils/ColorScale.cpp \
	$${GUI_SRC}/display/utils/CubeApplication.cpp \
	$${GUI_SRC}/display/utils/DownloadThread.cpp \
	$${GUI_SRC}/display/utils/Environment.cpp \
	$${GUI_SRC}/display/utils/FindWidget.cpp \
	$${GUI_SRC}/display/utils/HelpBrowser.cpp \
	$${GUI_SRC}/display/utils/HtmlWidget.cpp \
	$${GUI_SRC}/display/utils/HtmlWidgetTextView.cpp \
	$${GUI_SRC}/display/utils/InfoWidget.cpp \
	$${GUI_SRC}/display/utils/Pretty.cpp \
	$${GUI_SRC}/display/utils/ProgressBar.cpp \
	$${GUI_SRC}/display/utils/PresentationCursor.cpp \
	$${GUI_SRC}/display/utils/RemoteFileDialog.cpp \
	$${GUI_SRC}/display/utils/RemoteFileSystemModel.cpp \
	$${GUI_SRC}/display/utils/ScrollArea.cpp \
	$${GUI_SRC}/display/utils/StatisticalInformation.cpp \
	$${GUI_SRC}/display/utils/StatusBar.cpp \
	$${GUI_SRC}/display/utils/StyleSheetEditor.cpp \
	$${GUI_SRC}/display/utils/TableWidget.cpp \
	$${GUI_SRC}/display/utils/ValueModusCombo.cpp \
	$${GUI_SRC}/display/utils/ValueViewConfig.cpp \
	$${GUI_SRC}/display/utils/VersionCheckWidget.cpp

# Additional dependencies
# PRE_TARGETDEPS += $CUBE/lib/libcube4.a



# Hack for IBM AIX
aix-g++ {
  QMAKE_CXXFLAGS_RELEASE += -O0
}
aix-g++-64 {
  QMAKE_CXXFLAGS_RELEASE += -O0
}

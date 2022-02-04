##*************************************************************************##
##  CUBE        http://www.scalasca.org/                                   ##
##*************************************************************************##
##  Copyright (c) 1998-2019                                                ##
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
# Simple check for shipped plugins

AC_DEFUN([AC_CUBE_SUPPORTED_PLUGINS],
[
## plugin example
AC_MSG_CHECKING([ plugin "Launch"])
AC_CUBE_PLUGIN_LAUNCH
AM_CONDITIONAL([WITH_GUI_PLUGIN_LAUNCH], [test x$WITH_GUI_PLUGIN_LAUNCH == "xyes"])

## plugin ScorePConfig
AC_MSG_CHECKING([ plugin "Scaling"])
AC_CUBE_PLUGIN_SCALING
AM_CONDITIONAL([WITH_GUI_PLUGIN_SCALING], [test x$WITH_GUI_PLUGIN_SCALING == "xyes"])


## plugin ScorePConfig
AC_MSG_CHECKING([ plugin "ScorePConfig"])
AC_CUBE_PLUGIN_SCOREP_CONFIG
AM_CONDITIONAL([WITH_GUI_PLUGIN_SCOREP_CONFIG], [test x$WITH_GUI_PLUGIN_SCOREP_CONFIG == "xyes"])

## plugin TreeItemMarker
AC_MSG_CHECKING([ plugin "TreeItemMarker"])
AC_CUBE_PLUGIN_CUBE_TREEITEM_MARKER
AM_CONDITIONAL([WITH_GUI_PLUGIN_CUBE_TREEITEM_MARKER], [test x$WITH_GUI_PLUGIN_CUBE_TREEITEM_MARKER == "xyes"])

## plugin MetricIdentify
AC_MSG_CHECKING([ plugin "MetricIdentify"])
AC_CUBE_PLUGIN_CUBE_METRIC_IDENTIFY
AM_CONDITIONAL([WITH_GUI_PLUGIN_CUBE_METRIC_IDENTIFY], [test x$WITH_GUI_PLUGIN_CUBE_METRIC_IDENTIFY == "xyes"])


## plugin MetricEditor
AC_MSG_CHECKING([ plugin "MetricEditor"])
AC_CUBE_PLUGIN_CUBE_METRIC_EDITOR
AM_CONDITIONAL([WITH_GUI_PLUGIN_CUBE_METRIC_EDITOR], [test x$WITH_GUI_PLUGIN_CUBE_METRIC_EDITOR == "xyes"])

## plugin CubeDiff
AC_MSG_CHECKING([ plugin "CubeDiff"])
AC_CUBE_PLUGIN_CUBE_DIFF
AM_CONDITIONAL([WITH_GUI_PLUGIN_CUBE_DIFF], [test x$WITH_GUI_PLUGIN_CUBE_DIFF == "xyes"])

## plugin CubeMean
AC_MSG_CHECKING([ plugin "CubeMean"])
AC_CUBE_PLUGIN_CUBE_MEAN
AM_CONDITIONAL([WITH_GUI_PLUGIN_CUBE_MEAN], [test x$WITH_GUI_PLUGIN_CUBE_MEAN == "xyes"])

## plugin CubeMerge
AC_MSG_CHECKING([ plugin "CubeMerge"])
AC_CUBE_PLUGIN_CUBE_MERGE
AM_CONDITIONAL([WITH_GUI_PLUGIN_CUBE_MERGE], [test x$WITH_GUI_PLUGIN_CUBE_MERGE == "xyes"])

## plugin CubeMerge
AC_MSG_CHECKING([ plugin "Advisor"])
AC_CUBE_PLUGIN_ADVISOR
AM_CONDITIONAL([WITH_GUI_PLUGIN_ADVISOR], [test x$WITH_GUI_PLUGIN_ADVISOR == "xyes"])



AC_MSG_CHECKING([ plugin "Tau2Cube"])
AC_CUBE_PLUGIN_TAU2CUBE
AM_CONDITIONAL([WITH_GUI_PLUGIN_TAU2CUBE], [test x$WITH_GUI_PLUGIN_TAU2CUBE == "xyes"])


## plugin graphwidgetcommon
AC_MSG_CHECKING([ plugin "graphwidgetcommon"])
AC_CUBE_PLUGIN_GRAPHWIDGETCOMMON
AM_CONDITIONAL([WITH_GUI_PLUGIN_GRAPHWIDGET_COMMON], [test x$WITH_GUI_PLUGIN_GRAPHWIDGETCOMMON == "xyes"])

## plugin barplot
AC_MSG_CHECKING([ plugin "Barplot"])
AC_CUBE_PLUGIN_BARPLOT
AM_CONDITIONAL([WITH_GUI_PLUGIN_BARPLOT], [test x$WITH_GUI_PLUGIN_BARPLOT == "xyes"])

## plugin heatmap
AC_MSG_CHECKING([ plugin "Heatmap"])
AC_CUBE_PLUGIN_HEATMAP
AM_CONDITIONAL([WITH_GUI_PLUGIN_HEATMAP], [test x$WITH_GUI_PLUGIN_HEATMAP == "xyes"])

## plugin source
AC_MSG_CHECKING([ plugin "SourceEditor"])
AC_CUBE_PLUGIN_SOURCE_EDITOR
AM_CONDITIONAL([WITH_GUI_PLUGIN_SOURCE_EDITOR], [test x$WITH_GUI_PLUGIN_SOURCE_EDITOR == "xyes"])

## plugin statistics
AC_MSG_CHECKING([ plugin "Statistics"])
AC_CUBE_PLUGIN_STATISTICS
AM_CONDITIONAL([WITH_GUI_PLUGIN_STATISTICS], [test x$WITH_GUI_PLUGIN_STATISTICS == "xyes"])

## plugin vampir
AC_CUBE_PLUGIN_VAMPIR
AM_CONDITIONAL([WITH_GUI_PLUGIN_VAMPIR], [test x$WITH_GUI_PLUGIN_VAMPIR == "xyes"])

## plugin paraver
AC_MSG_CHECKING([ plugin "Paraver"])
AC_CUBE_PLUGIN_PARAVER
AM_CONDITIONAL([WITH_GUI_PLUGIN_PARAVER], [test x$WITH_GUI_PLUGIN_PARAVER == "xyes"])

## plugin advanced color map	
AC_MSG_CHECKING([ plugin "AdvancedColorMaps"])
AC_CUBE_PLUGIN_ADVANCEDCM
AM_CONDITIONAL([WITH_GUI_PLUGIN_ADVANCEDCM], [test x$WITH_GUI_PLUGIN_ADVANCEDCM == "xyes"])

## plugin sunburst
AC_MSG_CHECKING([ plugin "Sunburst"])
AC_CUBE_PLUGIN_SUNBURST
AM_CONDITIONAL([WITH_GUI_PLUGIN_SUNBURST], [test x$WITH_GUI_PLUGIN_SUNBURST == "xyes"])


## plugin system box plot
AC_MSG_CHECKING([ plugin "System Statistics"])
AC_CUBE_PLUGIN_SYSTEM_STATISTICS
AM_CONDITIONAL([WITH_GUI_PLUGIN_SYSTEM_STATISTICS], [test x$WITH_GUI_PLUGIN_SYSTEM_STATISTICS == "xyes"])


## plugin system topology
AC_MSG_CHECKING([ plugin "System Topology"])
AC_CUBE_PLUGIN_SYSTEM_TOPOLOGY
AM_CONDITIONAL([WITH_GUI_PLUGIN_SYSTEM_TOPOLOGY], [test x$WITH_GUI_PLUGIN_SYSTEM_TOPOLOGY == "xyes"])
])





AC_DEFUN([AC_CUBE_PLUGIN_SYSTEM_TOPOLOGY],
[
WITH_GUI_PLUGIN_SYSTEM_TOPOLOGY=yes
AC_ARG_WITH(plugin-system-topology, [AS_HELP_STRING([--with-plugin-system-topology | --without-plugin-system-topology], 
          [ Enables (default) or disables build and installation of a plugin "System Topology"])], [], [])

AS_IF( [ test x$with_plugin_system_topology = "xno" || test x$without_plugin_system_topology = "xyes"], [
WITH_GUI_PLUGIN_SYSTEM_TOPOLOGY="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])


AC_DEFUN([AC_CUBE_PLUGIN_SYSTEM_STATISTICS],
[
WITH_GUI_PLUGIN_SYSTEM_STATISTICS=yes
AC_ARG_WITH(plugin-system-statistics, [AS_HELP_STRING([--with-plugin-system-statistics | --without-plugin-system-statistics], 
          [ Enables (default) or disables build and installation of a plugin "System BoxPlot"])], [], [])

AS_IF( [ test x$with_plugin_system_statistics = "xno" || test x$without_plugin_system_statistics = "xyes"], [
WITH_GUI_PLUGIN_SYSTEM_STATISTICS="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])

AC_DEFUN([AC_CUBE_PLUGIN_ADVISOR],
[
WITH_GUI_PLUGIN_ADVISOR=yes
AC_ARG_WITH(plugin-advisor, [AS_HELP_STRING([--with-plugin-advisor | --without-plugin-advisor], 
          [ Enables (default) or disables build and installation of a plugin "Advisor"])], [], [])

AS_IF( [ test x$with_plugin_advisor = "xno" || test x$without_plugin_advisor = "xyes"], [
WITH_GUI_PLUGIN_ADVISOR="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])

AC_DEFUN([AC_CUBE_PLUGIN_SUNBURST],
[
WITH_GUI_PLUGIN_SUNBURST=yes
AC_ARG_WITH(plugin-sunburst, [AS_HELP_STRING([--with-plugin-sunburst | --without-plugin-sunburst], 
          [ Enables (default) or disables build and installation of a plugin "Sunburst"])], [], [])

AS_IF( [ test x$with_plugin_sunburst = "xno" || test x$without_plugin_sunburst = "xyes"], [
WITH_GUI_PLUGIN_SUNBURST="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])

AC_DEFUN([AC_CUBE_PLUGIN_SCALING],
[
WITH_GUI_PLUGIN_SCALING=yes
AC_ARG_WITH(plugin-scaling, [AS_HELP_STRING([--with-plugin-scaling | --without-plugin-scaling], 
          [ Enables (default) or disables build and installation of a plugin "Scaling"])], [], [])

AS_IF( [ test x$with_plugin_scaling = "xno" || test x$without_plugin_scaling = "xyes"], [
WITH_GUI_PLUGIN_SCALING="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])


AC_DEFUN([AC_CUBE_PLUGIN_CUBE_DIFF],
[
WITH_GUI_PLUGIN_CUBE_DIFF=yes
AC_ARG_WITH(plugin-cube-diff, [AS_HELP_STRING([--with-plugin-cube-diff | --without-plugin-cube-diff], 
          [ Enables (default) or disables build and installation of a plugin "CubeDiff"])], [], [])

AS_IF( [ test x$with_plugin_cube_diff = "xno" || test x$without_plugin_cube_diff = "xyes"], [
WITH_GUI_PLUGIN_CUBE_DIFF="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])

AC_DEFUN([AC_CUBE_PLUGIN_SCOREP_CONFIG],
[
WITH_GUI_PLUGIN_SCOREP_CONFIG=yes
AC_ARG_WITH(plugin-scorep-config, [AS_HELP_STRING([--with-plugin-scorep-config | --without-plugin-scorep-config],
          [ Enables (default) or disables build and installation of a plugin "ScorePConfig"])], [], [])
AS_IF( [ test x$with_plugin_scorep_config = "xno" || test x$without_plugin_scorep_config = "xyes"], [
WITH_GUI_PLUGIN_SCOREP_CONFIG="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])


AC_DEFUN([AC_CUBE_PLUGIN_CUBE_MEAN],
[
WITH_GUI_PLUGIN_CUBE_MEAN=yes
AC_ARG_WITH(plugin-cube-mean, [AS_HELP_STRING([--with-plugin-cube-mean | --without-plugin-cube-mean],
          [ Enables (default) or disables build and installation of a plugin "CubeMean"])], [], [])

AS_IF( [ test x$with_plugin_cube-mean = "xno" || test x$without_plugin_cube-mean = "xyes"], [
WITH_GUI_PLUGIN_CUBE_MEAN="no";
AC_MSG_RESULT([ deactivate.])
], [
AC_MSG_RESULT([ activate.])
])
])



AC_DEFUN([AC_CUBE_PLUGIN_CUBE_MERGE],
[
WITH_GUI_PLUGIN_CUBE_MERGE=yes
AC_ARG_WITH(plugin-cube-merge, [AS_HELP_STRING([--with-plugin-cube-merge | --without-plugin-cube-merge],
          [ Enables (default) or disables build and installation of a plugin "CubeMerge"])], [], [])

AS_IF( [ test x$with_plugin_cube-merge = "xno" || test x$without_plugin_cube-merge = "xyes"], [
WITH_GUI_PLUGIN_CUBE_MERGE="no";
AC_MSG_RESULT([ deactivate.])
], [
AC_MSG_RESULT([ activate.])
])
])

AC_DEFUN([AC_CUBE_PLUGIN_TAU2CUBE],
[
WITH_GUI_PLUGIN_TAU2CUBE=yes
AC_ARG_WITH(plugin_tau2cube, [AS_HELP_STRING([--with-plugin-tau2cube | --without-plugin-tau2cube],
          [ Enables (default) or disables build and installation of a plugin "Tau2Cube"])], [], [])

AS_IF( [ test x$with_plugin_tau2cube = "xno" || test x$without_plugin_tau2cube = "xyes"], [
WITH_GUI_PLUGIN_TAU2CUBE="no";
AC_MSG_RESULT([ deactivate.])
], [
AC_MSG_RESULT([ activate.])
])
])




AC_DEFUN([AC_CUBE_PLUGIN_CUBE_TREEITEM_MARKER],
[
WITH_GUI_PLUGIN_CUBE_TREEITEM_MARKER=yes
AC_ARG_WITH(plugin-treeitem-marker, [AS_HELP_STRING([--with-plugin-treeitem-marker | --without-plugin-treeitem-marker], 
          [ Enables (default) or disables build and installation of a plugin "CubeDiff"])], [], [])

AS_IF( [ test x$with_plugin_treeitem_marker = "xno" || test x$without_plugin_treeitem_marker = "xyes"], [
WITH_GUI_PLUGIN_CUBE_TREEITEM_MARKER="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])

AC_DEFUN([AC_CUBE_PLUGIN_CUBE_METRIC_IDENTIFY],
[
WITH_GUI_PLUGIN_CUBE_METRIC_IDENTIFY=yes
AC_ARG_WITH(plugin-metric-identify, [AS_HELP_STRING([--with-plugin-metric-identify | --without-plugin-metric-identify], 
          [ Enables (default) or disables build and installation of a plugin "CubeDiff"])], [], [])

AS_IF( [ test x$with_plugin_cube_metric-identify = "xno" || test x$without_plugin_cube_metric-identify = "xyes"], [
WITH_GUI_PLUGIN_CUBE_METRIC_IDENTIFY="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])

AC_DEFUN([AC_CUBE_PLUGIN_CUBE_METRIC_EDITOR],
[
WITH_GUI_PLUGIN_CUBE_METRIC_EDITOR=yes
AC_ARG_WITH(plugin-metriceditor, [AS_HELP_STRING([--with-plugin-metriceditor | --without-plugin-metriceditor], 
          [ Enables (default) or disables build and installation of a plugin "CubeDiff"])], [], [])

AS_IF( [ test x$with_plugin_cube_metriceditor = "xno" || test x$without_plugin_cube_metriceditor = "xyes"], [
WITH_GUI_PLUGIN_CUBE_METRIC_EDITOR="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])

AC_DEFUN([AC_CUBE_PLUGIN_LAUNCH],
[
WITH_GUI_PLUGIN_LAUNCH=yes
AC_ARG_WITH(plugin-launch, [AS_HELP_STRING([--with-plugin-launch | --without-plugin-launch], 
          [ Enables (default) or disables build and installation of a plugin "Launch"])], [], [])

AS_IF( [ test x$with_plugin_launch = "xno" || test x$without_plugin_launch = "xyes" ], [
WITH_GUI_PLUGIN_LAUNCH="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])


])


AC_DEFUN([AC_CUBE_PLUGIN_BARPLOT],
[
WITH_GUI_PLUGIN_BARPLOT=yes
AC_ARG_WITH(plugin-barplot, [AS_HELP_STRING([--with-plugin-barplot| --without-plugin-barplot ], 
          [ Enables (default) or disables build and installation of a plugin "Barplot"])], [], [])

AS_IF( [ test x$with_plugin_barplot = "xno" || test x$without_plugin_barplot = "xyes"], [
WITH_GUI_PLUGIN_BARPLOT="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])

AC_DEFUN([AC_CUBE_PLUGIN_HEATMAP],
[
WITH_GUI_PLUGIN_HEATMAP=yes
AC_ARG_WITH(plugin-heatmap, [AS_HELP_STRING([--with-plugin-heatmap | --without-plugin-heatmap], 
          [ Enables (default) or disables build and installation of a plugin "Heatmap"])], [], [])

AS_IF( [ test x$with_plugin_heatmap = "xno" || test x$without_plugin_heatmap = "xyes"], [
WITH_GUI_PLUGIN_HEATMAP="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])


AC_DEFUN([AC_CUBE_PLUGIN_STATISTICS],
[
WITH_GUI_PLUGIN_STATISTICS=yes
AC_ARG_WITH(plugin-statistics, [AS_HELP_STRING([--with-plugin-statistics | --without-plugin-statistics], 
          [ Enables (default) or disables build and installation of a plugin "Statistics"])], [], [])

AS_IF( [ test x$with_plugin_statistics = "xno" || test x$without_plugin_statistics = "xyes"], [
WITH_GUI_PLUGIN_STATISTICS="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])


AC_DEFUN([AC_CUBE_PLUGIN_SOURCE_EDITOR],
[
WITH_GUI_PLUGIN_SOURCE_EDITOR=yes
AC_ARG_WITH(plugin-source, [AS_HELP_STRING([--with-plugin-source | --without-plugin-source], 
          [ Enables (default) or disables build and installation of a plugin "Source Code Viewer"])], [], [])

AS_IF( [ test x$with_plugin_source = "xno" || test x$without_plugin_source = "xyes"], [
WITH_GUI_PLUGIN_SOURCE_EDITOR="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])


AC_DEFUN([AC_CUBE_PLUGIN_VAMPIR],
[
WITH_GUI_PLUGIN_VAMPIR=yes
AC_ARG_WITH(plugin-vampir, [AS_HELP_STRING([--with-plugin-vampir | --without-plugin-vampir], 
          [ Enables (default) or disables build and installation of a plugin "Vampir connection"])], [], [])

AS_IF( [ test x$with_plugin_vampir = "xno" || test x$without_plugin_vampir = "xyes"], [
WITH_GUI_PLUGIN_VAMPIR="no";
AC_MSG_CHECKING([ plugin "Vampir"])
AC_MSG_RESULT([ deactivate.])
], [ 
  WITH_GUI_PLUGIN_VAMPIR="yes"

  AC_DBUS_AVAILABLE

AC_MSG_CHECKING([ plugin "Vampir"])
AS_IF( [ test x$DBUS_AVAILABLE = "xno"], [
WITH_GUI_PLUGIN_VAMPIR="no"
AC_MSG_RESULT([ deactivate.])
],[
AC_MSG_RESULT([ activate.])
])

])
])

AC_DEFUN([AC_CUBE_PLUGIN_PARAVER],
[
WITH_GUI_PLUGIN_PARAVER=yes
AC_ARG_WITH(plugin-paraver, [AS_HELP_STRING([--with-plugin-paraver | --without-plugin-paraver], 
          [ Enables (default) or disables build and installation of a plugin "Paraver connection"])], [], [])

AS_IF( [ test x$with_plugin_paraver = "xno" || test x$without_plugin_paraver = "xyes"], [
WITH_GUI_PLUGIN_PARAVER="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])



AC_DEFUN([AC_CUBE_PLUGIN_GRAPHWIDGETCOMMON],
[
WITH_GUI_PLUGIN_GRAPHWIDGETCOMMON=yes
AC_MSG_RESULT([ activate.]) 
#AC_ARG_WITH(plugin-graphwidgetcommon, [AS_HELP_STRING([--with-plugin-graphwidgetcommon | --without-plugin-graphwidgetcommon],  
          #[ Enables (default) or disables build and installation of a plugin "GraphwidgetCommon"])], [], []) 


#AS_IF( [ test x$with_plugin_graphwidgetcommon = "xno" ], [
#WITH_GUI_PLUGIN_GRAPHWIDGETCOMMON="no";
#AC_MSG_RESULT([ deactivate.])
#], [ 
#AC_MSG_RESULT([ activate.])
#])
AC_DEFUN([AC_CUBE_PLUGIN_ADVANCEDCM],
[
WITH_GUI_PLUGIN_ADVANCEDCM=yes
AC_ARG_WITH(plugin-advancedcolormaps, [AS_HELP_STRING([--with-plugin-advancedcolormaps | --without-plugin-advancedcolormaps], 
          [ Enables (default) or disables build and installation of a plugin "Advanced Color Maps"])], [], [])

AS_IF( [ test x$with_plugin_advancedcolormaps = "xno" || test x$without_plugin_advancedcolormaps = "xyes"], [
WITH_GUI_PLUGIN_ADVANCEDCM="no";
AC_MSG_RESULT([ deactivate.])
], [ 
AC_MSG_RESULT([ activate.])
])
])


])

/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PLUGINSERVICEEXPORTS_H
#define PLUGINSERVICEEXPORTS_H

#include "Globals.h"

namespace cubegui
{
class TreeItem;
class AggregatedTreeItem;
class ColorMap;
class SettingsHandler;
class MarkerLabel;
class DefaultMarkerLabel;
class Task;
class Future;
};

namespace cubepluginapi
{
// classes
using cubegui::ColorMap;
using cubegui::SettingsHandler;
using cubegui::TreeItem;
using cubegui::AggregatedTreeItem;
using cubegui::MarkerLabel;
using cubegui::DefaultMarkerLabel;
using cubegui::Task;
using cubegui::Future;

// enums;
using cubegui::DisplayType;
using cubegui::TabType;
using cubegui::LineType;
using cubegui::TreeType;
using cubegui::MessageType;
using cubegui::TreeItemType;
using cubegui::ValueModus;
using cubegui::PrecisionFormat;
using cubegui::UserAction;

// enum PrecisionFormat
using cubegui::FORMAT_TREES;
using cubegui::FORMAT_DEFAULT;
using cubegui::FORMAT_DOUBLE;

// enum MessageType
using cubegui::Verbose;
using cubegui::Information;
using cubegui::Warning;
using cubegui::Error;
using cubegui::Critical;

// enum TreeItemType
using cubegui::METRICITEM;
using cubegui::CALLITEM;
using cubegui::REGIONITEM;
using cubegui::SYSTEMTREENODEITEM;
using cubegui::LOCATIONGROUPITEM;
using cubegui::LOCATIONITEM;

// enum DisplayType
using cubegui::METRIC;
using cubegui::CALL;
using cubegui::SYSTEM;

// enum TreeType
using cubegui::METRICTREE;
using cubegui::DEFAULTCALLTREE;
using cubegui::FLATTREE;
using cubegui::TASKTREE;
using cubegui::SYSTEMTREE;

// enum LineType
using cubegui::BLACK_LINES;
using cubegui::GRAY_LINES;
using cubegui::WHITE_LINES;
using cubegui::NO_LINES;

// enum DisplayType { METRIC, CALL;SYSTEM };
using cubegui::METRIC;
using cubegui::CALL;
using cubegui::SYSTEM;

// enum TabType
using cubegui::DEFAULT_TAB;
using cubegui::OTHER_PLUGIN_TAB;

// enum UserAction { LoopMarkAction };
using cubegui::LoopMarkAction;


//enum ValueModus
using cubegui::ABSOLUTE_VALUES;
using cubegui::OWNROOT_VALUES;
using cubegui::METRICROOT_VALUES;
using cubegui::CALLROOT_VALUES;
using cubegui::SYSTEMROOT_VALUES;
using cubegui::METRICSELECTED_VALUES;
using cubegui::CALLSELECTED_VALUES;
using cubegui::SYSTEMSELECTED_VALUES;
using cubegui::PEER_VALUES;
using cubegui::PEERDIST_VALUES;
using cubegui::EXTERNAL_VALUES;
};

#endif // PLUGINSERVICEEXPORTS_H

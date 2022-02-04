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




#ifndef _CONSTANTS_H
#define _CONSTANTS_H

// the application's name
// #define CUBE_NAME "Cube 4.0.1 RC"

#include "cubegui-version.h"

// // #ifndef CUBE_SHORT_NAME
// // #warning "Something is broken in build system. Macro CUBE_SHORT_NAME should be defined in Makefiles."
// // #define CUBE_SHORT_NAME "cube"
// // #endif
// // #ifndef CUBE_VERSION
// // #warning "Something is broken in build system. Macro CUBE_VERSION should be defined in Makefiles."
// // #define CUBE_VERSION "0.0.0"
// // #endif
// // #ifndef CUBE_CODENAME
// // #warning "Something is broken in build system. Macro CUBE_CODENAME should be defined in Makefiles."
// // #define CUBE_CODENAME "snapshot"
// // #endif
// // #ifndef CUBE_REVISION
// // #warning "Something is broken in build system. Macro CUBE_REVISION should be defined in Makefiles."
// // #define CUBE_REVISION "0"
// // #endif
// //
// // #define CUBE_NAME CUBE_SHORT_NAME " "CUBE_VERSION " "CUBE_CODENAME

// initial margin for the topology display when resetting the display
#define TOPOLOGY_MARGIN    10

// margin and spacing for the main widget's layout
#define LAYOUT_MARGIN 2
#define LAYOUT_SPACING 2

// names of value modi as displayed in the combos;
// see mainwidget.cpp for further explanation
#define ABSOLUTE_NAME "Absolute"
#define OWNROOT_NAME "Own root percent"
#define METRICSELECTED_NAME "Metric selection percent"
#define CALLSELECTED_NAME "Call selection percent"
#define SYSTEMSELECTED_NAME "System selection percent"
#define METRICROOT_NAME "Metric root percent"
#define CALLROOT_NAME "Call root percent"
#define SYSTEMROOT_NAME "System root percent"
#define PEER_NAME "Peer percent"
#define PEERDIST_NAME "Peer distribution"
#define EXTERNAL_NAME "External percent"

namespace cubegui
{
// the internal type for value modus
enum ValueModus { ABSOLUTE_VALUES,
                  OWNROOT_VALUES,
                  METRICROOT_VALUES,
                  CALLROOT_VALUES,
                  SYSTEMROOT_VALUES,
                  METRICSELECTED_VALUES,
                  CALLSELECTED_VALUES,
                  SYSTEMSELECTED_VALUES,
                  PEER_VALUES,
                  PEERDIST_VALUES,
                  EXTERNAL_VALUES };

// the internal type for the lines in the topology display
enum LineType { BLACK_LINES, GRAY_LINES, WHITE_LINES, NO_LINES };

#define DISPLAYS  3
// identifier for the three main panels
enum DisplayType { METRIC, CALL, SYSTEM };

// identifier for the trees types inside the main panels
enum TreeType { METRICTREE, DEFAULTCALLTREE, FLATTREE, TASKTREE, SYSTEMTREE };

// plugin tab type, used to sort plugin tabs
enum TabType { DEFAULT_TAB, OTHER_PLUGIN_TAB };

// action that the user activated in the GUI, used to notify plugins
enum UserAction { LoopMarkAction };

// We support two precision types:
// FORMAT_TREES for the tree widgets and
// FORMAT_DEFAULT e.g. for the value widgets (below the tab widgets) and for topologies
enum PrecisionFormat { FORMAT_TREES = 0, FORMAT_DEFAULT, FORMAT_DOUBLE };


// In tree widgets there is possibility to hide items.
// We support 3 hiding modi:
// NO_HIDING     : All items are non-hidden.
// DYNAMIC_HYDING: Entering this modus, all items get non-hidden.
//                The user defines a threshold percent value. All items whose color position
//                is below this threshold on the color scale, get hidden.
//                If the selections and implicitely the current values change,
//                hiding is re-computed (thus items are hidden dynamically depending on the current values).
// STATIC_HIDING : The user can explicitely hide and show states.
//                When changing into this modus, previously hidden states keep being (statically) hidden.
//                Value changes have no effect on hiding.
// Each tree has its independent hiding modus.
enum HidingState { STATIC_HIDING, DYNAMIC_HIDING, NO_HIDING };

// the type of tree items
enum TreeItemType { METRICITEM, CALLITEM, REGIONITEM, SYSTEMTREENODEITEM, LOCATIONGROUPITEM, LOCATIONITEM };

// available methods to distribute the colors on the color scale
enum ColoringMethod { LINEAR, QUADRATIC1, QUADRATIC2, EXPONENTIAL1, EXPONENTIAL2 };

// modi how to compute values for items
// INCLUSIVE            : the total value for the item's subtree, e.g. for collapsed items
// EXCLUSIVE            : the item's own value
// EXCLUSIVE_PLUS_HIDDEN: the item's own value plus the inclusive value of all hidden children
enum ItemState { EXCLUSIVE, INCLUSIVE, EXCLUSIVE_PLUS_HIDDEN };

// type for defining how selected items in trees should be marked: by background or by framing
enum SelectionSyntax { BACKGROUND_SELECTION, FRAME_SELECTION };
};


#endif

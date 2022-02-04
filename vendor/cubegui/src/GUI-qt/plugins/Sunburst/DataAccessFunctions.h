/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2015-2016                                                **
**  RWTH Aachen University, JARA-HPC                                       **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef DATA_ACCESS_FUNCTIONS_H
#define DATA_ACCESS_FUNCTIONS_H

// Cube forward declarations
namespace cubepluginapi
{
class PluginServices;
}

namespace cubegui
{
class TreeItem;
}

// Sunburst forward declarations
namespace cube_sunburst
{
class SunburstCursorData;
class SunburstShapeData;
class TransformationData;
}

// Qt includes
#include <QList>
#include <QPoint>
#include <QStringList>

namespace cube_sunburst
{
namespace detail
{
/// @brief Calculates the depth of the tree for the entire tree or the visible tree.
///
/// @param root
///     Root element of (sub-)tree
///
/// @return
///     Depth of the tree. Minimum value is 1 (leaf).
///
int
getTreeDepth( const cubegui::TreeItem& root );

/// @brief Calculates the number of elements at a given tree depth.
///
/// @param root
///     Root element of (sub-)tree
/// @param level
///     Current (recursion) level
///
/// @return
///     Number of items on specified level
///
int
getQuantityOfLevel( const cubegui::TreeItem& root,
                    int                      level );

/// @brief Calculates a list of elements at a given tree depth.
///
/// @param root
///     Root element of (sub-)tree
/// @param level
///     Current (recursion) level
///
/// @return
///     List of items on specified level
///
QList< cubegui::TreeItem* >
getElementsOfLevel( cubegui::TreeItem& root,
                    int                level );

/// @brief Calculates a list of const elements at a given tree depth.
///
/// @param root
///     Root element of (sub-)tree
/// @param level
///     Current (recursion) level
///
/// @return
///     List of items on specified level
///
QList< const cubegui::TreeItem* >
getElementsOfLevel( const cubegui::TreeItem& root,
                    int                      level );

/*
   bool
   levelExpanded(const cubegui::TreeItem& root,
              int             level);*/

/// @brief Checks the visibility of the given item.
///
/// @param item
///     Checked item
///
/// @return
///     True if visible
///
bool
itemVisible( const cubegui::TreeItem& item );

/// @brief Calculates the arcPosition indeces (innerRadia/outerRadia index, degrees index) of the given item regarded from the root item.
///
/// @param root
///     Root element of (sub-)tree
/// @param item
///     Regarded item
///
/// @return
///     arcPosition indeces (innerRadia/outerRadia index, degrees index) stored in int pair
///
QPoint
getItemLocation( const cubegui::TreeItem& root,
                 const cubegui::TreeItem& item );


/// @brief Returns the tooltipText for the given TreeItem.
///
/// @param item
///     Regarded item
/// @param service
///     PluginServices storing format information.
///
/// @return
///     Tooltip text in a string list
///
// modified copy from src/GUI-qt/plugins/SystemTopology/SystemTopologyData.h
QStringList
getTooltipText( const cubegui::TreeItem&             item,
                const cubepluginapi::PluginServices& service );


/// @brief Returns the TreeItem* corresponding to the given item.
///
/// @param shapeData
///     shapeData of the regarded sunburst
/// @param item
///     an item of the regarded sunburst
///
/// @return
///     corresponding TreeItem* of item
///
cubegui::TreeItem*
getTreeItem( SunburstShapeData& shapeData,
             const QPoint&      item );


/// @brief Returns the position of the item at the given position.
///
/// @param shapeData
///     shapeData of a sunburst
/// @param transformationData
///     transformationData of a scene
/// @param position
///     Pixel coordinates
///
/// @return
///     Cursor data for the given pixel coordinates.
///
SunburstCursorData
getCursorData( SunburstShapeData&  shapeData,
               TransformationData& transformationData,
               const QPointF&      position );


/// @brief Resizes the given arc and evenly resizes the arcs of the same and higher levels.
///
/// @param shapeData
///     shapeData of the regarded sunburst
/// @param cursorData
///     Regarded item and information about the resize direction
/// @param newAngle
///     New starting angle of the item.
///
void
resizeArc( SunburstShapeData&  shapeData,
           SunburstCursorData& cursorData,
           qreal               newAngle );


/// @brief Checks whether the given angle can be used as newAngle for a resize interaction
///
/// @param shapeData
///     shapeData of the regarded sunburst
/// @param cursorData
///     Regarded item and information about the resize direction
/// @param newAngle
///     New starting angle of the item.
///
/// @return
///     writes 0 if the process was successful, 1 if the angle was too big, 2 if too small.
///
int
checkAngle( SunburstShapeData&  shapeData,
            SunburstCursorData& cursorData,
            qreal               newAngle );
}      // namespace detail
}      // namespace cube_sunburst

#endif /* DATA_ACCESS_FUNCTIONS_H */

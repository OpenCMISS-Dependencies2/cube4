/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2015-2016                                                **
**  RWTH Aachen University, JARA-HPC                                       **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SUNBURST_SHAPE_DATA_H
#define SUNBURST_SHAPE_DATA_H

// Cube forward declarations
namespace cubepluginapi
{
class PluginServices;
}

namespace cubegui
{
class TreeItem;
}

// Qt includes
#include <QPoint>
#include <QVector>
#include <vector>

namespace cube_sunburst
{
/// Simple Class for storing the degree data of sunburst arcs
class DegreeData
{
public:
    // public member methods
    /// Getter for degree.
    qreal
    getDegree( int level,
               int index );

    /// Getter for degree (overloaded).
    qreal
    getDegree( const QPoint& item );

    /// Setter for degrees.
    void
    setDegree( int   level,
               int   index,
               qreal value );

    /// Setter for degrees (overloaded).
    void
    setDegree( const QPoint& item,
               qreal         value );

    /// Resizes the data structure and resets values.
    void
    resize( int                     numberOfLevels,
            const QVector< qreal >& itemsPerLevel );

    /// Calls count() of the outer vector.
    qreal
    count();

    /// Calls count() of the inner vector at level.
    qreal
    count( int level );

    /// Checks level and index as valid indexes for degrees.
    bool
    itemExists( int level,
                int index );

protected:
    // protected member attributes
    /// Simple data structure to store all the degrees.
    QVector< QVector< qreal > > degrees;
};


/// Module for storing the shape data of the sunburst (not the position!).
class SunburstShapeData
{
public:
    // public member methods
    /// Resets all variables defining the shape of the sunburst. The given arguments define the number of levels and number of elements per level.
    void
    reset( int                     numberOfLevels,
           const QVector< qreal >& itemsPerLevel );

    /// Resets the degree values to have even arc sizes.
    void
    resetDegrees();

    /// Resets expanded and visible to hide all items.
    void
    resetVisibilityData();

    /// Returns the level at the given radius.
    int
    getLevel( qreal radius );

    /// Returns the number of levels.
    int
    getNumberOfLevels();

    /// Returns the number of elements at the given level.
    int
    getNumberOfElements( int level );

    /// Setter for innerRadia.
    void
    setInnerRadius( int   index,
                    qreal value );

    /// Getter for innerRadia at the given index.
    qreal
    getInnerRadius( int index );

    /// Setter for outerRadia.
    void
    setOuterRadius( int   index,
                    qreal value );

    /// Getter for innerRadia at the given index.
    qreal
    getOuterRadius( int index );

    /// Returns the number of levels that have at least one visible element.
    int
    numberOfVisibleLevels();

    /// Returns the number of levels that are fully expanded and visible.
    int
    numberOfCompleteLevels();

    /// Setter for service.
    void
    setService( cubepluginapi::PluginServices* service );

    /// Getter for service.
    cubepluginapi::PluginServices*
    getService();

    /// Setter for topLevelItem.
    void
    setTopLevelItem( cubegui::TreeItem* item );

    /// Getter for topLevelItem.
    cubegui::TreeItem*
    getTopLevelItem();

    /// Check function whether the data is complete and valid.
    bool
    isValid();

    /// Get the index of the parent of the given item.
    int
    getParentIndex( int level,
                    int index );

    /// Get the range of children of a given item (first and last index in a QPoint)
    QPoint
    getRangeOfChildren( int level,
                        int index );

    /// Get the number of children of a given item
    int
    getNumberOfChildren( int level,
                         int index );

    /// Getter for the absolute degree.
    qreal
    getAbsDegree( int level,
                  int index );

    /// Getter for the absolute degree of the successor of item. Returns 360.0 instead of 0.0.
    qreal
    getSuccAbsDegree( int level,
                      int index );

    /// Getter for the relative degree to the position of its siblings.
    qreal
    getRelDegree( int level,
                  int index );

    /// Setter for degrees.
    void
    setRelDegree( int   level,
                  int   index,
                  qreal value );

    /// Calculated absDegrees from relDegrees.
    void
    calculateAbsDegrees();

    /// Setter for absDegreeOffset.
    void
    setAbsDegreeOffset( qreal angle );

    /// Getter for absDegreeOffset.
    qreal
    getAbsDegreeOffset();

    /// Check the existence of an item.
    bool
    itemExists( int level,
                int index );

    /// Check the existence of an item (overloaded).
    bool
    itemExists( const QPoint& item );

    /// Check whether an item is visible.
    bool
    itemIsVisible( int level,
                   int index );

    /// Check whether an item is visible (overloaded).
    bool
    itemIsVisible( const QPoint& item );

    /// Check whether an item is expanded.
    bool
    getExpanded( int level,
                 int index );

    /// Check whether an item is expanded (overloaded).
    bool
    getExpanded( const QPoint& item );

    /// Set the expanded status of an item.
    void
    setExpanded( int  level,
                 int  index,
                 bool value );

    /// Set the expanded status of an item (overloaded).
    void
    setExpanded( const QPoint& item,
                 bool          value );

    /// Updates level sizes. Call manually for efficiency after many calls to setExpanded().
    void
    updateLevelSizes();

    static qreal
    getMaxSizeDivisor();

    static qreal
    getExpandButtonWidth();

    static qreal
    getCollapseButtonWidth();

private:
    // private member methods
    void
    hideDescendants( int level,
                     int index );

    void
    showDescendants( int level,
                     int index );

    // private member attributes
    /// The vector that stores the inner radia of all ring levels.
    QVector< qreal > innerRadia;

    /// The vector that stores the inner radia of all ring levels.
    QVector< qreal > outerRadia;

    /// Stored the degrees relative to the position of their siblings from 0 to 1.
    DegreeData relDegrees;

    /// Stores the absolute degrees. This is derived from RelDegreeData and stored to avoid recomputations.
    DegreeData absDegrees;

    /// Stores visibility data for all elements.
    QVector< QVector< bool > > expanded;

    /// Derived from expanded: Cached to save computation time. 0 = false, 1 = true
    QVector< std::vector< int > > visible;

    /// An offset enabling the arc with index 0 at the first level with more than one arc to be different than 0.0.
    qreal absDegreeOffset;

    /// PluginServices needed to select and check for selections.
    cubepluginapi::PluginServices* service;

    /// The topLevelItem defining the tree we are working on.
    cubegui::TreeItem* topLevelItem;

    // design decisions
    /// Defines the minimal size of an item.
    static const qreal maxSizeDivisor;

    /// A divisor applied to the size of complete levels
    static const qreal completeLevelSizeDivisor;

    /// The fraction of a level that is a gap to the next level.
    static const qreal levelGapRelativeSize;

    /// Defines the size of the expand button relative to the level size.
    static const qreal expandButtonWidth;

    /// Defines the size of the collapse button relative to the level size.
    static const qreal collapseButtonWidth;
};
} // namespace cube_sunburst

#endif    /* SUNBURST_SHAPE_DATA */

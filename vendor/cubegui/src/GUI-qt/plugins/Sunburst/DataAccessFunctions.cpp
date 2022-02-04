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


/*-------------------------------------------------------------------------*/
/**
 *  @file    DataAccessFunctions.cpp
 *  @ingroup CubeSunburst
 *  @brief   Definition of the functional module DataAccessFunctions.
 **/
/*-------------------------------------------------------------------------*/

#include <config.h>

#include "DataAccessFunctions.h"

// general includes
#include <cassert>
#include <cmath>

// Cube includes
#include <CubeLocation.h>
#include <CubeLocationGroup.h>
#include <CubeProcess.h>
#include <CubeSysres.h>
#include <CubeSystemTreeNode.h>
#include <PluginServices.h>
#include <TreeItem.h>

// Sunburst includes
#include "SunburstCursorData.h"
#include "SunburstShapeData.h"
#include "TransformationData.h"

// introduce Cube namespaces
using namespace cubegui;
using namespace cubepluginapi;

using namespace cube_sunburst;


int
detail::getTreeDepth( const TreeItem& root )
{
    // This is the base case of our recursive approach.
    if ( root.isLeaf() )
    {
        return 1;
    }

    // Recursively call this function for the children of root.
    int max = 0;
    foreach( TreeItem * current, root.getChildren() )
    {
        int newValue = getTreeDepth( *current );
        if ( newValue > max )
        {
            max = newValue;
        }
    }
    return max + 1;
}


int
detail::getQuantityOfLevel( const TreeItem& root,
                            int             level )
{
    // Just to cover the case 0. Not needed for the recursive approach for level > 1.
    if ( level == 0 )
    {
        return 1;
    }

    // This is the base case of our recursive approach.
    if ( level == 1 )
    {
        return root.getChildren().count();
    }
    int sum = 0;

    // Recursively call this function for the children of root. It will decrease level until it reaches the base case 1.
    foreach( TreeItem * current, root.getChildren() )
    {
        sum += getQuantityOfLevel( *current, level - 1 );
    }

    return sum;
}


QList< TreeItem* >
detail::getElementsOfLevel( TreeItem& root,
                            int       level )
{
    // Just to cover the case 0. Not needed for the recursive approach for level > 1.
    if ( level == 0 )
    {
        QList< TreeItem* > newList;
        newList.append( &root );

        return newList;
    }

    // This is the base case of our recursive approach.
    if ( level == 1 )
    {
        return root.getChildren();
    }

    // Recursively call this function for the children of root. It will decrease level until it reaches the base case 1.
    QList< TreeItem* > newList;
    foreach( TreeItem * current, root.getChildren() )
    {
        newList += getElementsOfLevel( *current, level - 1 );
    }

    return newList;
}


// needed to reinterpret QList<TreeItem*> as QList<const TreeItem*> in the following function getElementsOfLevel
template< class T >
inline QList< T const* > const&
constList( QList< T* > const& list )
{
    return reinterpret_cast< QList< T const* > const& >(
        list
        );
}


QList< const TreeItem* >
detail::getElementsOfLevel( const TreeItem& root,
                            int             level )
{
    // Just to cover the case 0. Not needed for the recursive approach for level > 1.
    if ( level == 0 )
    {
        QList< const TreeItem* > newList;
        newList.append( &root );

        return newList;
    }

    // This is the base case of our recursive approach.
    if ( level == 1 )
    {
        return constList( root.getChildren() );
    }

    // Recursively call this function for the children of root. It will decrease level until it reaches the base case 1.
    QList< const TreeItem* > newList;
    foreach( const TreeItem * current, constList( root.getChildren() ) )
    {
        newList += getElementsOfLevel( *current, level - 1 );
    }

    return newList;
}

/*
   bool
   detail::levelExpanded(const TreeItem& root,
                      int             level)
   {
    QList< const TreeItem* > levelList = getElementsOfLevel(root, level);
    foreach(const TreeItem * current, levelList)
    {
        if (!current->isExpanded())
        {
            return false;
        }
    }

    return true;
   }*/


bool
detail::itemVisible( const TreeItem& item )
{
    return item.isTopLevelItem()
           || (  item.getParent()->isExpanded()
                 && itemVisible( *item.getParent() ) );
}


// expensive
QPoint
detail::getItemLocation( const TreeItem& root,
                         const TreeItem& item )
{
    int                      level     = item.getDepth() - 1;
    QList< const TreeItem* > levelList = getElementsOfLevel( root, level );
    return QPoint( level, levelList.indexOf( &item ) );
}


// local procedure used by getTooltipText
void
appendNodeRankInfo( const TreeItem& item,
                    QString&        rankStr,
                    QString&        idStr,
                    QString&        nodeStr )
{
    cube::Sysres* sysres = ( ( cube::Sysres* )item.getCubeObject() );
    if ( item.getType() == LOCATIONGROUPITEM )
    {
        rankStr.append( QString::number( ( ( cube::Process* )sysres )->get_rank() ) );
        if ( ( ( cube::Process* )sysres )->num_children() == 0 )
        {
            idStr.append( "-" );
        }
        else
        {
            assert( ( ( cube::Process* )sysres )->num_children() == 1 );
            idStr.append( QString::number( ( ( cube::Process* )sysres )->get_child( 0 )
                                           ->get_rank() ) );
        }
    }
    else if ( item.getType() == LOCATIONITEM )
    {
        rankStr.append( QString::number( ( ( cube::Process* )sysres )->get_rank() ) );
        if ( ( ( cube::Process* )sysres )->num_children() == 0 )
        {
            idStr.append( "-" );
        }
        else
        {
            assert( ( ( cube::Process* )sysres )->num_children() == 1 );
            idStr.append( QString::number( ( ( cube::Process* )sysres )->get_child( 0 )
                                           ->get_rank() ) );
        }
    }
    nodeStr.append( QString::fromStdString( ( ( cube::Process* )sysres )->get_parent()
                                            ->get_name() ) );
}


// modified copy from src/GUI-qt/plugins/SystemTopology/SystemTopologyData.cpp
QStringList
detail::getTooltipText( const TreeItem&       item,
                        const PluginServices& service )
{
    QStringList ret;
    QString     nameStr, valueStr, absValueStr, rankStr, idStr, nodeStr;
/*
    bool   userDefinedMinMaxValues;
    double userMinValue, userMaxValue;
    userDefinedMinMaxValues = service.getUserDefinedMinValues(SYSTEMTREE,
                                                              userMinValue,
                                                              userMaxValue);
 */
    nameStr.append( item.getName() );
    bool intType = service.intMetricSelected();

    // compute percentage of the value on the color scale
    double value = item.getValue();
    valueStr.append( service.formatNumber( value, FORMAT_DOUBLE, intType ) );

    QString text1, text2;

    if ( item.isLeaf() )
    {
        appendNodeRankInfo( item, rankStr, idStr, nodeStr );
    }
    else
    {
        // display the range of the child nodes, means the name of the first and the last leaf by rank
        QList< const TreeItem* > leafChildren = getElementsOfLevel( item,
                                                                    getTreeDepth(
                                                                        item )
                                                                    - 1 );

        rankStr.append( "(" );
        idStr.append( "(" );
        nodeStr.append( "(" );
        appendNodeRankInfo( *leafChildren.first(), rankStr, idStr, nodeStr );

        if ( leafChildren.count() == 2 )
        {
            rankStr.append( ", " );
            idStr.append( ", " );
            nodeStr.append( ", " );
        }
        else
        {
            rankStr.append( ", .., " );
            idStr.append( ", .., " );
            nodeStr.append( ", .., " );
        }

        appendNodeRankInfo( *leafChildren.last(), rankStr, idStr, nodeStr );
        rankStr.append( ")" );
        idStr.append( ")" );
        nodeStr.append( ")" );

        // display range info
        text1.append( QObject::tr( "No. leafs:" ) );
        text1.append( "\n\n" );

        text2.append( QString( "%1" ).arg( leafChildren.count() ) );
        text2.append( "\n" );
        if ( leafChildren.count() == 2 )
        {
            text2.append( QObject::tr( "(first, second)" ) );
        }
        else
        {
            text2.append( QObject::tr( "(first, .., last)" ) );
        }
        text2.append( "\n" );
    }

    text1.append( QObject::tr( "Node:" ) );
    text1.append( "\n" );

    text1.append( QObject::tr( "Name:" ) );
    text1.append( "\n" );

    text1.append( QObject::tr( "MPI rank:" ) );
    text1.append( "\n" );

    text1.append( QObject::tr( "Thread id:" ) );
    text1.append( "\n" );

    text1.append( QObject::tr( "Value:" ) );

    if ( service.getValueModus() != ABSOLUTE_VALUES )
    {
        text1.append( "\n" );
        text1.append( QObject::tr( "Absolute:" ) );
    }

    // text1.append( "\n" );

    text2.append( nodeStr );
    text2.append( "\n" );

    text2.append( nameStr );
    text2.append( "\n" );

    text2.append( rankStr );
    text2.append( "\n" );

    text2.append( idStr );
    text2.append( "\n" );

    text2.append( valueStr );

    if ( service.getValueModus() != ABSOLUTE_VALUES )
    {
        text2.append( "\n" );
        text2.append( absValueStr );
    }

    // text2.append( "\n" );

    ret.append( text1 );
    ret.append( text2 );

    return ret;
}


TreeItem*
detail::getTreeItem( SunburstShapeData& shapeData,
                     const QPoint&      item )
{
    if ( !shapeData.isValid() || !shapeData.itemExists( item ) )
    {
        return NULL;
    }
    // rename for improved readability
    const int itemLevel = item.x();
    const int itemIndex = item.y();

    // get the selected item using the calculated level and angle
    QList< TreeItem* > levelList = getElementsOfLevel(
        *shapeData.getTopLevelItem(),
        itemLevel );
    return levelList.at( itemIndex );
}


// local function used by getCursorData
/* Returns true if there is a button located at the given position. The item positions are uniquely defined by the
 * combination of the given shapeData and transformationData. Returns false if there is no button at the position. */
bool
buttonSelected( SunburstShapeData&  shapeData,
                TransformationData& transformationData,
                qreal               diff_norm )
{
    // calculate the ring level from diff_norm with regard to the current zoom
    int level = shapeData.getLevel( diff_norm / transformationData.getBoundingRect().width() * 2.0
                                    / transformationData.getZoomFactor() );
    if ( level < 0 || level >= shapeData.numberOfVisibleLevels() )
    {
        return false;
    }

    // calculate the arc radia from level
    qreal innerRadius = shapeData.getInnerRadius( level );
    qreal outerRadius = shapeData.getOuterRadius( level );
    qreal buttonWidth = shapeData.getExpandButtonWidth();
    if ( level < shapeData.numberOfCompleteLevels() - 1 )
    {
        buttonWidth = shapeData.getCollapseButtonWidth();
    }

    // return true if the position is within the bounds of the button which lies within the selected arc
    return diff_norm / transformationData.getBoundingRect().width() * 2.0
           / transformationData.getZoomFactor()
           >= innerRadius + ( outerRadius - innerRadius ) * ( 1 - buttonWidth );
}


/** Calculates the SunburstCursorData corresponding to the given pixel coordinates. The item
 * positions are uniquely defined by the combination of the given shapeData and transformationData.
 * Returns SunburstCursorData(-1,0) if there is no item at the position. */
SunburstCursorData
detail::getCursorData( SunburstShapeData&  shapeData,
                       TransformationData& transformationData,
                       const QPointF&      position )
{
    if ( !shapeData.isValid() || !transformationData.getBoundingRect().isValid() )
    {
        return SunburstCursorData( -1, 0 );
    }

    // calculate the norm of the diff vector from the center to the position
    const QPoint center    = transformationData.getBoundingRect().center();
    QPointF      diff      = QPointF( position.x() - center.x(), position.y() - center.y() ) - transformationData.getDrawingShiftError();
    qreal        diff_norm = sqrt( pow( diff.x(), 2 ) + pow( diff.y(), 2 ) );

    // calculate the angle of the diff vector (relative to the unit circle)
    qreal phi;
    if ( diff.x() != 0 )
    {
        phi = atan( -1 * diff.y() / diff.x() ) / ( 2 * M_PI ) * 360;
        if ( diff.x() < 0 )
        {
            phi += 180;
        }
        else if ( diff.y() > 0 )
        {
            phi += 360;
        }
    }
    else
    {
        if ( diff.y() < 0 )
        {
            phi = 90;
        }
        else
        {
            phi = 270;
        }
    }

    // take rotation into account
    phi = fmod( phi - transformationData.getRotation() + 720, 360 );

    // calculate the ring level from diff_norm with regard to the current zoom
    int level = shapeData.getLevel( diff_norm / transformationData.getBoundingRect().width() * 2.0
                                    / transformationData.getZoomFactor() );

    // get the selected item using the calculated level and angle
    bool nearLowerBorder = false; // initialization only to avoid a warning
    int  currentIndex    = 0;
    for ( int i = 0; i < shapeData.getNumberOfElements( level ); i++ )
    {
        if ( phi > shapeData.getAbsDegree( level, i ) )
        {
            currentIndex = i;
            // now also check for the orientation
            if ( phi > ( shapeData.getAbsDegree( level, i ) + ( ( shapeData.getSuccAbsDegree( level, i ) - shapeData.getAbsDegree( level, i ) ) / 2 ) ) )
            {
                nearLowerBorder = false;
            }
            else
            {
                nearLowerBorder = true;
            }
        }
    }
    SunburstCursorData result( level, currentIndex );
    result.setNearLowerBorder( nearLowerBorder );
    result.setButtonTouched( buttonSelected( shapeData, transformationData, diff_norm ) );
    return result;
}


// local forward declarations for detail::resizeArc
void
algorithmResizePieces( QList<qreal>& pieces,
                       qreal         targetSize,
                       qreal         minSize );

void
resizeFullRing( SunburstShapeData& shapeData,
                int                level,
                int                index,
                qreal              newAngle,
                bool               clockwise );

void
resizeWithinParent( SunburstShapeData& shapeData,
                    int                level,
                    int                index,
                    qreal              newAngle,
                    bool               clockwise );

void
detail::resizeArc( SunburstShapeData&  shapeData,
                   SunburstCursorData& cursorData,
                   qreal               newAngle )
{
    // check whether the parent is the full ring which leads to another resizing behaviour (ignoring parent angle bounds)
    const bool parentIsRing = shapeData.getNumberOfElements( cursorData.level() - 1 ) == 1;

    if ( parentIsRing )
    {
        resizeFullRing( shapeData,
                        cursorData.level(),
                        cursorData.index(),
                        newAngle,
                        cursorData.getNearLowerBorder() );
    }
    else
    {
        resizeWithinParent( shapeData,
                            cursorData.level(),
                            cursorData.index(),
                            newAngle,
                            cursorData.getNearLowerBorder() );
    }
}


void
resizeFullRing( SunburstShapeData& shapeData,
                int                level,
                int                index,
                qreal              newAngle,
                bool               clockwise )
{
    // often used
    const int numberOfElements = shapeData.getNumberOfElements( level );

    // -- FIRST STEP: collect the sizes of the siblings
    QList<qreal> itemsToResize;

    // clockwise or counter-clockwise iterate through all siblings and append their sizes in itemsToResize
    if ( clockwise )
    {
        for ( int i = 0; i < numberOfElements - 1; i++ )
        {
            // definde relDegree for the current item
            qreal relCurrent =  shapeData.getRelDegree( level, ( index - 1 - i + numberOfElements ) % numberOfElements );
            qreal relSucc    =     shapeData.getRelDegree( level, ( index - i + numberOfElements ) % numberOfElements );
            if ( relSucc == 0.0 )
            {
                relSucc = 1.0;
            }

            // append size of the current item to the list
            itemsToResize.append( relSucc - relCurrent );
        }
    }
    else
    {
        for ( int i = 0; i < numberOfElements - 1; i++ )
        {
            // definde relDegree for the current item
            qreal relCurrent =  shapeData.getRelDegree( level, ( index + 1 + i ) % numberOfElements );
            qreal relSucc    =     shapeData.getRelDegree( level, ( index + 2 + i ) % numberOfElements );
            if ( relSucc == 0.0 )
            {
                relSucc = 1.0;
            }

            // append size of the current item to the list
            itemsToResize.append( relSucc - relCurrent );
        }
    }

    if ( itemsToResize.count() == 0 )
    {
        // there is nothing to do
        return;
    }
    // -- END FIRST STEP

    // -- SECOND STEP: calculate the relDegree of newAngle
    // the calculation depends on the space affected by the resize interaction defined by the 2 borders
    // the 2 borders are the lower and upper angle of the current item but assigned inverted (we resize everything else)
    const qreal lowerBorder = shapeData.getSuccAbsDegree( level, index );
    const qreal upperBorder = shapeData.getAbsDegree( level, index );

    // calculate the relDegree of newAngle
    qreal relNewAngle;
    if ( clockwise )
    {
        relNewAngle = fmod( 360.0 + newAngle - lowerBorder, 360 ) / 360.0;
    }
    else
    {
        relNewAngle = fmod( 360.0 + newAngle - upperBorder, 360 ) / 360.0;
    }
    // -- END SECOND STEP

    // -- THIRD STEP: calculate all necessary inputs for the algorithmResizePieces and apply it to itemsToResize:
    // determine the remaining space for the resizing siblings
    qreal newCombinatedSiblingSize;
    if ( clockwise )
    {
        newCombinatedSiblingSize = relNewAngle;
    }
    else
    {
        newCombinatedSiblingSize = 1.0 - relNewAngle;
    }

    // calculate the relDegree size of each item when evenly distributed
    const int   indexParentItem = shapeData.getParentIndex( level, index );
    const qreal evenSingleSize  = 1.0 / shapeData.getNumberOfChildren( level - 1, indexParentItem );

    // calculate the minimal relDegree size
    const qreal minSingleSize = evenSingleSize / shapeData.getMaxSizeDivisor();

    // resize all items from the resized element to the parent items border using the algorithmResizePieces
    algorithmResizePieces( itemsToResize, newCombinatedSiblingSize, minSingleSize );
    // -- END THIRD STEP

    // -- FOURTH STEP: update relDegree values using the updated sizes in itemsToResize
    if ( clockwise )
    {
        qreal lb = lowerBorder;
        if ( lb == 0.0 )
        {
            lb = 360.0;
        }
        qreal sizeSum = fmod( relNewAngle + lb / 360.0, 1 );
        for ( int i = 0; i < itemsToResize.count(); i++ )
        {
            shapeData.setRelDegree( level, ( index - i + numberOfElements ) % numberOfElements, sizeSum );
            sizeSum -= itemsToResize.at( i );
            if ( sizeSum < 0 )
            {
                sizeSum += 1;
            }
        }
    }
    else
    {
        qreal sizeSum = fmod( relNewAngle + upperBorder / 360.0, 1 );
        for ( int i = 0; i < itemsToResize.count(); i++ )
        {
            shapeData.setRelDegree( level, ( index + i + 1 ) % numberOfElements, sizeSum );
            sizeSum += itemsToResize.at( i );
            if ( sizeSum > 1 )
            {
                sizeSum -= 1;
            }
        }
    }
    /* The arc with index 0 might have moved which breaks the convention that this arc should have relDegree 0.0.
     * We solve this by having an offset storing the relDegree it has at this stage and subtracting this offset from
     * every relDegree to restore the convention. Further calls to this function take the offset into account by modifying
     * the input angle stored in shapeData.absDegreeOffset. */

    qreal zeroArcOffset = shapeData.getRelDegree( level, 0 );
    for ( int i = 0; i < numberOfElements; i++ )
    {
        shapeData.setRelDegree( level, i, fmod( shapeData.getRelDegree( level, i ) - zeroArcOffset + 1, 1 ) );
    }
    shapeData.setAbsDegreeOffset( fmod( shapeData.getAbsDegreeOffset() + zeroArcOffset * 360, 360 ) );
    // -- END FOURTH STEP

    // FINAL STEP: update absDegrees
    shapeData.calculateAbsDegrees();
}


void
resizeWithinParent( SunburstShapeData& shapeData,
                    int                level,
                    int                index,
                    qreal              newAngle,
                    bool               clockwise )
{
    // often used
    const int numberOfElements = shapeData.getNumberOfElements( level );
    // -- END DECLARATIONS

    // -- FIRST STEP: collect the sizes of the siblings that also resize
    QList<qreal> itemsToResize;

    if ( clockwise && shapeData.getRelDegree( level, index ) != 0.0 )
    {
        /* clockwise iterate through the shrinking items until the next item with lowest
         * relDegree ( = 0.0, the sibling with the lowest angle) is detected */
        int   indexCurrent = index;
        qreal relCurrent, relSucc;
        do
        {
            // definde relDegree for the current item
            relCurrent = shapeData.getRelDegree( level, indexCurrent - 1 );
            relSucc    = shapeData.getRelDegree( level, indexCurrent );

            // append size of the current item to the list
            itemsToResize.append( relSucc - relCurrent );

            // continue with the clockwise next item
            indexCurrent--;

            // break when we are done with the last sibling that has relCurrent 0.0
        }
        while ( relCurrent != 0.0 );
    }
    else
    if ( !clockwise && shapeData.getRelDegree( level, ( index + 1 ) % numberOfElements ) != 0.0 )
    {
        /* counter-clockwise iterate through the level and count items until the next item with highest
         * relDegree (the sibling with the highest angle) is detected
         * implemented by searching for the next item that has a successor with relDegree 0.0 */
        int   indexCurrent = index + 1;
        qreal relCurrent, relSucc;
        do
        {
            // definde relDegree for the current item
            relCurrent = shapeData.getRelDegree( level, indexCurrent );
            relSucc    = shapeData.getRelDegree( level, ( indexCurrent + 1 ) % numberOfElements );
            if ( relSucc == 0.0 )
            {
                relSucc = 1.0;
            }

            // append size of the current item to the list
            itemsToResize.append( relSucc - relCurrent );

            // continue with the counter-clockwise next item
            indexCurrent++;

            // break when we are done with the last sibling that has relSucc 1.0
        }
        while ( relSucc != 1.0 );
    }

    if ( itemsToResize.count() == 0 )
    {
        // there is nothing to do
        return;
    }
    // -- END FIRST STEP

    // -- SECOND STEP: calculate the relDegree of newAngle
    // the calculation depends on the space affected by the resize interaction defined by the 2 borders
    // the 2 borders are the lower and upper angle of the parent item
    const int   indexParentItem = shapeData.getParentIndex( level, index );
    const qreal lowerBorder     = shapeData.getAbsDegree( level - 1, indexParentItem );
    const qreal upperBorder     = shapeData.getSuccAbsDegree( level - 1, indexParentItem );

    // calculate the relDegree of newAngle
    const qreal relNewAngle = ( newAngle - lowerBorder ) / ( upperBorder - lowerBorder );
    // -- END SECOND STEP

    // -- THIRD STEP: calculate all necessary inputs for the algorithmResizePieces and apply it to itemsToResize:
    // determine the remaining space for the resizing siblings
    qreal newCombinatedSiblingSize;
    if ( clockwise )
    {
        newCombinatedSiblingSize = relNewAngle;
    }
    else
    {
        newCombinatedSiblingSize = 1.0 - relNewAngle;
    }

    // calculate the relDegree size of each item when evenly distributed
    const qreal evenSingleSize = 1.0 / shapeData.getNumberOfChildren( level - 1, indexParentItem );

    // calculate the minimal relDegree size
    const qreal minSingleSize = evenSingleSize / shapeData.getMaxSizeDivisor();

    // resize all items from the resized element to the parent items border using the algorithmResizePieces
    algorithmResizePieces( itemsToResize, newCombinatedSiblingSize, minSingleSize );
    // -- END THIRD STEP

    // -- FOURTH STEP: update relDegree values using the updated sizes in itemsToResize
    /* One does not simply add the values from itemsToResize to get the final relDegreeData because there might
     * be a lot of elements with minimal size that cause the addition/ subtraction of the element to be inexact.
     * Thus, we have to make sure that any difference of the overall sizeSum (sizeSum in the last iteration) is
     * identical to the newCombinatedSiblingSize and the rounding error eliminated by dividing the error factor. */
    qreal overallSizeSum = 0;
    for ( int i = 0; i < itemsToResize.count(); i++ )
    {
        overallSizeSum += itemsToResize.at( i );
    }
    qreal errorFactor = overallSizeSum / newCombinatedSiblingSize;
    if ( clockwise )
    {
        qreal sizeSum = relNewAngle;
        for ( int i = 0; i < itemsToResize.count(); i++ )
        {
            shapeData.setRelDegree( level, index - i, sizeSum );
            sizeSum -= itemsToResize.at( i ) / errorFactor;
        }
        assert( sizeSum / errorFactor <= newCombinatedSiblingSize );
    }
    else
    {
        qreal sizeSum = relNewAngle;
        for ( int i = 0; i < itemsToResize.count(); i++ )
        {
            shapeData.setRelDegree( level, index + i + 1, sizeSum );
            sizeSum += itemsToResize.at( i ) / errorFactor;
        }
        assert( sizeSum / errorFactor >= newCombinatedSiblingSize );
    }
    // -- END FOURTH STEP

    // FINAL STEP: update absDegrees
    shapeData.calculateAbsDegrees();
}


// local forward declarations for detail::checkAngle
int
checkForFullRing( SunburstShapeData& shapeData,
                  int                level,
                  int                index,
                  qreal              newAngle,
                  bool               clockwise );

int
checkForWithinParent( SunburstShapeData& shapeData,
                      int                level,
                      int                index,
                      qreal              newAngle,
                      bool               clockwise );


int
detail::checkAngle( SunburstShapeData&  shapeData,
                    SunburstCursorData& cursorData,
                    qreal               newAngle )
{
    if ( cursorData.level() == 0 )
    {
        // currently we do not support multiple items at level 0 so there is nothing to do if we want to resize this 1 item (its always a circle)
        // TODO: has to be implemented seperately if we choose to support multiple items at level 0 because we would not have to regard parent borders
        return 0;
    }

    // check whether the parent is the full ring which leads to another resizing behaviour (ignoring parent angle bounds)
    const bool parentIsRing = shapeData.getNumberOfElements( cursorData.level() - 1 ) == 1;

    if ( parentIsRing )
    {
        return checkForFullRing( shapeData,
                                 cursorData.level(),
                                 cursorData.index(),
                                 newAngle,
                                 cursorData.getNearLowerBorder() );
    }
    else
    {
        return checkForWithinParent( shapeData,
                                     cursorData.level(),
                                     cursorData.index(),
                                     newAngle,
                                     cursorData.getNearLowerBorder() );
    }
}


int
checkForFullRing( SunburstShapeData& shapeData,
                  int                level,
                  int                index,
                  qreal              newAngle,
                  bool               clockwise )
{
    assert( level > 0 ); // assumption for the following code

    // often used
    const int numberOfElements = shapeData.getNumberOfElements( level );

    // -- FIRST STEP: count the siblings that also resize
    int const itemsToResize = numberOfElements - 1;

    if ( itemsToResize == 0 )
    {
        // resizing not possible: return 1 or 2 to indicate this
        return 1;
    }
    // -- END FIRST STEP

    // -- SECOND STEP: calculate the minimal size of an item
    // the calculation depends on the space affected by the resize interaction defined by the 2 borders
    // the 2 borders are the lower and upper angle of the current item but assigned inverted (we resize everything else)
    const qreal lowerBorder = fmod( shapeData.getSuccAbsDegree( level, index ), 360 );
    const qreal upperBorder = shapeData.getAbsDegree( level, index );
    /* NOTE: lowerBorder, upperBorder and newAngle are shifted by shapeData.absDegreeOffset relative to the visualization.
     * This is due to the fact that the inner representation of the first element is fixed to 0 degree and then shifted by
     * shapeData.absDegreeOffset for the visualization. Since we get shifted input data and the shift does not change their
     * position relatively to one another we work with the shifted data here. In case you want to output the 3 angles you
     * should add shapeData.absDegreeOffset to all of them to be able to interpret them correctly. */

    // calculate the size of each item when evenly distributed
    const qreal evenSingleSize = 360.0 / numberOfElements;

    // calculate the minimal size
    const qreal minSingleSize            = evenSingleSize / shapeData.getMaxSizeDivisor();
    const qreal minCombinatedSiblingSize = itemsToResize * minSingleSize;
    // -- END SECOND STEP

    // check whether there is enough space for all items from the resized element to the parent items border regarding the minimal size for each item
    if ( clockwise )
    {
        // the resized item would require so much space that its siblings would violate the minimal size threshold:
        if ( fmod( 360 + newAngle - lowerBorder, 360 ) < minCombinatedSiblingSize )
        {
            return 1;
        }

        // the resized item would become too small:
        if ( fmod( 360 + lowerBorder - newAngle, 360 ) < minSingleSize )
        {
            return 2;
        }
    }
    else
    {
        // the resized item would require so much space that its siblings would violate the minimal size threshold:
        if ( fmod( 360 + upperBorder - newAngle, 360 ) < minCombinatedSiblingSize )
        {
            return 1;
        }

        // the resized item would become too small:
        if ( fmod( 360 + newAngle - upperBorder, 360 ) < minSingleSize )
        {
            return 2;
        }
    }

    return 0;
}


int
checkForWithinParent( SunburstShapeData& shapeData,
                      int                level,
                      int                index,
                      qreal              newAngle,
                      bool               clockwise )
{
    assert( level > 0 ); // assumption for the following code

    // often used
    const int numberOfElements = shapeData.getNumberOfElements( level );

    // -- FIRST STEP: count the siblings that also resize
    int itemsToResize = 0;
    if ( clockwise )
    {
        // clockwise iterate through the shrinking items until the next item with lowest relDegree ( = 0.0, the sibling with the lowest angle) is detected
        int indexCurrent = index;
        while ( shapeData.getRelDegree( level, indexCurrent ) != 0.0 )
        {
            // increase count
            itemsToResize++;

            // update index
            indexCurrent--;

            assert( indexCurrent >= 0 ); // holds: item with index 0 has relDegree == 0.0
        }
    }
    else
    {
        // counter-clockwise iterate through the level and count items until the next item with highest relDegree (the sibling with the highest angle) is detected
        // implemented by searching for the next item that has a successor with relDegree 0.0
        int indexCurrent = index;
        while ( shapeData.getRelDegree( level, indexCurrent + 1 ) != 0.0 )
        {
            // increase count
            itemsToResize++;

            // update index
            indexCurrent++;

            assert( indexCurrent <= numberOfElements ); // holds: successor of item with index numberOfElements-1 (= item with index 0) has relDegree == 0.0
        }
    }

    if ( itemsToResize == 0 )
    {
        // resizing not possible: return 1 or 2 to indicate this
        return 1;
    }
    // -- END FIRST STEP

    // -- SECOND STEP: calculate the minimal size of an item which is a fix fraction of the parent items size:
    const int indexParentItem = shapeData.getParentIndex( level, index );

    // get the lower border of the parent
    const qreal parentLowerBorder = shapeData.getAbsDegree( level - 1, indexParentItem );

    // get the upper border of the parent
    const qreal parentUpperBorder = shapeData.getSuccAbsDegree( level - 1, indexParentItem );

    // calculate the size of each item when evenly distributed
    const qreal evenSingleSize = ( parentUpperBorder - parentLowerBorder ) / shapeData.getNumberOfChildren( level - 1, indexParentItem );
    ;

    // calculate the minimal size
    const qreal minSingleSize            = evenSingleSize / shapeData.getMaxSizeDivisor();
    const qreal minCombinatedSiblingSize = itemsToResize * minSingleSize;
    // -- END SECOND STEP

    // check whether there is enough space for all items from the resized element to the parent items border regarding the minimal size for each item
    if ( clockwise )
    {
        // check whether newAngle is even outside of the region between the resized item and the parent items border in the direction to resize
        if ( parentLowerBorder >= newAngle )
        {
            // can return 1 or 2 here, since the region where this case applies is 'adjacent' to both regions where we get 1 or 2
            return 1;
        }

        // the resized item would require so much space that its siblings would violate the minimal size threshold:
        if ( newAngle - parentLowerBorder < minCombinatedSiblingSize )
        {
            return 1;
        }

        const qreal itemBorder = shapeData.getSuccAbsDegree( level, index );
        // the resized item would become too small:
        if ( itemBorder - newAngle < minSingleSize )
        {
            return 2;
        }
    }
    else
    {
        // check whether newAngle is even outside of the region between the resized item and the parent items border in the direction to resize
        if ( parentUpperBorder <= newAngle )
        {
            // can return 1 or 2 here, since the region where this case applies is 'adjacent' to both regions where we get 1 or 2
            return 1;
        }

        // the resized item would require so much space that its siblings would violate the minimal size threshold:
        if ( parentUpperBorder - newAngle < minCombinatedSiblingSize )
        {
            return 1;
        }

        const qreal itemBorder = shapeData.getAbsDegree( level, index );
        // the resized item would become too small:
        if ( newAngle - itemBorder < minSingleSize )
        {
            return 2;
        }
    }

    return 0;
}


/* An algorithm used by resizeLevel.
 * Takes a list of pieces each with a size and tries to resize them evenly to maintain their relative size.
 * If the size of a piece falls beneath the given minimal size, it has the minimal size.
 * Accordingly, to reach the given size, the other pieces shrink more than pieces reaching the minimal size.
 * The given size to reach is expected to be greater than the number of pieces times the minimal piece size. */
void
algorithmResizePieces( QList<qreal>& pieces,
                       qreal         targetSize,
                       qreal         minSize )
{
    /* Steps of the algorithm:
     * 1. select the smallest arc to resize
     * 2. if it would not fall beneath the minimal size when resizing the arcs, we just do it and are done. if not go to 3
     * 3. set the selected arcs size to the minimal size and resize the other arcs accordingly to preserve their relative size to the selected arc.
     * 4. we continue with 1 disregarding arcs with minimal size and using the size still to go (since we already decreased arcs and won space) */

    // implementation of the algorithm:

    const int count = pieces.count();

    // nothing to do for an empty list
    if ( count == 0 )
    {
        return;
    }

    // initializations:
    // true if item at index was resized to minimal size in step 3 (used in step 1 to skip the item)
    bool setToMin[ count ];
    for ( int i = 0; i < count; i++ )
    {
        setToMin[ i ] = false;
    }
    // the current size of the other resizing arcs together, also needed for both cases of shrinkElem
    qreal sizeSum = 0;
    for ( int i = 0; i < count; i++ )
    {
        sizeSum += pieces.at( i );
    }

    if ( targetSize > sizeSum )
    {
        // no need to regard the minimal size, since we only increase the size of the pieces
        qreal resizeFactor = targetSize / sizeSum;
        for ( int i = 0; i < count; i++ )
        {
            pieces.replace( i, pieces.at( i ) * resizeFactor );
        }
        // ... and we are done
        return;
    }

    // needed to have a reliable break condition
    qreal roundedTarget, roundedSum;

    // loop for the steps of the algorithm:
    do
    {
        // implemention of 1:
        int   smallest     = -1;
        qreal smallestSize = sizeSum;
        for ( int i = 0; i < count; i++ )
        {
            if ( setToMin[ i ] )
            {
                // was already resized to min by step 3
                continue;
            }
            if ( pieces.at( i ) <= smallestSize )
            {
                smallestSize = pieces.at( i );
                smallest     = i;
            }
        }
        if ( smallest == -1 )
        {
            break;
        }

        // implementation of 2 and 3:
        qreal resizeFactor = targetSize / sizeSum;
        if ( smallestSize * resizeFactor < minSize )
        {
            resizeFactor = minSize / smallestSize;
            pieces.replace( smallest, minSize );
            setToMin[ smallest ] = true;
        }
        for ( int i = 0; i < count; i++ )
        {
            if ( !setToMin[ i ] )
            {
                pieces.replace( i, pieces.at( i ) * resizeFactor );
            }
        }

        // update sizeSum
        sizeSum = 0;
        for ( int i = 0; i < count; i++ )
        {
            sizeSum += pieces.at( i );
        }

        // implementation of 4
        // round to 2 after-0-digits
        roundedTarget = static_cast<qreal>( static_cast<int>( targetSize * 100 + 0.5 ) ) / 100.0;
        roundedSum    = static_cast<qreal>( static_cast<int>( sizeSum * 100 + 0.5 ) ) / 100.0;
    }
    while ( roundedTarget != roundedSum );
}

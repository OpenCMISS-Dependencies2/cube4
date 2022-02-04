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


/*-------------------------------------------------------------------------*/
/**
 *  @file    SunburstShapeData.cpp
 *  @ingroup CubeSunburst
 *  @brief   Definition of the class SunburstShapeData.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "SunburstShapeData.h"

// general includes
#include <cassert>

// introduce Cube namespaces
using namespace cubegui;
using namespace cubepluginapi;

using namespace cube_sunburst;

const qreal SunburstShapeData::maxSizeDivisor           = 4.;
const qreal SunburstShapeData::completeLevelSizeDivisor = 0.5;
const qreal SunburstShapeData::levelGapRelativeSize     = 0.05;
const qreal SunburstShapeData::expandButtonWidth        = 0.2;
const qreal SunburstShapeData::collapseButtonWidth      = 0.6;



void
DegreeData::setDegree( int   level,
                       int   index,
                       qreal value )
{
    if ( itemExists( level, index ) )
    {
        degrees[ level ].replace( index, value );
    }
}


qreal
DegreeData::getDegree( int level,
                       int index )
{
    if ( itemExists( level, index ) )
    {
        return degrees.at( level ).at( index );
    }
    else
    {
        return 0;
    }
}


void
DegreeData::resize( int                     numberOfLevels,
                    const QVector< qreal >& itemsPerLevel )
{
    degrees.resize( numberOfLevels );
    for ( int i = 0; i < numberOfLevels; i++ )
    {
        degrees[ i ].fill( 0, itemsPerLevel[ i ] );
    }
}


qreal
DegreeData::count()
{
    return degrees.count();
}


qreal
DegreeData::count( int level )
{
    if ( level >= 0 && level < degrees.count() )
    {
        return degrees.at( level ).count();
    }
    else
    {
        return degrees.at( level ).count();
        return 0;
    }
}


bool
DegreeData::itemExists( int level,
                        int index )
{
    if ( level >= 0 && level < degrees.count() && index >= 0 && index < degrees[ level ].count() )
    {
        return true;
    }
    else
    {
        return false;
    }
}


void
SunburstShapeData::reset( int                     numberOfLevels,
                          const QVector< qreal >& itemsPerLevel )
{
    if ( numberOfLevels <= 0 )
    {
        return;
    }
    innerRadia.fill( 0, numberOfLevels );
    outerRadia.fill( 0, numberOfLevels );
    relDegrees.resize( numberOfLevels, itemsPerLevel );
    absDegrees.resize( numberOfLevels, itemsPerLevel );
    // we need 1 level less for expanded since the outer level cannot be expanded
    expanded.resize( numberOfLevels - 1 );
    for ( int i = 0; i < numberOfLevels - 1; i++ )
    {
        expanded[ i ].fill( false, itemsPerLevel[ i ] );
    }
    // we need 1 level less for visible since the inner level is always visible
    visible.resize( numberOfLevels - 1 );
    for ( int i = 0; i < numberOfLevels - 1; i++ )
    {
        visible[ i ].resize( itemsPerLevel[ i + 1 ] );
        visible[ i ].assign( itemsPerLevel[ i + 1 ], 0 );
    }

    // reset degree values
    resetDegrees();

    // reset expanded and visible values
    resetVisibilityData();

    // update level size values
    updateLevelSizes();

    service      = NULL;
    topLevelItem = NULL;
}


void
SunburstShapeData::resetDegrees()
{
    // we currently assume that the first level always has only 1 item
    relDegrees.setDegree( 0, 0, 0 );
    absDegrees.setDegree( 0, 0, 0 );
    absDegreeOffset = 0;
    for ( int i = 1; i < getNumberOfLevels(); i++ )
    {
        for ( int j = 0; j < getNumberOfElements( i ); j++ )
        {
            qreal fraction = ( qreal )j / getNumberOfElements( i ) * getNumberOfElements( i - 1 );
            // get values x with 0 <= x < 1
            fraction -= ( int )fraction;
            relDegrees.setDegree( i, j, fraction );
        }
    }
    calculateAbsDegrees();
}


void
SunburstShapeData::resetVisibilityData()
{
    // we currently assume that the first level always has only 1 item

    // reset expanded
    setExpanded( 0, 0, false );
    for ( int i = 1; i < getNumberOfLevels() - 1; i++ )
    {
        for ( int j = 0; j < getNumberOfElements( i ); j++ )
        {
            expanded[ i ].replace( j, false );
        }
    }

    // reset visible
    for ( int i = 1; i < getNumberOfLevels(); i++ )
    {
        for ( int j = 0; j < getNumberOfElements( i ); j++ )
        {
            visible[ i - 1 ][ j ] =  0;
        }
    }

    // update level size values
    updateLevelSizes();
}


int
SunburstShapeData::getLevel( qreal radius )
{
    for ( int i = 0; i < getNumberOfLevels(); i++ )
    {
        if (  ( radius >= getInnerRadius( i ) )
              && ( radius <= getOuterRadius( i ) ) )
        {
            return i;
        }
    }
    return -1;
}


int
SunburstShapeData::getNumberOfLevels()
{
    return relDegrees.count();
}


int
SunburstShapeData::getNumberOfElements( int level )
{
    if ( level >= 0 && level < getNumberOfLevels() )
    {
        return relDegrees.count( level );
    }
    else
    {
        return -1;
    }
}


void
SunburstShapeData::setInnerRadius( int   index,
                                   qreal value )
{
    if ( index >= 0 && index < innerRadia.count() )
    {
        innerRadia.replace( index, value );
    }
}


qreal
SunburstShapeData::getInnerRadius( int index )
{
    return innerRadia.value( index );
}


void
SunburstShapeData::setOuterRadius( int   index,
                                   qreal value )
{
    if ( index >= 0 && index < outerRadia.count() )
    {
        outerRadia.replace( index, value );
    }
}


qreal
SunburstShapeData::getOuterRadius( int index )
{
    return outerRadia.value( index );
}


int
SunburstShapeData::numberOfVisibleLevels()
{
    int result = 0;
    for ( int i = 0; i < getNumberOfLevels(); i++ )
    {
        bool levelVisible = false;
        for ( int j = 0; j < getNumberOfElements( i ) && !levelVisible; j++ )
        {
            levelVisible = itemIsVisible( i, j );
        }
        if ( levelVisible )
        {
            result++;
        }
        else
        {
            break;
        }
    }
    return result;
}


int
SunburstShapeData::numberOfCompleteLevels()
{
    int result = 0;
    for ( int i = 0; i < getNumberOfLevels(); i++ )
    {
        bool levelComplete = true;
        for ( int j = 0; j < getNumberOfElements( i ) && levelComplete; j++ )
        {
            levelComplete = levelComplete && itemIsVisible( i, j );
        }
        if ( levelComplete )
        {
            result++;
        }
        else
        {
            break;
        }
    }
    return result;
}


void
SunburstShapeData::setService( PluginServices* service )
{
    this->service = service;
}


PluginServices*
SunburstShapeData::getService()
{
    return service;
}


void
SunburstShapeData::setTopLevelItem( TreeItem* item )
{
    topLevelItem = item;
}


TreeItem*
SunburstShapeData::getTopLevelItem()
{
    return topLevelItem;
}


bool
SunburstShapeData::isValid()
{
    return topLevelItem != NULL && service != NULL;
}


int
SunburstShapeData::getParentIndex( int level, int index )
{
    assert( level >= 0 );
    int currentParentIndex = 0;
    for ( int i = 0; i <= index; i++ )
    {
        if ( relDegrees.getDegree( level, i ) == 0.0 )
        {
            currentParentIndex++;
        }
    }
    return currentParentIndex - 1;
}


QPoint
SunburstShapeData::getRangeOfChildren( int level, int index )
{
    assert( level >= 0 );
    int    currentParentIndex = -1;
    QPoint result( -1, -1 );
    for ( int i = 0; i < getNumberOfElements( level + 1 ) && currentParentIndex <= index; i++ )
    {
        if ( relDegrees.getDegree( level + 1, i ) == 0.0 )
        {
            currentParentIndex++;
        }
        if ( currentParentIndex == index )
        {
            if ( result.x() == -1 )
            {
                result.setX( i );
            }
            result.setY( i );
        }
    }
    return result;
}


int
SunburstShapeData::getNumberOfChildren( int level, int index )
{
    assert( level >= 0 );
    int currentParentIndex = -1;
    int elementCount       = 0;
    for ( int i = 0; i < getNumberOfElements( level + 1 ) && currentParentIndex <= index; i++ )
    {
        if ( relDegrees.getDegree( level + 1, i ) == 0.0 )
        {
            currentParentIndex++;
        }
        if ( currentParentIndex == index )
        {
            elementCount++;
        }
    }
    return elementCount;
}


qreal
SunburstShapeData::getAbsDegree( int level,
                                 int index )
{
    return absDegrees.getDegree( level, index );
}


qreal
SunburstShapeData::getSuccAbsDegree( int level,
                                     int index )
{
    if ( index == absDegrees.count( level ) - 1 )
    {
        return 360.0;
    }
    return absDegrees.getDegree( level, index + 1 );
}


qreal
SunburstShapeData::getRelDegree( int level,
                                 int index )
{
    return relDegrees.getDegree( level, index );
}


void
SunburstShapeData::setRelDegree( int   level,
                                 int   index,
                                 qreal value )
{
    assert( value >= 0 && value <= 1 );
    relDegrees.setDegree( level, index, value );
}


void
SunburstShapeData::calculateAbsDegrees()
{
    qreal parentFirstDegree  = 0;
    qreal parentSecondDegree = 360;
    for ( int i = 0; i < getNumberOfLevels(); i++ )
    {
        int parentIndex = -1;
        for ( int j = 0; j < getNumberOfElements( i ); j++ )
        {
            if ( i > 0 && relDegrees.getDegree( i, j ) == 0.0 )
            {
                parentIndex++;
                parentFirstDegree = absDegrees.getDegree( i - 1, parentIndex );
                if ( parentIndex + 1 != getNumberOfElements( i - 1 ) )
                {
                    parentSecondDegree = absDegrees.getDegree( i - 1, parentIndex + 1 );
                }
                else
                {
                    parentSecondDegree = 360;
                }
            }
            qreal currentDegree = parentFirstDegree + relDegrees.getDegree( i, j ) * ( parentSecondDegree - parentFirstDegree );
            absDegrees.setDegree( i, j, currentDegree );
        }
    }
}

void
SunburstShapeData::setAbsDegreeOffset( qreal angle )
{
    assert( angle >= 0 && angle <= 360 );
    absDegreeOffset = angle;
}

qreal
SunburstShapeData::getAbsDegreeOffset()
{
    return absDegreeOffset;
}


bool
SunburstShapeData::itemExists( int level,
                               int index )
{
    if ( level >= 0
         && level < getNumberOfLevels()
         && index >= 0
         && index < getNumberOfElements( level ) )
    {
        return true;
    }
    return false;
}


bool
SunburstShapeData::itemExists( const QPoint& item )
{
    return itemExists( item.x(), item.y() );
}


bool
SunburstShapeData::itemIsVisible( int level, int index )
{
    if ( !itemExists( level, index ) )
    {
        return false;
    }
    if ( level == 0 )
    {
        return true;
    }
    return visible.at( level - 1 ).at( index );
}


bool
SunburstShapeData::itemIsVisible( const QPoint& item )
{
    return itemIsVisible( item.x(), item.y() );
}


bool
SunburstShapeData::getExpanded( int level, int index )
{
    if ( !itemExists( level, index ) || level == getNumberOfLevels() - 1 )
    {
        return false;
    }
    return expanded.at( level ).at( index );
}


bool
SunburstShapeData::getExpanded( const QPoint& item )
{
    return getExpanded( item.x(), item.y() );
}


void
SunburstShapeData::setExpanded( int level, int index, bool value )
{
    if ( !itemExists( level, index ) || ( level == getNumberOfLevels() - 1 ) )
    {
        return;
    }
    expanded[ level ].replace( index, value );
    // update derived attribute visible
    if ( value && ( level == 0 || ( visible.at( level - 1 ).at( index ) ) == 1 ) )
    {
        showDescendants( level, index );
    }
    else
    {
        hideDescendants( level, index );
    }
}


void
SunburstShapeData::setExpanded( const QPoint& item, bool value )
{
    setExpanded( item.x(), item.y(), value );
}


void
SunburstShapeData::updateLevelSizes()
{
    // we normalize the max size of the widget to 1
    qreal radius = 1 / 2.0;

    // calculate the width of level rings
    qreal reducedWidth     = radius / numberOfVisibleLevels() * completeLevelSizeDivisor;
    int   completeChildren = numberOfCompleteLevels() - 1;
    qreal normalWidth      = ( radius - completeChildren * reducedWidth ) / ( numberOfVisibleLevels() - completeChildren );

    // update the shapeData variable
    for ( int i = 0; i < numberOfVisibleLevels(); i++ )
    {
        // check which width to use for the current ring level
        qreal width = normalWidth;
        if ( i < completeChildren )
        {
            width = reducedWidth;
        }

        // update the radia variables of shapeData
        if ( i == 0 )
        {
            setOuterRadius( i, width );
        }
        else
        {
            setInnerRadius( i, getOuterRadius( i - 1 ) + width * levelGapRelativeSize );
            setOuterRadius( i, getOuterRadius( i - 1 ) + width );
        }
    }
}


qreal
SunburstShapeData::getMaxSizeDivisor()
{
    return maxSizeDivisor;
}


qreal
SunburstShapeData::getCollapseButtonWidth()
{
    return collapseButtonWidth;
}


qreal
SunburstShapeData::getExpandButtonWidth()
{
    return expandButtonWidth;
}


void
SunburstShapeData::hideDescendants( int level, int index )
{
    if ( level >= getNumberOfLevels() - 1 )
    {
        return;
    }
    QPoint childrenRange = getRangeOfChildren( level, index );
    for ( int i = childrenRange.x(); i <= childrenRange.y(); i++ )
    {
        // remember: the first level is not represented so we dont need to take level-1
        visible[ level ][ i ] = 0;
        hideDescendants( level + 1, i );
    }
}


void
SunburstShapeData::showDescendants( int level, int index )
{
    if ( level >= getNumberOfLevels() - 1 )
    {
        return;
    }
    QPoint childrenRange = getRangeOfChildren( level, index );
    for ( int i = childrenRange.x(); i <= childrenRange.y(); i++ )
    {
        // remember: the first level is not represented so we dont need to take level-1
        visible[ level ][ i ] = 1;
        if ( getExpanded( level + 1, i ) )
        {
            showDescendants( level + 1, i );
        }
    }
}

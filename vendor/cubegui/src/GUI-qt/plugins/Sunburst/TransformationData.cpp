/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2016-2017                                                **
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
 *  @file    TransformationData.cpp
 *  @ingroup CubeSunburst
 *  @brief   Definition of the class TransformationData.
 **/
/*-------------------------------------------------------------------------*/


#include <config.h>

#include "TransformationData.h"

// general includes
#include <cmath>

using namespace cube_sunburst;


const qreal   TransformationData::zoomBaseFactor    = 0.001;
const QPointF TransformationData::drawingShiftError = QPointF( 2.0, 2.0 );


void
TransformationData::initialize()
{
    boundingRect       = QRect();
    antialiasing       = true;
    zoomLevel          = 0;
    rotation           = 0;
    frameLineColor     = Qt::black;
    selectionLineColor = Qt::blue;
    frameSizeThreshold = 0;
}


void
TransformationData::setBoundingRect( const QRect& value )
{
    boundingRect = value;
}


QRect
TransformationData::getBoundingRect()
{
    return boundingRect;
}


void
TransformationData::setAntialiasing( bool value )
{
    antialiasing = value;
}


bool
TransformationData::antialiazed()
{
    return antialiasing;
}


void
TransformationData::setZoomLevel( int value )
{
    zoomLevel = value;
}


void
TransformationData::zoomIn()
{
    zoomLevel += 1;
}


void
TransformationData::zoomOut()
{
    zoomLevel -= 1;
}


qreal
TransformationData::getZoomFactor()
{
    return pow( zoomBaseFactor, zoomLevel / 100.0 );
}


void
TransformationData::setRotation( qreal value )
{
    while ( value < 0 )
    {
        value += 360;
    }
    rotation = fmod( value, 360 );
}


qreal
TransformationData::getRotation()
{
    return rotation;
}


void
TransformationData::setFrameLineColor( const QColor& value )
{
    frameLineColor = value;
}


QColor
TransformationData::getFrameLineColor()
{
    return frameLineColor;
}


void
TransformationData::setSelectionLineColor( const QColor& value )
{
    selectionLineColor = value;
}


QColor
TransformationData::getSelectionLineColor()
{
    return selectionLineColor;
}


void
TransformationData::setFrameSizeThreshold( int value )
{
    frameSizeThreshold = value;
}


int
TransformationData::getFrameSizeThreshold()
{
    return frameSizeThreshold;
}


QPointF
TransformationData::getDrawingShiftError()
{
    return drawingShiftError;
}

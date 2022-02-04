/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#include "config.h"

#include "SystemTopologyViewTransform.h"

SystemTopologyViewTransform::SystemTopologyViewTransform( SystemTopologyData* data )
{
    this->data          = data;
    focusEnabled        = true;
    absoluteScaleFactor = 1.;
    relativeScaleFactor = 1.;
    angleHasChanged     = false;
    distanceChanged     = false;
    planeDistance       = 10;
    currentPlane        = -1;
    fullPlaneDistance   = 0;
    distanceSum         = 0;

    if ( data->getUsedDimensions() > 2 )
    {
        xAngle = 300;
        yAngle = 30;
    }
    else
    {
        xAngle = 0;
        yAngle = 0;
    }

    resetPlaneDistances();
}

int
SystemTopologyViewTransform::getXAngle() const
{
    return xAngle;
}
int
SystemTopologyViewTransform::getYAngle() const
{
    return yAngle;
}

/**
 * creates a vector which contains the distances between the planes of the topology
 * -1 = show plany fully
 *  0 = default plane distance
 *  a value > 0 adds a percentual space to the default distance
 * @param currentPlane selected plane
 */
void
SystemTopologyViewTransform::initPlaneDistances( int currentPlane )
{
    int planes     = data->getDim( 2 );
    int distance[] = { -1, -1, 40, 30, 20, 10 };
    int distanceL  = sizeof( distance ) / sizeof( int );

    distribute.clear();
    distribute.push_back( 0 );
    for ( int i = 0; i < planes + 1; ++i )
    {
        int idx = abs( currentPlane - i );
        int val = ( idx < distanceL ) ? distance[ idx ] : 0;
        distribute.push_back( val ); // 0 = default plane distance, -1 = full plane distance
    }

    // calculate sum of all positive elements in distribute
    distanceSum = 0;
    for ( int i = 0; i < distanceL; ++i )
    {
        if ( distance[ i ] > 0 )
        {
            distanceSum += distance[ i ] * 2;
        }
    }
}

void
SystemTopologyViewTransform::addFullPlaneDistance( int direction )
{
    int position = ( direction == -1 ) ? currentPlane : currentPlane + 1;
    distribute.insert( distribute.begin() + position, -1 );
    if ( direction == -1 )
    {
        distribute.erase( distribute.begin() );
    }
}

/**
 * @brief SystemTopologyViewTransform::getPlaneDistance
 * @param planeIndex currently painted plane
 * @param isRising
 * @param direction get space above (-1) or below the plane (+1)
 * @return space beetween current and following plane (see direction)
 */
int
SystemTopologyViewTransform::getPlaneDistance( int planeIndex, bool isRising, int direction ) const
{
    int height = ( int )planeDistance;      // default plane distance
    // if no plane is active or plane distance is big enough => return same distance for all planes
    if ( currentPlane == -1  || planeIndex == -1 || ( planeDistance > fullPlaneDistance ) )
    {
        return height;
    }

    int add  =  isRising ? 1 : 0;
    int dir  =  direction == -1 ? 0 : 1;
    int dist = distribute[ planeIndex + dir + add ]; // plane distance for planeIndex to next plane in direction

    int distSpace = 3 * fullPlaneDistance;           // space to distribute between neighbors of current plane,
    int pad       = 5;

    if ( dist == -1 ) // show plane completely
    {
        height = fullPlaneDistance + pad;
    }
    else
    {
        int diff = distSpace * dist / distanceSum;
        height = planeDistance + diff;
    }
    if ( height > fullPlaneDistance )
    {
        height = fullPlaneDistance + pad;
    }
    return height;
}

/**
   rescales the drawing to fit into the widget and resets scrollbar position to top left
   @param resetValues if true (default), resets angles of topology and plane distance
 */
void
SystemTopologyViewTransform::rescale( bool resetValues )
{
    absoluteScaleFactor = 1.;

    // only reset the angle if desired
    if ( resetValues )
    {
        angleHasChanged = false;
        distanceChanged = false;

        if ( data->getUsedDimensions() > 2 )
        {
            setXAngle( 300 );
            setYAngle( 30 );
        }
        else
        {
            setXAngle( 0 );
            setYAngle( 0 );
        }
    }
    if ( !distanceChanged ) // user has not changed the plane distance
    {
        planeDistance = 0;  // recalculate plane distance
    }
    emit rescaleRequest();
}

//if the x/y angle has been changed internally, then we should
//additionally emit the corresponding signal;
//if it has been changed externally, then the signal is emitted
//by the x/y spin

void
SystemTopologyViewTransform::setXAngle( int angle )
{
    /* user controls (spin box) also send events, if value was changed by program.
       => don't emit signal, if value hasn't changed
     */
    if ( xAngle != angle )
    {
        while ( angle >= 360 )
        {
            angle -= 360;
        }
        while ( angle < 0 )
        {
            angle += 360;
        }
        xAngle          = angle;
        angleHasChanged = true;
        emit xAngleChanged( xAngle );
    }
}

void
SystemTopologyViewTransform::setYAngle( int angle )
{
    if ( yAngle != angle )
    {
        while ( angle >= 360 )
        {
            angle -= 360;
        }
        while ( angle < 0 )
        {
            angle += 360;
        }
        yAngle          = angle;
        angleHasChanged = true;
        emit yAngleChanged( yAngle );
    }
}

void
SystemTopologyViewTransform::increasePlaneDistance()
{
    planeDistance  += 1;
    distanceChanged = true;
    emit viewChanged(); //todo other signal
}

void
SystemTopologyViewTransform::decreasePlaneDistance()
{
    if ( planeDistance >= 2 )
    {
        planeDistance  -= 1;
        distanceChanged = true;
        emit viewChanged();
    }
}

void
SystemTopologyViewTransform::zoom( double factor )
{
    relativeScaleFactor  = factor;
    absoluteScaleFactor *= factor;
    planeDistance       *= factor;
}

void
SystemTopologyViewTransform::zoomIn()
{
    emit zoomChanged( 1.1 ); // notify view to check if zooming in is valid
}

void
SystemTopologyViewTransform::zoomOut()
{
    emit zoomChanged( 1. / 1.1 );
}

void
SystemTopologyViewTransform::moveDown()
{
    emit positionChanged( 0, 5 );
}

void
SystemTopologyViewTransform::moveUp()
{
    emit positionChanged( 0, -5 );
}

void
SystemTopologyViewTransform::moveRight()
{
    emit positionChanged( 5, 0 );
}

void
SystemTopologyViewTransform::moveLeft()
{
    emit positionChanged( -5, 0 );
}

void
SystemTopologyViewTransform::redraw()
{
    emit viewChanged();
}

void
SystemTopologyViewTransform::saveSettings( QSettings& settings, int topologyId )
{
    QString groupName( "systemTopologyViewTransform" );
    groupName.append( QString::number( topologyId ) );
    settings.beginGroup( groupName );

    settings.setValue( "planeDistance", planeDistance );
    settings.setValue( "xAngle", xAngle );
    settings.setValue( "yAngle", yAngle );

    settings.endGroup();
}

bool
SystemTopologyViewTransform::loadSettings( QSettings& settings, int topologyId )
{
    QString groupName( "systemTopologyViewTransform" );
    groupName.append( QString::number( topologyId ) );

    settings.beginGroup( groupName );

    planeDistance = settings.value( "planeDistance", 1 ).toInt();
    xAngle        = settings.value( "xAngle", 300 ).toInt();
    yAngle        = settings.value( "yAngle", 30 ).toInt();

    settings.endGroup();

    emit xAngleChanged( xAngle );
    emit yAngleChanged( yAngle );

    return true;
}

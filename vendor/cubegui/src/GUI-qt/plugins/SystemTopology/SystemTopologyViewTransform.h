/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef TOPOLOGYVIEWPARAMETERS_H
#define TOPOLOGYVIEWPARAMETERS_H

#include "SystemTopologyData.h"
#include <QtCore>

/**
 * @brief contains data and methods which transform the view, e.g. rotation, scaling
 */
class SystemTopologyViewTransform : public QObject
{
    Q_OBJECT
public slots:
    void
    increasePlaneDistance();
    void
    decreasePlaneDistance();
    void
    rescale( bool resetValues = true );
    void
    setXAngle( int angle );
    void
    setYAngle( int angle );
    void
    zoomIn();
    void
    zoomOut();
    void
    moveDown();
    void
    moveUp();
    void
    moveRight();
    void
    moveLeft();
    void
    redraw();

signals:
    void
    rescaleRequest();
    void
    viewChanged();
    void
    zoomChanged( double factor );
    void
    xAngleChanged( int );
    void
    yAngleChanged( int );
    void
    positionChanged( int,
                     int );

public:
    SystemTopologyViewTransform( SystemTopologyData* data );
    bool
    distanceHasChanged() const
    {
        return distanceChanged;
    }
    QSize
    getWidgetSize()
    {
        return size;
    }
    int
    getPlaneDistance( int  planeIndex,
                      bool isRising = true,
                      int  direction = 1 ) const;
    int
    getXAngle() const;
    int
    getYAngle() const;

    double
    getAbsoluteScaleFactor()
    {
        return absoluteScaleFactor;
    }
    double
    getRelativeScaleFactor()
    {
        return relativeScaleFactor;
    }
    void
    setPlaneDistance( int d )
    {
        planeDistance = d;
    }
    void
    zoom( double factor );
    void
    saveSettings( QSettings& settings,
                  int        topologyId );
    bool
    loadSettings( QSettings& settings,
                  int        topologyId );

    void
    setFullPlaneDistance( int h )
    {
        fullPlaneDistance = h;
    }
    void
    setCurrentPlane( int i )
    {
        currentPlane = i;
        initPlaneDistances( currentPlane );
    }
    int
    getCurrentPlane()
    {
        return currentPlane;
    }
    /*
     * param direction: -1 or 1 to insert below or above current plane
     */
    void
    addFullPlaneDistance( int direction );

    void
    resetPlaneDistances()
    {
        initPlaneDistances( currentPlane );
    }

    bool
    isFocusEnabled()
    {
        return focusEnabled;
    }

    void setFocusEnabled( bool focus )
    {
        focusEnabled = focus;
    }

private:
    void
    initPlaneDistances( int currentPlane );

    double           planeDistance;    // distance between planes above each other
    int              xAngle;           // current angle
    int              yAngle;
    bool             distanceChanged;
    bool             angleHasChanged;
    double           absoluteScaleFactor;    // scale=1.0 = fill the visible area
    double           relativeScaleFactor;
    QSize            size;                   // size of the visible part of the drawing
    int              currentPlane;           // index of plane below the mouse
    int              fullPlaneDistance;      // min. distance between two planes which are shown completely
    int              distanceSum;            // sum of all positive elements in distribute
    std::vector<int> distribute;             // contains distances between all planes
    bool             focusEnabled;

    SystemTopologyData* data;
};


#endif // TOPOLOGYVIEWPARAMETERS_H

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




#ifndef _PLANE_H
#define _PLANE_H

#include <QPainter>
#include <vector>
#include <algorithm>

#include "Point.h"
#include "Constants.h"
#include "SystemTopologyData.h"
#include "Tetragon.h"

#define XX 0
#define YY 1
#define ZZ 2

//instances of this class represent a plane of the 3D topology

class Plane
{
public:

    //constructor
    Plane();

    void
    init( int xAngle,
          int yAngle );

    void
    setData( const SystemTopologyData* data )
    {
        this->data = data;
    }

    //rotate the plane about the X-axis
    void
    xRotate( double angle );

    //rotate the plane about the Y-axis
    void
    yRotate( double angle );

    //rotate the plane about the Z-axis
    void
    zRotate( double angle );

    //scroll the plane along the X-axis
    void
    xScroll( double value );

    //scroll the plane long the Y-axis
    void
    yScroll( double value );

    void
    addMargin( QSize margin );
    void
    setMargin( QSize margin = QSize( 0, 0 ) );

    void
    scale( double factor );

    double
    scale( QSize maxSize,
           int   planeDistance,
           int   xAngle,
           int   yAngle );

    //in the "bool* visible" array we store which positions on the planes
    //would not be hidden by another plane above it;
    //only those have to be painted on non-top planes;
    //this method computes the values of this array;
    //parameters:
    //dim[]         are the original dimensions (size = 3)
    //dimOrder[]    defines the order of dimensions (size = 3),
    //              for example in XYZ-view it's elements are 0,1,2
    //              and in the YZX-view 1,2,0
    //planeDistance gives the current vertical distance between planes in 3D-topologies
    void
    computeVisibles( int planeDistance );


    void
    getSelectedInPlane( const QPoint& clickedPoint,
                        int&          xx,
                        int&          yy );
    void
    paint( unsigned  currentPlaneIndex,
           QPainter& painter,
           bool      isPlaneAbove );

    int
    baseline();

    //returns the width and height on one plane on screen
    QSize
    size() const;

    //returns the index-th element of the points array
    const Point*
    getPoint( unsigned index );

    //determines if the front of the plane is lower then the back part,
    //in this case the deepest plane will be painted first,
    //otherwise we start with the one on the top, because we see the block from the bottom
    bool
    isRising();

    //sets the line type;
    //LineType is defined in constants.h;
    //lineType stores if the lines between the system items are black, gray,
    //white or if no lines should be drawn
    void
    setLineType( cubepluginapi::LineType lineType );

    void
    setFoldingSeparator( const unsigned dims[] = 0 );

    QSize
    getMargin() const;

private:
    //moves plane to the most-top-most-left point
    void
    moveToTopLeftPoint();

    //scale the distance from the argument point (x,y,z)
    //to the plane's points by the parameter factor
    bool
    scale( double x,
           double y,
           double z,
           double factor );
    void
    getGeom( int& dx01,
             int& dy01,
             int& dx03,
             int& dy03 );

    Tetragon
    getTetragon( int dx01,
                 int dy01,
                 int dx03,
                 int dy03 );

    void
    adjustToScreen();

    //returns the calculated width and height
    QSizeF
    sizeF();

#ifdef unused
    //mirror the plane on the X-axis
    void
    xMirror();

    //mirror the plane on the Y-axis
    void
    yMirror();

    //mirror the plane on the Z-axis
    void
    zMirror();

    //scroll the plane along the Z-axis
    void
    zScroll( double value );

#endif

    //points[0]..points[3] are the corner points;
    //points[4] is the middle point;
    Point points[ 5 ];         // used for calculating
    Point pointsOnScreen[ 5 ]; // used for drawing, each subelement width is truncated to int

    //in the visible array we store which positions on the planes
    //would not be hidden by another plane above it;
    //only those have to be painted on non-top planes
    bool* visible;

    //LineType is defined in constants.h;
    //lineType stores if the lines between the system items are black, gray, white or no lines
    cubepluginapi::LineType   lineType;
    const SystemTopologyData* data;

    bool  markMerged;                     // draw rectangle around merged elements, if true
    int   foldingDimensions[ 3 ];
    QSize margin;
};

#endif

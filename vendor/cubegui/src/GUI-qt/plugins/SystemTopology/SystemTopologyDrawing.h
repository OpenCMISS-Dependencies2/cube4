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




#ifndef _SystemTopologyDrawing_H
#define _SystemTopologyDrawing_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QFrame>

#include <vector>
#include <math.h>
#include <iostream>
#include <map>

#include "Plane.h"
#include "Constants.h"

#include "Cube.h"
#include "SystemTopologyData.h"
#include "SystemTopologyView.h"
#include "SystemTopologyViewTransform.h"

class QSettings;
class InfoToolTip;
class SystemTopologyToolBar;

class SystemTopologyDrawing : public SystemTopologyView
{
    Q_OBJECT

public slots:
    /** recreates drawing if values have changed */
    void
    updateDrawing();
    void
    rescaleDrawing();
    void
    changeZoom( double factor );
    void
    setXAngle( int xangle );
    void
    setYAngle( int yangle );
    void
    move( int dx,
          int dy );

public:
    SystemTopologyDrawing( SystemTopologyData*          data,
                           SystemTopologyViewTransform* transform,
                           SystemTopologyToolBar*       bar );
    ~SystemTopologyDrawing();
    void
    setSize( const QSize& size );
    void
    scrolledTo( int x,
                int y );

    // returns the preferred size
    virtual QSize
    sizeHint() const;

protected:
    //main painting method
    void
    paintEvent( QPaintEvent* );

    //mouse and key event handling
    void
    mousePressEvent( QMouseEvent* event );
    void
    mouseMoveEvent( QMouseEvent* event );
    void
    mouseReleaseEvent( QMouseEvent* event );
    void
    wheelEvent( QWheelEvent* e );
    void
    keyPressEvent( QKeyEvent* event );
    void
    keyReleaseEvent( QKeyEvent* event );

private:
    void
    setInitialFocus();

    // main drawing method
    void
    draw();
    void
    drawOffScreen();
    void
    drawOnScreen();
    int
    getFullPlaneDistance();
    QSize
    getDrawingSize() const;
    void
    paintTopology( QPainter& painter,
                   bool      optimizeScreen = false );

    // get index of the plane which contains the given point
    int
    getSelectedPlane( const QPoint& clickedPoint );
    void
    getSelectedPosition( const QPoint& clickedPoint,
                         int&          xx,
                         int&          yy,
                         int&          zz );
    int
    coordinateToScreenY( int y,
                         int z );

    void
    showInfoTooltip( const QPoint& mouse );

    //some transformation functions
    void
    rotateTo( const QPoint& endPoint );
    void
    moveTo( const QPoint& endPoint );
    void
    setDims( unsigned dim1,
             unsigned dim2,
             unsigned dim3 );

    void
    scale( int planeDistance );

    // rescale the topology such that the topology fits into the the current visible scrollArea
    void
    rescale( bool resetPlaneDistance = false );

    // resets zoom factor, angles and plane distance
    void
    reset();

    //re-compute the colors
    void
    updateColors();

    //returns the maximum of topology drawing size and widget size
    QSize
    getMinimumPixmapSize() const;

    //methods for the topology tool bar
    void
    moveDown();
    void
    moveUp();
    void
    moveRight();
    void
    moveLeft();
    int
    getYPositionToScroll( int y,
                          int z );

    // ---------------------------------------------------

    //we store only one plane and shift it for paining if several planes are defined
    Plane plane;

    QSize parentSize; // visible part of the Drawing

    //for mouse and key event handling
    double                   widgetScaleFactor;  // factor to scale from minimum to widget size
    QPoint                   lastPoint, firstPoint;
    bool                     leftMousePressed;
    bool                     rightMousePressed;
    bool                     shiftPressed;
    bool                     controlPressed;
    bool                     isRotating;
    cubepluginapi::TreeItem* selectedItem;
    int                      topX;  // position to start drawing in the offscreen image, Position is (0,0),
    int                      topY;  // if image is smaller or equal to offscreen image

    QPixmap*     offscreen;         // used for double buffering
    InfoToolTip* info;              // info tooltip

    // interactive changes of the view: scaling, rotating, moving
    int prevXAngle;
    int prevYAngle;

    SystemTopologyViewTransform* transform;
    SystemTopologyData*          data;
    SystemTopologyToolBar*       topologyToolBar;
};

class InfoToolTip : public QFrame
{
    QString left;
    QString right;
    QSize   size;
public:
    InfoToolTip();
    void
    paintEvent( QPaintEvent* );
    void
    showInfo( const QPoint&      pos,
              const QStringList& tooltipText );
};

#endif

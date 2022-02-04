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



#include "config.h"

#include "Tetragon.h"
#include "SystemTopologyDrawing.h"
#include "SystemTopologyViewTransform.h"
#include "PrecisionWidget.h"

#include "CubeCartesian.h"
#include "CubeSysres.h"
#include "CubeThread.h"
#include "CubeProcess.h"
#include "CubeNode.h"
#include "PluginServices.h"
#include "SystemTopologyToolBar.h"

#include <cassert>
#include <qdebug.h>

using namespace cubepluginapi;

/*************** constructor / destructor *************************/

SystemTopologyDrawing::SystemTopologyDrawing( SystemTopologyData*          dat,
                                              SystemTopologyViewTransform* transform,
                                              SystemTopologyToolBar*       bar )
{
    topologyToolBar  = bar;
    this->data       = dat;
    this->transform  = transform;
    this->prevXAngle = transform->getXAngle();
    this->prevYAngle = transform->getYAngle();
    this->info       = new InfoToolTip();
    this->topX       = 0;
    this->topY       = 0;

    offscreen         = NULL;
    leftMousePressed  = false;
    rightMousePressed = false;
    shiftPressed      = false;
    controlPressed    = false;
    isRotating        = false;
    parentSize        = QSize();
    lastPoint         = QPoint();
    firstPoint        = QPoint();
    widgetScaleFactor = 1.;
    selectedItem      = 0;

    plane.setData( data );

    // to handle keyboard input
    // todo setFocusPolicy( Qt::StrongFocus );
    setFocusPolicy( Qt::WheelFocus );

    setMinimumSize( 1, 1 ); // ensure a minimal size of (1,1), if zero no paintEvent is emitted
}


SystemTopologyDrawing::~SystemTopologyDrawing()
{
    delete offscreen;
}

/** This method is called, if the window has been resized.
 *  If the drawing previously has fit into the window, it is rescaled to the new window size.
 */
void
SystemTopologyDrawing::setSize( const QSize& size )
{
    int   eps      = 50;
    QSize drawSize = getDrawingSize();
    bool  doesFit  = ( ( ( drawSize.width() - parentSize.width() ) < eps ) &&
                       ( ( drawSize.height() - parentSize.height() ) < eps ) );
    bool init = ( parentSize.width() <= 0 );
    parentSize = size;

    if ( doesFit || init )
    {
        transform->rescale( false );
    }

    setMinimumSize( getMinimumPixmapSize() ); // for the scrollbars
    setMaximumSize( getMinimumPixmapSize() );
}

void
SystemTopologyDrawing::updateDrawing()
{
    plane.setLineType( data->getLineType() );

    plane.init( transform->getXAngle(), transform->getYAngle() );
    plane.scale( widgetScaleFactor * transform->getAbsoluteScaleFactor() );

    draw();
}

void
SystemTopologyDrawing::rescaleDrawing()
{
    plane.setMargin( QSize( 0, 0 ) );
    rescale();
    draw();
}

/******************** angle settings ********************/

void
SystemTopologyDrawing::setXAngle( int )
{
    updateDrawing();
}

void
SystemTopologyDrawing::setYAngle( int )
{
    updateDrawing();
}


/************************* coloring  ***********************/

void
SystemTopologyDrawing::updateColors()
{
    draw();
}



/*******************************************/

// calculate minimal distance beetween two planes to be shown completely
int
SystemTopologyDrawing::getFullPlaneDistance()
{
    double distance;
    //   p0   p1
    // p3  p2
    QPointF p0( plane.getPoint( 0 )->getX(), plane.getPoint( 0 )->getY() );
    QPointF p1( plane.getPoint( 1 )->getX(), plane.getPoint( 1 )->getY() );
    QPointF p2( plane.getPoint( 2 )->getX(), plane.getPoint( 2 )->getY() );
    QPointF p3( plane.getPoint( 3 )->getX(), plane.getPoint( 3 )->getY() );

    if ( p0.x() > p1.x() )
    {
        std::swap( p0, p1 );
        std::swap( p2, p3 );
    }

    // need less space if planes can be placed side by side
    // calculate triangle height with baseline b3p2
    //         p0   p1          p0   p1
    // p3  p2                            p3  p2
    if ( p2.x() < p0.x() || p1.x() < p3.x() )
    {
        double base  = p2.x() - p3.x();
        double dx    = p3.x() - p0.x();
        double dy    = p3.y() - p0.y();
        double m     = dy / dx;
        double alpha = atan( m );
        distance = fabs( tan( alpha ) * base );
    }
    else
    {
        distance =  abs( p3.y() - p0.y() );
    }

    return distance;
}

/**
 * @return the size of topology drawing
 */
QSize
SystemTopologyDrawing::getDrawingSize() const
{
    if ( ( data->getDim( XX ) == 0 ) || ( data->getDim( YY ) == 0 ) || ( data->getDim( ZZ ) == 0 ) )
    {
        return QSize( 0, 0 );
    }
    QSize psize  = plane.size();
    int   width  = psize.width() + 2 * plane.getMargin().width();
    int   height = psize.height() + 2 * plane.getMargin().height();  // height of the plane

    int distance = 0;
    for ( unsigned i = 0; i < data->getDim( ZZ ) - 1; i++ )
    {
        distance += transform->getPlaneDistance( i );
    }

    height += distance;
    return QSize( width, height );
}


QSize
SystemTopologyDrawing::sizeHint() const
{
    int preferred = data->getDim( XX ) * 2; // at least 2 pixel for each element in x direction
    return QSize( std::max( 100, preferred ), 100 );
}

/**
 * @return the maximum of topology drawing size and widget size
 */
QSize
SystemTopologyDrawing::getMinimumPixmapSize() const
{
    QSize ret = getDrawingSize();

    // ensure that the drawing size is not smaller than the widget size
    int w = parentSize.width();
    int h = parentSize.height();
    if ( ret.width() < w )
    {
        ret.setWidth( w );
    }
    if ( ret.height() < h )
    {
        ret.setHeight( h );
    }
    return ret;
}

/**
   scales the plane to fit into the widget
   @param planeDistance
 */
void
SystemTopologyDrawing::scale( int planeDistance )
{
    if ( data->getDim( XX ) == 0 )
    {
        return;
    }

    widgetScaleFactor = plane.scale( parentSize, planeDistance, transform->getXAngle(), transform->getYAngle() );
}

/**
   scales the topology to fit into the window
   @param resetDistance recalculate plane distance, if true
 */
void
SystemTopologyDrawing::rescale( bool resetPlaneDistance )
{
    if ( data->getDim( XX ) == 0 )
    {
        return;                     // invalid data
    }
    // reset the plane distance if desired or not yet changed
    bool recalculatePlaneDistance = ( resetPlaneDistance || !transform->distanceHasChanged() );

    int planeDistance = recalculatePlaneDistance ? 1 : transform->getPlaneDistance( -1 );
    scale( planeDistance );

    if ( data->getDim( ZZ ) == 1 )
    {
        planeDistance = 1; // only one plane, no need to calculate distance
    }
    else if ( recalculatePlaneDistance )
    {
        int height = plane.size().height();

        double distance = ( ( double )( parentSize.height() ) - height - 2 * plane.getMargin().height() )
                          / ( double )( data->getDim( ZZ ) - 1 );
        if ( distance <= 1.0 )
        {
            transform->setPlaneDistance( 1 );
        }
        else
        {
            transform->setPlaneDistance( ( int )distance );
        }
    }
}

/**
   If no mouse button is pressed, the image data hasn't changed and the image
   can be copied from offscreen image
   If a mouse button is pressed, the image is painted directly on screen to avoid
   unnecessary copy steps
 */
void
SystemTopologyDrawing::paintEvent( QPaintEvent* )
{
    if ( ( data->getDim( XX ) == 0 ) || ( data->getDim( YY ) == 0 ) || ( data->getDim( ZZ ) == 0 ) )
    {
        return; // topology only consist of unused hardware, which is hidden
    }
    QPainter painter( this );
    painter.fillRect( rect(), this->palette().brush( QPalette::Window ) );

    // mouse action cannot be handled with offscreen image
    if ( rightMousePressed || leftMousePressed )
    {
        drawOnScreen();
    }
    else                                                 // use offscreen image
    {
        bool selectionChanged = data->updateSelection(); // todo optimize
        if ( offscreen == 0 || selectionChanged )
        {
            drawOffScreen();
        }
        painter.drawPixmap( topX, topY, *offscreen ); // draw at position topX, topY
    }
}

void
SystemTopologyDrawing::setInitialFocus()
{
    if ( !transform->isFocusEnabled() )
    {
        transform->setCurrentPlane( -1 );
    }
    else
    {
        bool focusEnabled = ( data->getDim( ZZ ) > 1 ) && transform->isFocusEnabled();

        TreeItem* item = topologyToolBar->getRecentSelection();
        // selection has changed by the user in another tab
        bool selectionChanged = item != selectedItem;

        // only scroll, if selection of a plane changes the plane distances
        bool changesDistance = getFullPlaneDistance() > transform->getPlaneDistance( -1 );

        if ( focusEnabled && changesDistance && selectionChanged )
        {
            selectedItem = item;
            for ( uint x = 0; x < data->getDim( XX ); x++ )
            {
                for ( uint y = 0; y < data->getDim( YY ); y++ )
                {
                    for ( uint z = 0; z < data->getDim( ZZ ); z++ )
                    {
                        if ( data->getTreeItem( x, y, z ) == item )
                        {
                            transform->setCurrentPlane( z );
                            int  posx = this->size().width() / 2; // todo
                            emit scrollTo( posx, coordinateToScreenY( y, z ) );
                            break;
                        }
                    }
                }
            }
        }
    }
}

/**
   This method is called, if the data to draw has changed.
   If no mouse button is pressed, the offscreen image will be drawn.
   If mouse is pressed, drawing is done directly on screen using the
   paintEvent method.
 */
void
SystemTopologyDrawing::draw()
{
    if ( data->getDim( XX ) == 0  || !isVisible() )
    {
        return;
    }
    data->updateSelection(); // todo optimize: call updateSelection only if selection has changed

    setInitialFocus();

    if ( offscreen &&
         ( ( offscreen->width() < parentSize.width() ) ||
           ( offscreen->height() < parentSize.height() ) ) )
    {
        delete offscreen;
        offscreen = 0;
    }

    if ( ( offscreen == 0 ) || !( rightMousePressed || leftMousePressed ) )
    {
        drawOffScreen();
    }

    update();
}

void
SystemTopologyDrawing::drawOnScreen()
{
    // if no topology loaded, return
    if ( data->getDim( ZZ ) == 0 )
    {
        return;
    }

    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing, data->getAntialiasing() );

    paintTopology( painter, true );
}

// segfault with large images is fixed with Qt5.12.4, see https://bugreports.qt.io/browse/QTBUG-75082
// => use smaller offscreen images, so that maxX*maxY*QPixMap.dept() < INT_MAX
#define MAXPIX 8192

/**
   draws current topology to offscreen pixmap
 */
void
SystemTopologyDrawing::drawOffScreen()
{
    // if no topology loaded, return
    if ( data->getDim( XX ) == 0 )
    {
        return;
    }
    if ( !isVisible() )
    {
        return;               // avoid unnecessary painting
    }

    transform->setFullPlaneDistance( getFullPlaneDistance() );

    // create offscreen image
    const int diff = 400; // resize Pixmap, if more than diff pixel are unused

    QSize newSize = getMinimumPixmapSize();

    if ( newSize.width()  > MAXPIX )
    {
        newSize.setWidth( MAXPIX );
    }
    if ( newSize.height() > MAXPIX )
    {
        newSize.setHeight( MAXPIX );
    }

    if ( offscreen == 0 )
    {
        offscreen = new QPixmap( newSize );
    }
    else if ( ( ( offscreen->width() < newSize.width() ) || ( offscreen->height() < newSize.height() ) )
              || ( ( offscreen->width()  - newSize.width() )  > diff )
              || ( ( offscreen->height() - newSize.height() ) > diff ) )
    {
        delete offscreen;
        offscreen = new QPixmap( newSize );
    }

    offscreen->fill( this->palette().brush( QPalette::Window ).color() );

    QPainter painter( offscreen );
    painter.setRenderHint( QPainter::Antialiasing, data->getAntialiasing() );

    paintTopology( painter );
}

/**
 * @param point mouse 2D coordinates
 * @return index of the plane at mouse position
 */
int
SystemTopologyDrawing::getSelectedPlane( const QPoint& point )
{
    int  yscroll       = 0;
    int  selectedPlane = -1;
    bool isRising      = plane.isRising();

    // get selected plane index
    Tetragon polygon;
    polygon.push_back( QPointF( plane.getPoint( 0 )->getX(), plane.getPoint( 0 )->getY() ) );
    polygon.push_back( QPointF( plane.getPoint( 1 )->getX(), plane.getPoint( 1 )->getY() ) );
    polygon.push_back( QPointF( plane.getPoint( 2 )->getX(), plane.getPoint( 2 )->getY() ) );
    polygon.push_back( QPointF( plane.getPoint( 3 )->getX(), plane.getPoint( 3 )->getY() ) );

    // check in opposite direction than painting is done
    int direction = !isRising ? -1 : 1;
    if ( direction == -1 )
    {
        // since the plane is rising, we paint the lowest plane first -> scroll down
        int y = 0;
        for ( unsigned i  = 1; i < data->getDim( ZZ ); i++ )
        {
            y += transform->getPlaneDistance( i, isRising, direction );
        }
        polygon.translate( 0, y );
    }

    for ( unsigned ii = 0; ii < data->getDim( ZZ ); ii++ )
    {
        int idx = ( direction == -1 ) ?  ( data->getDim( ZZ ) - 1 - ii ) : ii;
        if ( polygon.containsPoint( point ) )
        {
            selectedPlane = idx;
            break;
        }
        int distance = direction * transform->getPlaneDistance( idx, isRising, direction );
        yscroll +=  distance;
        polygon.translate( 0, distance );
    }
    return selectedPlane;
}
/**
 * @return the 3D coordinates of the selected element at mouse position clickedPoint
 */
void
SystemTopologyDrawing::getSelectedPosition(
    const QPoint& clickedPoint,
    int&          xx,
    int&          yy,
    int&          zz )
{
    xx = -1;
    yy = -1;
    zz = getSelectedPlane( clickedPoint );
    if ( zz == -1 )
    {
        return;           // not found
    }
    bool isRising  = plane.isRising();
    int  direction = 1;
    int  yscroll   = 0;
    // move plane to the position of the selected plane
    for ( unsigned idx = 0; idx < ( unsigned )zz; idx++ )
    {
        int distance = transform->getPlaneDistance( idx, isRising, direction );
        yscroll +=  distance;
        plane.yScroll( distance );
    }
    plane.getSelectedInPlane( clickedPoint, xx, yy );
    plane.yScroll( -yscroll );
}

void
SystemTopologyDrawing::paintTopology( QPainter& painter, bool optimizeScreen )
{
    setMinimumSize( getMinimumPixmapSize() );
    setMaximumSize( getMinimumPixmapSize() );

    plane.setFoldingSeparator( data->getFoldingSeparators() );
    // --------------------
    // if the planes are rising, then we paint the lowest plane first,
    // otherwise we start with the plane on the top
    bool isRising  = plane.isRising();
    int  direction = isRising ? -1 : 1;
    int  yscroll   = 0;

    // move drawing, if offscreen image is smaller than the drawing
    plane.xScroll( -this->topX );
    plane.yScroll( -this->topY );

    if ( direction == -1 )
    {
        // since the plane is rising, we paint the lowest plane first -> scroll down
        int y = 0;
        for ( unsigned i  = 1; i < data->getDim( ZZ ); i++ )
        {
            y += transform->getPlaneDistance( i, isRising, direction );
        }
        plane.yScroll( y );
        yscroll += y;
    }
    // optimisation: only paint outline while rotating
    int elements = data->getDim( XX ) * data->getDim( YY ) * data->getDim( ZZ );
    if ( ( elements > 50000 ) && ( isRotating ) )
    {
        for ( unsigned ii = 0; ii < data->getDim( ZZ ); ii++ )
        {
            unsigned idx = ( direction == -1 ) ?  ( data->getDim( ZZ ) - 1 - ii ) : ii; // number of plane to paint
            // compute which items on planes are visible if the plane is partly
            // covered by another plane
            int planeDistance = transform->getPlaneDistance( idx, isRising, direction );

            Tetragon polygon;
            polygon.push_back( QPointF( plane.getPoint( 0 )->getX(), plane.getPoint( 0 )->getY() ) );
            polygon.push_back( QPointF( plane.getPoint( 1 )->getX(), plane.getPoint( 1 )->getY() ) );
            polygon.push_back( QPointF( plane.getPoint( 2 )->getX(), plane.getPoint( 2 )->getY() ) );
            polygon.push_back( QPointF( plane.getPoint( 3 )->getX(), plane.getPoint( 3 )->getY() ) );

            QPen pen( Qt::black );
            pen.setWidth( 2 );
            painter.setBrush( Qt::lightGray );
            painter.setPen( pen );
            painter.drawPolygon( polygon );

            // scroll the plane to represent the next plane above the
            // currently painted one
            int y = direction * planeDistance;
            plane.yScroll( y );
            yscroll += y;
        }
    }
    else
    {
        // paint all planes, direction depends on isRising
        for ( unsigned ii = 0; ii < data->getDim( ZZ ); ii++ )
        {
            unsigned idx = ( direction == -1 ) ?  ( data->getDim( ZZ ) - 1 - ii ) : ii; // number of plane to paint
            // compute which items on planes are visible if the plane is partly
            // covered by another plane
            int planeDistance = transform->getPlaneDistance( idx, isRising, direction );
            plane.computeVisibles( direction * planeDistance );

            bool toPaint = true;
            if ( optimizeScreen )
            {
                // this is the region of this widget which is not covered by other
                // widgets; only planes in this widget must be painted
                QRect rect = visibleRegion().boundingRect();
                // don't paint if whole plane is below the visible rectangle
                if ( ( int )plane.getPoint( 0 )->getY() > rect.y() + rect.height() &&
                     ( int )plane.getPoint( 1 )->getY() > rect.y() + rect.height() &&
                     ( int )plane.getPoint( 2 )->getY() > rect.y() + rect.height() &&
                     ( int )plane.getPoint( 3 )->getY() > rect.y() + rect.height() )
                {
                    toPaint = false;
                }
                // also don't paint if plane is above the visible rectangle
                else if ( ( int )plane.getPoint( 0 )->getY() < rect.y() &&
                          ( int )plane.getPoint( 1 )->getY() < rect.y() &&
                          ( int )plane.getPoint( 2 )->getY() < rect.y() &&
                          ( int )plane.getPoint( 3 )->getY() < rect.y() )
                {
                    toPaint = false;
                }
            }

            // mark current plane with thick outline

            if ( ( int )idx == transform->getCurrentPlane() )
            {
                Tetragon polygon;
                polygon.push_back( QPointF( plane.getPoint( 0 )->getX(), plane.getPoint( 0 )->getY() ) );
                polygon.push_back( QPointF( plane.getPoint( 1 )->getX(), plane.getPoint( 1 )->getY() ) );
                polygon.push_back( QPointF( plane.getPoint( 2 )->getX(), plane.getPoint( 2 )->getY() ) );
                polygon.push_back( QPointF( plane.getPoint( 3 )->getX(), plane.getPoint( 3 )->getY() ) );

                QPen pen( Qt::black );
                pen.setWidth( 3 );
                painter.setBrush( Qt::NoBrush );
                painter.setPen( pen );
                painter.drawPolygon( polygon );
            }

            bool isPlaneAbove = ( direction == -1 ) ? ( idx > 0 ) : ( idx < data->getDim( ZZ ) - 1 );

            if ( toPaint )
            {
                plane.paint( idx, painter, isPlaneAbove );
            }

            // scroll the plane to represent the next plane above the
            // currently painted one
            int y = direction * planeDistance;
            plane.yScroll( y );
            yscroll += y;
        } // for
    }
    plane.yScroll( -yscroll );

    // ----------------------------

    QPen pen( Qt::black );
    pen.setWidth( 0 );
    painter.setPen( pen );

    if ( data->hasInvalidDimensions() )
    {
        painter.setOpacity( 0.7 );
        painter.fillRect( this->rect(), Qt::gray );
    }

    // undo move for offscreen image
    plane.xScroll( this->topX );
    plane.yScroll( this->topY );
}

/**************** mouse and key event handling ******************/

void
SystemTopologyDrawing::showInfoTooltip( const QPoint& mouse )
{
    int x, y, z;
    getSelectedPosition( mouse, x, y, z );
    QPoint pos = this->mapToGlobal( mouse );
    pos.rx() += 5;
    pos.ry() += 5;
    info->showInfo( pos, data->getTooltipText( x, y, z ) );
}

// handle mouse press events
//
void
SystemTopologyDrawing::mousePressEvent( QMouseEvent* event )
{
    Qt::KeyboardModifiers mods = event->modifiers();
    // check if shift is pressed
    if ( mods.testFlag( Qt::ShiftModifier ) )
    {
        shiftPressed = true;
    }
    else
    {
        shiftPressed = false;
    }
    // check if control is pressed
    if ( mods.testFlag( Qt::ControlModifier ) )
    {
        controlPressed = true;
    }
    else
    {
        controlPressed = false;
    }

    // get the position of the click
    lastPoint = event->pos();
    // for left-mouse-pull remember the first point to compute the
    // difference later for mouse movement
    if ( !leftMousePressed )
    {
        firstPoint = lastPoint;
    }
    // left button is handled at mouse release or mouse movement
    if ( event->button() == Qt::LeftButton )
    {
        leftMousePressed = true;
        event->accept();
    }
    else if ( event->button() == Qt::RightButton )
    {
        // right button is for context info
        event->accept();
        showInfoTooltip( lastPoint );
    }
    else
    {
        event->ignore();
    }

    data->updateSelection();
}

/**
 * If the selection of a plane changes the plane distances, the new position of the selected element
 * is returned.
 * If scrolling to the new position is not possible, the drawing is enlarged by adding additional space
 * between neighbor planes.
 * @param y,z coordinates of the selected element
 */
int
SystemTopologyDrawing::getYPositionToScroll( int y, int z )
{
    double rowHeight = plane.size().height() / data->getDim( YY );
    int    yInWin    = this->parentWidget()->mapFromGlobal( QCursor::pos() ).y();

    int ynew = 0;
    for ( uint i = 0; i < data->getDim( ZZ ); ++i )
    {
        setMinimumSize( getMinimumPixmapSize() );
        setMaximumSize( getMinimumPixmapSize() );
        ynew = coordinateToScreenY( y, z );               // position to scroll to
        int yInDrawing = this->mapFromGlobal( QCursor::pos() ).y();
        int yscroll    = yInDrawing - ynew;               // this amount has to be scrolled to stay at current mouse position
        int scrollTop  = yInDrawing - yInWin;             // topmost y-position of visible area
        int scrollBot  = scrollTop + parentSize.height(); // lowermost y-position of visible area

        if ( ( yscroll > 0 && yscroll > scrollTop + rowHeight / 2 )
             || ( yscroll < 0 && yscroll < scrollBot - height() - rowHeight / 2 )    )
        {
            int direction = yscroll > 0 ? -1 : 1;
            transform->addFullPlaneDistance( direction );
            ynew = coordinateToScreenY( y, z );
        }
        else
        {
            break;
        }
    }
    return ynew;
}

// handle mouse release events
//
void
SystemTopologyDrawing::mouseReleaseEvent( QMouseEvent* event )
{
    if ( event->button() == Qt::LeftButton )
    {
        isRotating       = false;
        leftMousePressed = false;
        // if the left mouse button is pressed and released at the same point
        // then we select the corresponding tree item
        if ( firstPoint == event->pos() )
        {
            int x, y, z;
            getSelectedPosition( firstPoint, x, y, z );
            selectedItem = 0;
            if ( x >= 0 )
            {
                bool focusEnabled = false;

                if ( data->getDim( ZZ ) > 1 )
                {
                    focusEnabled = transform->isFocusEnabled();
                }
                if ( focusEnabled )
                {
                    transform->setCurrentPlane( z );
                }
                selectedItem = data->getTreeItem( x, y, z );
                data->updateSelection();

                transform->resetPlaneDistances();

                if ( selectedItem )
                {
                    emit selectItem( selectedItem, controlPressed );

                    // scroll, if selection of a plane changes the plane distances
                    if ( focusEnabled &&
                         ( getFullPlaneDistance() > transform->getPlaneDistance( -1 ) ) )
                    {
                        int  ynew = getYPositionToScroll( y, z ); // ynew = coordinateToScreenY( y, z );
                        emit scrollTo( -1, ynew );
                    }
                }
            }
            else
            {
                topologyToolBar->setRecentSelection( 0 );
                transform->setCurrentPlane( -1 );
            }
        }
        draw(); // draw to offline image
    }
    else if ( event->button() == Qt::RightButton )
    {
        // remove the info box for the previously right-clicked item
        rightMousePressed = false;
        event->accept();
        info->hide();
    }
    else
    {
        event->ignore();
    }
}

/**
 * @brief SystemTopologyDrawing::coordinateToYPos calculates the y position of
 * the given cube coordinates
 * @param y cube coordinate
 * @param z cube coordinate
 * @return screen coordinate in y direction
 */
int
SystemTopologyDrawing::coordinateToScreenY( int y,
                                            int z )
{
    bool isRising  = plane.isRising();
    int  direction = 1;
    int  yscroll   = plane.getMargin().height();

    // get position of the selected plane
    for ( unsigned idx = 0; idx < ( unsigned )z; idx++ )
    {
        int distance = transform->getPlaneDistance( idx, isRising, direction );
        yscroll +=  distance;
    }

    // get y-position inside the plane
    double rowHeight = plane.size().height() / data->getDim( YY );
    if ( plane.isRising() )
    {
        yscroll += ( y + 0.5 ) * rowHeight;
    }
    else
    {
        yscroll += ( data->getDim( YY ) - y - 0.5 ) * rowHeight;
    }
    return yscroll;
}

// handle mouse move events
//
void
SystemTopologyDrawing::mouseMoveEvent( QMouseEvent* event )
{
    if ( event->buttons() & Qt::LeftButton )
    {
        // if mouse is left-clicked and moved while shift is pressed, then
        // move the topology
        if ( shiftPressed )
        {
            moveTo( event->pos() );
        }
        // if mouse is left-clicked and moved while control is pressed,
        // then increase the plane distance
        else if ( controlPressed )
        {
            transform->setCurrentPlane( -1 );
            int planeDistance = transform->getPlaneDistance( -1 );
            planeDistance += ( int )event->pos().y() - lastPoint.y();
            lastPoint      = event->pos();
            if ( planeDistance <= 0.0 )
            {
                planeDistance = 1;
            }
            transform->setPlaneDistance( planeDistance );
            draw();
        }
        // if mouse is left-clicked and moved, rotate the topology
        else
        {
            QPoint diff = event->pos() - firstPoint;
            if ( diff.manhattanLength() > 3 ) // avoid accidental rotation with minimal difference
            {
                rotateTo( event->pos() );
                isRotating = true;
            }
        }
        event->accept();
    }
    else // not left mouse button
    {
        if ( event->buttons() & Qt::RightButton )
        {
            // if mouse is right-clicked and moved, then change the item for
            // which the info box is displayed
            lastPoint = event->pos();
            showInfoTooltip( lastPoint );
            event->accept();
        }
        event->accept();
    }
}

// handle mouse wheel events
//
void
SystemTopologyDrawing::wheelEvent( QWheelEvent* e )
{
#if QT_VERSION < QT_VERSION_CHECK( 5, 14, 0 )
    double angle = e->delta();
#else
    double angle = e->angleDelta().y();
#endif
    if ( controlPressed )
    {
        int cur    = transform->getCurrentPlane();
        int planes = data->getDim( ZZ );
        int next   = cur + ( angle > 0 ? -1 : 1 );
        if ( next >= 0 && next < planes )
        {
            transform->setCurrentPlane( next ); // show current plane completely
            draw();
        }
    }
    // the mouse wheel can be used for zooming
    else if ( angle > 0 )
    {
        transform->zoomIn();
    }
    else
    {
        transform->zoomOut();
    }
    e->accept();
}

void
SystemTopologyDrawing::keyPressEvent( QKeyEvent* event )
{
    switch ( event->key() )
    {
        case Qt::Key_Shift:
            if ( !controlPressed )
            {
                shiftPressed = true;
            }
            break;
        case Qt::Key_Control:
            if ( !shiftPressed )
            {
                controlPressed = true;
            }
            break;
        case Qt::Key_Up:
        case Qt::Key_Down:
        {
            int cur    = transform->getCurrentPlane();
            int planes = data->getDim( ZZ );
            int next   = cur + ( event->key() == Qt::Key_Up ? -1 : 1 );
            if ( ( transform->isFocusEnabled() )  &&
                 ( next >= 0 && next < planes ) )
            {
                transform->setCurrentPlane( next );
                updateDrawing();
                emit scrollTo( -1, coordinateToScreenY( 0, next ) );
            }
            break;
        }
        case Qt::Key_Plus:
            if ( !shiftPressed )
            {
                transform->zoomIn();
            }
            else
            {
                transform->increasePlaneDistance();
            }
            break;
        case Qt::Key_Minus:
            if ( !shiftPressed )
            {
                transform->zoomOut();
            }
            else
            {
                transform->decreasePlaneDistance();
            }
            break;
        default: // key event is not handled here
        {
            event->ignore();
            return;
        }
    }
    event->accept();
}

void
SystemTopologyDrawing::keyReleaseEvent( QKeyEvent* event )
{
    switch ( event->key() )
    {
        case Qt::Key_Shift:
            shiftPressed = false;
            event->accept();
            break;
        case Qt::Key_Control:
            controlPressed = false;
            break;
        default:
        { event->ignore();
        }
    }
}


/****************** basic plane transformations ************************/

void
SystemTopologyDrawing::rotateTo( const QPoint& endPoint )
{
    int dx = endPoint.x() - lastPoint.x();
    int dy = endPoint.y() - lastPoint.y();
    lastPoint = endPoint;
    int diffX = dx % 10;
    int diffY = dy % 10;

    if ( transform->getYAngle() <= 90 || transform->getYAngle() >= 270 )
    {
        diffY = -diffY;
    }

    int xAngle = transform->getXAngle() - diffY;
    int yAngle = transform->getYAngle() - diffX;

    if ( xAngle != prevXAngle )
    {
        transform->setXAngle( xAngle );
    }
    if ( yAngle != prevYAngle )
    {
        transform->setYAngle( yAngle );
    }
}

void
SystemTopologyDrawing::moveTo( const QPoint& endPoint )
{
    int dx = endPoint.x() - lastPoint.x();
    int dy = endPoint.y() - lastPoint.y();
    plane.addMargin( QSize( dx, dy ) );
    lastPoint = endPoint;

    draw();
}

/**
    moves the drawing by dx in x-direction and dy in y-direction
 */
void
SystemTopologyDrawing::move( int dx, int dy )
{
    plane.addMargin( QSize( dx, dy ) );

    draw();
}

/**
 * zoom with the given factor until baseline length has changed or minimum size has been reached
 */
void
SystemTopologyDrawing::changeZoom( double factor )
{
    // check if minimum zoom level has been reached
    int minPixelPerElement = data->getLineType() == NO_LINES ? 1 : 2;
    int minWidth           = data->getDim( XX ) * minPixelPerElement;
    int newWidth           = plane.baseline();
    int oldWidth           = newWidth;

    bool   finished      = false;
    double minimumFactor = data->getDim( XX ) * 1. / parentSize.width(); // minimum absolute scale factor
    double absFactor     = transform->getAbsoluteScaleFactor();
    double relFactor     = 1.;

    while ( !finished )
    {
        oldWidth = newWidth;
        plane.scale( factor );
        transform->zoom( factor );
        newWidth   = plane.baseline();
        relFactor *= factor;
        absFactor *= factor;

        if ( oldWidth != newWidth )
        {
            finished = true;                      // zoom successfull
        }
        else if ( factor < 1 )
        {
            if ( oldWidth <= minWidth )
            {
                finished = true;                        // minimum size reached
            }
            if ( absFactor < minimumFactor )
            {
                finished = true;
            }
        }
    }

    if ( oldWidth != newWidth )   // zooming is possible/successfull
    {
        // calculate new mouse position after zooming
        QPoint mouse = this->mapFromGlobal( QCursor::pos() ) * relFactor;

        draw();

        // scroll to stay at previous position
        emit scrollTo( mouse.x(), mouse.y() );
    }
}

/**
 * scrolledTo is called, if the scrollbars of the parent scrollpane are used. If the drawing
 * is bigger than the offscreen image, the offscreenimage may have to be redrawn to show the
 * current section.
 * @param x,y position of the upper left corner of the visible area
 */
void
SystemTopologyDrawing::scrolledTo( int x, int y )
{
    QSize minSize = getMinimumPixmapSize();
    int   maxY    = std::min( MAXPIX, minSize.height() ); // size of the offscreen image
    int   maxX    = std::min( MAXPIX, minSize.width() );

    QSize parentSize     = this->parentWidget()->size();
    int   heightToScroll = maxY - parentSize.height();
    int   widthToScroll  = maxX - parentSize.width();
    int   padX           = widthToScroll / 4;
    int   padY           = heightToScroll / 4;
    bool  redraw         = false;

    if ( y > topY + heightToScroll )
    {
        int maxY = std::max( minSize.height() - MAXPIX, 0 );
        topY   = std::min( y - padY, maxY );
        redraw = true;
    }
    else if ( y < topY )
    {
        topY   = std::max( y - heightToScroll + padY, 0 );
        redraw = true;
    }
    else if ( x > topX + widthToScroll )
    {
        topX   = std::max( x - padX, 0 );
        redraw = true;
    }
    else if ( x < topX )
    {
        int maxX = std::max( minSize.width() - MAXPIX, 0 );
        topX   = std::min( x - widthToScroll + padX, maxX );
        redraw = true;
    }
    if ( redraw )
    {
        drawOffScreen();
        update();
    }
}

// ----------------------------------------------------------------------------------------
// ----------------------------------------------------------------------------------------

InfoToolTip::InfoToolTip() : size( 1, 1 )
{
    setWindowFlags( Qt::ToolTip );
    setMinimumSize( size );
    setMaximumSize( size );
}

void
InfoToolTip::showInfo( const QPoint& pos, const QStringList& tooltipText )
{
    if ( tooltipText.size() == 2 )
    {
        left  = tooltipText.at( 0 );
        right = tooltipText.at( 1 );
        move( pos );
        show();
        repaint(); // use repaint to get correct size immediately (before setMinimumSize)
        setMinimumSize( size );
        setMaximumSize( size );
        update();
    }
    else
    {
        hide();
    }
}

void
InfoToolTip::paintEvent( QPaintEvent* )
{
    if ( left.isEmpty() )
    {
        return;
    }

    QPainter painter( this );
    painter.setBrush( Qt::white );
    painter.setPen( Qt::black );

    // get height and width of the text blocks
    QRect rect1 = painter.boundingRect( 0, 0, 0, 0, Qt::AlignLeft, left );
    QRect rect2 = painter.boundingRect( 0, 0, 0, 0, Qt::AlignLeft, right );

    QRect rect( 0, 0, rect1.width() + rect2.width() + 11,
                ( rect1.height() > rect2.height() ? rect1.height() + 6 : rect2.height() + 6 ) );

    size = rect.size();

    rect.adjust( 0, 0, -1, -1 );
    painter.drawRect( rect );
    rect1.moveTo( rect.x() + 3, rect.y() + 3 );
    painter.drawText( rect1, Qt::AlignLeft, left );
    rect2.moveTo( rect1.x() + rect1.width() + 5, rect1.y() );
    painter.drawText( rect2, Qt::AlignLeft, right );
}

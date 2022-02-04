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
#include <QDebug>
#include <cassert>
#include <cmath>
#include <iostream>

#include "Plane.h"
#include "SystemTopologyToolBar.h"
using namespace std;
using namespace cubepluginapi;

//constructor
Plane::Plane()
{
    visible    = NULL;
    data       = NULL;
    markMerged = false;
    margin     = QSize( TOPOLOGY_MARGIN, TOPOLOGY_MARGIN );
    lineType   = BLACK_LINES;

    for ( int i = 0; i < 5; ++i )
    {
        points[ i ]         = Point();
        pointsOnScreen[ i ] = Point();
    }
    memset( foldingDimensions, 0, 3 * sizeof( int ) );
}

/**
 * reset to minimal size (at least minpixel points for one element)
 */
void
Plane::init( int xAngle, int yAngle )
{
    static int minpixel = 2;
    double     x        = minpixel * ( double )data->getDim( XX );

    //get height according to width
    double y = x / ( double )data->getDim( XX ) * ( double )data->getDim( YY );
    //minimal height is min pixels per square
    if ( y < minpixel * ( double )data->getDim( YY ) )
    {
        y = minpixel * ( double )data->getDim( YY );
        x = y / ( double )data->getDim( YY ) * ( double )data->getDim( XX );
    }

    //get the middle
    x = x / 2;
    y = y / 2;

    //the four plane corners are the points 0 to 3
    points[ 0 ] = Point( -x, -y, 0 );
    points[ 1 ] = Point( x, -y, 0 );
    points[ 2 ] = Point( x, y, 0 );
    points[ 3 ] = Point( -x, y, 0 );
    //plane point 4 is the middle, needed for certain operations like y
    points[ 4 ] = Point( 0, 0, 0 );

    adjustToScreen();

    xRotate( xAngle );
    yRotate( yAngle );

    while ( size().width() < minpixel * ( double )data->getDim( XX ) )
    {
        this->scale( 1.2 );
    }
    adjustToScreen();
}

//rotate the plane about the X-axis
void
Plane::xRotate( double angle )
{
    for ( unsigned i = 0; i < 4; i++ )
    {
        points[ i ].xScroll( -points[ 4 ].getX() );
        points[ i ].yScroll( -points[ 4 ].getY() );
        points[ i ].zScroll( -points[ 4 ].getZ() );
        points[ i ].xRotate( angle );
        points[ i ].xScroll( points[ 4 ].getX() );
        points[ i ].yScroll( points[ 4 ].getY() );
        points[ i ].zScroll( points[ 4 ].getZ() );
    }
    adjustToScreen();
}

//rotate the plane about the Y-axis
void
Plane::yRotate( double angle )
{
    for ( unsigned i = 0; i < 4; i++ )
    {
        points[ i ].xScroll( -points[ 4 ].getX() );
        points[ i ].yScroll( -points[ 4 ].getY() );
        points[ i ].zScroll( -points[ 4 ].getZ() );
        points[ i ].yRotate( angle );
        points[ i ].xScroll( points[ 4 ].getX() );
        points[ i ].yScroll( points[ 4 ].getY() );
        points[ i ].zScroll( points[ 4 ].getZ() );
    }
    adjustToScreen();
}

//rotate the plane about the Z-axis
void
Plane::zRotate( double angle )
{
    for ( unsigned i = 0; i < 4; i++ )
    {
        points[ i ].xScroll( -points[ 4 ].getX() );
        points[ i ].yScroll( -points[ 4 ].getY() );
        points[ i ].zScroll( -points[ 4 ].getZ() );
        points[ i ].zRotate( angle );
        points[ i ].xScroll( points[ 4 ].getX() );
        points[ i ].yScroll( points[ 4 ].getY() );
        points[ i ].zScroll( points[ 4 ].getZ() );
    }
    adjustToScreen();
}

/** scroll the plane along the X-axis
 * @param value move the plane by the given value
 */
void
Plane::xScroll( double value )
{
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].xScroll( value );
        pointsOnScreen[ i ].xScroll( value );
    }
}

//scroll the plane along the Y-axis
void
Plane::yScroll( double value )
{
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].yScroll( value );
        pointsOnScreen[ i ].yScroll( value );
    }
}

/** the given margin is added to all screen coordinates */
void
Plane::addMargin( QSize m )
{
    margin += m;
    if ( margin.width() < TOPOLOGY_MARGIN )
    {
        margin.setWidth( TOPOLOGY_MARGIN );
    }
    if ( margin.height() < TOPOLOGY_MARGIN )
    {
        margin.setHeight( TOPOLOGY_MARGIN );
    }
    adjustToScreen();
}

void
Plane::setMargin( QSize m )
{
    margin = m;

    if ( margin.width() < TOPOLOGY_MARGIN )
    {
        margin.setWidth( TOPOLOGY_MARGIN );
    }
    if ( margin.height() < TOPOLOGY_MARGIN )
    {
        margin.setHeight( TOPOLOGY_MARGIN );
    }
    adjustToScreen();
}

QSize
Plane::getMargin() const
{
    return margin;
}

//scale(x,y,z,factor) scales the distance from the argument point (x,y,z)
//to the plane's points by the parameter factor
//
bool
Plane::scale( double x, double y, double z, double factor )
{
    bool ok = true;
    // only scale down, if plane will be still visible
    if ( factor < 1 )
    {
        QSize s = size();
        ok = ( s.width() > 10 && s.height() > 10 );
    }
    if ( ok )
    {
        for ( unsigned i = 0; i < 5; i++ )
        {
            points[ i ].scale( x, y, z, factor );
        }
    }
    adjustToScreen();
    return ok;
}

/**
 * scales the plane to fit into maxSize
 * @return absolute factor to scale from minimal size to the new size
 */
double
Plane::scale( QSize maxSize, int planeDistance, int xAngle, int yAngle )
{
    init( xAngle, yAngle ); // scale plane to minimum size
    QSizeF psize = sizeF(); // size of the plane

    /* if planeDistance is set to 1 (minimum), the available space will be used */
    double planeH = ( data->getDim( ZZ ) - 1 ) * planeDistance; // height to display n planes

    double xScaleFactor = ( maxSize.width() - 2 * margin.width() ) / psize.width();
    double yScaleFactor = ( maxSize.height() - planeH - 2 * margin.height() ) / psize.height();
    int    scaleFactor  = ( xScaleFactor < yScaleFactor ) ? xScaleFactor : yScaleFactor;
    /* absolute scale factor of 1.0 shows the plane with its minimal size */
    if ( scaleFactor < 1.0 )
    {
        scaleFactor = 1.0;
    }
    if ( scaleFactor > 1 )
    {
        this->scale( scaleFactor );
    }
    return scaleFactor;
}

/**
 * scales the plane with the given factor
 */
void
Plane::scale( double factor )
{
    moveToTopLeftPoint();
    scale( 0, 0, 0, factor );
}

//returns the index-th element of the points array;
//in the points array points[0]..points[3] are the corner points;
//points[4] is the middle point;
//
const Point*
Plane::getPoint( unsigned index )
{
    assert( index < 5 );
    return &( pointsOnScreen[ index ] );
}

//in the "bool* visible" array we store which positions on the planes
//would not be hidden by another plane above it;
//only those have to be painted on non-top planes;
//this method computes the values of this array;
//parameters:
//dim[]         : are the original dimensions (size = 3)
//dimOrder[]    : defines the order of dimensions (size = 3),
//                for example in XYZ-view it's elements are 0,1,2
//                and in the YZX-view 1,2,0
//planeDistance : gives the current vertical distance between planes in 3D-topologies
//
void
Plane::computeVisibles( int planeDistance )
{
    //if empty topology then return
    if ( data->getDim( ZZ ) == 0 )
    {
        return;
    }

    //clean up the visible array
    if ( visible != NULL )
    {
        delete[] visible;
    }
    visible = new bool[ ( int )( data->getDim( XX ) * data->getDim( YY ) ) ];
    for ( int i = 0; i < ( int )( data->getDim( XX ) * data->getDim( YY ) ); i++ )
    {
        visible[ i ] = true;
    }

    if ( data->getDim( ZZ ) <= 1 )
    {
        return;
    }

    /*        (dx01,dy01)
     *             -->
     *  points[0]  --------------------points[1]
     *            |  |  |  |  |  |  |  |
     *            |---------------------
     * (dx03,dy03)|  |  |  |  |  |  |  |
     *          V |---------------------
     *            |  |  |  |  |  |  |  |
     *            |---------------------
     *            |  |  |  |  |  |  |  |
     *  points[3]  --------------------points[2]
     */

    int dx01 = ( int )( ( points[ 1 ].getX() - points[ 0 ].getX() ) / ( double )data->getDim( XX ) );
    int dy01 = ( int )( ( points[ 1 ].getY() - points[ 0 ].getY() ) / ( double )data->getDim( XX ) );
    int dx03 = ( int )( ( points[ 3 ].getX() - points[ 0 ].getX() ) / ( double )data->getDim( YY ) );
    int dy03 = ( int )( ( points[ 3 ].getY() - points[ 0 ].getY() ) / ( double )data->getDim( YY ) );
    // todo dy01 always 0 ???

    //(x,y) is poinst[0]
    int x = ( int )( points[ 0 ].getX() );
    int y = ( int )( points[ 0 ].getY() );

    //quad is the first small square starting at points[0]
    Tetragon quad;
    quad.push_back( QPointF( ( double )( x ),               ( double )( y ) ) );
    quad.push_back( QPointF( ( double )( x + dx01 ),        ( double )( y + dy01 ) ) );
    quad.push_back( QPointF( ( double )( x + dx01 + dx03 ), ( double )( y + dy01 + dy03 ) ) );
    quad.push_back( QPointF( ( double )( x + dx03 ),        ( double )( y + dy03 ) ) );

    //topPlane is the plane above the current one
    Tetragon topPlane;
    y = ( int )( points[ 0 ].getY() + planeDistance );

    topPlane.push_back( QPointF( ( double )( x ),
                                 ( double )( y ) ) );
    topPlane.push_back( QPointF( ( double )( x + ( int )data->getDim( XX ) * dx01 ),
                                 ( double )( y + ( int )data->getDim( XX ) * dy01 ) ) );
    topPlane.push_back( QPointF( ( double )( x + ( int )data->getDim( XX ) * dx01 + ( int )data->getDim( YY ) * dx03 ),
                                 ( double )( y + ( int )data->getDim( XX ) * dy01 + ( int )data->getDim( YY ) * dy03 ) ) );
    topPlane.push_back( QPointF( ( double )( x + ( int )data->getDim( YY ) * dx03 ),
                                 ( double )( y + ( int )data->getDim( YY ) * dy03 ) ) );

    int index = 0;

    for ( int i = 0; i < ( int )data->getDim( XX ); i++ )
    {
        for ( int j = 0; j < ( int )data->getDim( YY ); j++, index++ )
        {
            //if one of the small square's (quad's) corner is visible we will pain it
            visible[ index ] = ( !topPlane.containsPoint( quad.at( 0 ) ) ||
                                 !topPlane.containsPoint( quad.at( 1 ) ) ||
                                 !topPlane.containsPoint( quad.at( 2 ) ) ||
                                 !topPlane.containsPoint( quad.at( 3 ) ) );
            //take the next square in the 3rd dimension
            quad.translate( dx03, dy03 );
        }
        //check the next square in the 2nd dimension
        quad.translate( dx01 - ( int )( data->getDim( YY ) ) * dx03, dy01 - ( int )( data->getDim( YY ) ) * dy03 );
    }
}
//end of computeVisibles()

/**
 * If two dimensions are merged into one, a separator is drawn
   @param dims if dimension consists of 2 merged dimensions, dims contains the size
          of the 2nd merged dimension
 */
void
Plane::setFoldingSeparator( const unsigned dims[] )
{
    if ( dims != 0 )
    {
        markMerged = false;
        for ( int i = 0; i < 3; i++ )
        {
            foldingDimensions[ i ] = dims[ i ];
            if ( dims[ i ] > 1 )
            {
                markMerged = true;                // dimensions to merge exists
            }
        }
    }
    else
    {
        markMerged = false;
    }
}

/**
 * @brief Plane::getSelectedInPlane calculates coordinates of the element at the
 * position clickedPoint
 * @param data
 * @param clickedPoint the position to calculate
 * @param xx the x index of the element or -1, if not found
 * @param yy the y index of the element or -1, if not found
 */
void
Plane::getSelectedInPlane( const QPoint& clickedPoint,
                           int&          xx,
                           int&          yy )
{
    xx = -1;
    yy = -1;

    //return if the topology is empty
    if ( data->getDim( XX ) == 0 )
    {
        return;
    }
    //----------------------------------------------
    int dx01, dy01, dx03, dy03;
    getGeom( dx01, dy01, dx03, dy03 );
    Tetragon quad = getTetragon( dx01, dy01, dx03, dy03 );

    int dim1 = data->getDim( XX );
    int dim2 = data->getDim( YY );
    for ( int k1 = 0; k1 < dim1; k1++ )
    {
        for ( int k2 = 0; k2 < dim2; k2++ )
        {
            //check if the clicked point is within the current square
            if ( quad.containsPoint( QPointF( ( double )clickedPoint.x(), ( double )clickedPoint.y() ) ) )
            {
                //store the position in (dim1Index,dim2Index)
                xx = k1;
                yy = k2;
            }
            quad.translate( dx03, dy03 );
        }
        //prepare for next row:
        //shift one row further and undo shifting of previous row
        int shifted = dim2;
        quad.translate( dx01 - shifted * dx03, dy01 - shifted * dy03 );
    }
}


/** Adjusts the corner points of the plane to screen coordinates
   The complete plane coordinates are calculated with floating point, but
   the elements inside the plane are drawn with integer sizes. So the plane
   outline has to be adjusted to fit multiples of a single element
 */
void
Plane::adjustToScreen()
{
    int dx01, dy01, dx03, dy03;
    getGeom( dx01, dy01, dx03, dy03 );

    //   p0   p1
    // p3  p2
    pointsOnScreen[ 0 ].setX( points[ 0 ].getX() );
    pointsOnScreen[ 0 ].setY( points[ 0 ].getY() );
    pointsOnScreen[ 1 ].setX( points[ 0 ].getX() + dx01 * ( int )data->getDim( XX ) );
    pointsOnScreen[ 1 ].setY( points[ 0 ].getY() );
    pointsOnScreen[ 3 ].setX( points[ 0 ].getX() + dx03 * ( int )data->getDim( YY ) );
    pointsOnScreen[ 3 ].setY( points[ 0 ].getY() + dy03 * ( int )data->getDim( YY ) );
    pointsOnScreen[ 2 ].setX( pointsOnScreen[ 3 ].getX() + dx01 * ( int )data->getDim( XX ) );
    pointsOnScreen[ 2 ].setY( pointsOnScreen[ 3 ].getY() );

    // get minX, maxY to move plane to top left corner
    Point* p      = pointsOnScreen;
    double xarr[] = { p[ 0 ].getX(), p[ 1 ].getX(), p[ 2 ].getX(), p[ 3 ].getX() };
    double minX   = *std::min_element( xarr, xarr + 4 );
    double yarr[] = { p[ 0 ].getY(), p[ 1 ].getY(), p[ 2 ].getY(), p[ 3 ].getY() };
    double minY   = *std::min_element( yarr, yarr + 4 );

    // set minimal coordinates in x direction to 0
    for ( int i = 0; i < 4; i++ )
    {
        pointsOnScreen[ i ].setX( pointsOnScreen[ i ].getX() - minX + margin.width() );
        pointsOnScreen[ i ].setY( pointsOnScreen[ i ].getY() - minY + margin.height() );
    }
}


void
Plane::getGeom( int& dx01, int& dy01, int& dx03, int& dy03 )
{
    int dimX = data->getDim( XX );
    int dimY = data->getDim( YY );

    dx01 = ( int )( ( points[ 1 ].getX() - points[ 0 ].getX() ) / dimX );
    dy01 = ( int )( ( points[ 1 ].getY() - points[ 0 ].getY() ) / dimX );
    dx03 = ( int )( ( points[ 3 ].getX() - points[ 0 ].getX() ) / dimY );
    dy03 = ( int )( ( points[ 3 ].getY() - points[ 0 ].getY() ) / dimY );
}

/**
 * generates a tetragon which represents one element of current plane
 */
Tetragon
Plane::getTetragon( int dx01, int dy01, int dx03, int dy03 )
{
    int x = ( int )pointsOnScreen[ 0 ].getX();
    int y = ( int )pointsOnScreen[ 0 ].getY();

    //quad is the first small square starting at points[0]
    Tetragon quad;
    quad.push_back( QPointF( ( double )( x ),               ( double )( y ) ) );
    quad.push_back( QPointF( ( double )( x + dx01 ),        ( double )( y + dy01 ) ) );
    quad.push_back( QPointF( ( double )( x + dx01 + dx03 ), ( double )( y + dy01 + dy03 ) ) );
    quad.push_back( QPointF( ( double )( x + dx03 ),        ( double )( y + dy03 ) ) );
    return quad;
}


static void
enlargeQuad( QPolygonF& quad, int width )
{
    double mx =  ( quad[ 0 ].y() - quad[ 3 ].y() ) / ( quad[ 0 ].x() - quad[ 3 ].x() );

    // enlarge in direction of the parallel lines
    int dy = width;
    int dx = ( int )dy / mx;
    if ( quad[ 0 ].y() > quad[ 3 ].y() )
    {
        dy = -dy;
        dx = -dx;
    }
    quad[ 0 ].ry() -= dy;
    quad[ 0 ].rx() -= dx;
    quad[ 1 ].ry() -= dy;
    quad[ 1 ].rx() -= dx;
    quad[ 2 ].ry() += dy;
    quad[ 2 ].rx() += dx;
    quad[ 3 ].ry() += dy;
    quad[ 3 ].rx() += dx;

    // enlarge in direction of the horizontal baseline
    int w = width;
    if ( quad[ 0 ].x() > quad[ 1 ].x() )
    {
        w = -w;
    }
    quad[ 0 ].rx() -= w;
    quad[ 3 ].rx() -= w;
    quad[ 1 ].rx() += w;
    quad[ 2 ].rx() += w;
}

//this method paints the plane;
//parameters:
//currentPlaneIndex is the index in the first dimension, needed to get the colors
//painter           is the QPainter to paint with
//isPlaneAbove      tells if there is another plane above this; if yes then
void
Plane::paint( unsigned  currentPlaneIndex,
              QPainter& painter,
              bool      isPlaneAbove )
{
    //return if the topology is empty
    if ( data->getDim( XX ) == 0 )
    {
        return;
    }
    //----------------------------------------------
    if ( !data->hasUniqueIds() )
    {
        markMerged = false;
    }
    QVector<QLineF> selectedArea;
    //----------------------------------------------

    QColor innerColor;
    QColor outerColor;

    if ( lineType == BLACK_LINES )
    {
        outerColor = Qt::black;
    }
    else if ( lineType == GRAY_LINES )
    {
        outerColor = Qt::gray;
    }
    else if ( lineType == WHITE_LINES )
    {
        outerColor = Qt::white;
    }
    if ( markMerged )
    {
        if ( lineType == BLACK_LINES )
        {
            innerColor = Qt::gray;
        }
        else if ( lineType == GRAY_LINES )
        {
            innerColor = Qt::black;
        }
        else if ( lineType == WHITE_LINES )
        {
            innerColor = Qt::gray;
        }
    }
    else
    {
        innerColor = outerColor;
    }

    //pen1 is the standard pen for painting
    QPen pen1;
    pen1.setWidth( 0 );
    pen1.setJoinStyle( Qt::MiterJoin );
    pen1.setMiterLimit( 0 );
    pen1.setColor( innerColor );
    painter.setPen( pen1 );

    int dx01, dy01, dx03, dy03;
    getGeom( dx01, dy01, dx03, dy03 );
    Tetragon quadIni = getTetragon( dx01, dy01, dx03, dy03 );
    Tetragon quad    = quadIni;

    //index is for iteration in the visible array
    int index = 0;
    //basically, we paint quad (the small square) and shift it further;
    //however, if it is not visible, we don't need to handle (shift and paint) it;
    //in skipped we count how many squares we did not paint (and thus did not shift quad),
    //thus before the next paint we can do shifting in one step
    int skipped = 0;
    //in shifted we count how often we shifted quad in a row of the second dimension;
    //before moving to the next row, we shift accordingly back to the beginning
    int shifted = 0;

    QPolygonF singlePoly;
    for ( int i = 0; i < quad.size(); i++ )
    {
        singlePoly.append( quad.at( i ) );
    }

    int dim1 = data->getDim( XX );
    int dim2 = data->getDim( YY );
    for ( int k1 = 0; k1 < dim1; k1++ )
    {
        for ( int k2 = 0; k2 < dim2; k2++, index++ )
        {
            //paint small square only if visible
            if ( !isPlaneAbove ? true : visible[ index ] )
            {
                //postponed shift for non-painted squares
                quad.translate( skipped * dx03, skipped * dy03 );
                shifted += skipped;
                skipped  = 1;
                //paint
                const QColor* color = data->getColor( k1, k2, currentPlaneIndex );
                painter.setBrush( *color );
                if ( lineType == NO_LINES )
                {
                    pen1.setColor( *color );
                    painter.setPen( pen1 );
                }
                if ( !data->hasUniqueIds() )
                {
                    /* multi process topology => don't draw lines for each element, but draw
                     * outline of each area consisting of multiple elements with same systemId */
                    int neighb = data->getNeighbors( k1, k2, currentPlaneIndex );
                    if ( neighb != 0 )
                    {
                        pen1.setColor( *color );
                        painter.setPen( pen1 );
                        painter.drawConvexPolygon( quad );
                        pen1.setColor( outerColor );
                        painter.setPen( pen1 );
                        for ( int i = 0; i < 4; i++ )   // draw Line if the neighbor has different systemID
                        {
                            bool insideArea = neighb & ( 1 << i );
                            if ( !insideArea )
                            {
                                if ( data->isSelected( k1, k2, currentPlaneIndex ) )
                                {
                                    selectedArea.append( QLineF( quad[ i ], quad[ ( i + 1 ) % 4 ] ) );
                                }
                                if ( lineType != NO_LINES )
                                {
                                    painter.drawLine( quad[ i ], quad[ ( i + 1 ) % 4 ] );
                                }
                            }
                        }
                    }
                    else
                    {
                        if ( lineType != NO_LINES )
                        {
                            pen1.setColor( outerColor );
                            painter.setPen( pen1 );
                        }
                        painter.drawConvexPolygon( quad );
                    }
                } // has unique id
                else
                {
                    painter.drawConvexPolygon( quad );
                }
            }
            else
            {
                skipped++;
            }
        }
        //prepare for next row:
        //shift one row further and undo shifting of previous row
        quad.translate( dx01 - shifted * dx03, dy01 - shifted * dy03 );

        skipped = 0;
        shifted = 0;
    }

    // mark merged elements by drawing separator lines
    if ( markMerged && ( lineType != NO_LINES ) )
    {
        painter.save();
        pen1.setColor( outerColor );
        pen1.setWidth( 1 );
        painter.setPen( pen1 );

        // draw horizontal
        int height = dy01 * dim1;
        int width  = dx01 * dim1;
        for ( int i = 0; i <= dim2; i += foldingDimensions[ YY ] )
        {
            int yy = ( int )pointsOnScreen[ 0 ].getY() + i * dy03;
            int xx = ( int )pointsOnScreen[ 0 ].getX() + i * dx03;
            painter.drawLine( xx, yy, xx + width, yy + height );
        }

        height = dy03 * dim2;
        width  = dx03 * dim2;
        for ( int i = 0; i <= dim1; i += foldingDimensions[ XX ] )
        {
            int yy = ( int )pointsOnScreen[ 0 ].getY() + i * dy01;
            int xx = ( int )pointsOnScreen[ 0 ].getX() + i * dx01;
            painter.drawLine( xx, yy, xx + width, yy + height );
        }
        painter.restore();
    }

    //if an item was marked, then mark the item and
    //on the 4 plane sides its position
    //such that the position is visible even if the plane is (partly) covered by another plane

    /*
                        __
       points[0] --------------------points[1]
     |                   S                      |
       points[3] --------------------points[2]
                        __
     */

    int enlarge = 2;
    for ( int k1 = 0; k1 < dim1; k1++ )
    {
        for ( int k2 = 0; k2 < dim2; k2++ )
        {
            if ( data->isSelected( k1, k2, currentPlaneIndex ) )
            {
                quad = quadIni;

                // selectionPen is for painting the selected items
                QPen selectionPen( Qt::magenta );
                selectionPen.setJoinStyle( Qt::MiterJoin );
                selectionPen.setMiterLimit( 0 );
                selectionPen.setWidth( 3 );

                if ( selectedArea.size() == 0 )
                {
                    int  len1    = ( quad.at( 0 ) - quad.at( 1 ) ).manhattanLength();
                    int  len2    = ( quad.at( 2 ) - quad.at( 3 ) ).manhattanLength();
                    bool isSmall = len1 < 8 || len2 < 8;

                    //paint the marked square for the selected item
                    quad.translate( k1 * dx01 + k2 * dx03, k1 * dy01 + k2 * dy03 );
                    if ( isSmall ) // draw small elements with thin lines
                    {
                        QPen pen( selectionPen );
                        pen.setWidth( 1 );
                        painter.setPen( pen );
                    }
                    else
                    {
                        enlargeQuad( quad, enlarge );
                        painter.setPen( selectionPen );
                    }
                    painter.setBrush( *data->getColor( k1, k2, currentPlaneIndex ) );
                    painter.drawConvexPolygon( quad );
                }
                else // selected element consists of several coordinates => draw outlines
                {
                    for ( int i = 0; i < selectedArea.size(); i++ )
                    {
                        painter.drawLine( selectedArea.at( i ) );
                    }
                }

                painter.setPen( selectionPen );
                //paint the lines on the plane sides for the selected item
                quad = quadIni;
                enlargeQuad( quad, enlarge + 1 );
                painter.drawLine( quad[ 0 ].x() + k1 * dx01,
                                  quad[ 0 ].y() + k1 * dy01,
                                  quad[ 1 ].x() + k1 * dx01,
                                  quad[ 1 ].y() + k1 * dy01 );
                painter.drawLine( quad[ 1 ].x() + ( dim1 - 1 ) * dx01 + k2 * dx03,
                                  quad[ 1 ].y() + ( dim1 - 1 ) * dy01 + k2 * dy03,
                                  quad[ 2 ].x() + ( dim1 - 1 ) * dx01 + k2 * dx03,
                                  quad[ 2 ].y() + ( dim1 - 1 ) * dy01 + k2 * dy03 );
                painter.drawLine( quad[ 2 ].x() + ( dim2 - 1 ) * dx03 + k1 * dx01,
                                  quad[ 2 ].y() + ( dim2 - 1 ) * dy03 + k1 * dy01,
                                  quad[ 3 ].x() + ( dim2 - 1 ) * dx03 + k1 * dx01,
                                  quad[ 3 ].y() + ( dim2 - 1 ) * dy03 + k1 * dy01 );
                painter.drawLine( quad[ 3 ].x() + k2 * dx03,
                                  quad[ 3 ].y() + k2 * dy03,
                                  quad[ 0 ].x() + k2 * dx03,
                                  quad[ 0 ].y() + k2 * dy03 );
                painter.setPen( pen1 );
            }  // if (selected_rects)
        }      // for (k2)
    }          // for (k1)
}
//end of paint()


//determines if the front of the plane is lower then the back part,
//in this case the deepest plane will be painted first,
//otherwise we start with the one on the top, because we see the block from the bottom
bool
Plane::isRising()
{
    double frontZ = points[ 0 ].getZ();
    double frontY = points[ 0 ].getY();

    for ( unsigned i = 1; i < 4; i++ )
    {
        if ( points[ i ].getZ() > frontZ )
        {
            frontZ = points[ i ].getZ();
            frontY = points[ i ].getY();
        }
    }
    return frontY > points[ 4 ].getY();
}


/**
 * @return length of the upper (and lower) line of each  plane
 */
int
Plane::baseline()
{
    return abs( pointsOnScreen[ 0 ].getX() - pointsOnScreen[ 1 ].getX() );
}

/**
 * returns the size of one plane without paddings as it is drawn on screen
 */
QSize
Plane::size() const
{
    int height = abs( pointsOnScreen[ 0 ].getY() - pointsOnScreen[ 3 ].getY() );

    const Point* p     = pointsOnScreen;
    double       x[]   = { p[ 0 ].getX(), p[ 1 ].getX(), p[ 2 ].getX(), p[ 3 ].getX() };
    int          maxX  = ( int )*std::max_element( x, x + 4 );
    int          minX  = ( int )*std::min_element( x, x + 4 );
    int          width = abs( maxX - minX );
    return QSize( width, height );
}

/**
 * return the size of the plane as it is calculated with floating point values. The
 * elements of the plane are drawn with integer sizes
 */
QSizeF
Plane::sizeF()
{
    //   p0   p1
    // p3  p2
    double height = abs( points[ 0 ].getY() - points[ 3 ].getY() );

    double x[]   = { points[ 0 ].getX(), points[ 1 ].getX(), points[ 2 ].getX(), points[ 3 ].getX() };
    int    maxX  = *std::max_element( x, x + 4 );
    int    minX  = *std::min_element( x, x + 4 );
    int    width = maxX - minX;

    return QSizeF( width, height );
}

//returns the most-top-most-left point of the plane
void
Plane::moveToTopLeftPoint()
{
    // get top left point
    int x = points[ 0 ].getX();
    int y = points[ 0 ].getY();

    double x2, y2;
    for ( unsigned int i = 1; i < 4; i++ )
    {
        x2 = points[ i ].getX();
        y2 = points[ i ].getY();
        if ( x2 < x )
        {
            x = x2;
        }
        if ( y2 < y )
        {
            y = y2;
        }
    }

    // scroll to top left point
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].xScroll( -x );
        points[ i ].yScroll( -y );
    }
}

//sets the line type;
//LineType is defined in constants.h;
//lineType stores if the lines between the system items are black, gray,
//white or if no lines should be drawn
void
Plane::setLineType( LineType lineType )
{
    this->lineType = lineType;
}

#ifdef unused
//mirror the plane on the X-axis
void
Plane::xMirror()
{
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].xMirror();
    }
    adjustToScreen();
}

//mirror the plane on the Y-axis
void
Plane::yMirror()
{
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].yMirror();
    }
    adjustToScreen();
}
//mirror the plane on the Z-axis
void
Plane::zMirror()
{
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].zMirror();
    }
    adjustToScreen();
}

//scroll the plane along the Z-axis
void
Plane::zScroll( double value )
{
    for ( unsigned i = 0; i < 5; i++ )
    {
        points[ i ].zScroll( value );
    }
}
#endif

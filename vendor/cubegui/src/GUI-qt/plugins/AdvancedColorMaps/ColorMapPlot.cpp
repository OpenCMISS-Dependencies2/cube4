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

#include <sstream>
#include <cmath>
#include <cstring>

#include <QPainter>
#include <QColor>
#include <QInputDialog>
#include <QToolTip>
#include <QtGlobal>
#include <QPainterPath>
#include "ColorMapPlot.h"
#include "ColorMapExtended.h"

using std::floor;
using std::pow;
using std::stringstream;
using std::max;
using std::min;
using std::abs;
using std::memset;
using std::memcpy;

const int     ColorMapPlot::    WIDTH                                                  = 100;
const double  ColorMapPlot:: WIDGET_WIDTH_BORDER                                       = 0.075;
const int     ColorMapPlot::    HEIGHT                                                 = 30;
const int     ColorMapPlot::    TEXT_WIDTH                                             = 10;
const int     ColorMapPlot::    TEXT_HEIGHT                                            = 15;
const int     ColorMapPlot::    TEXT_SPACING                                           = 5;
const int     ColorMapPlot::    SCALE_COUNT                                            = 5;
const int     ColorMapPlot::    SCALE_HEIGHT                                           = 5;
const int     ColorMapPlot::    TRIANGLE_HEIGHT                                        = 10;
const int     ColorMapPlot::    TRIANGLE_WIDTH                                         = 10;
const QString ColorMapPlot::CONTEXT_MENU_ITEMS[ ColorMapPlot::CONTEXT_MENU_ITEMS_NUM ] =
{ tr( "Invert" ), tr( "Adjust" ), tr( "Reset" ) };
const int ColorMapPlot::MARKER_TEXT_PRECISION     = 2;
const int ColorMapPlot::MARKER_TEXT_PRECISION_MAX = 5;

const double ColorMapPlot::MarkersPositions::DEFAULT_VALUES_MIN[ 2 ] = { 0.0, 0.0 };
const double ColorMapPlot::MarkersPositions::DEFAULT_VALUES_MAX[ 2 ] = { 100.0, 1.0 };
double       ColorMapPlot::MarkersPositions::      filterValues[ 6 ] = {
    DEFAULT_VALUES_MIN[ NUMERICAL ],  -1.0,  DEFAULT_VALUES_MAX[ NUMERICAL ],
    DEFAULT_VALUES_MIN[ PERCENTAGE ], -1.0,  DEFAULT_VALUES_MAX[ PERCENTAGE ]
};


ColorMapPlot::ColorMapPlot( ColorMapExtended&               colormap,
                            MarkersPositions::MarkersMethod method,
                            int                             _width,
                            int                             _height ) :
    currentColorMap( colormap ),
    markerValues( method ),
    startingPosition( -1.0 ),

    enabledMiddleMark( true ),
    currentlyChanging( MarkersPositions::MIDDLE_POINT ),
    doubleClickedColor( COLOR_VALUE_OUT_OF_RANGE ),
    doubleClickedMarker( MarkersPositions::MIDDLE_POINT ),
    currentlyDoubleClicked( DC_COLOR )
{
    enabledColorChange[ 0 ] = true;
    enabledColorChange[ 1 ] =  true;
    enabledColorChange[ 2 ] =  true;


    currentPositions[ 0 ] = 0.0;
    currentPositions[ 1 ] = 0.5;
    currentPositions[ 2 ] = 1.0;


    GUI_markerValuePosition[ 0 ][ 0 ] = 0;
    GUI_markerValuePosition[ 0 ][ 1 ] = 0;
    GUI_markerValuePosition[ 0 ][ 2 ] = 0;
    GUI_markerValuePosition[ 1 ][ 0 ] = 0;
    GUI_markerValuePosition[ 1 ][ 1 ] = 0;
    GUI_markerValuePosition[ 1 ][ 2 ] = 0;
    GUI_markerValuePosition[ 2 ][ 0 ] = 0;
    GUI_markerValuePosition[ 2 ][ 1 ] = 0;
    GUI_markerValuePosition[ 2 ][ 2 ] = 0;

    CONTEXT_MENU_HANDLERS[ 0 ] = &ColorMapPlot::invertPlot;
    CONTEXT_MENU_HANDLERS[ 1 ] = &ColorMapPlot::adjustPlot;
    CONTEXT_MENU_HANDLERS[ 2 ] = &ColorMapPlot::resetPlot;


    // height is
    setMinimumHeight( _height + 2 * TEXT_HEIGHT + 2 * TRIANGLE_HEIGHT );
    setMaximumHeight( minimumHeight() );
    setMinimumWidth( _width + ( int )floor( _width * 2 * WIDGET_WIDTH_BORDER ) );
    // vertical expand, horizontal constant
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );


    /**
     * Context menu
     */
    setContextMenuPolicy( Qt::CustomContextMenu );
    for ( int i = 0; i < CONTEXT_MENU_ITEMS_NUM; ++i )
    {
        contextMenu.addAction( CONTEXT_MENU_ITEMS[ i ] );
    }
    QObject::connect( this, SIGNAL( customContextMenuRequested( const QPoint & ) ),
                      this, SLOT( showContextMenu( const QPoint & ) ) );

    QObject::connect( &colorPicker, SIGNAL( colorSelected( const QColor & ) ),
                      this, SLOT( selectedColor( const QColor & ) ) );

    // try to fix this bug in Mac OS
    // https://bugreports.qt.io/browse/QTBUG-31819
        #if QT_VERSION < 0x050101
    colorPicker.setOption( QColorDialog::DontUseNativeDialog, true );
        #endif

    /**
     * caching - copy values to cache
     */
    applyChanges();
}

bool
ColorMapPlot::event( QEvent* event )
{
    /**
     * Intercept tooltips.
     */
    if ( event->type() == QEvent::ToolTip )
    {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>( event );
        int         index     = findMarkerValueObject( helpEvent->pos().x(), helpEvent->pos().y() );

        if ( index != -1 )
        {
            stringstream ss;
            ss << markerValues.getMarker( static_cast<MarkersPositions::Position>( index ) );
            QToolTip::showText( helpEvent->globalPos(), QString::fromStdString( ss.str() ) );
            event->accept();
        }
        else
        {
            QToolTip::hideText();
            event->ignore();
        }
        return true;
    }

    return QWidget::event( event );
}

void
ColorMapPlot::paintEvent( QPaintEvent* )
{
    QPainter painter( this );
    /**
     * Set used variables.
     */
    int    width        = getPlotWidth();
    int    border_width = floor( width * WIDGET_WIDTH_BORDER );
    int    height       = getPlotHeight();
    double currentPosition;
    double plot_values_width = getMarkersRange();
    /**
     * Triangle pointing to bottom.
     */
    QPolygon triangle( 4 );
    triangle << QPoint( -TRIANGLE_WIDTH / 2, 0 ) << QPoint( TRIANGLE_WIDTH / 2, 0 ) <<
        QPoint( 0, TRIANGLE_HEIGHT ) << QPoint( -TRIANGLE_WIDTH / 2, 0 );
    QPolygon triangleInversed( 4 );
    triangleInversed << QPoint( 0, 0 ) << QPoint( TRIANGLE_WIDTH / 2, TRIANGLE_HEIGHT ) <<
        QPoint( -TRIANGLE_WIDTH / 2, TRIANGLE_HEIGHT ) << QPoint( 0, 0 );

    stringstream ss;
    int          precision;
    if ( markerValues.method == MarkersPositions::PERCENTAGE )
    {
        /**
         * add one digit with every order of magnitude, e.g
         * for the distance of 0.1 should display middle marker as 0.047
         * for distance of 0.009 should display middle marker as 0.0012
         */
        precision = MARKER_TEXT_PRECISION - round( log10( plot_values_width ) );
        precision = min( precision, MARKER_TEXT_PRECISION_MAX );
    }
    else
    {
        precision = 1;
    }
    ss.precision( precision );
    ss.setf( stringstream::fixed );
    string str;

    int yPos       = getPlotY();
    int y_values[] = { yPos, yPos + height };
    for ( int i = 0; i < 2; ++i )
    {
        painter.drawLine( border_width, y_values[ i ], border_width + width - 1, y_values[ i ] );
        // then scale
        for ( int j = 0; j < SCALE_COUNT; ++j )
        {
            // SCALE_COUNT points from [0, width - 1]
            double xPos = border_width + ( width - 1 ) * ( ( double )j ) / ( SCALE_COUNT - 1 );
            painter.drawLine( xPos,   y_values[ i ] - pow( ( double )-1, ( double )i ) * SCALE_HEIGHT,   xPos, y_values[ i ] );
        }
    }

    /**
     * Plot between scale lines
     */
    double activePlotWidth = width * ( currentPositions[ MarkersPositions::END_POSITION ]
                                       - currentPositions[ MarkersPositions::START_POSITION ] );
    double step = plot_values_width / activePlotWidth;
    /**
     * Starting value:
     * start_marker_value - number_of_steps_till_start * step_value
     */
    double start = markerValues.getMarker( MarkersPositions::START_POSITION )
                   - ( currentPositions[ MarkersPositions::START_POSITION ] * width ) * step;
    for ( int i = 0; i < width; ++i )
    {
        currentPosition = start + step * ( i + 1 );
        painter.fillRect( border_width + i, y_values[ 0 ] + 1, 1,                 y_values[ 1 ] - y_values[ 0 ] - 1,
                          currentColorMap.getColor( currentPosition, 0.0,  1.0,                            false ) );
    }

    if ( enabledMiddleMark )
    {
        /**
         * First: middle position value at height 0
         */
        ss.str( "" );
        ss << markerValues.getMarker( MarkersPositions::MIDDLE_POINT );
        str = ss.str();
        int xPos = border_width + ( width - 1 ) * currentPositions[ MarkersPositions::MIDDLE_POINT ]
                   - str.length() * TEXT_WIDTH * 0.5;
        GUI_markerValuePosition[ MarkersPositions::MIDDLE_POINT ][ 0 ] = xPos;
        painter.drawText( xPos,       0,
                          str.length() * TEXT_WIDTH, TEXT_HEIGHT,
                          Qt::AlignCenter | Qt::AlignTop, QString::fromStdString( str ) );

        /**
         * Triangle indicator at height TEXT_WIDTH;
         */
        xPos = border_width + ( width - 1 ) * currentPositions[ MarkersPositions::MIDDLE_POINT ];
        yPos = TEXT_HEIGHT;
        triangle.translate( xPos,     yPos );
        painter.drawPolygon( triangle );
        QPainterPath tmpPath;
        tmpPath.addPolygon( triangle );
        painter.fillPath( tmpPath,    QBrush( Qt::black ) );

        /**
         * Line from triangle to end of plot.
         */
        int yPos = getPlotY();
        painter.drawLine( xPos,       yPos, xPos,                           yPos + height );

        GUI_markerValuePosition[ MarkersPositions::MIDDLE_POINT ][ 1 ] = 0;
        GUI_markerValuePosition[ MarkersPositions::MIDDLE_POINT ][ 3 ] = yPos;

        // left border can be set by triangle or text
        if ( str.length() * TEXT_WIDTH * 0.5 > TRIANGLE_WIDTH / 2 )
        {
            GUI_markerValuePosition[ MarkersPositions::MIDDLE_POINT ][ 0 ] = xPos - str.length() * TEXT_WIDTH * 0.5;
            GUI_markerValuePosition[ MarkersPositions::MIDDLE_POINT ][ 2 ] = xPos + str.length() * TEXT_WIDTH * 0.5;
        }
        else
        {
            GUI_markerValuePosition[ MarkersPositions::MIDDLE_POINT ][ 0 ] = xPos - TRIANGLE_WIDTH * 0.5;
            GUI_markerValuePosition[ MarkersPositions::MIDDLE_POINT ][ 2 ] = xPos + TRIANGLE_WIDTH * 0.5;
        }
    }

    /**
     * Pointers at the bottom + values
     */

    ss.str( "" );
    ss << markerValues.getMarker( MarkersPositions::START_POSITION );
    str = ss.str();
    ss.str( "" );
    ss << markerValues.getMarker( MarkersPositions::END_POSITION );
    string str_values[] = { str,      ss.str() };
    int    plot_width   = width - 1;
    int    x_values[]   = { border_width + ( int )floor(
                                plot_width * currentPositions[ MarkersPositions::START_POSITION ] ),
                            border_width + ( int )floor( plot_width * currentPositions[ MarkersPositions::END_POSITION ] ) };
    yPos = y_values[ 1 ];
    for ( int i = 0; i < 2; ++i )
    {
        /**
         * triangle with filling
         */
        triangleInversed.translate( x_values[ i ], yPos );
        painter.drawPolygon( triangleInversed );
        QPainterPath tmpPath;
        tmpPath.addPolygon( triangleInversed );
        painter.fillPath( tmpPath, QBrush( Qt::black ) );
        triangleInversed.translate( -x_values[ i ], -yPos );

        /**
         * Text
         */
        int xPos = x_values[ i ] - str_values[ i ].length() * TEXT_WIDTH * 0.5;
        /**
         * For many digits we can start writing outside the widget
         */
        xPos = max( xPos, 0 );
        xPos = min( xPos, this->width() );
        painter.drawText( xPos, yPos + TRIANGLE_HEIGHT,
                          TEXT_WIDTH * str_values[ i ].length(), TEXT_HEIGHT,
                          Qt::AlignCenter | Qt::AlignTop, QString::fromStdString( str_values[ i ] ) );
        /**
         * Line
         */
        painter.drawLine( x_values[ i ], yPos, x_values[ i ], yPos - height );

        int pos = i == 0 ? MarkersPositions::START_POSITION : MarkersPositions::END_POSITION;
        GUI_markerValuePosition[ pos ][ 1 ] = yPos;
        GUI_markerValuePosition[ pos ][ 3 ] = yPos + TRIANGLE_HEIGHT + TEXT_HEIGHT;

        // left border can be set by triangle or text
        if ( str_values[ i ].length() * TEXT_WIDTH * 0.5 > TRIANGLE_WIDTH / 2 )
        {
            GUI_markerValuePosition[ pos ][ 0 ] = x_values[ i ] - str_values[ i ].length() * TEXT_WIDTH * 0.5;
            GUI_markerValuePosition[ pos ][ 2 ] = x_values[ i ] + str_values[ i ].length() * TEXT_WIDTH * 0.5;
        }
        else
        {
            GUI_markerValuePosition[ pos ][ 0 ] = x_values[ i ] - TRIANGLE_WIDTH * 0.5;
            GUI_markerValuePosition[ pos ][ 2 ] = x_values[ i ] + TRIANGLE_WIDTH * 0.5;
        }
    }
}

void
ColorMapPlot::mousePressEvent( QMouseEvent* event )
{
    // the event should not be handled elsewhere
    event->accept();
    startingPosition = event->pos().x();
    int width = getPlotWidth();
    startingPosition -= getPlotX();
    int min = 100000.0;

    MarkersPositions::Position min_loc = MarkersPositions::START_POSITION;
    // find closest indicator
    for ( int i = MarkersPositions::START_POSITION; i <= MarkersPositions::END_POSITION; ++i )
    {
        if ( i != MarkersPositions::MIDDLE_POINT || enabledMiddleMark )
        {
            int val = abs( currentPositions[ i ] * width - startingPosition );
            if ( val < min )
            {
                min     = val;
                min_loc = static_cast<MarkersPositions::Position>( i );
            }
        }
    }

    currentlyChanging = min_loc;
}

void
ColorMapPlot::mouseReleaseEvent( QMouseEvent* event )
{
    event->accept();
    startingPosition = -1.0;
}

void
ColorMapPlot::mouseMoveEvent( QMouseEvent* event )
{
    event->accept();
    int xPos  = event->pos().x() - getPlotX();
    int width = getPlotWidth();

    if ( startingPosition >= 0.0 && xPos >= 0 && xPos <= width )
    {
        /**
         * Compute movement of position
         * If moving to left, then < 0. Otherwise > 0
         */
        double difference = ( ( double )xPos - startingPosition ) / width;

        currentPositions[ currentlyChanging ] += difference;
        markerValues.applyDifference( difference,
                                      currentPositions[ MarkersPositions::END_POSITION ] -
                                      currentPositions[ MarkersPositions::START_POSITION ],
                                      currentlyChanging );

        currentPositions[ currentlyChanging ] = max( currentPositions[ currentlyChanging ], 0.0 );
        currentPositions[ currentlyChanging ] = min( currentPositions[ currentlyChanging ], 1.0 );

        normalize( currentlyChanging );

        emit markerChanged( currentlyChanging, currentPositions[ currentlyChanging ] );

        startingPosition = xPos;
        update();
    }
}

void
ColorMapPlot::mouseDoubleClickEvent( QMouseEvent* event )
{
    // locate position
    event->accept();
    int width = getPlotWidth();

    int x = event->pos().x();
    int y = event->pos().y();
    // inside plot
    if ( y >= getPlotY() && y < ( getPlotY() + getPlotHeight() ) )
    {
        double xPos = x / ( ( double )width );

        if ( !( xPos >= 0.0 && xPos <= 1.0 ) )
        {
            return;
        }

        if ( xPos <= currentPositions[ MarkersPositions::START_POSITION ]  ||
             xPos > currentPositions[ MarkersPositions::END_POSITION ] )
        {
            doubleClickedColor = COLOR_VALUE_OUT_OF_RANGE;
        }
        else if ( !enabledMiddleMark ||
                  xPos <= currentPositions[ MarkersPositions::MIDDLE_POINT ] )
        {
            doubleClickedColor = COLOR_FIRST;
        }
        else
        {
            doubleClickedColor = COLOR_SECOND;
        }
        currentlyDoubleClicked = DC_COLOR;
        if ( enabledColorChange[ doubleClickedColor ] )
        {
            colorPicker.setVisible( true );
        }
    }
    else
    {
        int index = findMarkerValueObject( x, y );

        if ( index != -1 && ( enabledMiddleMark || index != MarkersPositions::MIDDLE_POINT ) )
        {
            doubleClickedMarker    = static_cast<MarkersPositions::Position>( index );
            currentlyDoubleClicked = DC_MARKER;
            bool   ok;
            double val = QInputDialog::getDouble( this, tr( "Set marker value" ), tr( "Marker value" ),
                                                  markerValues.getMarker( doubleClickedMarker ), 0.0, 1.0, 12, &ok );
            if ( ok )
            {
                markerValues.setMarker( val, doubleClickedMarker );

                repositionMiddleMarker();

                update();

                emit markerChanged( doubleClickedMarker, val );
            }
        }
    }
}

void
ColorMapPlot::selectedColor( const QColor& color )
{
    emit colorChanged( doubleClickedColor, color );
}

void
ColorMapPlot::showContextMenu( const QPoint& pos )
{
    QPoint   globalPos = mapToGlobal( pos );
    QAction* selected  = contextMenu.exec( globalPos );

    if ( selected )
    {
        for ( int i = 0; i < CONTEXT_MENU_ITEMS_NUM; ++i )
        {
            if ( selected->text().contains( CONTEXT_MENU_ITEMS[ i ] ) )
            {
                // call the appropriate event handler
                ( this->*CONTEXT_MENU_HANDLERS[ i ] )();
            }
        }
    }
}

void
ColorMapPlot::normalize( MarkersPositions::Position pos )
{
    /**
     * Normalize: start <= middle <= end
     */
    if ( pos == MarkersPositions::START_POSITION )
    {
        normalizeBeginMarker();
    }
    else if ( pos == MarkersPositions::END_POSITION )
    {
        normalizeEndMarker();
    }
    else
    {
        normalizeMiddleMarker();
    }
}

void
ColorMapPlot::normalizeBeginMarker()
{
    currentPositions[ currentlyChanging ] =
        min( currentPositions[ currentlyChanging ], currentPositions[ MarkersPositions::MIDDLE_POINT ] );
}

void
ColorMapPlot::normalizeEndMarker()
{
    currentPositions[ currentlyChanging ] =
        max( currentPositions[ currentlyChanging ], currentPositions[ MarkersPositions::MIDDLE_POINT ] );
}

void
ColorMapPlot::normalizeMiddleMarker()
{
    currentPositions[ MarkersPositions::MIDDLE_POINT ] =
        max( currentPositions[ MarkersPositions::MIDDLE_POINT ],
             currentPositions[ MarkersPositions::START_POSITION ] );
    currentPositions[ MarkersPositions::MIDDLE_POINT ] =
        min( currentPositions[ MarkersPositions::MIDDLE_POINT ],
             currentPositions[ MarkersPositions::END_POSITION ] );
}

void
ColorMapPlot::setEnabledColorChange( Color col, bool value )
{
    enabledColorChange[ col ] = value;
}

void
ColorMapPlot::setEnableMiddleMark( bool val )
{
    enabledMiddleMark = val;
}

ColorMapPlot::MarkersPositions*
ColorMapPlot::getMarkersPositionsObject()
{
    return &markerValues;
}

int
ColorMapPlot::getPlotX()
{
    // horizontal position:
    // border | plot | border
    int w = this->width();
    return w * WIDGET_WIDTH_BORDER;
}

int
ColorMapPlot::getPlotWidth()
{
    // horizontal position:
    // border | plot | border
    int w = this->width();
    return w - 2 * floor( w * WIDGET_WIDTH_BORDER );
}

int
ColorMapPlot::getPlotY()
{
    // vertical position:
    // marker (triangle + text) | plot
    return TEXT_HEIGHT + TRIANGLE_HEIGHT;
}

int
ColorMapPlot::getPlotHeight()
{
    // horizontal position:
    // marker (as in function getPlotY) | plot | marker
    return this->height() - 2 * ( TEXT_HEIGHT + TRIANGLE_HEIGHT );
}

double
ColorMapPlot::getMarkersRange()
{
    return markerValues.getMarker( MarkersPositions::END_POSITION ) -
           markerValues.getMarker( MarkersPositions::START_POSITION );
}

void
ColorMapPlot::adjustPlot()
{
    currentPositions[ MarkersPositions::START_POSITION ] = 0.0;
    currentPositions[ MarkersPositions::END_POSITION ]   = 1.0;

    repositionMiddleMarker();

    update();
}

void
ColorMapPlot::resetPlot()
{
    currentPositions[ MarkersPositions::START_POSITION ] = 0.0;
    currentPositions[ MarkersPositions::MIDDLE_POINT ]   = 0.5;
    currentPositions[ MarkersPositions::END_POSITION ]   = 1.0;

    markerValues.resetValues();
    update();
}

void
ColorMapPlot::invertPlot()
{
    currentColorMap.invertColorMap();
    update();
}


int
ColorMapPlot::findMarkerValueObject( int x, int y )
{
    int index = -1;

    for ( int i = MarkersPositions::START_POSITION; i <= MarkersPositions::END_POSITION; ++i )
    {
        // check if the point is inside the rectangle
        if ( x >= GUI_markerValuePosition[ i ][ 0 ] && x <= GUI_markerValuePosition[ i ][ 2 ] &&
             y >= GUI_markerValuePosition[ i ][ 1 ] && y <= GUI_markerValuePosition[ i ][ 3 ] )
        {
            /**
             * Special case - the begin and end marker are too close.
             * Find the distance between middle points.
             */
            if ( index != -1 )
            {
                double diff = abs( x - ( GUI_markerValuePosition[ i ][ 0 ] +
                                         GUI_markerValuePosition[ i ][ 2 ] ) * 0.5 );
                double diff_prev = abs( x -
                                        ( GUI_markerValuePosition[ doubleClickedMarker ][ 0 ]
                                          + GUI_markerValuePosition[ doubleClickedMarker ][ 2 ] ) * 0.5 );
                if ( diff < diff_prev )
                {
                    index = i;
                }
            }
            else
            {
                index = i;
            }
        }
    }

    return index;
}

void
ColorMapPlot::repositionMiddleMarker()
{
    double marker_width = currentPositions[ MarkersPositions::END_POSITION ]
                          - currentPositions[ MarkersPositions::START_POSITION ];
    double marker_diff     = getMarkersRange();
    double marker_relative = markerValues.getMarker( MarkersPositions::MIDDLE_POINT )
                             - markerValues.getMarker( MarkersPositions::START_POSITION );
    /**
     * set position of middle marker
     * start_position + plot_width * middle_value/values_width
     */
    currentPositions[ MarkersPositions::MIDDLE_POINT ] =
        currentPositions[ MarkersPositions::START_POSITION ]
        + marker_width * marker_relative / marker_diff;
}

/**
 * MarkersPositions inner substructure.
 */

ColorMapPlot::MarkersPositions::MarkersPositions( MarkersMethod _method ) :
    method( _method )
{
    middleMarker = 0.5 * getMarker( END_POSITION ) + 0.5 * getMarker( START_POSITION );
}

double
ColorMapPlot::MarkersPositions::adjustFiltering( double min_, double max_, bool ifEnabled,
                                                 FilterPosition pos ) const
{
    double filter_val;

    if ( pos == FILTER_MAX )
    {
        filter_val = getMarker( END_POSITION );
        filter_val = method == PERCENTAGE ? min_ + ( max_ - min_ ) * filter_val
                     : filter_val;
        return ifEnabled ? min( max_, filter_val ) : max_;
    }
    else
    {
        filter_val = getMarker( START_POSITION );
        filter_val = method == PERCENTAGE ? min_ + ( max_ - min_ ) * filter_val
                     : filter_val;
        return ifEnabled ? max( min_, filter_val ) : min_;
    }
}

double
ColorMapPlot::MarkersPositions::getMiddleMarker( double min, double max ) const
{
    if ( method == PERCENTAGE )
    {
        return min + middleMarker * ( max - min );
    }
    else
    {
        return middleMarker;
    }
}

double
ColorMapPlot::MarkersPositions::getMarker( Position pos ) const
{
    if ( pos == MIDDLE_POINT )
    {
        return middleMarker;
    }
    else
    {
        int offset = method == PERCENTAGE ? 3 : 0;
        return filterValues[ offset + pos ];
    }
}

double
ColorMapPlot::MarkersPositions::getFilteringValue( FilterPosition pos ) const
{
    int offset = method == PERCENTAGE ? 3 : 0;
    return filterValues[ offset + pos ];
}

void
ColorMapPlot::MarkersPositions::setMarker( double val, Position pos )
{
    if ( pos == MIDDLE_POINT )
    {
        middleMarker = val;
        // normalize
        middleMarker = max( middleMarker, getMarker( START_POSITION ) );
        middleMarker = min( middleMarker, getMarker( END_POSITION ) );
    }
    else
    {
        int offset = method == PERCENTAGE ? 3 : 0;
        filterValues[ offset + pos ] = val;

        if ( pos == END_POSITION )
        {
            // normalize
            val = max( filterValues[ offset + pos ], getMarker( MIDDLE_POINT ) );
            if ( method == PERCENTAGE )
            {
                val = min( val, 1.0 );
            }
            filterValues[ offset + pos ] = val;
        }
        else
        {
            val = min( filterValues[ offset + pos ], getMarker( MIDDLE_POINT ) );
            if ( method == PERCENTAGE )
            {
                val = max( val, 0.0 );
            }
            filterValues[ offset + pos ] = val;
        }
    }
}

void
ColorMapPlot::MarkersPositions::applyDifference( double diff, double posDiff, Position pos )
{
    double modifier = ( getMarker( MarkersPositions::END_POSITION )
                        - getMarker( MarkersPositions::START_POSITION ) ) / posDiff;
    setMarker( getMarker( pos ) + diff * modifier, pos );
}

void
ColorMapPlot::MarkersPositions::resetValues()
{
    int offset = 0;
    if ( method == PERCENTAGE )
    {
        offset = 3;
    }
    filterValues[ offset ]     = DEFAULT_VALUES_MIN[ method ];
    filterValues[ offset + 2 ] = DEFAULT_VALUES_MAX[ method ];

    middleMarker = 0.5 * getMarker( END_POSITION ) + 0.5 * getMarker( START_POSITION );
}


void
ColorMapPlot::saveMiddleMarker( QSettings& settings, QString prefix )
{
    settings.setValue( QString( "%1_middleMarker" ).arg( prefix ), markerValues.getMarker( MarkersPositions::MIDDLE_POINT ) );
}

void
ColorMapPlot::loadMiddleMarker( QSettings& settings, QString prefix )
{
    // not found - use default value
    double def = 0.5 * markerValues.getMarker( MarkersPositions::END_POSITION ) +
                 0.5 * markerValues.getMarker( MarkersPositions::START_POSITION );
    bool   ok;
    double val = settings.value( QString( "%1_middleMarker" ).arg( prefix ), def ).toDouble( &ok );
    if ( !ok )
    {
        val = def;
    }
    markerValues.setMarker( val, MarkersPositions::MIDDLE_POINT );
    normalizeMiddleMarker();
    repositionMiddleMarker();
}


void
ColorMapPlot::revertChanges()
{
    memcpy( markerValues.filterValues, cachedFilteringValues, sizeof( double ) * 6 );
    memcpy( currentPositions, cachedCurrentPositions, sizeof( double ) * 3 );
    markerValues.middleMarker = cachedMiddleMarkerPos;
}

void
ColorMapPlot::applyChanges()
{
    memcpy( cachedFilteringValues, markerValues.filterValues, sizeof( double ) * 6 );
    memcpy( cachedCurrentPositions, currentPositions, sizeof( double ) * 3 );
    cachedMiddleMarkerPos = markerValues.middleMarker;
}

void
ColorMapPlot::loadSettings( QSettings& settings )
{
    bool flag = true;
    // read min
    for ( int i = 0; i < 4; i += 3 )
    {
        MarkersPositions::filterValues[ i ] = settings.value( QString( "Filtering_Values_%1" ).arg( i ), MarkersPositions::DEFAULT_VALUES_MIN[ i % 2 ] ).toDouble( &flag );

        // ill-formed double
        if ( !flag )
        {
            MarkersPositions::filterValues[ i ] = MarkersPositions::DEFAULT_VALUES_MIN[ i % 2 ];
        }
    }

    // read max
    for ( int i = 2; i < 6; i += 3 )
    {
        MarkersPositions::filterValues[ i ] = settings.value( QString( "Filtering_Values_%1" ).arg( i ), MarkersPositions::DEFAULT_VALUES_MAX[ i % 2 ] ).toDouble();

        // ill-formed double
        if ( !flag )
        {
            MarkersPositions::filterValues[ i ] = MarkersPositions::DEFAULT_VALUES_MAX[ i % 2 ];
        }
    }
}

void
ColorMapPlot::saveSettings( QSettings& settings )
{
    for ( int i = 0; i < 6; ++i )
    {
        if ( i == 1 || i == 4 )
        {
            continue;
        }
        settings.setValue( QString( "Filtering_Values_%1" ).arg( i ), MarkersPositions::filterValues[ i ] );
    }
}

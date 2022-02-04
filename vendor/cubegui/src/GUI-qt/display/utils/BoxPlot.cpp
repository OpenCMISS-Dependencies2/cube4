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

#include "BoxPlot.h"
#include "Pretty.h"
#include "TableWidget.h"
#include "StatisticalInformation.h"
#include "Globals.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

#include <QFontMetrics>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QDialog>
#include <QString>
#include <QPen>
#include <QDebug>
#include <math.h>

using namespace std;
using namespace cubegui;

namespace
{
class HeightCalculator
{
public:
    HeightCalculator( int    offset,
                      int    height,
                      double max,
                      double min )
        : lowOffset( offset ), clientHeight( height ),
        maxValue( max ), minValue( min )
    {
    }
    int
    operator()( double val ) const
    {
        return static_cast<int>( lowOffset - clientHeight /
                                 ( maxValue - minValue ) * ( Clip( val ) - minValue ) );
    }
    enum Position { Inside, Above, Below };
    Position
    CalculatePosition( double value ) const
    {
        if ( value > maxValue )
        {
            return Above;
        }
        if ( value < minValue )
        {
            return Below;
        }
        return Inside;
    }
    double
    Clip( double value ) const
    {
        if ( value > maxValue )
        {
            return maxValue;
        }
        if ( value < minValue )
        {
            return minValue;
        }
        return value;
    }
private:
    int    lowOffset, clientHeight;
    double maxValue, minValue;
};
}

BoxPlot::BoxPlot( QWidget* parent )
    : QWidget( parent ), firstPosition( 0, 0 )
{
    tooltip      = 0;
    minimumValue = start;
    maximumValue = 0.0;
    start        = 0.;
    borderPad    = 10;
    tickWidth    = 3;

    // setWindowTitle("BoxPlot");
    resize( 300, 450 );
    items.resize( 0 );
    ResetMinMaxValues();
    if ( parent != 0 )
    {
        parent->adjustSize();
    }
}

void
BoxPlot::Add( StatisticalInformation const& theStatisticalInformation )
{
    items.push_back( theStatisticalInformation );
    resize( 150 + 80 * items.size(), 450 );
    if ( parentWidget() != 0 )
    {
        parentWidget()->adjustSize();
    }
    ResetView();
}

/**
   set the data of a boxplot which consists only of one StatisticalInformation
   existing StatisticalInformations will be deleted
   @param theStatisticalInformation values of the boxplot
   @param abs the corresponding absoulte values

 */
void
BoxPlot::Set( StatisticalInformation const& theStatisticalInformation,  StatisticalInformation const& absStatisticalInformation, double startValue )
{
    start = startValue;
    items.clear();
    items.push_back( theStatisticalInformation );
    absolute = absStatisticalInformation;
    ResetView();
}

void
BoxPlot::Reset()
{
    items.clear();
    ResetView();
}

void
BoxPlot::ResetMinMaxValues()
{
    minimumValue = start;
    maximumValue = 0.0;
    if ( items.size() != 0 )
    {
        double maxVal = numeric_limits<double>::min();
        for ( vector<StatisticalInformation>::const_iterator it = items.begin(); it != items.end(); ++it )
        {
            if ( it->getMaximum() > maxVal )
            {
                maxVal = it->getMaximum();
            }
        }
        maximumValue = maxVal;
    }

    setAxisScale( minimumValue, maximumValue );
}

void
BoxPlot::setAxisScale( double min, double max )
{
    if ( fabs( max - min ) < 1e-8 )
    {
        max = min + 0.5; // create valid box
        min = max - 1.0;
    }

    // calculate y axis scale
    yAxisScale.resize( 0 );
    Pretty( min, max, numberOfAxisEntries, yAxisScale );
    yAxisMin = *yAxisScale.begin();
    yAxisMax = *yAxisScale.rbegin();
}

void
BoxPlot::ResetView()
{
    ResetMinMaxValues();
    update();
}


int
BoxPlot::valueToYCoord( double value )
{
    int upperOffset  = GetUpperBorderHeight();
    int clientHeight = height() - GetLowerBorderHeight() - upperOffset;

    return static_cast<int>( clientHeight + upperOffset
                             - clientHeight / ( yAxisMax - yAxisMin ) * ( value - yAxisMin ) );
}

// Calculates the value on the y-axis which belongs to a height of koord pixels on the axis
double
BoxPlot::GetValue( int koord ) const
{
    int lowOffset    = GetCanvasHeight() - GetLowerBorderHeight();
    int clientHeight = lowOffset - GetUpperBorderHeight();
    return ( lowOffset - koord ) * ( yAxisMax - yAxisMin )
           / clientHeight + yAxisMin;
}

void
BoxPlot::paintEvent( QPaintEvent* )
{
    QPainter painter( this );
    if ( items.size() == 0 )
    {
        painter.drawText( rect(), Qt::AlignCenter, tr( "No values to display." ) );
        return;
    }

    DrawLeftLegend( painter );
    DrawLowerLegend( painter );
    DrawRightLegend( painter );
    DrawBoxPlots( painter );
    painter.drawRect( GetLeftBorderWidth(), GetUpperBorderHeight(),
                      GetCanvasWidth() - GetRightBorderWidth() - GetLeftBorderWidth(),
                      GetCanvasHeight() - GetLowerBorderHeight() - GetUpperBorderHeight() );
}

void
BoxPlot::mousePressEvent( QMouseEvent* event )
{
    if ( items.size() == 0 )
    {
        return;
    }
    else
    {
        firstPosition.first  = event->pos().x();
        firstPosition.second = event->pos().y();

        if ( event->button() == Qt::RightButton )
        {
            StatisticalInformation const* clicked = GetClickedStatisticalInformation( event );
            if ( clicked != 0 )
            {
                ShowStatisticsTooltip( clicked );
            }
        }
    }
}

#define EPS 10
void
BoxPlot::mouseReleaseEvent( QMouseEvent* event )
{
    switch ( event->button() )
    {
        case Qt::LeftButton:
            if ( items.size() == 0 )
            {
                return;
            }
            else
            {
                double newMaxVal, newMinVal;
                if ( firstPosition.second < event->pos().y() )
                {
                    newMaxVal = GetValue( firstPosition.second );
                    newMinVal = GetValue( event->pos().y() );
                }
                else
                {
                    newMinVal = GetValue( firstPosition.second );
                    newMaxVal = GetValue( event->pos().y() );
                }
                if ( event->pos().y() - firstPosition.second < EPS )
                {
                    StatisticalInformation const* clicked = GetClickedStatisticalInformation( event );
                    if ( clicked != 0 )
                    {
                        ShowStatistics( clicked );
                    }
                }
                else
                {
                    SetMinMaxValues( newMinVal, newMaxVal );
                }
            }
            break;
        case Qt::MiddleButton:
            ResetView();
            break;
        default:
            break;
    }
    if ( tooltip )
    {
        tooltip->close();
    }
}

void
BoxPlot::SetMinMaxValues( double newMinVal, double newMaxVal )
{
    if ( newMaxVal < maximumValue )
    {
        maximumValue = newMaxVal;
    }
    if ( newMinVal > minimumValue )
    {
        minimumValue = newMinVal;
    }
    setAxisScale( newMinVal, newMaxVal );
    update();
}

int
BoxPlot::GetCanvasWidth() const
{
    return width();
}

int
BoxPlot::GetCanvasHeight() const
{
    return height();
}

int
BoxPlot::GetTextWidth( QString const& text ) const
{
    QFontMetrics fm( font() );
    return fm.boundingRect( text ).width();
}

int
BoxPlot::GetTextWidth( string const& text ) const
{
    return GetTextWidth( QString::fromStdString( text ) );
}

int
BoxPlot::GetTextHeight( QString const& ) const
{
    QFontMetrics fm( font() );
    return 3 * fm.ascent() / 2;
}

int
BoxPlot::GetTextHeight( string const& text ) const
{
    return GetTextHeight( QString::fromStdString( text ) );
}

int
BoxPlot::GetLeftBorderWidth() const
{
    double value      = GetMaxValue();
    double labelWidth = GetTextWidth( Globals::formatNumber( value, value, true ) );

    return labelWidth + 3 * tickWidth + borderPad;
}

/**
   calculate height neede for DrawLowerLegend
 */
int
BoxPlot::GetLowerBorderHeight() const
{
    if ( items.size() == 1 )                        // only one boxplot StatisticalInformation
    {
        if ( items.at( 0 ).getName().length() > 0 ) // has label
        {
            return GetTextHeight( items.at( 0 ).getName() );
        }
        else
        {
            return GetTextHeight( QString( "|" ) );
        }
    }

    string longestName;
    {
        unsigned int longestLength = 0;
        for ( vector<StatisticalInformation>::const_iterator it = items.begin(); it != items.end(); ++it )
        {
            if ( it->getName().length() > longestLength )
            {
                longestLength = it->getName().length();
                longestName   = it->getName();
            }
        }
    }
    int textHeight  = GetTextHeight( longestName );
    int clientWidth = GetCanvasWidth() - GetRightBorderWidth()
                      - GetLeftBorderWidth();
    int textWidth       = GetTextWidth( "00:" + longestName );
    int numberOfColumns = clientWidth / textWidth;
    int numberOfRows    = ( numberOfColumns == 0
                            ? 2
                            : ( items.size() + numberOfColumns - 1 ) / numberOfColumns );
    return ( 2 + 2 * numberOfRows ) * textHeight;
}

int
BoxPlot::GetUpperBorderHeight() const
{
    return GetCanvasHeight() / 20;
}

int
BoxPlot::GetRightBorderWidth() const
{
    if ( absolute.getCount() == 0 ) // no right legend
    {
        return GetCanvasWidth() / 20;
    }
    else
    {
        int    maxWidth = 0;
        double values[] = { absolute.getMedian(), absolute.getMean(), absolute.getQ1(), absolute.getQ3(), absolute.getMaximum(), absolute.getMinimum() };
        for ( int i = 0; i < 6; i++ )
        {
            maxWidth = max( maxWidth, GetTextWidth( Globals::formatNumber( values[ i ], values[ i ], true ) ) );
        }
        maxWidth += 3 * tickWidth + borderPad;
        return maxWidth;
    }
}

double
BoxPlot::GetMinValue() const
{
    return minimumValue;
}

double
BoxPlot::GetMaxValue() const
{
    return maximumValue;
}

/** draws right legend with absolute values, if absolute values are set */
void
BoxPlot::DrawRightLegend( QPainter& painter )
{
    if ( absolute.getCount() == 0 )
    {
        return;
    }

    int        x = GetCanvasWidth() - GetRightBorderWidth();
    QList<int> oldPos;
    double     referenceValue = absolute.getMaximum();
    DrawMark( painter, x, items.at( 0 ).getMedian(), absolute.getMedian(), referenceValue, oldPos );
    DrawMark( painter, x, items.at( 0 ).getMean(), absolute.getMean(), referenceValue, oldPos );
    DrawMark( painter, x, items.at( 0 ).getQ1(), absolute.getQ1(), referenceValue, oldPos );
    DrawMark( painter, x, items.at( 0 ).getQ3(), absolute.getQ3(), referenceValue, oldPos );
    DrawMark( painter, x, items.at( 0 ).getMaximum(), absolute.getMaximum(), referenceValue, oldPos );
    DrawMark( painter, x, items.at( 0 ).getMinimum(),  absolute.getMinimum(), referenceValue, oldPos );
}

/** draws one right tick and corresponding label
    @param x x-position of the tick
    @param value current value which defindes the y position
    @param label absolute value corresponding to value, used as label text
    @param oldPos list of previous positions to avoid collisions
 */
void
BoxPlot::DrawMark( QPainter& painter, int x, double value, double label, double reference, QList<int>& oldPos )
{
    int y = valueToYCoord( value );
    int h = GetTextHeight( QString( "8" ) );

    for ( int i = 0; i < oldPos.size(); ++i )
    {
        if ( ( y + h > oldPos[ i ] ) && ( y < oldPos[ i ] + h ) ) // collision with previous label => do not draw
        {
            return;
        }
    }
    oldPos.append( y );

    painter.drawLine( x, y, x + tickWidth, y );
    painter.drawText( x + 3 * tickWidth, y - h / 3,
                      100, h, Qt::AlignLeft, Globals::formatNumber( label, reference, true ) );
}

void
BoxPlot::DrawLeftLegend( QPainter& painter )
{
    int clientWidth = GetLeftBorderWidth();
    int textHeight  = GetTextHeight( QString( "8" ) );

    double referenceValue = GetMaxValue();
    for ( vector<double>::const_iterator it = yAxisScale.begin(); it != yAxisScale.end(); ++it )
    {
        int currYPos = valueToYCoord( *it );

        painter.drawLine( clientWidth, currYPos, clientWidth - tickWidth, currYPos );
        QPen oldPen = painter.pen();
        QPen dashedPen( Qt::DashLine );
        dashedPen.setColor( Qt::cyan );
        painter.setPen( dashedPen );
        painter.drawLine( clientWidth, currYPos,
                          GetCanvasWidth() - GetRightBorderWidth(), currYPos );
        painter.setPen( oldPen );
        painter.drawText( 0, currYPos - textHeight / 3, clientWidth - 3 * tickWidth,
                          textHeight, Qt::AlignRight,
                          Globals::formatNumber( *it, referenceValue, true ) );
    }
}

void
BoxPlot::DrawLowerLegend( QPainter& painter )
{
    int    leftOffset   = GetLeftBorderWidth();
    int    clientWidth  = GetCanvasWidth() - GetRightBorderWidth() - leftOffset;
    int    clientHeight = GetLowerBorderHeight();
    string longestName;
    {
        unsigned int longestLength = 0;
        for ( vector<StatisticalInformation>::const_iterator it = items.begin(); it != items.end(); ++it )
        {
            if ( it->getName().length() > longestLength )
            {
                longestLength = it->getName().length();
                longestName   = it->getName();
            }
        }
    }
    int textWidth = GetTextWidth( "00:" + longestName ) * 4 / 3;

    int numberOfEntries = items.size();
    int numberOfColumns = clientWidth / textWidth;
    if ( numberOfColumns == 0 )
    {
        return;
    }
    int numberOfRows = ( numberOfEntries + numberOfColumns - 1 ) / numberOfColumns;

    int legendHeight      = 2 * GetTextHeight( QString( "1" ) );
    int descriptionHeight = clientHeight - legendHeight;

    int i = 0;
    for ( vector<StatisticalInformation>::const_iterator it = items.begin(); it != items.end(); ++it, ++i )
    {
        int currXOffset = leftOffset + clientWidth / ( numberOfEntries + 1 ) * ( i + 1 );
        /* no legend if only one StatisticalInformation exist */
        if ( items.size() > 1 )
        {
            painter.drawLine( currXOffset, GetCanvasHeight() - clientHeight,
                              currXOffset, GetCanvasHeight() - clientHeight + legendHeight / 4 );
            painter.drawText( currXOffset - GetTextWidth( QString::number( i + 1 ) ) / 3,
                              GetCanvasHeight() - clientHeight + legendHeight * 2 / 3,
                              QString::number( i + 1 ) );
        }
        /* no legend if text if empty */
        if ( it->getName().length() > 0 )
        {
            painter.drawText( leftOffset + ( i % numberOfColumns ) * textWidth,
                              GetCanvasHeight() - descriptionHeight
                              + descriptionHeight / ( 3 * numberOfRows / 2 + 1 )
                              * ( 1 + 3 * ( i / numberOfColumns ) ) / 2,
                              QString::number( i + 1 ) + ": " + it->getName().c_str() );
        }
    }
}

StatisticalInformation const*
BoxPlot::GetClickedStatisticalInformation( QMouseEvent* event ) const
{
    if ( items.size() == 1 )
    {
        return &( items[ 0 ] );
    }

    int x               = event->pos().x();
    int numberOfEntries = items.size();
    int leftOffset      = GetLeftBorderWidth();
    int clientWidth     = GetCanvasWidth() - GetRightBorderWidth() - leftOffset;
    int boxWidth        = 2 * clientWidth / ( 3 * ( numberOfEntries + 1 ) );

    int index  = ( x - leftOffset - boxWidth / 2 ) / ( clientWidth / ( numberOfEntries + 1 ) );
    int middle = leftOffset + clientWidth / ( numberOfEntries + 1 ) * ( index + 1 );
    if ( x >= middle - boxWidth / 2 && x <= middle + boxWidth / 2 )
    {
        if ( index >= 0 && index < numberOfEntries )
        {
            return &( items[ index ] );
        }
    }
    return 0;
}

void
BoxPlot::ShowStatisticsTooltip( StatisticalInformation const* StatisticalInformation )
{
    if ( tooltip )
    {
        tooltip->close();
        delete tooltip;
        tooltip = 0;
    }

    QPoint localMouse( firstPosition.first, firstPosition.second );
    QPoint mousePos = this->mapToGlobal( localMouse );
    tooltip = TableWidget::showInToolTip( this, mousePos, StatisticalInformation->print() );
}

void
BoxPlot::ShowStatistics( StatisticalInformation const* StatisticalInformation )
{
    TableWidget::showInWindow( this, tr( "Statistics info" ), "", StatisticalInformation->print() );
}

void
BoxPlot::DrawBoxPlots( QPainter& painter )
{
    int numberOfEntries = items.size();

    int leftOffset   = GetLeftBorderWidth();
    int clientWidth  = GetCanvasWidth() - GetRightBorderWidth() - leftOffset;
    int lowOffset    = GetCanvasHeight() - GetLowerBorderHeight();
    int clientHeight = lowOffset - GetUpperBorderHeight();
    int boxWidth     = 2 * clientWidth / ( 3 * ( numberOfEntries + 1 ) );

    HeightCalculator        valueModifier( lowOffset, clientHeight, yAxisMax, yAxisMin );
    HeightCalculator const& CalculateHeight = valueModifier;

    int i = 0;
    for ( vector<StatisticalInformation>::const_iterator it = items.begin(); it != items.end(); ++it, ++i )
    {
        int currWidth = leftOffset + clientWidth / ( numberOfEntries + 1 ) * ( i + 1 );

        // Draw the horizontal lines for minimum and maximum
        QPen oldPen = painter.pen();
        QPen thickPen;
        thickPen.setWidth( 2 );
        painter.setPen( thickPen );
        if ( valueModifier.CalculatePosition( it->getMinimum() ) == HeightCalculator::Inside )
        {
            painter.drawLine( currWidth - boxWidth / 4, CalculateHeight( it->getMinimum() ),
                              currWidth + boxWidth / 4, CalculateHeight( it->getMinimum() ) );
        }
        if ( valueModifier.CalculatePosition( it->getMaximum() ) == HeightCalculator::Inside )
        {
            painter.drawLine( currWidth - boxWidth / 4, CalculateHeight( it->getMaximum() ),
                              currWidth + boxWidth / 4, CalculateHeight( it->getMaximum() ) );
        }
        painter.setPen( oldPen );

        // if there are enough values to have Q75 and Q25
        if ( it->getCount() >= 5 )
        {
            // Draw the connection lines between max/min and Q75/Q25
            painter.setPen( Qt::DashLine );
            if ( valueModifier.CalculatePosition( it->getMaximum() ) != valueModifier.CalculatePosition( it->getQ3() )
                 || valueModifier.CalculatePosition( it->getMaximum() ) == HeightCalculator::Inside )
            {
                painter.drawLine( currWidth, CalculateHeight( it->getMaximum() ), currWidth,
                                  CalculateHeight( it->getQ3() ) );
            }
            if ( valueModifier.CalculatePosition( it->getMinimum() ) != valueModifier.CalculatePosition( it->getQ1() )
                 || valueModifier.CalculatePosition( it->getMinimum() ) == HeightCalculator::Inside )
            {
                painter.drawLine( currWidth, CalculateHeight( it->getMinimum() ), currWidth,
                                  CalculateHeight( it->getQ1() ) );
            }
            painter.setPen( Qt::SolidLine );

            // Draw the box
            if ( valueModifier.CalculatePosition( it->getQ3() ) != valueModifier.CalculatePosition( it->getQ1() )
                 || valueModifier.CalculatePosition( it->getQ3() ) == HeightCalculator::Inside )
            {
                QRect myBox( currWidth - boxWidth / 2, CalculateHeight( it->getQ3() ),
                             boxWidth,
                             CalculateHeight( it->getQ1() ) - CalculateHeight( it->getQ3() ) );
                painter.fillRect( myBox, QBrush( Qt::white ) );
                painter.drawRect( myBox );
            }
        }
        else
        {
            // Just draw a connection between min and max and no box
            painter.setPen( Qt::DashLine );
            if ( valueModifier.CalculatePosition( it->getMaximum() ) != valueModifier.CalculatePosition( it->getMinimum() )
                 || valueModifier.CalculatePosition( it->getMaximum() ) == HeightCalculator::Inside )
            {
                painter.drawLine( currWidth, CalculateHeight( it->getMaximum() ), currWidth,
                                  CalculateHeight( it->getMinimum() ) );
            }
            painter.setPen( Qt::SolidLine );
        }

        // Draw the median line
        if ( valueModifier.CalculatePosition( it->getMedian() ) == HeightCalculator::Inside )
        {
            thickPen.setWidth( 3 );
            painter.setPen( thickPen );
            painter.drawLine( currWidth - boxWidth / 2, CalculateHeight( it->getMedian() ),
                              currWidth + boxWidth / 2, CalculateHeight( it->getMedian() ) );
            painter.setPen( oldPen );
        }

        // Draw the mean line
        if ( valueModifier.CalculatePosition( it->getMean() ) == HeightCalculator::Inside )
        {
            painter.setPen( Qt::DashLine );
            painter.drawLine( currWidth - boxWidth / 2, CalculateHeight( it->getMean() ),
                              currWidth + boxWidth / 2, CalculateHeight( it->getMean() ) );
            painter.setPen( Qt::SolidLine );
        }
        // values are invalid. Paint error message
        if ( !it->isValid() )
        {
            QPen redPen;
            redPen.setColor( Qt::red );
            painter.setPen( redPen );

            QRect rect = painter.boundingRect( 0, 0, 0, 0, Qt::AlignLeft, "W" ); // get a hight of the line

            for ( int i = 0; i < it->getDataStatusDescription().size(); i++ )
            {
                painter.drawText(
                    GetLeftBorderWidth(), GetUpperBorderHeight() +  +( rect.height() + 3 ) * ( i + 1 ),
                    it->getDataStatusDescription().at( i )  );
            }
            painter.setPen( oldPen );
        }
    }
}

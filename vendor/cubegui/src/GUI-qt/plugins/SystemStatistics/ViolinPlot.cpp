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

#include "ViolinPlot.h"
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
#include <QPainter>
#include <QEasingCurve>
#include <QPropertyAnimation>
#include <QPointF>
#include <QPainterPath>

using namespace std;
using namespace cubegui;
using namespace system_statistics;

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

ViolinPlot::ViolinPlot( QWidget* parent )
    : QWidget( parent ), firstPosition( 0, 0 )
{
    tooltip      = 0;
    minimumValue = start;
    maximumValue = 0.0;
    start        = 0.;
    borderPad    = 10;
    tickWidth    = 3;

    // setWindowTitle("ViolinPlot");
    resize( 300, 450 );
    if ( parent != 0 )
    {
        parent->adjustSize();
    }
}

/**
   set the data of a boxplot which consists only of one StatisticalInformation
   existing StatisticalInformations will be deleted
   @param theStatisticalInformation values of the boxplot
   @param abs the corresponding absoulte values
 */
void
ViolinPlot::set( const ViolinStatistics& theStatisticalInformation, double startValue )
{
    start = startValue;
    item  = theStatisticalInformation;
    resetView();
}

void
ViolinPlot::set( const ViolinStatistics& theStatisticalInformation, const StatisticalInformation& abs, double startValue )
{
    start    = startValue;
    item     = theStatisticalInformation;
    absolute = abs;
    resetView();
}

void
ViolinPlot::ResetMinMaxValues()
{
    minimumValue = start;
    maximumValue = 0.0;
    maximumValue = item.getStatistics().getMaximum();
    AdjustMinMaxValues();
}

void
ViolinPlot::AdjustMinMaxValues()
{
    vector<double> values;
    values.resize( 0 );
    Pretty( minimumValue, maximumValue, numberOfAxisEntries, values );
    minimumValue = *values.begin();
    maximumValue = *values.rbegin();
}

void
ViolinPlot::resetView()
{
    ResetMinMaxValues();
    update();
}

// Calculates the value on the y-axis which belongs to a height of koord pixels on the axis
double
ViolinPlot::GetValue( int koord ) const
{
    int lowOffset    = GetCanvasHeight() - GetLowerBorderHeight();
    int clientHeight = lowOffset - GetUpperBorderHeight();
    return ( lowOffset - koord ) * ( GetMaxValue() - GetMinValue() )
           / clientHeight + GetMinValue();
}

void
ViolinPlot::paintEvent( QPaintEvent* )
{
    QPainter painter( this );
    //painter.drawText( rect(), Qt::AlignCenter, "This is Violin Plot" );
    if ( item.getStatistics().getCount() == 0 )
    {
        painter.drawText( rect(), Qt::AlignCenter, tr( "Not enough values to display." ) );
        return;
    }
    if ( fabs( maximumValue - minimumValue ) < 1e-8 )
    {
        maximumValue = minimumValue + 0.5; // create valid box
        minimumValue = maximumValue - 1.0;
    }

    DrawLeftLegend( painter );
    DrawRightLegend( painter );
    drawViolinPlot( painter );
    painter.drawRect( GetLeftBorderWidth(), GetUpperBorderHeight(),
                      GetCanvasWidth() - GetRightBorderWidth() - GetLeftBorderWidth(),
                      GetCanvasHeight() - GetLowerBorderHeight() - GetUpperBorderHeight() );
}

void
ViolinPlot::mousePressEvent( QMouseEvent* event )
{
    if ( item.getStatistics().getCount() == 0 )
    {
        return;
    }
    else
    {
        firstPosition.first  = event->pos().x();
        firstPosition.second = event->pos().y();

        if ( event->button() == Qt::RightButton )
        {
            showStatisticsTooltip();
        }
    }
}

#define EPS 10
void
ViolinPlot::mouseReleaseEvent( QMouseEvent* event )
{
    switch ( event->button() )
    {
        case Qt::LeftButton:
            if ( item.getStatistics().getCount() == 0 )
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
                    showStatistics();
                }
                else
                {
                    setMinMaxValues( newMinVal, newMaxVal );
                }
            }
            break;
        case Qt::MiddleButton:
            resetView();
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
ViolinPlot::setMinMaxValues( double newMinVal, double newMaxVal )
{
    if ( newMaxVal < maximumValue )
    {
        maximumValue = newMaxVal;
    }
    if ( newMinVal > minimumValue )
    {
        minimumValue = newMinVal;
    }
    AdjustMinMaxValues();
    update();
}

int
ViolinPlot::GetCanvasWidth() const
{
    return width();
}

int
ViolinPlot::GetCanvasHeight() const
{
    return height();
}

int
ViolinPlot::GetTextWidth( QString const& text ) const
{
    QFontMetrics fm( font() );
    return fm.boundingRect( text ).width();
}

int
ViolinPlot::GetTextWidth( string const& text ) const
{
    return GetTextWidth( QString::fromStdString( text ) );
}

int
ViolinPlot::GetTextHeight( QString const& ) const
{
    QFontMetrics fm( font() );
    return 3 * fm.ascent() / 2;
}

int
ViolinPlot::GetTextHeight( string const& text ) const
{
    return GetTextHeight( QString::fromStdString( text ) );
}

int
ViolinPlot::GetLeftBorderWidth() const
{
    double value      = GetMaxValue();
    double labelWidth = GetTextWidth( Globals::formatNumber( value, value, true ) );

    return labelWidth + 3 * tickWidth + borderPad;
}

/**
   calculate height neede for DrawLowerLegend
 */
int
ViolinPlot::GetLowerBorderHeight() const
{
    return GetTextHeight( QString( "|" ) );
}

int
ViolinPlot::GetUpperBorderHeight() const
{
    return GetCanvasHeight() / 20;
}

int
ViolinPlot::GetRightBorderWidth() const
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
ViolinPlot::GetMinValue() const
{
    return minimumValue;
}

double
ViolinPlot::GetMaxValue() const
{
    return maximumValue;
}

/** draws right legend with absolute values, if absolute values are set */
void
ViolinPlot::DrawRightLegend( QPainter& painter )
{
    if ( absolute.getCount() == 0 )
    {
        return;
    }

    int        x = GetCanvasWidth() - GetRightBorderWidth();
    QList<int> oldPos;
    double     referenceValue = absolute.getMaximum();
    DrawMark( painter, x, item.getStatistics().getMedian(), absolute.getMedian(), referenceValue, oldPos );
    DrawMark( painter, x, item.getStatistics().getMean(), absolute.getMean(), referenceValue, oldPos );
    DrawMark( painter, x, item.getStatistics().getQ1(), absolute.getQ1(), referenceValue, oldPos );
    DrawMark( painter, x, item.getStatistics().getQ3(), absolute.getQ3(), referenceValue, oldPos );
    DrawMark( painter, x, item.getStatistics().getMaximum(), absolute.getMaximum(), referenceValue, oldPos );
    DrawMark( painter, x, item.getStatistics().getMinimum(),  absolute.getMinimum(), referenceValue, oldPos );
}

/** draws one right tick and corresponding label
    @param x x-position of the tick
    @param value current value which defindes the y position
    @param label absolute value corresponding to value, used as label text
    @param oldPos list of previous positions to avoid collisions
 */
void
ViolinPlot::DrawMark( QPainter& painter, int x, double value, double label, double reference, QList<int>& oldPos )
{
    int clientHeight = GetCanvasHeight() - GetLowerBorderHeight()
                       - GetUpperBorderHeight();

    int y = static_cast<int>( GetCanvasHeight() - GetLowerBorderHeight()
                              - clientHeight / ( maximumValue - minimumValue ) * ( value - minimumValue ) );
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
ViolinPlot::DrawLeftLegend( QPainter& painter )
{
    int upperOffset  = GetUpperBorderHeight();
    int clientHeight = GetCanvasHeight() - GetLowerBorderHeight()
                       - upperOffset;
    int clientWidth = GetLeftBorderWidth();

    vector<double> values;
    values.resize( 0 );
    Pretty( GetMinValue(), GetMaxValue(), numberOfAxisEntries, values );
    double minValue = *values.begin();
    double maxValue = *values.rbegin();

    int textHeight = GetTextHeight( QString( "8" ) );

    double referenceValue = GetMaxValue();
    for ( vector<double>::const_iterator it = values.begin(); it != values.end(); ++it )
    {
        int currYPos = static_cast<int>( clientHeight + upperOffset
                                         - clientHeight / ( maxValue - minValue ) * ( *it - minValue ) );
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
ViolinPlot::showStatisticsTooltip()
{
    if ( tooltip )
    {
        tooltip->close();
        delete tooltip;
        tooltip = 0;
    }

    QPoint localMouse( firstPosition.first, firstPosition.second );
    QPoint mousePos = this->mapToGlobal( localMouse );
    tooltip = TableWidget::showInToolTip( this, mousePos, item.getStatistics().print() );
}

void
ViolinPlot::showStatistics()
{
    TableWidget::showInWindow( this, tr( "Statistics info" ), "", item.getStatistics().print() );
}

void
ViolinPlot::makeWidth( double width )
{
    used_width = ( 1.0 / 100 ) * width;
}

void
ViolinPlot::drawViolinPlot( QPainter& painter )
{
    int    leftOffset   = GetLeftBorderWidth();
    int    clientWidth  = GetCanvasWidth() - GetRightBorderWidth() - leftOffset;
    int    lowOffset    = GetCanvasHeight() - GetLowerBorderHeight();
    int    clientHeight = lowOffset - GetUpperBorderHeight();
    double maxValue     = GetMaxValue();
    double minValue     = GetMinValue();

    HeightCalculator        valueModifier( lowOffset, clientHeight, maxValue, minValue );
    HeightCalculator const& CalculateHeight = valueModifier;

    const std::vector<double>& kernelDensity      = item.getKernelDensity();
    double                     kernel_density_min = item.getKernelDensityMin();
    double                     kernel_density_max = item.getKernelDensityMax();
    //item.selectKernel( used_kernel, used_width, used_order, CalculateHeight( item.getStatistics().getMinimum() ), CalculateHeight( item.getStatistics().getMaximum() ), item.getStatistics().getMinimum(), item.getStatistics().getMaximum() );
    //    item.make_width( used_width, CalculateHeight( item.getStatistics().getMinimum() ), CalculateHeight( item.getStatistics().getMaximum() ), item.getStatistics().getMinimum(), item.getStatistics().getMaximum() );
    //item.optimalKernel(CalculateHeight( item.getStatistics().getMinimum() ), CalculateHeight( item.getStatistics().getMaximum() ), item.getStatistics().getMinimum(), item.getStatistics().getMaximum());
    int middle = leftOffset + clientWidth / 2;

    int  x_current   = middle + ( ( kernelDensity.at( 0 ) - kernel_density_min ) / ( kernel_density_max - kernel_density_min ) ) * ( clientWidth / 2 );
    int  opx_current = middle - ( ( kernelDensity.at( 0 ) - kernel_density_min ) / ( kernel_density_max - kernel_density_min ) ) * ( clientWidth / 2 );
    int  y_current   =  CalculateHeight( item.getStatistics().getMinimum() );
    QPen thickPen;
    QPen myPen;
    /*   QPointF p1, p2, p3;
           p2.setX( x_current );
           p2.setY( y_current );
           QLinearGradient myGradient;
           QPainter
           Path    myPath( p2 );*/
    myPen.setWidth( 1 );
    myPen.setColor( Qt::black );
    thickPen.setWidth( 3 );
    thickPen.setColor( Qt::blue );
    painter.setPen( thickPen );

    painter.drawLine( x_current, y_current, opx_current, y_current ); // line from left to right side of the violin plot
    for ( unsigned j = 1; j < kernelDensity.size(); j++ )
    {
        int x_next   = middle + ( ( kernelDensity.at( j ) - kernel_density_min ) / ( kernel_density_max - kernel_density_min ) ) * ( clientWidth / 2 );
        int y_next   =  CalculateHeight( item.getNewdata().at( j ) );
        int opx_next = middle - ( ( kernelDensity.at( j ) - kernel_density_min ) / ( kernel_density_max - kernel_density_min ) ) * ( clientWidth / 2 );
        painter.drawLine( x_current, y_current, x_next, y_next );     // right half of the violin plot
        painter.drawLine( opx_current, y_current, opx_next, y_next ); // left half
        y_current   = y_next;
        x_current   = x_next;
        opx_current = opx_next;
        /* p1.setX( x_next );
               p1.setY( y_next );
               myPath.cubicTo( p2, p1, p1 );
               p2.setX( x_current );
               p2.setY( y_current );*/
    }
    painter.drawLine( x_current, y_current, opx_current, y_current ); // line from left to right side of the violin plot

    thickPen.setWidth( 3 );
    thickPen.setColor( Qt::black );
    painter.setPen( thickPen );
    painter.drawLine( leftOffset, CalculateHeight( item.getStatistics().getMedian() ), clientWidth + leftOffset, CalculateHeight( item.getStatistics().getMedian() ) );
    painter.setPen( Qt::DashLine );
    painter.drawLine( leftOffset, CalculateHeight( item.getStatistics().getMean() ), clientWidth + leftOffset, CalculateHeight( item.getStatistics().getMean() ) );
    thickPen.setWidth( 1 );
    thickPen.setColor( Qt::red );
    painter.setPen( thickPen );
    painter.drawLine( leftOffset, CalculateHeight( item.getStatistics().getQ1() ), clientWidth + leftOffset,  CalculateHeight( item.getStatistics().getQ1() )  );
    painter.drawLine( leftOffset,   CalculateHeight( item.getStatistics().getQ3() ), clientWidth + leftOffset,  CalculateHeight( item.getStatistics().getQ3() )  );
    myPen.setWidth( 1 );
    myPen.setColor( Qt::black );
    painter.setPen( myPen );
    //Just draw a connection between min and max and no box
    painter.setPen( Qt::DashLine );
    if ( valueModifier.CalculatePosition( item.getStatistics().getMaximum() ) != valueModifier.CalculatePosition( item.getStatistics().getMinimum() )
         || valueModifier.CalculatePosition( item.getStatistics().getMaximum() ) == HeightCalculator::Inside )
    {
        painter.drawLine( middle, CalculateHeight( item.getStatistics().getMaximum() ), middle,
                          CalculateHeight( item.getStatistics().getMinimum() ) );
    }
    //draw the little box for the violin plot
    /*if ( valueModifier.CalculatePosition( item.getStatistics().getQ3() ) != valueModifier.CalculatePosition( item.getStatistics().getQ1() )
    || valueModifier.CalculatePosition( item.getStatistics().getQ3() ) == HeightCalculator::Inside )
           {
            QRect myBox( currWidth - boxWidth / 8, CalculateHeight( item.getStatistics().getQ3() ),
                         boxWidth-(currWidth-boxWidth/8),
                         CalculateHeight( item.getStatistics().getQ1() ) - CalculateHeight( item.getStatistics().getQ3() ) );
            painter.fillRect( myBox, QBrush( Qt::black ) );
            painter.drawRect( myBox );
           }*/
    myPen.setWidth( 1 );
    myPen.setColor( Qt::black );
    painter.setPen( myPen );
}

#ifdef SELECT_KERNELS
//slot for the order slider
void
ViolinPlot::orderSLider( int value )
{
    if ( 1 == stacker->currentIndex() )
    {
        ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
        if ( _vp != NULL )
        {
            _vp->selectOrder( static_cast<TaylorOrder>( value ) );
            _vp->update();
        }
    }
}
//slot for width_slider
void
ViolinPlot::widthSlider( int value )
{
    if ( 1 == stacker->currentIndex() )
    {
        ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
        if ( _vp != NULL )
        {
            _vp->makeWidth( value );
            _vp->update();
        }
    }
}


//functions for the selector
void
ViolinPlot::UniK( bool ok )
{
    if ( ok )
    {
        if ( 1 == stacker->currentIndex() )
        {
            ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
            if ( _vp != NULL )
            {
                _vp->selectKernel( UNIFORM );
                _vp->update();
            }
        }
    }
}
void
ViolinPlot::TriangK( bool ok )
{
    if ( ok )
    {
        if ( 1 == stacker->currentIndex() )
        {
            ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
            if ( _vp != NULL )
            {
                _vp->selectKernel( TRIANGULAR );
                _vp->update();
            }
        }
    }
}
void
ViolinPlot::EpaK( bool ok )
{
    if ( ok )
    {
        if ( 1 == stacker->currentIndex() )
        {
            ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
            if ( _vp != NULL )
            {
                _vp->selectKernel( EPANECHNIKOV );
                _vp->update();
            }
        }
    }
}
void
ViolinPlot::QuarK( bool ok )
{
    if ( ok )
    {
        if ( 1 == stacker->currentIndex() )
        {
            ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
            if ( _vp != NULL )
            {
                _vp->selectKernel( QUARTIC );
                _vp->update();
            }
        }
    }
}
void
ViolinPlot::TriwK( bool ok )
{
    if ( ok )
    {
        if ( 1 == stacker->currentIndex() )
        {
            ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
            if ( _vp != NULL )
            {
                _vp->selectKernel( TRIWEIGHT );
                _vp->update();
            }
        }
    }
}
void
ViolinPlot::TcubeK( bool ok )
{
    if ( ok )
    {
        if ( 1 == stacker->currentIndex() )
        {
            ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
            if ( _vp != NULL )
            {
                _vp->selectKernel( TRICUBE );
                _vp->update();
            }
        }
    }
}
void
ViolinPlot::GaussK( bool ok )
{
    if ( ok )
    {
        if ( 1 == stacker->currentIndex() )
        {
            ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
            if ( _vp != NULL )
            {
                _vp->selectKernel( GAUSSIAN );
                _vp->update();
            }
        }
    }
}
void
ViolinPlot::CosK( bool ok )
{
    if ( ok )
    {
        if ( 1 == stacker->currentIndex() )
        {
            ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
            if ( _vp != NULL )
            {
                _vp->selectKernel( COSINE );
                _vp->update();
            }
        }
    }
}
void
ViolinPlot::LogK( bool ok )
{
    if ( ok )
    {
        if ( 1 == stacker->currentIndex() )
        {
            ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
            if ( _vp != NULL )
            {
                _vp->selectKernel( LOGISTIC );
                _vp->update();
            }
        }
    }
}
void
ViolinPlot::SigK( bool ok )
{
    if ( ok )
    {
        if ( 1 == stacker->currentIndex() )
        {
            ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
            if ( _vp != NULL )
            {
                _vp->selectKernel( SIGMOID );
                _vp->update();
            }
        }
    }
}
void
ViolinPlot::SilvK( bool ok )
{
    if ( ok )
    {
        if ( 1 == stacker->currentIndex() )
        {
            ViolinPlot* _vp = dynamic_cast<ViolinPlot*>( stacker->currentWidget() );
            if ( _vp != NULL )
            {
                _vp->selectKernel( SILVERMAN );
                _vp->update();
            }
        }
    }
}
#endif

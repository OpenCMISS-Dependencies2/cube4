/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#include "config.h"


#include "HeatmapPlotArea.h"
#include <QMap>
#include <QPainter>
#include <QDebug>
#include <QPointF>
#include <QTransform>

using namespace plotWidget;
using namespace cubepluginapi;


HeatMapPlotArea::HeatMapPlotArea( QWidget* parent ) :
    AbstractPlotArea( parent )
{
    initComponents();
}

void
HeatMapPlotArea::initComponents()
{
    legendIsVisible         = false;
    legendBackgroundIsSolid = false;
    legendXAlignment        = Left;
    legendYAlignment        = Top;
    // dataMinColor            = DEFAULT_HEAT_MAP_MIN_COLOR;
    // dataMaxColor            = DEFAULT_HEAT_MAP_MAX_COLOR;
    AbstractPlotArea::initComponents();
    legendItemsCount = 2;
}
void
HeatMapPlotArea::paintEvent( QPaintEvent*  )
{
    QPainter painter( this );
    if ( data.size() == 0 )
    {
        painter.setPen( QPen( Qt::black, 2 ) );
        painter.setBrush( palette().color( backgroundRole() ) );
        painter.drawText( rect(), Qt::AlignCenter, tr( "No data to display." ) );
        dataToDisplay = false;
        return;
    }
    painter.setPen( QPen( Qt::white, 1 ) );
    painter.setBrush( Qt::white );

    painter.drawRect( 0, 0, width(), height() );
    doPaint( QRect( 0, 0, width(), height() ), &painter );
}

void
HeatMapPlotArea::doPaint( QRect viewport, QPainter* painter )
{
    //   qDebug()<<"heatmap " <<viewport.x()<<" "<< viewport.y()<<" "<< viewport.width()<<" "<< viewport.height()<<" "<<data.size()<<" "<<data[ 0 ].size();

    dataToDisplay = true;
    painter->setBrush( Qt::NoBrush );
    painter->setViewport( viewport.x(), viewport.y(), viewport.width(), viewport.height() );
    painter->setWindow( 0, 0, data.size() /*iterations, we dont have 0 iteration*/, data[ 0 ].size() /*processes*/ );
    painter->fillRect( painter->window(), Qt::white );
    painter->setPen( QPen( Qt::white ) );
    painter->setBrush( Qt::blue );
    QPen p;
    p.setWidthF( 1.0f );
    painter->setPen( p );
    for ( int iteration_counter = 0; iteration_counter < data.size(); iteration_counter++ )
    {
        for ( int process_counter = 0; process_counter < data[ iteration_counter ].size(); process_counter++ )
        {
            double myValue         = data[ iteration_counter ][ process_counter ];
            QColor calculatedColor = service->getColor( myValue, dataMin, dataMax );
            p.setColor( calculatedColor );
            painter->setPen( p );
            painter->drawPoint( QPointF( iteration_counter + 0.50f, data[ iteration_counter ].size() - process_counter - 0.50f ) );
        }
    }
    AbstractPlotArea::doPaint( viewport, painter );
}

void
HeatMapPlotArea::reset()
{
    initComponents();
    data.clear();
}

QPixmap
HeatMapPlotArea::generateLegend()
{
    QFont        serifFont( "Times", 9, QFont::Bold );
    QFontMetrics fm( serifFont );
    int          fontHeight          = fm.height();
    int          totalLengthInPixels = 4 + ( ( ( fontHeight * 0.75 ) + 5 ) * legendItemsCount );
    // to calculate the width we need to calculate the longest word's number of characters
    int maxStringWidthInPixels = fm.boundingRect( QString::number( ( double )( ( ( int )dataMax ) + 0.8888 ), 'f', 4 ) ).width();
    int totalWidthInPixels     = 4 + ( fontHeight * 0.75 ) + 5 + maxStringWidthInPixels + 4;
    // create a picture
    QPixmap picture( totalWidthInPixels, totalLengthInPixels );
    if ( legendBackgroundIsSolid == true )
    {
        picture.fill( Qt::white );
        QPainter p( &picture );
        p.setPen( QPen( Qt::black, 1 ) );
        p.drawRect( 0, 0, picture.width() - 1, picture.height() - 1 );
    }
    else
    {
        picture.fill( Qt::transparent );
    }
    QPainter painter( &picture );
    painter.setRenderHint( QPainter::Antialiasing, false );
    double dataStep = ( dataMax - dataMin ) / ( legendItemsCount - 1 );

    for ( int i = 0; i < legendItemsCount; i++ )
    {
        painter.setFont( serifFont );
        //   painter.setPen( normalize( ( dataStep * i ) + dataMin ) );
        // painter.setBrush( normalize( ( dataStep * i ) + dataMin ) );

        QColor calculatedColor = service->getColor( ( dataStep * i ) + dataMin, dataMin, dataMax );
        painter.setPen( calculatedColor );
        painter.setBrush( calculatedColor );


        painter.drawRect( 4, 4 + i * ( ( fontHeight * 0.75 ) + 5 ), fontHeight * 0.75, fontHeight * 0.75 );
        painter.setPen( Qt::black );
        QPoint p = QPoint( 4 + ( fontHeight * 0.75 ) + 3, ( i + 1 ) * ( ( fontHeight * 0.75 ) + 5 ) - 2 );
        painter.drawText( p, QString::number( dataStep * i + dataMin, 'f', 4 ) );
    }
    return picture;
}

void
HeatMapPlotArea::setData( QVector<QVector<double> > data )
{
    this->data = data;
}
void
HeatMapPlotArea::setPluginService( PluginServices* service )
{
    this->service = service;
}
void
HeatMapPlotArea::setDataColor( QVector<QVector<QColor> > dataColortemp )
{
    this->dataColor = dataColortemp;
}
double
HeatMapPlotArea::getData( int processId, int iterationId ) const
{
    if ( !data.isEmpty() && iterationId < data.size() &&
         !data.at( iterationId ).isEmpty() && processId < data.at( iterationId ).size() )
    {
        return data.at( iterationId ).at( processId );
    }
    return -1.0;
}

/*QColor
   HeatMapPlotArea::normalize( double value )
   {
    double rMin, gMin, bMin, rMax, gMax, bMax, rNew, gNew, bNew;
    dataMinColor.getRgbF( &rMin, &gMin, &bMin );
    dataMaxColor.getRgbF( &rMax, &gMax, &bMax );
    rNew = ( ( value - dataMin ) / ( dataMax - dataMin ) ) * ( rMax - rMin ) + rMin;
    gNew = ( ( value - dataMin ) / ( dataMax - dataMin ) ) * ( gMax - gMin ) + gMin;
    bNew = ( ( value - dataMin ) / ( dataMax - dataMin ) ) * ( bMax - bMin ) + bMin;
    return QColor::fromRgbF( rNew, gNew, bNew );
   }

   double
   HeatMapPlotArea::denormalize( int value )
   {
    double aLength         = dataMax - dataMin;
    double bLength         = 255 - 0;
    int    nLength         = value - 0;
    double normalizedValue = ( nLength * bLength ) / aLength;
    return normalizedValue + dataMin;
   }*/

void
HeatMapPlotArea::setDataMin( double min )
{
    this->dataMin = min;
}

void
HeatMapPlotArea::setDataMax( double max )
{
    this->dataMax = max;
}

/*void
   HeatMapPlotArea::setDataMinColor( QColor dataMinColor )
   {
    this->dataMinColor = dataMinColor;
   }

   void
   HeatMapPlotArea::setDataMaxColor( QColor dataMaxColor )
   {
    this->dataMaxColor = dataMaxColor;
   }

   QColor
   HeatMapPlotArea::getDataMinColor() const
   {
    return this->dataMinColor;
   }

   QColor
   HeatMapPlotArea::getDataMaxColor() const
   {
    return this->dataMaxColor;
   }
 */
void
HeatMapPlotArea::setLegendItemsCount( int legendItemsCount )
{
    this->legendItemsCount = legendItemsCount;
}

int
HeatMapPlotArea::getLegendItemsCount() const
{
    return legendItemsCount;
}

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

#include "BarplotArea.h"
#include "GraphWidgetGlobals.h"
#include <QMap>
#include <QPainter>
#include <QDebug>
#include <QTransform>

BarPlotArea::BarPlotArea( QWidget* parent ) :
    AbstractPlotArea( parent )
{
    initComponents();
}
void
BarPlotArea::initComponents()
{
    legendIsVisible         = false;
    legendBackgroundIsSolid = false;
    legendXAlignment        = plotWidget::Left;
    legendYAlignment        = plotWidget::Top;
    AbstractPlotArea::initComponents();
}
void
BarPlotArea::paintEvent( QPaintEvent* )
{
// Debug()<<"3^^^^^^";
    QPainter painter( this );

    if ( data.size() == 0 ) // || displayStatus ==false)
    {
        painter.setPen( QPen( Qt::black, 2 ) );
        painter.setBrush( palette().color( backgroundRole() ) );
        painter.drawText( rect(), Qt::AlignCenter, tr( "No data to display." ) );
        dataToDisplay = false; // use for showing context menu or not
        return;
    }
    painter.setPen( QPen( Qt::white, 1 ) );
    painter.setBrush( Qt::white );
    painter.drawRect( 0, 0, width(), height() );
    doPaint( QRect( 0, 0, width(), height() ), &painter );
}
void
BarPlotArea::doPaint( QRect viewport, QPainter* painter )
{
    dataToDisplay = true;
    painter->setBrush( Qt::NoBrush );
    // if ( data.size() == 0 )
    // {
    //   return;
    // }
    if ( ( ( ( *data.constBegin() ) ).getData().size() == 0 ) )
    {
        return;
    }
    painter->setViewport( viewport.x(), viewport.y(), viewport.width(), viewport.height() );
    //  painter->setWindow(0, 0, ((*data.constBegin())).getData().size()-1, plotWidget::VERTICAL_PRECISION);
    painter->setWindow( 0, 0, ( ( *data.constBegin() ) ).getData().size(), plotWidget::VERTICAL_PRECISION );
    painter->fillRect( painter->window(), Qt::white );
    painter->setPen( QPen( Qt::white ) );

    for ( QList<CubeDataItem>::const_iterator i = data.constBegin(); i != data.constEnd(); i++ )
    {
        if ( ( *i ).isVisible() == false )
        {
            continue;
        }
        QPen   p;
        QColor color = ( *i ).getColor();
        p.setColor( QColor( color.red(), color.green(), color.blue(), 255 ) );
        p.setWidthF( 1.0f );
        painter->setPen( p );
        QVector<double> bars = ( *i ).getData();
        for ( int i = 0; i < bars.size(); i++ )
        {
            // painter->drawLine( QLineF( i + 0.50f, plotWidget::VERTICAL_PRECISION, i + 0.50f,  plotWidget::VERTICAL_PRECISION - bars.at( i ) * plotWidget::VERTICAL_PRECISION ) );
            painter->drawLine( QLineF( i, plotWidget::VERTICAL_PRECISION, i,  plotWidget::VERTICAL_PRECISION - bars.at( i ) * plotWidget::VERTICAL_PRECISION ) );
        }
    }
    AbstractPlotArea::doPaint( viewport, painter );
}

QPixmap
BarPlotArea::generateLegend()
{
    QFont        serifFont( tr( "Times" ), 9, QFont::Bold );
    QFontMetrics fm( serifFont );
    int          fontHeight = fm.height();
    // / determine the size of the legend
    // / to calculate the width we need to calculate the longest word's number of characters and
    // / the number of visible data items
    int visibleCubeDataItemsCount = 0;
    int maxStringWidthInPixels    = 0;
    for ( QList<CubeDataItem>::const_iterator i = data.constBegin(); i != data.constEnd(); i++ )
    {
        if ( ( *i ).isVisible() == true )
        {
            plotWidget::MathOp itemOpType = ( *i ).getMathOp();
            if ( itemOpType < plotWidget::Minimum || itemOpType >= plotWidget::MathOpUnknown )
            {
                qDebug() << tr( "Trying to generate a legend for a plot of unknown math operation type!" );
            }

            if ( maxStringWidthInPixels < ( fm.boundingRect( ( *i ).getName() + ", " + plotWidget::CALC_NAMES[ static_cast<int>( itemOpType ) ].toLower() ) ).width() )
            {
                maxStringWidthInPixels = fm.boundingRect( ( *i ).getName() + ", " + plotWidget::CALC_NAMES[ static_cast<int>( itemOpType ) ].toLower() ).width();
            }
            visibleCubeDataItemsCount++;
        }
    }
    int totalWidthInPixels  = 4 + 11 + 5 + maxStringWidthInPixels + 4;
    int totalLengthInPixels = 4 + ( ( ( fontHeight * 0.75 ) + 5 ) * visibleCubeDataItemsCount );
    // / create a picture
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
    // / create a painter
    QPainter painter( &picture );
    painter.setRenderHint( QPainter::Antialiasing, false );
    // / iterate and draw each element
    int cnt = 0;
    for ( QList<CubeDataItem>::const_iterator i = data.constBegin(); i != data.constEnd(); i++ )
    {
        if ( ( *i ).isVisible() == true )
        {
            painter.setFont( serifFont );
            painter.setPen( ( *i ).getColor() );
            painter.setBrush( QBrush( ( *i ).getColor(), Qt::SolidPattern ) );
            painter.drawRect( 4, 4 + cnt * ( ( fontHeight * 0.75 ) + 5 ), fontHeight * 0.75, fontHeight * 0.75 );
            painter.setPen( Qt::black );
            QPoint             p          = QPoint( 4 + ( fontHeight * 0.75 ) + 3, ( cnt + 1 ) * ( ( fontHeight * 0.75 ) + 5 ) - 2 );
            plotWidget::MathOp itemOpType = ( *i ).getMathOp();
            painter.drawText( p, ( *i ).getName() + ", " + plotWidget::CALC_NAMES[ static_cast<int>( itemOpType ) ].toLower() );
            cnt++;
        }
    }
    return picture;
}

void
BarPlotArea::setDataRef( QList<CubeDataItem> data )
{
    this->data = data;
    this->update();
}
void
BarPlotArea::reset()
{
    initComponents();
    data.clear();
}
/*void
   BarPlotArea::setDisplayStatus(bool newDisplayStatus)
   {
    displayStatus = newDisplayStatus;
   }*/

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

#include <QPainter>
#include <QDebug>
#include <QStringList>
#include <QToolTip>
#include <QMouseEvent>
#include "Globals.h"
#include "Bar.h"

using namespace cubegui;
using namespace advisor;

Bar::Bar()
{
    value       = 0;
    padding     = 5;
    min         = -1;
    max         = -1;
    singleValue = true;
}

void
Bar::setMinimum( double min )
{
    minValue = min;
}
void
Bar::setMaximum( double max )
{
    maxValue = max;
}

void
Bar::setValue( double val )
{
    value = val;
}

void
Bar::setValues( double min,
                double avg,
                double max )
{
    this->min   = min;
    this->value = avg;
    this->max   = max;
    singleValue = false;
}

void
Bar::setColor( QColor color )
{
    this->color = color;
}


void
Bar::paintEvent( QPaintEvent* )
{
    // start painting
    QPainter painter;
    painter.begin( this );
    painter.fillRect( 0, padding, width(), height() - padding, QColor( 220, 220, 220 ) );

    // draw contents
    if ( color.isValid() )
    {
        painter.fillRect( 1, padding + 1, ( width() - 2 ) * value, height() - padding - 2, color );
    }

    painter.setPen( Qt::lightGray );
    //painter.drawText( QRect( 0, padding, width(), height() - padding ), Qt::AlignVCenter,  "  " + QString::number( value ) + "%" );

    // draw lines for minimum, average and maximum
    if ( min >= 0 ) // valid
    {
        QPen pen( Qt::darkGray );
        pen.setWidth( 2 );
        painter.setPen( pen );
        QList<double> values;
        values << min << value << max;
        QList<double> heights;
        heights << 5 << 10 << 5;
        int y = ( height() + 2 ) / 2;
        for ( int i = 0; i < values.size(); i++ )
        {
            int x = values[ i ] * ( width() - 2 );
            int h = heights[ i ];
            painter.drawLine( x, y - h, x, y + h );
        }
        painter.drawLine( min * ( width() - 2 ), y, max * ( width() - 2 ), y );
    }

    // draw an outline
    // painter.drawRect( 0, 0, width() - 1, height() - 1 );
}



void
Bar::mousePressEvent( QMouseEvent* event )
{
    QString tooltip = tr( "Minimal value: %1\nAverage: %2\nMaximal value: %3" ).arg( this->min ).arg( this->value ).arg( this->max );
    QToolTip::showText( event->globalPos(), tooltip );
}

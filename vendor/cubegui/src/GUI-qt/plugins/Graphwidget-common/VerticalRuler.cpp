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

#include "VerticalRuler.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMap>
#include <QDebug>
#include <QVBoxLayout>

VerticalRuler::VerticalRuler( QWidget* parent ) : AbstractRuler( parent )
{
    initComponents();
}

void
VerticalRuler::initComponents()
{
    this->setLayout( new QVBoxLayout() );
    notchesAlignment = plotWidget::Right;
}

int
VerticalRuler::getRulerLength()
{
    return height();
}

void
VerticalRuler::paintEvent( QPaintEvent* )
{
    QPainter painter( this );
    doPaint( QRect( 0, 0, width(), height() ), &painter );
}

void
VerticalRuler::doPaint( QRect viewport, QPainter* painter )
{
    if ( majorNotchesIndexAndLocation.size() == 0 )
    {
        return;
    }

    const QFont MAJORfont = QFont( "Helvetica", 7, -1, false );  // using MAJOR_NOTCH_FONT which is defined in abstractRuler in defining Font , is not worked! I have to define local QFont
    const QFont MINORfont = QFont( "Helvetica", 7, -1, false );


    painter->setViewport( viewport.x(), viewport.y(), viewport.width(), viewport.height() );
    painter->setWindow( 0, 0, viewport.width(), viewport.height() );
    painter->setRenderHint( QPainter::Antialiasing );
    painter->fillRect( painter->window(), palette().color( backgroundRole() ) );
    painter->setPen( QPen( Qt::black, 1 ) );

    //Draw major notches first
    painter->setPen( QPen( plotWidget::MAJOR_NOTCHES_COLOR ) );
    painter->setFont( MAJORfont );//MAJOR_NOTCH_FONT);/////////////////////////

    for ( QMap<double, int>::iterator i = majorNotchesIndexAndLocation.begin(); i != majorNotchesIndexAndLocation.end(); i++ )
    {
        drawNotch( i.key(), i.value(), painter, plotWidget::MAJOR_NOTCH_SIZE, viewport );
    }

    //Draw minor notches second
    painter->setPen( QPen( plotWidget::MINOR_NOTCHES_COLOR ) );
    painter->setFont( MINORfont );//MINOR_NOTCH_FONT);

    for ( QMap<double, int>::iterator i = minorNotchesIndexAndLocation.begin(); i != minorNotchesIndexAndLocation.end(); i++ )
    {
        drawNotch( i.key(), i.value(), painter, plotWidget::MINOR_NOTCH_SIZE, viewport );
    }
    //Draw the title
    if ( isTitleVisible == true )
    {
        painter->setPen( QPen( RULER_TITLE_COLOR ) );
        painter->setFont( RULER_TITLE_FONT );
        QFontMetrics fontMetrics( RULER_TITLE_FONT );
        QSize        titleSize = fontMetrics.size( Qt::TextSingleLine, title );
        switch ( titleAlignment )
        {
            case plotWidget::Top:
            { painter->drawText( QRect( ( this->rect().left() + viewport.width() ) / 2 - ( titleSize.width() + 7 ) / 2,
                                        0,
                                        titleSize.width() + 4,
                                        titleSize.height() ), title );
            } break;
            case plotWidget::Bottom:
            { painter->drawText( QRect( ( this->rect().left() + viewport.width() ) / 2 - ( titleSize.width() + 7 ) / 2,
                                        this->rect().top() + viewport.height() - titleSize.height(),
                                        titleSize.width() + 4,
                                        titleSize.height() ),
                                 title );
            } break;
            default:
            { painter->drawText( QRect( ( this->rect().left() + viewport.width() ) / 2 - ( titleSize.width() + 7 ) / 2,
                                        ( this->rect().top() + height() ) / 2 - ( titleSize.height() ) / 2,
                                        titleSize.width() + 4,
                                        titleSize.height() ),
                                 title );
            } break;
        }
    }
}

void
VerticalRuler::drawNotch( double number, int yDisplacement, QPainter* painter, int notchSize, QRect viewport )
{
    if ( yDisplacement > viewport.height() )
    {
        return;
    }
    QFontMetrics fontMetric( painter->font() );

    QString numberAsString;
    if ( printRealValues )
    {
        numberAsString = QString::number( number, 'g', 4 );
    }
    else
    {
        numberAsString = QString::number( number );
    }


    int textTop = ( ( viewport.height() - yDisplacement ) > fontMetric.height() ) ? viewport.height() - yDisplacement : fontMetric.height();
    int textLeft;
    if ( notchesAlignment == plotWidget::Right )
    {
        painter->drawLine( viewport.width() - LINE_PADDING - notchSize,
                           textTop,
                           viewport.width() - LINE_PADDING,
                           viewport.height() - yDisplacement );
        textLeft = viewport.width() - LINE_PADDING - notchSize - fontMetric.boundingRect( numberAsString ).width();
    }
    else  //notchesAlignment == constants::Left
    {
        painter->drawLine( 0, viewport.height() - yDisplacement, LINE_PADDING + notchSize, viewport.height() - yDisplacement );
        textLeft = LINE_PADDING + notchSize;
    }
    painter->setPen( QPen( Qt::black, 1 ) );
    painter->drawText( textLeft, textTop, numberAsString );
}

void
VerticalRuler::mouseMoveEvent( QMouseEvent* mouseEvent )
{
    if ( isMousePressed && mouseEvent->pos().y() <= maximumLimitInPixels && mouseEvent->pos().y() >= 0 )
    {
        double yIndex = ( maximumLimitInPixels * mouseEvent->pos().y() ) / ( double )maximumLimitInPixels;
        emit   indexAtMousePositionChanged( yIndex );
    }
}

void
VerticalRuler::forceIndexAtMousePositionChangedSignal( QMouseEvent* mouseEvent )
{
    double yIndex = ( maximumLimitInPixels * mouseEvent->pos().y() ) / ( double )maximumLimitInPixels;
    emit   indexAtMousePositionChanged( yIndex );
}

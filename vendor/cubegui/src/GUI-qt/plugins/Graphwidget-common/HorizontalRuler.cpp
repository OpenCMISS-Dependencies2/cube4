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

#include "HorizontalRuler.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMap>
#include <QDebug>
#include <QVBoxLayout>

HorizontalRuler::HorizontalRuler( QWidget* parent ) : AbstractRuler( parent )
{
    initComponents();
}

void
HorizontalRuler::initComponents()
{
    this->setLayout( new QVBoxLayout() );
    notchesAlignment = plotWidget::Bottom;
}

int
HorizontalRuler::getRulerLength()
{
    return width();
}

void
HorizontalRuler::doPaint( QRect viewport, QPainter* painter )
{
    if ( majorNotchesIndexAndLocation.size() == 0 )
    {
        return;
    }
    const QFont MAJORfont = QFont( "Halventica", 7, -1, false );
    const QFont MINORfont = QFont( "Halventica", 7, -1, false );

// qDebug()<<viewport.x()<<" "<< viewport.y()<<" "<< viewport.width()<<" "<< viewport.height();

    painter->setViewport( viewport.x(), viewport.y(), viewport.width(), viewport.height() );
    painter->setWindow( 0, 0, viewport.width(), viewport.height() );
    painter->setRenderHint( QPainter::Antialiasing );
    painter->setPen( QPen( palette().color( backgroundRole() ) ) );
    painter->setBrush( palette().color( backgroundRole() ) );
    painter->drawRect( rect() );
    painter->setPen( QPen( Qt::black, 1 ) );
    // painter->drawLine(LINE_PADDING, LINE_PADDING, width()-LINE_PADDING, LINE_PADDING);
    // Draw major notches first
    painter->setPen( QPen( plotWidget::MAJOR_NOTCHES_COLOR ) );

    painter->setFont( MAJORfont ); // MAJOR_NOTCH_FONT);////////////////////////////////

    // draw first major notch
    drawNotch( majorNotchesIndexAndLocation.begin().key(), majorNotchesIndexAndLocation.begin().value(), painter, plotWidget::MAJOR_NOTCH_SIZE );

    // draw the rest of the major notches
    for ( QMap<double, int>::iterator i = majorNotchesIndexAndLocation.begin(); i != majorNotchesIndexAndLocation.end(); i++ )
    {
        drawNotch( i.key(), i.value(), painter, plotWidget::MAJOR_NOTCH_SIZE );
    }

    // draw minor notches second
    painter->setPen( QPen( plotWidget::MINOR_NOTCHES_COLOR ) );
    painter->setFont( MINORfont ); // MINOR_NOTCH_FONT);
    for ( QMap<double, int>::iterator i = minorNotchesIndexAndLocation.begin(); i != minorNotchesIndexAndLocation.end(); i++ )
    {
        drawNotch( i.key(), i.value(), painter, plotWidget::MINOR_NOTCH_SIZE );
    }

    // Draw the title
    if ( isTitleVisible == true )
    {
        painter->setPen( QPen( RULER_TITLE_COLOR ) );
        painter->setFont( RULER_TITLE_FONT );
        QFontMetrics fontMetrics( RULER_TITLE_FONT );
        QSize        titleSize = fontMetrics.size( Qt::TextSingleLine, title );
        switch ( titleAlignment )
        {
            // needs minor revision
            case plotWidget::Right:
            { painter->drawText( QRect( this->rect().left() + width() - titleSize.width() - 7, ( this->rect().top() + height() - titleSize.height() ) / 2, titleSize.width() + LINE_PADDING, titleSize.height() ), title );
            } break;
            case plotWidget::Left:
            { painter->drawText( QRect( this->rect().left(), ( this->rect().top() + height() - titleSize.height() ) / 2, titleSize.width() + LINE_PADDING, titleSize.height() ), title );
            } break;
            default:
            { painter->drawText( QRect( ( this->rect().left() + width() ) / 2 - ( titleSize.width() + 7 ) / 2, ( this->rect().top() + height() - titleSize.height() ) / 2, titleSize.width() + LINE_PADDING, titleSize.height() ), title );
            } break;
        }
    }
}

void
HorizontalRuler::mouseMoveEvent( QMouseEvent* mouseEvent )
{
    if ( isMousePressed && mouseEvent->pos().x() <= maximumLimitInPixels && mouseEvent->pos().x() >= 0 )
    {
        double xIndex = ( maximumLimitInPixels * mouseEvent->pos().x() ) / ( double )maximumLimitInPixels;
        emit   indexAtMousePositionChanged( xIndex );
    }
}

void
HorizontalRuler::paintEvent( QPaintEvent* )
{
    QPainter painter( this );
    doPaint( QRect( 0, 0, width(), height() ), &painter );
}

void
HorizontalRuler::forceIndexAtMousePositionChangedSignal( QMouseEvent* mouseEvent )
{
    double xIndex = ( maximumLimitInPixels * mouseEvent->pos().x() ) / ( double )maximumLimitInPixels;
    emit   indexAtMousePositionChanged( xIndex );
}

void
HorizontalRuler::drawNotch( double number, int xDisplacement, QPainter* painter, int notchSize )
{
    QFontMetrics fontMetric( painter->font() );
    QString      numberAsString;
    if ( printRealValues )
    {
        numberAsString = QString::number( number, 'f', 3 );
    }
    else
    {
        numberAsString = QString::number( number );
    }
    int translationY;
    int numberWidth  = fontMetric.boundingRect( numberAsString ).width();
    int translationX = ( width() - xDisplacement > numberWidth ) ? xDisplacement : ( xDisplacement - numberWidth );
    if ( notchesAlignment == plotWidget::Top )
    {
        painter->drawLine( xDisplacement, LINE_PADDING, xDisplacement, LINE_PADDING + notchSize );
        translationY = LINE_PADDING + notchSize;
    }
    else   // notchesAlignment == constants::Bottom
    {
        painter->drawLine( xDisplacement, height() - LINE_PADDING, xDisplacement, height() - LINE_PADDING - notchSize );
        translationY = height() - LINE_PADDING - notchSize - numberWidth; // not sure
    }
    painter->save();
    painter->translate( translationX, translationY );
    painter->rotate( 90 );
    painter->setPen( QPen( Qt::black, 1 ) );
    painter->drawText( 0, 0, numberAsString );
    painter->restore();
}

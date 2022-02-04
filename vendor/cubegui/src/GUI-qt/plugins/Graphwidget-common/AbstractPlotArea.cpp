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

#include "AbstractPlotArea.h"
#include "GraphWidgetGlobals.h"
#include <QMap>
#include <QPainter>
#include <QDebug>
#include <QTransform>


AbstractPlotArea::AbstractPlotArea( QWidget* parent ) :
    QWidget( parent )
{
    initComponents();
}

void
AbstractPlotArea::initComponents()
{
    showMouseAxis                = false;
    leftMouseIsClicked           = false;
    mouseAxisHorizontalAlignment = plotWidget::Right;
    mouseAxisVerticalAlignment   = plotWidget::Top;
    // this->setContextMenuPolicy(Qt::ActionsContextMenu);
    this->setContextMenuPolicy( Qt::CustomContextMenu );
}

void
AbstractPlotArea::paintEvent( QPaintEvent* )
{
    QPainter painter( this );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setPen( QPen( Qt::white, 1 ) );
    painter.setBrush( Qt::white );
    painter.drawRect( 0, 0, width(), height() );

    doPaint( QRect( 0, 0, width(), height() ), &painter );
}

void
AbstractPlotArea::doPaint( QRect viewport, QPainter* painter )
{
    painter->resetTransform();
    painter->setCompositionMode( QPainter::CompositionMode_SourceOver );
    //reset window scale back to normal
    painter->setViewport( viewport.x(), viewport.y(), viewport.width(), viewport.height() );
    painter->setWindow( 0, 0, width(), height() );

    //draw horizontal and vertical mesh lines
    painter->setPen( QPen( plotWidget::MAJOR_NOTCHES_COLOR, 1.0f ) );
    foreach( int xDisplacement, VMajorNotches )
    painter->drawLine( xDisplacement, 0, xDisplacement, height() );
    foreach( int yDisplacement, HMajorNotches )
    painter->drawLine( 0, height() - yDisplacement, width(), height() - yDisplacement );
    painter->setPen( QPen( plotWidget::MINOR_NOTCHES_COLOR, 0.7f ) );
    foreach( int xDisplacement, VMinorNotches )
    painter->drawLine( xDisplacement, 0, xDisplacement, height() );
    foreach( int yDisplacement, HMinorNotches )
    painter->drawLine( 0, height() - yDisplacement, width(), height() - yDisplacement );

    if ( showMouseAxis )
    {
        painter->setPen( QPen( Qt::red, 1.0f, Qt::DotLine, Qt::RoundCap ) );
        painter->setCompositionMode( QPainter::CompositionMode_Xor );
        painter->setWindow( 0, 0, this->width(), this->height() );
        if ( mouseAxisVerticalAlignment == plotWidget::Bottom )
        {
            painter->drawLine( mousePosition.x(), mousePosition.y(), mousePosition.x(), this->height() );
        }
        else if ( mouseAxisVerticalAlignment == plotWidget::Top )
        {
            painter->drawLine( mousePosition.x(), mousePosition.y(), mousePosition.x(), 0 );
        }
        if ( mouseAxisHorizontalAlignment == plotWidget::Left )
        {
            painter->drawLine( mousePosition.x(), mousePosition.y(), 0, mousePosition.y() );
        }
        else if ( mouseAxisHorizontalAlignment == plotWidget::Right )
        {
            painter->drawLine( mousePosition.x(), mousePosition.y(), this->width(), mousePosition.y() );
        }
        painter->setCompositionMode( QPainter::CompositionMode_SourceOver );
    }

    QPixmap legend = generateLegend();
    if ( legendIsVisible ) //legend must be the first in z-order
    {
        QPoint legendPosition;
        if ( legendXAlignment == plotWidget::Left )
        {
            legendPosition.setX( plotWidget::DEFAULT_LEGEND_DISTANCE );
        }
        else
        {
            legendPosition.setX( this->width() - legend.width() - plotWidget::DEFAULT_LEGEND_DISTANCE ); //tolerance
        }
        if ( legendYAlignment == plotWidget::Top )
        {
            legendPosition.setY( plotWidget::DEFAULT_LEGEND_DISTANCE );
        }
        else
        {
            legendPosition.setY( this->height() - legend.height() - plotWidget::DEFAULT_LEGEND_DISTANCE ); //tolerance
        }
        painter->drawPixmap( legendPosition, legend );
    }
}

void
AbstractPlotArea::mousePressEvent( QMouseEvent* e )
{
    if ( e->button() == Qt::LeftButton )
    {
        leftMouseIsClicked = true;
        mousePosition.setX( e->pos().x() );
        mousePosition.setY( e->pos().y() );
        showMouseAxis = true;
        this->update();
        emit mouseIsClicked( e );
    }
//else if (e->button() == Qt::RightButton)
//{
    // emit rightMouseIsClicked();
//}
}

void
AbstractPlotArea::mouseReleaseEvent( QMouseEvent* )
{
    leftMouseIsClicked = false;
    showMouseAxis      = false;
    this->update();
}

void
AbstractPlotArea::mouseMoveEvent( QMouseEvent* e )
{
    if ( leftMouseIsClicked == true && e->pos().x() <= this->width() && e->pos().x() >= 0
         && e->pos().y() <= this->height() && e->pos().y() >= 0 )                 //rect.contains
    {
        mousePosition.setX( e->pos().x() );
        mousePosition.setY( e->pos().y() );
        showMouseAxis = true;
        this->update();
        emit mouseIsClicked( e );
    }
    else
    {
        showMouseAxis = false;
    }
}


void
AbstractPlotArea::setMouseAxisHorizontalAlignment( plotWidget::Alignment alignment )
{
    this->mouseAxisHorizontalAlignment = alignment;
}

plotWidget::Alignment
AbstractPlotArea::getMouseAxisHorizontalAlignment() const
{
    return mouseAxisHorizontalAlignment;
}

void
AbstractPlotArea::setMouseAxisVerticalAlignment( plotWidget::Alignment alignment )
{
    this->mouseAxisVerticalAlignment = alignment;
}

plotWidget::Alignment
AbstractPlotArea::getMouseAxisVerticalAlignment() const
{
    return mouseAxisVerticalAlignment;
}

void
AbstractPlotArea::setHMajorNotches( QList<int> HMajorNotches )
{
    this->HMajorNotches.clear();
    this->HMajorNotches.append( HMajorNotches );
}

QList<int> AbstractPlotArea::getHMajorNotches() const
{
    return this->HMajorNotches;
}

void
AbstractPlotArea::setVMajorNotches( QList<int> VMajorNotches )
{
    this->VMajorNotches.clear();
    this->VMajorNotches.append( VMajorNotches );
}

QList<int> AbstractPlotArea::getVMajorNotches() const
{
    return this->VMajorNotches;
}

void
AbstractPlotArea::setHMinorNotches( QList<int> HMinorNotches )
{
    this->HMinorNotches.clear();
    this->HMinorNotches.append( HMinorNotches );
}

QList<int> AbstractPlotArea::getHMinorNotches() const
{
    return this->HMinorNotches;
}

void
AbstractPlotArea::setVMinorNotches( QList<int> VMinorNotches )
{
    this->VMinorNotches.clear();
    this->VMinorNotches.append( VMinorNotches );
}

QList<int> AbstractPlotArea::getVMinorNotches() const
{
    return this->VMinorNotches;
}

void
AbstractPlotArea::setLegendVisible( bool legendIsVisible )
{
    this->legendIsVisible = legendIsVisible;
}

bool
AbstractPlotArea::isLegendVisible() const
{
    return legendIsVisible;
}

void
AbstractPlotArea::setLegendBackgroundSolid( bool legendBackgroundIsSolid )
{
    this->legendBackgroundIsSolid = legendBackgroundIsSolid;
}

bool
AbstractPlotArea::isLegendBackgroundSolid() const
{
    return legendBackgroundIsSolid;
}

void
AbstractPlotArea::setLegendXAlignment( plotWidget::Alignment legendXAlignment )
{
    this->legendXAlignment = legendXAlignment;
}

plotWidget::Alignment
AbstractPlotArea::getLegendXAlignment() const
{
    return this->legendXAlignment;
}

void
AbstractPlotArea::setLegendYAlignment( plotWidget::Alignment legendYAlignment )
{
    this->legendYAlignment = legendYAlignment;
}

plotWidget::Alignment
AbstractPlotArea::getLegendYAlignment() const
{
    return this->legendYAlignment;
}
bool
AbstractPlotArea::getDataToDisplay()
{
    return dataToDisplay;
}

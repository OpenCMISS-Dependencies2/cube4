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

#include "AbstractRuler.h"
#include <QPainter>
#include <QtCore/qmath.h>
#include <QtDebug>

const QFont  AbstractRuler:: RULER_TITLE_FONT = QFont( "Halventica", 10, -1, false );
const int    AbstractRuler::   LINE_PADDING   = 0;
const QFont  AbstractRuler:: MAJOR_NOTCH_FONT = QFont( "Halventica", 9, -1, false );
const QFont  AbstractRuler:: MINOR_NOTCH_FONT = QFont( "Halventica", 7, -1, false );
const QColor AbstractRuler::RULER_TITLE_COLOR = QColor( Qt::darkBlue );

AbstractRuler::AbstractRuler( QWidget* parent ) :
    QWidget( parent )
{
    initComponents();
}

void
AbstractRuler::initComponents()
{
    setTitle( tr( "Unknown" ), plotWidget::Center );
    isMousePressed  = false;
    isTitleVisible  = false;
    printRealValues = true;
    minimumLimit    = 0;
    maximumLimit    = 0;
}

AbstractRuler::~AbstractRuler()
{
}

void
AbstractRuler::setMajorNotchesIndexAndLocationF( QMap<double, int> newValue )
{
    printRealValues = true;
    majorNotchesIndexAndLocation.clear();
    for ( double key : newValue.keys() )
    {
        majorNotchesIndexAndLocation.insert( key, newValue[ key ] );
    }
}

void
AbstractRuler::setMinorNotchesIndexAndLocationF( QMap<double, int> newValue )
{
    printRealValues = true;
    minorNotchesIndexAndLocation.clear();
    for ( double key : newValue.keys() )
    {
        minorNotchesIndexAndLocation.insert( key, newValue[ key ] );
    }
}
// *******
void
AbstractRuler::clearMajorNotchesIndexAndLocation()
{
    printRealValues = false;
    majorNotchesIndexAndLocation.clear();
}
int
AbstractRuler::getSizeMajorNotchesIndexAndLocation()
{
    return majorNotchesIndexAndLocation.size();
}
void
AbstractRuler::clearMinorNotchesIndexAndLocation()
{
    printRealValues = false;
    minorNotchesIndexAndLocation.clear();
}
int
AbstractRuler::getSizeMinorNotchesIndexAndLocation()
{
    return minorNotchesIndexAndLocation.size();
}
// ***********
void
AbstractRuler::setMajorNotchesIndexAndLocation( QMap<int, int> newValue )
{
    printRealValues = false;
    majorNotchesIndexAndLocation.clear();
    for ( QMap<int, int>::iterator i = newValue.begin(); i != newValue.end(); i++ )
    {
        majorNotchesIndexAndLocation.insert( ( double )i.key(), i.value() );
    }
}

void
AbstractRuler::setMinorNotchesIndexAndLocation( QMap<int, int> newValue )
{
    printRealValues = false;
    minorNotchesIndexAndLocation.clear();
    for ( QMap<int, int>::iterator i = newValue.begin(); i != newValue.end(); i++ )
    {
        minorNotchesIndexAndLocation.insert( ( double )i.key(), i.value() );
    }
}

void
AbstractRuler::setMaximumLimitInPixels( int newMaximumLimitInPixels )
{
    this->maximumLimitInPixels = newMaximumLimitInPixels;
}

void
AbstractRuler::setTitle( QString title, plotWidget::Alignment align )
{
    this->title          = title;
    this->titleAlignment = align;
}

QString
AbstractRuler::getTitle()
{
    return title;
}

void
AbstractRuler::setTitleVisible( bool isVisible )
{
    isTitleVisible = isVisible;
    this->update();
}

void
AbstractRuler::mousePressEvent( QMouseEvent* )
{
    isMousePressed = true;
}

void
AbstractRuler::mouseReleaseEvent( QMouseEvent* )
{
    isMousePressed = false;
}

void
AbstractRuler::resizeEvent( QResizeEvent* )
{
    emit sizeChanged();
}

void
AbstractRuler::setLimits( double minimumLimit, double maximumLimit )
{
    // if(minimumLimit >= maximumLimit){
    if ( minimumLimit > maximumLimit )
    {
        //qDebug() <<  "Setting the limits is wrong, minimumLimit:" << minimumLimit << ", maximumLimit:" << maximumLimit;
        return;
    }
    this->minimumLimit = minimumLimit;
    this->maximumLimit = maximumLimit;
}

double
AbstractRuler::getMaximumLimit()
{
    return maximumLimit;
}

double
AbstractRuler::getMinimumLimit()
{
    return minimumLimit;
}

plotWidget::Alignment
AbstractRuler::getNotchesAlignment()
{
    return notchesAlignment;
}

void
AbstractRuler::setNotchesAligment( plotWidget::Alignment notchesAligment )
{
    this->notchesAlignment = notchesAligment;
}

void
AbstractRuler::setTitleAlignment( plotWidget::Alignment titleAlignment )
{
    this->titleAlignment = titleAlignment;
}

plotWidget::Alignment
AbstractRuler::getTitleAlignment()
{
    return titleAlignment;
}

/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
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

#include "CubeDataItem.h"
CubeDataItem::CubeDataItem()
{
    _isVisible  = false;
    _isScaled   = false;
    color       = Qt::green;
    style       = plotWidget::Dots;
    metricIndex = -1;
    zOrder      = -1;
    name        = QObject::tr( "Unknown" );
}

void
CubeDataItem::setColor( QColor color )
{
    this->color = color;
}

void
CubeDataItem::setStyle( plotWidget::Style style )
{
    this->style = style;
}

void
CubeDataItem::setScaled( bool isScaled )
{
    this->_isScaled = isScaled;
}

void
CubeDataItem::setMetricIndex( int index )
{
    this->metricIndex = index;
}

void
CubeDataItem::setScaleMinValue( double minScaleValue )
{
    this->scaleMinValue = minScaleValue;
}

void
CubeDataItem::setScaleMaxValue( double maxScaleValue )
{
    this->scaleMaxValue = maxScaleValue;
}

void
CubeDataItem::setData( QVector<double> data )
{
    this->data = data; //implicit sharing, but will cause COW later.
}

void
CubeDataItem::setVisible( bool isVisible )
{
    this->_isVisible = isVisible;
}

void
CubeDataItem::setZOrder( int zOrder )
{
    this->zOrder = zOrder;
}

void
CubeDataItem::setMathOp( plotWidget::MathOp mathOp )
{
    this->mathOp = mathOp;
}

void
CubeDataItem::setName( QString name )
{
    this->name = name;
}

void
CubeDataItem::setMaxValue( double maxValue )
{
    this->maxValue = maxValue;
}

void
CubeDataItem::setMinValue( double minValue )
{
    this->minValue = minValue;
}
void
CubeDataItem::setfirstItrID( unsigned int tempfirstItrID )
{
    this->firstItrID = tempfirstItrID;
}
void
CubeDataItem::setcalcType( cube::CalculationFlavour tempcalcType )
{
    this->calcType = tempcalcType;
}
bool
CubeDataItem::cubeDataItemOrderComparator( const CubeDataItem& firstElement, const CubeDataItem& secondElement )
{
    return firstElement.getZOrder() < secondElement.getZOrder();
}

QColor
CubeDataItem::getColor() const
{
    return this->color;
}

plotWidget::Style
CubeDataItem::getStyle() const
{
    return this->style;
}

bool
CubeDataItem::isScaled() const
{
    return this->_isScaled;
}

int
CubeDataItem::getMetricIndex() const
{
    return this->metricIndex;
}

double
CubeDataItem::getScaleMinValue() const
{
    return this->scaleMinValue;
}

double
CubeDataItem::getScaleMaxValue() const
{
    return this->scaleMaxValue;
}

QVector<double>&
CubeDataItem::getData()
{
    return this->data;
}

const QVector<double>&
CubeDataItem::getData() const
{
    return this->data;
}

bool
CubeDataItem::isVisible() const
{
    return _isVisible;
}

int
CubeDataItem::getZOrder() const
{
    return zOrder;
}

plotWidget::MathOp
CubeDataItem::getMathOp() const
{
    return mathOp;
}

QString
CubeDataItem::getName() const
{
    return name;
}

QString
CubeDataItem::toString() const
{
    return QObject::tr( "index: " ) + QString::number( this->getMetricIndex() ) + QObject::tr( " name: " ) + this->getName() + QObject::tr( " min: " ) + QString::number( this->getScaleMinValue() ) +
           QObject::tr( "max: " ) + QString::number( this->getScaleMaxValue() ) + QObject::tr( " style: " ) + QString::number( ( int )this->getStyle() );
}

double
CubeDataItem::getMaxValue() const
{
    return this->maxValue;
}

double
CubeDataItem::getMinValue() const
{
    return this->minValue;
}
unsigned int
CubeDataItem::getfirstItrID() const
{
    return this->firstItrID;
}
cube::CalculationFlavour
CubeDataItem::getcalcType() const
{
    return this->calcType;
}

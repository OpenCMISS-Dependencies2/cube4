/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"

#include "TreeItemMarker.h"
#include "TreeItem.h"
#include "MarkerLabel.h"

using namespace cubegui;

QList<MarkerAttributes> MarkerAttributes::definedMarker = QList<MarkerAttributes>();

MarkerAttributes::MarkerAttributes( const QColor& c )
{
    color = c;
}

const QColor&
MarkerAttributes::getColor() const
{
    return color;
}

void
MarkerAttributes::resetColors()
{
    definedMarker.clear();
}

void
MarkerAttributes::setColor( const QColor& color )
{
    this->color = color;
}

void
MarkerAttributes::release()
{
    if ( color.isValid() ) // ignore insignificant marker
    {
        definedMarker.insert( 0, *this );
    }
}

MarkerAttributes
MarkerAttributes::getMarkerAttributes()
{
    int alpha = 50;
    if ( definedMarker.isEmpty() )
    {
        definedMarker.append( MarkerAttributes( Qt::green ) );
        definedMarker.append( MarkerAttributes( Qt::cyan ) );
        definedMarker.append( MarkerAttributes( Qt::darkYellow ) );
        definedMarker.append( MarkerAttributes( Qt::darkMagenta ) );
        definedMarker.append( MarkerAttributes( Qt::blue ) );
        definedMarker.append( MarkerAttributes( Qt::red ) );
        for ( int i = 0; i < definedMarker.size(); i++ )
        {
            definedMarker[ i ].color.setAlpha( alpha );
        }
    }
    else if ( definedMarker.size() == 1 )
    {
        definedMarker.append( MarkerAttributes( Qt::red ) );
        definedMarker.last().color.setAlpha( alpha );
    }
    return definedMarker.takeFirst();
}

// =========================================================
bool TreeItemMarker::grayItems = false;

TreeItemMarker::TreeItemMarker( const QString& label, const QList<QPixmap>& icons, bool isInsignificant, MarkerLabel* markerLabel )
{
    this->label       = label;
    this->icons       = icons;
    attribute         = isInsignificant ? MarkerAttributes() : MarkerAttributes::getMarkerAttributes();
    currentIcon       = QPixmap();
    showColor         = !isInsignificant;
    showIcon          = true;
    insignificant     = isInsignificant;
    this->markerLabel = markerLabel;
}

TreeItemMarker::~TreeItemMarker()
{
    attribute.release();
}

const MarkerAttributes&
TreeItemMarker::getAttributes() const
{
    return attribute;
}

QString
TreeItemMarker::getLabel( const TreeItem* item ) const
{
    if ( markerLabel && item )
    {
        return markerLabel->getLabel( item );
    }
    else
    {
        return label;
    }
}

const QPixmap
TreeItemMarker::getIcon() const
{
    return currentIcon;
}

void
TreeItemMarker::setSize( int iconHeight )
{
    if ( icons.size() == 0 || !showIcon )
    {
        currentIcon = QPixmap();
        return;
    }

    foreach( QPixmap p, icons )
    {
        if ( p.size().height() > iconHeight )
        {
            currentIcon = p;
            break;
        }
    }

    if ( !currentIcon.isNull() )
    {
        currentIcon = currentIcon.scaledToHeight( iconHeight );
    }
}

void
TreeItemMarker::setColorVisible( bool visible )
{
    showColor = visible;
}

void
TreeItemMarker::setIconVisible( bool visible )
{
    showIcon = visible;
}

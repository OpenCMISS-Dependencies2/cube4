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


#include <QPainter>
#include "Globals.h"
#include "ColorScale.h"

using namespace cubegui;

ColorScale::ColorScale() : QWidget()
{
    setMinimumHeight( 20 );
    setMinimumWidth( 100 );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    setWhatsThis( tr( "By default, the colors are taken from a spectrum ranging from blue over cyan, green, and yellow to red, representing the whole range of possible values.  You can change the color settings in the menu, see menu Display -> Coloring -> General coloring. Exact zero values are represented by the color white (in topologies you can decide if you would like to use white or the minimal color, see menu Display -> Coloring -> Topology coloring)." ) );
}

//this method paints the color scale
void
ColorScale::paintEvent( QPaintEvent* )
{
    //generate the color gradient
    QLinearGradient linearGrad( QPointF( 0, 0 ), QPointF( width(), 0 ) );
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    QList<QGradientStop> gradStops;
#else
    QVector<QGradientStop> gradStops;
#endif

    unsigned numGradStops = 500;
    for ( unsigned i = 0; i <= numGradStops; i++ )
    {
        gradStops.push_back( QPair<qreal, QColor>( ( double )i / ( double )numGradStops,
                                                   Globals::getColor( ( double )i, 0.0, ( double )numGradStops ) ) );
    }
    linearGrad.setStops( gradStops );

    //draw the color gradient
    QPainter painter( this );
    painter.fillRect( rect(), linearGrad );
}

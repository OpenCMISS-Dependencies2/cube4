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


#include <cassert>

#include <QDialog>
#include "ColorMapPlugin.h"

using namespace colormapplugin;
using namespace cubepluginapi;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( ColorMapPlugin, ColorMapPlugin );
#endif

void
ColorMapPlugin::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

QString
ColorMapPlugin::name() const
{
    return "Colormap";
}

bool
ColorMapPlugin::cubeOpened( PluginServices* service )
{
    service->addColorMap( this );
    return true;
}

void
ColorMapPlugin::cubeClosed()
{
}

QString
ColorMapPlugin::getHelpText() const
{
    return "colormap help text";
}

QColor
ColorMapPlugin::getColor( double value, double minValue, double maxValue, bool ) const
{
    double percent = value / ( maxValue - minValue );
    QColor gray;
    gray.setHsvF( 0, 0, 1 - percent );
    return gray;
}

QString
ColorMapPlugin::getMapName() const
{
    return "grayscale colormap";
}

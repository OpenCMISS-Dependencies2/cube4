/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#include "config.h"

#include <cmath>
#include "../plugins/AdvancedColorMaps/AdvancedColorMaps.h"

using std::floor;
using namespace advancedcolormaps;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( AdvancedColorMapsPlugin, AdvancedColorMaps );
#endif

AdvancedColorMaps::AdvancedColorMaps()
{
    colorMapSettings = 0;
}

AdvancedColorMaps::~AdvancedColorMaps()
{
    delete colorMapSettings;
}

QColor
AdvancedColorMaps::getColor( double value,
                             double minValue,
                             double maxValue,
                             bool   whiteForZero ) const
{
    // proxy - call internal state
    QColor q = currentColorMap->getColor( value, minValue, maxValue, whiteForZero );
    int    r, g, b;
    q.getRgb( &r, &g, &b );
    return q;
}

QString
AdvancedColorMaps::getMapName() const
{
    return tr( "Advanced Color Maps" );
}

bool
AdvancedColorMaps::dialogAvailable() const
{
    return true;
}

void
AdvancedColorMaps::showDialog()
{
    colorMapSettings->show();
}

bool
AdvancedColorMaps::cubeOpened( PluginServices* service )
{
    if ( !colorMapSettings )
    {
        colorMapSettings = new AdvancedColorMapsSettings();
        currentColorMap  = colorMapSettings->getCurrentColorMap();
        // GUI - changed color map
        QObject::connect( colorMapSettings, SIGNAL( changeColorMap( cubepluginapi::ColorMap* ) ),
                          this, SLOT( updateColorMap( cubepluginapi::ColorMap* ) ) );
        // GUI - changed preferences of selected color map
        QObject::connect( colorMapSettings, SIGNAL( colorMapChanged() ),
                          this, SLOT( applyChanges() ) );
    }
    service->addColorMap( this );
    service->addSettingsHandler( colorMapSettings );
    return true;
}

void
AdvancedColorMaps::version( int& major,
                            int& minor,
                            int& bugfix ) const
{
    major  = 0;
    minor  = 1;
    bugfix = 0;
}

QString
AdvancedColorMaps::name() const
{
    return "Advanced Color Maps";
}

QString
AdvancedColorMaps::getHelpText() const
{
    return tr( "Contains implementations of various color maps." );
}

void
AdvancedColorMaps::updateColorMap( ColorMap* map )
{
    currentColorMap = map;
}

void
AdvancedColorMaps::applyChanges()
{
    emit colorMapChanged();
}

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

#include <stdexcept>

#include "maps/SequentialColorMap.h"
#include "maps/DivergentColorMap.h"
#include "maps/CubehelixColorMap.h"
#include "maps/ImprovedRainbowColorMap.h"
#include "ColorMapsFactory.h"

using std::runtime_error;

const ColorMapsFactory::ColorMapsNames ColorMapsFactory::mapNames = ColorMapsFactory::initializeMap();

// ColorMapsFactory & ColorMapsFactory::getInstance()
// {
//	static ColorMapsFactory    instance;
//	return instance;
// }

ColorMapsFactory::~ColorMapsFactory()
{
    for ( std::map<ColorMaps, ColorMapExtended*>::iterator it = mapObjects.begin();
          it != mapObjects.end(); ++it )
    {
        delete ( *it ).second;
    }
}

ColorMapsFactory::ColorMapsNames
ColorMapsFactory::initializeMap()
{
    ColorMapsFactory::ColorMapsNames ret;
    ret[ SEQUENTIAL ]       = QObject::tr( "Sequential" ).toUtf8().data();
    ret[ DIVERGENT ]        = QObject::tr( "Divergent" ).toUtf8().data();
    ret[ CUBEHELIX ]        = QObject::tr( "Cubehelix" ).toUtf8().data();
    ret[ IMPROVED_RAINBOW ] = QObject::tr( "Improved rainbow" ).toUtf8().data();
    return ret;
}

ColorMapExtended*
ColorMapsFactory::getColorMap( ColorMaps map )
{
    std::map<ColorMaps, ColorMapExtended*>::iterator it = mapObjects.find( map );
    if ( it != mapObjects.end() )
    {
        // filtering values may have been changed in other widget
        return ( *it ).second;
    }
    // map currently doesn't exist, create the instance
    ColorMapExtended* ret = NULL;
    switch ( map )
    {
        case SEQUENTIAL:
            ret = new SequentialColorMap();
            break;
        case DIVERGENT:
            ret = new DivergentColorMap();
            break;
        case CUBEHELIX:
            ret = new CubehelixColorMap();
            break;
        case IMPROVED_RAINBOW:
            ret = new ImprovedRainbowColorMap();
            break;
        default:
            throw runtime_error( QObject::tr( "Illegal value of enum ColorMapsFactory::ColorMaps!" ).toUtf8().data() );
            break;
    }
    mapObjects[ map ] = ret;
    return ret;
}

/**
 * @param color map instance
 * @return enum value corresponding to this color map
 */
ColorMapsFactory::ColorMaps
ColorMapsFactory::getColorMapIndex( ColorMapExtended* map )
{
    if ( dynamic_cast<DivergentColorMap*>( map ) )
    {
        return DIVERGENT;
    }
    else if ( dynamic_cast<SequentialColorMap*>( map ) )
    {
        return SEQUENTIAL;
    }
    else if ( dynamic_cast<ImprovedRainbowColorMap*>( map ) )
    {
        return IMPROVED_RAINBOW;
    }
    else
    {
        return CUBEHELIX;
    }
}

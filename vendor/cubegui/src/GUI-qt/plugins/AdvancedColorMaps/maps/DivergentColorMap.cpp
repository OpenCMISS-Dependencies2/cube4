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

// using std::max;
// using std::min;

#include "DivergentColorMap.h"

const PSMap DivergentColorMap::DIVERGENT_PREDEFINED_SCHEMES =
    DivergentColorMap::divergentInitializePredefinedSchemes();

const QString DivergentColorMap::DIVERGENT_COLOR_MAP_NAME =
    tr( "Divergent Color Map" );
const QString DivergentColorMap::DIVERGENT_COLOR_MAP_DESCRIPTION =
    tr( "Divergent color maps are configured by two colors - start and end - with an interpolation"
        " method. The colour distribution goes from starting to ending color, with a pure white in the"
        " middle. <br>"
        "Predefined schemes provide exemplary usage of color map. "
        "One may select any other RGB color for start or end by double click on "
        "the plot. <br>"
        "This kind of color map is usually used for representation of data with a critical value "
        "in the middle or for the depiction of data deviation around some point. " );

PSMap
DivergentColorMap::divergentInitializePredefinedSchemes()
{
    PSMap ret;

    /**
     * Black
     */
    QColor   rgb           = QColor::fromRgb( 1, 1, 1 );
    MSHColor startingPoint = MSHColor::fromRGB( rgb );
    ret.insert( std::make_pair( tr( "Greyscale" ).toUtf8().data(), std::make_pair( startingPoint, startingPoint ) ) );

    /**
     * Blue
     */
    rgb           = QColor::fromRgb( 85, 170, 255 );
    startingPoint = MSHColor::fromRGB( rgb );
    ret.insert( std::make_pair( tr( "Blues" ).toUtf8().data(), std::make_pair( startingPoint, startingPoint ) ) );

    /**
     * Orange
     */
    rgb           = QColor::fromRgb( 255, 85, 0 );
    startingPoint = MSHColor::fromRGB( rgb );
    ret.insert( std::make_pair( tr( "Oranges" ).toUtf8().data(), std::make_pair( startingPoint, startingPoint ) ) );

    /**
     * Red
     */
    rgb           = QColor::fromRgb( 255, 0, 0 );
    startingPoint = MSHColor::fromRGB( rgb );
    ret.insert( std::make_pair( tr( "Reds" ).toUtf8().data(), std::make_pair( startingPoint, startingPoint ) ) );

    /**
     * Green
     */
    rgb           = QColor::fromRgb( 0, 68, 26 );
    startingPoint = MSHColor::fromRGB( rgb );
    ret.insert( std::make_pair( tr( "Greens" ).toUtf8().data(), std::make_pair( startingPoint, startingPoint ) ) );

    /**
     * Purple
     */
    rgb           = QColor::fromRgb( 63, 0, 125 );
    startingPoint = MSHColor::fromRGB( rgb );
    ret.insert( std::make_pair( tr( "Purples" ).toUtf8().data(), std::make_pair( startingPoint, startingPoint ) ) );

    return ret;
}

DivergentColorMap::DivergentColorMap() :
    SequentialColorMap( new DivergentColorMapWidget( *this ),
                        DIVERGENT_PREDEFINED_SCHEMES,
                        DIVERGENT_COLOR_MAP_NAME,
                        DIVERGENT_COLOR_MAP_DESCRIPTION
                        )
{
    getWidget().constructAndInitializeData();
}

DivergentColorMap::~DivergentColorMap()
{
}

QColor
DivergentColorMap::getColor( double value, double minValue,
                             double maxValue, bool whiteForZero ) const
{
    pair<unsigned short, QColor> preprocess = ColorMapExtended::getColorBasicProcessing( value,
                                                                                         minValue, maxValue, whiteForZero );
    // preprocess returns value
    if ( preprocess.first == 2 )
    {
        return preprocess.second;
    }
    double               position;
    pair<double, double> minMax         = adjustFilteringBorders( minValue, maxValue );
    double               markerPosition = markersPositions->getMiddleMarker( minValue, maxValue );
    minValue = minMax.first;
    maxValue = minMax.second;
    double divergencePosition_ = ( markerPosition - minValue ) / ( maxValue - minValue );

    // usual interpolation
    if ( preprocess.first == 0 )
    {
        position = ( value - minValue ) / ( maxValue - minValue );

        if ( invertedCM )
        {
            position = 1.0 - position;
        }

        // first half
        if ( position < divergencePosition_ )
        {
            // convert position from [0, divergencePosition] -> [0, 1]
            position /= divergencePosition_;
            if ( interMethod == EXPONENTIAL )
            {
                return MSHColor::interpolateExp( currentScheme.first, MSHColor::WHITE, position ).toRGB();
            }
            else
            {
                return MSHColor::interpolate( currentScheme.first, MSHColor::WHITE, position ).toRGB();
            }
        }
        // second half of divergence
        else
        {
            // convert position from [divergencePosition, 1] -> [0, 1]
            position = ( position - divergencePosition_ ) / ( 1 - divergencePosition_ );
            if ( interMethod == EXPONENTIAL )
            {
                return MSHColor::interpolateExp( MSHColor::WHITE, currentScheme.second, position ).toRGB();
            }
            else
            {
                return MSHColor::interpolate( MSHColor::WHITE, currentScheme.second, position ).toRGB();
            }
        }
    }
    // case when maxValue == minValue
    else
    {
        position = preprocess.second.red() / 255.0;
        return MSHColor::interpolate( currentScheme.first, currentScheme.second, position ).toRGB();
    }
}

void
DivergentColorMap::saveGlobalSettings( QSettings& settings )
{
    ColorMapExtended::saveGlobalSettings( settings );
    getWidget().getCurrentPlot()->saveMiddleMarker( settings, "Divergent" );
    saveUDSchemes( "Divergent", settings );
    saveCurrentColorMap( "Divergent", settings );
}

void
DivergentColorMap::loadGlobalSettings( QSettings& settings )
{
    ColorMapExtended::loadGlobalSettings( settings );
    getWidget().getCurrentPlot()->loadMiddleMarker( settings, "Divergent" );
    loadUDSchemes( "Divergent", settings );
    loadCurrentColorMap( "Divergent", settings );
    widget->parentUpdated();
}

DivergentColorMapWidget::DivergentColorMapWidget( DivergentColorMap& parent ) :
    SequentialColorMapWidget( parent )
{
}

DivergentColorMapWidget::~DivergentColorMapWidget()
{
}

DivergentColorMap&
DivergentColorMapWidget::getParent() const
{
    return dynamic_cast<DivergentColorMap&>( parent );
}

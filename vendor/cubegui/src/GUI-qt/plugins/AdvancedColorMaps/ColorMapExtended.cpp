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
#include <sstream>

#include "ColorMapExtended.h"

using std::pow;
using std::max;
using std::min;
using std::fabs;

const QColor ColorMapExtended::DEFAULT_COLOR_VALUES_OUT_OF_RANGE = Qt::gray;
QColor       ColorMapExtended::      colorValuesOutOfRange       = DEFAULT_COLOR_VALUES_OUT_OF_RANGE;
// bool ColorMapExtended::enableFilters[2] = { false, false};

/**
 * Default colors
 */
const ColorMapExtended::MSHColor ColorMapExtended::MSHColor::WHITE = MSHColor::fromRGB( Qt::white );

const double ColorMapExtended::CIELABColor::WHITEPOINT_X = 0.95047;
const double ColorMapExtended::CIELABColor::WHITEPOINT_Y = 1.0;
const double ColorMapExtended::CIELABColor::WHITEPOINT_Z = 1.08883;

const double ColorMapExtended::CIELABColor::RGBXYZ_CONVERSION_MATRIX[ 3 ][ 3 ] = {
    { 0.412453, 0.357580, 0.180423 },
    { 0.212671, 0.715160, 0.072169 },
    { 0.019334, 0.119193, 0.950227 }
};

ColorMapExtended::ColorMapExtended() :
    invertedCM( false )
{
    // initialize widget in some other place!
    widget           = NULL;
    markersPositions = NULL;
}

ColorMapExtended::~ColorMapExtended()
{
    delete widget;
}

void
ColorMapExtended::invertColorMap()
{
    invertedCM = !invertedCM;
}

bool
ColorMapExtended::invertedColorMap()
{
    return invertedCM;
}

void
ColorMapExtended::setColorForValuesOutOfRange( const QColor& color )
{
    colorValuesOutOfRange = color;
}

QColor
ColorMapExtended::getColorForValuesOutOfRange()
{
    return colorValuesOutOfRange;
}


void
ColorMapExtended::enableFiltering( ColorMapPlot::MarkersPositions::FilterPosition, bool )
{
    /**
     * Change design - always enabled.
     */
    // enableFilters[pos] = val;
}

bool
ColorMapExtended::isFilteringEnabled( ColorMapPlot::MarkersPositions::FilterPosition )
{
    /**
     * Change design - always enabled.
     */
    return true;
    // return enableFilters[pos];
}


void
ColorMapExtended::setMarkersPositions( ColorMapPlot::MarkersPositions* pos )
{
    markersPositions = pos;
}

pair<double, double>
ColorMapExtended::adjustFilteringBorders( double minValue, double maxValue ) const
{
    double oldMax = maxValue;
    maxValue = markersPositions->adjustFiltering( minValue, oldMax,
                                                  isFilteringEnabled( ColorMapPlot::MarkersPositions::FILTER_MAX ),
                                                  ColorMapPlot::MarkersPositions::FILTER_MAX );
    minValue = markersPositions->adjustFiltering( minValue, oldMax,
                                                  isFilteringEnabled( ColorMapPlot::MarkersPositions::FILTER_MIN ),
                                                  ColorMapPlot::MarkersPositions::FILTER_MIN );
    return pair<double, double>( minValue, maxValue );
}

void
ColorMapExtended::saveGlobalSettings( QSettings& settings )
{
    settings.setValue( "invert", invertedCM );
}

void
ColorMapExtended::loadGlobalSettings( QSettings& settings )
{
    invertedCM = settings.value( "invert", false ).toBool();
}

pair<unsigned short, QColor>
ColorMapExtended::getColorBasicProcessing( double value, double minValue,
                                           double maxValue, bool whiteForZero ) const
{
    /**
     * Code taken from src/GUI-qt/display/Coloring.h, class DefaultColorMap,
     * method getColor.
     * Handles basic cases.
     */
    double threshold = 0.000001;
    double _minValue = markersPositions->adjustFiltering( minValue, maxValue,
                                                          isFilteringEnabled( ColorMapPlot::MarkersPositions::FILTER_MIN ),
                                                          ColorMapPlot::MarkersPositions::FILTER_MIN );
    double _maxValue = markersPositions->adjustFiltering( minValue, maxValue,
                                                          isFilteringEnabled( ColorMapPlot::MarkersPositions::FILTER_MAX ),
                                                          ColorMapPlot::MarkersPositions::FILTER_MAX );

    // if the value is out of range, return gray
    if ( value < _minValue - threshold || value > _maxValue + threshold )
    {
        return std::make_pair( 2, colorValuesOutOfRange );
    }

    // normally we return white for zero values, but
    // for topology widgets optionally only if all thread values are zero
    if ( value == 0.0 && ( whiteForZero || ( minValue == 0.0 && maxValue == 0.0 ) ) )
    {
        return std::make_pair( 2, Qt::white );
    }

    double x;
    if ( minValue == maxValue )
    {
        if ( value == 0.0 )
        {
            x = 0.0;
        }
        else
        {
            x = 1.0;
        }
        return std::make_pair( 1, QColor( x * 255, 0, 0 ) );
    }
    // second argument will be ignored
    return std::make_pair( 0, Qt::white );
}

/**
 * CIELAB
 */

ColorMapExtended::CIELABColor::CIELABColor( double _lightness,
                                            double _a,
                                            double _b ) :
    lightness( _lightness ), a( _a ), b( _b )
{
}

ColorMapExtended::CIELABColor::CIELABColor( const CIELABColor& c ) :
    lightness( c.lightness ), a( c.a ), b( c.b )
{
}

ColorMapExtended::CIELABColor
ColorMapExtended::CIELABColor::fromRGB( const QColor& rgb_color )
{
    /**
     * Source of the algorithm:
     * - Wikipedia:
     * http://en.wikipedia.org/wiki/SRGB
     * http://en.wikipedia.org/wiki/Lab_color_space
     * http://www.cs.rit.edu/~ncs/color/t_convert.html
     * http://www.sandia.gov/~kmorel/documents/ColorMaps/
     */

    /**
     * First, convert RGB color to physically linear RGB
     */
    int r, g, b;
    rgb_color.getRgb( &r, &g, &b );
    double rgb[ 3 ] = { r / 255.0, g / 255.0, b / 255.0 };
    for ( int i = 0; i < 3; ++i )
    {
        if ( rgb[ i ] > 0.04045 )
        {
            rgb[ i ] = pow( ( rgb[ i ] + 0.055 ) / 1.055, 2.4 );
        }
        else
        {
            rgb[ i ] /= 12.92;
        }
    }

    /**
     * Convert from linear RGB to XYZ by multiplying with conversion matrix.
     */
    double xyz[ 3 ] = { 0, 0, 0 };
    for ( int i = 0; i < 3; ++i )
    {
        for ( int j = 0; j < 3; ++j )
        {
            xyz[ i ] += RGBXYZ_CONVERSION_MATRIX[ i ][ j ] * rgb[ j ];
        }
    }

    /**
     * Normalize for the referenced white point.
     */
    xyz[ 0 ] /= WHITEPOINT_X;
    xyz[ 1 ] /= WHITEPOINT_Y;
    xyz[ 2 ] /= WHITEPOINT_Z;

    /**
     * Now convert CIEXYZ to CIELAB
     */
    double lightness = 116 * cielabConversionFunction( xyz[ 1 ] ) - 16;
    double a_star    = 500 * ( cielabConversionFunction( xyz[ 0 ] ) - cielabConversionFunction( xyz[ 1 ] ) );
    double b_star    = 200 * ( cielabConversionFunction( xyz[ 1 ] ) - cielabConversionFunction( xyz[ 2 ] ) );

    return CIELABColor( lightness, a_star, b_star );
}

double
ColorMapExtended::CIELABColor::cielabConversionFunction( const double& x )
{
    // check against threshold value (6/29)^3
    if ( x > 0.008856 )
    {
        // x^(1/3)
        return pow( x, 1.0 / 3 );
    }
    else
    {
        // 1/3 * (29/6)^2 * x + 4/29
        return 7.787037 * x + 0.137931;
    }
}

double
ColorMapExtended::CIELABColor::cielabConversionFunctionInverse( const double& x )
{
    // check against threshold value (6/29)^3*1/3*(29/6)^2 + 4/29 = 6/29
    if ( x > 0.206797 )
    {
        // x^3
        return pow( x, 3 );
    }
    else
    {
        // 3 * (6/29)^2 * (x - 4/29)
        return 0.128419 * ( x - 0.137931 );
    }
}

QColor
ColorMapExtended::CIELABColor::toRGB() const
{
    /**
     * Source of the algorithm:
     * - Wikipedia:
     * http://en.wikipedia.org/wiki/SRGB
     * http://en.wikipedia.org/wiki/Lab_color_space
     * http://www.poynton.com/notes/colour_and_gamma/ColorFAQ.html#RTFToC18
     */
    /**
     * First, convert CIELAB to CIEXYZ
     */
    double xyz[ 3 ];
    xyz[ 1 ] = WHITEPOINT_Y * cielabConversionFunctionInverse( 0.008621 * ( lightness + 16 ) );
    xyz[ 0 ] = WHITEPOINT_X * cielabConversionFunctionInverse( 0.008621 * ( lightness + 16 ) + a / 500 );
    xyz[ 2 ] = WHITEPOINT_Z * cielabConversionFunctionInverse( 0.008621 * ( lightness + 16 ) - b / 200 );

    /**
     * Now convert CIEXYZ to sRGB
     */
    double rgb[ 3 ];
    rgb[ 0 ] = 3.2406 * xyz[ 0 ] - 1.5372 * xyz[ 1 ] - 0.4986 * xyz[ 2 ];
    rgb[ 1 ] = -0.9689 * xyz[ 0 ] + 1.8758 * xyz[ 1 ] + 0.0415 * xyz[ 2 ];
    rgb[ 2 ] = 0.0557 * xyz[ 0 ] - 0.204 * xyz[ 1 ] + 1.057 * xyz[ 2 ];

    /**
     * Normalize for real-world monitor
     */
    for ( int i = 0; i < 3; ++i )
    {
        if ( rgb[ i ] < 0.0031308 )
        {
            rgb[ i ] = 12.92 * rgb[ i ];
        }
        else
        {
            rgb[ i ] = 1.055 * pow( rgb[ i ], 0.416667 ) - 0.055;
        }
        // cielab contains broader range of colors
        rgb[ i ] = max( min( rgb[ i ], 1.0 ), 0.0 );
        // scale from 0-1 to 0-255
        rgb[ i ] = round( rgb[ i ] * 255 );
    }
    return QColor::fromRgb( rgb[ 0 ], rgb[ 1 ], rgb[ 2 ] );
}

std::string
ColorMapExtended::CIELABColor::toString() const
{
    std::stringstream str;
    str << tr( "Lightness: " ).toUtf8().data() << lightness;
    str << " a: " << a;
    str << " b: " << b;
    return str.str();
}

void
ColorMapExtended::CIELABColor::setLightness( const double& l )
{
    lightness = l;
}

double
ColorMapExtended::CIELABColor::getLightness() const
{
    return lightness;
}

double
ColorMapExtended::CIELABColor::getAStar() const
{
    return a;
}

double
ColorMapExtended::CIELABColor::getBStar() const
{
    return b;
}

/**
 * MSH color space
 */

ColorMapExtended::MSHColor::MSHColor( double _m,
                                      double _s,
                                      double _h ) :
    m( _m ), s( _s ), h( _h )
{
}

ColorMapExtended::MSHColor::MSHColor( const MSHColor& c ) :
    m( c.m ), s( c.s ), h( c.h )
{
}

ColorMapExtended::MSHColor::MSHColor() :
    m( WHITE.getM() ), s( WHITE.getS() ), h( WHITE.getH() )
{
}

ColorMapExtended::MSHColor
ColorMapExtended::MSHColor::fromRGB( const QColor& color )
{
    CIELABColor cielab = CIELABColor::fromRGB( color );
    /**
     * vector length of all variables from cielab
     */
    double m = pow( cielab.getLightness(), 2 );
    m += pow( cielab.getAStar(), 2 );
    m += pow( cielab.getBStar(), 2 );
    m  = sqrt( m );
    double s = acos( cielab.getLightness() / m );
    /**
     * Here is the problem - atan described in the article works fine for
     * situations where b* and a* > 0 or b* < 0 and a* > 0.
     * However, it fails when a* < 0 and b* > 0 - it should be angle of vector projection
     * on plane a* and b*. Therefore we have to use atan2.
     */
    double h = atan2( cielab.getBStar(), cielab.getAStar() );

    return MSHColor( m, s, h );
}

QColor
ColorMapExtended::MSHColor::toRGB() const
{
    return toCIELAB().toRGB();
}

ColorMapExtended::CIELABColor
ColorMapExtended::MSHColor::toCIELAB() const
{
    double lightness = m * cos( s );
    double a_star    = m * sin( s ) * cos( h );
    double b_star    = m * sin( s ) * sin( h );

    return CIELABColor( lightness, a_star, b_star );
}

std::string
ColorMapExtended::MSHColor::toString() const
{
    std::stringstream str;
    str << "M: " << m;
    str << " s: " << s;
    str << " h: " << h;
    return str.str();
}

ColorMapExtended::MSHColor
ColorMapExtended::MSHColor::interpolate(
    const MSHColor& start,
    const MSHColor& end,
    double          interpolation_place )
{
    pair<double, double> h_values = adjustHues( start, end );

    double start_vals[ 3 ] = { start.m, start.s, h_values.first };
    double end_vals[ 3 ]   = { end.m, end.s, h_values.second };
    double vals[ 3 ];
    for ( int i = 0; i < 3; ++i )
    {
        vals[ i ] = start_vals[ i ] * ( 1 - interpolation_place ) + end_vals[ i ] * interpolation_place;
    }

    return MSHColor( vals[ 0 ], vals[ 1 ], vals[ 2 ] );
}

ColorMapExtended::MSHColor
ColorMapExtended::MSHColor::interpolateExp(
    const MSHColor& start,
    const MSHColor& end,
    double          interpolation_place )
{
    pair<double, double> h_values = adjustHues( start, end );

    double start_vals[ 3 ] = { start.m, start.s, h_values.first };
    double end_vals[ 3 ]   = { end.m, end.s, h_values.second };
    double vals[ 3 ];
    interpolation_place = ( exp( interpolation_place ) - 1.0 ) /
                          ( exp( 1.0 ) - 1.0 );
    for ( int i = 0; i < 3; ++i )
    {
        vals[ i ] = start_vals[ i ] * ( 1 - interpolation_place ) + end_vals[ i ] * interpolation_place;
    }

    return MSHColor( vals[ 0 ], vals[ 1 ], vals[ 2 ] );
}

ColorMapExtended::MSHColor
ColorMapExtended::MSHColor::interpolate(
    const MSHColor& start,
    const MSHColor& end,
    double          interpolation_place,
    double          marker_position )
{
    pair<double, double> h_values = adjustHues( start, end );

    double interpolationParameter = marker_position;
    double start_vals[ 3 ]        = { start.m, start.s, h_values.first };
    double end_vals[ 3 ]          = { end.m, end.s, h_values.second };
    // midpoint
    double vals[ 3 ];
    for ( int i = 0; i < 3; ++i )
    {
        vals[ i ] = start_vals[ i ] * 0.5 + end_vals[ i ] * 0.5;
    }

    if ( interpolation_place < interpolationParameter )
    {
        // scale to new interval [0, 1) from [0, interParam)
        double new_interpolation_place = interpolation_place / interpolationParameter;

        for ( int i = 0; i < 3; ++i )
        {
            vals[ i ] = ( 1 - new_interpolation_place ) * start_vals[ i ] + new_interpolation_place * vals[ i ];
        }
    }
    else
    {
        // scale to new interval [0, 1) from [interParam, 1)
        // position scaled by the length of interval
        double new_interpolation_place = ( interpolation_place - interpolationParameter ) /
                                         ( 1 - interpolationParameter );

        for ( int i = 0; i < 3; ++i )
        {
            vals[ i ] = ( 1 - new_interpolation_place ) * vals[ i ] + new_interpolation_place * end_vals[ i ];
        }
    }

    return MSHColor( vals[ 0 ], vals[ 1 ], vals[ 2 ] );
}

ColorMapExtended::MSHColor
ColorMapExtended::MSHColor::interpolateExp(
    const MSHColor& start,
    const MSHColor& end,
    double          interpolation_place,
    double          marker_position )
{
    pair<double, double> h_values = adjustHues( start, end );

    double interpolationParameter = marker_position;
    double start_vals[ 3 ]        = { start.m, start.s, h_values.first };
    double end_vals[ 3 ]          = { end.m, end.s, h_values.second };
    // midpoint
    double vals[ 3 ];
    for ( int i = 0; i < 3; ++i )
    {
        vals[ i ] = start_vals[ i ] * 0.5 + end_vals[ i ] * 0.5;
    }

    if ( interpolation_place < interpolationParameter )
    {
        // scale to new interval [0, 1) from [0, interParam)
        double new_interpolation_place = interpolation_place / interpolationParameter;
        new_interpolation_place = ( exp( new_interpolation_place ) - 1.0 ) /
                                  ( exp( 1.0 ) - 1.0 );
        for ( int i = 0; i < 3; ++i )
        {
            vals[ i ] = ( 1 - new_interpolation_place ) * start_vals[ i ] + new_interpolation_place * vals[ i ];
        }
    }
    else
    {
        // scale to new interval [0, 1) from [interParam, 1)
        // position scaled by the length of interval
        double new_interpolation_place = ( interpolation_place - interpolationParameter ) /
                                         ( 1 - interpolationParameter );
        new_interpolation_place = ( exp( new_interpolation_place ) - 1.0 ) /
                                  ( exp( 1.0 ) - 1.0 );

        for ( int i = 0; i < 3; ++i )
        {
            vals[ i ] = ( 1 - new_interpolation_place ) * vals[ i ] + new_interpolation_place * end_vals[ i ];
        }
    }

    return MSHColor( vals[ 0 ], vals[ 1 ], vals[ 2 ] );
}

double
ColorMapExtended::MSHColor::getM() const
{
    return m;
}

double
ColorMapExtended::MSHColor::getS() const
{
    return s;
}

double
ColorMapExtended::MSHColor::getH() const
{
    return h;
}

bool
ColorMapExtended::MSHColor::compare( const MSHColor& other ) const
{
    static const double DOUBLE_EPS = 1e-5;
    if ( fabs( other.getM() - getM() ) < DOUBLE_EPS &&
         fabs( other.getS() - getS() ) < DOUBLE_EPS &&
         fabs( other.getH() - getH() ) < DOUBLE_EPS )
    {
        return true;
    }
    else
    {
        return false;
    }
}

double
ColorMapExtended::MSHColor::adjustHue( const MSHColor& colorSat, double Munsat )
{
    /**
     * Algorithm from 'Diverging Color Maps for...' publication
     */
    if ( colorSat.getM() >= Munsat )
    {
        return colorSat.getH();
    }
    else
    {
        double hsat  = colorSat.getH();
        double hSpin = colorSat.getS() * sqrt( pow( Munsat, 2.0 ) - pow( colorSat.getM(), 2.0 ) ) / \
                       ( colorSat.getM() * sin( colorSat.getS() ) );
        if ( hsat > -M_PI / 3 )
        {
            return hsat + hSpin;
        }
        else
        {
            return hsat - hSpin;
        }
    }
}

pair<double, double>
ColorMapExtended::MSHColor::adjustHues( const MSHColor& start, const MSHColor& end )
{
    double s1 = start.getS();
    double s2 = end.getS();
    double h1 = start.getH();
    double h2 = end.getH();

    // unsaturated colors
    if ( s1 < 0.05 && s2 > 0.05 )
    {
        h1 = adjustHue( end, start.getM() );
    }
    else if ( s2 < 0.05 && s1 > 0.05 )
    {
        h2 = adjustHue( start, end.getM() );
    }

    return std::make_pair( h1, h2 );
}

/**
 * ColorMapWidget class
 */

/*
   const QString ColorMapWidget::FILTER_CHECKBOX_NAMES[2] = {
                QString::fromStdString("Exclude values below left indicator"),
                QString::fromStdString("Exclude values above right indicator")
        };
 */

QColor ColorMapWidget::cachedColorOutOfRangesParent = ColorMapExtended::DEFAULT_COLOR_VALUES_OUT_OF_RANGE;

ColorMapWidget::ColorMapWidget( ColorMapExtended& _parent ) :
    parent( _parent ),
    // plot(parent),
    plotPercentage( parent, ColorMapPlot::MarkersPositions::PERCENTAGE ),
    currentPlot( &plotPercentage ),
    cacheInvertedParent( _parent.invertedColorMap() )
{
    parent.setMarkersPositions( currentPlot->getMarkersPositionsObject() );
    // plotLayout.addWidget(&plot);
    plotLayout.addWidget( &plotPercentage );
    // plotLayout.setCurrentWidget(&plot);
    plotWidget.setLayout( &plotLayout );

    /*plotChoose.addItem("Plot values as numerical");
       plotChoose.addItem("Plot values as percentage");

       filterCheck[0].setChecked(ColorMapExtended::isFilteringEnabled(
                    ColorMapPlot::MarkersPositions::FILTER_MIN));
       filterCheck[1].setChecked(ColorMapExtended::isFilteringEnabled(
                    ColorMapPlot::MarkersPositions::FILTER_MAX));
       for(int i = 0;i < 2;++i) {
            filterBorders.addRow(FILTER_CHECKBOX_NAMES[i],
                            &filterCheck[i]);
            QObject::connect(&filterCheck[i], SIGNAL(  stateChanged(int) ),
                            this, SLOT( filteringValueStateChanged(int) ));
       }

       filterWidget.setLayout(&filterBorders);

       QObject::connect(&plot , SIGNAL(colorChanged(ColorMapPlot::Color, const QColor &)), this,
                    SLOT(processColorChanged(ColorMapPlot::Color, const QColor &)) );
       QObject::connect(&plotChoose , SIGNAL(currentIndexChanged(int)),
                    this, SLOT(plotTypeChanged(int)) );
     */
    QObject::connect( &plotPercentage, SIGNAL( colorChanged( ColorMapPlot::Color, const QColor & ) ), this,
                      SLOT( processColorChanged( ColorMapPlot::Color, const QColor & ) ) );
}

ColorMapWidget::~ColorMapWidget()
{
}

void
ColorMapWidget::colorMapUpdated()
{
    currentPlot->update();
}

void
ColorMapWidget::addPlotToGUI( QLayout& layout )
{
    layout.addWidget( &plotWidget );
    // layout.setAlignment(&plotWidget, Qt::AlignCenter);
    // layout.addWidget(&plotChoose);
    // layout.addWidget(&filterWidget);
    parent.setMarkersPositions( currentPlot->getMarkersPositionsObject() );
}

void
ColorMapWidget::processColorChanged( ColorMapPlot::Color pos, const QColor& col )
{
    if ( pos == ColorMapPlot::COLOR_VALUE_OUT_OF_RANGE )
    {
        parent.setColorForValuesOutOfRange( col );
    }
}

/*
   void ColorMapWidget::filteringValueStateChanged(int)
   {
        parent.enableFiltering(ColorMapPlot::MarkersPositions::FILTER_MIN, filterCheck[0].isChecked());
        parent.enableFiltering(ColorMapPlot::MarkersPositions::FILTER_MAX, filterCheck[1].isChecked());

        currentPlot->update();
   }
 */

void
ColorMapWidget::enablePlotColorChange( bool val )
{
    /*
     * Currently not used.
       plot.setEnabledColorChange(ColorMapPlot::COLOR_FIRST, val);
       plot.setEnabledColorChange(ColorMapPlot::COLOR_SECOND, val);
     */

    plotPercentage.setEnabledColorChange( ColorMapPlot::COLOR_FIRST, val );
    plotPercentage.setEnabledColorChange( ColorMapPlot::COLOR_SECOND, val );
}


void
ColorMapWidget::enablePlotMiddleMark( bool val )
{
    // plot.setEnableMiddleMark(val);
    plotPercentage.setEnableMiddleMark( val );
}

bool
ColorMapWidget::hasHeightForWidth() const
{
    return false;
}

void
ColorMapWidget::applyChanges()
{
    cacheInvertedParent          = parent.invertedColorMap();
    cachedColorOutOfRangesParent = parent.getColorForValuesOutOfRange();
    currentPlot->applyChanges();
}

void
ColorMapWidget::revertChanges()
{
    // actions common for all color maps - parent inverting
    if ( parent.invertedColorMap() != cacheInvertedParent )
    {
        parent.invertColorMap();
    }
    parent.setColorForValuesOutOfRange( cachedColorOutOfRangesParent );
    currentPlot->revertChanges();
}

/*
   void ColorMapWidget::plotTypeChanged(int type)
   {
        if( type == 1) {
                currentPlot = &plotPercentage;
        } else {
                currentPlot = &plot;
        }
        plotLayout.setCurrentWidget(currentPlot);
        parent.setMarkersPositions(currentPlot->getMarkersPositionsObject());

        currentPlot->update();
   }
 */

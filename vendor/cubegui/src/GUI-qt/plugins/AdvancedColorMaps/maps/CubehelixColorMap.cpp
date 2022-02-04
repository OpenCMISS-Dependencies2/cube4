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

#include <limits>
#include <cmath>


#include "CubehelixColorMap.h"

using std::numeric_limits;
using std::pow;
using std::sin;
using std::cos;
using std::min;
using std::max;
using std::floor;

const QString CubehelixColorMap::COLOR_MAP_DESCRIPTION =
    tr( "Cubehelix is a color map designed primarily for display of astronomical intensity images."
        "The distribution of colours is from black to white, with a helix in cube giving the deviation from"
        " greyscale. The perceived intensity is increasing monotonically. <br>"
        "Cubehelix is defined by four parameters: <br>"
        "1. Start colour - starting value, R = 1, G = 2, B = 0 <br>"
        "2. Rotations - number of R->G->B rotations from the start to the end <br>"
        "3. Hue - controls saturation of the scheme, with pure greyscale for hue equal to 0<br>"
        "4. Gamma factor - configures intensity of colours; values below 1 will give brighter color scheme." );

const double CubehelixColorMap::DEFAULT_VALUES[ 4 ] = { 0.5, -1.5, 1.0, 1.0 };

const double CubehelixColorMap::MAP_MATRIX[ 3 ][ 2 ] = {
    { -0.14861, 1.78277 }, { -0.29227, -0.90649 }, { 1.97294, 0.0 }
};


CubehelixColorMap::CubehelixColorMap()
{
    parameterValues[ 0 ] = DEFAULT_VALUES[ START_COLOR ];
    parameterValues[ 1 ] = DEFAULT_VALUES[ NUMBER_OF_ROTATIONS ];

    parameterValues[ 2 ] = DEFAULT_VALUES[ HUE_PARAMETER ];
    parameterValues[ 3 ] = DEFAULT_VALUES[ GAMMA_FACTOR ];

    widget = new CubehelixColorMapWidget( *this );
}

CubehelixColorMap::~CubehelixColorMap()
{
}

const QString&
CubehelixColorMap::getColorMapDescription() const
{
    return COLOR_MAP_DESCRIPTION;
}

ColorMapWidget*
CubehelixColorMap::getConfigurationPanel()
{
    return widget;
}

QColor
CubehelixColorMap::getColor( double value, double minValue,
                             double maxValue, bool whiteForZero ) const
{
    pair<unsigned short, QColor> preprocess = ColorMapExtended::getColorBasicProcessing( value,
                                                                                         minValue, maxValue, whiteForZero );

    // preprocess returns value
    if ( preprocess.first == 2 )
    {
        return preprocess.second;
    }
    pair<double, double> minMax = adjustFilteringBorders( minValue, maxValue );
    minValue = minMax.first;
    maxValue = minMax.second;

    /**
     * Code based directly on JavaScript implementation of Cubehelix:
     * https://www.mrao.cam.ac.uk/~dag/CUBEHELIX/cubetry.html
     */

    // current position
    double lambda = ( value - minValue ) / ( maxValue - minValue );
    if ( invertedCM )
    {
        lambda = 1.0 - lambda;
    }
    double angle       = 2 * M_PI * ( parameterValues[ START_COLOR ] / 3.0 + 1 + parameterValues[ NUMBER_OF_ROTATIONS ] * lambda );
    double cos_angle   = cos( angle );
    double sin_angle   = sin( angle );
    double lambdaGamma = pow( lambda, parameterValues[ GAMMA_FACTOR ] );
    double amp_param   = parameterValues[ HUE_PARAMETER ] * lambda * ( 1 - lambda ) * 0.5;

    double rgb[ 3 ];
    for ( int i = 0; i < 3; ++i )
    {
        rgb[ i ] = lambdaGamma + amp_param * ( cos_angle * MAP_MATRIX[ i ][ 0 ] +  sin_angle * MAP_MATRIX[ i ][ 1 ] );
        rgb[ i ] = max( 0.0, min( 1.0, rgb[ i ] ) );
    }

    return QColor::fromRgb( floor( rgb[ 0 ] * 255 ), floor( rgb[ 1 ] * 255 ), floor( rgb[ 2 ] * 255 ) );
}

QString
CubehelixColorMap::getMapName() const
{
    return tr( "Cubehelix" );
}

void
CubehelixColorMap::saveGlobalSettings( QSettings& settings )
{
    ColorMapExtended::saveGlobalSettings( settings );
    for ( int i = START_COLOR; i <= GAMMA_FACTOR; ++i )
    {
        settings.setValue( QString( "Cubehelix_param_%1" ).arg( i ), parameterValues[ i ] );
    }
}

void
CubehelixColorMap::loadGlobalSettings( QSettings& settings )
{
    ColorMapExtended::loadGlobalSettings( settings );
    for ( int i = START_COLOR; i <= GAMMA_FACTOR; ++i )
    {
        parameterValues[ i ] = settings.value( QString( "Cubehelix_param_%1" ).arg( i ), DEFAULT_VALUES[ i ] ).toDouble();
    }
    widget->parentUpdated();
}

double
CubehelixColorMap::getMapParameter( Parameters param ) const
{
    return parameterValues[ param ];
}

void
CubehelixColorMap::setMapParameter( Parameters param, double val )
{
    parameterValues[ param ] = val;
}

const QString CubehelixColorMapWidget::PARAMETERS_EDIT_LABEL[ 4 ] =
{ tr( "Start colour" ), tr( "Number of rotations" ), tr( "Hue parameter" ),
  tr( "\u03B3 factor " ) };

const QString CubehelixColorMapWidget::PARAMETERS_HINT[ 4 ] =
{ tr( "Value between 1.0 (red), 2.0 (green) and 3.0 (blue), periodic over 3" ),
  tr( "Negative value gives negative direction of rotation" ),
  tr( "Non-negative value describing the saturation of colours" ),
  tr( "Non-negative value used to emphasize low intensity values, for \u03B3 < 1, "
      "or high intensity values for \u03B3 > 1" ) };

const double CubehelixColorMapWidget::PARAMETERS_VALIDATORS_DATA[ 4 ][ 2 ] =
{ { 0.0, 3.0                           }, { numeric_limits<double>::min(), numeric_limits<double>::max()                           },
  { 0.0, numeric_limits<double>::max() }, { 0.0,                           numeric_limits<double>::max()                           } };

const int CubehelixColorMapWidget::PLOT_DEFAULT_HEIGHT = 128;
const int CubehelixColorMapWidget::PLOT_DEFAULT_WIDTH  = 256;


CubehelixColorMapWidget::CubehelixColorMapWidget( CubehelixColorMap& map ) :
    ColorMapWidget( map ),
    plot( PLOT_DEFAULT_WIDTH, PLOT_DEFAULT_HEIGHT, currentPlot, map ),
    plotLabel( tr( "R, G, B and greyscale" ) )
{
    setMinimumHeight( 425 );
    for ( int i = CubehelixColorMap::START_COLOR; i <= CubehelixColorMap::GAMMA_FACTOR; i++ )
    {
        parametersEditLabels[ i ].setText( PARAMETERS_EDIT_LABEL[ i ] );

        /**
         * Hints and tool tip
         */
        parametersEditLabels[ i ].setWhatsThis( PARAMETERS_HINT[ i ] );
        parametersEditLabels[ i ].setToolTip( PARAMETERS_HINT[ i ] );
        parametersEdit[ i ].setWhatsThis( PARAMETERS_HINT[ i ] );
        parametersEdit[ i ].setToolTip( PARAMETERS_HINT[ i ] );


        configurationPanelLayout.addRow( &parametersEditLabels[ i ], &parametersEdit[ i ] );
        parametersEdit[ i ].setMaximumHeight( 30 );
        parametersEdit[ i ].setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
        parametersEdit[ i ].setText( QString::number(
                                         map.getMapParameter( static_cast<CubehelixColorMap::Parameters>( i ) )
                                         ) );
        /**
         * Configure validators.
         */
        parametersValidators[ i ] = new QDoubleValidator( this );
        parametersValidators[ i ]->setBottom( PARAMETERS_VALIDATORS_DATA[ i ][ 0 ] );
        parametersValidators[ i ]->setTop( PARAMETERS_VALIDATORS_DATA[ i ][ 1 ] );
        parametersEdit[ i ].setValidator( parametersValidators[ i ] );

        /**
         * Connect slot.
         */
        QObject::connect( &parametersEdit[ i ], SIGNAL( textEdited( const QString & ) ),
                          this, SLOT( parameterChanged( const QString & ) ) );
    }
    configurationPanel.setLayout( &configurationPanelLayout );

    layout.addWidget( &configurationPanel );
    layout.setAlignment( &configurationPanel, Qt::AlignHCenter );
    layout.addWidget( &plotLabel );
    layout.setAlignment( &plotLabel, Qt::AlignHCenter );
    layout.addWidget( &plot );
    layout.setAlignment( &plot, Qt::AlignHCenter );

    addPlotToGUI( layout );
    enablePlotColorChange( false );
    enablePlotMiddleMark( false );

    setLayout( &layout );

    /**
     * Init cache
     */
    applyChanges();
}

CubehelixColorMapWidget::~CubehelixColorMapWidget()
{
    for ( int i = CubehelixColorMap::START_COLOR; i <= CubehelixColorMap::GAMMA_FACTOR; i++ )
    {
        delete parametersValidators[ i ];
    }
}

CubehelixColorMap&
CubehelixColorMapWidget::getParent() const
{
    return dynamic_cast<CubehelixColorMap&>( parent );
}

void
CubehelixColorMapWidget::parameterChanged( const QString& )
{
    // update our color map
    for ( int i = CubehelixColorMap::START_COLOR; i <= CubehelixColorMap::GAMMA_FACTOR; i++ )
    {
        CubehelixColorMap::Parameters param = static_cast<CubehelixColorMap::Parameters>( i );
        // thanks to validator the value will always be true
        getParent().setMapParameter( param, parametersEdit[ i ].text().toDouble() );
    }
    plot.repaint();
    // redraw rest of GUI
    colorMapUpdated();
}

void
CubehelixColorMapWidget::parentUpdated()
{
    // update our color map
    for ( int i = CubehelixColorMap::START_COLOR; i <= CubehelixColorMap::GAMMA_FACTOR; i++ )
    {
        CubehelixColorMap::Parameters param = static_cast<CubehelixColorMap::Parameters>( i );
        parametersEdit[ i ].setText( QString::number( getParent().getMapParameter( param ) ) );
    }
    plot.repaint();
    // redraw rest of GUI
    colorMapUpdated();
}

void
CubehelixColorMapWidget::applyChanges()
{
    ColorMapWidget::applyChanges();

    for ( int i = CubehelixColorMap::START_COLOR; i <= CubehelixColorMap::GAMMA_FACTOR; i++ )
    {
        CubehelixColorMap::Parameters param = static_cast<CubehelixColorMap::Parameters>( i );
        cachedParameterValues[ i ] = getParent().getMapParameter( param );
    }
}

void
CubehelixColorMapWidget::revertChanges()
{
    ColorMapWidget::revertChanges();

    for ( int i = CubehelixColorMap::START_COLOR; i <= CubehelixColorMap::GAMMA_FACTOR; i++ )
    {
        CubehelixColorMap::Parameters param = static_cast<CubehelixColorMap::Parameters>( i );
        getParent().setMapParameter( param, cachedParameterValues[ i ] );
    }

    // update and redraw GUI
    parentUpdated();
}

CubehelixPlot::CubehelixPlot( int                w,
                              int                h,
                              ColorMapPlot*      plot,
                              CubehelixColorMap& parent ) :
    parentObject( parent )
{
    QObject::connect( plot, SIGNAL( markerChanged( ColorMapPlot::MarkersPositions::Position, double ) ),
                      this, SLOT( markerChanged( ColorMapPlot::MarkersPositions::Position, double ) ) );

    setFixedHeight( h );
    setFixedWidth( w );
    setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
}

void
CubehelixPlot::repaint()
{
    update();
}

void
CubehelixPlot::paintEvent( QPaintEvent* )
{
    QPainter painter( this );
    int      w = width();
    int      h = height();

    painter.fillRect( 0, 0, w, h, Qt::white );
    // horizontal
    painter.drawLine( 0, 0, w - 1, 0 );
    painter.drawLine( 0, h - 1, w - 1, h - 1 );

    // vertical
    painter.drawLine( 0, 0, 0, h - 1 );
    painter.drawLine( w - 1, 0, w - 1, h - 1 );

    for ( int i = 0; i < w; ++i )
    {
        QColor col = parentObject.getColor( i, 0, 255, false );

        if ( col != parentObject.getColorForValuesOutOfRange() )
        {
            double lumaValue = floor( 0.3 * col.red() + 0.59 * col.green() + 0.11 * col.blue() );
            // greyscale line (or curve for gamma parameter != 1)
            painter.fillRect( i - 1, minmax( floor( h - lumaValue / 2.0 ) - 1, 0, h ), 2, 2, Qt::gray );

            painter.fillRect( i - 1, minmax( floor( h - col.red() / 2.0 ) - 1, 0, h ), 2, 2, Qt::red );
            painter.fillRect( i - 1, minmax( floor( h - col.green() / 2.0 ) - 1, 0, h ), 2, 2, Qt::green );
            painter.fillRect( i - 1, minmax( floor( h - col.blue() / 2.0 ) - 1, 0, h ), 2, 2, Qt::blue );
        }
        else
        {
            painter.fillRect( i - 1, 0, 2, 2, Qt::gray );
        }
    }
}

double
CubehelixPlot::minmax( double val, double min_, double max_ )
{
    return max( min( val, max_ ), min_ );
}

void
CubehelixPlot::markerChanged( ColorMapPlot::MarkersPositions::Position pos, double )
{
    // repaint plot when filtering values have changed
    if ( pos == ColorMapPlot::MarkersPositions::START_POSITION || pos == ColorMapPlot::MarkersPositions::END_POSITION )
    {
        update();
    }
}

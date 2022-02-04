/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/




#ifndef SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_MAPS_CUBEHELIXCOLORMAP_H_
#define SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_MAPS_CUBEHELIXCOLORMAP_H_
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QLabel>

#include "ColorMapExtended.h"

/**
 * @class CubehelixColorMap
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date March 2015
 * @brief Implementation of color map 'Cubehelix' - fancy algorithm based
 * on rotations of RGB curves in space
 * Source:
 * Green, D. A., 2011, `A colour scheme for the display of astronomical intensity images',
 * Bulletin of the Astronomical Society of India, 39, 289.
 *
 * https://www.mrao.cam.ac.uk/~dag/CUBEHELIX/
 */
class CubehelixColorMap : public ColorMapExtended
{
    Q_OBJECT
public:
    CubehelixColorMap();

    ~CubehelixColorMap();

    const QString&
    getColorMapDescription() const;

    ColorMapWidget*
    getConfigurationPanel();

    QColor
    getColor( double value,
              double minValue,
              double maxValue,
              bool   whiteForZero ) const;

    QString
    getMapName() const;

    void
    saveGlobalSettings( QSettings& settings );

    void
    loadGlobalSettings( QSettings& settings );

    /**
     * Cubehelix parameters.
     */
    enum Parameters
    {
        START_COLOR = 0,            // !< Starting color for map
        NUMBER_OF_ROTATIONS,        // !< Number of R/G/B rotations from start to the end
        HUE_PARAMETER,              // !< Hue parameter for the colors
        GAMMA_FACTOR                // !< Gamma parameter used for emphasizing (low intensity, gamma < 1)
    };

    /**
     * @param param
     * @return current value of map parameter 'param'
     */
    double
    getMapParameter( Parameters param ) const;

    /**
     * Set new value 'val' of map parameter 'param'
     * @param param
     * @param val
     */
    void
    setMapParameter( Parameters param,
                     double     val );


private:
    /**
     * Starting color for map.
     */
    double parameterValues[ 4 ];

    /**
     * Default values for map parameters.
     */
    static const double DEFAULT_VALUES[ 4 ];

    /**
     * Matrix 3x2 used in computation of colormap values.
     */
    static const double MAP_MATRIX[ 3 ][ 2 ];

    static const QString COLOR_MAP_DESCRIPTION;
};

/**
 * @class CubehelixPlot
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date March 2015
 * @brief GUI widget for CubehelixColorMapWidget - display curves
 */
class CubehelixPlot : public QWidget
{
    Q_OBJECT
public:
    CubehelixPlot( int                w,
                   int                h,
                   ColorMapPlot*      plot,
                   CubehelixColorMap& parent );

    /**
     * Force redraw of plot.
     */
    void
    repaint();

protected:
    /**
     * Map object.
     */
    CubehelixColorMap& parentObject;

    /**
     * Overloaded paint event - draw plot.
     * @param event
     */
    void
    paintEvent( QPaintEvent* event );

    /**
     * Truncate value outside min and max.
     * @param val
     * @param min
     * @param max
     * @return value fitted into [min, max]
     */
    double
    minmax( double val,
            double min_,
            double max_ );

protected slots:

    void markerChanged( ColorMapPlot::MarkersPositions::Position, double );
};

/**
 * @class CubehelixColorMapWidget
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date March 2015
 * @brief GUI widget for CubehelixColorMap - configure coloring and display plot of
 */
class CubehelixColorMapWidget : public ColorMapWidget
{
    Q_OBJECT
public:
    /**
     * @param map which will be 'served' by this widget
     */
    CubehelixColorMapWidget( CubehelixColorMap& map );

    ~CubehelixColorMapWidget();

    /**
     * Overridden from parent.
     */
    CubehelixColorMap&
    getParent() const;

    void
    parentUpdated();

    void
    revertChanges();

    void
    applyChanges();

protected:

    /**
     * Cached parameteres of Cubehelix.
     */
    double cachedParameterValues[ 4 ];

    /**
     * Main layout for widget.
     */
    QVBoxLayout layout;

    /**
     * Map configuration.
     */
    QWidget configurationPanel;

    /**
     * Layout for map configuration.
     */
    QFormLayout configurationPanelLayout;

    /**
     * Parameters edits.
     */
    QLineEdit parametersEdit[ 4 ];

    /**
     * Description of parameters.
     */
    QLabel parametersEditLabels[ 4 ];

    /**
     * Validator for parameters edits.
     */
    QDoubleValidator* parametersValidators[ 4 ];

    /**
     * Curves plot.
     */
    CubehelixPlot plot;

    /**
     * Plot name.
     */
    QLabel plotLabel;
protected slots:
    /**
     * Slot - one of edit labels has been changed.
     */
    void
    parameterChanged( const QString& );

private:

    /**
     * Names of all parameters.
     */
    static const QString PARAMETERS_EDIT_LABEL[ 4 ];

    /**
     * "Hints" with parameter description.
     */
    static const QString PARAMETERS_HINT[ 4 ];

    /**
     * Start colour - between 0.0 and 3.0
     * Number of rotations - each double value
     * Hue, gammma - non-negative value (although value 0 may doesn't make any sense),
     * but still it will generate some colormap.
     */
    static const double PARAMETERS_VALIDATORS_DATA[ 4 ][ 2 ];

    /**
     * Height of plot - fixed.
     */
    static const int PLOT_DEFAULT_HEIGHT;

    /**
     * Width of plot - fixed.
     */
    static const int PLOT_DEFAULT_WIDTH;
};


#endif /* SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_MAPS_CUBEHELIXCOLORMAP_H_ */

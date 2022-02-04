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



#ifndef SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_COLORMAPEXTENDED_H_
#define SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_COLORMAPEXTENDED_H_

#include <string>
#include <utility>

#include <QWidget>
#include <QColor>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <QGroupBox>
#include <QCheckBox>
#include <QFormLayout>
#include <QStackedLayout>
#include <QComboBox>
#include <QSettings>

#include "ColorMap.h"
#include "ColorMapPlot.h"

using std::pair;
using std::string;

class ColorMapWidget;

/**
 * @class ColorMapExtended
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief Extended parent class for color maps.
 * Implements basic processing, common for all color maps (special cases, filtering etc).
 */
class ColorMapExtended : public cubegui::ColorMap
{
    Q_OBJECT
public:
    ColorMapExtended();

    virtual
    ~ColorMapExtended();

    /**
     * Invert color map - draw from 1 to 0, instead of 0 to 1.
     */
    void
    invertColorMap();

    /**
     * @return true iff the map is inverted
     */
    bool
    invertedColorMap();

    /**
     * Default color for values out of range.
     */
    static const QColor DEFAULT_COLOR_VALUES_OUT_OF_RANGE;

    /**
     * @return color used currently by all maps for values out of range
     */
    static QColor
    getColorForValuesOutOfRange();

    /**
     * @param new color for values out of range
     */
    static void
    setColorForValuesOutOfRange( const QColor& );

    /**
     * Enable/disable filtering at limit pos, providing boolean value.
     * @param pos
     * @param val
     */
    static void
    enableFiltering( ColorMapPlot::MarkersPositions::FilterPosition pos,
                     bool                                           val );

    /**
     * @param pos
     * @return true when filtering is enabled for selected position
     */
    static bool
    isFilteringEnabled( ColorMapPlot::MarkersPositions::FilterPosition pos );

    /**
     * @return description of color map, displayed to user in GUI
     */
    virtual const QString&
    getColorMapDescription() const = 0;

    /**
     * @return instance of widget used to configure this color map
     */
    virtual ColorMapWidget*
    getConfigurationPanel() = 0;

    void
    setMarkersPositions( ColorMapPlot::MarkersPositions* pos );

    /**
     * Apply filtering borders to current data min/max.
     * @param min
     * @param max
     * @return pair <min, max>
     */
    pair<double, double>
    adjustFilteringBorders( double min,
                            double max ) const;

    /**
     * Save user-modified maps.
     * @param settings
     */
    virtual void
    saveGlobalSettings( QSettings& settings );

    /**
     * Load user-modified maps.
     * @param settings
     */
    virtual void
    loadGlobalSettings( QSettings& settings );

    /**
     * @class CIELABColor
     * Implementation of CIELAB color space.
     */
    class CIELABColor
    {
        /**
         * L* value in color space.
         */
        double lightness;

        /**
         * a* value in color space.
         */
        double a;

        /**
         * b* value in color space.
         */
        double b;

        /**
         * Used in the process of RGB -> CIELAB (and vice versa) conversion.
         *
         * Use whitepoint from Illuminant D65.
         * http://en.wikipedia.org/wiki/Illuminant_D65
         */
        static const double WHITEPOINT_X;
        static const double WHITEPOINT_Y;
        static const double WHITEPOINT_Z;
        static const double RGBXYZ_CONVERSION_MATRIX[ 3 ][ 3 ];

        /**
         * CIELAB conversion function used in conversion RGB->CIELAB
         */
        static double
        cielabConversionFunction( const double& x );

        /**
         * CIELAB inverse conversion function used in conversion CIELAB->RGB
         */
        static double
        cielabConversionFunctionInverse( const double& x );

public:
        /**
         * Create color in CIELAB space using explicit values.
         * @param _lightness
         * @param _a
         * @param _b
         */
        CIELABColor( double _lightness,
                     double _a,
                     double _b );

        /**
         * Copy constructor.
         * @param c
         */
        CIELABColor( const CIELABColor& c );

        /**
         * @param RGB color to convert
         * @return representation of RGB color in CIELAB space
         */
        static CIELABColor
        fromRGB( const QColor& );

        /**
         * @return representation of current color in RGB
         */
        QColor
        toRGB() const;

        /**
         * @return textual representation of current color
         */
        string
        toString() const;

        /**
         * @param l new value for lightness
         */
        void
        setLightness( const double& l );

        /**
         * @return L* value of color
         */
        double
        getLightness() const;

        /**
         * @return a* value of color
         */
        double
        getAStar() const;

        /**
         * @return b* value of color
         */
        double
        getBStar() const;
    };

    /**
     * @class MSHColor
     * Implementation of MSH color space - polar version of CIELAB.
     */
    class MSHColor
    {
        /**
         * M value - vector length
         */
        double m;

        /**
         * S value - trigonometric function of angle
         */
        double s;

        /**
         * S value - trigonometric function of angle
         */
        double h;

        /**
         * Function used in MSH color space
         * @param colorSat
         * @param Munsat
         * @return
         */
        static double
        adjustHue( const MSHColor& colorSat,
                   double          Munsat );

        static pair<double, double>
        adjustHues( const MSHColor& start,
                    const MSHColor& end );

public:

        /**
         * Create color in MSH space using explicit values.
         * @param _m
         * @param _s
         * @param _h
         */
        MSHColor( double _m,
                  double _s,
                  double _h );

        /**
         * Copy constructor.
         * @param c
         */
        MSHColor( const MSHColor& c );

        /**
         * Default constructor - create white.
         */
        MSHColor();

        MSHColor&
        operator=( const MSHColor& ) = default;

        /**
         * @param RGB color to convert
         * @return representation of RGB color in CIELAB space
         */
        static MSHColor
        fromRGB( const QColor& );

        /**
         * @return representation of current color in CIELAB space
         */
        CIELABColor
        toCIELAB() const;

        /**
         * @return representation of current color in RGB
         */
        QColor
        toRGB() const;

        /**
         * @return textual representation of current color
         */
        string
        toString() const;

        /**
         * Interpolate linearly between two colors.
         * @param start begin color
         * @param end end color
         * @param interpolation_place value between 0.0 and 1.0
         * @return interpolation of color at given position
         */
        static MSHColor
        interpolate( const MSHColor& start,
                     const MSHColor& end,
                     double          interpolation_place );

        /**
         * Interpolate exponentially between two colors.
         * @param start begin color
         * @param end end color
         * @param interpolation_place value between 0.0 and 1.0
         * @return interpolation of color at given position
         */
        static MSHColor
        interpolateExp( const MSHColor& start,
                        const MSHColor& end,
                        double          interpolation_place );

        /**
         * Interpolate linearly between two colors with additional "speed" parameter.
         * @param start begin color
         * @param end end color
         * @param interpolation_place value between 0.0 and 1.0
         * @param marker_position position of middle marker, specifying interpolation "speed"
         * @return interpolation of color at given position
         */
        static MSHColor
        interpolate( const MSHColor& start,
                     const MSHColor& end,
                     double          interpolation_place,
                     double          marker_position );

        /**
         * Interpolate exponentially between two colors.
         * @param start begin color
         * @param end end color
         * @param interpolation_place value between 0.0 and 1.0
         * @param marker_position position of middle marker, specifying interpolation "speed"
         * @return interpolated color
         */
        static MSHColor
        interpolateExp(
            const MSHColor& start,
            const MSHColor& end,
            double          interpolation_place,
            double          marker_position );

        /**
         * @return M value of color
         */
        double
        getM() const;

        /**
         * @return s value of color
         */
        double
        getS() const;

        /**
         * @return h value of color
         */
        double
        getH() const;

        /**
         * @param
         * @return true iff both colors are the same (or at least very similar - float comparison)
         */
        bool
        compare( const MSHColor& ) const;

        /**
         * White color in MSH color space.
         */
        static const MSHColor WHITE;
    };
protected:
    /**
     * Common preprocessing for getColor method. Arguments are the same as in getColor.
     * @param value
     * @param minValue
     * @param maxValue
     * @param whiteForZero
     * @return pair of unsigned integer and color:
     * integer: 0 -> generate color as usual
     * 1 -> red field of second element contains position
     * 2 -> second element is generated color
     */
    pair<unsigned short, QColor>
    getColorBasicProcessing( double value,
                             double minValue,
                             double maxValue,
                             bool   whiteForZero ) const;

    /**
     * Color used when the value is out of range.
     */
    static QColor colorValuesOutOfRange;

    /**
     * Enabled/disabled filtering for min/max.
     */
    // static bool enableFilters[2];

    /**
     * Currently used plot position method;
     */
    ColorMapPlot::MarkersPositions* markersPositions;

    /**
     * If true, then invert color map values.
     */
    bool invertedCM;

    /**
     * GUI widget.
     */
    ColorMapWidget* widget;
};

/**
 * @class ColorMapWidget
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief Abstract parent class for configuration widgets used by color maps.
 */
class ColorMapWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructor; initialize plot to NULL.
     */
    ColorMapWidget( ColorMapExtended& );

    /**
     * Delete plot.
     */
    virtual
    ~ColorMapWidget() = 0;

    /**
     * @return reference to parent color map, automatically casted to proper subclass
     */
    virtual ColorMapExtended&
    getParent() const = 0;

    /**
     * Repaint plot and other updates of GUI.
     */
    virtual void
    colorMapUpdated();

    /**
     * Notifies widget about parent update - used only when settings are loaded.
     */
    virtual void
    parentUpdated()
    {
    };

    /**
     * Overriden from QWidget
     */
    virtual bool
    hasHeightForWidth() const;

    /**
     * Informs internal structure of subclasses that the buttons 'Apply' or 'Ok' have been pressed,
     * which means that temporal changes introduced in color map are now considered as stable.
     */
    virtual void
    applyChanges();

    /**
     * Informs internal structure of subclasses that the button 'Cancel' or 'Ok' has been pressed,
     * which means that temporal changes introduced in color map should be replaced by previous values.
     */
    virtual void
    revertChanges();

protected:
    ColorMapExtended& parent;
    ColorMapPlot      plotPercentage;
    ColorMapPlot*     currentPlot;

    /**
     * Status of CM inversion between any changes.
     */
    bool cacheInvertedParent;

    /**
     * Status of color for values out of range between any changes.
     */
    static QColor cachedColorOutOfRangesParent;

    /**
     * Add plot to GUI.
     * @param parent layout for plot
     */
    void
    addPlotToGUI( QLayout& );

    /**
     * Enable/disable color change at plots.
     * @param col
     * @param val
     */
    void
    enablePlotColorChange( bool val );

    /**
     * Enable/disable drawing and using of middle mark in plot.
     * @param val
     */
    void
    enablePlotMiddleMark( bool val );

protected slots:
    virtual void
    processColorChanged( ColorMapPlot::Color,
                         const QColor& );

    /**
     * Currently not used
            void filteringValueStateChanged(int);
            void plotTypeChanged(int type);
     */
private:
    /**
     * Currently not used
            ColorMapPlot plot;
            QCheckBox filterCheck[2];
            QFormLayout filterBorders;
            QComboBox plotChoose;
            QWidget filterWidget;
            static const QString FILTER_CHECKBOX_NAMES[2];
     */
    QStackedLayout plotLayout;
    QWidget        plotWidget;
};

#endif /* SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_COLORMAPEXTENDED_H_ */

/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_COLORMAPPLOT_H_
#define SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_COLORMAPPLOT_H_

#include <QWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QColorDialog>
#include <QPoint>
#include <QMenu>
#include <QSettings>

class ColorMapExtended;

/**
 * @class ColorMapPlot
 * @author Marcin Copik (m.copik@fz-juelich.de)
 * @date January 2015
 * @brief Basic plot of color map.
 */
class ColorMapPlot : public QFrame
{
    Q_OBJECT
    /**
     * Plotted color map.
     */
    ColorMapExtended& currentColorMap;
public:
    struct MarkersPositions
    {
        friend class ColorMapPlot;

        /**
         * Marker positions at plot.
         */
        enum Position
        {
            START_POSITION = 0,            // !< Minimum value
            MIDDLE_POINT   = 1,            // !< divergence/middle position
            END_POSITION   = 2             // !< Maximum value
        };
public:
        /**
         * Type of computing filtering values and middle marker.
         */
        enum MarkersMethod
        {
            NUMERICAL  = 0,            // !< Direct numerical value
            PERCENTAGE = 1             // !< Percentage of data
        };

        /**
         * Types of filtering.
         */
        enum FilterPosition
        {
            FILTER_MIN = 0,            // !< minimal acceptable value
            FILTER_MAX = 1             // !< maximal acceptable value
        };

        /**
         * @param method used markers method
         */
        MarkersPositions( MarkersMethod method );

        /**
         * Adjust max/min value if it's out of [min,max] interval
         * @param minOrMax min or maximal value
         * @param pos
         * @return original value or filtering border
         */
        double
        adjustFiltering( double         min,
                         double         max,
                         bool           ifEnabled,
                         FilterPosition pos ) const;

        /**
         * @param pos
         * @return filtering value - min/max - depending on pos
         */
        double
        getFilteringValue( FilterPosition pos ) const;

        /**
         * @param min min value in data
         * @param max max value in data
         * @return middle marker position
         */
        double
        getMiddleMarker( double min,
                         double max ) const;

private:
        /**
         * @param pos
         * @return get marker value at position pos
         */
        double
        getMarker( Position pos ) const;

        /**
         * Change marker value.
         * @param val
         * @param pos
         */
        void
        setMarker( double   val,
                   Position pos );

        /**
         * Modify marker value by adding the difference
         * @param diff
         * @param plotWidth difference between positions of max/min markers
         * @param pos
         */
        void
        applyDifference( double   diff,
                         double   plotWidth,
                         Position pos );

        /**
         * Set marker values to default.
         */
        void
        resetValues();

        /**
         * Filter values for statically-shared - MIN and MAX for
         * NUMERICAL and PERCENTAGE. Middle values are not being used.
         */
        static double filterValues[ 6 ];

        /**
         * Value at MIDDLE_POINT
         */
        double middleMarker;

        /**
         * Used method of updating and evaluating markers.
         */
        const MarkersMethod method;

        /**
         * Default minimal values of plot.
         */
        static const double DEFAULT_VALUES_MIN[ 2 ];

        /**
         * Default maximal values of plot.
         */
        static const double DEFAULT_VALUES_MAX[ 2 ];
    };


    /**
     *
     * Create new plot.
     * @param map plotted color map
     * @param pos positions structure
     * @param _width plot width
     * @param _height plot height
     */
    ColorMapPlot( ColorMapExtended&               map,
                  MarkersPositions::MarkersMethod pos = MarkersPositions::NUMERICAL,
                  int                             _width = WIDTH,
                  int                             _height = HEIGHT );

    /**
     * @return marker position object
     */
    MarkersPositions*
    getMarkersPositionsObject();

    /**
     * Supported types of color in plot
     */
    enum Color
    {
        COLOR_VALUE_OUT_OF_RANGE = 0,        // !< color for values below min/above max limit
        COLOR_FIRST,                         // !< first color in scheme
        COLOR_SECOND                         // !< second color in scheme
    };

    /**
     * Plot objects with defined action after a double click.
     */
    enum DoubleClick
    {
        DC_COLOR,               // color change
        DC_MARKER               // manual change of value in marker
    };

    /**
     * Enable/disable color changing for first/second color.
     * @param value true enables
     */
    void
    setEnabledColorChange( Color col,
                           bool  value );

    /**
     * @param val appropriate value enables/disables middle mark.
     */
    void
    setEnableMiddleMark( bool val );

    /**
     * Update cached values.
     */
    void
    applyChanges();

    /**
     * Revert any temporal change - set markers positions from cache.
     */
    void
    revertChanges();

    /**
     * Save static data - filtering values.
     */
    static void
    loadSettings( QSettings& );
    static void
    saveSettings( QSettings& );

    /**
     * Save/load position of middle marker in QSettings.
     * Variable name: prefix_middleMarker
     */
    void
    saveMiddleMarker( QSettings& settings,
                      QString    prefix );
    void
    loadMiddleMarker( QSettings& settings,
                      QString    prefix );

signals:
    /**
     * Signal - change one of colors in colormap.
     */
    void
    colorChanged( ColorMapPlot::Color,
                  const QColor& );

    /**
     * Signal - informs that the user changed position of marker.
     */
    void
    markerChanged( ColorMapPlot::MarkersPositions::Position pos,
                   double                                   val );

protected:

    /**
     * Overridden from parent object.
     */

    bool
    event( QEvent* );

    /**
     * Marker position object.
     */
    MarkersPositions markerValues;

    /**
     * Current position of mouse.
     * If negative -> mouse released, movement doesn't change divergence.
     * If positive -> contains last position of mouse, so by mouse movement
     * one can easily compute change.
     */
    int startingPosition;

    /**
     * Current positions of pointers (in %).
     */
    double currentPositions[ 3 ];

    /**
     * If true, then it's possible to change selected colors.
     * Position given by the enum Color.
     */
    bool enabledColorChange[ 3 ];

    /**
     * When true, the plot will draw middle point marker and
     * process the changes.
     * Default value: true;
     */
    bool enabledMiddleMark;

    /**
     * Currently changing pointer (by mouse event).
     */
    MarkersPositions::Position currentlyChanging;

    /**
     * Selected color (by mouse double click).
     */
    Color doubleClickedColor;

    /**
     * Selected marker with a double click.
     */
    MarkersPositions::Position doubleClickedMarker;

    /**
     * Currently double clicked object.
     */
    DoubleClick currentlyDoubleClicked;

    /**
     * Color picker, used when double click was performed.
     */
    QColorDialog colorPicker;

    /**
     * Context menu, used for adjust/reset.
     */
    QMenu contextMenu;

    /**
     * Qt mouse event methods.
     */

    void
    mousePressEvent( QMouseEvent* event );

    void
    mouseReleaseEvent( QMouseEvent* event );

    void
    mouseMoveEvent( QMouseEvent* event );

    void
    mouseDoubleClickEvent( QMouseEvent* event );

    /**
     * Overrriden QWidget method.
     * @param event
     */
    void
    paintEvent( QPaintEvent* event );

    /**
     * Normalize marker position.
     * @param pos marker to normalize
     */
    void
    normalize( MarkersPositions::Position pos );

    /**
     * Normalize middle marker - value and position will be
     * between begin and end marker.
     */
    void
    normalizeMiddleMarker();

    /**
     * Normalize begin marker - value and position below middle marker.
     */
    void
    normalizeBeginMarker();

    /**
     * Normalize end marker - value and position above middle marker.
     */
    void
    normalizeEndMarker();

private:

    /**
     * Cached values of variables which change should be reverted after 'cancel'
     */
    double cachedMiddleMarkerPos;

    double cachedFilteringValues[ 6 ];

    double cachedCurrentPositions[ 3 ];

    /**
     * Contains current location of the triangular indicator
     * and marker value - left upper point (x,y) and right bottom (x,y).
     * Used for location of double click.
     */
    int GUI_markerValuePosition[ 3 ][ 4 ];

    /**
     * Default width.
     */
    static const int WIDTH;

    /**
     * Space in width between border and plot.
     */
    static const double WIDGET_WIDTH_BORDER;

    /**
     * Default height.
     */
    static const int HEIGHT;

    /**
     * Maximal height of the plot.
     */
    static const int MAX_HEIGHT;

    /**
     * Default width of font.
     */
    static const int TEXT_WIDTH;

    /**
     * Default height of font.
     */
    static const int TEXT_HEIGHT;

    /**
     * Default spacing between text and pointer (triangle indicator).
     */
    static const int TEXT_SPACING;

    /**
     * Number of positions on the scale.
     */
    static const int SCALE_COUNT;

    /**
     * Height of scale.
     */
    static const int SCALE_HEIGHT;

    /**
     * Height of triangle indicator.
     */
    static const int TRIANGLE_HEIGHT;

    /**
     * Width of triangle indicator.
     */
    static const int TRIANGLE_WIDTH;

    /**
     * Number of items in context menu.
     */
    static const int CONTEXT_MENU_ITEMS_NUM = 3;

    /**
     * Items in context menu.
     */
    static const QString CONTEXT_MENU_ITEMS[ CONTEXT_MENU_ITEMS_NUM ];

    /**
     * Function pointer - no arguments, no return value.
     */
    typedef void ( ColorMapPlot::* EventHandler )();

    /**
     * Handlers for menu events.
     */
    EventHandler CONTEXT_MENU_HANDLERS[ CONTEXT_MENU_ITEMS_NUM ];

    /**
     * Precision (number of digits in fractional part) of displayed marker value for "percentage" mode.
     * It's a basic precision, one digit is always added when the width of plot (distance between lower
     * and upper mark) is smaller by one order of magnitude.
     */
    static const int MARKER_TEXT_PRECISION;

    /**
     * Maximal number of digits in fractional part of displayed text.
     */
    static const int MARKER_TEXT_PRECISION_MAX;

    /**
     * X component of plot localization in the widget.
     */
    int
    getPlotX();

    /**
     * Y component of plot localization in the widget.
     */
    int
    getPlotY();

    /**
     * Plot width.
     */
    int
    getPlotWidth();

    /**
     * Plot height.
     */
    int
    getPlotHeight();

    /**
     * @return difference between the upper marker and the lower one
     */
    double
    getMarkersRange();

    /**
     * Change settings of plot to the default ones.
     */
    void
    resetPlot();

    /**
     * "Scale" - move the plot markers to the boundaries.
     */
    void
    adjustPlot();

    /**
     * "Invert" - invert color scheme.
     */
    void
    invertPlot();

    /**
     * Find marker value (e.g. triangle/text) for given coordinates.
     * @param x
     * @param y
     * @return -1 if no marker is present at given coordinates
     */
    int
    findMarkerValueObject( int x,
                           int y );

    /**
     * Reset the position of middle marker, according to current values
     * and positions of filtering markers.
     */
    void
    repositionMiddleMarker();

private slots:
    /**
     * Slot - color selected in QColorDialog.
     */
    void
    selectedColor( const QColor& );

    /**
     * Slot - display context menu.
     */
    void
    showContextMenu( const QPoint& );
};


#endif /* SRC_GUI_QT_PLUGINS_ADVANCEDCOLORMAPS_COLORMAPPLOT_H_ */

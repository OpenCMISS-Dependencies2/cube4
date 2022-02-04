/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef GGLOBALS_H
#define GGLOBALS_H

#include <QDebug>
#include <QColor>
#include <QString>
#include <QStringList>
#include <QList>
#include <QFont>
#include <QVector>

/**
 * Project-wide enumerations and names which are used interchangeably in both logic and UI
 */
namespace plotWidget
{
const int VERTICAL_PRECISION = 500; /**<  Used to normalize the data between Min and Max, assuming that Min is 0 and Max is VERTICAL_PRECISION */

/**< Names of the calculations that can be done. These names are used in the UI and consistant with the corresponding enum */
const QStringList CALC_NAMES = QStringList() << QObject::tr( "Minimum" ) << QObject::tr( "Maximum" ) << QObject::tr( "Average" ) << QObject::tr( "Median" ) << QObject::tr( "1st Quartile" ) << QObject::tr( "3rd Quartile" ) << QObject::tr( "All (Max/Avg/Min)" );

/**< Default calculation colors */
//const QList<QColor> CALC_COLORS = QList<QColor>()<<Qt::darkGreen<<Qt::darkRed<<Qt::darkBlue<<Qt::darkMagenta<<Qt::darkRed<<Qt::darkYellow<<Qt::black;
//const QList<QColor> CALC_COLORS = QList<QColor>()<<Qt::green<<Qt::red<<Qt::blue<<Qt::darkMagenta<<Qt::darkRed<<Qt::yellow<<Qt::black;
const QList<QColor> CALC_COLORS = QList<QColor>() << QColor( "green" ) << QColor( "red" ) << QColor( "blue" ) << QColor( "darkMagenta" ) << QColor( "darkRed" ) << QColor( "yellow" ) << QColor( "black" );

/**< Names of the drawing styles. These names are used in the UI and consistant with the corresponding enum */
const QStringList STYLES_NAMES = QStringList() << QObject::tr( "Filled" ) << QObject::tr( "Line" ) << QObject::tr( "Dots" );

/**< Calculations that can be done*/
enum MathOp
{
    Minimum = 0, Maximum, Average, Median, FirstQuartile, ThirdQuartile, MathOpUnknown
};

/**< A general enumeration usef for the alignment of the widgets*/
enum Alignment
{
    Center = 0, Left, Top, Right, Bottom
};

/**< Drawing styles */
enum Style
{
    Filled = 0, Line, Dots
};

/**< The default thickness of a major notch */
const int MAJOR_NOTCH_SIZE = 12;
/**< The default thickness of a minor notch */
const int MINOR_NOTCH_SIZE = 8;
/**< The default color of a major notch */
const QColor MAJOR_NOTCHES_COLOR = QColor( Qt::darkGray );
/**< The default color of a minor notch */
const QColor MINOR_NOTCHES_COLOR = QColor( Qt::darkGray );
/**< The default number of plot widget major notches to be displayed on the plot widget */
const int DEFAULT_PLOT_WIDGET_MAJOR_NOTCHES_COUNT = 3;
/**< The default number of processes per plot widget major notch to be displayed on the plot widget */
const int DEFAULT_PLOT_WIDGET_MAJOR_NOTCHES_INTERVAL = 30;
/**< The default number of plot widget minor notches to be displayed on the plot widget */
const int DEFAULT_PLOT_WIDGET_MINOR_NOTCHES_COUNT = 2;
/**< The default max value of the plot widget horizontal axis */
const int DEFAULT_PLOT_WIDGET_H_AXIS_MAX = 100;
/**< The default min value of the plot widget horizontal axis */
const int DEFAULT_PLOT_WIDGET_H_AXIS_MIN = 0;
/**< The default max value of the plot widget vertical axis */
const int DEFAULT_PLOT_WIDGET_V_AXIS_MAX = 1;
/**< The default min value of the plot widget vertical axis */
const int DEFAULT_PLOT_WIDGET_V_AXIS_MIN = 0;
/**< The default number of heat widget horizontal major notches to be displayed on the plot widget */
const int DEFAULT_HEAT_WIDGET_H_MAJOR_NOTCHES_COUNT = 3;
/**< The default number of processes per heat widget horizontal major notch to be displayed on the plot widget */
const int DEFAULT_HEAT_WIDGET_H_MAJOR_NOTCHES_INTERVAL = 30;
/**< The default number of heat widget horizontal minor notches to be displayed on the plot widget */
const int DEFAULT_HEAT_WIDGET_H_MINOR_NOTCHES_COUNT = 2;
/**< The default number of heat widget vertical major notches to be displayed on the plot widget */
const int DEFAULT_HEAT_WIDGET_V_MAJOR_NOTCHES_COUNT = 3;
/**< The default number of processes per heat widget vertical major notch to be displayed on the plot widget */
const int DEFAULT_HEAT_WIDGET_V_MAJOR_NOTCHES_INTERVAL = 30;
/**< The default number of heat widget vertical minor notches to be displayed on the plot widget */
const int DEFAULT_HEAT_WIDGET_V_MINOR_NOTCHES_COUNT = 2;
/**< The default max value of the heat widget horizontal axis */
const int DEFAULT_HEAT_WIDGET_H_AXIS_MAX = 100;
/**< The default min value of the heat widget horizontal axis */
const int DEFAULT_HEAT_WIDGET_H_AXIS_MIN = 0;
/**< The default max value of the heat widget vertical axis */
const int DEFAULT_HEAT_WIDGET_V_AXIS_MAX = 100;
/**< The default min value of the heat widget vertical axis */
const int DEFAULT_HEAT_WIDGET_V_AXIS_MIN = 0;
/**< The default legend spacing (in pixels) from the plot area edge */
const int DEFAULT_LEGEND_DISTANCE = 10;
/**< The default thickness of a ruler-West (in pixels)*/
const int DEFAULT_RULER_THICKNESS_WEST = 60;
/**< The default thickness of a ruler- North (in pixels)*/
const int DEFAULT_RULER_THICKNESS_NORTH = 40;
/**< The default color for the minimum value in the heatmap*/
const QColor DEFAULT_HEAT_MAP_MIN_COLOR = Qt::blue;
/**< The default color for the maximum value in the heatmap*/
const QColor DEFAULT_HEAT_MAP_MAX_COLOR = Qt::red;
}

/**
 * Utility function that returns the corresponding style enumeration item for a given math operation string
 * @param mathOpStr The name of the math operation
 * @return The enumeration item of the given math operation. If no math operation is found then (Minimum) math operation will be returned by default
 */
inline plotWidget::MathOp
qStringToMathOp( QString mathOpStr )
{
    int                mathOpIndex = plotWidget::CALC_NAMES.indexOf( mathOpStr );
    plotWidget::MathOp mathOp;
    switch ( mathOpIndex )
    {
        case 0:
            mathOp = plotWidget::Minimum;
            break;
        case 1:
            mathOp = plotWidget::Maximum;
            break;
        case 2:
            mathOp = plotWidget::Average;
            break;
        case 3:
            mathOp = plotWidget::Median;
            break;
        case 4:
            mathOp = plotWidget::FirstQuartile;
            break;
        case 5:
            mathOp = plotWidget::ThirdQuartile;
            break;
        default:
            mathOp = plotWidget::MathOpUnknown;
            break;
    }
    return mathOp;
}

/**
 * Utility function that returns the corresponding style enumeration item for a given style string
 * @param styleStr The name of the style
 * @return The enumeration item of the given style. If no match is found then (Filled) style will be returned by default
 */
inline plotWidget::Style
qStringToStyle( QString styleStr )
{
    int               styleIndex = plotWidget::CALC_NAMES.indexOf( styleStr );
    plotWidget::Style style      = plotWidget::Filled;
    switch ( styleIndex )
    {
        //case 0: style = Filled; break;
        case 1:
            style = plotWidget::Line;
            break;
        case 2:
            style = plotWidget::Dots;
            break;
    }
    return style;
}

#endif //  GLOBALS_H

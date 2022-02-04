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



#include "config.h"



#include <QString>
#include <QPainter>
#include <QLineEdit>

#include "ValueWidget.h"
#include "Globals.h"
#include "Tree.h"

#include <cassert>
#include <iostream>
#include <cmath>

using namespace cubegui;

ValueWidget::ValueWidget( QWidget* parent ) : QWidget( parent )
{
    QFontMetrics fm( font() );

    statistics           = 0;
    useDefaultStatistics = true;
    setMinimumHeight( 2 * ( fm.lineSpacing() + 3 ) );
    setMinimumWidth( 100 );
    setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    setWhatsThis( tr( "Below each pane there is a selected value information widget. "
                      "If no data is loaded, the widget is empty. Otherwise, the "
                      "widget displays more extensive and precise information about "
                      "the selected values in the tree above. This information widget "
                      "and the topologies may have different precision settings than "
                      "the trees, such that there is the possibility to display more "
                      "precise information here than in the trees (see menu Display "
                      "-> Precision).\nThe widget has a two-line display. The first "
                      "line displays at most 4 numbers. The leftmost number shows "
                      "the smallest value in the tree (or 0.0 in any percentage value "
                      "mode for trees, or the user-defined minimal value for coloring "
                      "if activated), and the rightmost number shows the largest "
                      "value in the tree (or 100.0 in any percentage value mode in "
                      "trees, or the user-defined maximal value for coloring if "
                      "activated). Between these two numbers the current value of the "
                      "selected node is displayed, if it is defined. Additionally, in "
                      "the absolute value mode it follows in brackets the percentage "
                      "of the selected value on the scale between the minimal and "
                      "maximal values. Note that the values of expanded non-leaf "
                      "system nodes and of nodes of trees on the left-hand-side of "
                      "the metric tree are not defined.\nIf the value mode is not the "
                      "absolute value mode, then in the second line similar "
                      "information is displayed for the absolute values in a light "
                      "gray color.\nIn case of multiple selection, the information "
                      "refers to the sum of all selected values. In case of multiple "
                      "selection in system trees in the peer distribution and in the "
                      "peer percent modes this sum does not state any valuable "
                      "information, but it is displayed for consistency reasons.\n"
                      "When a system topology is shown with a percentage value mode, "
                      "the mean \"+/-\" standard deviation of all values is presented "
                      "when no process or thread is selected.\n"
                      "If the widget width is not large enough to display all numbers "
                      "in the given precision, then a part of the number displays get "
                      "truncated and \"...\" ellipsis indicates that not all digits "
                      "could be displayed.\nBelow these numbers, a small "
                      "color bar shows the position of the color of the selected node "
                      "in the color legend. In case of undefined values, the legend "
                      "is filled with a gray grid." ) );
}

void
ValueWidget::setStatistics( TreeStatistics* s )
{
    statistics = s;
}

ValueWidget::~ValueWidget()
{
}

void
ValueWidget::paintEvent( QPaintEvent* )
{
    QFontMetrics fm( font() );

    QPainter painter;
    painter.begin( this );

    QPalette palette = QLineEdit().palette(); // use fore- and background color of QLineEdit

    // draw a rectangle
    painter.fillRect( 0, 0, width() - 1, height() - 1, palette.base() );
    painter.setPen( palette.windowText().color() );
    painter.drawRect( 0, 0, width() - 1, height() - 1 );
    // divide the rectangle into two parts for the numbers and the color gradient
    painter.drawLine( 0, height() - 6, width() - 1, height() - 6 );

    // if color gradient should be painted
    if ( !selectedValueString.isEmpty() )
    {
        // define color gradient
        QLinearGradient linearGrad( QPointF( 0, 0 ), QPointF( width(), 0 ) );
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        QList<QGradientStop> gradStops;
#else
        QVector<QGradientStop> gradStops;
#endif
        unsigned grades = 100;
        for ( unsigned i = 0; i <= grades; i++ )
        {
            gradStops.push_back( QPair<qreal, QColor>( ( double )i / ( double )grades, Globals::getColor( ( double )i, 0.0, ( double )grades ) ) );
        }
        linearGrad.setStops( gradStops );
        // fill out the available scale until the value percentage with the gradient
        int xPos = ( int )( fabs( percent ) * ( double )width() / 100.0 );
        painter.fillRect( 2, height() - 5, xPos, 3, linearGrad );
    }
    else
    {
        // no color gradient, but gray filled scale (no values defined)
        QBrush brush( Qt::black, Qt::Dense5Pattern );
        painter.fillRect( 2, height() - 5, width() - 2, 3, brush );
    }

    // draw min/max value in the 1rst row
    if ( !minValueString.isEmpty() && !maxValueString.isEmpty() )
    {
        painter.setPen( palette.windowText().color() );
        painter.drawText( 2, 2, width() - 4, height() - 10, Qt::AlignLeft  | Qt::AlignTop, minValueString );
        painter.drawText( 2, 2, width() - 4, height() - 10, Qt::AlignRight | Qt::AlignTop, maxValueString );
    }

    const int ypos = height() - 8;
    //todo int ypos = fm.lineSpacing();
    if ( !selectedValueString.isEmpty() ) // draw selected value in 1st row
    {
        int minWidth = fm.boundingRect( minValueString ).width();
        int maxWidth = fm.boundingRect( maxValueString ).width();
        painter.drawText( minWidth + 4, 2,
                          width() - minWidth - maxWidth - 8,
                          ypos, Qt::AlignHCenter | Qt::AlignTop, selectedValueString );
    }
    if ( !absValueString.isEmpty() ) // draw second row
    {
        int minWidth = fm.boundingRect( minAbsValueString ).width();
        int maxWidth = fm.boundingRect( maxAbsValueString ).width();
        painter.setPen( Qt::gray );
        painter.drawText( 2, 2, width() - 4, ypos, Qt::AlignLeft  | Qt::AlignBottom, minAbsValueString );
        painter.drawText( 2, 2, width() - 4, ypos, Qt::AlignRight | Qt::AlignBottom, maxAbsValueString );
        painter.drawText( minWidth + 4, 2,
                          width() - minWidth - maxWidth - 8, ypos,
                          Qt::AlignHCenter | Qt::AlignBottom,
                          absValueString );
    }

    painter.end();
}

void
ValueWidget::update()
{
    if ( statistics && statistics->isValid() && useDefaultStatistics )
    {
        double mean         = statistics->mean();
        double varianceSqrt =  statistics->varianceSqrt();
        if ( std::isnan( statistics->selectedValue() ) )
        {
            mean         = std::nan( "" );
            varianceSqrt = std::nan( "" );
        }
        createLabel( statistics->selectedValue(), statistics->minValue(), statistics->maxValue(),
                     statistics->absValue(), statistics->minAbsValue(), statistics->maxAbsValue(),
                     mean, varianceSqrt );
        QWidget::update();
    }
    else
    {
        this->clear();
        QWidget::update();
    }
}

void
ValueWidget::update( double value, double minValue, double maxValue,
                     double absValue, double minAbsValue, double maxAbsValue,
                     double mean, double varianceSqrt )
{
    useDefaultStatistics = false;
    createLabel( value, minValue, maxValue,
                 absValue, minAbsValue, maxAbsValue,
                 mean, varianceSqrt );
}

/**
 * create the 3 or 6 label for the widget: minimum value, selected value und maximum value and an additional line
 * for percentage value modi with absolute values
 */
void
ValueWidget::createLabel( double value, double minValue, double maxValue,
                          double absValue, double minAbsValue, double maxAbsValue,
                          double mean, double varianceSqrt )
{
    bool absoluteValueModus         = statistics ? statistics->isAbsoluteValueModus() : true;
    bool isInteger                  = statistics ? statistics->isInteger() : false;
    bool hasUserDefinedMinMaxValues = statistics ? statistics->hasUserDefinedMinMaxValues() : false;

    QFontMetrics fm( font() );

    minValueString.clear();
    maxValueString.clear();
    selectedValueString.clear();
    minAbsValueString.clear();
    maxAbsValueString.clear();
    absValueString.clear();
    percent = 0.0;

    if ( !std::isnan( value ) )
    {
        percent = maxValue - minValue;
        if ( percent != 0.0 )
        {
            percent = 100.0 * ( value - minValue ) / percent;
        }
        else
        {
            percent = 100.0;
        }
    }

    double absPercent = std::nan( "" );
    if ( !absoluteValueModus && !std::isnan( absValue ) )
    {
        absPercent = maxAbsValue - minAbsValue;
        if ( absPercent != 0.0 )
        {
            absPercent = 100.0 * ( absValue - minAbsValue ) / absPercent;
        }
        else
        {
            absPercent = 100.0;
        }
    }

    bool intType1 = ( absoluteValueModus ? isInteger : false );
    // type of the absolute value
    bool intType2 = isInteger;

    minValueString.append( Globals::formatNumber( minValue, intType1, FORMAT_DEFAULT ) );
    if ( hasUserDefinedMinMaxValues )
    {
        minValueString.append( "(u)" );
    }
    maxValueString.append( Globals::formatNumber( maxValue, intType1, FORMAT_DEFAULT ) );
    if ( hasUserDefinedMinMaxValues )
    {
        maxValueString.append( "(u)" );
    }
    if ( !std::isnan( value ) )
    {
        selectedValueString.append( Globals::formatNumber( value, intType1, FORMAT_DEFAULT ) );
        if ( minValue != 0.0 || maxValue != 100.0 )
        {
            selectedValueString.append( " (" );
            selectedValueString.append( Globals::formatNumber( percent, false, FORMAT_DEFAULT ) );
            selectedValueString.append( "%)" );
        }
    }
    else if ( !std::isnan( mean ) )
    {
        // no selected value but statistics -> clear first row
        minValueString.clear();
        maxValueString.clear();
    }


    int minWidth = fm.boundingRect( minValueString ).width();
    int selWidth = fm.boundingRect( selectedValueString ).width();
    int maxWidth = fm.boundingRect( maxValueString ).width();
    // if the strings to display are too wide, we shorten them and add "..." at the end
    if ( minWidth + selWidth + maxWidth > width() - 10 )
    {
        minValueString.append( "..." );
        maxValueString.append( "..." );
        selectedValueString.append( "..." );
        while ( ( minValueString.length() > 4 || selectedValueString.length() > 4 || maxValueString.length() > 4 ) &&
                minWidth + selWidth + maxWidth > width() - 10 )
        {
            if ( minValueString.length() > 4 && minValueString.length() >= selectedValueString.length() && minValueString.length() >= maxValueString.length() )
            {
                minValueString.chop( 4 );
                minValueString.append( "..." );
            }
            else if ( selectedValueString.length() > 4 && selectedValueString.length() >= maxValueString.length() )
            {
                selectedValueString.chop( 4 );
                selectedValueString.append( "..." );
            }
            else
            {
                assert( maxValueString.length() > 4 );
                maxValueString.chop( 4 );
                maxValueString.append( "..." );
            }
        }
    }

    if ( !absoluteValueModus ) // draw second line
    {
        // compute the absolute value strings for display
        minAbsValueString.append( Globals::formatNumber( minAbsValue, intType2, FORMAT_DEFAULT ) );
        maxAbsValueString.append( Globals::formatNumber( maxAbsValue, intType2, FORMAT_DEFAULT ) );

        if ( std::isnan( mean ) )
        {
            absValueString.append( Globals::formatNumber( absValue, intType2, FORMAT_DEFAULT ) );
            absValueString.append( " (" );
            absValueString.append( Globals::formatNumber( absPercent, false, FORMAT_DEFAULT ) );
            absValueString.append( "%)" );
        }
        else
        {
            absValueString.append( Globals::formatNumber( mean, intType1, FORMAT_DEFAULT ) );
            absValueString.append( " +/- " );
            absValueString.append( Globals::formatNumber( varianceSqrt, intType1, FORMAT_DEFAULT ) );
        }

        int minWidth = fm.boundingRect( minAbsValueString ).width();
        int selWidth = fm.boundingRect( absValueString ).width();
        int maxWidth = fm.boundingRect( maxAbsValueString ).width();
        // shorten them, too, if necessary
        if ( minWidth + selWidth + maxWidth > width() - 10 )
        {
            minAbsValueString.append( "..." );
            maxAbsValueString.append( "..." );
            absValueString.append( "..." );
            while ( ( minAbsValueString.length() > 4 || absValueString.length() > 4 || maxAbsValueString.length() > 4 ) &&
                    minWidth + selWidth + maxWidth > width() - 10 )
            {
                if ( minAbsValueString.length() > 4 && minAbsValueString.length() >= absValueString.length() && minAbsValueString.length() >= maxAbsValueString.length() )
                {
                    minAbsValueString.chop( 4 );
                    minAbsValueString.append( "..." );
                }
                else if ( absValueString.length() > 4 && absValueString.length() >= maxAbsValueString.length() )
                {
                    absValueString.chop( 4 );
                    absValueString.append( "..." );
                }
                else
                {
                    assert( maxAbsValueString.length() > 4 );
                    maxAbsValueString.chop( 4 );
                    maxAbsValueString.append( "..." );
                }
            }
        }
    }

    QWidget::update();
}

// clean up, no data loaded
void
ValueWidget::clear()
{
    minValueString.clear();
    maxValueString.clear();
    selectedValueString.clear();
    minAbsValueString.clear();
    maxAbsValueString.clear();
    absValueString.clear();
    percent = 0.0;
    QWidget::update();
}

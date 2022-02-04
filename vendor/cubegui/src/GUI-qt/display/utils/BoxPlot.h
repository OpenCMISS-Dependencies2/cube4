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



#ifndef BOXPLOT_H
#define BOXPLOT_H

#include <QWidget>
#include <QPainter>
#include <QString>

#include <utility>
#include <vector>
#include <string>
#include "StatisticalInformation.h"

namespace cubegui
{
class BoxPlot : public QWidget
{
    Q_OBJECT
public:
    BoxPlot( QWidget* parent );
    void
    Add( StatisticalInformation const& theStatisticalInformation );
    void
    Set( StatisticalInformation const& theStatisticalInformation,
         StatisticalInformation const& absStatisticalInformation,
         double                        start );
    void
    Reset();
    void
    ResetView();
    void
    SetMinMaxValues( double newMinVal,
                     double newMaxVal );

private:
    virtual void
    paintEvent( QPaintEvent* event );
    virtual void
    mousePressEvent( QMouseEvent* event );
    virtual void
    mouseReleaseEvent( QMouseEvent* event );

    /** returns the y coordinate which corresponds the the given value */
    int
    valueToYCoord( double value );

    double
    GetValue( int koord ) const;
    void
    DrawLeftLegend( QPainter& painter );
    void
    DrawLowerLegend( QPainter& painter );
    void
    DrawRightLegend( QPainter& painter );
    void
    ResetMinMaxValues();
    void
    setAxisScale( double min,
                  double max );
    void
    DrawBoxPlots( QPainter& painter );
    int
    GetCanvasWidth() const;
    int
    GetCanvasHeight() const;
    int
    GetLeftBorderWidth() const;
    int
    GetLowerBorderHeight() const;
    int
    GetUpperBorderHeight() const;
    int
    GetRightBorderWidth() const;
    int
    GetTextWidth( std::string const& theText ) const;
    int
    GetTextHeight( std::string const& theText ) const;
    int
    GetTextWidth( QString const& theText ) const;
    int
    GetTextHeight( QString const& theText ) const;
    StatisticalInformation const*
    GetClickedStatisticalInformation( QMouseEvent* event ) const;
    void
    ShowStatisticsTooltip( StatisticalInformation const* StatisticalInformation );
    void
    ShowStatistics( StatisticalInformation const* StatisticalInformation );
    double
    GetMinValue() const;
    double
    GetMaxValue() const;
    void
    DrawMark( QPainter&   painter,
              int         x,
              double      value,
              double      label,
              double      reference,
              QList<int>& oldPos );

    int                                 tickWidth;  // width of the label ticks
    int                                 borderPad;  // padding around the boxplots
    std::vector<double>                 yAxisScale; // position of the x axis tics
    double                              yAxisMin;   // miminum value of the x axis
    double                              yAxisMax;
    double                              start;      // the minimum value, the boxplot starts with (default 0.0)
    static int const                    digits              = 6;
    static int const                    numberOfAxisEntries = 5;
    std::vector<StatisticalInformation> items;
    StatisticalInformation              absolute;
    double                              minimumValue, maximumValue; // lowest and hightest value of the boxplot
    std::pair<int, int>                 firstPosition;
    QDialog*                            tooltip;
};
}
#endif

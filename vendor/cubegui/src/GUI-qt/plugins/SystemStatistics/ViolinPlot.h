/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef VIOLINPLOT_H
#define VIOLINPLOT_H

#include <QWidget>
#include <QPainter>
#include <QString>

#include <utility>
#include <vector>
#include <string>

#include "ViolinStatistics.h"
#include "PluginServices.h"
#include "CubePlugin.h"

namespace cubegui
{
class StatisticalInformation;
}

namespace system_statistics
{
class ViolinPlot : public QWidget
{
public:
    ViolinPlot( QWidget* parent );
    void
    set( ViolinStatistics const& theStatisticalInformation,
         double                  start = 0 );
    void
    set( const ViolinStatistics&                theStatisticalInformation,
         const cubegui::StatisticalInformation& abs,
         double                                 startValue );

    void
    setMinMaxValues( double newMinVal,
                     double newMaxVal );

    void
    selectKernel( DensityKernel kernel )
    {
        used_kernel = kernel;
    }

    void
    makeWidth( double width );

    void
    selectOrder( TaylorOrder value )
    {
        used_order = value;
    }

private:
    virtual void
    paintEvent( QPaintEvent* event );
    virtual void
    mousePressEvent( QMouseEvent* event );
    virtual void
    mouseReleaseEvent( QMouseEvent* event );

    void
    resetView();
    double
    GetValue( int koord ) const;
    void
    DrawLeftLegend( QPainter& painter );
    void
    DrawRightLegend( QPainter& painter );
    void
    ResetMinMaxValues();
    void
    AdjustMinMaxValues();
    void
    drawViolinPlot( QPainter& painter );
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
    void
    showStatisticsTooltip();
    void
    showStatistics();
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

    int                 tickWidth;                          // width of the label ticks
    int                 borderPad;                          // padding around the boxplots
    double              start;                              // the minimum value, the boxplot starts with (default 0.0)
    static int const    digits              = 6;
    static int const    numberOfAxisEntries = 5;
    double              minimumValue, maximumValue;
    std::pair<int, int> firstPosition;
    QDialog*            tooltip;
    DensityKernel       used_kernel;
    TaylorOrder         used_order;
    double              used_width;

    ViolinStatistics                item;
    cubegui::StatisticalInformation absolute;
};
}
#endif

#ifdef SELECT_KERNELS
void
UniK( bool ok );

void
TriangK( bool ok );

void
EpaK( bool ok );

void
QuarK( bool ok );

void
TriwK( bool ok );

void
TcubeK( bool ok );

void
GaussK( bool ok );

void
CosK( bool ok );

void
LogK( bool ok );

void
SigK( bool ok );

void
SilvK( bool ok );

void
widthSlider( int );

void
orderSLider( int );

//void
// mmlines(int);

#endif

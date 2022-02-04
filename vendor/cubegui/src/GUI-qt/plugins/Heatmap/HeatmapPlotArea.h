/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
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




#ifndef HEATMAPPLOTAREA_H_H
#define HEATMAPPLOTAREA_H_H

#include <QWidget>
#include <QList>
#include <QPoint>
#include <QMouseEvent>
#include <QVector>
#include "CubeDataItem.h"
#include "AbstractPlotArea.h"
#include "PluginServices.h"

class HeatMapPlotArea : public AbstractPlotArea
{
    // Q_OBJECT
public:
    explicit
    HeatMapPlotArea( QWidget* parent = 0 );



public:
    void
    doPaint( QRect     viewport,
             QPainter* painter );
    void
    setData( QVector<QVector<double> > data );
    double
    getData( int processId,
             int iterationId ) const;
    void
    setDataMin( double min );
    void
    setDataMax( double max );

    /*  void
       setDataMinColor( QColor dataMinColor );
       void
       setDataMaxColor( QColor dataMaxColor );*/
    void
    setPluginService( cubepluginapi::PluginServices* service );

    /* QColor
       getDataMinColor() const;
       QColor
       getDataMaxColor() const;*/
    void
    setLegendItemsCount( int legendItemsCount );
    int
    getLegendItemsCount() const;
    void
    reset();
    void
    setDataColor( QVector<QVector<QColor> > dataColortemp );

protected:
    virtual QPixmap
    generateLegend();
    virtual void
    initComponents();

private:
    QVector<QVector<double> > data;
    QVector<QVector<QColor> > dataColor;
    double                    dataMin, dataMax;
    // QColor                    dataMinColor, dataMaxColor;
    virtual void
    paintEvent( QPaintEvent* );

    // QColor
    // normalize( double value );
    // double
    // denormalize( int value );
    cubepluginapi::PluginServices* service;

    int legendItemsCount;
};

#endif // HEATMAPPLOTAREA_H_H

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




#ifndef HEATMAPCONTROLLER_H
#define HEATMAPCONTROLLER_H
#include <QObject>
#include <QColor>

#include "AbstractPlotAreaController.h"
#include "PluginServices.h"

//class HeatMapPlotArea;

class HeatMapPlotAreaController : public AbstractPlotAreaController
{
    Q_OBJECT
//public:
//  HeatMapPlotArea* view;
public:
    HeatMapPlotAreaController( QWidget* parent = 0 );
    void
    setDataColor( QVector<QVector< QColor> > tempDataColor );

public slots:
    void
    setData( QVector< QVector< double> >    data,
             double                         dataMin,
             double                         dataMax,
             cubepluginapi::PluginServices* service ) const;
    double
    getData( int processId,
             int iterationId ) const;
    void
    reset();
    void
    setLegendItemsCount( int legendItemsCount );
    int
    getLegendItemsCount() const;

    /*  void
       setColorMin( QColor min );
       void
       setColorMax( QColor max );*/
};

#endif // HEATMAPCONTROLLER_H

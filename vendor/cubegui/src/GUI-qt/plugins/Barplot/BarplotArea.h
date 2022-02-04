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



#ifndef BARPLOTAREA_H
#define BARPLOTAREA_H


#include <QWidget>
#include <QList>
#include <QPoint>
#include <QMouseEvent>
#include "CubeDataItem.h"
#include "AbstractPlotArea.h"
#include "GraphWidgetGlobals.h"


/**
 * This class is resposible for drawing the bars plots. It displays the plots according to their z-order, it also displays the legend. It offers a method to draw the plots using any QPainter so it can used to draw the plots on an image file for example.
 * Please note that you should not use this class directly as a placeholder for sending/receiving events. The corresponding controller must be used instead.
 * @see AbstractBarsPlotAreaController
 */

class BarPlotArea : public AbstractPlotArea
{
public:
    explicit
    BarPlotArea( QWidget* parent = 0 );

public:
    void
    doPaint( QRect     viewport,
             QPainter* painter );
    void
    setDataRef(QList<CubeDataItem> data);
    void
    reset();
//    void
  //  setDisplayStatus(bool newDisplayStatus);
protected:
    virtual void
    initComponents();
    virtual QPixmap
    generateLegend();
    virtual void
    paintEvent(QPaintEvent *);

    QList<CubeDataItem> data;
//bool displayStatus;

};

#endif // BARPLOTAREA_H

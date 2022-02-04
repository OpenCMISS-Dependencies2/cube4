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


#ifndef ABSTRACTPLOTAREACONTROLLER_H
#define ABSTRACTPLOTAREACONTROLLER_H


#include <QObject>
#include "AbstractPlotArea.h"

class AbstractPlotAreaController : public QObject
{
    Q_OBJECT
protected:
    AbstractPlotAreaController(QWidget *parent = 0);
public:
    AbstractPlotArea* getView();
signals:
    void mouseIsClicked(QMouseEvent* mouseEvent);
public slots:
    void updateVerticalMajorMeshLines(QList<int> VMajorNotches);
    void updateVerticalMinorMeshLines(QList<int> VMinorNotches);
    void updateHorizontalMajorMeshLines(QList<int> HMajorNotches);
    void updateHorizontalMinorMeshLines(QList<int> HMinorNotches);
    void updateMouseAxisHorizontalAlignment(plotWidget::Alignment alignment);
    void updateMouseAxisVerticalAlignment(plotWidget::Alignment alignment);
    void setLegendVisible(bool isLegendVisible);
    void updateLegendXAlignment(plotWidget::Alignment newXAlignment);
    void updateLegendYAlignment(plotWidget::Alignment newYAlignment);
    void setLegendBackgroundSolid(bool isLegendBackgroundSolid);
protected:
    void setConnections();
    AbstractPlotArea* view;
};

#endif // ABSTRACTPLOTAREACONTROLLER_H

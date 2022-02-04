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


#ifndef BARPLOTCONTROLLER_H
#define BARPLOTCONTROLLER_H

#include <QObject>
#include "AbstractPlotAreaController.h"

class BarsPlotAreaController : public AbstractPlotAreaController
{
    Q_OBJECT
public:
    BarsPlotAreaController(QWidget *parent = 0);

public slots:
    void
    updateData(QList<CubeDataItem>& data);
    void
    reset();
  //  void
    //setDisplayStatus(bool newDisplayStatus);
};

#endif // BARPLOTCONTROLLER_H

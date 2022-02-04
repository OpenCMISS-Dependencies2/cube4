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


#ifndef PLOTSLISTCONTROLLER_H
#define PLOTSLISTCONTROLLER_H

#include <QWidget>
#include <QList>
#include "DataProvider.h"

class PlotsListController: public QWidget
{
    Q_OBJECT
public:
    explicit PlotsListController(DataProvider* model, QWidget* parent = 0);
    virtual ~PlotsListController() {}

public slots:
    void
    setData(QList<CubeDataItem>& data); //shallow copy (COW)
    void
    deleteItemHandler();
private:
    DataProvider* model;
    QList<CubeDataItem> data; //probably not needed any more!
};

#endif // PLOTSLISTCONTROLLER_H

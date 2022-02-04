/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#ifndef CUBE_ADVISOR_TABLE_WIDGET_ITEM_H
#define CUBE_ADVISOR_TABLE_WIDGET_ITEM_H

#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QListWidget>
#include <QTableWidget>
#include "CubePlugin.h"
#include "PluginServices.h"

namespace advisor
{
class CubeAdvisorTableWidgetItem : public QTableWidgetItem
{
private:
    const cubegui::TreeItem* callpath_item;

public:
    CubeAdvisorTableWidgetItem(  const cubegui::TreeItem*,
                                 const QString& );

    const cubegui::TreeItem*
    getCallPathItem() const;
};
};

#endif // CUBE_TOOLS_H

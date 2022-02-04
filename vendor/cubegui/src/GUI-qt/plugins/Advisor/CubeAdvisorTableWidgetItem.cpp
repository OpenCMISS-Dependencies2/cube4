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


#include <config.h>
#include <QTableWidget>
#include "CubeAdvisorTableWidgetItem.h"

using namespace advisor;


CubeAdvisorTableWidgetItem::CubeAdvisorTableWidgetItem( const cubegui::TreeItem* c_item,
                                                        const QString&           text ) :
    QTableWidgetItem( text ),
    callpath_item( c_item )
{
}

const cubegui::TreeItem*
CubeAdvisorTableWidgetItem::getCallPathItem() const
{
    return callpath_item;
}

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


#ifndef METRICTREEVIEW_H
#define METRICTREEVIEW_H

#include "TreeView.h"

namespace cubegui
{
/**
 * MetricTreeView extends the default tree view with new context menu items
 */
class MetricTreeView : public TreeView
{
    Q_OBJECT
public:
    MetricTreeView( TreeModelInterface* modelInterface,
                    const QString&      tabLabel );
    virtual void
    selectionChanged( const QItemSelection& selected,
                      const QItemSelection& deselected );

protected:
    virtual void
    fillContextMenu();

    virtual QString
    getContextDescription( TreeItem* item );

    bool
    selectionIsValid( QList<TreeItem*>& oldItems,
                      TreeItem*         newItem );

private:
    enum MetricTreeContextMenuAction { MetricFullInfo };
    QHash <MetricTreeContextMenuAction, QAction*> metricContextMenuHash; // contains all tree specific context menu actions
};
}
#endif // METRICTREEVIEW_H

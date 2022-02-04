/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef CallTree_H
#define CallTree_H

#include <QSet>
#include <QHash>
#include <QPair>
#include "Tree.h"

namespace cubegui
{
class TreeItem;


/** interface to call tree specific tree functions */
class CallTree : public Tree
{
public:
    CallTree( cube::CubeProxy* cube ) : Tree( cube )
    {
        displayType = CALL;
    }
    virtual
    ~CallTree();

    /** @brief getSelectedNodes returns a list of all cnodes of the current call tree. If the tree is unmodified CubeProxy::ALL_CNODES
     * is returned. */
    virtual cube::list_of_cnodes
    getNodes() const = 0;

    /** @brief getSelectedNodes returns a list of all cnodes that correspond to the given tree items
     * In case of iterations or pruned items, the list of nodes may be longer than the list of of given tree items */
    virtual cube::list_of_cnodes
    getNodes( const QList<TreeItem*> items ) const = 0;

    /** @brief getSelectedNodes returns a list of all cnodes that correspond to the selected tree items.
     * In case of iterations or pruned items, the list of nodes may be longer than the list of of selected tree items */
    virtual cube::list_of_cnodes
    getSelectedNodes() const = 0;

    virtual bool
    isValidSelection() const
    {
        return true;
    }

    /** cache metric values */
    virtual void
    saveMetricValues( Tree* metric );
    virtual void
    restoreMetricValues( Tree* metric );

protected:
    QHash<TreeItem*, QPair<cube::Value*, cube::Value*> > metricValueHash;
};
}

#endif // CallTree_H

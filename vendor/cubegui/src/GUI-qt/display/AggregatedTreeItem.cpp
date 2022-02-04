/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"

#include "AggregatedTreeItem.h"
#include "DefaultCallTree.h"

#include "CubeCnode.h"

using namespace cubegui;

/*
 * creates a IterationTreeItem with the data of item and the given depth
 */
AggregatedTreeItem::AggregatedTreeItem( Tree* tree, int depth, TreeItem* item ) : TreeItem( tree, item->getName(), item->getType(), 0 )
{
    this->name          = item->getName();
    this->hidden        = item->isHidden();
    this->expanded      = item->isExpanded();
    this->tree          = item->getTree();
    this->textExpanded  = "- " + item->getName();
    this->textCollapsed = "- " + item->getName();

    // todo selected, hidden, visible
    this->depth = depth;
}

const QList<cube::Cnode*>&
AggregatedTreeItem::getIterations() const
{
    return iterations;
}

bool
AggregatedTreeItem::isAggregatedRootItem() const
{
    DefaultCallTree* tree = static_cast<DefaultCallTree*> ( getTree() );
    return this == tree->getAggregatedRootItem();
}

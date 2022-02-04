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
#ifndef DEFAULTCALLTREE_H
#define DEFAULTCALLTREE_H

#include <QSet>
#include <QHash>
#include <QPair>
#include "CallTree.h"
#include "AggregatedTreeItem.h"
#include "CubeTypes.h"

namespace cubegui
{
class TreeItem;

/** Default call tree */
class DefaultCallTree : public CallTree
{
    Q_OBJECT
public:
    DefaultCallTree( cube::CubeProxy*                   cube,
                     const std::vector< cube::Cnode* >& topCnodes );
    ~DefaultCallTree();

    virtual cube::list_of_cnodes
    getSelectedNodes() const;

    virtual cube::list_of_cnodes
    getNodes() const;

    virtual cube::list_of_cnodes
    getNodes( const QList<TreeItem*> items ) const;

    virtual QString
    getLabel() const;

    TreeItem*
    getTreeItem( uint32_t cnodeId ) const;

    // ----  iteration handling

    /** true, if iterations are aggregated */
    bool
    iterationsAreHidden() const;

    /** return the root item of the current loop */
    TreeItem*
    getLoopRootItem() const;

    /** return the root item of the aggregated loop */
    AggregatedTreeItem*
    getAggregatedRootItem() const;

    // ----  end iteration handling

    friend class CallTreeView;
    friend class TabManager;

protected:
    cube::list_of_cnodes        initialNodes; // cnodes, which were active at startup (default: ALL_CNODES)
    cube::list_of_cnodes        activeNodes;  // cnodes, which are currently used by the View (not cut)
    std::vector< cube::Cnode* > topCnodes;    // cnodes from which the tree is created

    virtual TreeItem*
    createTree();

    virtual QString
    getItemName( cube::Vertex* vertex ) const;

    /**
     * Set the basic inclusive and exclusive values in the full tree.
     * @param left  List of trees to the left of this tree
     * @param right List of trees to the right of this tree
     * @param itemsToCalculate, if list is not empty, only the given items are calculated, otherwise all visible items are calculated
     */
    virtual QList<Task*>
    setBasicValues( const QList<Tree*>&     leftTrees,
                    const QList<Tree*>&     rightTrees,
                    const QList<TreeItem*>& itemsToCalculate = QList<TreeItem*>( )
                    );

    void
    initTree();

private:
    /* calltree can aggregate loop iterations */
    TreeItem*           loopRoot;                       // item which is handled as a root of a loop and whose children are iterations
    QList<TreeItem*>    userRoots;                      // user has created new roots with cut -> set as root
    AggregatedTreeItem* aggregatedLoopRoot;             // root of the aggregated items of the currenly defined loop
    bool                iterationsAreHidden_;

    QHash<TreeItem*, QList<TreeItem*> > prunedChildren; // parent item -> pruned children

    // ---- iteration handling ----

    /** defines the given tree item as root of a loop whose children are are iterations */
    void
    setAsLoop( TreeItem* item,
               bool      reinit = true );

    /** hide iterations, show aggregated subtree */
    void
    hideIterations();

    /** show all iterations */
    void
    showIterations();

    void
    unsetLoop();
    void
    deleteMergedIterations( TreeItem* iter );
    void
    mergeIterations( TreeItem*               newParent,
                     const QList<TreeItem*>& iterations );
    AggregatedTreeItem*
    mergeIterations( TreeItem* loopRoot );

    // ---- end iteration handling ----

    /** replaces the current root items by the given ones */
    void
    setAsRoot( QList<TreeItem*> newRoots );

    void
    pruneItem( TreeItem* item );

    void
    updateActiveNodes();
    TreeItem*
    getDefinedLoopRoot();
};
}

#endif // DefaultCallTree_H

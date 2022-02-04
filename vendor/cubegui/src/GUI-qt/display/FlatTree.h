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


#ifndef FLATTREE_H
#define FLATTREE_H

#include "CallTree.h"

namespace cubegui
{
class TreeItem;
typedef cube::Region* RegionPointer;

class FlatTree : public CallTree
{
    Q_OBJECT
public:
    /** @brief creates a flat tree from the items of the given call tree
        @param callTree the call tree on whose items the flat tree is built
        @param taskTree optional value - if present, the items of this tree are added to the flat tree
     */
    FlatTree( CallTree* callTree,
              CallTree* taskTree );

    /** @brief result according to getNodes() for a special case: task tree is added and exclusive metric is selected
     * getSelectedNodes returns a list of all cnodes of the current flat tree if task tree is included and exclusive metric is selected */
    cube::list_of_cnodes
    getAllNodesMetricExclusive() const;

    /** @brief getNodes returns a list of all cnodes of the call tree on which the flat tree is based */
    cube::list_of_cnodes
    getNodes() const;

    /** @brief getNodes returns a list of all cnodes of the current flat tree that belong to the given items */
    cube::list_of_cnodes
    getNodes( const QList<TreeItem*> items ) const;

    /** @brief getSelectedNodes returns a list of all selected cnodes of the current flat tree */
    cube::list_of_cnodes
    getSelectedNodes() const;

    bool
    isValidSelection() const;

    bool
    hasTasks()
    {
        return this->taskTree != nullptr;
    }

    void
    saveMetricValues( Tree* metric );
    void
    restoreMetricValues( Tree* metric );

    /** metricChanged is required for special case: task tree items have been added and exclusive metric is selected and on the left */
    void
    metricSelected( const QList<TreeItem*>& metrics );

    friend class FlatTreeView;

protected:
    /** creates flat tree from call tree root item */
    virtual TreeItem*
    createTree();

    virtual QString
    getItemName( cube::Vertex* vertex ) const;

    /**
     * Set the basic inclusive and exclusive values in the full tree.
     * @param left  List of trees to the left of this tree
     * @param right List of trees to the right of this tree
     */
    virtual QList<Task*>
    setBasicValues( const QList<Tree*>&     leftTrees,
                    const QList<Tree*>&     rightTrees,
                    const QList<TreeItem*>& itemsToCalculate );

private slots:
    void
    rebuild();

private:
    CallTree*            callTree;          // tree from whose items this flat tree is created
    CallTree*            taskTree;          // if set, items from task tree added to the flat tree
    cube::list_of_cnodes activeNodes;       // currently used nodes
    QList<TreeItem*>     subroutines;       // items of the child level

    // special case task tree and exclusive metric
    cube::list_of_cnodes                       flatTreeExclRootNodes; // special case: task tree and exclusive metric
    QHash<cube::Region*, cube::list_of_cnodes> tasksToAdd;            // parents of task region -> corresponding task tree children to add
    QSet<TreeItem*>                            invalidParents;        // flat tree items that cannot be calculated for exclusive metrics
    bool                                       isExclusiveMetric;

    void
    computeValue( cube::list_of_metrics& mv,
                  TreeItem*              item );
    void
    computeValue( cube::list_of_metrics&      mv,
                  cube::list_of_sysresources& sv,
                  TreeItem*                   item );

    void
    mergeExclusiveTaskItems( QHash<RegionPointer, cube::list_of_cnodes>& tasksToAdd,
                             QList<TreeItem*>&    toRemove,
                             QSet<cube::Region*>& invalidRegions );
};
}
#endif // FLATTREE_H

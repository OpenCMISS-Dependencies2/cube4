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


#ifndef SYSTEMTREE_H
#define SYSTEMTREE_H

#include "Tree.h"

namespace cubegui
{
class SystemTree : public Tree
{
    Q_OBJECT
public:
    SystemTree( cube::CubeProxy* cubeProxy );

    cube::list_of_sysresources
    getSelectedNodes();

    QList<TreeItem*>
    getVisitedItems() const;

    TreeItem*
    getTreeItem( uint32_t systemId ) const;

protected:
    TreeItem*
    createTree();

    virtual QString
    getItemName( cube::Vertex* vertex ) const;

    /**
     * Assign inclusive and exclusive values to tree items recursively.
     * @param item  Root tree item
     * @param inclusive_values  Vector of inclusive values indexed by Id.
     * @param exclusive_values  Vector of exclusive values indexed by Id.
     * @note Default implementation has to be overridden to use get_sys_id() over get_id().
     */
    void
    assignValues( TreeItem*                          item,
                  const std::vector< cube::Value* >& inclusive_values,
                  const std::vector< cube::Value* >& exclusive_values );


    /**
     * Set the basic inclusive and exclusive values in the full tree.
     * @param left  List of trees to the left of this tree
     * @param right List of trees to the right of this tree
     * @param itemsToCalculate, if list is not empty, only the given items are calculated, otherwise all visible items are calculated
     */
    virtual QList<Task*>
    setBasicValues( const QList<Tree*>&     leftTrees,
                    const QList<Tree*>&     rightTrees,
                    const QList<TreeItem*>& itemsToCalculate = QList<TreeItem*>( ) );

private:
    void
    computeMinMax();
    bool
    calculationRequired( const QList<TreeItem*>& itemsToCalculate );

    /**
     * Assign inclusive and exclusive values to single tree item.
     * @param item
     * @param inclusiveValue
     * @param exclusiveValue
     */
    void
    assignValues( TreeItem*                        item,
                  const std::vector<cube::Value*>& inclusive_values,
                  const std::vector<cube::Value*>& exclusive_values,
                  int                              depth,
                  cube::IdIndexMap*                indexMap );

    friend class SystemTreeData;
    friend class SystemTreeFlatData;
};


/** for use in QConcurrent */
class SystemTreeData : public Task
{
public:
    SystemTreeData( SystemTree* t, const cube::list_of_metrics& m, const cube::list_of_cnodes& c ) :
        tree( t ), metric_selection( m ), cnode_selection( c )
    {
    }

    virtual void
    calculate();

private:
    SystemTree*                 tree;
    const cube::list_of_metrics metric_selection;
    const cube::list_of_cnodes  cnode_selection;
    cube::Cnode*                root_cnode;
};

class SystemTreeFlatData : public Task
{
public:
    SystemTreeFlatData( SystemTree* t, const cube::list_of_metrics& m, const cube::list_of_regions& r ) :
        tree( t ), metric_selection( m ), region_selection( r )
    {
    }

    virtual void
    calculate();

private:
    SystemTree*                 tree;
    const cube::list_of_metrics metric_selection;
    const cube::list_of_regions region_selection;
    cube::Cnode*                root_cnode;
};
}
#endif // METRICTREE_H

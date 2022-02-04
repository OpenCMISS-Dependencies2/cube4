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


#include "config.h"

#include <QSet>
#include "FlatTree.h"
#include "CubeMetric.h"
#include "CubeRegion.h"
#include "TreeView.h"
#include "CubeProxy.h"
#include "TabManager.h"

using namespace std;
using namespace cube;
using namespace cubegui;

FlatTree::FlatTree( CallTree* baseTree, CallTree* taskTree = nullptr ) : CallTree( baseTree->getCube() )
{
    treeType       = FLATTREE;
    callTree       = baseTree;
    this->taskTree = taskTree;
    // rebuild flat tree if corresponding  call tree structure changes (e.g. reroot)
    connect( callTree->getModel(), SIGNAL( treeStructureChanged() ), this, SLOT( rebuild() )  );
    label = tr( "Flat tree" );
}

QString
FlatTree::getItemName( cube::Vertex* vertex ) const
{
    return QString( ( ( cube::Region* )vertex )->get_name().c_str() );
}

void
FlatTree::rebuild()
{
    TreeModel* model = treeModel;

    model->removeItem( getRootItem() );
    initialize();
    model->addItem( getRootItem() );
}

/**
 * @brief mergeExclusiveTaskItems handles OMP special case, if a task tree is merged to the flat tree and an exclusive metric is selected
 * @param toRemove items of the default call tree, that should be ignored in the flat tree because its values are already given by
 * the corresponding task tree items
 * @param tasksToAdd hash: region -> tasks to add to the flat tree item. The task leafs of the call tree correspond to items of the task tree,
 * which have children. The inclusive values of these children have to be added to the flat tree.
 * @param invalidRegions regions that cannot be calculated for exclusive metrics, because they contain an unknown part of a task tree item
 */
void
FlatTree::mergeExclusiveTaskItems( QHash<RegionPointer, cube::list_of_cnodes>& tasksToAdd,
                                   QList<TreeItem*>& toRemove,
                                   QSet<cube::Region*>& invalidRegions )
{
    QHash<TreeItem*, TreeItem*>          callHash;       // call item -> task item
    QHash<cube::Region*, TreeItem*>      regionTaskHash; // region -> task item
    QMultiHash<cube::Region*, TreeItem*> regionCallHash; // region -> call item (multiple entries possible)

    // fill region -> task item Hash (for all regions of the children of the task tree artificial root nodes)
    for ( TreeItem* rootItem : taskTree->getRootItem()->getChildren() ) // task tree artificial root nodes
    {
        for ( TreeItem* flatItem : rootItem->getChildren() )
        {
            Cnode* cnode = static_cast<Cnode*> ( flatItem->getCubeObject() );
            if ( cnode )
            {
                cube::Region* task_region = cnode->get_callee();
                regionTaskHash[ task_region ] = flatItem;
            }
        }
    }

    // find leafs of the default call tree, which are also children of the task tree artificial root nodes
    // fill hashes (for all regions of the children of the task tree artificial root nodes)
    // - region -> call items multi hash
    // - call item -> task item
    for ( TreeItem* callItem : callTree->getRootItem()->getLeafs() )
    {
        Cnode*        cnode  = static_cast<Cnode*> ( callItem->getCubeObject() );
        cube::Region* region = cnode->get_callee();
        if ( regionTaskHash.contains( region ) )
        {
            TreeItem* taskItem = regionTaskHash[ region ];
            callHash[ callItem ] = taskItem;
            regionCallHash.insert( region, callItem );

            toRemove.append( callItem ); // avoid double accounting: remove call tree leafs which are also part of the task tree
        }
    }

    // fill tasksToAdd to add children of the corresponding task item to all parents of the call tree
    // step1: fill parentHash: call item parent -> tasks items of all children
    QHash<TreeItem*, QSet<TreeItem*> > parentHash; // call -> task
    for ( TreeItem* callItem : callHash.keys() )
    {
        TreeItem* taskItem = callHash[ callItem ];

        TreeItem* parent = callItem->getParent();
        while ( parent->getParent() != nullptr ) // add task item to all parent items
        {
            parentHash[ parent ].insert( taskItem );
            parent = parent->getParent();
        }
    }
    // step2: fill tasksToAdd
    // add children of the corresponding task tree item. These children are are not part of the call tree.
    for ( TreeItem* callItem : parentHash.keys() ) // for all parent call tree items
    {
        cube::list_of_cnodes list;
        for ( TreeItem* taskItem : parentHash[ callItem ] ) // all corresponding task items
        {
            for ( TreeItem* taskChild : taskItem->getChildren() )
            {
                cnode_pair pair;
                pair.first  = static_cast<Cnode*> ( taskChild->getCubeObject() );
                pair.second = CUBE_CALCULATE_INCLUSIVE;
                list.push_back( pair );
            }
        }
        Cnode*        cnode  = static_cast<Cnode*> ( callItem->getCubeObject() );
        cube::Region* region = cnode->get_callee();
        tasksToAdd[ region ] = list; //
    }

    // mark items, that cannot be calculated for exclusive metrics:
    // find tasks, whose region is referred from several call tree items; their call tree parents cannot be calculated in flat tree
    for ( cube::Region* region : regionCallHash.uniqueKeys() )
    {
        QList<TreeItem*> sameRegion = regionCallHash.values( region );
        if ( sameRegion.size() > 1 ) // region is referred from multiple call tree items
        {
            QHash<TreeItem*, int> count;
            for ( TreeItem* item : sameRegion )
            {
                TreeItem* parent = item->getParent();
                while ( parent != nullptr )
                {
                    count[ parent ]++;
                    parent = parent->getParent();
                }
            }
            for ( TreeItem* item : sameRegion )
            {
                int       allReferences = sameRegion.size(); // number of all call tree items that refer to the region sameRegion
                TreeItem* parent        = item->getParent();
                while ( parent != callTree->getRootItem() )
                {
                    Cnode*        cnode   = static_cast<Cnode*> ( parent->getCubeObject() );
                    cube::Region* pregion = cnode->get_callee();
                    if ( count[ parent ] < allReferences ) // not all referrers of the region are yet part of the subtree -> cannot calculate
                    {
                        invalidRegions.insert( pregion );
                    }
                    parent = parent->getParent();
                }
            }
        }
    }
}


/**
 * @brief FlatTree::createTree
 * creates a flat tree that has only one level of items.
 */
TreeItem*
FlatTree::createTree()
{
    std::vector< cube::Region* > usedRegions;

    for ( TreeItem* subroutine : subroutines )
    {
        delete subroutine;
    }
    subroutines.clear();

    QList<TreeItem*> allItems = callTree->getItems();

    QSet<cube::Region*> invalidRegions;                                     // OMP special case
    if ( taskTree )                                                         // also add task tree items to flat tree
    {
        QList<TreeItem*> taskItems = taskTree->getItems();                  // all tasks tree items, that will be added to the flat tree
        for ( TreeItem* rootItem : taskTree->getRootItem()->getChildren() ) // task tree artificial root nodes
        {
            // don't add the artificial root items to flat tree
            taskItems.removeOne( rootItem );
        }
        QList<TreeItem*> toRemove;

        mergeExclusiveTaskItems( tasksToAdd, toRemove, invalidRegions );

        for ( TreeItem* item : toRemove )
        {
            allItems.removeOne( item );
        }
        allItems += taskItems;
    }


    QSet<cube::Region*> regionSet;
    // for all regions: erase and reinit list of cnodes from whom the region is called from; fill usedRegions
    for ( TreeItem* item : allItems )
    {
        // todo handle exclusive value of loop root
        if ( item->isAggregatedLoopItem() )
        {
            const QList<cube::Cnode*>& iterations = static_cast<AggregatedTreeItem*>( item )->getIterations();
            for ( Cnode* cnode : iterations )
            {
                cube::Region* region = cnode->get_callee();
                if ( !regionSet.contains( region ) ) // found new region entry -> clear cnodes and fill afterwards
                {
                    regionSet.insert( region );
                    region->clear_cnodev();
                    usedRegions.push_back( region ); // add region to the list of used regions in order of occurrence
                }
                region->add_cnode( cnode );
            }
        }
        else
        {
            Cnode*        cnode  = static_cast<Cnode*> ( item->getCubeObject() );
            cube::Region* region = cnode->get_callee();
            if ( !regionSet.contains( region ) ) // found new region entry -> clear cnodes and fill afterwards
            {
                regionSet.insert( region );
                region->clear_cnodev();
                usedRegions.push_back( region ); // add region to the list of used regions in order of occurrence
            }
            region->add_cnode( cnode );
        }
    }

    // create invisible top level item
    TreeItem* top = new TreeItem( this, QString(), REGIONITEM, 0 );
    createItems<cube::Region>( top, usedRegions, REGIONITEM );

    // create set of callers
    const vector<cube::Cnode*>& cnodeVector = cube->getCnodes();
    QSet<cube::Vertex*>         callerItems;
    for ( Cnode* cnode : cnodeVector )
    {
        if ( cnode )
        {
            cube::Vertex* caller = cnode->get_caller();
            callerItems.insert( caller );
        }
    }

    // create a subroutine subitem, if the item has callees
    foreach( TreeItem * item, top->getChildren() )
    {
        cube::Vertex* region = item->getCubeObject();
        if ( callerItems.contains( region ) )
        {
            TreeItem* child = new TreeItem( this, "Subroutines", REGIONITEM, 0 );
            item->addChild( child );
            subroutines.append( child );
        }
    }

    // set activeNodes: list of cnodes that correspond to CubeProxy::ALL_CNODES even if the call tree has been cut
    activeNodes.clear();
    for ( TreeItem* item :  top->getChildren() )
    {
        cube::Region* region = static_cast<cube::Region*> ( item->getCubeObject() );
        for ( cube::Cnode* cnode : region->get_cnodev() )
        {
            cnode_pair pair;
            pair.first  = cnode;
            pair.second = item->isExpanded() ? CUBE_CALCULATE_EXCLUSIVE :  CUBE_CALCULATE_INCLUSIVE;
            activeNodes.push_back( pair );
        }
    }

    if ( taskTree ) // OMP special case: get nodes required to calculate metric values
    {
        // get flat tree item that correspond to top level call tree items
        QList<TreeItem*> topItems;
        for ( TreeItem* item : top->getChildren() )
        {
            for ( TreeItem* callItem : callTree->getRootItem()->getChildren() )
            {
                cube::Region* region1 = static_cast<cube::Region*> ( item->getCubeObject() );
                Cnode*        cnode   = static_cast<Cnode*> ( callItem->getCubeObject() );
                if ( !cnode )
                {
                    break;
                }
                cube::Region* region2 = cnode->get_callee();
                if ( region1 == region2 )
                {
                    topItems.append( item );
                }
            }
        }

        // invalid regions -> invalid flat tree items
        for ( TreeItem* item : top->getChildren() )
        {
            cube::Region* region = static_cast<cube::Region*> ( item->getCubeObject() );
            if ( invalidRegions.contains( region ) )
            {
                invalidParents.insert( item );
            }
        }

        isExclusiveMetric     = true;
        flatTreeExclRootNodes = getNodes( topItems );
        isExclusiveMetric     = false;
    } // taskTree

    return top;
}


list_of_cnodes
FlatTree::getAllNodesMetricExclusive() const
{
    return flatTreeExclRootNodes;
}


list_of_cnodes
FlatTree::getNodes() const
{
    return callTree->getNodes();
}

list_of_cnodes
FlatTree::getNodes( const QList<TreeItem*> items ) const
{
    list_of_cnodes list;

    for ( TreeItem* item : items )
    {
        if ( item->getCubeObject() != nullptr ) // top level items
        {
            cube::Region* region = static_cast<cube::Region*> ( item->getCubeObject() );
            for ( cube::Cnode* cnode : region->get_cnodev() )
            {
                cnode_pair pair;
                pair.first  = cnode;
                pair.second = item->isExpanded() ? CUBE_CALCULATE_EXCLUSIVE :  CUBE_CALCULATE_INCLUSIVE;
                list.push_back( pair );
            }
            if ( taskTree && isExclusiveMetric ) // OMP special case
            {
                // special case: add task tree nodes to inclusive top level entries that are parent of the tasks
                if ( !item->isExpanded() && tasksToAdd.contains( region ) )
                {
                    const list_of_cnodes& tasks = tasksToAdd[ region ];
                    list.insert( std::end( list ), std::begin( tasks ), std::end( tasks ) );
                }  // end task tree specific
            }
        }
        else // subroutines
        {
            cube::Region* region = static_cast<cube::Region*> ( item->getParent()->getCubeObject() );
            for ( cube::Cnode* cnode : region->get_cnodev() )       // get children of parent region
            {
                for ( cube::Vertex* child : cnode->get_children() ) // add inclusive child values
                {
                    cnode_pair pair;
                    pair.first  = static_cast<cube::Cnode*> ( child );
                    pair.second = CUBE_CALCULATE_INCLUSIVE;
                    list.push_back( pair );
                }
            }
            if ( taskTree && isExclusiveMetric )     // OMP special case
            {
                if ( tasksToAdd.contains( region ) ) // add task tree nodes to subroutines of items that are parent of the tasks
                {
                    const list_of_cnodes& tasks = tasksToAdd[ region ];
                    list.insert( std::end( list ), std::begin( tasks ), std::end( tasks ) );
                }
            }
        }
    }
    return list;
}


list_of_cnodes
FlatTree::getSelectedNodes() const
{
    return getNodes( selectionList );
}

bool
FlatTree::isValidSelection() const
{
    for ( TreeItem* item : selectionList )
    {
        if ( isExclusiveMetric && invalidParents.contains( item ) )
        {
            return false;
        }
    }
    return true;
}

void
FlatTree::saveMetricValues( Tree* metric )
{
    if ( !taskTree || !isExclusiveMetric )
    {
        callTree->saveMetricValues( metric ); // use metric values of the default call tree
    }
    else
    {
        CallTree::saveMetricValues( metric );
    }
}

void
FlatTree::restoreMetricValues( Tree* metric )
{
    if ( !taskTree || !isExclusiveMetric )
    {
        callTree->restoreMetricValues( metric ); // use metric values of the default call tree
    }
    else
    {
        CallTree::restoreMetricValues( metric );
    }
}


// metricChanged is required for OMP special case: task tree items have been added and exclusive metric is selected
void
FlatTree::metricSelected( const QList<TreeItem*>& metrics )
{
    if ( taskTree == nullptr )
    {
        return;
    }
    isExclusiveMetric = false;

    for ( const TreeItem* item : metrics )
    {
        cube::Metric* metric = static_cast<cube::Metric* > ( item->getCubeObject() );
        if ( metric->get_type_of_metric() == CUBE_METRIC_EXCLUSIVE || metric->get_type_of_metric() == CUBE_METRIC_PREDERIVED_EXCLUSIVE )
        {
            isExclusiveMetric = true;
        }
    }
}

QList<Task*>
FlatTree::setBasicValues( const QList<Tree*>&     leftTrees,
                          const QList<Tree*>&     rightTrees,
                          const QList<TreeItem*>& itemsToCalculate = QList<TreeItem*>( ) )
{
    QList<Task*> workerData;
    /* No update of tree is necessary if the flat tree is to the left of the metric tree */
    if ( ( leftTrees.size() == 0 ) || ( ( leftTrees.size() == 1 ) && ( leftTrees[ 0 ]->getType() != METRIC ) ) )
    {
        return workerData;
    }

    if ( itemsToCalculate.size() == 0 ) // all visible items are already up to date
    {
        return workerData;
    }

    QSet<DisplayType>         isLeft;
    QHash<DisplayType, Tree*> trees;
    for ( Tree* tree : leftTrees )
    {
        isLeft.insert( tree->getType() );
        trees.insert( tree->getType(), tree );
    }
    for ( Tree* tree : rightTrees )
    {
        trees.insert( tree->getType(), tree );
    }

    // Metric tree is definitely in the left trees, as the initial if-statement at the
    // beginning of the call already filtered out the other cases.
    // Set up selection for metric tree
    const QList<TreeItem*>& selected_metrics = trees[ METRIC ]->getSelectionList();
    list_of_metrics         metric_selection;

    foreach( TreeItem * item, selected_metrics )
    {
        metric_pair mp;
        mp.first  = static_cast<cube::Metric*> ( item->getCubeObject() );
        mp.second = ( item->isExpanded() ) ? CUBE_CALCULATE_EXCLUSIVE : CUBE_CALCULATE_INCLUSIVE;
        if ( item->getChildren().size() == 0 )
        {
            mp.second = CUBE_CALCULATE_INCLUSIVE;
        }
        metric_selection.push_back( mp );
    }

    // Set up selection for system tree, leave empty if system tree is right to call tree
    list_of_sysresources sysres_selection;
    if ( isLeft.contains( SYSTEM ) )  // call tree is on the right
    {
        QList< TreeItem* > selected_systemnodes = trees[ SYSTEM ]->getSelectionList();
        foreach( TreeItem * item, selected_systemnodes )
        {
            sysres_pair sp;
            sp.first  = static_cast<cube::Sysres*> ( item->getCubeObject() );
            sp.second = ( item->isExpanded() ) ? cube::CUBE_CALCULATE_EXCLUSIVE : cube::CUBE_CALCULATE_INCLUSIVE;
            sysres_selection.push_back( sp );
        }
    }
    else
    {
        sysres_selection = CubeProxy::ALL_SYSTEMNODES;
    }

    QSet<TreeItem*> invalidItems;
    if ( taskTree && isExclusiveMetric ) // OMP special case: invalidate parents of tasks that cannot be calculated
    {
        for ( TreeItem* item : itemsToCalculate )
        {
            if ( isExclusiveMetric && invalidParents.contains( item ) )
            {
                TreeItem* invItem = item->isExpanded() ? item->getChildren().first() : item;
                invItem->invalidate();
                invalidItems.insert( invItem );
            }
        }
    }

    // calculate values for all invalidated tree items
    for ( TreeItem* item : itemsToCalculate )
    {
        if ( !invalidItems.contains( item ) )
        {
            const list_of_cnodes& cnodes = getNodes( QList<TreeItem*>() << item );

            TreeTask* data = new TreeTask( item,
                                           metric_selection,
                                           cnodes,
                                           sysres_selection );
            workerData.append( data );
        }
    }

    return workerData;
}

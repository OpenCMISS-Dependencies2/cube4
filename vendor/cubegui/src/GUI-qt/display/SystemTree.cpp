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

#include "SystemTree.h"
#include "CallTree.h"
#include "FlatTree.h"
#include "TreeView.h"
#include "MetricTree.h"
#include "CubeMetric.h"
#include "CubeProxy.h"
#include "CubeIdIndexMap.h"
#include <QDebug>

using namespace cube;
using namespace std;
using namespace cubegui;

SystemTree::SystemTree( CubeProxy* cubeProxy ) : Tree( cubeProxy )
{
    treeType    = SYSTEMTREE;
    displayType = SYSTEM;
    label       = tr( "System tree" );
}

list_of_sysresources
SystemTree::getSelectedNodes()
{
    QList< TreeItem* >   selected_systemnodes = selectionList;
    list_of_sysresources sysres_selection;
    foreach( TreeItem * item, selected_systemnodes )
    {
        sysres_pair sp;
        sp.first  = static_cast<cube::Sysres*> ( item->getCubeObject() );
        sp.second = ( item->isExpanded() ) ? cube::CUBE_CALCULATE_EXCLUSIVE : cube::CUBE_CALCULATE_INCLUSIVE;
        sysres_selection.push_back( sp );
    }
    return sysres_selection;
}

/**
 * creates a system tree
 * The first level of the system tree consists of items of type SYSTEMTREENODEITEM. The children
 * of this level can be also SYSTEMTREENODEITEM.
 * The last two levels of the tree are items of type LOCATIONGROUPITEM which have children of
 * type LOCATION.
 */
TreeItem*
SystemTree::createTree()
{
    TreeItem*                  top = new TreeItem( this, QString(), SYSTEMTREENODEITEM, 0 );
    std::vector<cube::Vertex*> vec;

    // insert root systen tree nodes and its children recursively
    createItems( top, cube->getRootSystemTreeNodes(), SYSTEMTREENODEITEM );

    // insert LOCATIONGROUP items but not its children of type LOCATION
    QList<TreeItem*> groups;
    createItems( top, cube->getLocationGroups(), LOCATIONGROUPITEM, &groups, false );

    // insert LOCATION items
    createItems( top, cube->getLocations(), LOCATIONITEM );

    // for single-threaded system trees : hide the threads
    bool singleThreaded = true;
    foreach( TreeItem * group, groups ) // for all items of type LOCATIONGROUPITEM
    {
        if ( group->getChildren().size() > 1 )
        {
            singleThreaded = false;
            break;
        }
    }
    if ( singleThreaded )
    {
        foreach( TreeItem * group, groups )
        {
            if ( group->getChildren().size() == 1 )
            {
                TreeItem* child = group->child( 0 );
                group->removeChild( child );
                treeItemHash.remove( child->getCubeObject() );
                delete child;
            }
        }
    }
    // end handling of single-threaded trees

    return top;
}

QString
SystemTree::getItemName( cube::Vertex* vertex ) const
{
    return QString( ( ( cube::Sysres* )vertex )->get_name().c_str() );
}

void
SystemTree::computeMinMax()
{
    double         tmp = 0.0;
    vector<double> min_vector;
    vector<double> max_vector;
    vector<bool>   depth_seen;
    foreach( TreeItem * item, getItems() )
    {
        size_t depth = item->getDepth();

        if ( min_vector.size() <= depth )
        {
            min_vector.resize( depth + 1 );
        }
        if ( max_vector.size() <= depth )
        {
            max_vector.resize( depth + 1 );
        }
        while ( depth_seen.size() <= depth )
        {
            depth_seen.push_back( false );
        }

        tmp = item->totalValue;

        if ( !depth_seen[ depth ] )
        {
            depth_seen[ depth ] = true;
            min_vector[ depth ] = tmp;
            max_vector[ depth ] = tmp;
        }
        else
        {
            min_vector[ depth ] = ( min_vector[ depth ] > tmp ) ? tmp : min_vector[ depth ];
            max_vector[ depth ] = ( max_vector[ depth ] < tmp ) ? tmp : max_vector[ depth ];
        }
    }

    foreach( TreeItem * item, getItems() )
    {
        int depth = item->getDepth();
        item->minValue = min_vector[ depth ];
        item->maxValue = max_vector[ depth ];
    }
}


void
SystemTree::assignValues( TreeItem*               item,
                          const vector< Value* >& inclusive_values,
                          const vector< Value* >& exclusive_values )
{
    assert( item );
    assert( item->getCubeObject() );

    SystemTreeNode* stn = static_cast<SystemTreeNode*>( item->getCubeObject() );
    uint32_t        id  = stn->get_sys_id();
    // value vector is empty, if selected left tree item is zero -> set to NULL
    item->setValues( ( ( id >= inclusive_values.size() ) ? NULL : inclusive_values[ id ] ),
                     ( ( id >= exclusive_values.size() ) ? NULL : exclusive_values[ id ] ) );

    foreach( TreeItem * child, item->getChildren() )
    {
        assignValues( child, inclusive_values, exclusive_values );
    }
}

/**
   returns true, if all items have to be recalculated and calculation isn't disabled
 */
bool
SystemTree::calculationRequired( const QList<TreeItem*>& itemsToCalculateNow )
{
    if ( !top || top->getChildren().size() == 0 )
    {
        return false;
    }
    if ( itemsToCalculateNow.size() > 0 )
    {
        return true;
    }
    TreeItem* visibleTop = top->getChildren().first();
    if ( visibleTop->isCalculationEnabled() && !visibleTop->isCalculated() )
    {
        return true;
    }
    return false;
}

QList<Task*>
SystemTree::setBasicValues( const QList<Tree*>&     leftTrees,
                            const QList<Tree*>&     rightTrees,
                            const QList<TreeItem*>& itemsToCalculate )
{
    QList<Task*> workerData;
    /* No update of tree is necessary if the system tree is to the left of the metric tree */
    if ( ( leftTrees.size() == 0 ) || ( ( leftTrees.size() == 1 ) && ( leftTrees[ 0 ]->getType() != METRIC ) ) )
    {
        return workerData;                   // the system tree is left of the metric tree.
    }

    if ( !calculationRequired( itemsToCalculate ) )
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

    // Set up selection for metric tree
    list_of_metrics metric_selection;
    MetricTree*     metricTree = static_cast<MetricTree*>( trees[ METRIC ] );
    metric_selection = metricTree->getSelectedMetrics();

    // Set up selection for the active call tree
    CallTree*             callTree        = static_cast<CallTree*> ( trees[ CALL ] );
    const list_of_cnodes& cnode_selection = isLeft.contains( CALL ) ? callTree->getSelectedNodes() : callTree->getNodes();

    if ( isLeft.contains( CALL ) && !callTree->isValidSelection() )
    {
        invalidateItems();
        QString msg = tr( "Cannot calculate values for selected flat tree item because the item contains an undetermined part the task tree" );
        Globals::setStatusMessage( msg, Warning );
    }
    else
    {
        SystemTreeData* data = new SystemTreeData( this,
                                                   metric_selection,
                                                   cnode_selection );
        workerData.append( data );
    }

    computeMinMax();
    return workerData;
}
// end of computeBasicValues()

/**
 * @brief getVisitedItems
 * @return all threads with visited > 0
 */
QList<TreeItem*>
SystemTree::getVisitedItems() const
{
    QList<TreeItem*> visited;

    Tree* metricTree = getActiveTree( METRIC );
    Tree* callTree   = getActiveTree( CALL );

    TreeItem* visitedMetric = 0;
    foreach( TreeItem * item, metricTree->getItems() )
    {
        if ( item->getName() == "Visits" )
        {
            visitedMetric = item;
            break;
        }
    }

    // build up metric selection
    list_of_metrics metric_selection;
    metric_selection.push_back( make_pair( static_cast<Metric*>( visitedMetric->getCubeObject() ), CUBE_CALCULATE_INCLUSIVE ) );

    // build up cnode selection
    list_of_cnodes cnode_selection;
    foreach( TreeItem * callItem, callTree->getSelectionList() )
    {
        cube::CalculationFlavour callTreeState;
        if ( !callItem->isExpanded() )
        {
            callTreeState = cube::CUBE_CALCULATE_INCLUSIVE;
        }
        else
        {
            callTreeState = CUBE_CALCULATE_EXCLUSIVE;
        }
        cnode_selection.push_back( make_pair( static_cast<Cnode*>( callItem->getCubeObject() ), callTreeState ) );
    }

    vector<Value*> inclusive_values;
    vector<Value*> exclusive_values;

    // Get values for full tree. Inclusive values are needed as single threaded measurements
    // do not generate tree items for thread level and process level has 0 exclusive value.
    cube->getSystemTreeValues( metric_selection,
                               cnode_selection,
                               inclusive_values,
                               exclusive_values );

    foreach( TreeItem * item, getItems() )
    {
        if ( ( ( item->type == LOCATIONGROUPITEM && item->getChildren().size() == 0 )
               || ( item->type == LOCATIONITEM ) )
             && ( inclusive_values[ static_cast<Sysres*>( item->getCubeObject() )->get_sys_id() ]->getSignedLong() > 0 ) )
        {
            visited.append( item );
        }
    }

    return visited;
}


TreeItem*
SystemTree::getTreeItem( uint32_t systemId ) const
{
    foreach( TreeItem * item, treeItems )
    {
        cube::Sysres* sysres = static_cast<cube::Sysres*> ( item->getCubeObject() );
        if ( sysres->get_sys_id() == systemId )
        {
            return item;
        }
    }
    return 0;
}

void
SystemTreeData::calculate()
{
    // request bulk data from CubeProxy
    vector< Value* > inclusive_values;
    vector< Value* > exclusive_values;

    // get values for full tree
    cube::CubeProxy* cube = tree->getCube();
    cube->getSystemTreeValues( metric_selection,
                               cnode_selection,
                               inclusive_values,
                               exclusive_values );

    // Assign values to each toplevel tree
    foreach( TreeItem * child, tree->getRootItem()->getChildren() )
    {
        tree->assignValues( child, inclusive_values, exclusive_values );
    }
    tree->computeMinMax();
}
void
SystemTreeFlatData::calculate()
{
    // request bulk data from CubeProxy
    vector< Value* > inclusive_values;
    vector< Value* > exclusive_values;

    // get values for full tree
    cube::CubeProxy* cube = tree->getCube();
    cube->getSystemTreeValues( metric_selection,
                               region_selection,
                               inclusive_values,
                               exclusive_values );

    // Assign values to each toplevel tree
    foreach( TreeItem * child, tree->getRootItem()->getChildren() )
    {
        tree->assignValues( child, inclusive_values, exclusive_values );
    }
    tree->computeMinMax();
}

void
SystemTree::assignValues( TreeItem*               item,
                          const vector< Value* >& inclusive_values,
                          const vector< Value* >& exclusive_values,
                          int                     depth,
                          cube::IdIndexMap*       indexMap
                          )
{
    assert( item );

    if ( item->getDepth() > depth )
    {
        return;
    }

    if ( item->getCubeObject() )
    {
        size_t id = item->getCubeObject()->get_id();
        if ( indexMap )
        {
            id = indexMap->getIndex( id );
        }

        if ( inclusive_values.size() > 0 )
        {
            item->setInclusiveValue( ( id >= inclusive_values.size() ) ? NULL : inclusive_values[ id ] );
        }
        if ( exclusive_values.size() > 0 )
        {
            item->setExclusiveValue( ( id >= exclusive_values.size() ) ? NULL : exclusive_values[ id ] );
        }

        foreach( TreeItem * child, item->getChildren() )
        {
            assignValues( child, inclusive_values, exclusive_values, depth, indexMap );
        }
    }
}

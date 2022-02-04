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


#include <cmath>
#include <QDebug>

#include "AggregatedTreeItem.h"
#include "CubeMetric.h"
#include "CubeProxy.h"
#include "CubeRegion.h"
#include "CubeIdIndexMap.h"

#include "MetricTree.h"
#include "Globals.h"
#include "TreeView.h"
#include "AggregatedTreeItem.h"
#include "CallTree.h"
#include "FlatTree.h"
#include "SystemTree.h"

using namespace std;
using namespace cube;
using namespace cubegui;

MetricTree::MetricTree( CubeProxy* cubeProxy ) : Tree( cubeProxy )
{
    displayType = METRIC;
    label       = tr( "Metric tree" );
}

list_of_metrics
MetricTree::getSelectedMetrics() const
{
    return getMetrics( selectionList );
}

list_of_metrics
MetricTree::getMetrics( const QList<TreeItem*> items ) const
{
    list_of_metrics list;

    foreach( TreeItem * item, items )
    {
        metric_pair mp;
        mp.first  = static_cast<cube::Metric*> ( item->getCubeObject() );
        mp.second = ( item->isExpanded() ) ? CUBE_CALCULATE_EXCLUSIVE : CUBE_CALCULATE_INCLUSIVE;
        list.push_back( mp );
    }
    return list;
}

TreeItem*
MetricTree::createTree()
{
    TreeItem* top = new TreeItem( this, QString(), METRICITEM, 0 );

    vector<cube::Metric*> metrics = cube->getRootMetrics();

    if ( Globals::optionIsSet( ExpertMode ) ) // also show ghost metrics
    {
        vector<cube::Metric*> ghost = cube->getGhostMetrics();
        metrics.insert( metrics.end(), ghost.begin(), ghost.end() );
    }
    createItems<cube::Metric>( top, metrics, METRICITEM );

    return top;
}

/** return false, if the item is invalid and should not be added to the tree */
bool
MetricTree::itemIsValid( cube::Vertex* vertex )
{
    // if the metric item is VOID, it will not inserted into the tree
    cube::Metric* metric     = ( cube::Metric* )vertex;
    bool          voidValued = false;
    while ( true )
    {
        if ( std::strcmp( metric->get_val().c_str(), "VOID" ) == 0 )
        {
            voidValued = true;
            break;
        }
        metric = metric->get_parent();
        if ( metric == NULL )
        {
            break;
        }
    }
    return !voidValued;
}

QString
MetricTree::getItemName( cube::Vertex* vertex ) const
{
    return QString( ( ( cube::Metric* )vertex )->get_disp_name().c_str() );
}

double
MetricTree::getMaxValue( const TreeItem* item ) const
{
    return maxValueIndex.size() > 0  ? maxValues.at( maxValueIndex.value( item ) ) : std::nan( "" );
}

/**
 * @brief fills the hash derivedDependencies, which contains for each derived metric item a list of metric items on which it depends
 * see MetricTree::getDependencyHash()
 */
void
MetricTree::initializeFullDependencyHash()
{
    derivedDependencies.clear();
    foreach( TreeItem * item, unavailableItems )
    {
        delete item;
    }
    unavailableItems.clear();

    QList<TreeItem*>          derivedMetrics; // list with all derived metrics from metric tree
    QHash<Metric*, TreeItem*> metricHash;     // metric object -> metric tree item
    foreach( TreeItem * item, treeItems )
    {
        cube::Metric* metric = static_cast<cube::Metric*> ( item->getCubeObject() );
        metricHash.insert( metric, item );
        if ( item->isDerivedMetric() )
        {
            derivedMetrics.append( item );
        }
    }
    if ( derivedMetrics.isEmpty() )
    {
        return;
    }

    foreach( TreeItem * item, derivedMetrics )
    {
        std::vector<cube::Metric*> dependentMetrics;
        cube::Metric*              metric = static_cast<cube::Metric*> ( item->getCubeObject() );
        metric->fillReqMetrics( dependentMetrics );
        QList<TreeItem*> dependentItems;
        for ( Metric* metric : dependentMetrics )
        {
            if ( !metricHash.contains( metric ) ) // invisible ghost metric or inactive metric
            {
                TreeItem* item = new TreeItem( this, QString( metric->get_disp_name().c_str() ) + " " + metric->get_uniq_name().c_str(), METRICITEM, metric );
                metricHash.insert( metric, item );
                unavailableItems.append( item );
            }
            TreeItem* item = metricHash.value( metric );
            if ( !dependentItems.contains( item ) )
            {
                dependentItems.append( item );
            }
        }
        derivedDependencies.insert( item, dependentItems );
    }
}

/**
 * @brief MetricTree::getDependencyHash resolves dependencies of the derived metric items which should be calculated in parallel.
 *
 * Each derived metric depends on other metrics. If multiple metrics, that depend on the same (not yet calculated) metric, are calculated in parallel, the
 * execution time will increase because of locking.
 *
 * The dependencies of all derived metric items are stored in the hash derivedDependencies (seeMetricTree::initializeFullDependencyHash()).
 * The lists of tree items to calculate only contains the visible metrics. For that reason a metric item may have an unresolved dependency
 * to a metric that isn't visible. In that case, all dependencies of that metric item are removed and cubelib handles the dependencies.
 * If other tree items depend on the same unresolved dependency, this dependency is replaced by first tree item with this dependency.
 * @param derivedMetrics list of derived metrics, which will be calculated
 * @return hash [item -> list of dependent metric items]
 */
QHash< TreeItem*, QList<TreeItem*> >
MetricTree::getDependencyHash( const QList<TreeItem*>& metrics, const QList<TreeItem*>& derivedMetrics )
{
    QHash< TreeItem*, QList<TreeItem*> > hash;       // return value
    QSet<TreeItem*>                      allItemSet; // all derived metrics
    QSet<TreeItem*>                      derivedSet; // all derived metrics to calculate
    QSet<TreeItem*>                      metricSet;  // all non-derived metrics to calculate

    for ( TreeItem* item : derivedDependencies.keys() )
    {
        allItemSet.insert( item );
    }
    for ( TreeItem* item : derivedMetrics )
    {
        derivedSet.insert( item );
    }
    for ( TreeItem* item : metrics )
    {
        metricSet.insert( item );
    }

    QHash< TreeItem*, TreeItem* > unresolvedDeps;                                                              // metrics that are not part of the metric tree

    // could be called once after tree is created/modified, but cost of this method call is very low
    initializeFullDependencyHash();

    QList<TreeItem*> metricsToCalculate = derivedMetrics;
    while ( !metricsToCalculate.isEmpty() )
    {
        TreeItem*        item         = metricsToCalculate.takeLast();
        QList<TreeItem*> dependencies = derivedDependencies[ item ];
        foreach( TreeItem * dep, derivedDependencies[ item ] )
        {
            if ( dep->isCalculated() ) // dependency is already calculated -> remove it
            {
                dependencies.removeOne( dep );
                continue;
            }
            bool hasUnresolvedDeps = false;
            if ( !metricSet.contains( dep ) && !derivedSet.contains( dep ) )
            {
                // add dependent (currently unvisible) item to list of items to calculate
                if ( allItemSet.contains( dep ) )
                {
                    metricsToCalculate.append( dep );
                    derivedSet.insert( dep );
                }
                else // unresolveable dependency -> remove dependency of current item and add dependency to current item to all following occurences
                {
                    dependencies.removeOne( dep );
                    if ( !unresolvedDeps.contains( dep ) ) // first unresolvable dependency
                    {
                        unresolvedDeps.insert( dep, item );
                        hasUnresolvedDeps = true;
                    }
                    else // following unresolvable dependency -> refer to first
                    {
                        dependencies.append( unresolvedDeps.value( dep ) );
                    }
                }
            }
            if ( hasUnresolvedDeps )
            {
                dependencies.clear();  // let cubelib handle the dependencies
            }
        }
        hash.insert( item, dependencies );
    }

    return hash;
}

QList<Task*>
MetricTree::setBasicValues( const QList<Tree*>& leftTrees,
                            const QList<Tree*>& rightTrees, const QList<TreeItem*>& items )
{
    QList<Task*> workerData;

    if ( items.size() == 0 )
    {
        return workerData;
    }

    QList<TreeItem*> itemsToCalculate = items;
    QList<TreeItem*> derivedMetrics;
    foreach( TreeItem * child, itemsToCalculate )
    {
        if ( child->isSelected() ) // calculate selected items first
        {
            itemsToCalculate.removeOne( child );
            itemsToCalculate.prepend( child );
        }
        if ( child->isDerivedMetric() ) // derived metrics are computationally expensive -> calculate later
        {
            itemsToCalculate.removeOne( child );
            derivedMetrics.append( child );
        }
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

    // Set up selection for system tree, leave empty if metric tree is right to call tree
    list_of_sysresources sysres_selection;
    SystemTree*          systemTree = static_cast<SystemTree*>( trees[ SYSTEM ] );
    sysres_selection = isLeft.contains( SYSTEM ) ? systemTree->getSelectedNodes() : CubeProxy::ALL_SYSTEMNODES;

    // Set up selection for the active call tree
    CallTree*      callTree                = static_cast<CallTree*> ( trees[ CALL ] );
    list_of_cnodes cnode_selection_default = isLeft.contains( CALL ) ? callTree->getSelectedNodes() : callTree->getNodes();
    list_of_cnodes cnode_selection_tasks; // special case

    // OMP special case: exclusive metrics with a flattree, which includes tasks
    FlatTree* flatTree = dynamic_cast<FlatTree*>( trees[ CALL ] );
    if ( flatTree )
    {
        cnode_selection_tasks = flatTree->getAllNodesMetricExclusive();
    }

    // calculate simple metrics
    foreach( TreeItem * item, itemsToCalculate )
    {
        Metric*         metric          = dynamic_cast<Metric*> ( item->getCubeObject() );
        bool            specialCase     = flatTree && flatTree->hasTasks() && metric->get_type_of_metric() == cube::CUBE_METRIC_EXCLUSIVE;
        list_of_cnodes& cnode_selection = specialCase ? cnode_selection_tasks : cnode_selection_default;

        const list_of_metrics& metrics = getMetrics( QList<TreeItem*>() << item );

        TreeTask* data = new TreeTask( item,
                                       metrics,
                                       cnode_selection,
                                       sysres_selection );
        workerData.append( data );
    }

    // calculate derived metrics
    QHash< TreeItem*, QList<TreeItem*> > dependencies = getDependencyHash( itemsToCalculate, derivedMetrics );
    foreach( TreeItem * item, dependencies.keys() )
    {
        Metric*         metric          = dynamic_cast<Metric*> ( item->getCubeObject() );
        bool            specialCase     = flatTree && flatTree->hasTasks() && metric->get_type_of_metric() == cube::CUBE_METRIC_PREDERIVED_EXCLUSIVE;
        list_of_cnodes& cnode_selection = specialCase ? cnode_selection_tasks : cnode_selection_default;

        const list_of_metrics& metrics = getMetrics( QList<TreeItem*>() << item );

        TreeTask* data = new MetricTreeTask( item,
                                             metrics,
                                             cnode_selection,
                                             sysres_selection,
                                             dependencies.value( item ) );
        workerData.append( data );
    }
    return workerData;
}

/** returns true, if all dependent items have already been calculated */
bool
MetricTreeTask::isReady()
{
    foreach( TreeItem * dep, dependentItems )
    {
        if ( !dep->isCalculated() )
        {
            return false;
        }
    }
    return true;
}



// -----------------------------------------------------------------
// ---------------- calculation functions --------------------------
// -----------------------------------------------------------------

void
MetricTree::computeMaxValues()
{
    double roundThreshold = Globals::getRoundThreshold( FORMAT_TREES );
    double value;
    double maxValue = 100.0;

    maxValues.clear();
    maxValueIndex.clear();

    // for metric trees we compute the max values for each root's subtree
    QList<TreeItem*> subTree;

    for ( unsigned i = 0; i < ( unsigned )top->getChildren().size(); i++ )
    {
        subTree.clear();
        subTree.append( top->getChildren().at( i ) );
        maxValue = top->getChildren().at( i )->totalValue;

        for ( int j = 0; j < subTree.size(); j++ )
        {
            maxValueIndex.insert( subTree[ j ], i );
            foreach( TreeItem * child, subTree[ j ]->getChildren() )
            {
                subTree.append( child );
            }
            getMax( subTree[ j ], value );
            // we save max absolute value (for the case of differences of cube)
            if ( fabs( maxValue ) < fabs( value ) )
            {
                maxValue = value;
            }
        }

        if ( fabs( maxValue ) <= fabs( roundThreshold ) )
        {
            maxValue = 0.0;
        }
        maxValues.append( maxValue );
    }
} // end of computeMaxValues()


TreeItem*
MetricTree::getTreeItem( std::string metricId ) const
{
    foreach( TreeItem * item, this->getItems() )
    {
        cube::Metric* metric = static_cast<cube::Metric*>( item->cubeObject );
        if ( metric->get_uniq_name() == metricId )
        {
            return item;
        }
    }
    return 0;
}





// return the name if the ith external metric item
//
double
MetricTree::getExternalReferenceValue( const QString& name )
{
    return externalMetricValue.value( name, 0.0 );
}


// compute the needed external values from the "cubeExternal"
// database for the external percentage value modus
//
void
MetricTree::computeExternalReferenceValues( cube::CubeProxy* cubeExternal )
{
    externalMetricValue.clear();

    const std::vector<cube::Metric*>& metricVec = cubeExternal->getMetrics();
    const std::vector<cube::Cnode*>&  callVec   = cubeExternal->getCnodes();
    const std::vector<cube::Thread*>& threadVec = cubeExternal->getLocations();

    // calculate inclusive values for all metric root items
    for ( int i = 0; i < ( int )metricVec.size(); i++ )
    {
        // take the next metric item
        cube::Metric* metric     = metricVec[ i ];
        QString       metricName = QString::fromStdString( metric->get_uniq_name() );

        // compute external metric values
        double totalValue = 0.0;
        for ( int j = 0; j < ( int )callVec.size(); j++ )
        {
            for ( int k = 0; k < ( int )threadVec.size(); k++ )
            {
                /// @fixme @todo Implement proxy computation for external reference values
                //totalValue += cube->get_sev( metric, callVec[ j ], threadVec[ k ] );
            }
        }
        externalMetricValue[ metricName ] = totalValue;
    }
}

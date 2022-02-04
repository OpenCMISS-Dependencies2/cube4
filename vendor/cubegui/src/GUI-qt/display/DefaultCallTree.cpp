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
#include "DefaultCallTree.h"
#include "CubeRegion.h"
#include "MetricTree.h"
#include "SystemTree.h"
#include "TreeView.h"
#include "TreeItem.h"
#include "CubeMetric.h"
#include "AggregatedTreeItem.h"
#include "TreeModel.h"
#include "CubeProxy.h"
#include "CubeIdIndexMap.h"
#include "TabManager.h"

#include <QLayout>
#include <QPropertyAnimation>

using namespace std;
using namespace cube;
using namespace cubegui;

DefaultCallTree::DefaultCallTree( CubeProxy* cubeProxy, const std::vector< cube::Cnode* >& cnodes ) : CallTree( cubeProxy ), topCnodes( cnodes )
{
    treeType             = DEFAULTCALLTREE;
    iterationsAreHidden_ = false;
    loopRoot             = 0;
    aggregatedLoopRoot   = 0;
    initialNodes         = CubeProxy::ALL_CNODES;
    label                = ( cube->getAttribute( "calltree title" ) != "" ) ? cube->getAttribute( "calltree title" ).c_str() : tr( "Call tree" );
}

DefaultCallTree::~DefaultCallTree()
{
    unsetLoop(); // delete merged iterations
}

list_of_cnodes
DefaultCallTree::getNodes() const
{
    // empty list => CubeProxy::ALL_CNODES
    return this->activeNodes;
}

list_of_cnodes
DefaultCallTree::getSelectedNodes() const
{
    return getNodes( selectionList );
}

list_of_cnodes
DefaultCallTree::getNodes( const QList<TreeItem*> items ) const
{
    list_of_cnodes list;
    cube::Cnode*   cnode = 0;

    for ( TreeItem* item : items )
    {
        if ( item->isAggregatedRootItem() && item->isExpanded() ) // special case aggregated loops: root item of the loop
        {
            cnode = static_cast<cube::Cnode*> ( item->getCubeObject() );

            // If iterations are merged, the tree level with the iteration items is removed. The iterations items
            // may have exclusive values, which have to be added to the exclusive value of its parent.
            for ( TreeItem* child : loopRoot->getChildren() )
            {
                cnode_pair pair;
                pair.first  = static_cast<cube::Cnode*> ( child->getCubeObject() );
                pair.second = cube::CUBE_CALCULATE_EXCLUSIVE;
                list.push_back( pair );
            }
        }
        else if ( item->isAggregatedLoopItem() ) // add all loop entries
        {
            const QList<cube::Cnode*>& iterations = static_cast<AggregatedTreeItem*>( item )->getIterations();
            for ( cube::Cnode* cnode : iterations )
            {
                if ( cnode )
                {
                    cnode_pair pair;
                    pair.first  = cnode;
                    pair.second =  ( item->isExpanded() ) ? cube::CUBE_CALCULATE_EXCLUSIVE : cube::CUBE_CALCULATE_INCLUSIVE;
                    list.push_back( pair );
                }
            }
        }
        else // normal item (which may have pruned children)
        {
            cnode = static_cast<cube::Cnode*> ( item->getCubeObject() );

            // add inclusive value of all pruned children to exclusive value of parent
            const QList<TreeItem*>& pruned = prunedChildren.value( item );
            if ( item->isExpanded() && pruned.size() > 0 )
            {
                // add inclusive values of all pruned children to parent item
                for ( TreeItem* item : pruned )
                {
                    cnode_pair pair;
                    pair.first  = static_cast<cube::Cnode*> ( item->getCubeObject() );
                    pair.second = CUBE_CALCULATE_INCLUSIVE;
                    list.push_back( pair );
                }
            }
        }

        if ( cnode )
        {
            cnode_pair pair;
            pair.first  = cnode;
            pair.second = item->isExpanded() ? CUBE_CALCULATE_EXCLUSIVE :  CUBE_CALCULATE_INCLUSIVE;
            list.push_back( pair );
        }
    }
    return list;
}

QString
DefaultCallTree::getLabel() const
{
    return label;
}

void
DefaultCallTree::initTree()
{
    userRoots.clear();
    activeNodes = initialNodes;
    metricValueHash.clear();
    prunedChildren.clear();
    TreeItem* loop = getDefinedLoopRoot();
    if ( loop )
    {
        setAsLoop( loop, false );
    }
}

TreeItem*
DefaultCallTree::createTree()
{
    top = new TreeItem( this, QString( "Invisible call tree root" ), CALLITEM, 0 );
    createItems<cube::Cnode>( top, topCnodes, CALLITEM );
    initTree();

    return top;
}

/** checks if an item is defined as a loop ("CLUSTER ROOT CNODE ID" is set) */
TreeItem*
DefaultCallTree::getDefinedLoopRoot()
{
    TreeItem* loop = nullptr;
    QString   nrs  = QString( cube->getAttribute( "CLUSTER ROOT CNODE ID" ).c_str() );
    if ( nrs.length() > 0 )
    {
        uint             cnodeID = nrs.toUInt();
        QList<TreeItem*> list;
        list.append( top->getChildren() );
        while ( !list.isEmpty() )
        {
            TreeItem* it = list.takeFirst();
            if ( it->getCubeObject() )
            {
                uint itemCnode = it->getCubeObject()->get_id();
                if ( itemCnode == cnodeID )
                {
                    loop = it;
                    break;
                }
            }
            list.append( it->getChildren() );
        }
    }
    return loop;
}

QString
DefaultCallTree::getItemName( cube::Vertex* vertex ) const
{
    std::string name = ( static_cast<cube::Cnode* > ( vertex ) )->get_callee()->get_name();

    std::vector<std::pair<std::string, double> > num_params = ( static_cast<cube::Cnode* > ( vertex ) )->get_num_parameters();
    std::string                                  sep        = " (";
    std::string                                  end        = "";
    for ( unsigned i = 0; i < num_params.size(); i++ )
    {
        char buffer[ 100 ];
        sprintf( buffer, "%g", num_params[ i ].second );
        name += sep + num_params[ i ].first + "=" + std::string( buffer );
        sep   = ", ";
        end   = ")";
    }
    std::vector<std::pair<std::string, std::string> > str_params = ( static_cast<cube::Cnode* > ( vertex ) )->get_str_parameters();
    for ( unsigned i = 0; i < str_params.size(); i++ )
    {
        name += sep + str_params[ i ].first + "=" + str_params[ i ].second;
        sep   = ", ";
        end   = ")";
    }
    name += end;

    return QString( name.c_str() );
}


// -----------------------------------------------------------------
// ---------------- iteration handling --------------------------
// -----------------------------------------------------------------



/** The given item is set as a loop. This allows to hide its iterations.
 * @param item the top loop item
 * @param reinit it true, the values of the loop root and visible children and all tabs right to this tree are recalculated
 */
void
DefaultCallTree::setAsLoop( TreeItem* item, bool reinit )
{
    assert( item );
    if ( item == aggregatedLoopRoot || item == loopRoot || item->children.size() == 0 )
    {
        return;
    }

    unsetLoop(); // unset previously set loop
    loopRoot           = item;
    aggregatedLoopRoot = mergeIterations( loopRoot );

    int iterations = aggregatedLoopRoot->iterations.size();
    aggregatedLoopRoot->name.append( " [" + QString::number( iterations ) + tr( " iterations" ) + "]" );

    loopRoot->displayName.clear();
    loopRoot->invalidateLabel(); // update tree item label of new loop root
    if ( reinit )
    {
        Globals::getTabManager()->reinit( loopRoot );
    }
}

void
DefaultCallTree::unsetLoop()
{
    if ( loopRoot != 0 )
    {
        if ( iterationsAreHidden_ )
        {
            showIterations();
        }

        deleteMergedIterations( aggregatedLoopRoot );               // recursively delete all merged items
        treeItemHash.insert( loopRoot->getCubeObject(), loopRoot ); // same key as aggregatedLoopRoot

        TreeItem* oldLoop = loopRoot;
        aggregatedLoopRoot = 0;
        loopRoot           = 0;

        oldLoop->displayName.clear();
        oldLoop->invalidateLabel(); // update tree item label after loopRoot has been changed
    }
}


/**
   Hides the iteration of the currenly defined loop.
 */
void
DefaultCallTree::hideIterations()
{
    if ( !loopRoot  || iterationsAreHidden_ )
    {
        return;
    }
    iterationsAreHidden_ = true;

    // replace loop structure with aggregated structure
    treeModel->replaceSubtree( loopRoot, aggregatedLoopRoot );

    aggregatedLoopRoot->select();
    aggregatedLoopRoot->setExpanded( true );

    aggregatedLoopRoot->displayName.clear();
    aggregatedLoopRoot->invalidateLabel();
}

void
DefaultCallTree::showIterations()
{
    if ( !loopRoot || !iterationsAreHidden_ )
    {
        return;
    }
    iterationsAreHidden_ = false;

    // replace aggregated loop structure with iterations
    treeModel->replaceSubtree( aggregatedLoopRoot, loopRoot );

    loopRoot->select();
    loopRoot->setExpanded( true );

    loopRoot->displayName.clear();
    loopRoot->invalidateLabel();
}

bool
DefaultCallTree::iterationsAreHidden() const
{
    return iterationsAreHidden_;
}

TreeItem*
DefaultCallTree::getLoopRootItem() const
{
    return loopRoot;
}

AggregatedTreeItem*
DefaultCallTree::getAggregatedRootItem() const
{
    return aggregatedLoopRoot;
}

/**
   Treats the current item as root of a loop and merges its children. References from the merged iterations
   to the single iterations and vice versa are inserted.
 */
AggregatedTreeItem*
DefaultCallTree::mergeIterations( TreeItem* loopRoot )
{
    AggregatedTreeItem* aggregatedRoot = new AggregatedTreeItem( this, loopRoot->getDepth(), loopRoot );
    aggregatedRoot->cubeObject = loopRoot->cubeObject;
    aggregatedRoot->parentItem = loopRoot->parentItem;

    foreach( TreeItem * iteration, loopRoot->getChildren() )
    {
        aggregatedRoot->iterations.append( static_cast<Cnode*> ( iteration->getCubeObject() ) );
    }

    mergeIterations( aggregatedRoot, loopRoot->getChildren() );

    return aggregatedRoot;
}


/**
 * @brief mergeIterations recursively merges iteration items with the same name into
 * new items and appends them to newParent.
 * Only items of the same tree depth which have the same name are merged.
 */
void
DefaultCallTree::mergeIterations( TreeItem* newParent, const QList<TreeItem*>& iterations )
{
    QSet<QString>                       allChildren; // all distinct children names of the current tree level
    QHash<QString, AggregatedTreeItem*> hash;        // child name -> aggregated tree item

    // find all children of iterations, eleminate those with same name and append a copy to newparent
    foreach( TreeItem * iteration, iterations )
    {
        if ( iteration )
        {
            foreach( TreeItem * child, iteration->getChildren() )
            {
                allChildren.insert( child->name );
                if ( !hash.contains( child->name ) ) // create pseudo-item which contains iterations
                {
                    AggregatedTreeItem* newChild = new AggregatedTreeItem( this, newParent->getDepth() + 1, child );
                    newParent->addChild( newChild );
                    hash.insert( child->name, newChild );
                }
                //AggregatedTreeItem* aggregated = hash.value( child->name );
                //aggregated->iterations.append( static_cast<cube::Cnode*> ( child->cubeObject ) );
            }
        }
    }

    // required if functions are not called in all iterations -> append null cnode for non existing iterations
    // (optimize: inner loop not necessary if function is called in all iterations)
    foreach( TreeItem * iteration, iterations )
    {
        foreach( QString childName, allChildren.values() )
        {
            AggregatedTreeItem* aggregated = hash.value( childName );
            cube::Cnode*        cnode      = 0;
            if ( iteration )
            {
                foreach( TreeItem * currentIterChild, iteration->getChildren() ) // seach child in current iteration
                {
                    if ( childName == currentIterChild->name )
                    {
                        cnode = static_cast<cube::Cnode*> ( currentIterChild->cubeObject );
                        break;
                    }
                }
                if ( cnode )
                {
                    aggregated->iterations.append( cnode );
                }
            }
        }
    }

    // if newly added children of newParent have children, call this method recursively
    foreach( TreeItem * item, newParent->getChildren() )
    {
        QString childName = item->name;

        int              childCount = 0;
        QList<TreeItem*> listOfChildren;
        foreach( TreeItem * iteration, iterations )
        {
            TreeItem* nextChild = 0;
            if ( iteration )
            {
                foreach( TreeItem * child, iteration->getChildren() )
                {
                    if ( child->name == childName )
                    {
                        nextChild = child;
                        childCount++;
                        break;
                    }
                }
            }
            listOfChildren.push_back( nextChild );
        }

        if ( childCount > 0 )
        {
            mergeIterations( item, listOfChildren );
        }
    }
}

/**
   delete the loop root of merged iterations and all its children recursively
 */
void
DefaultCallTree::deleteMergedIterations( TreeItem* aggregated )
{
    foreach( TreeItem * item, aggregated->getChildren() )              // for all aggregated items
    {
        deleteMergedIterations( item );
    }
    treeItemHash.remove( aggregated->getCubeObject() );
    delete aggregated;
}

// -----------------------------------------------------------------
// ---------------- end iterations ------ --------------------------
// -----------------------------------------------------------------



// -----------------------------------------------------------------
// ---------------- calculation functions --------------------------
// -----------------------------------------------------------------

/**
 * @brief getSelectedFromHiddenIterations returns a list of items of the hidden iterations which correspond
 * to the selected merged item
 * @param iterations a list of hidden iterations, starting with tree level after root
 * @param selected path to the selected item, contains the names of each ancestor of the selected item,
 * starting with the level after root
 * @return list with the selected item for each iteration where an item with the same name at the same tree
 * level is found
 */
QList<TreeItem*>
getSelectedFromHiddenIterations( const QList<TreeItem*>& iterations, QStringList& selected )
{
    QList<TreeItem*> nextLevel;
    QList<TreeItem*> ret;

    QString selectedName = selected.takeFirst();
    foreach( TreeItem * iter, iterations )
    {
        foreach( TreeItem * child, iter->getChildren() )
        {
            if ( child->getName() == selectedName )             // found selected item in different iteration
            {
                nextLevel.push_back( child );
            }
        }
    }
    if ( !selected.empty() )
    {
        ret = getSelectedFromHiddenIterations( nextLevel, selected );
    }
    else
    {
        ret = nextLevel;
    }
    return ret;
}

QList<Task*>
DefaultCallTree::setBasicValues( const QList<Tree*>& leftTrees,
                                 const QList<Tree*>& rightTrees, const QList<TreeItem*>& itemsToCalculate )
{
    QList<Task*> workerData;
    /*
     * No update of tree is necessary if the call tree is to the left of the metric tree
     */
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
    list_of_metrics metric_selection;
    MetricTree*     metricTree = static_cast<MetricTree*>( trees[ METRIC ] );
    metric_selection = metricTree->getSelectedMetrics();

    // Set up selection for system tree, leave empty if system tree is right to call tree
    list_of_sysresources sysres_selection;
    SystemTree*          systemTree = static_cast<SystemTree*>( trees[ SYSTEM ] );
    sysres_selection = isLeft.contains( SYSTEM ) ? systemTree->getSelectedNodes() : CubeProxy::ALL_SYSTEMNODES;

    // calculate values for all invalidated tree items
    foreach( TreeItem * item, itemsToCalculate )
    {
        const list_of_cnodes& cnodes = getNodes( QList<TreeItem*>() << item );

        TreeTask* data = new TreeTask( item,
                                       metric_selection,
                                       cnodes,
                                       sysres_selection );
        workerData.append( data );
    }
    return workerData;
}

// -----------------------------------------------------------------
// ---------------- end calculation functions ----------------------
// -----------------------------------------------------------------

TreeItem*
DefaultCallTree::getTreeItem( uint32_t cnodeId ) const
{
    foreach( TreeItem * item, treeItems )
    {
        cube::Cnode* cnode = static_cast<cube::Cnode*> ( item->getCubeObject() );
        if ( cnode->get_id() == cnodeId )
        {
            return item;
        }
    }
    return 0;
}

/**
   updates the list of cnodes, that will be used for calculation of the trees left to the call tree
 */
void
DefaultCallTree::updateActiveNodes()
{
    // reduce active nodes to items that are available in model
    if ( !userRoots.isEmpty() ) // user has defined a new root => use inclusive value of the root
    {
        activeNodes = cube::list_of_cnodes();
        for ( TreeItem* userRoot : userRoots )
        {
            cnode_pair cp;
            cp.first  =  static_cast<cube::Cnode*> ( userRoot->getCubeObject() );
            cp.second = CUBE_CALCULATE_INCLUSIVE;
            activeNodes.push_back( cp );
        }
    }
    else
    {
        activeNodes = initialNodes;
    }
}

/** sets current item as call tree root item
 */
void
DefaultCallTree::setAsRoot( QList<TreeItem*> newRoots )
{
    userRoots = newRoots;

    // update tree item list after tree items have been removed
    updateActiveNodes();

    // invalidate all trees left of the call tree
    TabManager*  tab = Globals::getTabManager();
    QList<Tree*> left, right;
    tab->getNeighborTrees( left, right, this );
    foreach( Tree * tree, left )
    {
        tree->invalidateItems();
    }

    // update selection list: search for selected items in the new tree and select them again
    for ( TreeItem* item : treeItems )
    {
        if ( item->isSelected() )
        {
            selectItem( item, true );
        }
    }
    if ( selectionList.isEmpty() ) // at least one item has to be selected
    {
        selectItem( newRoots.first() );
    }

    updateHidenItemsInSelectionList();

    tab->reinit(); // recalculate trees
}

/** removes the given item from the tree
 */
void
DefaultCallTree::pruneItem( TreeItem* item )
{
    QList<TreeItem*>& prunedItems = prunedChildren[ item->getParent() ];
    prunedItems.append( item );

    TabManager* tab = Globals::getTabManager();
    item->getParent()->invalidate();  // parent's exclusive value has to be recalculated
    tab->reinit( item->getParent() ); // recalculate parent
}

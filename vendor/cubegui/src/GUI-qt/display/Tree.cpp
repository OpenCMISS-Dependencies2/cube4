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

#include "Tree.h"
#include "TreeModel.h"
#include "TreeView.h"

#include "Future.h"
#include "CubeProxy.h"
#include "CubeMetric.h"
#include "CubeCnode.h"
#include "CubeRegion.h"
#include "CubeMachine.h"
#include "CubeNode.h"
#include "CubeProcess.h"
#include "CubeThread.h"
#include "CubeVertex.h"
#include "CubeError.h"
#include "CubeGeneralEvaluation.h"
#include "CubeIdIndexMap.h"
#include "Globals.h"
#include "MetricTree.h"
#include "TreeStatistics.h"
#include "TreeItem.h"

using namespace std;
using namespace cube;
using namespace cubegui;

Tree::Tree( CubeProxy* cubeProxy ) : cube( cubeProxy )
{
    top           = nullptr;
    maxValue      = 0;
    lastSelection = 0;
    maxDepth      = -1;
    integerType   = false;
    active        = true;
    valueModus    = ABSOLUTE_VALUES;
    treeModel     = new TreeModel( this );
    activeFilter  = FILTER_NONE;
    hidingEnabled = false;
    toCalculate   = !Globals::optionIsSet( ManualCalculation );

    invalidateItemsPreCalc   = true;
    userDefinedMinMaxValues  = false;
    valueModusReferenceValue = 0.;

    statistics = new TreeStatistics( this );
}

Tree::~Tree()
{
    foreach( TreeItem * item, treeItemHash.values() )
    {
        delete item;
    }
    delete treeModel;
    delete top;
}

void
Tree::initialize()
{
    if ( top ) // reinit tree
    {
        delete top;
        top = nullptr;
    }

    top = createTree();
    top->setExpandedStatus( true );

    updateItemList();
}

QString
Tree::getLabel() const
{
    return label;
}

// update ordered list of all tree items of the tree and set visible root item for each element
void
Tree::updateItemList()
{
    treeItems.clear();
    foreach( TreeItem * rootItem, top->getChildren() )
    {
        QStack<TreeItem*> stack;
        stack.append( rootItem );
        while ( !stack.isEmpty() )
        {
            TreeItem* item = stack.pop();
            item->rootItem = rootItem;
            treeItems.append( item );

            const QList<TreeItem*>& children = item->getChildren();
            for ( auto i = children.size() - 1; i >= 0; i-- )
            {
                TreeItem* child = children[ i ];
                stack.push( child );
            }
        }
    }
}

void
Tree::calculateValues( const QList<Tree*>&     leftTrees,
                       const QList<Tree*>&     rightTrees,
                       const QList<TreeItem*>& itemsToCalculate
                       )
{
    calculationIsFinished = false;
    Globals::setStatusMessage( tr( "Calculating " ) + this->getLabel() + tr( " values ..." ) );

    integerType = false;
    foreach( Tree * tree, leftTrees )
    {
        if ( tree->getType() == METRIC )
        {
            // set integerType of the tree to true, if all selected metrics have integer type
            integerType = true;
            const QList<TreeItem*>& selected = tree->getSelectionList();
            foreach( TreeItem * item, selected )
            {
                if ( !static_cast<cube::Metric*> ( item->getCubeObject() )->isIntegerType() )
                {
                    integerType = false;
                    break;
                }
            }
        }
    }

    if ( invalidateItemsPreCalc )
    {
        invalidateItemsNow();
        invalidateItemsPreCalc = false;
    }

    QList<Task*> workerData;
    // Ensure that the inclusive value of all top level items is calculated. This is required to show the maximum value in the value view.
    if ( getType() != SYSTEM ) // systemtree is always calculated completely
    {
        for ( TreeItem* item : top->getChildren() )
        {
            if ( item->isExpanded() )
            {
                item->expanded = false; // calculate inclusive value
                if ( !item->isCalculated() )
                {
                    workerData += setBasicValues( leftTrees, rightTrees, { item } );
                }
                item->expanded = true;  // restore previous state
            }
        }
    }

    QList<TreeItem*> items = itemsToCalculate.size() == 0 ? getItemsToCalculate() : itemsToCalculate;
    workerData += setBasicValues( leftTrees, rightTrees, items );
    if ( workerData.size() > 0 )
    {
        connect( future, SIGNAL( calculationFinished() ), this, SLOT( calculationFinishedSlot() ) );
        future->addCalculations( workerData );
        future->startCalculation( true );

#ifdef CUBE_CONCURRENT_LIB
        emit calculationStarted( future );
#endif
    }
    else
    {
        if ( invalidateItemsPreCalc )  // required to draw invalid items, e.g. FlatTree::isValidSelection()
        {
            invalidateItemsNow();
            invalidateItemsPreCalc = false;
        }
        updateCachedItems();
        getStatistics()->update(); // update statistics for value view
        updateItems();
        emit calculationFinished();
    }
}

void
Tree::updateCachedItems()
{
    if ( getType() != METRIC )
    {
        return; // caching is only used for metric trees
    }
    bool updateItems = false;

    // check, if at least one visible item is not READY
    QList<TreeItem*> list = top->getChildren();
    while ( !list.isEmpty() )
    {
        TreeItem* item = list.takeFirst();
        if ( item->isExpanded() )
        {
            foreach( TreeItem * child, item->getChildren() )
            list.append( child );
        }
        if ( !item->isCalculationEnabled() )
        {
            continue;
        }
        updateItems = item->isExpanded() ? item->statusExclusive != item->READY : item->statusInclusive != item->READY;
        if ( updateItems )
        {
            break;
        }
    }

    if ( updateItems )
    {
        calculationFinishedSlot(); // notify and repaint
    }
}

void
Tree::invalidateItemsNow()
{
    foreach( TreeItem * item, this->getItems() )
    {
        item->invalidate();
        if ( activeFilter == FILTER_DYNAMIC )
        {
            item->hidden = false; // hidden items aren't recalculated -> ensure recalculation of all items for dynamic filtering
        }
    }
}

/**
 * clear data after work started in computeAllValues() is done
 * update the view
 */
void
Tree::calculationFinishedSlot()
{
    getStatistics()->update(); // update statistics for value view

    updateItems();
    calculationIsFinished = true;
    emit calculationFinished();
    disconnect( future, SIGNAL( calculationFinished() ), this, SLOT( calculationFinishedSlot() ) );
}

TreeModel*
Tree::getModel() const
{
    return treeModel;
}

void
Tree::setTrees( QList<Tree*> trees )
{
    treeList = trees;
}

bool
Tree::hasIntegerType() const
{
    if ( getValueModus() != ABSOLUTE_VALUES )
    {
        return false;
    }
    else
    {
        return integerType;
    }
}

const QList<TreeItem*>&
Tree::getItems() const
{
    return treeItems;
}

bool
Tree::itemIsValid( cube::Vertex* )
{
    return true;
}

TreeItem*
Tree::getRootItem() const
{
    return top;
}

DisplayType
Tree::getType() const
{
    return displayType;
}

TreeType
Tree::getTreeType() const
{
    return treeType;
}

double
Tree::getValueModusReferenceValue() const
{
    return valueModusReferenceValue;
}

void
Tree::valueModusChanged( ValueModus modus )
{
    this->valueModus = modus;
    this->invalidateItemLabel();
    updateItems();
}


Tree*
Tree::getActiveTree( DisplayType type ) const
{
    for ( Tree* tree: treeList )
    {
        if ( tree->getType() == type && tree->isActive() )
        {
            return tree;
        }
    }
    return nullptr;
}

void
Tree::setActive()
{
    for ( Tree* tree: treeList )
    {
        if ( tree->getType() == this->displayType )
        {
            tree->active = ( tree == this );
        }
    }
}

bool
Tree::isActive() const
{
    return active;
}

void
Tree::expandItem( TreeItem* item, bool expand )
{
    emit itemExpanded( item, expand );
}

void
Tree::selectItem( TreeItem* item, bool addToSelection )
{
    emit itemSelected( item, addToSelection );
}

void
Tree::deselectItem( TreeItem* item )
{
    emit itemDeselected( item );
}

TreeItem*
Tree::getLastSelection() const
{
    return lastSelection;
}

void
Tree::setLastSelection( TreeItem* value )
{
    lastSelection = value;
}

const QList<TreeItem*>&
Tree::getSelectionList() const
{
    return selectionList;
}

/**
 * Adds hidden children of selected parent items to selection list. This allows trees right of the current one to
 * calculate values for the hidden items, too.
 * Removes hidden children from previous selections.
 */
void
Tree::updateHidenItemsInSelectionList()
{
    foreach( TreeItem * item, hiddenSelections )
    {
        selectionList.removeOne( item );
    }
    hiddenSelections.clear();

    if ( activeFilter != FILTER_NONE )
    {
        // add hidden children to each selected item
        foreach( TreeItem * item, selectionList )
        {
            if ( item->isExpanded() && !item->isHidden() )
            {
                foreach( TreeItem * child, item->getChildren() )
                {
                    if ( child->isHidden() )
                    {
                        hiddenSelections.append( child );
                    }
                }
            }
        }
        selectionList.append( hiddenSelections );
    }
}

void
Tree::enableHiding()
{
    hidingEnabled = true;
}

void
Tree::setSelectionList( const QList<TreeItem*>& value )
{
    hiddenSelections.clear();
    foreach( TreeItem * item, selectionList )
    {
        item->setSelectionStatus( false );
    }

    selectionList = value;
    foreach( TreeItem * item, selectionList )
    {
        item->setSelectionStatus( true );
    }

    if ( activeFilter != FILTER_NONE )
    {
        updateHidenItemsInSelectionList();
    }
}

cube::CubeProxy*
Tree::getCube() const
{
    return cube;
}

/*
 * updates the labels of the tree and the value widget after calculation has been done
 */
void
Tree::updateItems( bool calculationFinished )
{
    bool maximumIsCalculated = false;
    if ( calculationFinished )
    {
        valueModusReferenceValue = computeReferenceValue( valueModus );
        computeMaxValues();
        maximumIsCalculated = true;
    }

    /* update the color and the label of the visible (expanded) items */
    QList<TreeItem*> list;
    list.append( top->getChildren() );

    while ( !list.isEmpty() )
    {
        TreeItem* item = list.takeFirst();
        item->updateItem( maximumIsCalculated );
        // Only update expanded items, only system tree has to update all its values as they are used in the topologies.
        // All system tree values are calculated at once, so no further call to this method is required if items are expanded.
        if ( item->isExpanded() || displayType == SYSTEM )
        {
            list.append( item->getChildren() );
        }
    }
}

void
Tree::invalidateItems()
{
    invalidateItemsPreCalc = true;
}

void
Tree::invalidateItemLabel()
{
    foreach( TreeItem * item, this->getItems() )
    {
        item->invalidateLabel();
    }
}

void
Tree::invalidateItemDisplayNames()
{
    foreach( TreeItem * item, this->getItems() )
    {
        item->displayName = "";
    }
}

void
Tree::removeItem( TreeItem* item )
{
    if ( item->getParent() )
    {
        item->getParent()->children.removeOne( item ); // remove item from parent
    }

    // remove item and all its children from treeItem and selection list
    QStack<TreeItem*> stack;
    stack.push( item );
    while ( !stack.isEmpty() )
    {
        TreeItem* item = stack.pop();

        selectionList.removeOne( item );
        treeItems.removeOne( item );

        foreach( TreeItem * child, item->getChildren() )
        {
            stack.push( child );
        }
    }
}

void
Tree::addItem( TreeItem* item, TreeItem* parent )
{
    assert( parent );
    parent->addChild( item );

    // add item and all its children to treeItem list and hash
    QStack<TreeItem*> stack;
    stack.push( item );
    while ( !stack.isEmpty() )
    {
        TreeItem* item = stack.pop();
        treeItemHash.insert( item->getCubeObject(), item );
        treeItems.append( item );
        foreach( TreeItem * child, item->getChildren() )
        {
            stack.push( child );
        }
    }
}

double
Tree::getMaxValue( const TreeItem* ) const
{
    return maxValue;
}

// get the larger one from the item's current own and total value
void
Tree::getMax( TreeItem* item, double& value )
{
    // we compare just the absolut values. In the case of the differences
    // of the cubes it may appear negatives values.
    value = ( fabs( item->ownValue ) < fabs( item->totalValue ) ? item->totalValue : item->ownValue );

    // don't use own value, if value object is zero ( = maximum negative value )
    if ( item->getOwnValueObject() && ( ( cube::Value* )item->getOwnValueObject() )->isZero() )
    {
        value = item->totalValue;
    }
}

void
Tree::computeMaxValues()
{
    double value;
    double max = getItems().first()->totalValue;

    // search for the minmal/maximal values in the whole tree
    foreach( TreeItem * item, getItems() )
    {
        getMax( item, value );
        // If the difference of two cube files (e.g. File->Open external) is displayed, the inclusive value of a parent item may be smaller than the
        // absolute values of the child nodes. For that reason, the absolute values of all children have to be compared.
        if ( fabs( max ) < fabs( value ) )
        {
            max = value;
        }
    }
    double roundThreshold = Globals::getRoundThreshold( FORMAT_TREES );
    if ( fabs( max ) <= fabs( roundThreshold ) )
    {
        max = 0.0;
    }
    maxValue = max;
}

// this method computes the reference value (100%) for percentage-based value modi
//
double
Tree::computeReferenceValue( ValueModus valueModus )
{
    Tree* tree;
    if ( valueModus == METRICSELECTED_VALUES  ||
         valueModus == METRICROOT_VALUES ||
         valueModus == EXTERNAL_VALUES )
    {
        tree = getActiveTree( METRIC );
    }
    else if ( valueModus == CALLSELECTED_VALUES  ||
              valueModus == CALLROOT_VALUES )
    {
        tree = getActiveTree( CALL );
    }
    else // ( valueModus == SYSTEMSELECTED  || valueModus == SYSTEMROOT )
    {
        tree = getActiveTree( SYSTEM );
    }

    // since we divide the absolute value by the reference value,
    // the reference value 0.0 would result in the value "undefined"
    double referenceValue = 0.0;

    // get the proper value of the required selected, root, or external item
    if ( valueModus == METRICSELECTED_VALUES ||
         valueModus == CALLSELECTED_VALUES ||
         valueModus == SYSTEMSELECTED_VALUES )
    {
        QList<TreeItem*> selected = tree->getSelectionList();
        foreach( TreeItem * item, selected )
        {
            referenceValue += item->isExpanded() ? item->ownValue : item->totalValue;
        }
    }
    else if ( valueModus == METRICROOT_VALUES )
    {
        // for metrics, only items with the same root can be selected simultaneously;
        // thus we take this common root's value
        if ( tree->getSelectionList().size() > 0 )
        {
            referenceValue = tree->lastSelection->rootItem->totalValue;
        }
    }
    else if ( valueModus == CALLROOT_VALUES  ||
              valueModus == SYSTEMROOT_VALUES )
    {
        // for the call tree, items with different roots can be selected;
        // in this case we take the sum of all roots as reference value
        bool* seen = new bool[ tree->getItems().size() ];
        for ( int i = 0; i < tree->getItems().size(); i++ )
        {
            seen[ i ] = false;
        }
        foreach( const TreeItem * item, tree->getSelectionList() )
        {
            item = item->getTopLevelItem();

            int id = item->cubeObject->get_id();
            if ( !seen[ id ] )
            {
                referenceValue += item->rootItem->totalValue;
                seen[ id ]      = true;
            }
        }
        delete[] seen;
    }
    else if ( valueModus == EXTERNAL_VALUES )
    {
        // get root item of the recently selected metric tree item as reference item for trees but metric tree
        TreeItem*     item       = tree->getLastSelection();
        cube::Metric* metric     = static_cast<cube::Metric* > ( item->getTopLevelItem()->getCubeObject() );
        QString       metricName = QString::fromStdString( metric->get_uniq_name() );
        MetricTree*   metricTree = static_cast<MetricTree*> ( tree );
        referenceValue = metricTree->getExternalReferenceValue( metricName );
    }

    // round values very close to 0.0 down to 0.0
    double roundThreshhold = Globals::getRoundThreshold( FORMAT_TREES );
    if ( referenceValue <= roundThreshhold && referenceValue >= -roundThreshhold )
    {
        referenceValue = 0.0;
    }
    return referenceValue;
}
// end of computeReferenceValues()

/**
 * Hiding is done by TreeModelProxy, but the values of the hidden children have to
 * be added to the visible parent item.
 */
void
Tree::hideMinorValues( double threshold )
{
    QList<TreeItem*> items = top->getChildren();
    while ( !items.isEmpty() )
    {
        TreeItem* item = items.takeLast();

        if ( item->isCalculated() )
        {
            double maxValue = getMaxValue( item );
            bool   visible  =  item->getTotalValue() * 100 / maxValue >= threshold;
            item->setHidden( !visible );
        }
        else // if child values aren't calculated, the parent should remain expandable
        {
            item->setHidden( false );
        }

        if ( !item->isHidden() )
        {
            items.append( item->getChildren() );
        }
    }

    /* Ensure that no hidden elements remain selected before filtering is done, otherwise they automatically get deselected
     * while filtering is done and the handling of the emitted signals cause problems.
     * Deselect all hidden elements and select visible parents instead.
     */
    QList<TreeItem*> previousSelections = selectionList;
    foreach( TreeItem * item, previousSelections ) // select parent item of selected hidden items
    {
        TreeItem* parent = item;
        while ( parent && !parent->isHidden() )
        {
            parent = parent->getParent();
        }
        if ( parent )                              // item is hidden or has hidden parent
        {
            while ( parent && parent->isHidden() ) // search visible parent
            {
                parent = parent->getParent();
            }
            parent->select( true ); // first select then deselect because at least one selection is required
            item->deselect();
        }
    }
}

void
Tree::unhideItems()
{
    updateHidenItemsInSelectionList();
    foreach( TreeItem * item, treeItems )
    {
        item->setHidden( false );
    }
}


bool
Tree::hasUserDefinedMinMaxValues() const
{
    return userDefinedMinMaxValues;
}

double
Tree::getUserDefinedMinValue() const
{
    return userMinValue;
}

double
Tree::getUserDefinedMaxValue() const
{
    return userMaxValue;
}

void
Tree::unsetUserDefinedMinMaxValues()
{
    userDefinedMinMaxValues = false;
}

void
Tree::setUserDefinedMinMaxValues( double minValue, double maxValue )
{
    this->userDefinedMinMaxValues = true;
    this->userMinValue            = minValue;
    this->userMaxValue            = maxValue;
}

void
Tree::setFilter( FilterType filter, double threshold )
{
    activeFilter = filter;
    if ( filter == FILTER_NONE )
    {
        unhideItems();
    }
    else if ( threshold >= 0 )
    {
        hideMinorValues( threshold );
    }

    updateHidenItemsInSelectionList();
    invalidateItemLabel(); // recreate label to add percentage of hidden children to parent
    updateItems();         // add value of hidden items to parent
}

int
Tree::getDepth()
{
    return maxDepth;
}

QList<TreeItem*>
Tree::getItemsToCalculate()
{
    QList<TreeItem*> list = top->getChildren();
    QList<TreeItem*> itemsToCalculate;
    while ( !list.isEmpty() )
    {
        TreeItem* item = list.takeFirst();
        if ( item->isExpanded() )
        {
            foreach( TreeItem * child, item->getChildren() )
            list.append( child );
        }
        if ( item->isCalculationEnabled() && !item->isCalculated() )
        {
            itemsToCalculate.append( item );
        }
    }

    // qDebug() << "Tree::getItemsToCalculate() " << this->getLabel() << itemsToCalculate.size();
    return itemsToCalculate;
}

// static method for use in QtConcurrent
void
Tree::calculate( Task* data )
{
    data->calculate();
    delete data;
}

/**
 * @brief Tree::getStatisticValues returns the required statistical values for the value widgets.
 */
TreeStatistics*
Tree::getStatistics()
{
    return statistics;
}

void
Tree::setFuture( Future* future )
{
    this->future = future;
}


/** used to calculate the data of root_cnode in a thread */
void
TreeTask::calculate()
{
    Tree*            tree = static_cast<Tree*> ( item->getTree() );
    cube::CubeProxy* cube = tree->getCube();

    Value* result = cube->calculateValue( metric_selection,
                                          cnode_selection,
                                          sysres_selection );

    if ( expanded ) // set exclusive/inclusive value depending of the state of the item when the task was created
    {
        item->setExclusiveValue( result );
    }
    else
    {
        item->setInclusiveValue( result );
    }
}

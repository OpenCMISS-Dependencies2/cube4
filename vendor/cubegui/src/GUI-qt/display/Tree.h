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


#ifndef TREE_H
#define TREE_H
#include <cassert>
#include <QtCore>
#include <functional>

#include "Constants.h"
#include "CubeVertex.h"
#include "TreeItem.h"
#include "CubeTypes.h"
#include "CubeSysres.h"
#include "CubeSystemTreeNode.h"
#include "CubeLocationGroup.h"
#include "CubeLocation.h"
#include "CubeMetric.h"
#include "CubeRegion.h"
#include "CubeCnode.h"
#include "CubeCartesian.h"
#include "CubeValues.h"
#include "Task.h"

namespace cube
{
class CubeProxy;
class Value;
class Vertex;
class IdIndexMap;
}

namespace cubegui
{
class TreeModel;
class Task;
class TreeStatistics;
class Future;

class Tree : public QObject
{
    Q_OBJECT

public:
    Tree( cube::CubeProxy* cube );
    ~Tree();
    /** creates the tree structure from the given cube
     * If cube is null, the existing tree is reinitialized.
     */
    void
    initialize();

    /** returns the tree label */
    virtual QString
    getLabel() const;

    /** Future allows calculations to be done in parallel */
    void
    setFuture( Future* future );

    /**
     * Set the values according to position in Tab order
     * @param left List of trees to the left of this tree
     * @param right List of trees to the right of this tree
     * @param itemsToCalculateNow List of tree items, that should be calculated only once in expert mode
     */
    void
    calculateValues( const QList<Tree*>&     left,
                     const QList<Tree*>&     right,
                     const QList<TreeItem*>& itemsToCalculateNow = QList<TreeItem*>( ) );

    /** uses the computed values to generate the labels and colors */
    void
    updateItems( bool calculationFinished = true );

    /**
     * Invalidates all tree items and sets their values to zero after current calculation has been finished and before next calculation is done
     */
    void
    invalidateItems();

    /**
     * Invalidates all tree items label
     */
    void
    invalidateItemLabel();

    /**
     * Invalidates all tree items display names
     */
    void
    invalidateItemDisplayNames();


    /** returns the top level item of the tree. This item is an empty invisible item
     *  whose children are displayed.
     */
    TreeItem*
    getRootItem() const;

    /** returns the concrete type of the tree */
    TreeType
    getTreeType() const;

    /** returns the type of the tree METRIC, CALL, SYSTEM */
    DisplayType
    getType() const;


    /** returns true, if all tree items have integer values */
    bool
    hasIntegerType() const;

    /** returns all tree items in arbitrary order */
    const QList<TreeItem*>&
    getItems() const;

    /** returns an interface to the tree data which can be used by Qt TreeViews */
    TreeModel*
    getModel() const;

    /** Returns the maximum value of this tree. For metric trees, the maximum depends on the root element
        of the given item, for all other trees, the item is ignored */
    virtual double
    getMaxValue( const TreeItem* item = 0 ) const;

    /** returns the reference value (100%) to calculate the value of each item in the current value modus */
    double
    getValueModusReferenceValue() const;

    /** provides the tree with the data of the other trees */
    void
    setTrees( QList<Tree*> trees );

    // ----- interaction with the view ----

    /** returns the current value modus, which the user has selected in the GUI */
    ValueModus
    getValueModus() const
    {
        return valueModus;
    }

    /** called by the view, if the user has changed the value modus */
    void
    valueModusChanged( ValueModus modus );

    /** activates current Tree and deactives all other trees of the same type */
    void
    setActive();

    /** returns the active (=visible) tree of the given type */
    Tree*
    getActiveTree( DisplayType type ) const;

    bool
    isActive() const;

    /** notifies the view that the given item has been expanded */
    void
    expandItem( TreeItem* item,
                bool      expand );

    /** notifies the registered view that the given item has been selected */
    void
    selectItem( TreeItem* item,
                bool      addToSelection = false );

    /** notifies the registered view that the given item has been deselected */
    void
    deselectItem( TreeItem* item );

    /** returns the most recently selected item */
    TreeItem*
    getLastSelection() const;

    /** returns the items, which have been selected in the view */
    const QList<TreeItem*>&
    getSelectionList() const;

    /* used by the view to set the information about the selected item */
    void
    setLastSelection( TreeItem* value );
    void
    setSelectionList( const QList<TreeItem*>& value );

    cube::CubeProxy*
    getCube() const;

    bool
    hasUserDefinedMinMaxValues() const;
    double
    getUserDefinedMinValue() const;
    double
    getUserDefinedMaxValue() const;
    void
    unsetUserDefinedMinMaxValues();
    void
    setUserDefinedMinMaxValues( double minValue,
                                double maxValue );

    enum FilterType { FILTER_STATIC, FILTER_DYNAMIC, FILTER_NONE };

    /** @brief Tree::setFilter allows to set or unset a dynamic or static tree item filter. If a filter is set,
     *  all tree items whose values are less than threshold are set to hidden
     *  @param threshold threshold in percent of the maximum value */
    void
    setFilter( FilterType filter,
               double     threshold = -1 );

    FilterType
    getFilter()
    {
        return activeFilter;
    }
    int
    getDepth(); // returns the depth of the tree

    void
    enableHiding();

    bool
    isHidingEnabled()
    {
        return hidingEnabled;
    }

    void
    unhideItems();

    bool
    isManualCalculation()
    {
        return !toCalculate;
    }

    TreeStatistics*
    getStatistics();

    friend class TreeModel;

    void
    setCreationFilter( const std::function<bool ( cube::Vertex* )>& value );

protected:
    QString label;

    /* @brief adds the given item and its children to the given parent item. */
    void
    addItem( TreeItem* item,
             TreeItem* parent );

    /* @brief removes the given item and its children from the tree
     * The items are deleted when the tree is deleted. */
    void
    removeItem( TreeItem* item );

signals:
    void
    itemExpanded( TreeItem* item,
                  bool      expand );

    void
    itemSelected( TreeItem* item,
                  bool      addToSelection = false );

    void
    itemDeselected( TreeItem* item );

    /** required to update tree view while calculation is still running */
    void
    calculationStarted( const Future* );

    /** emitted, if calculation of tree items has been finished -> connect to views */
    void
    calculationFinished();

protected:
    // ============================================================================================

    cube::CubeProxy*                cube;
    FilterType                      activeFilter;
    DisplayType                     displayType;     // METRIC, CALL, SYSTEM
    TreeType                        treeType;        // concrete type
    TreeItem*                       top;             // invisible top level tree item (root)
    QHash<cube::Vertex*, TreeItem*> treeItemHash;    // hash to find the tree item for a given Vertex, also contains items that have been cut/pruned
    QList<TreeItem*>                treeItems;       // list of all items from current root
    bool                            integerType;     // true, if metric has integer type
    double                          maxValue;        // the maximum value of the tree

    QList<Tree*>     treeList;                       // all available trees
    ValueModus       valueModus;                     // currently selected value modus
    double           valueModusReferenceValue;       // calculated reference value for current value modus
    bool             active;                         // true, if corresponding view is visible (tab is selected)
    QList<TreeItem*> selectionList;                  // all currently selected items
    TreeItem*        lastSelection;                  // the most recently selected item
    bool             calculationIsFinished;          // true, if parallel computation is still running

    TreeModel* treeModel;                            // simple model: Tree->QAbstractItemModel

    // ============================================================================================

    /**
     * Set the basic inclusive and exclusive values in the full tree.
     * @param left  List of trees to the left of this tree
     * @param right List of trees to the right of this tree
     * @param top   Pointer to the top of the tree
     * @param itemsToCalculate, if list is not empty, only the given items are calculated, otherwise all visible items are calculated
     */
    virtual QList<Task*>
    setBasicValues( const QList<Tree*>&     left,
                    const QList<Tree*>&     right,
                    const QList<TreeItem*>& itemsToCalculate = QList<TreeItem*>( ) ) = 0;

    /** returns a list with tree items, that are visible and whose values aren't calculated yet */
    QList<TreeItem*>
    getItemsToCalculate();

    virtual TreeItem*
    createTree() = 0;

    virtual bool
    itemIsValid( cube::Vertex* vertex );

    virtual QString
    getItemName( cube::Vertex* vertex ) const = 0;

    virtual void
    computeMaxValues();

    void
    getMax( TreeItem* item,
            double&   value );

    void
    updateHidenItemsInSelectionList();

    // static method for use in QtConcurrent
    static void
    calculate( Task* data );

    /**
       creates tree structure with Tree::top as (empty) top level element from a list of Vertex. It the vertex
       has children, all children will be added recursively.
     * @param top top level element of the tree
     * @param vec list of vertex which will be added to the tree
     * @param itemType METRICITEM, CALLITEM, REGIONITEM, SYSTEMTREENODEITEM, LOCATIONGROUPITEM, LOCATIONITEM
     * @param newItems if this list exists, it is filled with the created items
     */
    template <typename T>
    void
    createItems( TreeItem* top, const std::vector<T*>& vec, const TreeItemType itemType,
                 QList<TreeItem*>* newItems = 0, bool recursive = true )
    {
        for ( cube::Vertex* vertex : vec )
        {
            if ( !itemIsValid( vertex ) )
            {
                continue;
            }

            QString name = getItemName( vertex );

            TreeItem* parent = treeItemHash.value( vertex->get_parent() );
            if ( parent == nullptr ) // top-level item without parent (or after reroot)
            {
                parent = top;
            }

            TreeItem* item = treeItemHash.value( vertex );
            if ( !item ) // build new tree
            {
                item = new TreeItem( this, name, itemType, vertex );
                treeItemHash.insert( vertex, item );
            }
            else // rebuild existing tree
            {
                item->children.clear();
            }
            parent->addChild( item );

            if ( newItems )
            {
                newItems->append( item );
            }

            if ( recursive && vertex->num_children() > 0 )
            {
                createItems<cube::Vertex>( top, vertex->get_children(), itemType, newItems );
            }
            if ( item->getDepth() > maxDepth )
            {
                maxDepth = item->getDepth();
            }
        }
    }     // end of createItems()

    void
    updateItemList();

private slots:
    void
    calculationFinishedSlot();

private:
    double userMinValue;                  // user defined minimum value
    double userMaxValue;                  // user defined maximum value
    bool   userDefinedMinMaxValues;
    int    maxDepth;
    bool   hidingEnabled;
    bool   toCalculate;                                  // = Globals::getOption( "DisableCalculation" )
    bool   invalidateItemsPreCalc;                       // if true, all items have to be invalidated before next calculation starts

    QList<TreeItem*> hiddenSelections;                   // filtering

    TreeStatistics* statistics;

    Future* future;

    double
    computeReferenceValue( ValueModus valueModus );

    /** @brief Tree::hideMinorValues sets all tree items to hidden whose values are less than threshold
     *  @param threshold threshold in percent of the maximum value */
    void
    hideMinorValues( double threshold );

    /** Directly invalidates all items. Should not be called while calculation is running, @see invalidateItems */
    void
    invalidateItemsNow();
    void
    updateCachedItems();
};

/** for use in QConcurrent */
class TreeTask : public Task
{
public:
    TreeTask( TreeItem* item_, const cube::list_of_metrics& m, const cube::list_of_cnodes& c, const cube::list_of_sysresources& s ) :
        item( item_ ), metric_selection( m ), cnode_selection( c ), sysres_selection( s )
    {
        expanded = item->isExpanded();
    }

    virtual void
    calculate();

private:
    TreeItem*                        item;
    const cube::list_of_metrics      metric_selection;
    const cube::list_of_cnodes       cnode_selection;
    const cube::list_of_sysresources sysres_selection;
    bool                             expanded;
};
}
#endif // TREE_H

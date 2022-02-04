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


#ifndef TREEITEMDATA_H
#define TREEITEMDATA_H
#include "Constants.h"
#include <QColor>
#include <QList>
#include <mutex>

namespace cube
{
class Cube;
class Vertex;
class Value;
class Cnode;
}

namespace cubepluginapi
{
class PluginServices;
}

namespace cubegui
{
class Tree;
class TreeItemMarker;
class TabManager;
class TreeItemMarker;
class TreeStatistics;

class TreeItem
{
public:
    // allow all trees to modify all item properties, but don't allow plugins
    friend class Tree;
    friend class TreeStatistics;
    friend class MetricTree;
    friend class SystemTree;
    friend class CallTree;
    friend class DefaultCallTree;
    friend class FlatTree;
    friend class TaskCallTree;
    friend class AggregatedTreeItem;
    friend class TreeModel;
    friend class TreeTask;

    friend class TabManager;
    friend class TreeView;
    friend class cubepluginapi::PluginServices;
    friend class TreeItemDelegate;

    TreeItem( cubegui::Tree* tree,
              const QString& name,
              TreeItemType   type,
              cube::Vertex*  cube );
    virtual
    ~TreeItem();

    // ---- getter ----------------------------------------

    /** @brief calculates and returns the value for the selected value modus.
     * @param ok If ok is given, it returns if the value is valid.
     * For some metrics (e.g. metrics which are building a minimum) no valid exclusive value
     * exists. The zero value for a minimum metrics is the maximum negative value. */
    inline double
    getValue( bool* ok = 0 ) const
    {
        if ( ok )
        {
            *ok = currentValid;
        }
        return currentValue;
    }

    /** @brief returns the absolute value of the item in its current state (collapsed/expanded) */
    double
    getAbsoluteValue() const;

    /** @brief returns the absolute exclusive value */
    double
    getOwnValue() const;

    const cube::Value*
    getOwnValueObject() const;

    /** @brief returns the absolute inclusive value */
    double
    getTotalValue() const;

    const cube::Value*
    getTotalValueObject() const;

    cube::Value*
    getValueObject() const;

    /** returns the tree level of the current item */
    int
    getDepth() const; // check usage

    /** return true, if the currently displayed (expanded/collapsed) value has already been calculated */
    bool
    isCalculated() const
    {
        return getCalculationStatus() != INVALID;
    }

    bool
    isExpanded() const
    {
        // use inclusive value for hidden items
        return !hidden && expanded;
    }
    bool
    isHidden() const;
    bool
    isSelected() const;

    /** returns the corresponding cube Vertex object or NULL, if the item is aggregated */
    cube::Vertex*
    getCubeObject() const;

    /** returns the name of the item */
    const QString&
    getName() const
    {
        return displayName;
    }

    /** returns the name of the item, eg. a call tree item which is not demangled */
    const QString&
    getOriginalName() const
    {
        return name;
    }

    /** returns the label of the item with consists of its name and the item value */
    const QString&
    getLabel() const
    {
        return label;
    }

    /** returns the color, which is used as icon the corresponding tree widget */
    const QColor&
    getColor() const;

    /** returns the parent of the current item */
    TreeItem*
    getParent() const;

    /** true, if the item has no children */
    bool
    isLeaf() const;

    /** returns true, if the item is the root item of an aggregated loop */
    virtual bool
    isAggregatedRootItem() const;

    /** returns true, if the item is an aggregation from all loop iteration */
    bool
    isAggregatedLoopItem() const;

    /** returns a list of all children of the current item */
    const QList<TreeItem*>&
    getChildren() const
    {
        return children;
    }

    TreeItemType
    getType() const;

    /** @brief Searches recursively for all children of the item which are leafs.
     *  @returns list of leaf items */
    QList<TreeItem*>
    getLeafs() const;

    /** expands the current item or collapses it (expand = false) */
    void
    setExpanded( bool expand );

    /** selects the current item
     *  @param addToSelection if false, all previously selected item are deselected */
    void
    select( bool addToSelection = false );

    /** deselects the current item */
    void
    deselect();

    /** updates the label and color of the item according to its value
     * @param inProgress true, if calculation hasn't finished -> maximum/minimum not yet available -> colors not yet available
     */
    void
    updateItem( bool maximumIsCalculated = true );

    /** returns true, if the item is an item of the first visible tree level */
    bool
    isTopLevelItem() const;

    bool
    isDerivedMetric() const;

    /** returns the visible root of the current item */
    const TreeItem*
    getTopLevelItem() const;

    /** returns a list of TreeItemMarkers which have been set for the current item */
    const QList<const TreeItemMarker*>&
    getMarkerList() const;

    /** if item is a call tree item or a flat tree item, the corresponding source file and the
        line number information is returned */
    void
    getSourceInfo( QString& filename,
                   int&     startLine,
                   int&     endLine ) const;

    /** returns true, if the item in the current value modus should be displayed in integer format */
    bool
    isIntegerType() const;

    double
    calculateValue( ValueModus modus,
                    bool&      ok,
                    bool       exclusive ) const;

    QColor
    calculateColor( double value ) const;

    /** returns the kind of tree: METRIC, CALL OR SYSTEM */
    DisplayType
    getDisplayType() const;

    /** returns the subtype of tree the item is part of */
    TreeType
    getTreeType() const;


    // returns the tree, the item is part of4
    Tree*
    getTree() const;

    /** converts the position of the item into a QVariant which can be saved in QSettings */
    QVariant
    convertToQVariant();

    /** expert mode: only calculate choosen items */
    bool
    isCalculationEnabled()
    {
        return toCalculate;
    }
    void
    setCalculationEnabled( bool enabled = true );

    /** returns the tree item at the position given by entry (@see convertToQVariant)
     * @param entry contains the tree type and the position of the item in the tree
     */
    static TreeItem*
    convertQVariantToTreeItem( QVariant entry );

protected:
    // the type of the node;
    // needed to distinguish how to handle nodes
    TreeItemType type;

    QString name;         // initial name
    QString displayName;  // mangled/demangled name, depends on settings
    QString label;        // text of the item in the QTreeView

    // the corresponding cube object
    cube::Vertex* cubeObject;

    // tree structure
    Tree*            tree; // the tree the item is part of
    TreeItem*        parentItem;
    QList<TreeItem*> children;
    int              depth; // depth of the current item in the tree
    TreeItem*        rootItem;

    // values attached to the item
    double       ownValue;   // expanded value
    double       totalValue; // collapsed value
    bool         singleValue;
    cube::Value* ownValue_adv;
    cube::Value* totalValue_adv;
    double       displayValue; // value including hidden childen

    /** INVALID: not yet calculated
     *  CALCULATED: cube value has been calculated, but hidden values aren't calculated yet, label isn't set
     *  READY: label and icon are ready to be displayed */
    enum CalculationStatus { INVALID, CALCULATED, READY };
    CalculationStatus statusInclusive, statusExclusive;

    // item values/colors for the current state
    QColor  colorExpanded, colorCollapsed;
    QString textExpanded, textCollapsed;

    /* view status */
    bool                         expanded;
    bool                         hidden;
    bool                         selected;
    QList<const TreeItemMarker*> markerList;             // markers for current item
    QList<const TreeItemMarker*> parentMarkerList;       // markers for child items; used to mark parent if child is not expanded
    QList<const TreeItemMarker*> dependencyMarkerList;   // markers to show a dependency to another marked item

    // the minimal and maximal peer values for system tree items
    double minValue;
    double maxValue;

    // ============================================================================
    // function to build the tree
    void
    addChild( TreeItem* item );
    void
    removeChild( TreeItem* item );
    void
    setValue( cube::Value* value );
    void
    setValues( cube::Value* total,
               cube::Value* own );
    void
    setInclusiveValue( cube::Value* value );
    void
    setExclusiveValue( cube::Value* value );

    // end function to build the tree

    void
    setMarker( const TreeItemMarker* marker,
               bool                  isDependencyMarker = false,
               bool                  markParents = true );
    void
    removeMarker( const TreeItemMarker* marker );

    void
    invalidate();

    void
    invalidateLabel();

private:
    mutable std::mutex cmutex;

    // currentValue for expanded items can differ from ownValue, if the item has hidden children. In this case, the
    // total values of the hidden children are added up to the value
    double currentValue; // the currently displayed value
    bool   currentValid; // @see getValue
    bool   toCalculate;  // expert mode: only calculate items if toCalculate is set

    // setter to be called by the view to update the status of the item
    void
    setExpandedStatus( bool expanded,
                       bool recursive = false );
    void
    setSelectionStatus( bool selected = true );

    void
    setHidden( bool hidden );

    CalculationStatus
    getCalculationStatus() const
    {
        return isExpanded() ? statusExclusive : statusInclusive;
    }

    void
    setCalculationStatus( CalculationStatus status )
    {
        if ( isExpanded() )
        {
            statusExclusive = status;
        }
        else
        {
            statusInclusive = status;
        }
    }

    // functions to build TreeModel
    TreeItem*
    child( int row ) const;
    int
    row() const;

    // end functions for TreeModel

    void
    setDepth( int value );

    void
    calculateValuePlusHidden( bool&   valueOk,
                              double& value,
                              bool&   hiddenValueOk,
                              double& hiddenChildrenValue,
                              int&    hiddenChildren );
    void
    calculateValuePlusHidden( bool&      valueOk,
                              double&    value,
                              bool&      hiddenValueOk,
                              double&    hiddenChildrenValue,
                              int&       hiddenChildren,
                              ValueModus modus );
};
}
#endif // TREEITEMDATA_H

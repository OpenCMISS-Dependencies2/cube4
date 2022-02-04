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


#ifndef TREEVIEW_H
#define TREEVIEW_H

#include "cubegui-concurrent.h"
#include <QTreeView>
#include <QHeaderView>
#include <QMenu>
#include <QStyledItemDelegate>

#include "TabInterface.h"
#include "TreeModel.h"
#include "PluginServices.h"
#include "SettingsHandler.h"
#include "Environment.h"
#include "FindWidget.h"
#include "Tree.h"

namespace cubegui
{
class HelpBrowser;
class TreeItem;
class InfoWidget;
class TreeModelProxy;
class FindWidget;

/**
 * @brief The TreeItemDelegate class is responsible for the drawing of the tree items
 */
class TreeItemDelegate : public QStyledItemDelegate
{
public:
    virtual QSize
    sizeHint( const QStyleOptionViewItem& option,
              const QModelIndex&          index ) const;
    virtual void
    paint( QPainter*                   painter,
           const QStyleOptionViewItem& option,
           const QModelIndex&          index ) const;

    void
    updateRowHeight( QModelIndex& index );

private:
    const static int iconPad = 2; // space left to the icon

    QPixmap
    createIcon( TreeItem* item,
                int       iconSize,
                bool      gray ) const;
    void
    paintMarker( QPainter*    painter,
                 const QRect& textRect,
                 TreeItem*    item ) const;
};

/**
 * @brief TreeView is a graphical representation of the Tree class.
 * The mapping is done with treeModel which inherits from QAbstractItemModel.
 */
class TreeView : public QTreeView, public TabInterface, public SettingsHandler
{
    Q_OBJECT
public:
    /**
     * @param modelInterface contains a QAbstractItemModel e.g. QSortFilterProxyModel. It will be deleted in destructor.
     * @param tabLabel the label of the QTabWidget in the view
     */
    TreeView( TreeModelInterface* modelInterface,
              const QString&      tabLabel );
    ~TreeView();

    // implementation of TabInterface
    virtual QWidget*
    widget();
    virtual QString
    label() const;
    virtual QIcon
    icon() const;

    void
        valueModusChanged( ValueModus );
    // end implementation of TabInterface

    /** called if current Tab becomes active or inactive */
    virtual void
    setActive( bool );

    void
    setIcon( const QIcon& icon );

    /** returns the tree that belongs to the model of the view */
    Tree*
    getTree() const;

    /** inserts a menu item from a plugin into the context menu below the tree menu items */
    void
    insertPluginMenuItem( QAction* pluginAction );

    void
    loadExperimentSettings( QSettings& );

    void
    saveExperimentSettings( QSettings& );

    void
    loadStatus( QSettings& settings );

    /** @brief TreeView::saveStatus
     * Saves the tree status to the given settings. The tree items are identified by their name, not by their
     * index (@see saveExperimentSettings). This function is used to send the tree status to Clipboard */
    void
    saveStatus( QSettings& settings );

    void
    mousePressEvent( QMouseEvent* event );

    /** @brief TreeView::saveExperimentSettingsByName
     *  @param settings
     *  Saves the tree settings. It identifies the tree items not by index but by the name of the item and its parents.
     */

    /** selects all items */
    void
    selectItems( const QModelIndexList& toSelect );

    /** searched for all items that match the given item names */
    QModelIndexList
    searchItems( const QStringList& items );

    bool
    hasUserDefinedMinMaxValues() const;
    void
    setUserDefinedMinMaxValues( bool value );

    virtual void
    selectionChanged( const QItemSelection& selected,
                      const QItemSelection& deselected );

    QSize
    sizeHint() const;

    QString
    settingName();

    QModelIndexList
    convertToModelIndexList( QList<TreeItem*> items );

    void
    updateRowHeight();

    void
    expandFoundItems();

protected:
    virtual QString
    getContextDescription( TreeItem* item ) = 0;
    void
    paintEvent( QPaintEvent* event );

signals:
    /** this signal is emitted, if the selected items have been changed */
    void
    selectionChanged( Tree* tree );

    /** this signal is emitted, if the item has been expanded (expanded = true) or collapsed */
    void
    itemExpanded( TreeItem* item,
                  bool      expanded );

    /** this signal is emitted, if the currenly displayed values have to be recalculated */
    void
    recalculateRequest( Tree* tree );

    /** this signal is emitted, if the given tree items should be calculated (only used in ManualCalculation mode) */
    void
    calculateRequest( QList<TreeItem*> );

public slots:
    /** selects the given tree item. If add is false, the previous selection is cleared */
    void
    selectItem( TreeItem* item,
                bool      add );

    /** calls updateValues on the model */
    void
    updateValues();

    /** shows documentation about the recently selected item */
    void
    onShowDocumentation();

    /** shows a dialog set set minimum and maximum value for coloring */
    void
    onMinMaxValues();

    /** notifies view, that selected items of a dependent view (e.g. flat tree view) have changed */
    void
    invalidateSelection( Tree* )
    {
    }

    void
    updateInfoWidget();

    /*
     * thee modifications (e.g. hiding/filtering) may cause that the status of hidden elements gets lost in the view which causes model and
     * view to become inconsistent => set collapsed status from model
     */
    void
    updateExpandedCollapsedStatus();

    void
    filteringEnded();

    void
    setFuture( const Future* future );

#ifdef CUBE_CONCURRENT_LIB
private:
    const QFutureWatcher<void>* futureWatcher;
#endif

private slots:
    /** mark item as found */
    void
    markItem();

    /** unmark context menu item */
    void
    unmarkItem();

    /** expands the given tree item if expand is true, otherwise the item is collapsed */
    void
    expandItem( TreeItem* item,
                bool      expand );

    /** deselects the given tree item */
    void
    deselectItem( TreeItem* item );

    /** update already calculated items */
    void
    showProgress( int progress );

    void
    itemExpanded( QModelIndex index );
    void
    itemCollapsed( QModelIndex index );

    // ====== context menu =======
    // only used in ManualCalculation mode to manually trigger calculation
    void
    calculateItemNow();
    void
    calculateMarked();
    void
    calculateSelectedItemsNow();
    void
    calculateItem();
    void
    calculateSelectedItems();
    void
    disableAutomaticCalculation();

    // end only used in ManualCalculation mode

    void
    onCollapseAll();
    void
    onCustomContextMenu( const QPoint& point );
    void
    contextMenuClosed();
    void
    onExpandAll();
    void
    onExpandLevel();
    void
    onExpandSubtree();
    void
    onExpandLargest();
    void
    onExpandMarked();
    void
    onCollapseSubtree();
    void
    onCopyToClipboard();
    void
    onHidingThreshold();
    void
    onHiding( bool askThreshold = true );
    void
    onHideItem();
    void
    onUnhideItem();
    void
    onDisableHiding();
    void
    onFindItems();
    void
    onUnmarkItems();
    void
    onInfo();
    void
    onUnhideTreeLevel();
    void
    onHideTreeLevel();
    void
    onSortByName();
    void
    onSortByInclusiveValue( bool always = true );
    void
    onSortByExclusiveValue( bool always = true );
    void
    onSortByInclusiveValueOnce();
    void
    onSortByExclusiveValueOnce();
    void
    onSortByNameAndNumber();
    void
    onDisableSorting();
    bool
    synchronizationIsDefault();
    void
    handleFoundItems( const QString& searchText,
                      FindAction     action );
    void
    closeFindWidget();

    void
    nextFoundItem();
    void
    prevFoundItem();

protected:
    QWidget*            containerWidget;
    QIcon               icon_;
    QString             tabLabel;
    TreeModelInterface* modelInterface;
    QList<QAction*>     pluginActions;    // menu items of the context menu which have been created by plugins
    QMenu*              contextMenu;      // top level context menu of tree items
    TreeItem*           contextMenuItem;  // item on which the context menu is activated
    QModelIndex         contextMenuIndex; // index on which the context menu is activated
    bool                contextIsInvalid; // true, if a child and its parent are selected
    bool                multipleContext;  // true, if the contextMenuItem is part of more than one selected items
    double              userMinValue;     // user defined minimum value
    double              userMaxValue;     // user defined maximum value

    enum ContextMenuAction
    {
        TreeItemInfo, ExpandMenu, ExpandAll, ExpandLevel, CollapseAll, ExpandClicked, CollapseClicked, ExpandLargest,
        ExpandMarked, CollapseSubtree, ExpandSubtree, FindItems, ClearFound, CopyClipboard,
        HidingThreshold, DynamicHiding, StaticHiding, NoHiding, HideItem, UnhideItem, Documentation,
        SortByName, SortByNameNumber, SortByInclusiveValue, SortByExclusiveValue, DisableSorting, SortByExclusiveValueOnce,
        SortByInclusiveValueOnce, SortingMenu, UserDefinedMinMax, HideTreeLevel, UnhideTreeLevel
    };
    QHash <ContextMenuAction, QAction*> contextMenuHash;

    virtual void
    fillContextMenu();

    /** return false, if additional items cannot not be selected, eg. overwritten by metric tree to check if the units are compatible */
    virtual bool
    selectionIsValid( QList<TreeItem*>& list,
                      TreeItem* );
    QString
    getInfoString( TreeItem* item );

    void
    addExpertContextMenuOptions();

    QString
    modelIndexToPath( const QModelIndex& idx ) const;
    QModelIndex
    convertQVariantToIndex( QList<QVariant> list );

    static QList<QVariant>
    convertIndexToQVariant( QModelIndex idx );

private:
    double                       hidingThreshold; // save user input for next input dialog
    TreeItemDelegate*            delegate;        // paints the tree items
    bool                         hasHiddenLevel;
    QList<QPersistentModelIndex> foundItems;
    bool                         markSelected; // mark selected items, if more than MAX_EXPANDED items were selected
    FindWidget*                  findWidget;
    int                          currentFoundIndex;
    QElapsedTimer                timer;
    int                          progress;
    int                          updateTime; // time required for previous update of tree items

    void
    generateContextActions();
    void
    expandSubtree( TreeItem* item,
                   bool      expand );
    void
    expandLargest( TreeItem* item );
    double
    getHidingThreshold( TreeItem* item,
                        bool&     ok );
    void
    updateContextMenu();
    void
    setCollapsedExpanded( const QList<QModelIndex>& changed );
    void
    getExpandedTreeItemList( const QList<QVariant>& expandedItemsText,
                             QList<QModelIndex>&    expandedItems );
    void
    getExpandedTreeItemList( QList<QVariant>&    expandedItemsText,
                             QModelIndex         current,
                             int                 level,
                             QList<QModelIndex>& expandedItems );

    QList<QModelIndex>
    getCollapseExpandChanges( const QList<QVariant>& expandedItems );
    void
    updateSelectionStatus();
    void
    setSortAlways( TreeModelProxy* model,
                   bool            always );
    void
    checkCurrentSelection();
};
}
#endif // TREEVIEW_H

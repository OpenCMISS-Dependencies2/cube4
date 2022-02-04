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

#include <QAction>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QApplication>
#include <QInputDialog>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QScrollBar>
#include "Compatibility.h"
#ifdef HAS_QREGULAR_EXPRESSION
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

#include "Future.h"
#include "CubeProxy.h"
#include "CubeRegion.h"
#include "TreeView.h"
#include "Tree.h"
#include "TreeModel.h"
#include "TreeItem.h"
#include "PluginManager.h"
#include "Globals.h"
#include "HelpBrowser.h"
#include "Environment.h"
#include "InfoWidget.h"
#include "TreeModelProxy.h"
#include "SettingsHandler.h"
#include "HmiInstrumentation.h"
#include "TreeItemMarker.h"
#include "ValueView.h"
#include "TabManager.h"

using namespace cubegui;

TreeView::TreeView( TreeModelInterface* model,
                    const QString&      tabLabel ) : QTreeView()
{
    containerWidget = 0;
    findWidget      = 0;

    hasHiddenLevel       = false;
    markSelected         = false;
    hidingThreshold      = -1;
    this->icon_          = QIcon();
    this->tabLabel       = tabLabel;
    this->modelInterface = model;
    this->delegate       = new TreeItemDelegate();
    this->setModel( model->getModel() );
    this->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    this->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    this->setSelectionMode( QAbstractItemView::ExtendedSelection );
    this->header()->hide();

    this->setItemDelegate( delegate );
    this->setAnimated( false );

    connect( this, SIGNAL( expanded( QModelIndex ) ), this, SLOT( itemExpanded( QModelIndex ) ) );
    connect( this, SIGNAL( collapsed( QModelIndex ) ), this, SLOT( itemCollapsed( QModelIndex ) ) );

    // context menu
    this->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this, SIGNAL( customContextMenuRequested( const QPoint & ) ),
             this, SLOT( onCustomContextMenu( const QPoint & ) ) );
    contextMenuItem = 0;
    contextMenu     = 0;
    generateContextActions();
    // end context menu

    this->setUniformRowHeights( true ); // improves performance

    // set the column width to fit the contents and set correct scrollbars
    this->header()->setStretchLastSection( false );
#if QT_VERSION >= 0x050000
    this->header()->setSectionResizeMode( QHeaderView::ResizeToContents );
#else
    this->header()->setResizeMode( QHeaderView::ResizeToContents );
#endif
}

TreeView::~TreeView()
{
    foreach( QAction * action, pluginActions )
    {
        delete action;
    }
    delete contextMenu;
    delete delegate;
}

/**
 * @brief selectionIsValid checks if newItem can be added to the current selection
 * @param oldItems previously selected tree items
 * @param newItem item that should become selected
 * @return true, if the item can be added to the current selection
 */
bool
TreeView::selectionIsValid( QList<TreeItem*>&, TreeItem* )
{
    /*
       // multiple selection may not be allowed if derived metric is selected
       if ( oldItems.size() > 0 && getTree()->getType() != METRIC )
       {
        Tree*     metricTree = Globals::getTabManager()->getTree( METRICTREE );
        TreeItem* metricItem = metricTree->getLastSelection();

        if ( metricItem != NULL && metricItem->isDerivedMetric() )
        {
            cube::Metric* metric = dynamic_cast<cube::Metric*> ( metricItem->getCubeObject() );
            if ( metric->get_type_of_metric() == cube::CUBE_METRIC_POSTDERIVED )
            {
                QString message( "Multiple selection of items is not possible, if postderived metric is selected." );
                Globals::setStatusMessage( message, Error );
                return false;
            }
            else
            if ( ( metric->getAggrPlusEvaluation() != 0 ) || ( metric->getAggrMinusEvaluation() != 0 ) || ( metric->getAggrAggrEvaluation() != 0 ) )
            {
                QString message( "Multiple selection of items is not possible for the selected prederived metric." );
                Globals::setStatusMessage( message, Error );
                return false;
            }
        }
       }
     */
    return true;
}

/** overwrite mousePressEvent to disable selection of tree items with the right mouse button
 * right mouse button should only pop up the context menu
 */
void
TreeView::mousePressEvent( QMouseEvent* event )
{
    if ( event->button() != Qt::RightButton )
    {
        // also ignore mouse clicks on white space (index is not valid)
        QModelIndex index = this->indexAt( event->pos() );
        if ( index.isValid() )
        {
            QTreeView::mousePressEvent( event );
        }
    }
}

// ----------- public slots --------------------
/** selects the given item in model and view */
void
TreeView::selectItem( TreeItem* item, bool addToSelection )
{
    QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::Select;
    if ( !addToSelection )
    {
        flags |= QItemSelectionModel::Clear;
    }
    QPersistentModelIndex index = QPersistentModelIndex( modelInterface->find( item ) );
    if ( !selectionModel()->isSelected( index ) )
    {
        selectionModel()->select( QItemSelection( index, index ), flags );
    }
}

/**
 * @brief TreeView::selectionChanged is a virtual function from QTreeView, that is connected to the selectionChanged signal
 * of QItemSelectionModel.
 * @param iselected the new selections
 * @param ideselected the deselected items
 */
void
TreeView::selectionChanged( const QItemSelection& iselected, const QItemSelection& ideselected )
{
    START_TRANSITION();

    if ( iselected.indexes().size() == 0 && ideselected.indexes().size() == 0 )
    {
        return;                                            // With Qt 5.7, the parameters are empty lists for the first call ( = nothing has changed )
    }

    QModelIndexList allSelected = this->selectedIndexes(); // all currently selected items
    QTreeView::selectionChanged( iselected, ideselected );
    QModelIndexList selected   = iselected.indexes();      // recently selected items
    QModelIndexList deselected = ideselected.indexes();

    if ( allSelected.isEmpty() )  // at least one item has to be selected -> don't allow one to deselect solely selected item
    {
        QModelIndex index = deselected.last();
        selectionModel()->blockSignals( true );
        selectionModel()->select( QItemSelection( index, index ), QItemSelectionModel::Select );
        selectionModel()->blockSignals( false );
        allSelected.append( index );
        return;
    }

    if ( Globals::optionIsSet( ExperimentalMode ) && getTree()->getType() == METRIC )
    {
        foreach( QModelIndex idx, deselected )
        {
            TreeItem*     item   = modelInterface->getTreeItem( idx );
            cube::Metric* metric = static_cast<cube::Metric*>( item->cubeObject );
            getTree()->getCube()->dropAllRowsInMetric( metric );
            qDebug() << "TreeView::drop unselected metric " << item->getLabel();
        }
    }

    bool invalidSelection = false;
    // check if selection is valid, otherwise unselect recently selected item
    // invalid selections are for example metrics with different units
    if ( selected.size() > 0 && allSelected.size() > 1 )
    {
        QList<TreeItem*> oldSelections = getTree()->getSelectionList();
        // remove deselected items from the list of the previous selections
        foreach( QModelIndex idx, deselected )
        {
            TreeItem* item = modelInterface->getTreeItem( idx );
            oldSelections.removeOne( item );
        }
        foreach( QModelIndex index, selected )
        {
            TreeItem* newSelection = modelInterface->getTreeItem( index );
            if ( !selectionIsValid( oldSelections, newSelection ) )
            {
                invalidSelection = true;
                selected.removeOne( index );
                allSelected.removeOne( index );
                selectionModel()->blockSignals( true );
                selectionModel()->select( QItemSelection( index, index ), QItemSelectionModel::Deselect ); // emits selectionChanged
                selectionModel()->blockSignals( false );
            }
            else
            {
                oldSelections.append( newSelection );
            }
        }
    }

    QList<TreeItem*> selectedList;
    foreach( QModelIndex listIndex, allSelected )
    {
        TreeItem* item = modelInterface->getTreeItem( listIndex );
        selectedList.append( item );
    }

    // update internal list of selected items: deselect all items and select all items which are selected in the view
    getTree()->setSelectionList( selectedList );
    if ( !invalidSelection )
    {
        TreeItem* item = modelInterface->getTreeItem( allSelected.last() );
        getTree()->setLastSelection( item );
    }

    modelInterface->markSelectedParents( this->selectedIndexes() );
    dataChanged( QModelIndex(), QModelIndex() ); // required to update the background of selected parents

    // update status line with selection info
    if ( selected.size() > 0 )
    {
        QString line;
        if ( selected.size() == 1 )
        {
            TreeItem* item = modelInterface->getTreeItem( allSelected.last() );
            line = QString( tr( "Selected \"" ) ).append( item->getOriginalName() ).append( "\"" );
        }
        else
        {
            line = QString( tr( "Selected " ) ).append( QString::number( allSelected.size() ) ).append( tr( " items" ) );
        }
        Globals::setStatusMessage( line );
    }
    else if ( deselected.size() > 0 )
    {
        TreeItem* item = modelInterface->getTreeItem( deselected.last() );
        Globals::setStatusMessage( QString( tr( "Deselected \"" ) ).append( item->getOriginalName() ).append( "\"" ) );
    }

    emit selectionChanged( getTree() );          // notify tab manager to recalculate tree values

    // notify plugins
    if ( !invalidSelection && this->isVisible() )
    {
        TreeItem* item = modelInterface->getTreeItem( allSelected.last() );
        PluginManager::getInstance()->treeItemIsSelected( item );
    }

    END_TRANSITION();
}

QSize
TreeView::sizeHint() const
{
    // return width of the tree drawing + scollbar width
    int scrollBarWidth = this->verticalScrollBar()->width();
    return QSize( sizeHintForColumn( DATA_COL ) + scrollBarWidth, 1 );
}

void
TreeView::paintEvent( QPaintEvent* event )
{
    // set TreeItemMarker icons to correct height
    QFontMetrics metrics = QFontMetrics( font() );
    int          height  = metrics.ascent() + metrics.descent();
    PluginManager::getInstance()->resizeTreeItemMarker( height );

    QTreeView::paintEvent( event );
}

void
TreeView::deselectItem( TreeItem* item )
{
    QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::Deselect;

    selectItem( item, flags );
}

void
TreeView::expandItem( TreeItem* item, bool expand )
{
    QModelIndex index = modelInterface->find( item );
    setExpanded( index, expand );
}

// ----------- end public slots --------------------

void
TreeView::valueModusChanged( ValueModus modus )
{
    getTree()->valueModusChanged( modus );
    modelInterface->updateValues();
}

void
TreeView::setActive( bool active )
{
    if ( active )
    {
        emit recalculateRequest( getTree() );
    }
}

Tree*
TreeView::getTree() const
{
    return modelInterface->getTree();
}

/**
 * called if the user has expanded an item
   // * @param index
 */
void
TreeView::itemExpanded( QModelIndex index )
{
    START_TRANSITION();

    TreeItem* item = modelInterface->getTreeItem( index );
    item->setExpandedStatus( true );

    if ( item->isSelected() ) // selected item is expanded -> recalculate
    {
        selectionChanged( QItemSelection( index, index ), QItemSelection() );
    }
    emit itemExpanded( item, true );

    END_TRANSITION();
}

void
TreeView::itemCollapsed( QModelIndex parentIndex )
{
    TreeItem* item = modelInterface->getTreeItem( parentIndex );
    item->setExpandedStatus( false );

    QItemSelection selection;
    bool           childIsSelected = false;
    {   // recursively search selected children of collapsed item
        QModelIndexList indexList;
        indexList.append( parentIndex );
        while ( !indexList.isEmpty() )
        {
            QModelIndex index = indexList.takeFirst();
            if ( selectionModel()->isSelected( index ) )
            {
                childIsSelected = true;
                selection.append( QItemSelection( index, index ) );
            }
            // check children
            int count = model()->rowCount( index );
            for ( int i = 0; i < count; ++i )
            {
                indexList.append( model()->index( i, DATA_COL, index ) );
            }
        }
    } // end search

    if ( childIsSelected ) // deselect all selected children and select the collapsed item
    {
        selectionModel()->blockSignals( true );
        selectionModel()->select( selection, QItemSelectionModel::Deselect );
        selectionModel()->select( QItemSelection( parentIndex, parentIndex ), QItemSelectionModel::Select );
        selectionModel()->blockSignals( false );
    }

    if ( item->isSelected() || childIsSelected ) // selected item is collapsed -> recalculate
    {
        selectionChanged( QItemSelection( parentIndex, parentIndex ), QItemSelection() );
    }
    emit itemExpanded( item, false );
}

/*
 * thee modifications (hiding/filtering) may cause that the status of hidden elements gets lost in the view which causes model and
 * view to become inconsistent => set collapsed the status from model
 */
void
TreeView::updateExpandedCollapsedStatus()
{
    this->blockSignals( true );                        // optimization
    QModelIndexList indexList;
    indexList.append( QModelIndex() );

    while ( !indexList.isEmpty() )                     // recursively check all items
    {
        QModelIndex index = indexList.takeFirst();
        TreeItem*   item  = modelInterface->getTreeItem( index );
        setExpanded( index, item->isExpanded() );

        if ( item->isExpanded() )
        {
            // check children
            int count = model()->rowCount( index );
            for ( int i = 0; i < count; ++i )
            {
                indexList.append( model()->index( i, DATA_COL, index ) );
            }
        }
    } // end recursively update expanded status
    this->blockSignals( false );
}


/** converts a list of TreeItems to a list of QModelIndex
 */
QModelIndexList
TreeView::convertToModelIndexList( QList<TreeItem*> items )
{
    QModelIndexList indexList;

    QModelIndexList tmpList;
    tmpList.append( QModelIndex() ); // start with root and traverse all elements
    QAbstractItemModel* imodel = model();
    while ( !tmpList.isEmpty() && !items.isEmpty() )
    {
        QModelIndex parent = tmpList.takeFirst();
        int         rows   = imodel->rowCount( parent );
        for ( int i = 0; i < rows; ++i )
        {
            QModelIndex idx  = imodel->index( i, DATA_COL, parent );
            TreeItem*   item = modelInterface->getTreeItem( idx );

            bool found = items.removeOne( item );
            if ( found )
            {
                indexList.append( idx );
            }
            tmpList.append( idx ); // check children
        }
    }
    return indexList;
}

/*
 * thee modifications (hiding) may cause that the status of selected elements gets lost in the view which causes model and
 * view to become inconsistent => set selected status from model
 */
void
TreeView::updateSelectionStatus()
{
    // Convert list of TreeItems to QItemSelection to select them at once. Selecting each item separately is too slow.
    QItemSelection   selection;
    QList<TreeItem*> selectedItems = getTree()->getSelectionList();

    QModelIndexList tmpList;
    tmpList.append( QModelIndex() ); // start with root and traverse all elements
    QAbstractItemModel* imodel = model();
    while ( !tmpList.isEmpty() && !selectedItems.isEmpty() )
    {
        QModelIndex parent = tmpList.takeFirst();
        int         rows   = imodel->rowCount( parent );
        for ( int i = 0; i < rows; ++i )
        {
            QModelIndex idx  = imodel->index( i, DATA_COL, parent );
            TreeItem*   item = modelInterface->getTreeItem( idx );

            bool found = selectedItems.removeOne( item );
            if ( found )
            {
                selection.select( idx, idx );
            }
            tmpList.append( idx ); // check children
        }
    }

    selectionModel()->blockSignals( true ); // selection hasn't changed -> no calculation required
    selectionModel()->select( selection, QItemSelectionModel::Select );
    selectionModel()->blockSignals( false );
}


/**
 * mark the selections from the model in the view
 * this is usually automatically done with signal itemSelected
 */
void
TreeView::filteringEnded()
{
    /*
     * After filtering, the expanded/collapsed status of hidden elements gets lost in the view which causes model and
     * view to become inconsistent => set collapsed the status from model after filtering is done
     */
    updateExpandedCollapsedStatus();
    emit selectionChanged( getTree() ); // value of selected items may have changed due to added hidden values
}

void
TreeView::setIcon( const QIcon& icon )
{
    icon_ = icon;
}

QWidget*
TreeView::widget()
{
    if ( !containerWidget )
    {
        // create a container widget which consists of the tree view and the search widget
        containerWidget = new QWidget;
        QVBoxLayout* vlayout = new QVBoxLayout();
        vlayout->setContentsMargins( 0, 0, 0, 0 );
        containerWidget->setLayout( vlayout );
        containerWidget->layout()->addWidget( this );
    }
    return containerWidget;
}

QString
TreeView::label() const
{
    return tabLabel;
}

QIcon
TreeView::icon() const
{
    return icon_;
}

void
TreeView::updateValues()
{
    modelInterface->updateValues();
    updateInfoWidget();
}


QList<QVariant>
TreeView::convertIndexToQVariant( QModelIndex idx )
{
    QList<QVariant> list;
    do
    {
        list.prepend( idx.row() );
        idx = idx.parent();
    }
    while ( idx.isValid() );

    return list;
}

QModelIndex
TreeView::convertQVariantToIndex( QList<QVariant> list )
{
    QModelIndex elem;
    foreach( QVariant row, list )
    {
        elem = model()->index( row.toInt(), DATA_COL, elem );
    }
    return elem;
}

// ============= settings ===========================
void
TreeView::loadExperimentSettings( QSettings& settings )
{
    // traverse the tree to get the expanded items
    QList<QVariant> expandedItems = settings.value( "expandedItems" ).toList();
    foreach( QVariant var, expandedItems )
    {
        QModelIndex elem = convertQVariantToIndex( var.toList() );
        if ( elem.isValid() ) // select the item, if valid
        {
            expand( elem );
        }
    }

    // traverse the tree to get the selected items
    QList<QVariant> selectedItems = settings.value( "selectedItems" ).toList();
    QItemSelection  itemSelection;
    foreach( QVariant var, selectedItems )
    {
        QModelIndex elem = convertQVariantToIndex( var.toList() );
        if ( elem.isValid() ) // select the item, if valid
        {
            QItemSelection sel = QItemSelection( elem, elem );
            itemSelection.merge( sel, QItemSelectionModel::Select );
        }
    }
    if ( itemSelection.isEmpty() && getTree()->getItems().size() > 0 )
    {
        // no valid selection -> select first item
        QModelIndex    first = model()->index( 0, DATA_COL, QModelIndex() );
        QItemSelection sel   = QItemSelection( first, first );
        itemSelection.append( sel );
    }
    if ( !itemSelection.isEmpty() )
    {
        this->blockSignals( true ); // only set selection, don't notify
        selectionModel()->select( itemSelection, QItemSelectionModel::ClearAndSelect );
        this->blockSignals( false );
    }
}

void
TreeView::saveExperimentSettings( QSettings& settings )
{
    QAbstractItemModel* imodel = model();

    // save expanded items
    QList<QVariant>    expandedItems;
    QList<QModelIndex> list;
    QModelIndex        root = QModelIndex();
    QModelIndex        idx;
    list.append( root );
    while ( !list.isEmpty() )
    {
        QModelIndex parent = list.takeFirst();
        int         rows   = imodel->rowCount( parent );
        for ( int i = 0; i < rows; ++i )
        {
            idx = imodel->index( i, DATA_COL, parent );
            if ( isExpanded( idx ) )
            {
                expandedItems.append( QVariant( convertIndexToQVariant( idx ) ) );
                list.append( idx ); // check children
            }
        }
        settings.setValue( "expandedItems", QVariant( expandedItems ) );
    }
    // end save expanded items

    // save selected items
    QList<QVariant> selectedItems;
    foreach( QModelIndex idx, selectedIndexes() )
    {
        selectedItems.append( QVariant( convertIndexToQVariant( idx ) ) );
    }
    settings.setValue( "selectedItems", QVariant( selectedItems ) );
}

void
TreeView::loadStatus( QSettings& settings )
{
    if ( !isVisible() )
    {
        return;
    }

    // expand/collapse items
    if ( settings.allKeys().size() > 0 )
    {
        QList<QVariant>    expandedItems = settings.value( "expandedItems" ).toList();
        QList<QModelIndex> changed       = getCollapseExpandChanges( expandedItems );
        setCollapsedExpanded( changed );
    }

    // traverse the tree to get the selected items
    QList<TreeItem*> currentSelections = getTree()->getSelectionList();

    QList<QVariant> newSelections = settings.value( "selectedItems" ).toList();
    QItemSelection  itemSelection;

    bool selectionsDiffer = false;
    foreach( QVariant path, newSelections )
    {
        QModelIndex elem = modelInterface->findPath( path.toString() );
        TreeItem*   item = modelInterface->getTreeItem( elem );
        if ( !currentSelections.removeOne( item ) )
        {
            selectionsDiffer = true;
            break;
        }
    }
    if ( !currentSelections.isEmpty() )
    {
        selectionsDiffer = true;
    }

    if ( !selectionsDiffer )
    {
        return; // selections havn't changed
    }

    // activate selections
    foreach( QVariant path, newSelections )
    {
        QModelIndex elem = modelInterface->findPath( path.toString() );

        if ( elem.isValid() ) // select the item, if valid
        {
            QItemSelection sel = QItemSelection( elem, elem );
            itemSelection.merge( sel, QItemSelectionModel::Select );
        }
    }
    if ( !itemSelection.isEmpty() )
    {
        selectionModel()->select( itemSelection, QItemSelectionModel::ClearAndSelect );
    }
}

QString
TreeView::settingName()
{
    return getTree()->getLabel();
}

void
TreeView::setCollapsedExpanded( const QList<QModelIndex>& changed )
{
    foreach( QModelIndex idx, changed )
    {
        TreeItem* item = modelInterface->getTreeItem( idx );
        item->setExpanded( !item->isExpanded() );
        if ( item->isExpanded() )
        {
            expand( idx );
            emit itemExpanded( item, true );
        }
        else
        {
            collapse( idx );
            emit itemExpanded( item, false );
        }
    }

    //this->blockSignals( false );
    //emit itemExpanded( getTree()->getRootItem(), true );
}

/**
 * @brief TreeView::getCollapseExpandChanges retreives tree items whose state has changed into collapsed or expanded
 * @param expandedItemsText data read from QSettings in readStatus()
 * @return index list of all changed items
 */
QList<QModelIndex>
TreeView::getCollapseExpandChanges( const QList<QVariant>& expandedItemsText )
{
    // converts the list from QSettings to a list of QModelIndex
    QList<QModelIndex> expandedItems;
    getExpandedTreeItemList( expandedItemsText, expandedItems );

    QList<QModelIndex> changedItems;
    QModelIndexList    indexList;
    indexList.append( QModelIndex() ); // root
    while ( !indexList.isEmpty() )     // recursively check all items
    {
        QModelIndex index            = indexList.takeFirst();
        int         numberOfChildren = model()->rowCount( index );

        if ( numberOfChildren == 0 )
        {
            continue;
        }

        TreeItem* item = modelInterface->getTreeItem( index );

        if ( expandedItems.contains( index ) )
        {
            if ( !item->isExpanded() )  // found change from collapsed to expanded
            {
                changedItems.append( index );
            }
            expandedItems.removeOne( index );
        }
        else if ( item->isExpanded() )
        {
            changedItems.append( index ); // found change expanded to collapsed
        }

        // check children
        for ( int i = 0; i < numberOfChildren; ++i )
        {
            indexList.append( model()->index( i, DATA_COL, index ) );
        }
    } // end recursively check

    return changedItems;
}

// converts a list of items saved in QSettings (@see saveStatus) to a list of QModelIndex
void
TreeView::getExpandedTreeItemList( const QList<QVariant>& expandedItemsText, QList<QModelIndex>& expandedItems )
{
    QList<QVariant> expandedItemsTextTmp = expandedItemsText;
    getExpandedTreeItemList( expandedItemsTextTmp, QModelIndex(), 1, expandedItems );
}

void
TreeView::getExpandedTreeItemList( QList<QVariant>& expandedItemsText, QModelIndex current, int level,
                                   QList<QModelIndex>& expandedItems )
{
    // find expanded items and set the status to expanded in the model, the remaining items are collapsed
    int rows = model()->rowCount( current );
    for ( int i = 0; i < rows && !expandedItemsText.isEmpty(); ++i )
    {
        QModelIndex idx  = model()->index( i, DATA_COL, current );
        TreeItem*   item = modelInterface->getTreeItem( idx );

        foreach( QVariant path, expandedItemsText )
        {
            QStringList elems = path.toString().split( ":::" );
            if ( elems.size() == level )
            {
                TreeItem* it    = item;
                bool      found = true;
                for ( int i = elems.size() - 1; i >= 0; i-- ) // check if complete path is equal
                {
                    if ( elems[ i ] != it->getName() )
                    {
                        found = false;
                        break;
                    }

                    // go to next non-hidden parent item
                    bool hidden = true;
                    while ( hidden )
                    {
                        it     = it->getParent();
                        hidden = it->getTree()->getModel()->isHiddenLevel( it->getDepth() );
                        if ( !it )
                        {
                            found = false;
                            break;
                        }
                    }
                }

                if ( found )
                {
                    expandedItemsText.removeOne( path );
                    expandedItems.append( idx );
                    getExpandedTreeItemList( expandedItemsText, idx, level + 1, expandedItems );
                    break;
                }
            }
        }
    }
}

QString
TreeView::modelIndexToPath( const QModelIndex& idx ) const
{
    TreeItem* item = modelInterface->getTreeItem( idx );
    QString   itemLine;
    while ( item && item->getParent() )                                        // invisible root item is not included
    {
        if ( !item->getTree()->getModel()->isHiddenLevel( item->getDepth() ) ) // ignore hidden level
        {
            itemLine = item->getName() + ":::" + itemLine;
        }
        item = item->getParent();
    }
    itemLine.remove( itemLine.length() - 3, 3 );

    return itemLine;
}

void
TreeView::saveStatus( QSettings& settings )
{
    if ( !isVisible() )
    {
        return;
    }

    QAbstractItemModel* imodel = model();

    // save expanded items
    QList<QVariant>    expandedItems;
    QList<QModelIndex> list;
    QModelIndex        root = QModelIndex();
    QModelIndex        idx;
    list.append( root );
    while ( !list.isEmpty() )
    {
        QModelIndex parent = list.takeFirst();
        int         rows   = imodel->rowCount( parent );
        for ( int i = 0; i < rows; ++i )
        {
            idx = imodel->index( i, DATA_COL, parent );
            if ( isExpanded( idx ) )
            {
                expandedItems.append( QVariant( modelIndexToPath( idx ) ) );
                list.append( idx ); // check children
            }
        }
        settings.setValue( "expandedItems", QVariant( expandedItems ) );
    }
    // end save expanded items

    // save selected items
    QList<QVariant> selectedItems;
    foreach( QModelIndex idx, selectedIndexes() )
    {
        selectedItems.append( QVariant( modelIndexToPath( idx ) ) );
    }
    settings.setValue( "selectedItems", QVariant( selectedItems ) );
}

bool
TreeView::synchronizationIsDefault()
{
    return true;
}


// / ============= end settings =======================

// / ============= begin context menu =======================

static QString
getUrl( TreeItem* item )
{
    // we must distinguish on the item type to call the right get method
    if ( item->getCubeObject() == NULL )
    {
        return QString( "" );
    }


    if ( item->getType() == METRICITEM )
    {
        return QString::fromStdString( ( ( cube::Metric* )( item->getCubeObject() ) )->get_url() );
    }
    else if ( item->getType() == CALLITEM )
    {
        return QString::fromStdString( ( ( cube::Cnode* )( item->getCubeObject() ) )->get_callee()->get_url() );
    }
    else if ( item->getType() == REGIONITEM && item->getCubeObject() != NULL )
    {
        return QString::fromStdString( ( ( cube::Region* )( item->getCubeObject() ) )->get_url() );
    }

    return QString();
}

void
TreeView::insertPluginMenuItem( QAction* pluginAction )
{
    pluginActions.append( pluginAction );
}

/**
 * @brief TreeView::updateContextMenu enables or disables menu items depending on
 * the item the context menu is called on
 */
void
TreeView::updateContextMenu()
{
    //QString description = getContextDescription( contextMenuItem );
    //contextMenuHash.value( TreeItemInfo )->setEnabled( description.isEmpty() ? false : true );

    QString url = getUrl( contextMenuItem ).trimmed();
    contextMenuHash.value( Documentation )->setEnabled( url.isEmpty() ? false : true );
}

void
TreeView::onCustomContextMenu( const QPoint& point )
{
    contextMenu     = new QMenu();
    contextMenuItem = 0;
    connect( contextMenu, SIGNAL( destroyed() ), this, SLOT( contextMenuClosed() ) );

    QModelIndex index = this->indexAt( point );
    if ( index.isValid() ) // visible item selected
    {
        contextMenuIndex = index;
        contextMenuItem  = modelInterface->getTreeItem( index );
    }

    contextIsInvalid = false; // true, if a child and its parent are selected
    multipleContext  = false; // true, if the contextMenuItem is part of more than one selected items

    // check if context menu item is part of a group of selected items and if the selection is valid
    const QList<TreeItem*> selectedItems = getTree()->getSelectionList();
    foreach( TreeItem * sel, selectedItems )
    {
        if ( !contextIsInvalid )
        {
            TreeItem* parent = sel;
            while ( ( parent = parent->getParent() ) )
            {
                if ( selectedItems.contains( parent ) )
                {
                    contextIsInvalid = true;
                }
            }
        }
        if ( sel == contextMenuItem && selectedItems.size() > 1 )
        {
            multipleContext = true;
        }
    }

    if ( contextMenuItem ) // enable all generic context menu items
    {
        foreach( QAction * action, contextMenuHash.values() )
        {
            action->setEnabled( true );
        }
    }
    else // disable all generic context menu items that depend on choosen item
    {
        foreach( QAction * action, contextMenuHash.values() )
        {
            action->setEnabled( false );
        }
        // enable context menu actions which don't depend on a choosen item
        contextMenuHash.value( ExpandAll )->setEnabled( true );
        contextMenuHash.value( ExpandMenu )->setEnabled( true );
        contextMenuHash.value( ExpandMarked )->setEnabled( true );
        contextMenuHash.value( CollapseAll )->setEnabled( true );
        contextMenuHash.value( FindItems )->setEnabled( true );
        contextMenuHash.value( ClearFound )->setEnabled( true );
        contextMenuHash.value( CopyClipboard )->setEnabled( true );
        contextMenuHash.value( SortingMenu )->setEnabled( true );
        contextMenuHash.value( SortByName )->setEnabled( true );
        contextMenuHash.value( SortByNameNumber )->setEnabled( true );
        contextMenuHash.value( SortByInclusiveValue )->setEnabled( true );
        contextMenuHash.value( SortByExclusiveValue )->setEnabled( true );
        contextMenuHash.value( DisableSorting )->setEnabled( true );
        contextMenuHash.value( SortByInclusiveValueOnce )->setEnabled( true );
        contextMenuHash.value( SortByExclusiveValueOnce )->setEnabled( true );
        contextMenuHash.value( DynamicHiding )->setEnabled( true );
        contextMenuHash.value( StaticHiding )->setEnabled( true );
        contextMenuHash.value( NoHiding )->setEnabled( true );
    } // end context free

    contextMenuHash.value( UnhideTreeLevel )->setEnabled( hasHiddenLevel );

    fillContextMenu();

    if ( contextMenuItem )
    {
        updateContextMenu();
    }

    // add plugin context menu items
    PluginManager::getInstance()->contextMenuIsShown( getTree()->getType(), contextMenuItem );
    foreach( QAction * pluginAction, pluginActions )
    {
        contextMenu->addAction( pluginAction );
    }

    // QContextMenuEvent * event = new  QContextMenuEvent( QContextMenuEvent::Mouse ,point,this->mapToGlobal( point ) );
    // QApplication::postEvent(this,event);

    // show context menu
    contextMenu->exec( this->mapToGlobal( point ) );
    contextMenu->deleteLater();
}

void
TreeView::contextMenuClosed()
{
    // todo check if all menu items in fillContextMenu are deleted (if contextMenu is owner of the actions)

    // remove and delete previous context menu entries
    foreach( QAction * action, contextMenu->actions() )
    {
        contextMenu->removeAction( action );
        delete action;
    }
    contextMenu->clear();
    pluginActions.clear();
    contextMenu     = 0; // used deleteLater to free memory
    contextMenuItem = 0;
}

void
TreeView::fillContextMenu()
{
    addExpertContextMenuOptions();
    contextMenu->addAction( contextMenuHash.value( TreeItemInfo ) );
    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( ExpandMenu ) );
    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( FindItems ) );
    contextMenu->addAction( contextMenuHash.value( ClearFound ) );
    contextMenu->addAction( contextMenuHash.value( SortingMenu ) );
    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( UserDefinedMinMax ) );
    contextMenu->addSeparator();
    contextMenu->addAction( contextMenuHash.value( CopyClipboard ) );
    contextMenu->addSeparator();
}

// / ============= begin context menu actions  =======================

/**
 * The action which are created here are used to build the context menu. Some of the actions are used for
 * all trees.
 */
void
TreeView::generateContextActions()
{
    // ===================================================================================
    // create an action for showing the short metric info of the clicked item,
    // by default disabled, it will be enabled for items for which the info is defined
    QAction* action = new QAction( tr( "Info" ), this );
    action->setStatusTip( tr( "Shows a short description of the clicked item" ) );
    action->setWhatsThis( tr( "For all trees (for call trees under \"Called region\"). Gives some short information about the clicked node. Disabled if you did not click over a node or if no information is available for the clicked node." ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( onInfo() ) );
    connect( InfoWidget::getInstance(), SIGNAL( tabActivated() ), this, SLOT( onInfo() ) );
    contextMenuHash.insert( TreeItemInfo, action );

    // ===================================================================================
    // create an action for expanding all items in the tree
    QAction* expandAllAct = new QAction( tr( "Expand all" ), this );
    expandAllAct->setStatusTip( tr( "Expand all nodes" ) );
    expandAllAct->setWhatsThis( tr( "For all trees. Expands all nodes in the tree." ) );
    connect( expandAllAct, SIGNAL( triggered() ), this, SLOT( onExpandAll() ) );
    contextMenuHash.insert( ExpandAll, expandAllAct );

    QAction* expandLevelAct = new QAction( tr( "Expand current level" ), this );
    expandLevelAct->setStatusTip( tr( "Expand all items of the current level" ) );
    expandLevelAct->setWhatsThis( tr( "For all trees. Expand all items of the current level" ) );
    connect( expandLevelAct, SIGNAL( triggered() ), this, SLOT( onExpandLevel() ) );
    contextMenuHash.insert( ExpandLevel, expandLevelAct );


    // create an action for expanding all items in the subtree of the clicked item
    QAction* expandClickedAct = new QAction( tr( "Expand subtree" ), this );
    expandClickedAct->setStatusTip( tr( "Expand all nodes in the clicked subtree" ) );
    expandClickedAct->setWhatsThis( tr( "For all trees.  Enabled only if you clicked above a node. Expands all nodes in the subtree of the clicked node (inclusively the clicked node)." ) );
    connect( expandClickedAct, SIGNAL( triggered() ), this, SLOT( onExpandSubtree() ) );
    contextMenuHash.insert( ExpandClicked, expandClickedAct );

    // create an action for expanding the clicked item and
    // recursively expanding the child having the largest value,
    // starting from the clicked item untill a leaf item is reached
    QAction* expandLargestAct = new QAction( tr( "Expand largest" ), this );
    expandLargestAct->setStatusTip( tr( "Expand path with largest nodes below clicked" ) );
    expandLargestAct->setWhatsThis( tr( "For all trees.  Enabled only if you clicked above a node. Starting at the clicked node, expands its child with the largest inclusive value (if any), and continues recursively with that child until it finds a leaf. It is recommended to collapse all nodes before using this function in order to be able to see the path along the largest values." ) );
    connect( expandLargestAct, SIGNAL( triggered() ), this, SLOT( onExpandLargest() ) );
    contextMenuHash.insert( ExpandLargest, expandLargestAct );

    QAction* expandMarked = new QAction( tr( "Expand marked" ), this );
    expandMarked->setStatusTip( tr( "Expand all paths with marked nodes" ) );
    expandMarked->setWhatsThis( tr( "For all trees. Expands all child which are marked, and continues recursively with that child until it finds a leaf." ) );
    connect( expandMarked, SIGNAL( triggered() ), this, SLOT( onExpandMarked() ) );
    contextMenuHash.insert( ExpandMarked, expandMarked );

    // create an action for collapsing all items in the tree
    QAction* collapseAllAct = new QAction( tr( "Collapse all" ), this );
    collapseAllAct->setStatusTip( tr( "Collapse all nodes" ) );
    collapseAllAct->setWhatsThis( tr( "For all trees. Collapses all nodes in the tree." ) );
    connect( collapseAllAct, SIGNAL( triggered() ), this, SLOT( onCollapseAll() ) );
    contextMenuHash.insert( CollapseAll, collapseAllAct );

    // create an action for collapsing all items in the subtree of the clicked item
    QAction* collapseClickedAct = new QAction( tr( "Collapse subtree" ), this );
    collapseClickedAct->setStatusTip( tr( "Collapse all nodes in the clicked subtree" ) );
    collapseClickedAct->setWhatsThis( tr( "For all trees.  Enabled only if you clicked above a node. It collapses all nodes in the subtree of the clicked node (inclusively the clicked node)." ) );
    connect( collapseClickedAct, SIGNAL( triggered() ), this, SLOT( onCollapseSubtree() ) );
    contextMenuHash.insert( CollapseClicked, collapseClickedAct );
    // --------------------- end expand/collapse

    // creates an action for marking all items whose name contains a user-defined string
    QAction* findAct = new QAction( tr( "&Find items" ), this );
    findAct->setShortcutContext( Qt::WidgetWithChildrenShortcut );
    findAct->setShortcut( QKeySequence( Qt::CTRL | Qt::Key_F ) );
    this->addAction( findAct );

    findAct->setStatusTip( tr( "Marks all visible items containing the given text" ) );
    //  findAct->setShortcut(tr("Ctrl+F"));
    findAct->setWhatsThis( tr( "For all trees. Opens a dialog to get a text input from the user. If the user called the context menu over an item, the default text is the name of the clicked item, otherwise it is the last text which was searched for.\n\nThe function marks by a yellow background all non-hidden nodes whose names contain the given text, and by a light yellow background all collapsed nodes whose subtree contains such a non-hidden node. The current found node, that is initialized to the first found node, is marked by a distinguishable yellow hue." ) );
    connect( findAct, SIGNAL( triggered() ), this, SLOT( onFindItems() ) );
    contextMenuHash.insert( FindItems, findAct );

    // removes the markings made by findAct
    QAction* removeFindAct = new QAction( tr( "Clear found items" ), this );
    removeFindAct->setStatusTip( tr( "Clears background of all items" ) );
    removeFindAct->setWhatsThis( tr( "For all trees. Removes the background markings of the preceding find functions." ) );
    connect( removeFindAct, SIGNAL( triggered() ), this, SLOT( onUnmarkItems() ) );
    contextMenuHash.insert( ClearFound, removeFindAct );

    QMenu* expandMenu = new QMenu( tr( "Expand/collapse" ) );
    expandMenu->setWhatsThis( tr( "Collapse or expand the tree." ) );
    expandMenu->addAction( contextMenuHash.value( ExpandAll ) );
    expandMenu->addAction( contextMenuHash.value( ExpandClicked ) );
    expandMenu->addAction( contextMenuHash.value( ExpandLargest ) );
    expandMenu->addAction( contextMenuHash.value( ExpandMarked ) );
    expandMenu->addSeparator();
    expandMenu->addAction( contextMenuHash.value( CollapseAll ) );
    expandMenu->addAction( contextMenuHash.value( CollapseClicked ) );
    contextMenuHash.insert( ExpandMenu, expandMenu->menuAction() );

    // ====================================================================================
    // create an action for copying the text of the selected item to the
    // clipboard
    QAction* copyToClipboardAct = new QAction( tr( "Copy to clipboard" ), this );
    copyToClipboardAct->setStatusTip( tr( "Copies the selected node name to the clipboard" ) );
    copyToClipboardAct->setWhatsThis( tr( "For all trees. Copies the text of the selected node to the clipboard." ) );
    connect( copyToClipboardAct, SIGNAL( triggered() ), this, SLOT( onCopyToClipboard() ) );
    contextMenuHash.insert( CopyClipboard, copyToClipboardAct );
    copyToClipboardAct->setShortcut( QKeySequence( Qt::CTRL | Qt::Key_C ) );
    copyToClipboardAct->setShortcutContext( Qt::WidgetWithChildrenShortcut );
    this->addAction( copyToClipboardAct );

    QAction* thresholdAct = new QAction( tr( "Redefine threshold" ), this );
    thresholdAct->setStatusTip( tr( "" ) );
    connect( thresholdAct, SIGNAL( triggered() ), this, SLOT( onHidingThreshold() ) );
    thresholdAct->setWhatsThis( tr( "This function allows one to redefine the hiding threshold for dynamic or static hiding." ) );
    contextMenuHash.insert( HidingThreshold, thresholdAct );

    // create an action for dynamically hiding subtrees with minor values
    QAction* dynamicHidingAct = new QAction( tr( "Dynamic hiding" ), this );
    dynamicHidingAct->setCheckable( true );
    dynamicHidingAct->setStatusTip( tr( "Dynamically hide nodes with minor total values" ) );
    dynamicHidingAct->setWhatsThis( tr( "Activates dynamic hiding. All currently hidden nodes get shown. You are asked to define a percentage threshold between 0.0 and 100.0. All nodes whose color position on the color scale (in percent) is below this threshold get hidden. As default value, the color percentage position of the clicked node is suggested, if you right-clicked over a node. If not, the default value is the last threshold. The hiding is called dynamic, because upon value changes (caused for example by changing the node selection) hiding is re-computed for the new values. With other words, value changes may change the visibility of the nodes.\n\n During dynamic hiding, for expanded nodes with some hidden children and for nodes with all of its children hidden, their displayed (exclusive) value includes the hidden children's inclusive value. After this sum we display in brackets the percentage of the hidden children's value in it." ) );
    connect( dynamicHidingAct, SIGNAL( triggered() ), this, SLOT( onHiding() ) );
    contextMenuHash.insert( DynamicHiding, dynamicHidingAct );

    // action for static hiding of subtrees with minor values
    QAction* statHidAct = new QAction( tr( "Static hiding" ), this );
    statHidAct->setCheckable( true );
    statHidAct->setStatusTip( tr( "Statically hide nodes with minor total values" ) );
    statHidAct->setWhatsThis( tr( "All currently hidden nodes keep being hidden.\n\nLike for dynamic hiding, for expanded nodes with some hidden children and for nodes with all of its children hidden, their displayed (exclusive) value includes the hidden children's inclusive value.  After this sum we display in brackets the percentage of the hidden children's value in it." ) );
    connect( statHidAct, SIGNAL( triggered() ), this, SLOT( onHiding() ) );
    contextMenuHash.insert( StaticHiding, statHidAct );

    //create an action for hiding the node (turns on static hiding)
    QAction* hideAct = new QAction( tr( "   Hide this" ), this );
    hideAct->setStatusTip( tr( "Statically hide the clicked node" ) );
    connect( hideAct, SIGNAL( triggered() ), this, SLOT( onHideItem() ) );
    hideAct->setWhatsThis( tr( "Hides the clicked node. Automatically switches to static hiding mode." ) );
    contextMenuHash.insert( HideItem, hideAct );

    //create an action for unhiding children of the node (turns on static hiding)
    QAction* unhideAct = new QAction( tr( "   Show children of this" ), this );
    unhideAct->setStatusTip( tr( "Show all hidden children of the clicked node" ) );
    connect( unhideAct, SIGNAL( triggered() ), this, SLOT( onUnhideItem() ) );
    unhideAct->setWhatsThis( tr( "Shows all hidden children of the clicked node, if any. Automatically switches to static hiding mode." ) );
    contextMenuHash.insert( UnhideItem, unhideAct );

    // create an action for showing all hidden items
    QAction* noHidingAct = new QAction( tr( "No hiding" ), this );
    noHidingAct->setCheckable( true );
    noHidingAct->setStatusTip( tr( "Switch off hiding and show all hidden items" ) );
    noHidingAct->setWhatsThis( tr( "Deactivates any hiding, and shows all hidden nodes." ) );
    connect( noHidingAct, SIGNAL( triggered() ), this, SLOT( onDisableHiding() ) );
    contextMenuHash.insert( NoHiding, noHidingAct );

    noHidingAct->setChecked( true );
    QActionGroup* hidingGroup = new QActionGroup( this );
    hidingGroup->addAction( dynamicHidingAct );
    hidingGroup->addAction( statHidAct );
    hidingGroup->addAction( noHidingAct );

    //create an action for hiding one level
    QAction* hideLevelAct = new QAction( tr( "Hide current level" ), this );
    hideLevelAct->setStatusTip( tr( "Hide all items of the current level and move children to the parent item" ) );
    hideLevelAct->setWhatsThis( tr( "Hide all items of the current level and move children to the parent item" ) );
    connect( hideLevelAct, SIGNAL( triggered() ), this, SLOT( onHideTreeLevel() ) );
    contextMenuHash.insert( HideTreeLevel, hideLevelAct );
    //create an action for unhiding hidden level
    hideLevelAct = new QAction( tr( "Unhide hidden levels" ), this );
    hideLevelAct->setStatusTip( tr( "Unhide all hidden levels" ) );
    hideLevelAct->setWhatsThis( tr( "Unhide all hidden levels" ) );
    connect( hideLevelAct, SIGNAL( triggered() ), this, SLOT( onUnhideTreeLevel() ) );
    contextMenuHash.insert( UnhideTreeLevel, hideLevelAct );

    // --- info actions
    // create an action for showing the online info for the region of the clicked item,
    // by default disabled, it will be enabled for items for which the url is defined
    action = new QAction( tr( "Documentation" ), this );
    action->setStatusTip( tr( "Shows the documentation of the clicked item" ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( onInfo() ) );
    //connect( action, SIGNAL( triggered() ), this, SLOT( onShowDocumentation() ) ); // todo documentation still needed?
    action->setWhatsThis( tr( "For metric trees, flat call profiles, and call trees. Shows some (usually more extensive) online description for the callee of the clicked node. Disabled if no node is clicked or if no online information is available." ) );
    contextMenuHash.insert( Documentation, action );

    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    if ( model ) // Sorting is only available for models that inherit from TreeModelProxy
    {
        QActionGroup* sortingGroup = new QActionGroup( this );
        QMenu*        sortMenu     = new QMenu( tr( "Sort tree items..." ) );
        contextMenuHash.insert( SortingMenu, sortMenu->menuAction() );

        QAction* nameSort = new QAction( tr( "Sort by name (ascending)" ), this );
        copyToClipboardAct->setStatusTip( tr( "" ) );
        copyToClipboardAct->setWhatsThis( tr( "" ) );
        connect( nameSort, SIGNAL( triggered() ), this, SLOT( onSortByName() ) );
        contextMenuHash.insert( SortByName, nameSort );
        sortingGroup->addAction( nameSort );
        nameSort->setCheckable( true );

        QAction* numberSort = new QAction( tr( "Sort by name and trailing number (ascending)" ), this );
        copyToClipboardAct->setStatusTip( tr( "" ) );
        copyToClipboardAct->setWhatsThis( tr( "" ) );
        connect( numberSort, SIGNAL( triggered() ), this, SLOT( onSortByNameAndNumber() ) );
        contextMenuHash.insert( SortByNameNumber, numberSort );
        sortingGroup->addAction( numberSort );
        numberSort->setCheckable( true );

        if ( getTree()->getType() != SYSTEM )
        {
            sortMenu->addAction( nameSort );
        }
        else
        {
            sortMenu->addAction( numberSort );
        }

        action = new QAction( tr( "Sort by inclusive value (descending)" ), this );
        connect( action, SIGNAL( triggered() ), this, SLOT( onSortByInclusiveValue() ) );
        contextMenuHash.insert( SortByInclusiveValue, action );
        sortMenu->addAction( action );
        sortingGroup->addAction( action );
        action->setCheckable( true );
        action = new QAction( tr( "    Apply now" ), this );
        connect( action, SIGNAL( triggered() ), this, SLOT( onSortByInclusiveValueOnce() ) );
        contextMenuHash.insert( SortByInclusiveValueOnce, action );
        sortMenu->addAction( action );

        action = new QAction( tr( "Sort by exclusive value (descending)" ), this );
        connect( action, SIGNAL( triggered() ), this, SLOT( onSortByExclusiveValue() ) );
        contextMenuHash.insert( SortByExclusiveValue, action );
        sortMenu->addAction( action );
        sortingGroup->addAction( action );
        action->setCheckable( true );
        action = new QAction( tr( "    Apply now" ), this );
        connect( action, SIGNAL( triggered() ), this, SLOT( onSortByExclusiveValueOnce() ) );
        contextMenuHash.insert( SortByExclusiveValueOnce, action );
        sortMenu->addAction( action );

        action = new QAction( tr( "Sort by order of definition" ), this );
        copyToClipboardAct->setStatusTip( tr( "" ) );
        copyToClipboardAct->setWhatsThis( tr( "" ) );
        connect( action, SIGNAL( triggered() ), this, SLOT( onDisableSorting() ) );
        contextMenuHash.insert( DisableSorting, action );
        sortMenu->addAction( action );
        sortingGroup->addAction( action );
        action->setCheckable( true );
        action->setChecked( true );
    }
    // ====================================================================================

    action = new QAction( tr( "Min/max values" ), this );
    connect( action, SIGNAL( triggered() ), this, SLOT( onMinMaxValues() ) );
    action->setStatusTip( tr( "Sets user-defined minimal/maximal values that should correspond to the color extremes." ) );
    action->setWhatsThis( tr( "Sets user-defined minimal/maximal values that should correspond to the color extremes." ) );
    contextMenuHash.insert( UserDefinedMinMax, action );
}

void
TreeView::onExpandAll()
{
    getTree()->getRootItem()->setExpandedStatus( true, true );
    this->blockSignals( true ); // Qt5 sends expanded signal for each expanded item => only send for root item
    expandAll();
    this->blockSignals( false );
    getTree()->invalidateItemLabel();
    emit itemExpanded( getTree()->getRootItem(), true );
}

/** expands all items on the same level as the context menu item */
void
TreeView::onExpandLevel()
{
    contextMenuItem->isExpanded();
    int maxDepth = contextMenuItem->getDepth(); // expand all items of this level

    this->blockSignals( true );                 // don't emit signal for each subitem expand event
    QModelIndexList indexList;
    indexList.append( QModelIndex() );          // start with root item
    while ( !indexList.isEmpty() )
    {
        QModelIndex index = indexList.takeFirst();

        // expand all nodes <= maxDepth
        int count = model()->rowCount( index );
        for ( int i = 0; i < count; ++i )
        {
            QModelIndex idx  = model()->index( i, DATA_COL, index );
            TreeItem*   item = modelInterface->getTreeItem( idx );
            if ( item->getDepth() < maxDepth )
            {
                item->setExpandedStatus( true );
                this->setExpanded( idx, true );
                indexList.append( idx );
            }
        }
    }
    this->blockSignals( false );
    getTree()->invalidateItemLabel();
    emit itemExpanded( getTree()->getRootItem(), true );
}

/**
 * Recursively expands or collapses the given item and all its children in this view.
 * If the item is collapsed and children of the item are selected, the item gets selected
 * and the children get deselected.
 * @param expand if true, expands items, else it collapses them
 */
void
TreeView::expandSubtree( TreeItem* item, bool expand )
{
    this->blockSignals( true );             // don't emit signal for each subitem expand event

    QItemSelection  selection;
    bool            childIsSelected = false;
    QModelIndex     subtree         = modelInterface->find( item );
    QModelIndexList indexList;
    indexList.append( subtree );
    while ( !indexList.isEmpty() )
    {
        QModelIndex index = indexList.takeFirst();
        this->setExpanded( index, expand );
        // if tree is collapsed, deselect all selected children and select subtree root
        if ( !expand && selectionModel()->isSelected( index ) )
        {
            childIsSelected = true;
            selection.append( QItemSelection( index, index ) );
        }
        // expand children
        int count = model()->rowCount( index );
        for ( int i = 0; i < count; ++i )
        {
            indexList.append( model()->index( i, DATA_COL, index ) );
        }
    }
    this->blockSignals( false );
    if ( !expand && childIsSelected ) // select subtree root, if tree is collapsed and at least one child is selected
    {
        selectionModel()->blockSignals( true );
        selectionModel()->select( selection, QItemSelectionModel::Deselect );
        selectionModel()->blockSignals( false );
        selectionModel()->select( QItemSelection( subtree, subtree ), QItemSelectionModel::Select );
    }
}

void
TreeView::onCollapseAll()
{
    // set items in model collapsed
    getTree()->getRootItem()->setExpandedStatus( false, true );
    // set items in view collapsed
    this->blockSignals( true );
    collapseAll();
    this->blockSignals( false );
    // recalculate values
    emit itemExpanded( getTree()->getRootItem(), false );
}

void
TreeView::onExpandSubtree()
{
    QList<TreeItem*> selectedItems;
    if ( multipleContext )
    {
        selectedItems = getTree()->getSelectionList();
    }
    else
    {
        selectedItems.append( contextMenuItem );
    }

    foreach( TreeItem * item, selectedItems )
    {
        item->setExpandedStatus( true, true );
        expandSubtree( item, true );
        emit itemExpanded( item, true );
    }
}

/**
 * recursively expands the children with the largest values in view and model
 */
void
TreeView::expandLargest( TreeItem* item )
{
    // QPersistantModelIndex is required because ModelIndex becomes invalid after item has been expanded
    QPersistentModelIndex index( modelInterface->find( item ) );

    this->setExpanded( index, true ); // set expanded in view
    item->setExpandedStatus( true );  // set expanded in model

    int count = 1;
    while ( count > 0 )
    {
        TreeItem*   maxItem = 0;
        QModelIndex maxIndex;

        count = model()->rowCount( index );
        for ( int i = 0; i < count; ++i )
        {
            QModelIndex childIndex = model()->index( i, DATA_COL, index );
            TreeItem*   item       = modelInterface->getTreeItem( childIndex );
            if ( !maxItem || item->totalValue > maxItem->totalValue )
            {
                maxItem  = item;
                maxIndex = childIndex;
            }
        }
        // expand largest child
        index = QPersistentModelIndex( maxIndex );
        if ( maxItem )
        {
            this->setExpanded( maxIndex, true );
            maxItem->setExpandedStatus( true );
        }
    }
}

void
TreeView::onExpandLargest()
{
    QList<TreeItem*> selectedItems;
    if ( multipleContext )
    {
        selectedItems = getTree()->getSelectionList();
    }
    else
    {
        selectedItems.append( contextMenuItem );
    }

    foreach( TreeItem * item, selectedItems )
    {
        expandLargest( item );
        emit itemExpanded( item, true );
    }
}


// expand items with tree item markers and items which are marked as found
void
TreeView::onExpandMarked()
{
    // expand items with tree item markers
    const QList<TreeItem*>& items = getTree()->getItems();
    blockSignals( true );
    bool markedItemsExist = false;
    foreach( TreeItem * item, items )
    {
        if ( item->getMarkerList().size() > 0 )
        {
            markedItemsExist = true;
            TreeItem* parent = item;
            emit      itemExpanded( parent, true );
            do
            {
                QPersistentModelIndex index = QPersistentModelIndex( modelInterface->find( parent ) );
                this->setExpanded( index, true );  // set expanded in view
                parent->setExpandedStatus( true ); // set expanded in model
                parent = parent->getParent();
            }
            while ( parent->getParent() );
        }
    }
    blockSignals( false );
    if ( markedItemsExist )
    {
        emit itemExpanded( getTree()->getRootItem(), true );
    }
    expandFoundItems();
}

// expand items which are marked as found
void
TreeView::expandFoundItems()
{
    if ( foundItems.size() > 0 ) // expand found items
    {
        blockSignals( true );
        foreach( QModelIndex index, foundItems )
        {
            while ( ( index = index.parent() ).isValid() )
            {
                TreeItem* item = modelInterface->getTreeItem( index );
                this->setExpanded( index, true ); // set expanded in view
                item->setExpandedStatus( true );  // set expanded in model
            }
        }
        blockSignals( false );
        emit itemExpanded( getTree()->getRootItem(), true );
    }
}

void
TreeView::onCollapseSubtree()
{
    QList<TreeItem*> selectedItems;
    if ( multipleContext )
    {
        selectedItems = getTree()->getSelectionList();
    }
    else
    {
        selectedItems.append( contextMenuItem );
    }

    foreach( TreeItem * subtree, selectedItems )
    {
        // set items in model collapsed
        subtree->setExpandedStatus( false, true );
        // set items in view collapsed
        expandSubtree( subtree, false );
        // recalculate value (if selected recalculate values of right trees)
        emit itemExpanded( subtree, false );
    }
}

void
TreeView::onCopyToClipboard()
{
    /** recursively find all selected items in tree order */
    QString         text;
    QModelIndexList list;
    list.append( QModelIndex() ); // start with root
    while ( !list.isEmpty() )
    {
        QModelIndex parent = list.takeFirst();
        TreeItem*   item   = modelInterface->getTreeItem( parent );
        if ( item->isSelected() )
        {
            text += QString( 2 * item->getDepth(), ' ' ) + item->getLabel() + "\n";
        }

        int rows = model()->rowCount( parent );
        for ( int i = rows - 1; i >= 0; --i )
        {
            QModelIndex idx = model()->index( i, DATA_COL, parent );
            list.prepend( idx ); // check children
        }
    }
    QApplication::clipboard()->setText( text );
}

void
TreeView::onFindItems()
{
    // pop up a dialog to enter the expression to be searched for
    TreeItem* selected        = contextMenuItem;
    bool      enableSelection = this->getTree()->getType() != METRIC;
    if ( !findWidget )
    {
        findWidget = new FindWidget( this, modelInterface, enableSelection );
        connect( findWidget, SIGNAL( search( QString, FindAction ) ), this, SLOT( handleFoundItems( QString, FindAction ) ) );
        connect( findWidget, SIGNAL( close() ), this, SLOT( closeFindWidget() ) );
        connect( findWidget, SIGNAL( previous() ), this, SLOT( prevFoundItem() ) );
        connect( findWidget, SIGNAL( next() ), this, SLOT( nextFoundItem() ) );
    }
    findWidget->setVisible( true );

    if ( selected )
    {
#ifdef HAS_QREGULAR_EXPRESSION
        findWidget->setText( "^" + QRegularExpression::escape( selected->getName() ) + "$" );
#else
        findWidget->setText( "^" + QRegExp::escape( selected->getName() ) + "$" );
#endif
    }
    else
    {
        findWidget->setText( "" );
    }
    containerWidget->layout()->addWidget( findWidget );
}

void
TreeView::closeFindWidget()
{
    findWidget->setVisible( false );
}

void
TreeView::handleFoundItems( const QString& searchText, FindAction action )
{
    currentFoundIndex = -1;

    // search for the regular expression in the tree
#ifdef HAS_QREGULAR_EXPRESSION
    QModelIndexList found = modelInterface->find( QRegularExpression( searchText ) );
#else
    QModelIndexList found = modelInterface->find( QRegExp( searchText ) );
#endif
    modelInterface->setFoundItems( found );

    foundItems.clear();
    foreach( QModelIndex idx, found )
    {
        foundItems.append( QPersistentModelIndex( idx ) );
    }

    if ( foundItems.size() > 0 )
    {
        if ( action == SELECT )       // select the found items, if desired
        {
            QItemSelection selection; // adding to QItemSelection is extremly faster than calling selectionModel()->select() for each item
            foreach( QModelIndex idx, foundItems )
            {
                selection.select( idx, idx );
            }
            this->blockSignals( true ); // don't emit signal for each selection event
            selectionModel()->select( selection, QItemSelectionModel::ClearAndSelect );
            this->blockSignals( false );
            emit selectionChanged( getTree() );
        }
        else if ( action == EXPAND ) // expand all marked items
        {
            expandFoundItems();
        }
        else
        {
            QPersistentModelIndex idx = foundItems.at( 0 );
            this->scrollTo( idx );
        }
    }

    Globals::setStatusMessage( tr( "Found " ) + QString::number( foundItems.size() ) + tr( " items" ) );
    this->viewport()->update();
}

void
TreeView::checkCurrentSelection()
{
    QModelIndex selected = selectionModel()->selectedRows().first();
    int         idx      = foundItems.indexOf( selected );
    if ( idx >= 0 )
    {
        currentFoundIndex = idx;
    }
}

void
TreeView::nextFoundItem()
{
    checkCurrentSelection();
    if ( foundItems.size() > 0 )
    {
        currentFoundIndex = ( currentFoundIndex + 1 ) % foundItems.size();
        QPersistentModelIndex idx = foundItems.at( currentFoundIndex );
        this->scrollTo( idx );
        selectionModel()->select( idx, QItemSelectionModel::ClearAndSelect );
    }
}

void
TreeView::prevFoundItem()
{
    checkCurrentSelection();
    int size = foundItems.size();
    if ( size > 0 )
    {
        currentFoundIndex = currentFoundIndex >= 0 ? ( currentFoundIndex + size - 1 ) % size : size - 1;
        QPersistentModelIndex idx = foundItems.at( currentFoundIndex );
        this->scrollTo( idx );
        selectionModel()->select( idx, QItemSelectionModel::ClearAndSelect );
    }
}

QModelIndexList
TreeView::searchItems( const QStringList& items )
{
    QModelIndexList found;
    foreach( QString name, items )
    {
        found.append( modelInterface->find( name ) );
    }
    return found;
}

/**
 * @brief TreeView::selectItems selects all items in the given QModelIndexList and expands their parents
 * @param toSelect a list of item indices to select
 *
 */
void
TreeView::selectItems( const QModelIndexList& toSelect )
{
    if ( toSelect.isEmpty() )
    {
        return;
    }

    // scrolling gets very slow, if a large amount of items is selected -> set a limit and mark items instead
    // see QTBUG-59478, QTBUG-63022
    const int MAX_SELECT = 1000;

    // tree view gets extremly slow if a large amount of individual items is expanded -> set a limit and mark items instead
    const int MAX_EXPAND = 50;
    if ( markSelected )
    {
        modelInterface->clearFoundItems();
    }
    markSelected = false;

    this->blockSignals( true );         // don't emit signal for each selection event
    this->setUpdatesEnabled( false );

    QItemSelection selection;         // adding to QItemSelection is extremly faster than calling selectionModel()->select() for each item
    int            expandedCount  = 0;
    int            selectionCount = 0;
    foreach( QModelIndex idx, toSelect )
    {
        if ( selectionCount++ >= MAX_SELECT )
        {
            break;
        }
        selection.select( idx, idx );

        if ( !this->isExpanded( idx ) && expandedCount++ < MAX_EXPAND )
        {
            // expand parent items
            while ( ( idx = idx.parent() ).isValid() )
            {
                this->setExpanded( idx, true );  // set expanded in view
                TreeItem* item = modelInterface->getTreeItem( idx );
                item->setExpandedStatus( true ); // set expanded in model
            }
        }
    }
    selectionModel()->select( selection, QItemSelectionModel::ClearAndSelect );

    // not all selected items are expanded -> mark all selected items
    if ( expandedCount > MAX_EXPAND )
    {
        markSelected = true;
        modelInterface->setFoundItems( toSelect );
        foundItems.clear();
        foreach( QModelIndex idx, toSelect )
        {
            foundItems.append( QPersistentModelIndex( idx ) );
        }
    }

    this->setUpdatesEnabled( true );
    this->blockSignals( false );

    emit recalculateRequest( getTree() );

    if ( selectionCount > MAX_SELECT )
    {
        QString message = tr( "Warning: Only selecting %1 of %2 items. The items are marked instead." );
        QString msg     = message.arg( MAX_SELECT ).arg( toSelect.size() );
        Globals::setStatusMessage( msg, Warning );
    }
    else if ( expandedCount > MAX_EXPAND )
    {
        QString message = tr( "Warning: Only expanding %1 of %2 selected items." );
        QString msg     = message.arg( MAX_EXPAND ).arg( toSelect.size() );
        Globals::setStatusMessage( msg, Warning );
    }
}

void
TreeView::onUnmarkItems()
{
    modelInterface->clearFoundItems();
}

/* shows a window with information about the item on which the context menu is called */
void
TreeView::onInfo()
{
    InfoWidget::getInstance()->activate();
    InfoWidget::getInstance()->toFront();
    updateInfoWidget();
}

QString
TreeView::getInfoString( TreeItem* item )
{
    QString   path;
    TreeItem* currentItem = item;
    while ( currentItem->getParent() )
    {
        QString sep  = currentItem->getDepth() - 1 > 0 ? "+ " : "";
        QString line = QString( 2 * ( currentItem->getDepth() - 1 ), ' ' ) + sep + currentItem->getLabel();
        path        = ( currentItem == item ) ? line : line + "\n" + path;
        currentItem = currentItem->getParent();
    }

    TabManager* manager     = Globals::getTabManager();
    TreeView*   view        = manager->getView( item->getTree() );
    QString     description = view->getContextDescription( item );
    description += "\n\n" + tr( "Path" ) + ": \n" + path + "\n";

    if ( item->getValueObject() ) // add extended info from ValueView if available
    {
        cube::DataType dataType = item->getValueObject()->myDataType();
        QString        extended = Globals::getValueView( dataType )->getExtendedInfo( item );
        if ( !extended.isEmpty() )
        {
            description += tr( "\nValue details:\n" ) + extended;
        }
        // widget = Globals::getValueView( dataType )->getExtendedInfoWidget( item );
    }

    if ( item->markerList.size() > 0 )
    {
        description += tr( "\n\nDefined marker:\n" );
        foreach( const TreeItemMarker * marker, item->markerList )
        {
            description.append( "- " + marker->getLabel( item ) );
            description.append( "\n" );
        }
    }

    return description;
}



static
QString
_gatherGlobalInfo( cube::CubeProxy* cube )
{
    // path
    QString     global_info = QObject::tr( "Path:\n" );
    std::string _cubename   = cube->get_cubename();
    global_info += QString( "%1\n\n" ).arg( QString::fromStdString( _cubename ) ) + QString( QObject::tr( "Mirrors\n" ) );
    const std::vector< std::string >& _mirrors = cube->getMirrors();
    for ( std::vector< std::string >::const_iterator iter = _mirrors.begin(); iter != _mirrors.end(); ++iter )
    {
        global_info += QString::fromStdString( *iter ) + QString( "\n" );
    }
    global_info += QObject::tr( "\n\n" );
    global_info += QObject::tr( "Global attributes\n" );
    const std::map< std::string, std::string >& _attrs = cube->getAttributes();
    for ( std::map< std::string, std::string >::const_iterator iter = _attrs.begin(); iter != _attrs.end(); ++iter )
    {
        global_info += QString::fromStdString( iter->first ) += QString( "\t-> " ) + QString::fromStdString( iter->second ) + QString( "\n" );
    }
    global_info += QObject::tr( "\n\n" );
    return global_info;
}

void
TreeView::updateInfoWidget()
{
    if ( !InfoWidget::getInstance()->isVisible() )
    {
        return;
    }

    TabManager* manager    = Globals::getTabManager();
    TreeItem*   metricItem = manager->getActiveTree( METRIC )->getLastSelection();
    TreeItem*   callItem   = manager->getActiveTree( CALL )->getLastSelection();
    TreeItem*   sysItem    = manager->getActiveTree( SYSTEM )->getLastSelection();
    if ( !metricItem || !callItem || !sysItem )
    {
        return;
    }
    QString _globalInfo = _gatherGlobalInfo( manager->getCube() );
    QString _debugInfo;
    if ( Globals::optionIsSet( ExpertMode ) && Globals::optionIsSet( VerboseMode ) )
    {
        if ( manager->getCube()->hasCubePlMemoryManager() )
        {
            _debugInfo = QString::fromStdString( manager->getCube()->getCubePlMemoryManager().dump_memory() );
        }
        else
        {
            _debugInfo = tr( "CubePL memory dump is not available for Network Cube Proxy." );
        }
    }


    InfoWidget::getInstance()->showTreeInfo( getInfoString( metricItem ),
                                             getInfoString( callItem ),
                                             getInfoString( sysItem ),
                                             getUrl( metricItem ),
                                             getUrl( callItem ),
                                             _globalInfo,
                                             _debugInfo
                                             );
    InfoWidget::getInstance()->activateTab( getTree()->getType() );
}

void
TreeView::onShowDocumentation()
{
    TreeItem* item = contextMenuItem;

    if ( item == NULL ) // if called from help menu, show information about the recently selected item
    {
        item = getTree()->getLastSelection();
    }

    HelpBrowser* helpBrowser = HelpBrowser::getInstance( tr( "Documentation" ) );
    QString      url         = getUrl( item );
    helpBrowser->showUrl( url, QString( tr( "No accessible mirror found" ) ) );
}


// ------------------ hiding/sorting -------------------------------------------------------------
// ------------------ only available if the used TreeModel is a QSortFilterProxyModel ------------

void
TreeView::onHidingThreshold()
{
    bool   ok;
    double newThreshold = getHidingThreshold( contextMenuItem, ok );
    if ( ok )
    {
        hidingThreshold = newThreshold;
        onHiding( false );
    }
    contextMenuHash.value( HidingThreshold )->setEnabled( true );
}

void
TreeView::onHiding( bool askThreshold )
{
    getTree()->enableHiding();

    bool ok = true;

    bool             dynamic   = contextMenuHash.value( DynamicHiding )->isChecked();
    Tree::FilterType newFilter = dynamic ?  Tree::FILTER_DYNAMIC : Tree::FILTER_STATIC;

    if ( askThreshold && ( hidingThreshold < 0 || getTree()->getFilter() == newFilter ) )
    {
        double newThreshold = getHidingThreshold( contextMenuItem, ok );
        if ( ok )
        {
            hidingThreshold = newThreshold;
        }
    }

    if ( ok )
    {
        TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
        if ( model )
        {
            model->setFilter( newFilter, hidingThreshold );
        }
    }
}

void
TreeView::onHideItem()
{
    getTree()->enableHiding();

    contextMenuHash.value( StaticHiding )->setChecked( true );

    // hide clicked item
    contextMenuItem->setHidden( true );

    // check if clicked item or any of its children is selected -> select visible parent
    bool             hiddenSelected = false;
    QList<TreeItem*> children;
    children.append( contextMenuItem );
    while ( !children.isEmpty() )
    {
        TreeItem* child = children.takeLast();
        if ( child->isSelected() )
        {
            hiddenSelected = true;
            child->setSelectionStatus( false );
        }
        children.append( child->getChildren() );
    }
    if ( hiddenSelected )
    {
        contextMenuItem->getParent()->select( true ); // first select then deselect because at least one selection is required
    }

    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    if ( model )
    {
        model->setFilter( Tree::FILTER_STATIC );
    }
}

void
TreeView::onUnhideItem()
{
    contextMenuHash.value( StaticHiding )->setChecked( true );

    // unhide children of clicked item
    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    if ( model )
    {
        foreach( TreeItem * item, contextMenuItem->getChildren() )
        {
            item->setHidden( false );
        }
        model->setFilter( Tree::FILTER_STATIC );
    }
}

void
TreeView::onDisableHiding()
{
    contextMenuHash.value( HidingThreshold )->setEnabled( false );

    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    if ( model )
    {
        model->setFilter( Tree::FILTER_NONE );
    }
}

double
TreeView::getHidingThreshold( TreeItem* item, bool& ok )
{
    double clickedValue;
    double defaultThreshold;

    // first determine a default (suggested) threshold for hiding; this
    // threshold is a percentual value: we will hide items whose colors
    // are under this percentage on the color scale

    if ( item != NULL )
    {
        // get the current value of the clicked item
        clickedValue = item->getValue( &ok );
        if ( ok )
        {
            // take the absolute value
            clickedValue = fabs( clickedValue );

            // if the clickedValue of the current node is near by 0, then set it to 0
            if ( clickedValue <= Globals::getRoundThreshold( FORMAT_TREES ) )
            {
                clickedValue = 0.0;
            }
            // get values corresponding to the minimal and maximak colors
            double min = 0.0;
            double max = 0.0;

            if ( getTree()->hasUserDefinedMinMaxValues() )
            {
                min = getTree()->getUserDefinedMinValue();
                max = getTree()->getUserDefinedMaxValue();
            }
            else
            {
                min = 0.0;
                if ( getTree()->getValueModus() == ABSOLUTE_VALUES )
                {
                    max = getTree()->getMaxValue( item );
                }
                else
                {
                    max = 100.0;
                }
            }

            // the default threshold will be the percentual position of the
            // clicked value between the min and max values
            if ( max - min == 0.0 )
            {
                defaultThreshold = 0.0;
            }
            else
            {
                defaultThreshold = 100.0 * ( clickedValue - min ) / ( max - min );
                if ( defaultThreshold < 0.0 )
                {
                    defaultThreshold = 0.0;
                }
                else if ( defaultThreshold > 100.0 )
                {
                    defaultThreshold = 100.0;
                }
                else
                {
                    defaultThreshold += 1e-17; // todo where is this value defined ?????
                }
            }
        }
        else
        {
            defaultThreshold = hidingThreshold;
        }
    }
    else
    {
        defaultThreshold = hidingThreshold;
    }


    // get a user-defined threshold, using defaultThreshold as default
    double newThreshold;
    int    decimals = Globals::getRoundNumber( FORMAT_TREES );
    newThreshold = QInputDialog::getDouble( this, tr( "Dynamic hiding" ),
                                            tr( "Dynamically hide nodes below (less or equal) this percentage on the color scale:" ),
                                            defaultThreshold, 0.0, 100.0, decimals, &ok );
    return newThreshold;
}

void
TreeView::onSortByName()
{
    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    if ( model )
    {
        model->setSortAlways( false ); // name usually doesn't change
        model->setComparator( new NameComparator() );
        model->sort( -1 );
        model->sort( DATA_COL );
    }
}

void
TreeView::onSortByInclusiveValue( bool always )
{
    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    if ( model )
    {
        setSortAlways( model, always );
        model->setComparator( new InclusiveValueComparator() );
        model->sort( -1 ); // subsequent calls to sort only work, if called with different column before
        model->sort( DATA_COL );
    }
}

void
TreeView::onSortByExclusiveValue( bool always )
{
    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    if ( model )
    {
        setSortAlways( model, always );
        model->setComparator( new ExclusiveValueComparator() );
        model->sort( -1 );
        model->sort( DATA_COL );
    }
}

void
TreeView::onSortByNameAndNumber()
{
    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    if ( model )
    {
        model->setSortAlways( false ); // name doesn't change -> don't update
        model->setComparator( new NameAndNumberComparator() );
        model->sort( -1 );
        model->sort( DATA_COL );
    }
}

void
TreeView::setSortAlways( TreeModelProxy* model, bool always )
{
    model->setSortAlways( always );
    if ( !always ) // uncheck all radio buttons, if apply was choosen
    {
        contextMenuHash.value( SortByName )->setChecked( false );
        contextMenuHash.value( SortByNameNumber )->setChecked( false );
        contextMenuHash.value( SortByInclusiveValue )->setChecked( false );
        contextMenuHash.value( SortByExclusiveValue )->setChecked( false );
        contextMenuHash.value( DisableSorting )->setChecked( false );
    }
}

void
TreeView::onSortByInclusiveValueOnce()
{
    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    if ( model )
    {
        onSortByInclusiveValue( false );
    }
}

void
TreeView::onSortByExclusiveValueOnce()
{
    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    if ( model )
    {
        onSortByExclusiveValue( false );
    }
}


void
TreeView::onDisableSorting()
{
    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    if ( model )
    {
        model->setSortAlways( false );
        model->setComparator( 0 );
        model->sort( -1 ); // returns to the sort order of the underlying source model.
    }
}


void
TreeView::onUnhideTreeLevel()
{
    modelInterface->unhideTreeLevel();
    hasHiddenLevel = false;

    // set selections which have been lost after unhiding
    updateSelectionStatus();

    // status of expanded items gets lost after unhiding
    updateExpandedCollapsedStatus();
}

void
TreeView::onHideTreeLevel()
{
    modelInterface->hideTreeLevel( contextMenuItem->getDepth() );
    hasHiddenLevel = true;

    // set selections which have been lost after hiding
    updateSelectionStatus();

    // status of expanded items gets lost after hiding
    updateExpandedCollapsedStatus();
}

void
TreeView::onMinMaxValues()
{
    bool   ok = true;
    double minValue, maxValue;
    bool   updateView = false;

    // ask the user if user-defined minimal and maximal values should be used
    QMessageBox::StandardButton answer =
        QMessageBox::question( this, tr( "Min/max values" ), tr( "Use user-defined minimal and maximal values for coloring?" ),
                               QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes );

    // if the user canceled the input then do nothing
    if ( answer == QMessageBox::Cancel )
    {
        return;
    }

    // if the user de-activated user-defined min/max values for coloring,
    // then re-display with the default coloring
    else if ( answer == QMessageBox::No )
    {
        if ( getTree()->hasUserDefinedMinMaxValues() )
        {
            getTree()->unsetUserDefinedMinMaxValues();
            updateView = true;
        }
    }
    else // user defines min/max values activated
    {
        // if the user activated user-defined coloring, then ask for the minimal and maximal values
        // for the minimal and maximal colors;
        // ok gets false if the user canceled the input, in this case do nothing

        bool   userMinMax = getTree()->hasUserDefinedMinMaxValues();
        double min        = userMinMax ? getTree()->getUserDefinedMinValue() : 0.0;
        minValue = QInputDialog::getDouble( this, tr( "Minimal value" ), tr( "Minimal value for coloring:" ), min, 0.0, 1e+15, 15, &ok );

        if ( ok )
        {
            assert( getTree()->getType() != METRIC );
            double max = userMinMax ? getTree()->getUserDefinedMaxValue() : getTree()->getMaxValue();
            maxValue = QInputDialog::getDouble( this, tr( "Maximal value" ), tr( "Maximal value for coloring:" ),
                                                max, 0.0, 1e+15, 15, &ok );
        }

        if ( ok )
        {
            getTree()->setUserDefinedMinMaxValues( minValue, maxValue );
            userMinValue = minValue;
            userMaxValue = maxValue;
            updateView   = true;
        }
    }
    if ( updateView )
    {
        getTree()->updateItems();           // update colors
        emit selectionChanged( getTree() ); // used to update the value widget
    }
}
// end of onMinMaxValues()

// / ============= end context menu actions  =======================

/**
 * @brief TreeView::updateRowHeight has to be called, if a ValueView with a different row height is used
 */
void
TreeView::updateRowHeight()
{
    QAbstractItemModel* imodel = model();
    QModelIndex         first  = imodel->index( 0, DATA_COL, QModelIndex() );
    delegate->updateRowHeight( first );
}

// ==================================================================================================================
//  class TreeItemDelegate which paints each tree item
// ==================================================================================================================

/**
 * @brief TreeItemDelegate::sizeHint return width of the item to draw
 */
QSize
TreeItemDelegate::sizeHint( const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QSize     size = option.fontMetrics.boundingRect( option.rect, Qt::TextDontClip, index.data().toString() ).size();
    TreeItem* item = ( TreeItem* )qvariant_cast<void*> ( index.data( TreeItemRole ) );

    cube::DataType dataType = cube::CUBE_DATA_TYPE_NONE;
    if ( item && item->getValueObject() )
    {
        dataType = item->getValueObject()->myDataType();
    }
    QSize iconSize = Globals::getValueView( dataType )->getIconSize( item );
    int   pad1     = 2;
    int   pad2     = option.fontMetrics.height() / 2;
    int   textX    = iconSize.width() + pad1 + pad2; // start position of text = width of left icon + paddings
    size.setWidth( size.width() + textX );
    size.setHeight( std::max( iconSize.height(), option.fontMetrics.height() ) );

    return size;
}

/**
 * @brief TreeItemDelegate::updateRowHeight
 * @param index the in dex of the row whose height has changed
 * emits the signal sizeHintChanged which is required if sizeHint changes its value
 */
void
TreeItemDelegate::updateRowHeight( QModelIndex& index )
{
    emit sizeHintChanged( index );
}

/** draws the tree item marker for a given tree item */
void
TreeItemDelegate::paintMarker( QPainter* painter, const QRect& textRect, TreeItem* item ) const
{
    QRect rect = textRect.adjusted( 0, 0, 0, -1 );

    // item with marker -> fill rectangle with marker color
    foreach( const TreeItemMarker * marker, item->markerList )
    {
        if ( marker->isColorVisible() )
        {
            painter->fillRect( rect, marker->getAttributes().getColor() );
            rect.adjust( 0, 0, -4, -2 );
        }
    }

    rect = textRect.adjusted( 0, 0, 0, -1 );

    bool markedAsParent = false;
    // item is parent of a marked item => draw rectangle
    if ( !item->isExpanded() && !item->parentMarkerList.isEmpty() )
    {
        const TreeItemMarker* usedMarker = 0;
        // Use marker color to outline parent item. Use black, if more than one marker is set
        foreach( const TreeItemMarker * marker, item->parentMarkerList )
        {
            if ( marker->isColorVisible() )
            {
                if ( !usedMarker || usedMarker == marker )
                {
                    painter->setPen( marker->getAttributes().getColor() );
                    usedMarker = marker;
                }
                else
                {
                    painter->setPen( Qt::black );
                    break;
                }
                markedAsParent = true;
            }
        }
        if ( markedAsParent )
        {
            painter->drawRect( rect );
            painter->setPen( Qt::black );
        }
    }

    // mark items as dependency
    const TreeItemMarker* usedMarker = 0;
    foreach( const TreeItemMarker * marker, item->dependencyMarkerList )
    {
        if ( marker->isColorVisible() || !marker->getIcon().isNull() )
        {
            if ( markedAsParent )
            {
                rect = textRect.adjusted( 1, 1, -1, -2 );
            }

            painter->setPen( Qt::white );
            painter->drawRect( rect );
            QPen pen = painter->pen();
            pen.setStyle( Qt::DashLine );
            if ( !usedMarker || usedMarker == marker ) // outline with marker color
            {
                pen.setColor( marker->getAttributes().getColor() );
                usedMarker = marker;
            }
            else // more than one marker => mark with black dash outline
            {
                pen.setColor( Qt::black );
            }
            painter->setPen( pen );
            painter->drawRect( rect );
        }
        painter->setPen( Qt::black );
    }

    // === draw optional right icon(s)
    if ( !item->markerList.isEmpty() )
    {
        int posX = rect.x() + rect.width() + iconPad;
        foreach( const TreeItemMarker * marker, item->markerList )
        {
            const QPixmap& icon = marker->getIcon();
            if ( !icon.isNull() )
            {
                int padY = ( rect.height() - icon.height() ) / 2;
                padY = ( rect.height() - icon.height() ) / 2;
                painter->drawPixmap( posX, rect.y() + padY, icon );
                posX += icon.width();
            }
        }
    }
}

static QBrush foundBrush       = QBrush( Qt::yellow );
static QBrush foundParentBrush = QBrush( QColor( Qt::yellow ).lighter( 170 ) );

/**
 * @brief TreeItemDelegate::paint paints the item with the given index
 */
void
TreeItemDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    TreeItem* item = ( TreeItem* )qvariant_cast<void*> ( index.data( TreeItemRole ) );
    assert( item != NULL );

    cube::DataType type     = item->getValueObject() ? item->getValueObject()->myDataType() : cube::CUBE_DATA_TYPE_NONE;
    QSize          iconSize = Globals::getValueView( type )->getIconSize( item );
    int            pad      = painter->fontMetrics().height() / 2; // space right of the icon
    int            textX    = iconSize.width() + iconPad + pad;    // start position of text = width of left icon + paddings

    // textRect = bounding rectangle of the item text and its icons
    QRect textRect = option.fontMetrics.boundingRect( option.rect, Qt::TextDontClip, index.data().toString() );
    textRect.setWidth( textRect.width() + textX );
    textRect.setHeight( std::max( iconSize.height(), textRect.height() ) );

    // draw item background
    QBrush           backgroundBrush;
    TreeItemMarkType itemType = ( TreeItemMarkType )qvariant_cast<int> ( index.data( Qt::BackgroundRole ) );
    if (  itemType == FOUND_ITEM )
    {
        backgroundBrush = foundBrush;
    }
    else if ( itemType == FOUND_ITEM_PARENT )
    {
        backgroundBrush = foundParentBrush;
    }
    else if ( itemType == SELECTED_ITEM_PARENT )
    {
        QColor color = option.palette.highlight().color();
        color.setAlpha( 40 );
        backgroundBrush = QBrush( color );
    }
    painter->fillRect( textRect, backgroundBrush );
    paintMarker( painter, textRect, item );

    // gray out items without marker, if desired
    bool noMarkersInTree = item->getTree()->getRootItem()->parentMarkerList.size() == 0;
    bool gray            = TreeItemMarker::isGrayedOut() && item->getMarkerList().size() == 0 &&
                           item->parentMarkerList.size() == 0 && !noMarkersInTree;
    // only one marker which is maked as insignificant => gray out
    if ( ( item->getMarkerList().size() == 1 ) && ( item->getMarkerList().first()->isInsignificant() ) )
    {
        gray = true;
    }

    // set text/foreground color
    QColor textColor = gray ? Qt::lightGray : option.palette.text().color();

    // ensure that the contrast of background and foreground color is sufficient
    const QColor& backColor   = backgroundBrush.style() == Qt::NoBrush ? option.palette.base().color() : backgroundBrush.color();
    int           grayScaledB = qGray( backColor.rgb() );
    int           grayScaledF = qGray( textColor.rgb() );
    if ( backColor.alpha() < 255 ) // combine the transparent background with the base color
    {
        int base = qGray( option.palette.base().color().rgb() );
        grayScaledB = ( backColor.alpha() * grayScaledB / 255 + ( 255 - backColor.alpha() ) * base / 255 );
    }
    int colorDiff = abs( grayScaledB - grayScaledF ); // use grayscaled color values to compare
    if ( colorDiff < 90 )                             // the limit is an empirical value
    {
        textColor = ( grayScaledB < 150 ) ? Qt::white : Qt::black;
    }

    // if ManualCalculation mode is chosen -> gray out all items that aren't calculated
    if ( !item->isCalculationEnabled() && !item->isCalculated() )
    {
        textColor = Qt::gray;
    }

    // draw selected items: set background and foreground color from palette
    if ( option.state & QStyle::State_Selected )
    {
        painter->fillRect( textRect, option.palette.highlight() );
        textColor = option.palette.highlightedText().color();
    }
    // else if ( option.state & QStyle::State_HasFocus )

    // === draw left icon

    QRect   rect = option.rect;
    QPixmap pix  = Globals::getValueView( type )->getIcon( item, gray );
    int     padY = ( rect.height() - pix.height() ) / 2;
    painter->drawPixmap( rect.x() + iconPad, rect.y() + padY, pix );

    // === draw item text
    painter->setPen( textColor );
    int space = rect.height() - painter->fontMetrics().height();
    int y     = rect.y() + painter->fontMetrics().ascent() + space / 2;
    painter->drawText( rect.x() + textX, y, index.data().toString() );
}

//--- manual calculation context menu items --------------------------------------------------------------------------------------

void
TreeView::addExpertContextMenuOptions()
{
    if ( getTree()->isManualCalculation() )
    {
        // ===================================================================================
        QAction* markItemCalcAct = new QAction( tr( "Mark this item for calculation" ), this );
        connect( markItemCalcAct, SIGNAL( triggered() ), this, SLOT( markItem() ) );

        QAction* unmarkItemCalcAct = new QAction( tr( "Unmark this item" ), this );
        connect( unmarkItemCalcAct, SIGNAL( triggered() ), this, SLOT( unmarkItem() ) );

        QAction* calcMarkedAct = new QAction( tr( "Calculate marked items" ), this );
        connect( calcMarkedAct, SIGNAL( triggered() ), this, SLOT( calculateMarked() ) );

        QAction* calcItemNow = new QAction( tr( "Calculate this item" ), this );
        connect( calcItemNow, SIGNAL( triggered() ), this, SLOT( calculateItemNow() ) );

        QAction* calcSelNow = new QAction( tr( "Calculate selected items" ), this );
        connect( calcSelNow, SIGNAL( triggered() ), this, SLOT( calculateSelectedItemsNow() ) );

        QAction* calcItem = new QAction( tr(  "Always calculate this item" ), this );
        connect( calcItem, SIGNAL( triggered() ), this, SLOT( calculateItem() ) );

        QAction* calcSel = new QAction( tr( "Always calculate selected items" ), this );
        connect( calcSel, SIGNAL( triggered() ), this, SLOT( calculateSelectedItems() ) );

        QAction* calcDisable = new QAction( tr( "Disable automatic calculation of items" ), this );
        connect( calcDisable, SIGNAL( triggered() ), this, SLOT( disableAutomaticCalculation() ) );

        contextMenu->addAction( markItemCalcAct );
        contextMenu->addAction( unmarkItemCalcAct );
        contextMenu->addAction( calcMarkedAct );
        contextMenu->addAction( calcItemNow );
        contextMenu->addAction( calcSelNow );
        //contextMenu->addAction( calcItem );
        //contextMenu->addAction( calcSel );
        //contextMenu->addAction( calcDisable );
        contextMenu->addSeparator();

        if ( !contextMenuItem )
        {
            markItemCalcAct->setEnabled( false );
            calcItemNow->setEnabled( false );
        }
    }
}

void
TreeView::markItem()
{
    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    QModelIndex     index = model->find( contextMenuItem );
    QModelIndexList list  = model->getFoundItems();
    list.append( index );
    model->setFoundItems( list );
}

void
TreeView::unmarkItem()
{
    TreeModelProxy* model = dynamic_cast<TreeModelProxy*> ( this->model() );
    QModelIndex     index = model->find( contextMenuItem );
    QModelIndexList list  = model->getFoundItems();
    list.removeAll( index );
    model->setFoundItems( list );
}

/** ManualCalculation mode: if calculation of tree items is disabled, the choosen item will be calculated */
void
TreeView::calculateItemNow()
{
    if ( contextMenuItem )
    {
        QList<TreeItem*> list;
        list.append( contextMenuItem );
        emit calculateRequest( list );
    }
}

/** ManualCalculation mode: if calculation of tree items is disabled, the choosen item will be calculated */
void
TreeView::calculateMarked()
{
    TreeModelProxy*  model = dynamic_cast<TreeModelProxy*> ( this->model() );
    QList<TreeItem*> list;
    foreach( QModelIndex idx, model->getFoundItems() )
    {
        list.append( modelInterface->getTreeItem( idx ) );
    }
    if ( !list.isEmpty() )
    {
        emit calculateRequest( list );
    }
}

/** ManualCalculation mode: if calculation of tree items is disabled, the selected items will calculated */
void
TreeView::calculateSelectedItemsNow()
{
    const QList<TreeItem*>& list = getTree()->getSelectionList();
    emit                    calculateRequest( list );
}

/** ManualCalculation mode: if calculation of tree items is disabled, the choosen item will be marked to be calculated
    and calculation is triggered */
void
TreeView::calculateItem()
{
    if ( contextMenuItem )
    {
        contextMenuItem->setCalculationEnabled( true );
        calculateItemNow();
    }
}

/** ManualCalculation mode: if calculation of tree items is disabled, the selected items will be marked to be calculated
    and calculation is triggered */
void
TreeView::calculateSelectedItems()
{
    if ( contextMenuItem )
    {
        const QList<TreeItem*>& selected = getTree()->getSelectionList();
        foreach( TreeItem * item, selected )
        {
            item->setCalculationEnabled( true );
        }
        emit calculateRequest( selected );
    }
}

/** ManualCalculation mode: mark all items to not being calculated */
void
TreeView::disableAutomaticCalculation()
{
    if ( contextMenuItem )
    {
        foreach( TreeItem * item, getTree()->getItems() )
        {
            item->setCalculationEnabled( false );
        }
    }
}

//--- end manual calculation -----------------------------------------------------------------------------------------------------

void
TreeView::setFuture( const Future* future )
{
#ifdef CUBE_CONCURRENT_LIB
    progress            = 0;
    this->futureWatcher = &future->getFutureWatcher();
    if ( getTree()->getType() != SYSTEM ) // systemtree values are calculated in one chunk
    {
        connect( futureWatcher, SIGNAL( progressValueChanged( int ) ), this, SLOT( showProgress( int ) ) );
    }
    updateTime = 250;
    timer.start();
#endif
}

/** updates tree values while calculation is running */
void
TreeView::showProgress( int p )
{
#ifdef CUBE_CONCURRENT_LIB
    int range = ( futureWatcher->progressMaximum() - futureWatcher->progressMinimum() );
    if ( range == 0 || timer.elapsed() < 2 * updateTime )
    {
        return;  // don't update more often than every 500 ms
    }

    const int almost_finished = 98; // calculations are almost done and view will be updated after after beeing finished
    if ( p == progress || progress >= almost_finished )
    {
        return;  // show only changes
    }
    this->progress = p;

    long t1 = timer.elapsed();
    this->getTree()->updateItems( false ); // update values of already calculated items
    this->updateValues();                  // update view
    updateTime = std::max( timer.elapsed() - t1, ( qint64 )250 );
    timer.start();
#endif
}

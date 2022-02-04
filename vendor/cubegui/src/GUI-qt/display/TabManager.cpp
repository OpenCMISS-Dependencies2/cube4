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

#include "TabManager.h"

#include <QLabel>
#include <QApplication>
#include <QTime>
#include "CubeProxy.h"

#include "Tree.h"
#include "MetricTree.h"
#include "DefaultCallTree.h"
#include "FlatTree.h"
#include "TaskCallTree.h"
#include "SystemTree.h"

#include "TreeModelProxy.h"
#include "TreeView.h"
#include "MetricTreeView.h"
#include "CallTreeView.h"
#include "FlatTreeView.h"
#include "SystemTreeView.h"
#include "PluginManager.h"
#include "TreeItemMarker.h"
#include "ProgressBar.h"
#include "ValueWidget.h"
#include "Future.h"
#include "PluginList.h"

using namespace cubepluginapi;
using namespace cubegui;

#if QT_VERSION < QT_VERSION_CHECK( 5, 14, 0 )
constexpr QString::SplitBehavior SkipEmptyParts = QString::SkipEmptyParts;
#else
constexpr Qt::SplitBehaviorFlags SkipEmptyParts = Qt::SkipEmptyParts;
#endif

TabManager::TabManager( QWidget* parent ) :
    QSplitter( parent )
{
    isInitialized = false;
    cube          = nullptr;

    DisplayType tabType[] = { METRIC, CALL, SYSTEM };
    for ( int i = 0; i < DISPLAYS; i++ )
    {
        TabWidget* tab = new TabWidget( tabType[ i ] );
        this->addWidget( tab );
        tabWidgets.append( tab );
        order.append( tabType[ i ] );
    }
}


void
TabManager::cubeOpened( cube::CubeProxy* cube )
{
    // set the tab order
    foreach( DisplayType tabType, order )
    {
        tabWidgets[ tabType ]->initialize( order );
    }

    // create trees and corresponding views
    createTreesAndViews( cube );

    // add trees to tab widgets
    for ( Tree* tree: trees )
    {
        DisplayType tabType = tree->getType();
        tabWidgets[ tabType ]->addTreeTab( treeViewHash[ tree ] );

        tree->setTrees( trees ); // give each tree information about the other trees
    }

    // set Future for each tree
    for ( Tree* tree: trees )
    {
        DisplayType tabType   = tree->getType();
        TabWidget*  tabWidget = tabWidgets[ tabType ];
        TreeView*   view      = treeViewHash[ tree ];

        futures[ tree ] = new cubepluginapi::Future( tabWidget, view );
        tree->setFuture( futures[ tree ] );

#ifdef CUBE_CONCURRENT_LIB
        if ( !Globals::optionIsSet( Single ) )
        {
            // keep tree values up to date
            connect( tree, SIGNAL( calculationStarted( const Future* ) ), view, SLOT( setFuture( const Future* ) ) );
        }
#endif
    }

    // initial selection will be done in TabManager::initialize
}

void
TabManager::createTreesAndViews( cube::CubeProxy* _cube )
{
    cube = _cube;
    Tree*           tree       = 0;
    TreeView*       view       = 0;
    TreeModelProxy* proxyModel = 0;

    // MetricTree
    tree = new MetricTree( cube );
    tree->initialize();
    proxyModel = new TreeModelProxy( tree->getModel() ); // use proxy model for sorting
    view       = new MetricTreeView( proxyModel, tree->getLabel() );
    fillTreeHash( tree, view, proxyModel );
    treeHash[ METRICTREE ] = tree;

    // CallTree
    DefaultCallTree* callTree;
    callTree = new DefaultCallTree( cube, cube->getRootCnodes() );
    tree     = callTree;
    tree->initialize();
    proxyModel = new TreeModelProxy( tree->getModel() );
    view       = new CallTreeView( proxyModel, tree->getLabel() );
    fillTreeHash( tree, view, proxyModel );
    treeHash[ DEFAULTCALLTREE ] = tree;

    connect( proxyModel, SIGNAL( filteringEnded() ), view, SLOT( filteringEnded() ) ); // required for filtering: keep status of expanded items
    connect( tree->getModel(), SIGNAL( treeStructureChanged() ), view, SLOT( updateExpandedCollapsedStatus() )  );

    // TaskTree
    CallTree* taskTree = nullptr;
    if ( cube->getArtificialCnodes().size() > 0 )
    {
        taskTree = new TaskCallTree( cube );
        tree     = taskTree;
        tree->initialize();
        proxyModel = new TreeModelProxy( tree->getModel() );
        view       = new CallTreeView( proxyModel, "Tasks" );
        fillTreeHash( tree, view, proxyModel );
        treeHash[ TASKTREE ] = tree;

        connect( proxyModel, SIGNAL( filteringEnded() ), view, SLOT( filteringEnded() ) ); // required for filtering: keep status of expanded items
        connect( tree->getModel(), SIGNAL( treeStructureChanged() ), view, SLOT( updateExpandedCollapsedStatus() )  );
    }

    // FlatTree
    FlatTree* flatTree = new FlatTree( callTree, taskTree );
    tree = flatTree;
    tree->initialize();
    proxyModel = new TreeModelProxy( tree->getModel() );
    view       = new FlatTreeView( proxyModel, tree->getLabel() );
    fillTreeHash( tree, view, proxyModel );
    treeHash[ FLATTREE ] = tree;

    connect( proxyModel, SIGNAL( filteringEnded() ), view, SLOT( filteringEnded() ) ); // required for filtering: keep status of expanded items
    connect( tree->getModel(), SIGNAL( treeStructureChanged() ), view, SLOT( updateExpandedCollapsedStatus() )  );

    TreeView* callView = treeViewHash[ callTree ];
    /* notify flat view about changes in corresponding tree view and vice versa */
    connect( view, SIGNAL( selectionChanged( Tree* ) ), callView, SLOT( invalidateSelection( Tree* ) ) );
    connect( callView, SIGNAL( selectionChanged( Tree* ) ), view, SLOT( invalidateSelection( Tree* ) ) );
    if ( taskTree )
    {
        TreeView* taskView = treeViewHash[ taskTree ];
        connect( view, SIGNAL( selectionChanged( Tree* ) ), taskView, SLOT( invalidateSelection( Tree* ) ) );
        connect( taskView, SIGNAL( selectionChanged( Tree* ) ), view, SLOT( invalidateSelection( Tree* ) ) );
        connect( taskView, SIGNAL( selectionChanged( Tree* ) ), callView, SLOT( invalidateSelection( Tree* ) ) );
        connect( callView, SIGNAL( selectionChanged( Tree* ) ), taskView, SLOT( invalidateSelection( Tree* ) ) );
    }

    // SystemTree
    tree = new SystemTree( cube );
    tree->initialize();
    proxyModel = new TreeModelProxy( tree->getModel() );
    view       = new SystemTreeView( proxyModel, tree->getLabel() );
    fillTreeHash( tree, view, proxyModel );
    treeHash[ SYSTEMTREE ] = tree;
    connect( proxyModel, SIGNAL( filteringEnded() ), view, SLOT( filteringEnded() ) ); // required for filtering: keep status of expanded items
}

void
TabManager::fillTreeHash( Tree* tree, TreeView* view, TreeModelProxy* proxyModel )
{
    treeViewHash[ tree ] = view;
    trees.append( tree );
    proxyModels.append( proxyModel );
}

/**
 * @brief TabManager::reinit
 * recalculates the values of all trees and updates the display
 */
void
TabManager::reinit()
{
    QList<Tree*> left, right;
    Tree*        leftTree = getActiveTree( order.first() );

    getNeighborTrees( left, right, leftTree );

    // select the items of the tree on the left => recalculate all trees on the right
    treeItemSelected( leftTree );

    // values of the first tree have to be calculated, as they are not calculated on selection
    leftTree->calculateValues( left, right );

    foreach( TreeView * view, getViews() )
    {
        view->updateValues();
    }
}

/**
 * @brief TabManager::reinit
 * Recalculates the value of the given item and the dependent values of other trees, if required.
 */
void
TabManager::reinit( TreeItem* item )
{
    item->invalidate();

    if ( item->isSelected() )
    {
        treeItemSelected( item->getTree() ); // recalculate dependent values on the right
    }

    QList<Tree*> left;
    QList<Tree*> right;
    getNeighborTrees( left, right, item->getTree() );
    item->getTree()->calculateValues( left, right );
}

void
TabManager::cubeClosed()
{
    this->setVisible( false );          // ensure that no repaint events are sent any more

    PluginManager::getInstance()->disconnect();

    foreach( Future * future, futures.values() ) // cancel parallel calculations
    {
        future->cancelCalculation( true );
        delete future;
    }
    futures.clear();

    foreach( TabWidget * tabWidget, tabWidgets )
    {
        tabWidget->cubeClosed();
    }
    foreach( Tree * tree, trees )
    {
        delete tree;
    }
    foreach( QAbstractItemModel * model, proxyModels )
    {
        delete model;
    }

    trees.clear();
    proxyModels.clear();
    treeViewHash.clear();

    this->setVisible( true );
    isInitialized = false;
}

QList<DisplayType>
TabManager::getOrder() const
{
    return order;
}

void
TabManager::setOrder( const QList<DisplayType>& value )
{
    if ( value == order )
    {
        return; // order didn't change
    }
    this->order = value;

    if ( trees.size() > 0 )
    {
        // reorder widgets
        foreach( DisplayType tabType, order )
        {
            tabWidgets[ tabType ]->setOrder( order );
            this->addWidget( tabWidgets[ tabType ] );
        }
        // With changed order, all tree values are invalid and need to
        // be recomputed
        foreach( Tree * tree, trees )
        {
            tree->invalidateItems();
        }

        // set values according to new position
        reinit();

        PluginManager::getInstance()->setTabOrder( order ); // notify plugins
    }
}

/**
 * returns the TabWidget which corresponds to the given TabWidgetType
 * (METRICTAB, CALLTAB, SYSTEMTAB)
 */
TabWidget*
TabManager::getTab( DisplayType tabType ) const
{
    return tabWidgets[ tabType ];
}

/**
 * returns the tree which corresponds to the given TreeType
 * (METRICTREE, CALLTREE, FLATTREE, TASKTREE, SYSTEMTREE)
 */
Tree*
TabManager::getTree( TreeType type ) const
{
    return treeHash[ type ];
}

/**
 *  returns the active tree of the given tab
 */
Tree*
TabManager::getActiveTree( DisplayType tabType ) const
{
    if ( treeViewHash.isEmpty() )
    {
        return nullptr;
    }
    return tabWidgets[ tabType ]->getActiveTreeView()->getTree();
}

/**
 * returns the active TreeView which corresponds to the given TreeType
 * (METRICTREE, CALLTREE, SYSTEMTREE)
 */
TreeView*
TabManager::getActiveView( DisplayType type ) const
{
    return treeViewHash[ getActiveTree( type ) ];
}

TreeView*
TabManager::getView( Tree* tree ) const
{
    return treeViewHash[ tree ];
}

void
TabManager::initialize()
{
    // bring the default system plugin tab to front
    TabWidget* system = getTab( SYSTEM );
    if ( system && !defaultPlugin.isEmpty() )
    {
        system->setDefaultPlugin( defaultPlugin );
        system->toFront( PluginList::getCubePlugin( defaultPlugin ) );
    }

    // check that at least one tree item is selected, otherwise select first
    for ( TreeView* view : treeViewHash.values() )
    {
        Tree* tree = view->getTree();
        if ( tree->getSelectionList().size() == 0 )  // nothing selected
        {
            TreeItem* first = tree->getRootItem()->getChildren().first();
            view->selectItem( first, false );
        }
    }

    // connect signals after all selections have been done
    foreach( Tree * tree, trees )
    {
        TreeView*   view    = treeViewHash[ tree ];
        DisplayType tabType = tree->getType();

        connect( tree, SIGNAL( calculationFinished() ), tabWidgets[ tabType ], SLOT( valuesChanged() ) );
        connect( tree, SIGNAL( calculationFinished() ), view, SLOT( updateValues() ) );
        connect( tree, SIGNAL( calculationFinished() ), this, SLOT( updateStatus() ) );
        connect( tree, SIGNAL( itemSelected( TreeItem*, bool ) ), view, SLOT( selectItem( TreeItem*, bool ) ) );
        connect( tree, SIGNAL( itemDeselected( TreeItem* ) ), view, SLOT( deselectItem( TreeItem* ) ) );
        connect( tree, SIGNAL( itemExpanded( TreeItem*, bool ) ), view, SLOT( expandItem( TreeItem*, bool ) ) );
        connect( view, SIGNAL( selectionChanged( Tree* ) ), this, SLOT( treeItemSelected( Tree* ) ) );
        connect( view, SIGNAL( itemExpanded( TreeItem*, bool ) ), this, SLOT( treeItemExpanded( TreeItem*, bool ) ) );
        connect( view, SIGNAL( recalculateRequest( Tree* ) ), this, SLOT( recalculateTree( Tree* ) ) );
        connect( view, SIGNAL( calculateRequest( QList<TreeItem*> ) ),
                 this, SLOT( calculateTreeItems( QList<TreeItem*>) ) );
        connect( PluginManager::getInstance(), SIGNAL( recalculateRequest( Tree* ) ),
                 this, SLOT( recalculateTree( Tree* ) ) );
    }

    reinit();  // calculate tree data and display it

    isInitialized = true;
}

// -------------- settings -------------------

void
TabManager::loadGlobalSettings( QSettings& settings )
{
    settings.beginGroup( "TabManager" );
    // save defaultPlugin and handle it in TabManager::initialize() after PluginServices are initialised and the tabs are visible
    defaultPlugin = settings.value( "DefaultPlugin", "" ).toString();
    settings.endGroup();
}

void
TabManager::saveGlobalSettings( QSettings& settings )
{
    settings.beginGroup( "TabManager" );
    TabWidget* system = getTab( SYSTEM );
    if ( system && !system->getDefaultPlugin().isEmpty() )
    {
        settings.setValue( "DefaultPlugin", system->getDefaultPlugin() );
    }
    settings.endGroup();
}

void
TabManager::loadExperimentSettings( QSettings& settings )
{
    settings.beginGroup( "TabManager" );


    /* load initial selections */
    for ( Tree* tree : trees )
    {
        TreeView* view = treeViewHash[ tree ];
        settings.beginGroup( tree->getLabel().remove( ' ' ) );
        view->loadExperimentSettings( settings );
        settings.endGroup();
    }

    /* update order and activate initial selections */
    QList<DisplayType> sorder;
    QString            orderStr = settings.value( "TabOrder", "0 1 2" ).toString();

    foreach( QString s, orderStr.split( " ", SkipEmptyParts ) )
    {
        sorder.append( static_cast<DisplayType> ( s.toInt() ) );
    }
    if ( order != sorder )
    {
        setOrder( sorder );
    }

    foreach( TabWidget * widget, tabWidgets )
    {
        DisplayType display = widget->getDisplayType();
        QString     id      = QString::number( display );

        QString tabLabel = settings.value( "ActiveTab" + id, "" ).toString();
        getTab( display )->toFront( tabLabel );

        ValueModus valueModus = ( ValueModus )settings.value( "ValueModus" + id, 0 ).toInt();
        getTab( display )->setValueModus( valueModus );
    }
    QList<int>      sizes; // splitter sizes
    QList<QVariant> qsizes;
    qsizes = settings.value( "splitter" ).toList();
    if ( qsizes.size() == 3 )
    {
        foreach( QVariant qsize, qsizes )
        {
            sizes.append( qsize.toInt() );
        }
        this->setSizes( sizes );
    }

    settings.endGroup(); // TabManager
}

void
TabManager::saveExperimentSettings( QSettings& settings )
{
    settings.beginGroup( "TabManager" );

    QString orderStr;
    foreach( uint i, order )
    {
        orderStr += QString::number( static_cast<DisplayType> ( i ) ) + " ";
    }
    settings.setValue( "TabOrder", orderStr );

    for ( Tree* tree : trees )
    {
        // select first item, or item given by preferences
        TreeView* view = treeViewHash[ tree ];

        settings.beginGroup( tree->getLabel().remove( ' ' ) );
        view->saveExperimentSettings( settings );
        settings.endGroup();
    }

    foreach( TabWidget * widget, tabWidgets )
    {
        QString id = QString::number( widget->getDisplayType() );
        settings.setValue( QString( "ActiveTab" ) + id, widget->currentTabInterface()->label() );
        settings.setValue( "ValueModus" + id, getTab( widget->getDisplayType() )->getValueModus() );
    }

    QList<int>      sizes = this->sizes(); // splitter sizes
    QList<QVariant> qsizes;
    foreach( int size, sizes )
    {
        qsizes.append( size );
    }
    settings.setValue( "splitter", qsizes );

    settings.endGroup(); // TabManager
}

QString
TabManager::settingName()
{
    return "TabManager";
}

// -------------- end settings ----------------

// update status line after all calculations have been finished
void
TabManager::updateStatus()
{
    for ( Tree* tree: trees )
    {
        if ( futures[ tree ]->isRunning() )
        {
            return;
        }
    }
    Globals::setStatusMessage( tr( "Ready" ), DelayedInformation );
}

void
TabManager::updateTreeItemProperties()
{
    foreach( Tree * tree, trees )
    {
        tree->invalidateItemLabel();
    }
    foreach( Tree * tree, trees )
    {
        tree->updateItems();
    }
    foreach( TreeView * view, treeViewHash.values() )
    {
        view->updateValues();
    }
}

void
TabManager::updateTreeItems()
{
    foreach( TreeView * view, treeViewHash.values() )
    {
        if ( view != NULL )
        {
            view->updateValues();
        }
    }
}

/**
 * @brief TabManager::getNeighborTrees
 * @param left is filled with trees left of the tree current
 * @param right is filled with trees right of the tree current
 */
void
TabManager::getNeighborTrees( QList<Tree*>& left, QList<Tree*>& right, Tree* current ) const
{
    int  idx   = 0;
    bool found = false;

    for ( idx = 0; idx < order.size(); idx++ )
    {
        Tree* tree = getActiveTree( order[ idx ] );

        if ( tree == current )
        {
            found = true;
        }
        else
        {
            if ( !found )
            {
                left.append( tree );
            }
            else
            {
                right.append( tree );
            }
        }
    }
}

// todo: also recalculate only required subtree
void
TabManager::recalculateTree( Tree* tree )
{
    QList<Tree*> left;
    QList<Tree*> right;
    getNeighborTrees( left, right, tree );

    if ( !left.empty() )
    {
        Tree* leftTree = left.last();
        treeItemSelected( leftTree );
    }
    else
    {
        reinit();
    }
}

void
TabManager::calculateTreeItems( QList<TreeItem*> items )
{
    QList<Tree*> left;
    QList<Tree*> right;
    Tree*        current = items.first()->getTree();
    getNeighborTrees( left, right, current );

    current->calculateValues( left, right, items );
}

QString
TabManager::getDefaultPluginName() const
{
    return defaultPlugin;
}

/**
 * recalculates the values of all trees right of the selected one
 */
void
TabManager::treeItemSelected( Tree* selectedTree )
{
    // OMP special case: exclusive metrics and flat tree, which contains task tree items
    if ( selectedTree->getType() == METRIC )
    {
        for ( Tree* tree : trees )
        {
            FlatTree* flat = dynamic_cast<FlatTree*>( tree );
            if ( flat )
            {
                flat->metricSelected( selectedTree->getSelectionList() );
            }
        }
    }

    QList<Tree*> left;
    QList<Tree*> right;
    getNeighborTrees( left, right, selectedTree );

    PluginManager::getInstance()->cancelCalculations(); // previous values are obsolete -> cancel dependant plugin calculations

    // invalidate all trees to the right
    foreach( Tree * tree, right )
    {
        tree->invalidateItems();
        getView( tree )->updateValues();
        tree->getStatistics()->invalidate();
        DisplayType tabType = tree->getType();
        tabWidgets[ tabType ]->updateValueWidget(); // invalidate value widget
    }

    Globals::debug( "TabManager::treeItemSelected" ) << selectedTree->getLastSelection()->getLabel() << Qt::endl;
    Future::startTimer();

    /* call computeValues for all trees right of the selected one */
    left.append( selectedTree );
    foreach( Tree * tree, right )
    {
        tree->calculateValues( left, right );
        left.append( tree );
    }
    DisplayType                                        tabType = selectedTree->getType();
    selectedTree->getLastSelection()->isCalculated() ? selectedTree->getStatistics()->update() : selectedTree->getStatistics()->invalidate();
    tabWidgets[ tabType ]->updateValueWidget();  // update value widget of selected item
    getView( selectedTree )->updateInfoWidget(); // update info widget of the selected item


    activateStaticMarker();
}

/**
 * Calculates the expanded values. If the expanded item is selected, the trees values right of the
 * current one have to be recalculated.
 * @param expanded if true, the item has been expanded, otherwise it has been collapsed
 */
void
TabManager::treeItemExpanded( TreeItem* item, bool )
{
    //qDebug() << "TabManager::treeItemExpanded" << item->getName() << expanded;
    Tree*        tree = item->getTree();
    QList<Tree*> left;
    QList<Tree*> right;
    getNeighborTrees( left, right, tree );

    item->invalidateLabel();
    {
        tree->updateItems();

        // update view and check if items have to be hidden
        TreeView* view = treeViewHash[ tree ];
        view->updateValues();

        // update value widget
        tree->getStatistics()->update();
        DisplayType tabType = tree->getType();
        tabWidgets[ tabType ]->updateValueWidget();
    }

    if ( tree->getType() != SYSTEM ) // already calculated
    {
        tree->calculateValues( left, right );
    }
}



void
TabManager::updateColors()
{
    if ( isInitialized )
    {
        foreach( TabWidget * tab, tabWidgets )
        {
            tab->valuesChanged(); // notify plugin tabs
        }
    }
}

QList<TabWidget*>
TabManager::getTabWidgets() const
{
    return tabWidgets;
}

void
TabManager::addStaticMarker( const TreeItemMarker* marker, TreeItem* metric, TreeItem* call, TreeItem* system )
{
    if ( markerHash.contains( marker ) )
    {
        MarkerTripletList& list = markerHash[ marker ];
        list.addTriplet( metric, call, system );
    }
    else
    {
        MarkerTripletList list;
        list.addTriplet( metric, call, system );
        markerHash.insert( marker, list );
    }
}

void
TabManager::removeStaticMarker( DisplayType type )
{
    foreach( const TreeItemMarker * marker, markerHash.keys() )
    {
        foreach( QList<TreeItem*> triplet, markerHash.value( marker ) )
        {
            bool isInTree = false;
            foreach( TreeItem * item, triplet )
            {
                if ( item && item->getDisplayType() == type )
                {
                    isInTree = true;
                    break;
                }
            }
            if ( isInTree )
            {
                foreach( TreeItem * item, triplet )
                {
                    if ( item )
                    {
                        item->removeMarker( marker );
                    }
                }
            }
        }
    }
}

void
TabManager::removeStaticMarker( const TreeItemMarker* marker )
{
    foreach( QList<TreeItem*> triplet, markerHash.value( marker ) )
    foreach( TreeItem * item, triplet )
    if ( item )
    {
        item->removeMarker( marker );
    }
    markerHash.remove( marker );
}

void
TabManager::activateStaticMarker()
{
    // delete all static marker
    foreach( const TreeItemMarker * marker, markerHash.keys() )
    foreach( QList<TreeItem*> triplet, markerHash.value( marker ) )
    foreach( TreeItem * item, triplet )
    if ( item )
    {
        item->removeMarker( marker );
    }

    // set marker
    foreach( const TreeItemMarker * marker, markerHash.keys() )
    {
        const MarkerTripletList& list = markerHash.value( marker );
        for ( int tabNr = 0; tabNr < order.size(); tabNr++ )
        {
            DisplayType tabType = order[ tabNr ];
            foreach( QList<TreeItem*> triplet, list )
            {
                TreeItem* currentItem = triplet.at( tabType );
                if ( currentItem )
                {
                    // check if all triplet parts left of the current tab are selected
                    bool setMarker = true;
                    for ( int leftTab = 0; leftTab < tabNr; leftTab++ )
                    {
                        Tree*     tree     = tabWidgets[ order[ leftTab ] ]->getActiveTreeView()->getTree();
                        TreeItem* selected = tree->getSelectionList().first();
                        TreeItem* marked   = triplet.at( order[ leftTab ] );
                        if ( !marked || marked != selected )
                        {
                            setMarker = false;
                            break;
                        }
                    }
                    if ( setMarker )
                    {
                        bool rightMarkerExists = false;
                        for ( int rightTab = tabNr + 1; rightTab < order.size(); rightTab++ )
                        {
                            if ( triplet.at( order[ rightTab ] ) != 0 )
                            {
                                rightMarkerExists = true;
                                break;
                            }
                        }
                        if ( rightMarkerExists )
                        {
                            currentItem->setMarker( marker, true ); // mark only as dependency
                        }
                        else
                        {
                            currentItem->setMarker( marker );
                        }
                    }
                }
            } // all marker triplet
        }     // all tabs
    }
}

/**
 * @brief TabManager::updateValueViews has to be called, if the value view changes by configuration or if a metric
 * with another view is selected
 */
void
TabManager::updateValueViews()
{
    foreach( TreeView * view, treeViewHash.values() )
    {
        view->updateRowHeight(); // implies view->update
    }
}

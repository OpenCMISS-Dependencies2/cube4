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

#include <QDebug>
#include <QStatusBar>

#include <assert.h>
#include "CubeServices.h"
#include "CubeProxy.h"

#include "AggregatedTreeItem.h"
#include "PluginServices.h"
#include "PluginManager.h"
#include "TabInterface.h"
#include "MetricTree.h"
#include "DefaultCallTree.h"
#include "SystemTree.h"
#include "ValueWidget.h"
#include "Globals.h"
#include "TreeView.h"
#include "TreeItem.h"
#include "TabInterface.h"
#include "SystemTreeView.h"
#include "TabWidget.h"
#include "CubePlugin.h"
#include "PluginList.h"
#include "TreeItemMarker.h"
#include "ColorMap.h"
#include "Globals.h"
#include "Task.h"
#include "ProgressBar.h"
#include "Future.h"

using namespace std;
using namespace cubegui;
using namespace cubepluginapi;

PluginServices::PluginServices( PluginManager* pm,
                                const QString& name )
{
    this->pm               = pm;
    this->pluginName       = name;
    this->currentTabWidget = 0;
    this->settingsHandler  = 0;
}

PluginServices::~PluginServices()
{
    removeMarker();
    foreach( TreeItemMarker * marker, markerList )
    {
        delete marker;
    }
    foreach( Future * future, futureList )
    {
        delete future;
    }
}

void
PluginServices::toFront( TabInterface* tab )
{
    currentTabWidget->toFront( tab );
}

int
PluginServices::defineSystemTab( const QString& label, TabType tabType )
{
    currentTabWidget = pm->tabManager->getTab( SYSTEM );
    return currentTabWidget->createSystemTabWidget( label, tabType );
}

void
PluginServices::addTab( DisplayType type, TabInterface* tab, int tabWidgetID )
{
    assert( pm->tabManager != NULL );

    if ( !tab->widget() )
    {
        qDebug() << tr( "Warning: TabWidget::addTab has tried to add null widget" );
        return;
    }

    if ( type == SYSTEM )
    {
        currentTabWidget = pm->tabManager->getTab( type );
        currentTabWidget->addPluginTab( tab, pluginName, tabWidgetID );
        currentTabs.append( tab );
        CubePlugin* plugin = PluginList::getCubePlugin( pluginName );
        tab->widget()->setWhatsThis( plugin->getHelpText() );
    }
    else
    {
        qDebug() << tr( "plugins for METRICTAB and CALLTAB are not implemented yet" );
    }
}

void
PluginServices::removeTab( TabInterface* tab )
{
    currentTabWidget->removePluginTab( tab );
    currentTabs.removeAll( tab );
}

void
PluginServices::enableTab( TabInterface* tab, bool enabled )
{
    if ( currentTabWidget ) // if the plugin hasn't (yet) created a tab, this call is ignored
    {
        currentTabWidget->enableTab( tab, enabled );
    }
}

void
PluginServices::closeTabs()
{
    foreach( TabInterface * ti, currentTabs )
    {
        currentTabWidget->removePluginTab( ti );
    }
}

void
PluginServices::clearValueWidget()
{
    if ( !currentTabWidget )
    {
        qDebug() << tr( "Warning: clearValueWidget is only available for tabs" );
        return;
    }
    currentTabWidget->getValueWidget()->clear();
}


void
PluginServices::updateValueWidget( TabInterface* tc, double minValue, double maxValue, double value,
                                   double minAbsValue, double maxAbsValue, double absValue,
                                   double mean, double varianceSqrt )
{
    if ( !currentTabWidget )
    {
        qDebug() << tr( "Warning: updateValueWidget is only available for tabs" );
        return;
    }
    currentTabWidget->getTab( tc )->getValueWidget()->update( value, minValue, maxValue,
                                                              absValue, minAbsValue, maxAbsValue,
                                                              mean, varianceSqrt );
}

void
PluginServices::addSettingsHandler( SettingsHandler* s )
{
    settingsHandler = s;
}

void
PluginServices::addColorMap( ColorMap* map )
{
    pm->colormaps.insert( this, map );
}

void
PluginServices::addValueView( ValueView* view )
{
    pm->valueViews.insert( this, view );
}

QAction*
PluginServices::addContextMenuItem( DisplayType type, const QString& menuItemText )
{
    assert( pm->cube );

    Tree*     tree = pm->tabManager->getActiveTree( type );
    TreeView* view = pm->tabManager->getView( tree );

    QAction* newItem = new QAction( menuItemText, tree );
    view->insertPluginMenuItem( newItem );

    return newItem;
}


QMenu*
PluginServices::enablePluginMenu()
{
    assert( pm->cube );

    QMenu* parentMenu = pm->pluginMenus.value( pluginName );
    parentMenu->menuAction()->setVisible( true );

    return parentMenu;
}

void
PluginServices::addToolBar( QToolBar* toolbar, TabInterface* tab )
{
    assert( pm->cube );
    pm->toolBarHash.insert( this, toolbar );
    if ( tab )
    {
        if ( currentTabWidget )
        {
            currentTabWidget->addToolBar( toolbar, tab );
        }
        else
        {
            qDebug() << tr( "Warning: cannot create toolbar, TabInterface has to be added first" );
        }
    }
    else
    {
        pm->mainWindow->addToolBar( toolbar );
        toolbar->setVisible( true );
    }
}

void
PluginServices::removeToolBar( QToolBar* toolbar, TabInterface* tab )
{
    assert( pm->cube );
    pm->toolBarHash.remove( this, toolbar );
    if ( tab )
    {
        currentTabWidget->removeToolBar( toolbar, tab );
    }
    else
    {
        pm->mainWindow->removeToolBar( toolbar );
    }
}

cube::CubeProxy*
PluginServices::getCube() const
{
    return pm->cube;
}


bool
PluginServices::hasIterations()
{
    DefaultCallTree* call = static_cast<DefaultCallTree*> ( getActiveTree( CALL ) );

    return call->getLoopRootItem() != 0;
}

Future*
PluginServices::createFuture( TabInterface* tab )
{
    if ( !currentTabWidget )
    {
        cerr << tr( "Error: create tab widget before future is created!" ).toUtf8().data();
    }
    assert( currentTabWidget );
    Future* future = new Future( currentTabWidget, tab );
    futureList.append( future );
    return future;
}

QStringList
PluginServices::getBookmarks()
{
    return Globals::getSettings()->getBookmarks();
}

void
PluginServices::loadBookmark( const QString& name, bool loadGlobals )
{
    Globals::getSettings()->loadExperimentSettings( name, loadGlobals );
}

QString
PluginServices::getBookmarkDescription( const QString& name )
{
    return Globals::getSettings()->getBookmarkDescription( name );
}

QList<TabInterface*>
PluginServices::getTabList()
{
    return currentTabs;
}

const QString&
PluginServices::getCubeFileName() const
{
    return pm->cubeFileName;
}

QString
PluginServices::getCubeBaseName() const
{
    assert( pm->cube );
    return QString( cube::services::get_cube_name( getCubeFileName().toStdString() ).c_str() );
}

QString
PluginServices::getStatName() const
{
    string cubeFileName = getCube()->get_cubename();
    string statFileName = getCube()->get_statistic_name();

    string statisticsFileName = "";
    if ( statFileName != "" )
    {
        statisticsFileName =  cube::services::dirname( cubeFileName ) + "/" +  statFileName;
    }
    else
    {
        statisticsFileName = cube::services::get_cube_name( cubeFileName ) + ".stat";
    }

    return QString::fromStdString( statisticsFileName );
}

QWidget*
PluginServices::getParentWidget() const
{
    return pm->mainWindow;
}

const QList<DisplayType>&
PluginServices::getOrder() const
{
    return pm->order;
}

TreeItem*
PluginServices::getSelection( DisplayType type ) const
{
    assert( pm->cube );
    Tree* tree = pm->tabManager->getActiveTree( type );

    return tree->getLastSelection();
}

const QList<TreeItem*>&
PluginServices::getSelections( DisplayType type ) const
{
    assert( pm->cube );
    Tree* tree = pm->tabManager->getActiveTree( type );
    return tree->getSelectionList();
}

TreeItem*
PluginServices::getMetricTreeItem( std::string metricId ) const
{
    assert( pm->cube );
    MetricTree* tree = static_cast<MetricTree*> ( getActiveTree( METRIC ) );
    return tree->getTreeItem( metricId );
}

TreeItem*
PluginServices::getCallTreeItem( uint32_t cnodeId ) const
{
    assert( pm->cube );
    TreeItem*        item = nullptr;
    DefaultCallTree* tree = static_cast<DefaultCallTree*> ( getTree( DEFAULTCALLTREE ) );
    item = tree->getTreeItem( cnodeId );
    if ( !item )
    {
        DefaultCallTree* tree = static_cast<DefaultCallTree*> ( getTree( TASKTREE ) );
        return tree->getTreeItem( cnodeId );
    }
    return item;
}

TreeItem*
PluginServices::getSystemTreeItem( uint32_t sysId ) const
{
    assert( pm->cube );
    SystemTree* tree = static_cast<SystemTree*> ( getTree( SYSTEMTREE ) );
    return tree->getTreeItem( sysId );
}

const QList<TreeItem*>&
PluginServices::getActiveSubset() const
{
    assert( pm->cube );
    SystemTreeView* view = static_cast<SystemTreeView*> ( pm->tabManager->getActiveView( SYSTEM ) );
    return view->getActiveSubset();
}
int
PluginServices::getActiveSubsetIndex() const
{
    assert( pm->cube );
    SystemTreeView* view = static_cast<SystemTreeView*> ( pm->tabManager->getActiveView( SYSTEM ) );
    return view->getActiveSubsetIndex();
}
void
PluginServices::setActiveSubset( int index )
{
    assert( pm->cube );
    SystemTreeView* view = static_cast<SystemTreeView*> ( pm->tabManager->getActiveView( SYSTEM ) );
    return view->setActiveSubset( index );
}
QStringList
PluginServices::getSubsetLabelList() const
{
    assert( pm->cube );
    SystemTreeView* view = static_cast<SystemTreeView*> ( pm->tabManager->getActiveView( SYSTEM ) );
    return view->getSubsetLabelList();
}

void
PluginServices::addMarker( const TreeItemMarker* marker, TreeItem* metric, TreeItem* call, TreeItem* system )
{
    pm->tabManager->addStaticMarker( marker, metric, call, system );
}

void
PluginServices::addMarker( TreeItem* item, const TreeItemMarker* marker, bool isDependency )
{
    item->setMarker( marker, isDependency );
    markedItemList.append( item );
}

void
PluginServices::removeMarker( TreeItem* item, const TreeItemMarker* marker )
{
    item->removeMarker( marker );
    markedItemList.removeOne( item );
}

void
PluginServices::removeMarker( DisplayType type )
{
    foreach( TreeItem * item, markedItemList )
    {
        if ( item->getDisplayType() == type )
        {
            foreach( TreeItemMarker * marker, markerList )
            {
                item->removeMarker( marker );
                markedItemList.removeOne( item );
            }
        }
    }
    // also delete static marker
    pm->tabManager->removeStaticMarker( type );
}

void
PluginServices::removeMarker()
{
    foreach( TreeItem * item, markedItemList )
    {
        foreach( TreeItemMarker * marker, markerList )
        {
            item->removeMarker( marker );
        }
    }
    markedItemList.clear();

    // also delete static marker
    foreach( TreeItemMarker * marker, markerList )
    {
        pm->tabManager->removeStaticMarker( marker );
    }
}

void
PluginServices::resizeTreeItemMarker( int iconHeight )
{
    foreach( TreeItemMarker * marker, markerList )
    {
        marker->setSize( iconHeight );
    }
}

const TreeItemMarker*
PluginServices::getTreeItemMarker( const QString& label, const QList<QPixmap>& icons, bool isInsignificant,
                                   MarkerLabel* markerLabel )
{
    TreeItemMarker* marker = new TreeItemMarker( label, icons, isInsignificant, markerLabel );
    markerList.append( marker );
    return marker;
}

ValueModus
PluginServices::getValueModus() const
{
    if ( !currentTabWidget )
    {
        return ABSOLUTE_VALUES;                      // only valid for tabs
    }
    return currentTabWidget->getValueModus();
}


bool
PluginServices::intMetricSelected() const
{
    assert( pm->cube );
    foreach( TreeItem * item, getSelections( METRIC ) )
    {
        if ( !static_cast<cube::Metric*>( item->getCubeObject() )->isIntegerType() )
        {
            return false;
        }
    }
    return true;
}


/**
 * @brief PluginServices::selectItem
 * @param item the tree item which is selected/deselected
 * @param add if false, the previously selected items are deselected
 */
void
PluginServices::selectItem( TreeItem* newSelection, bool add )
{
    assert( pm->cube );
    Tree* tree     = newSelection->getTree();
    bool  deselect = false;

    if ( add ) // check if one previously implicitly (collapsed parent) selected element has been deselected
    {
        // check if selected element is child of a selected and collapsed parent
        TreeItem* parentCollapsed = 0;
        TreeItem* item            = newSelection->getParent();
        while ( item != 0 ) // get uppermost selected and collapsed parent node
        {
            if ( !item->isExpanded() && item->isSelected() )
            {
                parentCollapsed = item;
                break;
            }
            item = item->getParent();
        }
        // deselect previously implicitly selected item
        if ( parentCollapsed != 0 )
        {
            deselect = true;
            parentCollapsed->setExpanded( true );
            // All children off a selected collapsed parent item are implicitly selected.
            // If such a child gets deselected, its parents have to be expanded and
            // deselected, and it's siblings have to be selected.
            item = newSelection;
            while ( item != parentCollapsed )
            {
                item->setExpanded( true );

                // select all siblings of the deselected element
                foreach( TreeItem * sibling, item->getParent()->getChildren() )
                {
                    if ( sibling != item )
                    {
                        sibling->select( true );
                        sibling->setExpanded( false );
                    }
                }
                item = item->getParent();
            }
            // expand and deselect the parent item
            parentCollapsed->deselect();
        }

        // deselect previously explicitly selected item
        if ( tree->getSelectionList().contains( newSelection ) )
        {
            newSelection->deselect();
            deselect = true;
        }
    }
    else // deselect all previously selected items and select the new one
    {
        newSelection->select( false );
    }

    if ( !deselect ) // item has been selected
    {
        QString selection = newSelection->getLabel();
        setMessage( QString( tr( "Selected \"" ) ).append( selection ) );

        TreeItem* item = newSelection->getParent();
        while ( item != NULL )
        {
            if ( !item->isExpanded() )
            {
                item->setExpanded( true );
            }
            item = item->getParent();
        }
        newSelection->select( true );
    }
}

void
PluginServices::setMessage( const QString& str, MessageType type )
{
    Globals::setStatusMessage( str, type );
}


QColor
PluginServices::getColor( double value, double minValue, double maxValue, bool whiteForZero ) const
{
    return Globals::getColor( value, minValue, maxValue, whiteForZero );
}

QPair<QString, QString>
PluginServices::formatNumberAndUnit( double          value,
                                     const QString&  unit,
                                     PrecisionFormat format,
                                     bool            integerType ) const
{
    return Globals::formatNumberAndUnit( value, unit, integerType, format );
}

QString
PluginServices::formatNumber( double          value,
                              PrecisionFormat format,
                              bool            integerType ) const
{
    return Globals::formatNumber( value, integerType, format );
}

QString
PluginServices::formatNumber( double value, bool integerType ) const
{
    return Globals::formatNumber( value, integerType, FORMAT_DEFAULT );
}

// For the calltree and systemtree, TopLevelItems actually refer to the visible root nodes in CUBE.
const QList<TreeItem*>
PluginServices::getTopLevelItems( DisplayType type ) const
{
    assert( pm->cube );
    return getActiveTree( type )->getRootItem()->getChildren();
}

void
PluginServices::setGlobalValue( const QString& name, const QVariant& value, bool notifyMyself )
{
    pm->setGlobalValue( name, value, notifyMyself ? 0 : this );
}

QVariant
PluginServices::getGlobalValue( const QString& name ) const
{
    return pm->globalValues.value( name );
}

void
PluginServices::recalculateTreeItems()
{
    pm->tabManager->reinit();
}

void
PluginServices::recalculateTreeItem( TreeItem* item )
{
    pm->tabManager->reinit( item );
}

void
PluginServices::updateTreeItems()
{
    for ( TreeView* view : pm->tabManager->getViews() )
    {
        Tree*                   tree  = view->getTree();
        const QList<TreeItem*>& items = tree->getItems();
        foreach( TreeItem * item, items )
        {
            item->setValues( item->totalValue_adv, item->ownValue_adv );
        }
        view->updateValues();
    }
}

void
PluginServices::updateTreeView( DisplayType type )
{
    assert( pm->cube );
    TreeView* view = pm->tabManager->getView( pm->tabManager->getActiveTree( type ) );
    view->updateValues();
}

void
PluginServices::updateTreeView( TreeType type )
{
    assert( pm->cube );
    TreeView* view = pm->tabManager->getView( pm->tabManager->getTree( type ) );
    view->updateValues();
}

QTextStream&
PluginServices::debug()
{
    QTextStream& stream = Globals::debug( pluginName );
    return stream;
}

QTextStream&
PluginServices::debug( const QString& sender )
{
    QTextStream& stream = Globals::debug( sender );
    return stream;
}

bool
PluginServices::getUserDefinedMinValues( DisplayType type, double& min, double& max ) const
{
    Tree* tree = getActiveTree( type );
    min = tree->getUserDefinedMinValue();
    max = tree->getUserDefinedMaxValue();
    return tree->hasUserDefinedMinMaxValues();
}

void
PluginServices::setUserDefinedMinMaxValues( DisplayType type )
{
    pm->tabManager->getView( getActiveTree( type ) )->onMinMaxValues();
}


void
PluginServices::addMetric( cube::Metric* metric, TreeItem* parent )
{
    if ( metric == NULL )
    {
        return;
    }

    if ( !metric->isGhost() || Globals::optionIsSet( ExpertMode ) )
    {
        QString   name    = QString::fromStdString( metric->get_disp_name() );
        Tree*     tree    = this->getTree( METRICTREE );
        TreeItem* newItem = new TreeItem( tree, name, METRICITEM, metric );
        tree->getModel()->addItem( newItem, parent );

        pm->recalculateTree( tree );
    }
}

void
PluginServices::removeMetric( TreeItem* item )
{
    Tree*           tree = this->getTree( METRICTREE );
    const TreeItem* root = tree->getRootItem();
    if ( root->getChildren().size() > 1 )
    {
        TreeItem* newSelection = item != root->getChildren().at( 0 ) ? root->getChildren().at( 0 ) : root->getChildren().at( 1 );
        selectItem( newSelection, false );
        tree->getModel()->removeItem( item );

        cube::Metric* metric = dynamic_cast<cube::Metric*> ( item->getCubeObject() );
        metric->setActive( false );
    }
    else
    {
        Globals::setStatusMessage( tr( "MetricTree: at least one metric is required" ), Error );
    }
}

void
PluginServices::updateMetric( TreeItem* item )
{
    pm->recalculateTree( item->getTree() );
}

Tree*
PluginServices::getActiveTree( DisplayType type ) const
{
    assert( pm->cube );
    return pm->tabManager->getActiveTree( type );
}

Tree*
PluginServices::getTree( TreeType type ) const
{
    assert( pm->cube );
    return pm->tabManager->getTree( type );
}

const QList<TreeItem* >&
PluginServices::getTreeItems( DisplayType type ) const
{
    assert( pm->cube );
    return getActiveTree( type )->getItems();
}

const QList<TreeItem* >&
PluginServices::getTreeItems( TreeType type ) const
{
    assert( pm->cube );
    return getTree( type )->getItems();
}

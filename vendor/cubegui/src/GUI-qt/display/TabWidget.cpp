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

#include <QLabel>
#include <QVBoxLayout>
#include <QAction>
#include <QStackedLayout>
#include "TabWidget.h"
#include "ValueWidget.h"
#include "TreeView.h"
#include "Globals.h"
#include "TabInterface.h"
#include "Constants.h"
#include "PluginManager.h"
#include "CubePlugin.h"
#include "PluginList.h"
#include "ValueModusCombo.h"
#include "Tree.h"
#include "CallTree.h"

using namespace cubegui;
using namespace cubepluginapi;

TabWidget::TabWidget( DisplayType type )
{
    mainTabWidget     = nullptr;
    this->displayType = type;
}

/**
 * @brief TabWidget::initialize is called after a cube file has been loaded
 * @param order
 */
void
TabWidget::initialize( const QList<DisplayType>& order )
{
    currentCallView = nullptr;
    valueModus      = ABSOLUTE_VALUES;
    valueCombo      = new ValueModusCombo( displayType );
    valueCombo->setOrder( order );

    connect( valueCombo, SIGNAL( valueModusChanged( ValueModus ) ), this, SLOT( setValueModus( ValueModus ) ) );
    connect( valueCombo, SIGNAL( externalValueModusSelected() ), this, SLOT( externalValueModusSelectedSlot() ) );

    if ( displayType == SYSTEM ) // create nested tab widgets for system tab
    {
        mainTabWidget = new QTabWidget;
        mainTabWidget->setMovable( true );
        mainTabWidget->setTabPosition( QTabWidget::East );
        mainTabWidget->setDocumentMode( true );
#if QT_VERSION >= QT_VERSION_CHECK( 5, 4, 0 )
        mainTabWidget->setTabBarAutoHide( true );
#endif
        QVBoxLayout* mainLayout = new QVBoxLayout();
        mainLayout->setContentsMargins( 0, 0, 0, 0 );
        mainLayout->setSpacing( 0 );
        mainLayout->addWidget( mainTabWidget );
        setLayout( mainLayout );
        createSystemTabWidget( tr( "System View" ) );
        tabWidgetList.append( 0 ); // place holder for OTHER_PLUGIN_TAB
        systemTabChanged( 0 );
        connect( mainTabWidget, SIGNAL( currentChanged( int ) ), this, SLOT( systemTabChanged( int ) ) );
    }
    else // metric and call tabs
    {
        DetachableTabWidget* tab = new DetachableTabWidget();
        mainTabWidget = tab;
        tabWidgetList.append( tab );
        connect( mainTabWidget, SIGNAL( currentChanged( int ) ), this, SLOT( onCurrentChanged() ) );

        mainTabWidget->setUsesScrollButtons( true );
        mainTabWidget->setIconSize( QSize( 20, 20 ) );

        QVBoxLayout* layout = new QVBoxLayout();
        setLayout( layout );
        layout->setContentsMargins( LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN );
        layout->setSpacing( LAYOUT_SPACING );
        layout->addWidget( valueCombo );
        layout->addWidget( mainTabWidget );
    }
    setOrder( order );
}

/** called when cube file is closed; deletes all widget contents */
void
TabWidget::cubeClosed()
{
    delete mainTabWidget;
    delete layout();
    mainTabWidget = 0;
    valueCombo    = 0;

    systemTabs.clear();
    tabWidgetList.clear();
    tabHash.clear();
    treeList.clear();
    order.clear();
    changeInfo.clear();
}

/**
 * slot which is called if right side system tab widget is activated
 */
void
TabWidget::systemTabChanged( int index )
{
    // hide value combo for OTHER_PLUGIN_TAB
    QWidget*             widget    = mainTabWidget->widget( index );
    DetachableTabWidget* systemTab = systemTabs.value( widget );

    if ( systemTab->getType() == DEFAULT_TAB ) // widget contains value combobox...
    {
        // insert contents only if tab becomes active, because valueCombo is used in multiple tabs and above the tab bar,
        // but a widget can only be added to one parent widget
        QWidget* oldParent = valueCombo->parentWidget();
        if ( oldParent )
        {
            oldParent->layout()->removeWidget( valueCombo );
        }
        QVBoxLayout* layout = dynamic_cast<QVBoxLayout*> ( widget->layout() );
        layout->insertWidget( 0, valueCombo );
    }

    onCurrentChanged(); // notify child tab widget
}

void
TabWidget::externalValueModusSelectedSlot()
{
    emit externalValueModusSelected( this );
}

void
TabWidget::setDefaultPlugin( const QString& name )
{
    defaultPlugin = name;
}

QString
TabWidget::getDefaultPlugin() const
{
    return defaultPlugin;
}

DisplayType
TabWidget::getDisplayType() const
{
    return displayType;
}


/** creates tab widget which will be child of the system tab main widget
 * @returns tabID of the created tab widget which can be used in TabWidget::addPluginTab
 */
int
TabWidget::createSystemTabWidget( const QString& label, TabType tabType  )
{
    assert( this->getType() == SYSTEM );
    DetachableTabWidget* tabWidget = new DetachableTabWidget( tabType, defaultPlugin );
    tabWidgetList.append( tabWidget );

    QWidget* container = new QWidget();
    QLayout* layout    = new QVBoxLayout();
    layout->setContentsMargins( LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN, LAYOUT_MARGIN );
    layout->setSpacing( LAYOUT_SPACING );
    container->setLayout( layout );
    systemTabs.insert( container, tabWidget );
    layout->addWidget( tabWidget );

    if ( tabType == OTHER_PLUGIN_TAB )
    {
        mainTabWidget->addTab( container, label );
    }
    else
    {
        // system view tab is created first; put all other tabs between system view and OTHER_PLUGIN_TAB
        mainTabWidget->insertTab( 1, container, label );
    }
    connect( tabWidget, SIGNAL( currentChanged( int ) ), this, SLOT( onCurrentChanged() ) );
    connect( tabWidget, SIGNAL( activated( TabInterface* ) ), this, SLOT( setActiveTab( TabInterface* ) ) ); // detach signal

    return tabWidgetList.size() - 1;                                                                         // ID of the tab widget == listindex
}

void
TabWidget::addPluginTab( TabInterface* tc, const QString& pluginName, int tabWidgetID )
{
    this->addTabInterface( tc, true, tabWidgetID );
    TabContents* tab = tabHash.value( tc );
    tab->getParentTab()->setDetachable( tab, pluginName );

    connect( tab->getParentTab(), SIGNAL( pluginClosed( TabInterface* ) ), this, SLOT( removePluginTab( TabInterface* ) ) );
    connect( tab->getParentTab(), SIGNAL( defaultPluginChanged( QString ) ), this, SLOT( setDefaultPlugin( QString ) ) );
}

void
TabWidget::removePluginTab( TabInterface* ti )
{
    DetachableTabWidget* tabWidget = getParentTabWidget( ti );
    if ( tabWidget )
    {
        tabWidget->removeTabInterface( ti );
    }
    tabHash.remove( ti );
    changeInfo.remove( ti );
}

void
TabWidget::detachTab( TabInterface* ti )
{
    DetachableTabWidget* foundTabWidget = 0;
    foreach( DetachableTabWidget * tabWidget, tabWidgetList )
    {
        for ( int tabIndex = 0; tabIndex < tabWidget->count(); tabIndex++ )
        {
            QWidget* toCheck = tabWidget->widget( tabIndex );
            if ( toCheck == getTab( ti ) )
            {
                foundTabWidget = tabWidget;
                foundTabWidget->detachTab( tabIndex );
                setActiveTab( ti );
                break;
            }
        }
    }
}

void
TabWidget::toFront( const QString& label )
{
    if ( label.isEmpty() )
    {
        return;
    }
    foreach( TabInterface * tc, tabHash.keys() )
    {
        if ( tc->label().contains( label ) )
        {
            toFront( tc );
            break;
        }
    }
}

void
TabWidget::toFront( CubePlugin* plugin )
{
    if ( !plugin )
    {
        return;
    }
    PluginServices* service = PluginManager::getInstance()->getService( plugin->name() );
    if ( service )
    {
        TabInterface* tc = service->getTabList().isEmpty() ? 0 : service->getTabList().first();
        toFront( tc );
    }
}


void
TabWidget::toFront( TabInterface* tc )
{
    DetachableTabWidget* tabWidget = getParentTabWidget( tc );
    if ( !tabWidget )
    {
        return;
    }
    int index = tabWidget->indexOf( getTab( tc ) );

    if ( index >= 0 ) // tab is not detached
    {
        // bring parent tab widget of current tab widget to front
        for ( int i = 0; i < mainTabWidget->count(); i++ )
        {
            DetachableTabWidget* tw = systemTabs.value( mainTabWidget->widget( i ) );
            if ( tabWidget == tw )
            {
                mainTabWidget->setCurrentWidget( mainTabWidget->widget( i ) );
            }
        }

        tabWidget->setCurrentIndex( index );

        if ( displayType == SYSTEM ) // activate outer tab widget
        {
            for ( int i = 0; i < mainTabWidget->count(); i++ )
            {
                if ( mainTabWidget->widget( i ) == tabWidget )
                {
                    mainTabWidget->setCurrentIndex( i );
                }
            }
        }
        updateToolbarStatus( tc ); // ensure that toolbar becomes visible, even if currentChanged is not emitted (while global settings are evaluated)
    }
    else
    {
        QMainWindow* window = dynamic_cast<CloseableMainWindow*>( getTab( tc )->parent() );
        if ( window )
        {
            window->raise();
            window->showNormal();
        }
    }
}

void
TabWidget::addTabInterface( TabInterface* tc, bool detachable, int tabWidgetID )
{
    DetachableTabWidget* tabWidget = tabWidgetList.value( tabWidgetID );

    if ( !tabWidget && tabWidgetID == OTHER_PLUGIN_TAB )
    {
        int id = createSystemTabWidget( tr( "General" ), OTHER_PLUGIN_TAB );
        tabWidgetList.replace( OTHER_PLUGIN_TAB, tabWidgetList.takeAt( id ) );
        tabWidget = tabWidgetList.value( OTHER_PLUGIN_TAB );
    }

    if ( !tabWidget )
    {
        qDebug() << "tried to add system plugin tab to non existent tab widget";
        return;
    }

    int index = tabWidget->indexOf( getTab( tc ) );
    if ( index >= 0 )
    {
        tabWidget->setCurrentIndex( index );
        return;
    }

    TabContents* contents = new TabContents( tc, tabWidget, detachable );
    if ( getType() == SYSTEM && contents->getValueWidget() )
    {
        // set default value widget contents to statistics from system tree
        TreeStatistics* stat = getActiveTreeView()->getTree()->getStatistics();
        contents->getValueWidget()->setStatistics( stat );
    }
    tabWidget->addTab( contents );
    tabHash.insert( tc, contents );
}

void
TabWidget::addTreeTab( TreeView* treeView )
{
    treeList.append( treeView );
    addTabInterface( treeView );
    // set statistics for each tree
    TabContents* contents = tabHash[ treeView ];
    contents->getValueWidget()->setStatistics( treeView->getTree()->getStatistics() );
}

TreeView*
TabWidget::getActiveTreeView()
{
    for ( TreeView* treeView : treeList )
    {
        if ( treeView->getTree()->isActive() )
        {
            return treeView;
        }
    }
    assert( false ); // one tree has to be the active one
    return nullptr;
}

/** Returns currenly active tab interface or null, if no tab is visible.
 *  In case of System tab, the active tab of the active tab bar is returned.
 */
TabInterface*
TabWidget::currentTabInterface() const
{
    DetachableTabWidget* tabWidget = 0;

    if ( !mainTabWidget )
    {
        return NULL;
    }
    else if ( displayType == SYSTEM ) // for system tab: get active tab widget
    {
        tabWidget = systemTabs.value( mainTabWidget->currentWidget() );
    }
    else
    {
        tabWidget = dynamic_cast<DetachableTabWidget*> ( mainTabWidget );
    }
    return tabWidget && tabWidget->count() > 0 ? tabWidget->getTabInterface( tabWidget->currentWidget() ) : 0;
}

/** Returns currenly active tab widget or null, if no tab is visible.
 *  In case of System tab, the active tab of the active tab bar is returned.
 */
TabContents*
TabWidget::currentTab() const
{
    DetachableTabWidget* tabWidget = 0;

    if ( !mainTabWidget )
    {
        return NULL;
    }
    else if ( displayType == SYSTEM ) // for system tab: get active tab widget
    {
        tabWidget = systemTabs.value( mainTabWidget->currentWidget() );
    }
    else
    {
        tabWidget = dynamic_cast<DetachableTabWidget*> ( mainTabWidget );
    }
    return tabWidget && tabWidget->count() > 0 ? dynamic_cast<TabContents*> ( tabWidget->currentWidget() ) : 0;
}

void
TabWidget::setOrder( const QList<DisplayType>& order )
{
    this->order = order;
    updateValueCombo();
}

// return the current value modus, see constants.h for the ValueModus type
//
ValueModus
TabWidget::getValueModus()
{
    return valueModus;
}

ValueWidget*
TabWidget::getValueWidget()
{
    return currentTab()->getValueWidget();
}

DisplayType
TabWidget::getType() const
{
    return displayType;
}

/**********************************/
/** updates the value widget with current tree values */
void
TabWidget::updateValueWidget()
{
    // update all visible value widget tabs
    foreach( TabInterface * ti, tabHash.keys() )
    {
        if ( ti->widget()->isVisible() ) // notify all detached tabs
        {
            ValueWidget* valueWidget = getTab( ti )->getValueWidget();
            if ( valueWidget )
            {
                valueWidget->update();
            }
        }
    }
}

void
TabWidget::setValueModus( ValueModus modus )
{
    if ( modus == valueModus )
    {
        return;
    }

    valueModus = modus;
    valueCombo->setValueModus( modus );
    TabInterface* current = currentTabInterface();

    // always notify SystemTreeWidget, because following tabs may refer on its values
    if ( displayType == SYSTEM )
    {
        TreeView* systemTree = this->getActiveTreeView();
        if ( current != systemTree )
        {
            systemTree->valueModusChanged( valueModus );
        }
    }

    current->valueModusChanged( valueModus );
    valuesChanged();
}

bool
TabWidget::isOrder( DisplayType left, DisplayType right )
{
    return order.indexOf( right ) > order.indexOf( left );
}

void
TabWidget::updateValueCombo()
{
    valueCombo->updateLabel();
}

// update the view of the current tab element
//
/**
 * @brief TabWidget::valuesChanged
 * Notifies all active tabs, that the tree values have changed. If a tab
 * is inactive, changeInfo is set to true. The tab will be notified, if it becames active again.
 */
void
TabWidget::valuesChanged()
{
    TabInterface* current = currentTabInterface();
    if ( !current )
    {
        return; // no tab is visible
    }

    Tree* tree = getActiveTreeView()->getTree();
    tree->getStatistics()->update();

    // always notify SystemTree first, because following tabs may refer on its values
    if ( displayType == SYSTEM )
    {
        TreeView* systemTree = this->getActiveTreeView();
        if ( current != systemTree )
        {
            systemTree->valuesChanged();
        }
    }

    // save for all inactive tabs, that values have been changed -> onCurrentChanged
    foreach( TabInterface * ti, tabHash.keys() )
    {
        changeInfo.insert( ti, ti == current ? false : true );

        if ( ti != current  && ti->widget()->isVisible() )// notify all detached tabs
        {
            ti->valuesChanged();
        }
    }

    // first update value views
    // views which belong to trees are updated, all other value widgets are cleared
    updateValueWidget();

    // notify active tab; plugin tabs should update their value widget
    current->valuesChanged();
}

/** return the TabWidget that contains the widget of the given TabInterface */
DetachableTabWidget*
TabWidget::getParentTabWidget( TabInterface* tc ) const
{
    TabContents* tab = tabHash.value( tc );
    return tab ? tab->getParentTab() : 0;
}

TabContents*
TabWidget::getTab( TabInterface* tc ) const
{
    return tabHash.value( tc );
}

/** signal currentChanged() is emitted when the visible tab element has changed; that signal gets connected to this slot;
 */
void
TabWidget::onCurrentChanged()
{
    if ( !Globals::getTabManager() || treeList.size() == 0 )
    {
        return; // initialisation not yet finished
    }

    if ( currentTab() && currentTab()->getValueWidget() )
    {
        currentTab()->getValueWidget()->clear();
    }

    TabInterface* activeTab = currentTabInterface();

    // if a tree view has been activated, activate its tree and deactivate the other ones
    for ( TreeView* view : treeList )
    {
        if ( view == activeTab )
        {
            view->getTree()->setActive();
        }
    }

    // check all tabs if they have been activated or hidden
    foreach( DetachableTabWidget * tabWidget, tabWidgetList )
    {
        if ( !tabWidget )
        {
            continue;
        }
        for ( int i = 0; i < tabWidget->count(); i++ )
        {
            TabInterface* tab    = tabWidget->getTabInterface( tabWidget->widget( i ) );
            bool          active = ( tab == activeTab );
            if ( active )
            {
                bool changedValues = changeInfo.value( tab );
                if ( changedValues ) // only called, if tree values have been changed while tab has been inactive
                {
                    tab->valuesChanged();
                    changeInfo.insert( tab, false );
                }
                updateValueWidget();
            }
            tab->setActive( active );
            updateToolbarStatus( tab );
        }
    }

    if ( getType() == CALL )
    {
        TreeView* view = dynamic_cast<TreeView*> ( activeTab );
        // Call of updateRowHeight is required if value view changes while call tab is inactive.
        // Invisible tree views don't react on sizeHintChanged signals -> send signal again if view becomes visible.
        view->updateRowHeight();

        // save and restore metric values that belong to the call tree on the right
        Tree* metric = Globals::getTabManager()->getActiveTree( METRIC );
        if ( currentCallView ) // save current metric tree values after new tab is chosen
        {
            // save values of previous tab
            CallTree* callTree = static_cast<CallTree*> ( currentCallView->getTree() );
            callTree->saveMetricValues( metric );
            // restore values to new tab
            callTree = static_cast<CallTree*> ( view->getTree() );
            callTree->restoreMetricValues( metric );
            Globals::getTabManager()->reinit();
        }
        currentCallView = view;

        PluginManager::getInstance()->tabActivated( view->getTree()->getType() ); // notify that tree has changed
    }
}

/**
 * @brief TabWidget::manageToolBar
 * If a toolbar is added here, it will only be visible, if the corresponding tab is visible. If the tab is
 * detached, it will be added to to new window, otherwise it will be added to the main window.
 */
void
TabWidget::addToolBar( QToolBar* bar, TabInterface* ti )
{
    if ( !tabHash.contains( ti ) )
    {
        Globals::debug( "Warning: cannot add toolbar to non-existing tab " );
        bar->setVisible( false );
        return;
    }
    TabContents* tab = getTab( ti );
    tab->setToolbar( bar );
    bar->setVisible( false );
}

void
TabWidget::removeToolBar( QToolBar* bar, TabInterface* tab )
{
    DetachableTabWidget* tabWidget = getParentTabWidget( tab );
    tabWidget->removeToolbar( tab, bar );
}

/** This slot is called, if the tab gets active because it has been moved to a new window. */
void
TabWidget::setActiveTab( TabInterface* tc )
{
    tc->valuesChanged();
    tc->setActive( true );
    updateToolbarStatus( tc );
}

/** Shows the corresponding toolbar, if a tab gets active. Hides it, if it becomes invisible. */
void
TabWidget::updateToolbarStatus( TabInterface* ti )
{
    if ( !getTab( ti ) )
    {
        return;
    }
    QToolBar* toolbar = getTab( ti )->getToolbar();
    if ( !toolbar || !ti->widget() )
    {
        return;
    }
    // add toolbar to main window or to detached window
    if ( ti == currentTabInterface() ) // current tab (may not be visible at startup)
    {
        // The main window is is most cases already parent of the toolbar. On some systems, addToolBar has to be called later to work properly.
        Globals::getMainWindow()->addToolBar( toolbar );
        toolbar->setVisible( true );
    }
    else if ( ti->widget()->isVisible() ) // detached tab
    {
        QMainWindow* win = DetachableTabWidget::getParentWindow( getTab( ti ) );
        if ( win )
        {
            win->addToolBar( toolbar );
            toolbar->setVisible( true );
        }
    }
    else
    {
        toolbar->setVisible( false );
    }
}

QSize
TabWidget::sizeHint() const
{
    // use sizehint of currenly visible tab, default implementation uses the maximum of all tabs
    QSize hint;
    if ( currentTabInterface() )
    {
        hint = currentTab()->sizeHint();
    }
    else
    {
        hint = QSize();
    }
    hint.setWidth( hint.width() + 2 * LAYOUT_MARGIN + 2 * LAYOUT_SPACING );
    return hint;
}

/**
   enables/disables the given tab
 */
void
TabWidget::enableTab( TabInterface* ti, bool enabled )
{
    DetachableTabWidget* tabWidget = getParentTabWidget( ti );
    int                  index     = tabWidget->indexOf( getTab( ti ) );
    if ( index >= 0 )
    {
        tabWidget->setTabEnabled( index, enabled );
    }
}

// Settings are handled by TabManager

/************ miscellaneous **************/

DetachableTabWidget::DetachableTabWidget( TabType t, const QString& tab ) : type( t ), defaultTab( tab )
{
    tabBar()->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( this->tabBar(), SIGNAL( customContextMenuRequested( const QPoint & ) ), SLOT( showContextMenu( const QPoint & ) ) );
    setMovable( true );
}


void
DetachableTabWidget::addTab( TabContents* contents )
{
    QTabWidget::addTab( contents, contents->getTabInterface()->icon(), contents->getTabInterface()->label() );
    tabInterfaceHash.insert( contents->getTabInterface(), contents );
}

TabInterface*
DetachableTabWidget::getTabInterface( QWidget* widget ) const
{
    TabContents* contents = dynamic_cast<TabContents*> ( widget );
    return contents ? contents->getTabInterface() : 0;
}

TabContents*
DetachableTabWidget::currentTab()
{
    return dynamic_cast<TabContents*> ( currentWidget() );
}
TabContents*
DetachableTabWidget::tabContents( int index )
{
    return dynamic_cast<TabContents*> ( widget( index ) );
}

void
DetachableTabWidget::showContextMenu( const QPoint& point )
{
    if ( point.isNull() )
    {
        return;
    }

    QTabBar*     bar             = dynamic_cast<QTabBar*>( sender() );
    int          currentTabIndex = bar->tabAt( point );
    TabContents* tabWidget       = tabContents( currentTabIndex );

    QString tabLabel = tabWidget->getTabInterface()->label();
    if ( tabLabel == tr( "System tree" ) ) // special case: only this menu item
    {
        QMenu*   contextMenu = new QMenu();
        QAction* action      = new QAction( tr( "Set system tree as startup default" ), this );
        action->setCheckable( true );
        action->setData( "System Tree View" );
        action->setChecked( defaultTab != "System Tree View" );
        connect( action, SIGNAL( triggered() ), this, SLOT( defaultPluginChanged() ) );
        contextMenu->addAction( action );
        contextMenu->exec( this->mapToGlobal( point ) );
        contextMenu->deleteLater();
        return;
    }
    else if ( !detachableTabs.contains( tabWidget ) )
    {
        return;
    }

    CubePlugin* plugin = 0;
    if ( tabPluginHash.contains( tabWidget ) )
    {
        plugin = PluginList::getCubePlugin( tabPluginHash.value( tabWidget ) );
    }

    QMenu*   contextMenu = new QMenu();
    QAction* action      = new QAction( tr( "Detach tab" ), this );
    action->setData( QVariant( currentTabIndex ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( detachTabAction() ) );
    contextMenu->addAction( action );

    if ( plugin )
    {
        action = new QAction( tr( "Set this plugin as startup default" ), this );
        action->setCheckable( true );
        if ( plugin->name() != defaultTab )
        {
            action->setData( QVariant( plugin->name() ) );
            action->setChecked( false );
        }
        else
        {
            action->setData( QVariant( "System Tree View" ) ); // default view
            action->setChecked( true );
        }
        connect( action, SIGNAL( triggered() ), this, SLOT( defaultPluginChanged() ) );
        contextMenu->addAction( action );

        QString pluginName = plugin->name();
        pluginName.remove( "Plugin" );
        pluginName.remove( "plugin" );
        pluginName = pluginName.simplified();

        QAction* closeAction = new QAction( tr( "Close " ) + pluginName + " " + tr( "Plugin" ), this );
        closeAction->setData( QVariant( currentTabIndex ) );
        connect( closeAction, SIGNAL( triggered() ), this, SLOT( closePlugin() ) );
        contextMenu->addAction( closeAction );
    }
    else // non-plugin tab e.g. info tab
    {
        QAction* closeAction = new QAction( tr( "Close tab" ), this );
        closeAction->setData( QVariant( currentTabIndex ) );
        connect( closeAction, SIGNAL( triggered() ), this, SLOT( closeTab() ) );
        contextMenu->addAction( closeAction );
    }

    // show context menu
    contextMenu->exec( this->mapToGlobal( point ) );
    contextMenu->deleteLater();
}

/** closes non-plugin tab, e.g. info tab */
void
DetachableTabWidget::closeTab()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    if ( action != 0 )
    {
        int          currentTabIndex = action->data().toInt();
        TabContents* tabWidget       = tabContents( currentTabIndex );
        removeTab( currentTabIndex );
        detachableTabs.removeAll( tabWidget );
    }
}

TabType
DetachableTabWidget::getType() const
{
    return type;
}



/** close plugin and remove its tab */
void
DetachableTabWidget::closePlugin()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    if ( action != 0 )
    {
        int          currentTabIndex = action->data().toInt();
        TabContents* tabWidget       = tabContents( currentTabIndex );

        if ( tabPluginHash.contains( tabWidget ) )
        {
            PluginManager::getInstance()->closePlugin( tabPluginHash.value( tabWidget ) );
            emit pluginClosed( tabWidget->getTabInterface() );
        }
    }
}

void
DetachableTabWidget::removeAllTabs()
{
    // remove detached tabs
    foreach( QWidget * widget, detachedTabs )
    {
        widget->setVisible( false );
        widget->deleteLater();
    }
    detachedTabs.clear();

    // remove tabs
    while ( count() > 0 )
    {
        removeTab( 0 );
    }
    tabInterfaceHash.clear();
}

void
DetachableTabWidget::detachTab( int index )
{
    TabContents* tabWidget = tabContents( index );
    QSize        size      = tabWidget->size();
    QIcon        icon      = tabIcon( index );

    CloseableMainWindow* dialog = new CloseableMainWindow( this );

    // move toolbar from main window to detached window
    QToolBar* toolbar = tabWidget->getToolbar();
    if ( toolbar )
    {
        dialog->addToolBar( toolbar );
    }

    dialog->setWindowTitle( tabText( index ) );
    dialog->setCentralWidget( tabWidget );
    tabWidget->setVisible( true );
    dialog->resize( size );
    dialog->setWindowIcon( icon );
    dialog->setVisible( true );
    dialog->setProperty( "tabWidget", QVariant::fromValue( ( void* )tabWidget ) );
    dialog->setProperty( "tabIcon", icon );
    dialog->setProperty( "tabIndex", index );

    connect( dialog, SIGNAL( closed() ), this, SLOT( detachedTabClosed() ) );
    detachedTabs.append( dialog );

    // notify plugin
    TabInterface* ti = getTabInterface( tabWidget );
    ti->detachEvent( dialog, true );
}

QMainWindow*
DetachableTabWidget::getParentWindow( QWidget* tab )
{
    QMainWindow* parent = 0;
    while ( ( tab &&  !tab->isWindow() && tab->parentWidget() ) )
    {
        tab = tab->parentWidget();
    }
    parent = dynamic_cast<QMainWindow*> ( tab );
    return parent;
}


void
DetachableTabWidget::setToolbar( TabInterface* tc, QToolBar* bar )
{
    TabContents* tab = tabInterfaceHash.value( tc );

    QMainWindow* main = 0;
    if ( indexOf( tab ) >= 0 ) // visible tab
    {
        main = Globals::getMainWindow();
    }
    else // detached tab
    {
        main = getParentWindow( tab );
    }
    if ( main )
    {
        main->addToolBar( bar );
    }
}

void
DetachableTabWidget::removeToolbar( TabInterface* tc, QToolBar* bar )
{
    QMainWindow* main = 0;
    TabContents* tab  = tabInterfaceHash.value( tc );
    if ( !bar )
    {
        return;
    }
    bar->setVisible( false );
    if ( indexOf( tab ) >= 0 ) // visible tab
    {
        main = Globals::getMainWindow();
    }
    else // detached tab
    {
        main = getParentWindow( tab );
    }
    if ( main )
    {
        main->removeToolBar( bar );
    }
}

/** detached tab closed -> move contents of the window back to the tab container */
void
DetachableTabWidget::detachedTabClosed()
{
    QMainWindow* dialog =  qobject_cast<QMainWindow*>( sender() );
    QWidget*     widget = ( QWidget* )dialog->property( "tabWidget" ).value<void*>();
    QIcon        icon   = dialog->property( "tabIcon" ).value<QIcon>();
    int          index  = dialog->property( "tabIndex" ).value<int>();

    // notify plugin
    TabInterface* ti = getTabInterface( widget );
    ti->detachEvent( dialog, false );

    dialog->layout()->removeWidget( widget );
    detachedTabs.removeAll( dialog );

    // move toolbar from detached widget to main window before currentChanged is emitted
    TabContents* tab     = tabInterfaceHash.value( ti );
    QToolBar*    toolbar = tab->getToolbar();
    if ( toolbar )
    {
        toolbar->setVisible( false );
        dialog->removeToolBar( toolbar );
        Globals::getMainWindow()->addToolBar( toolbar );
    }

    insertTab( index, widget, icon, dialog->windowTitle() );
    setCurrentIndex( index );
    dialog->deleteLater();
}

/** sets current plugin as default plugin which should be visible after loading a new cube */
void
DetachableTabWidget::defaultPluginChanged()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    if ( action != 0 )
    {
        QString pluginName = action->data().toString();
        emit    defaultPluginChanged( pluginName );
    }
}

void
DetachableTabWidget::removeTabInterface( TabInterface* tc )
{
    TabContents* tab   = tabInterfaceHash.value( tc );
    int          index = indexOf( tab );

    if ( !tab )
    {
        return;
    }

    this->removeToolbar( tc, tab->getToolbar() );
    if ( index >= 0 ) // visible tab
    {
        this->removeTab( index );
    }
    else // detached tab
    {
        QMainWindow* dialog = dynamic_cast<QMainWindow*> ( tab->parentWidget() );
        dialog->disconnect();

        detachedTabs.removeAll( dialog );
        detachableTabs.removeAll( tab );
        tabPluginHash.remove( tab );
        tabInterfaceHash.remove( tc );

        delete tab->getValueWidget(); // value widget is managed by the plugin
        dialog->deleteLater();
    }
}

void
DetachableTabWidget::detachTabAction()
{
    QAction* action = qobject_cast<QAction*>( sender() );
    if ( action != 0 )
    {
        int          currentTabIndex  = action->data().toInt();
        TabContents* tabWidget        = tabContents( currentTabIndex );
        bool         isAlreadyVisible = tabWidget->isVisible();

        detachTab( currentTabIndex );

        if ( !isAlreadyVisible )
        {
            emit( activated( tabWidget->getTabInterface() ) );
        }
    }
}

void
DetachableTabWidget::setDetachable( TabContents* tab, const QString& pluginName )
{
    detachableTabs.append( tab );
    if ( !pluginName.isEmpty() )
    {
        tabPluginHash.insert( tab, pluginName );
    }
}



TabContents::TabContents( TabInterface* tc, DetachableTabWidget* parent, bool detachable )
{
    this->tc    = tc;
    parentTab   = parent;
    toolbar     = 0;
    valueWidget = 0;

    QVBoxLayout* layout = new QVBoxLayout();
    setLayout( layout );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( LAYOUT_SPACING );

    layout->addWidget( tc->widget() );
    tc->widget()->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

    statusPanel = new QWidget();
    statusPanel->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    statusLayout = new QStackedLayout();
    statusPanel->setLayout( statusLayout );

    if ( parent->getType() == DEFAULT_TAB )
    {
        valueWidget = new ValueWidget();
        statusLayout->addWidget( valueWidget );
    }
    layout->addWidget( statusPanel );

    if ( detachable )
    {
        parent->setDetachable( this );
    }
}

ValueWidget*
TabContents::getValueWidget()
{
    return valueWidget;
}

DetachableTabWidget*
TabContents::getParentTab()
{
    return parentTab;
}

QStackedLayout*
TabContents::getStatusLayout()
{
    return statusLayout;
}

QToolBar*
TabContents::getToolbar() const
{
    return toolbar;
}

void
TabContents::setToolbar( QToolBar* bar )
{
    if ( !toolbar )
    {
        toolbar = bar;
        parentTab->setToolbar( tc, bar );
    }
}

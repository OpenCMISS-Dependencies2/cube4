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

#include <assert.h>
#include <algorithm>
#include <QMenu>
#include <QFileDialog>

#include "PluginManager.h"
#include "PluginServices.h"
#include "PluginList.h"
#include "PluginSettingDialog.h"
#include "TreeItem.h"
#include "TreeItemMarkerDialog.h"
#include "MainWidget.h"
#include "Globals.h"
#include "TreeView.h"
#include "ValueView.h"
#include "TreeItemMarker.h"
#include "HelpBrowser.h"
#include "InfoWidget.h"
#include "Future.h"

using namespace cubegui;
using namespace cubepluginapi;

// workaround:leak in plugin may cause segfault https://bugreports.qt-project.org/browse/QTBUG-39070
#define NO_PLUGIN_UNLOAD

PluginManager* PluginManager::single        = 0;
bool           PluginManager::disableAlways = false;

PluginManager*
PluginManager::getInstance()
{
    if ( single == 0 )
    {
        single = new PluginManager();
    }
    return single;
}

PluginManager::PluginManager()
{
    cube         = 0;
    tabManager   = 0;
    cubeFileName = "";
    markerDialog = 0;

    valueViews.insert( 0, Globals::defaultValueView );

    currentContextFreePlugin = 0;

    /** create menu items which will be filled after plugins have been loaded */
    pluginMenu = new QMenu( tr( "&Plugins" ) );
    connect( pluginMenu, SIGNAL( aboutToShow() ), this, SLOT( checkPluginMenuStatus() ) );
    pluginMenu->setVisible( false );

    configPathItem = new QAction( tr( "Configure plugin search path" ), 0 );
    pluginMenu->addAction( configPathItem );
    configPathItem->setWhatsThis( tr( "Allows one to activate or deactivate plugins independent of the cube file" ) );
    connect( configPathItem, SIGNAL( triggered() ), this, SLOT( configureSearchPath() ) );

    initialSettingsItem = new QAction( tr( "Initial activation settings" ), 0 );
    initialSettingsItem->setWhatsThis( tr( "Allows one to activate or deactivate plugins independent of the cube file" ) );
    connect( initialSettingsItem, SIGNAL( triggered() ), this, SLOT( showSettingsWidget() ) );

    pluginStatusMenu = new QMenu( tr( "Activate/deactivate plugins" ) );
    pluginStatusMenu->setWhatsThis( tr( "Allows one to activate or deactivate plugins which are available for the current cube file" ) );

    contextFreePluginMenu = new QMenu( tr( "Context free plugin" ) );
    contextFreePluginMenu->setStatusTip( tr( "Opens a context free plugin" ) );
    contextFreePluginMenu->setEnabled( false );

    infoMenu = new QMenu( tr( "Plugin info" ) );
    infoMenu->setEnabled( false );
    helpMenu = new QMenu( tr( "Plugin documentation" ) );

    markerMenuAction = new QAction( tr( "Configure Tree Item Marker" ), 0 );
    markerMenuAction->setWhatsThis( tr( "Allows one to change colors and icons of the items that are marked by plugins" ) );
    markerMenuAction->setEnabled( false );
    connect( markerMenuAction, SIGNAL( triggered() ), this, SLOT( showTreeItemMarkerSettings() ) );
    markerMenuAction->setVisible( false );
}

/** is called after global settings are loaded or plugin path has been changed */
void
PluginManager::initializePlugins()
{
    if ( disableAlways )
    {
        return;
    }

    PluginList::updatePluginList( pluginPath );

    fillPluginMenu();
    fillHelpMenu();
    fillContextFreePluginMenu();
}

void
PluginManager::fillPluginMenu()
{
    pluginMenu->clear();
    pluginMenu->addAction( configPathItem );
    pluginMenu->addAction( initialSettingsItem );
    pluginMenu->addMenu( pluginStatusMenu );
    pluginMenu->addSeparator();
    pluginStatusMenu->clear();
    pluginActivationActions.clear();

    // Create initial plugin menu structure
    QList<cubepluginapi::CubePlugin*> pluginList = PluginList::getCubePluginList();
    if ( !pluginList.isEmpty() )
    {
        markerMenuAction->setVisible( true );

        foreach( CubePlugin * plugin, pluginList )
        {
            QMenu* subMenu = pluginMenus.value( plugin->name() );
            if ( !subMenu )
            {
                // ensure that plugin submenu is only created once, because the reference is used by the plugin
                subMenu = new QMenu( plugin->name(), pluginMenu );
                subMenu->menuAction()->setVisible( false );
            }
            pluginMenu->addMenu( subMenu );
            pluginMenus.insert( plugin->name(), subMenu );

            // add "active/deactivate plugin" menu item
            QAction* pa = new QAction( plugin->versionLabel(), this );
            pluginActivationActions.insert( plugin->name(), pa );
            pa->setCheckable( true );
            pa->setChecked( false );
            pa->setVisible( false );
            pa->setStatusTip( tr( "Activate/deactivate plugin" ) );
            pa->setWhatsThis( tr( "If this menu item is checked, the plugin is activated" ) );
            pa->setData( plugin->name() );
            connect( pa, SIGNAL( triggered( bool ) ), this, SLOT( enablePlugin( bool ) ) );
            pluginStatusMenu->addAction( pa );
        }
    }
}


QAction*
PluginManager::getMarkerConfigAction()
{
    return markerMenuAction;
}

void
PluginManager::setMainWindow( QMainWindow* main )
{
    mainWindow = main;
}

void
PluginManager::disablePlugins()
{
    PluginList::disablePlugins();
    disableAlways = true;
}

void
PluginManager::opened( cube::CubeProxy* cube, const QString& file, TabManager* tabManager )
{
    this->tabManager   = tabManager;
    this->cube         = cube;
    this->cubeFileName = file;

    openPlugins();
    infoMenu->setEnabled( true );
}

// called if cube file has been closed
void
PluginManager::closed()
{
    if ( !cube )
    {
        return; // not opened
    }
    infoMenu->setEnabled( false );

    TabManager* tabManager = PluginManager::getInstance()->tabManager;
    foreach( TabWidget * widget, tabManager->getTabWidgets() )
    {
        // all tabs will be closed -> don't react on tab changed signals any more
        QList<QTabWidget*> list = widget->findChildren<QTabWidget*>();
        foreach( QTabWidget * tab, list )
        {
            tab->disconnect();
        }
    }

    foreach( cubepluginapi::CubePlugin * plugin, PluginList::getCubePluginList() )
    {
        closePlugin( plugin->name() );
        pluginActivationActions.value( plugin->name() )->setVisible( false );
    }

    this->tabManager   = 0;
    this->cube         = 0;
    this->cubeFileName = "";

    // reinitialize plugin status to startup settings
    getPluginStatus( enableAllPlugins, disableAllPlugins, pluginStatus );

    if ( markerMenuAction )
    {
        markerMenuAction->setEnabled( false );
    }
    setMarkerConfig();
    MarkerAttributes::resetColors();
}

void
PluginManager::openPlugins()
{
    if ( cube == 0 || tabManager == 0 )
    {
        return; // not opened
    }
    if ( !disableAlways )
    {
        openAllPlugins();
    }
    setMarkerConfig();
}

bool
PluginManager::openPlugin( const QString& name )
{
    bool        ok     = true; // plugin has been loaded and is valid
    CubePlugin* plugin = PluginList::getCubePlugin( name );

#ifndef NO_PLUGIN_UNLOAD
    if ( !plugin )
    {
        plugin = PluginList::loadPlugin( name );
    }
#endif

    if ( plugin && !plugin->isInitialized )
    {
        PluginServices* services = new PluginServices( this, name );
        pluginServices[ plugin->name() ] = services;

        ok = plugin->cubeOpened( services );
    }

    pluginActivationActions.value( name )->setVisible( true );
    if ( ok )
    {
        pluginMenus.value( name )->menuAction()->setVisible( true );
        pluginActivationActions.value( name )->setChecked( true );
        plugin->isInitialized = true;
    }
    else
    {
        pluginMenus.value( name )->menuAction()->setVisible( false );
        pluginActivationActions.value( name )->setChecked( false );
    }
    setMarkerConfig();

    return ok;
}

PluginServices*
PluginManager::getService( const QString& pluginName )
{
    return pluginServices[ pluginName ];
}

void
PluginManager::closePlugin( const QString& pluginName )
{
    QAction* action = pluginActivationActions.value( pluginName );
    action->setChecked( false );

    CubePlugin* plugin = PluginList::getCubePlugin( pluginName );

    if ( plugin == 0 || !plugin->isInitialized )
    {
        return; // already unloaded or not loaded
    }

    // hide plugin menu entry
    QMenu* menu = pluginMenus.value( pluginName );
    menu->clear();
    menu->menuAction()->setVisible( false );

    PluginServices* service = pluginServices[ plugin->name() ];

    // remove plugin toolbar(s)
    foreach( QToolBar * bar, toolBarHash.values( service ) )
    {
        mainWindow->removeToolBar( bar );
        toolBarHash.remove( service, bar );
        bar->deleteLater();
    }

    // remove plugin colormaps
    foreach( ColorMap * map, colormaps.values( service ) )
    {
        if ( map == Globals::getColorMap() ) // replace closed plugin colormap by default colormap
        {
            Globals::setColorMap();
        }
        colormaps.remove( service, map );
    }
    // remove plugin value views
    foreach( ValueView * view, valueViews.values( service ) )
    {
        Globals::removeValueView( view );
        if ( view != Globals::defaultValueView )
        {
            valueViews.remove( service, view );
        }
    }

    if ( service )
    {
        foreach( Future * future, service->futureList )
        {
            // cancel calculation and wait to be finished to ensure that no further signals are send after plugin is closed
            future->cancelCalculation( true );
        }
        service->closeTabs();
        plugin->cubeClosed();
        plugin->isInitialized = false;

        // delete corresponding service
        delete service;
        pluginServices.remove( plugin->name() );
    }

    setMarkerConfig();

#ifndef NO_PLUGIN_UNLOAD
    PluginList::unloadPlugin( pluginName );
#endif
}

QMenu*
PluginManager::getPluginMenu() const
{
    return pluginMenu;
}

QMenu*
PluginManager::getPluginInfoMenu() const
{
    return infoMenu;
}

QMenu*
PluginManager::getPluginHelpMenu() const
{
    return helpMenu;
}

QMenu*
PluginManager::getContextFreePluginMenu() const
{
    return contextFreePluginMenu;
}

void
PluginManager::openAllPlugins()
{
    foreach( CubePlugin * plugin, PluginList::getCubePluginList() )
    {
        bool load = true;
        if ( enableAllPlugins )
        {
            load = true;
        }
        else if ( disableAllPlugins )
        {
            load = false;
        }
        else
        {
            load = pluginStatus.value( plugin->name(), true ); // load plugin, if not not set to false in settings
        }

        if ( load )
        {
            Globals::setStatusMessage( tr( "Opening plugin" ) + " " + plugin->name(), Information, false );
            bool ok = openPlugin( plugin->name() );
            if ( !ok )
            {
                QString msg = tr( "Cannot activate " ) + plugin->name();
                if ( !plugin->getDeactivationMessage().isEmpty() )
                {
                    msg = msg + ": " + plugin->getDeactivationMessage();
                }
                Globals::setStatusMessage( msg, Warning );
            }
        }
    }
}

/** slot for plugin activation/deactivation menu */
void
PluginManager::enablePlugin( bool enable )
{
    QAction* act = dynamic_cast<QAction*>( sender() );
    assert( act );

    QString pluginName = act->data().toString();
    if ( enable )
    {
        CubePlugin* plugin = PluginList::getCubePlugin( pluginName );
        bool        ok     = openPlugin( pluginName );
        if ( !ok )
        {
            QString msg = tr( "Cannot activate " ) + plugin->name();
            if ( !plugin->getDeactivationMessage().isEmpty() )
            {
                msg = msg + ": " + plugin->getDeactivationMessage();
            }
            Globals::setStatusMessage( msg, Warning );
        }
        else
        {
            tabManager->reinit();
            QString msg = plugin->name() + tr( " has been activated." );
            Globals::setStatusMessage( msg );
        }
    }
    else
    {
        closePlugin( pluginName );
    }
}

void
PluginManager::configureSearchPath()
{
    pluginPath = PathDialog::getPath( pluginPath );

    initializePlugins();
    openPlugins();
}

void
PluginManager::showSettingsWidget()
{
    PluginSettingDialog::showSettingsDialog( tabManager );
}

/**
 * checkPluginMenuStatus hides empty plugin menus
 */
void
PluginManager::checkPluginMenuStatus()
{
    foreach( QMenu * menu, pluginMenus )
    {
        if ( menu->isEmpty() )
        {
            menu->menuAction()->setVisible( false );
        }
        else
        {
            menu->menuAction()->setVisible( true );
        }
    }
}

// ---------- emit signals for plugins -----------------

void
PluginManager::contextMenuIsShown( DisplayType type, TreeItem* treeItem )
{
    QStringList names = pluginServices.keys();
    std::sort( names.begin(), names.end() );
    foreach( QString pluginName, names )
    {
        PluginServices* service = pluginServices.value( pluginName );
        if ( service )
        {
            emit service->contextMenuIsShown( type, treeItem );
        }
    }
}

void
PluginManager::treeItemIsSelected( TreeItem* treeItem )
{
    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service )
        {
            emit service->treeItemIsSelected( treeItem );
        }
    }
}

void
PluginManager::setTabOrder( const QList<DisplayType>& order )
{
    this->order = order;
    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service )
        {
            emit service->orderHasChanged( order );
        }
    }
}

void
PluginManager::setGlobalValue( const QString& name, const QVariant& value, PluginServices* sender )
{
    globalValues.insert( name, value );

    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service && service != sender )
        {
            emit service->globalValueChanged( name );
        }
    }
}

void
PluginManager::tabActivated( DisplayType type )
{
    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service )
        {
            emit service->tabActivated( type );
        }
    }
}

ValueView*
PluginManager::getValueView( const QString& name )
{
    foreach( ValueView * view, valueViews.values() )
    {
        if ( view->getName() == name )
        {
            return view;
        }
    }
    return 0;
}

QMainWindow*
PluginManager::getMainWindow()
{
    return mainWindow;
}


void
PluginManager::genericUserAction( UserAction action )
{
    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service )
        {
            emit service->genericUserAction( action );
        }
    }
}

// ---------- end emit signals for plugins -----------------



/**
 * reads the initial plugin status from settings
 */
void
PluginManager::getPluginStatus( bool& all, bool& none, QHash<QString, bool>& plugins )
{
    all     = enableAllPlugins;
    none    = disableAllPlugins;
    plugins = pluginStatus;
}


/**
 * @brief PluginManager::setPluginStatus
 * Activates or deactivates the available plugins and saves the status
 * @param all all plugins will be activated, if true
 * @param none all plugins will be deactivated, if true
 * @param plugins Hash with pluginName->activated value pairs
 */
void
PluginManager::setPluginStatus( bool& all, bool& none, QHash<QString, bool>& plugins )
{
    enableAllPlugins  = all;
    disableAllPlugins = none;
    pluginStatus      = plugins;

    if ( cube )
    {
        openAllPlugins();
    }
}

// --- settings for initial activation/deactivation of the plugins

/** this method is only called once if program ends */
void
PluginManager::saveGlobalStartupSettings( QSettings& settings )
{
    if ( disableAlways )
    {
        return;
    }

    QStringList enabledList, disabledList;
    foreach( QString pluginName, pluginStatus.keys() )
    {
        if ( pluginStatus.value( pluginName ) )
        {
            enabledList.append( pluginName );
        }
        else
        {
            disabledList.append( pluginName );
        }
    }

    settings.setValue( "pluginDisableAll", disableAllPlugins );
    settings.setValue( "pluginEnableAll", enableAllPlugins );
    settings.setValue( "pluginEnabledList", enabledList );
    settings.setValue( "pluginDisabledList", disabledList );
    settings.setValue( "pluginPath", pluginPath );
}

/** this method is only called once at program start */
void
PluginManager::loadGlobalStartupSettings( QSettings& settings )
{
    if ( disableAlways )
    {
        return;
    }

    QStringList enabledList, disabledList;
    enabledList  = settings.value( "pluginEnabledList" ).toStringList();
    disabledList = settings.value( "pluginDisabledList" ).toStringList();
    pluginPath   = settings.value( "pluginPath" ).toStringList();

    foreach( QString plugin, enabledList )
    {
        pluginStatus.insert( plugin, true );
    }
    foreach( QString plugin, disabledList )
    {
        pluginStatus.insert( plugin, false );
    }
    enableAllPlugins  = settings.value( "pluginEnableAll", true ).toBool();
    disableAllPlugins = settings.value( "pluginDisableAll", false ).toBool();
}

// ----- implementation of SettingsHandler interface to save settings of each plugin  -----------

QString
PluginManager::settingName()
{
    return "plugins";
}

void
PluginManager::saveGlobalSettings( QSettings& settings )
{
    // save settings for each registered pluginService
    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service && service->settingsHandler )
        {
            settings.beginGroup( service->settingsHandler->settingNameCompact() );
            service->settingsHandler->saveGlobalSettings( settings );
            settings.endGroup();
        }
    }
}

void
PluginManager::loadGlobalSettings( QSettings& settings )
{
    // load settings for pluginService with registered settingsHandler
    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service && service->settingsHandler )
        {
            settings.beginGroup( service->settingsHandler->settingNameCompact() );
            service->settingsHandler->loadGlobalSettings( settings );
            settings.endGroup();
        }
    }
}

void
PluginManager::saveGlobalOnlySettings( QSettings& settings )
{
    // save settings for each registered pluginService
    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service && service->settingsHandler )
        {
            settings.beginGroup( service->settingsHandler->settingNameCompact() );
            service->settingsHandler->saveGlobalOnlySettings( settings );
            settings.endGroup();
        }
    }
}

void
PluginManager::loadGlobalOnlySettings( QSettings& settings )
{
    // load settings for pluginService with registered settingsHandler
    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service && service->settingsHandler )
        {
            settings.beginGroup( service->settingsHandler->settingNameCompact() );
            service->settingsHandler->loadGlobalOnlySettings( settings );
            settings.endGroup();
        }
    }
}

void
PluginManager::saveExperimentSettings( QSettings& settings )
{
    // save settings for each registered pluginService
    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service && service->settingsHandler )
        {
            settings.beginGroup( service->settingsHandler->settingNameCompact() );
            service->settingsHandler->saveExperimentSettings( settings );
            settings.endGroup();
        }
    }
}

void
PluginManager::loadExperimentSettings( QSettings& settings )
{
    // load settings for each registered pluginService
    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service && service->settingsHandler )
        {
            settings.beginGroup( service->settingsHandler->settingNameCompact() );
            service->settingsHandler->loadExperimentSettings( settings );
            settings.endGroup();
        }
    }
}


void
PluginManager::fillHelpMenu()
{
    infoMenu->clear();
    helpMenu->clear();

    foreach( cubepluginapi::CubePlugin * plugin, PluginList::getCubePluginList() )
    {
        if ( plugin )
        {
            QAction* action = new QAction( plugin->name(), this );
            action->setData( plugin->name() );
            infoMenu->addAction( action );
            connect( action, SIGNAL( triggered( bool ) ), this, SLOT( showPluginInfo() ) );

            action = new QAction( plugin->name(), this );
            action->setData( plugin->name() );
            helpMenu->addAction( action );
            connect( action, SIGNAL( triggered( bool ) ), this, SLOT( showPluginHelp() ) );
            if ( plugin->getHelpURL().isEmpty() )
            {
                action->setEnabled( false );
            }
        }
    }
}

void
PluginManager::showPluginHelp()
{
    QAction*    action = qobject_cast<QAction*>( sender() );
    QString     name   = action->data().toString();
    CubePlugin* plugin = PluginList::getCubePlugin( name );

    QString      path        = Globals::getOption( DocPath ) + "cubegui/guide/html/";
    HelpBrowser* helpBrowser = HelpBrowser::getInstance( tr( "Documentation" ) );
    QString      html        = plugin->getHelpURL();
    html = html.isEmpty() ? "index.html" : html; // show contents of user guide, if no help file is given
    if ( !html.startsWith( "http:" ) && !html.startsWith( "https:" ) && !html.startsWith( "file:" )  )
    {
        html = path + html;
    }
    helpBrowser->showUrl( html, tr( "Documentation not found: " ) + html );
}

void
PluginManager::showPluginInfo()
{
    QAction*    action = qobject_cast<QAction*>( sender() );
    QString     name   = action->data().toString();
    CubePlugin* plugin = PluginList::getCubePlugin( name );
    QString     helpText;
    helpText.append( plugin->versionLabel() + "\n\n" );
    helpText.append( plugin->getHelpText() );
    helpText.append( "\n\n" );
    helpText.append( tr( "Location: " ) + PluginList::getCubePluginPath( name ) );

    InfoWidget* helpWidget = InfoWidget::getInstance();
    helpWidget->activate();
    helpWidget->toFront();
    QPushButton* doc = new QPushButton( tr( "Show Documentation" ) );
    helpWidget->showPluginInfo( helpText, doc );

    if ( plugin->getHelpURL().isEmpty() )
    {
        doc->setEnabled( false );
    }
    else
    {
        // call showPluginHelp() if doc button is pressed
        QAction* helpAction = new QAction( "", this );
        helpAction->setData( name );
        connect( helpAction, SIGNAL( triggered( bool ) ), this, SLOT( showPluginHelp() ) );
        connect( doc, SIGNAL( pressed() ), helpAction, SLOT( trigger() ) );
    }
}

// ============== context free plugins ====================================

#include "ContextFreeServices.h"

/**
 * @brief PluginManager::setContextFreePluginMenu
 * @param menu the menu which is filled with the labels of context free plugins
 */
void
PluginManager::fillContextFreePluginMenu()
{
    contextFreePluginMenu->clear();
    contextFreeActions.clear();

    QList<cubepluginapi::ContextFreePlugin*> pluginList = PluginList::getContextFreePluginList();
    if ( pluginList.isEmpty() )
    {
        contextFreePluginMenu->setEnabled( false );
    }
    else
    {
        contextFreePluginMenu->setEnabled( true );
        foreach( ContextFreePlugin * plugin, pluginList )
        {
            QAction* pa = new QAction( plugin->versionLabel(), this );
            pa->setObjectName( plugin->name() );
            contextFreePluginMenu->addAction( pa );
            contextFreeActions.append( pa );
            pa->setData( plugin->name() );
            connect( pa, SIGNAL( triggered() ), this, SLOT( startPlugin() ) );
        }
        contextFreePluginMenu->addSeparator();
        closeContextAction = new QAction( tr( "Close current Plugin" ), this );
        closeContextAction->setEnabled( false );
        contextFreePluginMenu->addAction( closeContextAction );
        connect( closeContextAction, SIGNAL( triggered() ), this, SLOT( closeContextFreePlugin() ) );
    }
}

QList<QAction*>
PluginManager::getContextFreeActions()
{
    return contextFreeActions;
}

void
PluginManager::recalculateTree( Tree* tree )
{
    emit recalculateRequest( tree );
}

cube::CubeProxy*
PluginManager::getCube() const
{
    return cube;
}

/** slot for menu action */
void
PluginManager::startPlugin()
{
    // close active plugin, if exists
    if ( currentContextFreePlugin )
    {
        closeContextFreePlugin();
    }

    dynamic_cast<MainWidget*>( mainWindow )->setContext( MainWidget::CONTEXT_FREE );

    // open chosen plugin
    QAction*           action = qobject_cast<QAction*>( sender() );
    QString            name   = action->data().toString();
    ContextFreePlugin* plugin = PluginList::getContextFreePlugin( name ); // selected plugin from menu
    currentContextFreePlugin = plugin;
    plugin->setArguments( contextFreeArgs );
    plugin->opened( ContextFreeServices::getInstance() );
    contextFreeArgs.clear();
    closeContextAction->setEnabled( true );
}

/* sets command line arguments for context free plugin */
void
PluginManager::setPluginArguments( QStringList& args )
{
    contextFreeArgs = args;
}

/** closes active context free plugin */
void
PluginManager::closeContextFreePlugin()
{
    if ( currentContextFreePlugin )
    {
        currentContextFreePlugin->closed();

        // remove layout items of the previously started plugin
        QWidget* w      = ContextFreeServices::getInstance()->getWidget();
        QLayout* layout = w->layout();
        while ( layout && layout->count() > 0 )
        {
            QWidget* child = layout->itemAt( 0 )->widget();
            delete child;
        }
        delete layout;

        closeContextAction->setEnabled( false );

        dynamic_cast<MainWidget*>( mainWindow )->setContext( MainWidget::CONTEXT_INIT );
    }
}

// ============== end context free plugins ====================================

void
PluginManager::resizeTreeItemMarker( int iconHeight )
{
    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service )
        {
            service->resizeTreeItemMarker( iconHeight );
        }
    }
}

QList<TreeItemMarker*>
PluginManager::getTreeItemMarker()
{
    QList<TreeItemMarker*> list;

    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service )
        {
            foreach( TreeItemMarker * marker, service->markerList )
            list.append( marker );
        }
    }
    return list;
}

/**
 * @brief PluginManager::setMarkerConfig
 * enables or disables the marker dialog menu item and updates the marker dialog
 */
void
PluginManager::setMarkerConfig()
{
    if ( markerMenuAction )
    {
        if ( getTreeItemMarker().size() == 0 )
        {
            markerMenuAction->setEnabled( false );
        }
        else
        {
            markerMenuAction->setEnabled( true );
        }
    }
    if ( markerDialog && markerDialog->isVisible() )
    {
        if ( getTreeItemMarker().size() == 0 )
        {
            markerDialog->grayItems( false );
            markerDialog->setVisible( false );
        }
        else
        {
            showTreeItemMarkerSettings();
        }
    }
}

void
PluginManager::showTreeItemMarkerSettings()
{
    QPoint pos;
    if ( markerDialog )
    {
        pos = markerDialog->pos();
    }
    delete markerDialog;
    markerDialog = new TreeItemMarkerDialog( tabManager );
    if ( !pos.isNull() )
    {
        markerDialog->move( pos );
    }
    markerDialog->setVisible( true );
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

#include <QApplication>

PathDialog::PathDialog( QWidget* parent ) : QDialog( parent )
{
    this->setWindowTitle( tr( "Plugin search path" ) );
    this->setModal( true );
    this->setMinimumWidth( 600 );

    this->setWhatsThis( tr( "Prepends the given paths to the plugin search path. Already loaded plugins aren't replaced until restart of cube." ) );

    QVBoxLayout* topLayout = new QVBoxLayout;
    this->setLayout( topLayout );

    QWidget*     horizontal = new QWidget();
    QHBoxLayout* hlay       = new QHBoxLayout;
    hlay->setContentsMargins( 0, 0, 0, 0 );
    horizontal->setLayout( hlay );
    topLayout->addWidget( horizontal );

    line = new QLineEdit();
    hlay->addWidget( line );

    QPushButton* but = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_FileDialogStart ), "" );
    connect( but, SIGNAL( pressed() ), this, SLOT( choose() ) );
    hlay->addWidget( but );

    listWidget = new QListWidget( this );
    listWidget->setSelectionMode( QAbstractItemView::SingleSelection );
    connect( listWidget, SIGNAL( itemActivated( QListWidgetItem* ) ), this, SLOT( edit() ) );

    topLayout->addWidget( listWidget );

    horizontal = new QWidget();
    hlay       = new QHBoxLayout;
    hlay->setContentsMargins( 0, 0, 0, 0 );
    horizontal->setLayout( hlay );
    topLayout->addWidget( horizontal );

    but = new QPushButton( tr( "add" ) );
    connect( but, SIGNAL( pressed() ), this, SLOT( add() ) );
    hlay->addWidget( but );

    but = new QPushButton( tr( "delete" ) );
    connect( but, SIGNAL( pressed() ), this, SLOT( deleteRow() ) );
    hlay->addWidget( but );

    but = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_ArrowUp ), "" );
    connect( but, SIGNAL( pressed() ), this, SLOT( up() ) );
    hlay->addWidget( but );

    but = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_ArrowDown ), "" );
    connect( but, SIGNAL( pressed() ), this, SLOT( down() ) );
    hlay->addWidget( but );
    hlay->addStretch( 1 );


    but = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_DialogOkButton ), tr( "Ok" ) );
    connect( but, SIGNAL( pressed() ), this, SLOT( close() ) );
    hlay->addWidget( but );
}

QStringList
PathDialog::getPath( QStringList init )
{
    PathDialog* pd = new PathDialog( Globals::getMainWindow() );

    foreach( QString str, init )
    {
        new QListWidgetItem( str, pd->listWidget );
    }

    pd->exec();

    QStringList list;
    for ( int i = 0; i < pd->listWidget->count(); i++ )
    {
        list.append( pd->listWidget->item( i )->text() );
    }
    return list;
}


void
PathDialog::edit()
{
    QString txt = listWidget->currentItem()->text();
    line->setText( txt );
}

void
PathDialog::up()
{
    if ( listWidget->currentRow() >= 0 )
    {
        int              newPos = ( listWidget->currentRow() + listWidget->count() - 1 ) % listWidget->count();
        QListWidgetItem* item   = listWidget->takeItem( listWidget->currentRow() );
        listWidget->insertItem( newPos, item );
        listWidget->setCurrentItem( item );
    }
}

void
PathDialog::down()
{
    if ( listWidget->currentRow() >= 0 )
    {
        int              newPos = ( listWidget->currentRow() + 1 ) % listWidget->count();
        QListWidgetItem* item   = listWidget->takeItem( listWidget->currentRow() );
        listWidget->insertItem( newPos, item );
        listWidget->setCurrentItem( item );
    }
}

void
PathDialog::deleteRow()
{
    listWidget->takeItem( listWidget->currentRow() );
}


void
PathDialog::add()
{
    QString path = line->text().trimmed();
    if ( !path.isEmpty() )
    {
        new QListWidgetItem( path, listWidget );
    }
}

void
PathDialog::choose()
{
    QString dir = QFileDialog::getExistingDirectory( Globals::getMainWindow(), tr( "Open Directory" ), line->text().trimmed()
                                                     , QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
    line->setText( dir );
}

void
PluginManager::cancelCalculations()
{
    // cancel running tasks, but don't wait for them to be finished
#ifdef CUBE_CONCURRENT_LIB
    foreach( PluginServices * service, pluginServices.values() )
    {
        if ( service )
        {
            foreach( Future * future, service->futureList )
            {
                if ( future->isRunning() && future->dependsOnTreeSelection() )
                {
                    future->cancelCalculation();
                }
            }
        }
    }
#endif
}

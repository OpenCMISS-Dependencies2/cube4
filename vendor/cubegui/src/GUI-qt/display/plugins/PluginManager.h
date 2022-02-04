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


#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QMainWindow>
#include <QLayout>
#include <QListWidget>
#include <QDialog>
#include "Constants.h"
#include "TabWidget.h"
#include "PluginServices.h"
#include "Settings.h"
#include "TabManager.h"

namespace cube
{
class CubeProxy;
}

namespace cubepluginapi
{
class PluginServices;
class CubePlugin;
class ContextFreePlugin;
class ValueView;
}

namespace cubegui
{
class TreeListener;
class TreeItemMarkerDialog;
using cubepluginapi::ValueView;

/**
 * Pluginmanager
 */

class PluginManager : public QObject, public InternalSettingsHandler
{
    Q_OBJECT
public:
    static PluginManager*
    getInstance();

    static void
    disablePlugins();

    void
    initializePlugins();

    /** returns main plugin menu, which has to be inserted into the menu bar */
    QMenu*
    getPluginMenu() const;

    QMenu*
    getPluginInfoMenu() const;
    QMenu*
    getPluginHelpMenu() const;

    QMenu*
    getContextFreePluginMenu() const;

    void
    opened( cube::CubeProxy* cube,
            const QString&   file,
            TabManager*      tabManager );

    void
    closed();

    // ---- virtual methods of class InternalSettingsHandler
    void
    loadGlobalStartupSettings( QSettings& settings );
    void
    saveGlobalStartupSettings( QSettings& settings );
    void
    saveGlobalSettings( QSettings& settings );
    void
    loadGlobalOnlySettings( QSettings& settings );
    void
    saveGlobalOnlySettings( QSettings& settings );
    void
    loadGlobalSettings( QSettings& settings );
    void
    saveExperimentSettings( QSettings& settings );
    void
    loadExperimentSettings( QSettings& settings );
    QString
    settingName();

    void
    setPluginStatus( bool& all,
                     bool& none,
                     QHash<QString, bool>& plugins );
    void
    getPluginStatus( bool& all,
                     bool& none,
                     QHash<QString, bool>& plugins );

    // ----- functions that emit signals in cubepluginapi::PluginServices

    /** called before context menu of a tree widget is shown */
    void
    contextMenuIsShown( DisplayType type,
                        TreeItem*   item );

    /** called, after tree widget item has been selected or deselected */
    void
    treeItemIsSelected( TreeItem* item );

    /** called after order of tabs has changed */
    void
    setTabOrder( const QList<DisplayType>& order );

    /** Sets the value of the given name and notifies all plugins that the value has changed .
     *  If sender is given, the sender isn't notified.
     */
    void
    setGlobalValue( const QString&                 name,
                    const QVariant&                value,
                    cubepluginapi::PluginServices* sender = 0 );

    void
    genericUserAction( UserAction action );

    void
    tabActivated( DisplayType type );

    // ----- end functions that emit signals in cubepluginapi::PluginServices

    const QList<ColorMap*>
    getColorMaps()
    {
        return colormaps.values();
    }

    const QList<ValueView*>
    getValueViews()
    {
        return valueViews.values();
    }
    ValueView*
    getValueView( const QString& name );

    QMainWindow*
    getMainWindow();
    void
    setMainWindow( QMainWindow* main );

    void
    resizeTreeItemMarker( int iconHeight );

    QList<TreeItemMarker*>
    getTreeItemMarker();

    QAction*
    getMarkerConfigAction();

    void
    closePlugin( const QString& pluginName );

    QList<QAction*>
    getContextFreeActions();

    cube::CubeProxy*
    getCube() const;

    void
    setPluginArguments( QStringList& args );

    /** stop parallel calculation of plugins; used if data (e.g. selection) has become deprecated */
    void
    cancelCalculations();

    cubepluginapi::PluginServices*
    getService( const QString& pluginName );

    friend class cubepluginapi::PluginServices;

public slots:
    void
    closeContextFreePlugin();

signals:
    void
    recalculateRequest( Tree* );

private slots:
    void
    enablePlugin( bool );
    void
    showSettingsWidget();
    void
    checkPluginMenuStatus();
    void
    showPluginInfo();
    void
    showPluginHelp();
    void
    showTreeItemMarkerSettings();

    // context free plugins
    void
    startPlugin();
    void
    configureSearchPath();

private:
    PluginManager();
    void
    openPlugins();
    void
    openAllPlugins();
    bool
    openPlugin( const QString& name );
    void
    setMarkerConfig();
    void
    fillPluginMenu();
    void
    fillHelpMenu();
    void
    fillContextFreePluginMenu();
    void
    recalculateTree( Tree* );

    QMainWindow*       mainWindow;           // required for toolbar, status bar...
    TabManager*        tabManager;
    QMenu*             pluginMenu;
    cube::CubeProxy*   cube;
    QList<DisplayType> order;
    QString            cubeFileName;

    QHash<QString, QMenu* >                        pluginMenus;             // contains parent menu for each plugin that creates menu items
    QHash<QString, cubepluginapi::PluginServices*> pluginServices;          // contains plugin service for each plugin and internal service
    QHash<QString, QAction*>                       pluginActivationActions; // active/deactivate action for each plugin
    QAction*                                       markerMenuAction;        // pops up marker configuration dialog
    QAction*                                       configPathItem;
    QAction*                                       initialSettingsItem;
    QMenu*                                         pluginStatusMenu;
    QMenu*                                         infoMenu;
    QMenu*                                         helpMenu;
    QMenu*                                         contextFreePluginMenu;
    QList<QAction*>                                contextFreeActions;

    QMultiHash<cubepluginapi::PluginServices*, ValueView*> valueViews;  // value views which are registered by a plugin
    QMultiHash<cubepluginapi::PluginServices*, ColorMap*>  colormaps;   // colormaps which are registered by a plugin
    QMultiHash<cubepluginapi::PluginServices*, QToolBar* > toolBarHash; // contains the ToolBar(s) for each registered service

    QHash<QString, bool>     pluginStatus;                              // contains current status (enabled/disabled) for each plugin
    QHash<QString, QVariant> globalValues;                              // allow the plugins to read and write global values
    bool                     enableAllPlugins;                          // if true, all plugins are ensabled
    bool                     disableAllPlugins;                         // if true, all plugins are disabled
    static bool              disableAlways;                             // true, if calls with -disable-plugins (plugin menu is not shown)
    static PluginManager*    single;

    // context free plugins
    cubepluginapi::ContextFreePlugin* currentContextFreePlugin;
    QAction*                          closeContextAction;

    TreeItemMarkerDialog* markerDialog;
    QStringList           pluginPath;
    QStringList           contextFreeArgs; // command line arguments for context free plugin
};

class PathDialog : public QDialog
{
    Q_OBJECT
public:
    static QStringList
    getPath( QStringList init = QStringList() );

private slots:
    void
    choose();
    void
    add();
    void
    edit();
    void
    deleteRow();
    void
    up();
    void
    down();

private:
    PathDialog( QWidget* parent = 0 );
    QLineEdit*   line;
    QListWidget* listWidget;
};
}
#endif // PLUGINMANAGER_H

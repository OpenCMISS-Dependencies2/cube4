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

#include <QSettings>
#include <QMessageBox>
#include <QDebug>
#include <QInputDialog>
#include "Settings.h"
#include "CubeServices.h"
#include "PluginManager.h"
#include "SettingsToolBar.h"
#include "Tree.h"

#define STARTUP "startup"

using namespace cubegui;

Settings::Settings()
{
    // create toolbar
    bar = new BookmarkToolBar();
    connect( bar, SIGNAL( changeBookmarks( const QList<Bookmark>& ) ), this, SLOT( changeBookmarks( QList<Bookmark> ) ) );
    connect( bar, SIGNAL( saveBookmark( const QString &, const QString & ) ), this, SLOT( saveExperimentSettings( const QString &, const QString & ) ) );
    connect( bar, SIGNAL( loadBookmark( const QString & ) ), this, SLOT( loadExperimentSettings( const QString & ) ) );
    bar->setVisible( false );
}

void
Settings::registerSettingsHandler( SettingsHandler* handler )
{
    handlerList.append( handler );
}

void
Settings::registerSettingsHandler( InternalSettingsHandler* handler )
{
    handlerList.append( handler );

    // load global startup setting for each handler once after registration
    QSettings settings;
    internalHandlerList.append( handler );
    settings.beginGroup( STARTUP );
    handler->loadGlobalStartupSettings( settings );
    settings.endGroup();
}

void
Settings::cubeLoaded( const QString& cubeFile )
{
    // always load global settings after having read a cube file, because plugins only get active after
    // an experiment has been loaded and after that global plugin settings may be set
    loadGlobalSettings();

    if ( !cubeFile.isEmpty() )
    {
        iniFile = QString( cube::services::get_cube_name( cubeFile.toStdString() ).c_str() ).append( ".ini" );

        const QString fileProtocol = "file://";
        if ( iniFile.startsWith( fileProtocol ) )
        {
            iniFile.remove( 0, fileProtocol.length() );
        }
        else // don't create ini-files if network protocol is used
        {
            iniFile.clear();
        }

        QSettings experimentSettings( iniFile, QSettings::IniFormat );
        updateBookmarkList( experimentSettings );

        bar->setVisible( settingIsTrue( "showBookmarkToolbar" ) );

        loadLastState();
    }
}

void
Settings::cubeClosed()
{
    saveGlobalSettings();
    saveLastState();
    bar->setVisible( false );
    iniFile = "";
}

void
Settings::saveLastState()
{
    if ( iniFile.isEmpty() || !settingIsTrue( "restoreLastState" ) )
    {
        return;
    }

    QSettings lastSettings( iniFile, QSettings::IniFormat );
    lastSettings.beginGroup( "lastState" );

    // image + status line
    QWidget* main = Globals::getMainWindow()->findChild<QWidget*>( "main" );
    QPixmap  pixmap;
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
    pixmap = main->grab();
#else
    pixmap = QPixmap::grabWidget( main );
#endif
    lastSettings.setValue( "image", pixmap );
    int numberOfLeafs = 0;
    foreach( TreeItem * item, Globals::getTabManager()->getActiveTree( SYSTEM )->getItems() )
    {
        if ( item->getChildren().size() == 0 )
        {
            numberOfLeafs++;
        }
    }
    lastSettings.setValue( "locations", numberOfLeafs );

    // bookmarks
    foreach( SettingsHandler * handler, handlerList )
    {
        lastSettings.beginGroup( handler->settingNameCompact() );
        handler->saveExperimentSettings( lastSettings );
        lastSettings.endGroup();
    }

    lastSettings.endGroup();
}

void
Settings::loadLastState()
{
    if ( iniFile.isEmpty() || !settingIsTrue( "restoreLastState" ) )
    {
        return;
    }
    QSettings lastSettings( iniFile, QSettings::IniFormat );
    lastSettings.beginGroup( "lastState" );

    foreach( SettingsHandler * handler, handlerList )
    {
        lastSettings.beginGroup( handler->settingNameCompact() );
        handler->loadExperimentSettings( lastSettings );
        lastSettings.endGroup();
    }

    lastSettings.endGroup();
}

Settings::~Settings()
{
    saveGlobalSettings();

    QSettings settings;
    settings.beginGroup( STARTUP );
    foreach( InternalSettingsHandler * handler, internalHandlerList )
    {
        handler->saveGlobalStartupSettings( settings );
    }
    settings.endGroup();
}

QAction*
Settings::getBookmarkToolbarAction()
{
    QAction* bookmarks = new QAction( tr( "Show bookmark toolbar" ), this );
    QString  name      = "showBookmarkToolbar";
    bookmarks->setCheckable( true );
    bookmarks->setData( name );
    connect( bookmarks, SIGNAL( toggled( bool ) ), this, SLOT( setGlobalBoolSetting( bool ) ) );
    bookmarks->setChecked( settingIsTrue( name, false ) );

    return bookmarks;
}

QToolBar*
Settings::getToolBar()
{
    return bar;
}

void
Settings::saveNamedGlobalSettings()
{
    QWidget*  widget = PluginManager::getInstance()->getMainWindow();
    QSettings settings;
    settings.beginGroup( "settings" );

    QStringList settingsList = settings.childGroups();
    bool        ok;
    QString     settingsName =
        QInputDialog::getItem( widget,
                               tr( "Save settings" ),
                               tr( "Save settings under the name:" ),
                               settingsList,
                               0,
                               true,
                               &ok );

    // save settings if everything is ok with the input
    if ( ok && !settingsName.isEmpty() )
    {
        saveGlobalSettings( settingsName );
    }
    settings.endGroup();
}

void
Settings::loadNamedGlobalSettings()
{
    QWidget*  widget = PluginManager::getInstance()->getMainWindow();
    QSettings settings;
    settings.beginGroup( "settings" );

    QStringList settingsList = settings.childGroups();
    bool        ok;
    QString     settingsName =
        QInputDialog::getItem( widget,
                               tr( "Load settings" ),
                               tr( "Load settings with name:" ),
                               settingsList,
                               0,
                               false,
                               &ok );
    if ( ok && !settingsName.isEmpty() )
    {
        loadGlobalSettings( settings, settingsName );
    }
    settings.endGroup();
}

void
Settings::deleteNamedGlobalSettings()
{
    QWidget*  widget = PluginManager::getInstance()->getMainWindow();
    QSettings settings;
    settings.beginGroup( "settings" );

    QStringList settingsList = settings.childGroups();
    bool        ok;
    QString     settingsName =
        QInputDialog::getItem( widget,
                               tr( "Delete settings" ),
                               tr( "Delete settings with name:" ),
                               settingsList,
                               0,
                               false,
                               &ok );
    if ( ok && !settingsName.isEmpty() )
    {
        settings.remove( settingsName );
    }
    settings.endGroup();
}

/** return true, if an entry with the given name and the value true exists in the global settings */
bool
Settings::settingIsTrue( const QString& name, bool init )
{
    bool      ok;
    QSettings settings;
    settings.beginGroup( STARTUP );
    ok = settings.value( name, init ).toBool();
    settings.endGroup();
    return ok;
}


/**
   slot, which saves state of an action to global settings in section [startup]
 */
void
Settings::setGlobalBoolSetting( bool state )
{
    QAction* action = qobject_cast<QAction*>( sender() );
    QString  name   = action->data().toString();

    QSettings settings;
    settings.beginGroup( STARTUP );
    settings.setValue( name, state );

    if ( name == "showBookmarkToolbar" && !iniFile.isEmpty() )
    {
        if ( bar )
        {
            bar->setVisible( state );
        }
    }

    settings.endGroup();
}

/** reinitializes bookmark list from settings */
void
Settings::updateBookmarkList( QSettings& settings )
{
    // fill list of saved experiment settings
    settings.beginGroup( "experiment" );
    QStringList names = settings.childGroups();
    bookmarks.clear();
    foreach( QString name, names )
    {
        settings.beginGroup( name );
        bookmarks.append( Bookmark( name, settings.value( "description" ).toString() ) );
        settings.endGroup();
    }
    settings.endGroup();
    bar->setBookmarks( bookmarks );
}

QStringList
Settings::getBookmarks() const
{
    QStringList list;
    foreach( Bookmark bm, bookmarks )
    list.append( bm.getName() );
    return list;
}

QString
Settings::getBookmarkDescription( const QString& name ) const
{
    foreach( Bookmark bm, bookmarks )
    {
        if ( bm.getName() == name )
        {
            return bm.getDescription();
        }
    }
    return QString();
}

void
Settings::loadExperimentSettings( const QString& group, bool loadGlobals )
{
    if ( iniFile.isEmpty() || !QFile( iniFile ).exists() )
    {
        return;
    }
    QSettings experimentSettings( iniFile, QSettings::IniFormat );
    loadExperimentSettings( experimentSettings, group, loadGlobals );
}

void
Settings::saveExperimentSettings( const QString& group, const QString& description )
{
    if ( !iniFile.isEmpty() )
    {
        QSettings experimentSettings( iniFile, QSettings::IniFormat );
        saveExperimentSettings( experimentSettings, group, description );
        updateBookmarkList( experimentSettings );
    }
}

// --------------- private -----------------------

void
Settings::loadExperimentSettings( QSettings& settings, const QString& group, bool loadGlobals )
{
    bool restoreExperiment = true;

    settings.beginGroup( "experiment" );
    if ( !settings.childGroups().contains( group ) ) // local ini file is invalid/empty
    {
        restoreExperiment = false;
    }
    settings.endGroup();

    if ( loadGlobals )
    {
        loadGlobalSettings( settings, "globalSettings", true );
    }

    if ( restoreExperiment )
    {
        settings.beginGroup( "experiment" );
        settings.beginGroup( group );
        foreach( SettingsHandler * handler, handlerList )
        {
            settings.beginGroup( handler->settingNameCompact() );
            handler->loadExperimentSettings( settings );
            settings.endGroup(); // settingName
        }
        settings.endGroup();     // group
        settings.endGroup();     // experiment
    }
}


/** saves the global settings and the settings which belong to a specific experiment
 * @param group optional parameter, if more than one state of the experiment is saved, a name
 * for the state has to be given.
 */
void
Settings::saveExperimentSettings( QSettings& settings, const QString& group, const QString& description )
{
    saveGlobalSettings( settings, "globalSettings", true );

    settings.beginGroup( "experiment" );
    settings.beginGroup( group ); // name selected by the user
    settings.setValue( "description", description );
    foreach( SettingsHandler * handler, handlerList )
    {
        settings.beginGroup( handler->settingNameCompact() );
        handler->saveExperimentSettings( settings );
        settings.endGroup(); // settingName
    }
    settings.endGroup();     // group
    settings.endGroup();     // experiment
}

/**
 * @brief Settings::updateBookmarks saves the changes from the bookmark list to file.
 * @param bookmarks list of bookmarks, which have been edited
 */
void
Settings::changeBookmarks( const QList<Bookmark>& changedBookmarks )
{
    if ( iniFile.isEmpty() )
    {
        return;
    }

    QSettings settings( iniFile, QSettings::IniFormat );

    foreach( Bookmark bookmark, changedBookmarks )
    {
        // update bookmark description
        settings.beginGroup( "experiment" );
        settings.beginGroup( bookmark.getName() );
        settings.setValue( "description", bookmark.getDescription() );
        settings.endGroup();
        settings.endGroup();

        // change bookmark name: create new entry and delete old one later
        if ( bookmark.getName() != bookmark.getOriginalName() )
        {
            renameBookmark( settings, bookmark );
        }
    }

    // delete bookmarks
    foreach( Bookmark bookmark, bookmarks )
    {
        if ( !changedBookmarks.contains( bookmark ) )
        {
            deleteExperimentSettings( bookmark.getName() );
        }
    }

    updateBookmarkList( settings ); // reinitialize bookmarks from settings
}

void
Settings::renameBookmark( QSettings& settings, const Bookmark& bookmark )
{
    QString         newName = bookmark.getName();
    QString         oldName = bookmark.getOriginalName();
    QList<QVariant> values;

    settings.beginGroup( "experiment" );
    settings.beginGroup( oldName );
    QStringList keys = settings.allKeys();
    foreach( QString key, keys )
    {
        values.append( settings.value( key ) );
    }
    settings.endGroup();
    settings.beginGroup( newName );
    for ( int i = 0; i < keys.size(); i++ )
    {
        settings.setValue( keys[ i ], values[ i ] );
    }
    settings.endGroup();

    settings.endGroup();
}

void
Settings::deleteExperimentSettings( const QString& name )
{
    if ( !iniFile.isEmpty() )
    {
        QSettings settings( iniFile, QSettings::IniFormat );

        settings.beginGroup( "experiment" );
        settings.remove( name );
        settings.endGroup(); // experiment

        updateBookmarkList( settings );
    }
}

/** Settings::loadGlobalSettings loads global settings from system location */
void
Settings::loadGlobalSettings( const QString& group )
{
    QSettings settings;
    settings.beginGroup( "settings" );
    loadGlobalSettings( settings, group );
    settings.endGroup();
}

/** Settings::saveGlobalSettings saves global settings to system location */
void
Settings::saveGlobalSettings( const QString& group )
{
    QSettings settings;
    settings.beginGroup( "settings" );
    saveGlobalSettings( settings, group );
    settings.endGroup();
}

/** Settings::loadGlobalSettings loads the global settings from the given location settings. */
void
Settings::loadGlobalSettings( QSettings& settings, const QString& group, bool isExperiment )
{
    settings.beginGroup( group );
    foreach( SettingsHandler * handler, handlerList )
    {
        settings.beginGroup( handler->settingNameCompact() );
        handler->loadGlobalSettings( settings );
        if ( !isExperiment )
        {
            handler->loadGlobalOnlySettings( settings );
        }
        settings.endGroup(); // settingName
    }
    settings.endGroup();     // group
}

/** Settings::saveGlobalSettings saves the global settings to the given location settings. */
void
Settings::saveGlobalSettings( QSettings& settings, const QString& group, bool isExperiment )
{
    settings.beginGroup( group );
    foreach( SettingsHandler * handler, handlerList )
    {
        settings.beginGroup( handler->settingNameCompact() );
        handler->saveGlobalSettings( settings );
        if ( !isExperiment )
        {
            handler->saveGlobalOnlySettings( settings );
        }
        settings.endGroup(); // settingName
    }
    settings.endGroup();     // group
}

QMenu*
Settings::getMenu()
{
    QMenu* settingsMenu = new QMenu( tr( "Settings" ) );
    settingsMenu->setStatusTip( tr( "Ready" ) );
    settingsMenu->setWhatsThis( tr( "This menu item offers the saving, loading, and the deletion of settings. Global settings store the outlook of the application like the widget sizes, color and precision settings, the order of panes, etc." ) );

    QAction* saveSettingsAct = new QAction( tr( "&Save global settings as..." ), this );
    saveSettingsAct->setStatusTip( tr( "Saves named global settings" ) );
    connect( saveSettingsAct, SIGNAL( triggered() ), this, SLOT( saveNamedGlobalSettings() ) );
    settingsMenu->addAction( saveSettingsAct );
    QAction* loadSettingsAct = new QAction( tr( "&Load global settings..." ), this );
    loadSettingsAct->setStatusTip( tr( "Loads the specified global settings" ) );
    connect( loadSettingsAct, SIGNAL( triggered() ), this, SLOT( loadNamedGlobalSettings() ) );
    settingsMenu->addAction( loadSettingsAct );
    QAction* delSettingsAct = new QAction( tr( "&Delete global settings..." ), this );
    connect( delSettingsAct, SIGNAL( triggered() ), this, SLOT( deleteNamedGlobalSettings() ) );
    settingsMenu->addAction( delSettingsAct );
    settingsMenu->addSeparator();

    QAction* saveLastAct = new QAction( tr( "Restore last state" ), this );
    saveLastAct->setCheckable( true );
    saveLastAct->setData( "restoreLastState" );
    saveLastAct->setChecked( settingIsTrue( "restoreLastState", false ) );
    connect( saveLastAct, SIGNAL( toggled( bool ) ), this, SLOT( setGlobalBoolSetting( bool ) ) );
    settingsMenu->addAction( saveLastAct );

    return settingsMenu;
}

// =====================================================================================================

Bookmark::Bookmark( const QString& n, const QString& d ) : name( n ), description( d ), originalName( n )
{
}

QString
Bookmark::getName() const
{
    return name;
}

QString
Bookmark::getOriginalName() const
{
    return originalName;
}

QString
Bookmark::getDescription() const
{
    return description;
}

void
Bookmark::setDescription( const QString& value )
{
    description = value;
}

void
Bookmark::rename( const QString& value )
{
    name = value;
}

void
Bookmark::erase()
{
    name.clear();
}

bool
Bookmark::operator==( const Bookmark& b ) const
{
    return this->name == b.name;
}

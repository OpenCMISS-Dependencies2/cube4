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


#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include "PluginServices.h"
#include "SettingsHandler.h"

namespace cubegui
{
class BookmarkToolBar;

class Bookmark
{
public:
    Bookmark( const QString& n,
              const QString& d );
    QString
    getName() const;
    QString
    getOriginalName() const;
    QString
    getDescription() const;
    void
    setDescription( const QString& value );
    void
    rename( const QString& value );
    void
    erase();
    bool
    operator==( const Bookmark& b ) const;

private:
    QString name;
    QString description;
    QString originalName;
};

/** Internal classes (not plugins) can implement this interface to load settings only at startup. */
class InternalSettingsHandler : public SettingsHandler
{
public:
    /** this method is only called once, at startup */
    virtual void
    loadGlobalStartupSettings( QSettings& )
    {
    }

    /** this method is only called once, if program ends */
    virtual void
    saveGlobalStartupSettings( QSettings& )
    {
    }
};

/**
   Three types of settings are available:
   - global settings without a group name (= groupname "General"), which are used to save settings
    options itself and the plugin settings. They are automatically saved in the native format.
   - global settings with default group or named group are also saved in native format. The named
     settings are created if the menu item "Save Global Settings as" is selected.
   - bookmarks (experiment settings) contain settings which depend on the loaded cube file. They are saved in
    cubeFileBasename.ini. They also contain the global settings.

   group structure of global settings in native format:
    -General
    -settings
      -settingNameA
   group structure of experiment settings in cubeFile.ini:
    -globalSettings
    -experiment
      -experimentNameA
 */
class Settings : public QObject
{
    Q_OBJECT
public:
    /**
     * @param settingsMenu menu to fill with setting
     * @param handler list of SettingHandler whose methods will be called if settings are loaded or saved */
    Settings();

    /** destroys the Settings object and saves the startup settings */
    ~Settings();

    /** creates a menu with the given settings as menu items */
    QMenu*
    getMenu();
    QAction*
    getBookmarkToolbarAction();

    /** loads the default experiment settings from the given location */
    void
    cubeLoaded( const QString& cubeFile );

    /** saves the default experiment settings to file */
    void
    cubeClosed();

    void
    registerSettingsHandler( SettingsHandler* handler );
    void
    registerSettingsHandler( InternalSettingsHandler* handler );

    /** loads global settings from system location */
    void
    loadGlobalSettings( const QString& group = "last" );

    QToolBar*
    getToolBar();

    QStringList
    getBookmarks() const;

    QString
    getBookmarkDescription( const QString& name ) const;

public slots:
    void
    loadExperimentSettings( const QString& name,
                            bool           loadGlobals = false );

private slots:
    /** saves the experiment settings of the previouly loaded cube
       @param group name of the settings */
    void
    saveExperimentSettings( const QString& group,
                            const QString& description = "" );

    /** load the experiment settings from the previouly loaded cube
       @param group name of the settings */
    void
    deleteExperimentSettings( const QString& group );

    void
    setGlobalBoolSetting( bool state );
    void
    saveNamedGlobalSettings();
    void
    loadNamedGlobalSettings();
    void
    deleteNamedGlobalSettings();
    void
    changeBookmarks( const QList<Bookmark>& bookmarks );

private:
    bool
    settingIsTrue( const QString& name,
                   bool           init = false );

    /** saves global settings to system location */
    void
    saveGlobalSettings( const QString& group = "last" );

    void
    loadGlobalSettings( QSettings&     settings,
                        const QString& group = "last",
                        bool           isExperiment = false );
    void
    saveGlobalSettings( QSettings&     settings,
                        const QString& group = "last",
                        bool           isExperiment = false );

    void
    loadExperimentSettings( QSettings&     settings,
                            const QString& group,
                            bool           loadGlobals );
    void
    saveExperimentSettings( QSettings&     settings,
                            const QString& group,
                            const QString& description );
    void
    updateBookmarkList( QSettings& settings );
    void
    renameBookmark( QSettings&      settings,
                    const Bookmark& bookmark );

    QList<SettingsHandler*>         handlerList;
    QList<InternalSettingsHandler*> internalHandlerList;

    QString          iniFile;                 // location of ini file to save experiment settings
    BookmarkToolBar* bar;                     // toolbar for experiment specific settings
    QList<Bookmark>  bookmarks;
    void
    saveLastState();
    void
    loadLastState();
};
}
#endif // SETTINGS_H

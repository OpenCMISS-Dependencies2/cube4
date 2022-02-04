/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SETTINGHANDLER_H
#define SETTINGHANDLER_H

#include <QSettings>

namespace cubegui
{
/**
 * SettingsHandler provides methods which are called, if the settings are loaded or saved
 * @see PluginServices::addSettingsHandler
 * @see examples/GUI/plugins/plugin-demo
 */
class SettingsHandler
{
public:
    /** these settings are only saved for global and experiment specific scope */
    virtual void
    loadGlobalSettings( QSettings& )
    {
    };
    virtual void
    saveGlobalSettings( QSettings& )
    {
    };

    /** these settings contain experiment specific options, that can only be applied if an experiment is loaded */
    virtual void
    loadExperimentSettings( QSettings& )
    {
    };
    virtual void
    saveExperimentSettings( QSettings& )
    {
    };

    /** these settings are only saved for global scope. Use these methods only, if experiment specific settings are not wanted */
    virtual void
    loadGlobalOnlySettings( QSettings& )
    {
    };
    virtual void
    saveGlobalOnlySettings( QSettings& )
    {
    };

    /** Used to synchronize status of several cube instances, @see ClipboardToolBar. */
    virtual void
    loadStatus( QSettings& )
    {
    }
    virtual void
    saveStatus( QSettings& )
    {
    }
    /** synchronizationIsDefault returns true, if the synchronization should be enabled without manual selection */
    virtual bool
    synchronizationIsDefault()
    {
        return false;
    }

    virtual QString
    settingName() = 0;

    QString
    settingNameCompact() // blanks would be escaped in ini file with a longer sequence
    {
        return settingName().remove( ' ' );
    }
};
}
#endif // SETTINGHANDLER_H

/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2017                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/




#ifndef CUBEPLUGIN_H
#define CUBEPLUGIN_H

#include <QTextStream>
#include "PluginInterface.h"

namespace cubegui
{
class PluginList;
class PluginManager;
}

namespace cubepluginapi
{
class PluginServices;

/**
 * @brief The CubePlugin class is base class of all plugins that depend on a loaded cube file.
 */
class CubePlugin : public PluginInterface
{
public:
    /**
     * @brief cubeOpened is called after a cube file has been loaded. It should return false, if
     * the plugin should not be started with current data.
     */
    virtual bool
    cubeOpened( cubepluginapi::PluginServices* service ) = 0;

    /**
     * @brief cubeClosed is called after the cube file has been closed. If resources have been allocated,
     * they should be freed here.
     */
    virtual void
    cubeClosed()
    {
    };

    virtual void
    version( int& major,
             int& minor,
             int& bugfix ) const = 0;

    /** @brief returns the unique plugin name. Only one plugin with this name will beloaded. */
    virtual QString
    name() const = 0;

    /** @brief returns a short text to describe the plugin. It will be used by help->plugin info menu of the Cube GUI. */
    virtual QString
    getHelpText() const = 0;

    /** @brief returns path to the html documentation. If path starts with http://, https:// or file:/ the path
     * is handled as absolute path. Otherwise it is a relative path and refers to the Cube User Guide.
     * The URL will be used by the help->plugins menu of the Cube GUI.
     */
    virtual QString
    getHelpURL() const
    {
        return QString();
    }

    /** @brief returns the reason why the plugin decided to deactivate itsself in cubeOpened */
    virtual QString
    getDeactivationMessage()
    {
        return "";
    }
    friend class cubegui::PluginManager;
    friend class cubegui::PluginList;

private:
    bool isInitialized;
};
}
Q_DECLARE_INTERFACE( cubepluginapi::CubePlugin, PLUGIN_VERSION )

#endif

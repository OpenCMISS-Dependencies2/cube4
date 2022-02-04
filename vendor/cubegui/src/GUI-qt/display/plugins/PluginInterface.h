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


#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#define PLUGIN_VERSION "cubeplugin/1.3.1"

#include "PluginServiceExports.h"

namespace cubepluginapi
{
class PluginInterface
{
public:
    /** @brief sets the version number of the plugin.
     *
     * Each plugin has to set a version number. If several plugins with the same identifier (@see name())
     * exist, the one with the highest version number will be loaded. */
    virtual void
    version( int& major,
             int& minor,
             int& bugfix ) const = 0;

    /** @brief returns the unique plugin name. Only one plugin with this name will beloaded. */
    virtual QString
    name() const = 0;

    /** @brief returns a text to describe the plugin. It will be used by help menu of the Cube GUI. */
    virtual QString
    getHelpText() const = 0;

    QString
    versionLabel()
    {
        int major, minor, bugfix;
        version( major, minor, bugfix );
        QString     label;
        QTextStream out( &label );
        out << name() << " V" << major << "." << minor << "." << bugfix;
        return label;
    }
    virtual
    ~PluginInterface()
    {
    }
};
}

Q_DECLARE_INTERFACE( cubepluginapi::PluginInterface, PLUGIN_VERSION )

#endif // PLUGININTERFACE_H

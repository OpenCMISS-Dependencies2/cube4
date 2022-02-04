/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2018                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef PLUGINLIST_H
#define PLUGINLIST_H

#include "CubePlugin.h"
#include "ContextFreePlugin.h"

#include <QtCore>

namespace cubegui
{
/**
 * @brief PluginList is a list of all available plugins that implement the CubePlugin interface
 */
class PluginData
{
public:
    QString                         name;      // unique name of the plugin, only one plugin of the same name will be loaded
    QString                         location;  // absolute file name of the plugin
    cubepluginapi::PluginInterface* plugin;
    QPluginLoader*                  pluginLoader;
};

class PluginList
{
public:
    static QList<cubepluginapi::CubePlugin*>
    getCubePluginList();
    static cubepluginapi::CubePlugin*
    getCubePlugin( const QString& name );
    static QString
    getCubePluginPath( const QString& name );

    static QList<cubepluginapi::ContextFreePlugin*>
    getContextFreePluginList();
    static cubepluginapi::ContextFreePlugin*
    getContextFreePlugin( const QString& name );
    static void
    disablePlugins();
    static void
    updatePluginList( const QStringList& );

private:
    void
    loadPluginList( const QStringList& pluginPath = QStringList() );
    bool
    loadCubePlugin( PluginData& data );
    bool
    loadContextFreePlugin( PluginData& data );
    bool
    loadPlugin( PluginData&        data,
                QList<PluginData>& list );

    static PluginList* single;
    static bool        disableAlways;

    QList<PluginData> cubePluginList;
    QList<PluginData> contextFreePluginList;
    PluginData*
    getPluginData( cubepluginapi::PluginInterface* plugin );
    static PluginList*
    getInstance();
    static bool
    comparePlugin( PluginData p1,
                   PluginData p2 );
};
}
#endif // PLUGINLIST_H

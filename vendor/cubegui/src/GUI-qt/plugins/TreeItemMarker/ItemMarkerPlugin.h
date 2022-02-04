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


#ifndef MARKERPLUGIN_H
#define MARKERPLUGIN_H
#include <QWidget>
#include <QLabel>
#include <QHash>
#include "PluginServices.h"
#include "CubePlugin.h"

namespace itemmarkerplugin
{
class ItemMarkerPlugin : public QObject, public cubepluginapi::CubePlugin, cubepluginapi::SettingsHandler
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "TreeItemMarkerPlugin" ) // unique plugin name
#endif

public:
    // CubePlugin implementation
    virtual bool
    cubeOpened( cubepluginapi::PluginServices* service );
    virtual void
    cubeClosed();
    virtual QString
    name() const;
    virtual void
    version( int& major,
             int& minor,
             int& bugfix ) const;
    virtual QString
    getHelpText() const;

    virtual QString
    getHelpURL() const
    {
        return "TreeItemMarkerPlugin.html";
    }

    virtual void
    loadExperimentSettings( QSettings& );

    virtual void
    saveExperimentSettings( QSettings& );

    virtual QString
    settingName()
    {
        return "ItemMarkerPlugin";
    }

private slots:

    void
    contextMenuIsShown( cubepluginapi::DisplayType,
                        cubepluginapi::TreeItem* item );

    void
    setMarker();

    void
    removeMarker();

    void
    editMarker();

private:
    cubepluginapi::PluginServices*       service;
    cubepluginapi::TreeItem*             contextItem;
    const cubepluginapi::TreeItemMarker* marker;
    cubepluginapi::DefaultMarkerLabel*   markerLabel;

    QList<cubepluginapi::TreeItem*> markedItems;
};
}

#endif

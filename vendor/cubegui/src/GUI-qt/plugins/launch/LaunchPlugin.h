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


#ifndef LAUNCHPLUGIN_H
#define LAUNCHPLUGIN_H
#include <QtGui>
#include "CubePlugin.h"
#include "PluginServices.h"
#include "LaunchInfo.h"



class LaunchPlugin : public QObject, cubepluginapi::CubePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "LaunchPlugin" )
#endif


public:
    virtual QString
    name() const;

    virtual bool
    cubeOpened( cubepluginapi::PluginServices* service );

    virtual void
    cubeClosed();

    virtual void
    version( int& major,
             int& minor,
             int& bugfix ) const;

    virtual QString
    getHelpText() const;

private slots:
    void
    contextMenuIsShown( cubepluginapi::DisplayType type,
                        cubepluginapi::TreeItem*   item );
    virtual void
    orderHasChanged( const QList<cubepluginapi::DisplayType>& order );

private slots:
    void
    onLaunch();

private:
    QList<LaunchInfo*>                                                       launchInfoList;
    cubepluginapi::PluginServices*                                           service;
    QHash<int, QPair<cubepluginapi::DisplayType, cubepluginapi::TreeItem*> > contextHash;
    const cubepluginapi::TreeItemMarker*                                     marker;
};

#endif // LAUNCHPLUGIN_H

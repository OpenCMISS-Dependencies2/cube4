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


#ifndef VAMPIRPLUGIN_H
#define VAMPIRPLUGIN_H
#include <QtGui>
#include "CubePlugin.h"
#include "Cube.h"
#include "CubeMetric.h"
#include "PluginServices.h"

class VampirConnecter;

class VampirPlugin : public QObject, cubepluginapi::CubePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "VampirPlugin" )
#endif


public:
    virtual QString
    name() const;
    virtual bool
    cubeOpened( cubepluginapi::PluginServices* service );

    virtual void
    cubeClosed();

    void
    version( int& major,
             int& minor,
             int& bugfix ) const;

    QString
    getHelpText() const;

    QString
    getDeactivationMessage();

private slots:
    void
    contextMenuIsShown( cubepluginapi::DisplayType type,
                        cubepluginapi::TreeItem*   item );
    void
    globalValueChanged( const QString& name );
    void
    onShowMaxSeverity();
    void
    vampirSettings();

private:
    cubepluginapi::PluginServices* service;
    cubepluginapi::TreeItem*       contextMenuItem;     // item on which the context menu has been invoked
    cubepluginapi::DisplayType     contextMenuTreeType; // tree type of contextMenuItem
    double                         enter, exit;         // most severe event enter and exit time (from StatisticPlugin)
    QString                        errorMsg;

    QList<VampirConnecter*> connecterList; // list of current vampir connections
};

#endif // VAMPIRPLUGIN_H

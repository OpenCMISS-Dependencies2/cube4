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


#ifndef ParaverPlugin_H
#define ParaverPlugin_H

#include <string>

#include <QtGui>
#include <QLineEdit>
#include "CubePlugin.h"
#include "Cube.h"
#include "CubeMetric.h"
#include "PluginServices.h"

class ParaverConnecter;

class ParaverPlugin : public QObject, cubepluginapi::CubePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "ParaverPlugin" )
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
    contextMenuIsShown( cubepluginapi::TreeItem* item );
    void
    onShowMaxSeverity();
    void
    paraverSettings();
    void
    getTraceFileName();
    void
    getConfigFileName();
    void
    globalValueChanged( const QString& name );

private:
    cubepluginapi::PluginServices* service;
    cubepluginapi::TreeItem*       contextMenuItem;       // item on which the context menu has been invoked
    cubepluginapi::DisplayType     contextMenuTreeType;   // tree type of contextMenuItem
    QLineEdit*                     configLine;
    QLineEdit*                     fileLine;
    double                         enter, exit;  // most severe event enter and exit time (from StatisticPlugin)
    ParaverConnecter*              connecter;
    QString                        errorMsg;

    QString
    connectToParaver( const std::string& configFileName,
                      const std::string& fileName );
};

#endif // ParaverPlugin_H

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


#ifndef StatisticPlugin_H
#define StatisticPlugin_H
#include <QtGui>
#include "CubePlugin.h"
#include "PluginServices.h"

#include "Cube.h"
#include "CubeMetric.h"
#include "StatisticalInformation.h"
class Statistics;

class StatisticPlugin : public QObject, cubepluginapi::CubePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "StatisticPlugin" )
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

    virtual QString
    getDeactivationMessage();

private slots:
    void
    contextMenuIsShown( cubepluginapi::DisplayType type,
                        cubepluginapi::TreeItem*   item );

    virtual void
    orderHasChanged( const QList<cubepluginapi::DisplayType>& order );

private slots:

    void
    onShowStatistics();
    void
    onShowMaxSeverityText();

private:
    cubepluginapi::PluginServices*       service;
    const cubepluginapi::TreeItemMarker* marker;
    cubepluginapi::TreeItem*             contextMenuItem;     // item on which the context menu has been invoked
    cubepluginapi::DisplayType           contextMenuTreeType; // tree type of contextMenuItem
    Statistics*                          statistics;
    QString                              errorMsg;
};

#endif // StatisticPlugin_H

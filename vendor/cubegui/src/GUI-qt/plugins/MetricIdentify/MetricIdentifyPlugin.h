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


#ifndef MetricIdentifyPlugin_H
#define MetricIdentifyPlugin_H

#include <string>

#include <QtGui>
#include <QLineEdit>
#include "CubePlugin.h"
#include "Cube.h"
#include "PluginServices.h"
#include "ScorePIdentificationRule.h"
#include "ScalascaIdentificationRule.h"
#include "RemapperIdentificationRule.h"
#include "CountersIdentificationRule.h"


namespace metric_identify
{
class MetricIdentifyPlugin : public QObject, cubepluginapi::CubePlugin
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "MetricIdentifyPlugin" )
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
    getHelpURL() const
    {
        return "MetricIdentificationPlugin.html";
    }


    virtual QString
    settingName()
    {
        return "MetricIdentify";
    }


private slots:
    void
    contextMenuIsShown( cubepluginapi::DisplayType type,
                        cubepluginapi::TreeItem*   item );

    void
    onIdentifyMetricType();
    void
    onIdentifyMetricOrigin();
    void
    onIdentifyMetricUsage();
    void
    onClearMarks();

private:
    void
    defineTreeItemMarker();

    RemapperMetricIdentificationRule remapper_rule;
    ScorePMetricIdentificationRule   scorep_rule;
    ScalascaMetricIdentificationRule scalasca_rule;
    CounterMetricIdentificationRule  counter_rule;


    QList<const cubepluginapi::TreeItemMarker*> markerList;
    cubepluginapi::PluginServices*              service;
    cubepluginapi::TreeItem*                    contextMenuItem; // item on which the context menu has been invoked
};
}
#endif // MetricIdentifyPlugin_H

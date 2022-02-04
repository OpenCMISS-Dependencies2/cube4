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


#ifndef MetricEditorPlugin_H
#define MetricEditorPlugin_H

#include <string>

#include <QtGui>
#include <QLineEdit>
#include "CubePlugin.h"
#include "CubeProxy.h"
#include "CubeMetric.h"
#include "PluginServices.h"

namespace metric_editor
{
class MetricData;
class NewDerivatedMetricWidget;

class MetricEditorPlugin : public QObject, cubepluginapi::CubePlugin, cubepluginapi::SettingsHandler
{
    Q_OBJECT
    Q_INTERFACES( cubepluginapi::CubePlugin )
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA( IID "MetricEditorPlugin" )
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
        return "MetricEditorPlugin.html";
    }

    virtual void
    loadGlobalOnlySettings( QSettings& );

    virtual void
    saveGlobalOnlySettings( QSettings& );

    virtual QString
    settingName()
    {
        return "MetricEditor";
    }


private slots:
    void
    contextMenuIsShown( cubepluginapi::DisplayType,
                        cubepluginapi::TreeItem* item );

    void
    onCreateDerivatedMetric();
    void
    onRemoveMetric();
    void
    onEditDerivatedMetric();
    void
    updateMetricFinished();
    void
    addMetricFinished();
    void
    metricEditorCancelled();

private:
    cubepluginapi::PluginServices* service;
    cubepluginapi::TreeItem*       contextMenuItem;       // item on which the context menu has been invoked
    NewDerivatedMetricWidget*      editorWidget;
    QList<MetricData*>             userMetrics;
};
}
#endif // MetricEditorPlugin_H

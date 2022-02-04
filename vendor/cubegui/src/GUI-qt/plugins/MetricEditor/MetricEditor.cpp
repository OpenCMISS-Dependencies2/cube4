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


#include "config.h"
#include <QMessageBox>
#include "MetricEditor.h"
#include "NewDerivatedMetricWidget.h"

using namespace std;
using namespace cubepluginapi;
using namespace cubegui;
using namespace metric_editor;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( MetricEditorPlugin, MetricEditorPlugin );
#endif

QString
MetricEditorPlugin::name() const
{
    return "Metric Editor";
}

void
MetricEditorPlugin::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}


bool
MetricEditorPlugin::cubeOpened( PluginServices* service )
{
    this->service = service;
    connect( service, SIGNAL( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ),
             this, SLOT( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ) );
    service->addSettingsHandler( this );

    editorWidget = 0;

    return true;
}

void
MetricEditorPlugin::contextMenuIsShown( DisplayType type, TreeItem* item )
{
    if ( type != METRIC )
    {
        return;
    }
    contextMenuItem = item;

    QAction* action = service->addContextMenuItem( cubegui::METRIC, item ? tr( "Edit metric..." ) : tr( "Create metric..." ) );
    if ( editorWidget )
    {
        action->setEnabled( false );
    }
    else if ( !item )
    {
        connect( action, SIGNAL( triggered() ), this, SLOT( onCreateDerivatedMetric() ) );
    }
    else
    {
        QMenu* editMetricMenu = new QMenu();
        action->setMenu( editMetricMenu );

        QAction* derivedMetric = new QAction( tr( "Create derived metric" ), this );
        editMetricMenu->addAction( derivedMetric );
        connect( derivedMetric, SIGNAL( triggered() ), this, SLOT( onCreateDerivatedMetric() ) );

        // exit the expression of the derived metric
        action = new QAction( tr( "Edit derived metric" ), this );
        action->setStatusTip( tr( "Shows the online description of the clicked item" ) );
        connect( action, SIGNAL( triggered() ), this, SLOT( onEditDerivatedMetric() ) );
        action->setWhatsThis( tr( "Creates a derived metric as a child of selected metric. Values of this metric are calculated as an arithmetcal expression of different constants and references to another existing metrics. Derived metrics support only DOUBLE values." ) );
        editMetricMenu->addAction( action );
        QAction* editDerived = action;
        editDerived->setEnabled( item->isDerivedMetric() );

        // create an action for showing the online metric info of the clicked item,
        // by default disabled, it will be enabled for items for which the url is defined
        action = new QAction( tr( "Remove metric" ), this );
        action->setStatusTip( tr( "Removes whole subtree of metrics from the cube" ) );
        connect( action, SIGNAL( triggered() ), this, SLOT( onRemoveMetric() ) );
        editMetricMenu->addAction( action );
        action->setWhatsThis( tr( "Removes whole subtree of metrics from the cube" ) );
        QAction* removeMetric = action;

        if ( !item )
        {
            editDerived->setEnabled( false );
            removeMetric->setEnabled( false );
        }
    }
}

void
MetricEditorPlugin::cubeClosed()
{
    foreach( MetricData * metric, userMetrics )
    {
        delete metric;
    }
}

QString
MetricEditorPlugin::getHelpText() const
{
    return "";
}

void
MetricEditorPlugin::onRemoveMetric()
{
    TreeItem*     item     = contextMenuItem;
    cube::Metric* metric   = dynamic_cast<cube::Metric*> ( item->getCubeObject() );
    QString       toRemove = QString::fromStdString( metric->get_uniq_name() );

    QStringList           availableMetricNames;
    vector<cube::Metric*> _metrics = service->getCube()->getMetrics();
    for ( vector<cube::Metric*>::iterator m_iter = _metrics.begin(); m_iter != _metrics.end(); ++m_iter )
    {
        availableMetricNames.append( QString::fromStdString( ( *m_iter )->get_uniq_name() ) );
    }

    foreach( MetricData * data, userMetrics )
    {
        if ( !availableMetricNames.contains( data->getUniq_name() ) )
        {
            continue;
        }
        NewDerivatedMetricWidget* tmp      = new NewDerivatedMetricWidget( service, nullptr, nullptr, nullptr, userMetrics );
        QList<QString>            referred = tmp->getReferredMetrics( data );
        delete tmp;
        if ( referred.contains( toRemove ) )
        {
            QMessageBox::critical( service->getParentWidget(), tr( "Error" ), tr( "Metric is referred by metric " ) + data->getUniq_name() + "." );
            return;
        }
    }

    if ( item != nullptr )
    {
        service->removeMetric( item );
    }
}

void
MetricEditorPlugin::onEditDerivatedMetric()
{
    TreeItem*     item   = contextMenuItem;
    cube::Metric* metric = static_cast<cube::Metric*> ( item->getCubeObject() );

    QWidget* parent = service->getParentWidget();
    editorWidget = new NewDerivatedMetricWidget( service, metric, nullptr, parent, userMetrics );
    editorWidget->setVisible( true );
    connect( editorWidget, SIGNAL( accepted() ), this, SLOT( updateMetricFinished() ) );
    connect( editorWidget, SIGNAL( rejected() ), this, SLOT( metricEditorCancelled() ) );
}

void
MetricEditorPlugin::metricEditorCancelled()
{
    editorWidget->disconnect();
    editorWidget->deleteLater();
    editorWidget = 0;
}

void
MetricEditorPlugin::updateMetricFinished()
{
    editorWidget->disconnect();
    service->updateMetric( contextMenuItem );
    editorWidget->deleteLater();
    editorWidget = 0;
}

void
MetricEditorPlugin::addMetricFinished()
{
    editorWidget->disconnect();
    cube::Metric* newMetric = editorWidget->get_created_metric();
    if ( newMetric != nullptr )                          // add metric to the metric tree only, if one was created
    {
        cube::Metric* parent = newMetric->get_parent();
        if ( parent == nullptr ) // create top level metric
        {
            service->addMetric( newMetric );
        }
        else // create child metric
        {
            TreeItem* parentItem = service->getMetricTreeItem( parent->get_uniq_name() );
            service->addMetric( newMetric, parentItem );
        }
    }

    editorWidget->deleteLater();
    editorWidget = 0;
}

void
MetricEditorPlugin::onCreateDerivatedMetric()
{
    TreeItem*     item   = contextMenuItem;
    cube::Metric* metric = nullptr;
    if ( item != nullptr )
    {
        metric = static_cast<cube::Metric*> ( item->getCubeObject() );
    }

    QWidget* parent = service->getParentWidget();

    editorWidget = new NewDerivatedMetricWidget( service, nullptr, metric, parent, userMetrics );
    editorWidget->setVisible( true );

    connect( editorWidget, SIGNAL( accepted() ), this, SLOT( addMetricFinished() ) );
    connect( editorWidget, SIGNAL( rejected() ), this, SLOT( metricEditorCancelled() ) );
}

void
MetricEditorPlugin::loadGlobalOnlySettings( QSettings& settings )
{
    userMetrics.clear();
    int size = settings.beginReadArray( "userMetrics" );
    for ( int i = 0; i < size; ++i )
    {
        settings.setArrayIndex( i );
        MetricData* data = new MetricData( service->getCube(), nullptr );
        data->setCubePL( settings.value( "metric" ).toString() );
        userMetrics.append( data );
    }
    settings.endArray();
}

void
MetricEditorPlugin::saveGlobalOnlySettings( QSettings& settings )
{
    settings.beginWriteArray( "userMetrics" );
    for ( int i = 0; i < userMetrics.size(); ++i )
    {
        settings.setArrayIndex( i );
        settings.setValue( "metric", userMetrics.at( i )->toString() );
    }
    settings.endArray();
}

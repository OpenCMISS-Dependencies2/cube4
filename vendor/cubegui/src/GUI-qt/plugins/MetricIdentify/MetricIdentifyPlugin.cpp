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
#include "MetricIdentifyPlugin.h"

using namespace std;
using namespace cubepluginapi;
using namespace cubegui;
using namespace metric_identify;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( MetricIdentifyPlugin, MetricIdentifyPlugin );
#endif

QString
MetricIdentifyPlugin::name() const
{
    return "Metric Identify";
}

void
MetricIdentifyPlugin::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}


bool
MetricIdentifyPlugin::cubeOpened( PluginServices* service )
{
    this->service = service;
    connect( service, SIGNAL( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ),
             this, SLOT( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ) );


    defineTreeItemMarker();

    return true;
}

void
MetricIdentifyPlugin::contextMenuIsShown( DisplayType type, TreeItem* )
{
    if ( type != METRIC )
    {
        return;
    }


    QAction* identifyAction     = service->addContextMenuItem( cubegui::METRIC, tr( "Identify metrics..." ) );
    QAction* removeIdentMarkers = service->addContextMenuItem( cubegui::METRIC, tr( "Remove identification markers" ) );
    connect( removeIdentMarkers, SIGNAL( triggered() ), this, SLOT( onClearMarks() ) );

    QMenu* itentifyMetricMenu = new QMenu();
    identifyAction->setMenu( itentifyMetricMenu );

    // create an action for creation of the derived metric
    QAction* action = new QAction( tr( "by its origin" ), this );
    action->setStatusTip( tr( "Identifies metric according to the tool created it." ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( onIdentifyMetricOrigin() ) );
    action->setWhatsThis( tr( "... to do ." ) );
    itentifyMetricMenu->addAction( action );

    // create an action for creation of the derived metric
    action = new QAction( tr( "by its type" ), this );
    action->setStatusTip( tr( "Identifies metric according to its character, e.g. communication metric, synchranisation, counter, etc." ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( onIdentifyMetricType() ) );
    action->setWhatsThis( tr( "...to do." ) );
    itentifyMetricMenu->addAction( action );
    action->setEnabled( false ); // don't know yet if useful

    // exit the expression of the derived metric
    action = new QAction( tr( "by its usage..." ), this );
    action->setStatusTip( tr( "Identifies metric according of its possible usage, like cache load analysis, load balance analysis etc." ) );
    connect( action, SIGNAL( triggered() ), this, SLOT( onIdentifyMetricUsage() ) );
    action->setWhatsThis( tr( "...to do" ) );
    itentifyMetricMenu->addAction( action );
    action->setEnabled( false ); // don't know yet if useful
}

void
MetricIdentifyPlugin::cubeClosed()
{
    markerList.clear(); // clear list for next usage
}

QString
MetricIdentifyPlugin::getHelpText() const
{
    return tr( "This plugin supports the user in identification of a metric by its origin, type, purpose and similar properties. Helps to interpret correctly presented values." );
}


/** defines a number of tree items markers, which are used to mark tree items with different background colour and
    icons */
void
MetricIdentifyPlugin::defineTreeItemMarker()
{
    QStringList    images;
    QList<QPixmap> icons;
    markerList.clear(); // clear list to store created markers
    images << ":images/CubeIcon.png"  << ":images/scorep-logo16.png" << ":images/scorep-logo32.png" << ":images/scorep-logo64.png" << ":images/scorep-logo.png" << ":images/remapping.png" << ":images/counters.png";

    icons.append( QPixmap( images.at( 0 ) ) );
    markerList.append( service->getTreeItemMarker( tr( "Scalasca Marker" ), icons ) );

    icons.clear();
    icons.append( QPixmap( images.at( 1 ) ) );
    icons.append( QPixmap( images.at( 2 ) ) );
    icons.append( QPixmap( images.at( 3 ) ) );
    icons.append( QPixmap( images.at( 4 ) ) );
    markerList.append( service->getTreeItemMarker( tr( "Score-P Marker" ), icons ) );

    icons.clear();
    icons.append( QPixmap( images.at( 5 ) ) );
    markerList.append( service->getTreeItemMarker( tr( "Remapper Marker" ), icons ) );

    icons.clear();
    icons.append( QPixmap( images.at( 6 ) ) );
    markerList.append( service->getTreeItemMarker( tr( "Counters Marker" ), icons ) );
}

void
MetricIdentifyPlugin::onIdentifyMetricType()
{
    onClearMarks();
}

void
MetricIdentifyPlugin::onIdentifyMetricOrigin()
{
    onClearMarks();
    const QList<TreeItem*>&
    metrics = service->getTreeItems( METRIC );

    foreach( TreeItem * metric, metrics )
    {
        if ( scalasca_rule.isApply( dynamic_cast<cube::Metric*>( metric->getCubeObject() ) ) )
        {
            service->addMarker( metric, markerList[ 0 ] );
        }
        if ( scorep_rule.isApply( dynamic_cast<cube::Metric*>( metric->getCubeObject() ) ) )
        {
            service->addMarker( metric, markerList[ 1 ] );
        }
        if ( remapper_rule.isApply( dynamic_cast<cube::Metric*>( metric->getCubeObject() ) ) )
        {
            service->addMarker( metric, markerList[ 2 ] );
        }
        if ( counter_rule.isApply( dynamic_cast<cube::Metric*>( metric->getCubeObject() ) ) )
        {
            service->addMarker( metric, markerList[ 3 ] );
        }
    }
    service->updateTreeView( METRIC );
}

void
MetricIdentifyPlugin::onIdentifyMetricUsage()
{
    onClearMarks();
}

void
MetricIdentifyPlugin::onClearMarks()
{
    service->removeMarker();
    service->updateTreeView( METRIC );
}

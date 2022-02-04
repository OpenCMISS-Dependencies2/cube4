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
#include <cassert>

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include "StatisticPlugin.h"
#include "CubeServices.h"
#include "Statistics.h"

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( StatisticPlugin, StatisticPlugin );
#endif

#define STRING_MARKER "max severe instance"

using namespace cubepluginapi;
using cubegui::BoxPlot;

void
StatisticPlugin::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

QString
StatisticPlugin::name() const
{
    return "Statistics";
}

bool
StatisticPlugin::cubeOpened( PluginServices* service )
{
    this->service = service;

    QList<QPixmap> icons;
    icons.append( QPixmap( ":images/boxplot-icon.png" ) );
    marker          = service->getTreeItemMarker( STRING_MARKER, icons );
    contextMenuItem = 0;

    statistics = new Statistics( service );

    connect( service, SIGNAL( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ),
             this, SLOT( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ) );

    if ( !statistics->existsStatFile() )
    {
        errorMsg = statistics->getStatFileName() + tr( " cannot be opened." );
        return false;
    }

    foreach( TreeItem * metricItem, service->getTreeItems( METRIC ) )
    {
        const cube::Metric* metric = dynamic_cast<cube::Metric* >( metricItem->getCubeObject() );
        if ( statistics->existsMaxSeverity( metric ) )
        {
            service->addMarker( marker, metricItem, 0, 0 );
            foreach( TreeItem * callItem, service->getTreeItems( CALL ) )
            {
                const cube::Cnode* cnode = dynamic_cast<cube::Cnode*> ( callItem->getCubeObject() );

                if ( statistics->existsMaxSeverity( metric, cnode ) )
                {
                    service->addMarker( marker, metricItem, callItem, 0 );
                }
            }
        }
    }

    return true;
}

void
StatisticPlugin::cubeClosed()
{
    delete statistics;
}

QString
StatisticPlugin::getHelpText() const
{
    return tr( "Shows metric statistics. Only available if a statistics file for the current cube file exists and if " \
               "statistical information for the selected metric is provided." );
}


QString
StatisticPlugin::getDeactivationMessage()
{
    return errorMsg;
}

/* create context menu items */
void
StatisticPlugin::contextMenuIsShown( DisplayType type, TreeItem* item )
{
    if ( item == NULL )
    {
        return;
    }
    contextMenuItem     = item;
    contextMenuTreeType = type;

    cube::Metric* metric = 0;

    if ( type == METRIC )
    {
        QAction* action = service->addContextMenuItem( type, tr( "Show metric statistics" ) );
        action->setStatusTip( tr( "Shows metric statistics" ) );
        action->setWhatsThis( tr( "Shows metric statistics. Only available if a statistics file for the current cube file exists and if statistical information for the selected metric is provided." ) );
        connect( action, SIGNAL( triggered() ), this, SLOT( onShowStatistics() ) );

        cube::Metric* metric = static_cast<cube::Metric*> (  item->getCubeObject() );
        bool          exists = statistics->existsStatistics( metric );
        action->setEnabled( exists );
    }
    if ( type == METRIC || type == CALL )
    {
        cube::Cnode* cnode  = 0;
        QAction*     action = service->addContextMenuItem( type, tr( "Show max severity information" ) );
        action->setStatusTip( tr( "Shows the most severe instance of pattern as text" ) );
        action->setWhatsThis( tr( "Shows the most severe instance of pattern as text. Only available if a statistics file for the current cube file exists." ) );
        connect( action, SIGNAL( triggered() ), this, SLOT( onShowMaxSeverityText() ) );

        bool   exists = true;
        double enter, exit;
        if ( type == METRIC )
        {
            metric = static_cast<cube::Metric*> (  item->getCubeObject() );
            exists = statistics->existsMaxSeverity( metric, cnode, enter, exit );
        }
        else // CALL
        {
            TreeItem* metricItem = service->getSelection( METRIC );
            metric = static_cast<cube::Metric*> (  metricItem->getCubeObject() );
            cnode  = static_cast<cube::Cnode*>( item->getCubeObject() );
            exists = statistics->existsMaxSeverity( metric, cnode, enter, exit );
        }
        if ( exists )
        {
            service->setGlobalValue( "Statistics::MaxSevereEventEnter", QVariant( enter ) );
            service->setGlobalValue( "Statistics::MaxSevereEventExit", exit );
        }
        action->setEnabled( exists );
    }
}

void
StatisticPlugin::onShowStatistics()
{
    QDialog* dialog = new QDialog();
    dialog->setAttribute( Qt::WA_DeleteOnClose );
    dialog->setWindowTitle( tr( "Statistics info" ) );
    BoxPlot* theBoxPlot = new BoxPlot( dialog );

    QList<TreeItem*> const& vec   = service->getSelections( METRIC );
    bool                    added = false;
    foreach( TreeItem * item, vec )
    {
        if ( statistics->existsStatistics( static_cast<cube::Metric*>( item->getCubeObject() ) ) )
        {
            theBoxPlot->Add( statistics->getInfo( static_cast<cube::Metric*>( item->getCubeObject() ) ) );
            if ( item == contextMenuItem )
            {
                added = true;
            }
        }
    }
    if ( !added )
    {
        theBoxPlot->Add( statistics->getInfo( static_cast<cube::Metric*>( contextMenuItem->getCubeObject() ) ) );
    }

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget( theBoxPlot );

    QPushButton* okButton = new QPushButton( tr( "Close" ), dialog );
    connect( okButton, SIGNAL( pressed() ), dialog, SLOT( accept() ) );
    layout->addWidget( okButton );

    dialog->setLayout( layout );

    dialog->setModal( false );
    dialog->show();
}

void
StatisticPlugin::onShowMaxSeverityText()
{
    TreeItem*     metricItem = service->getSelection( METRIC );
    cube::Metric* metric     = static_cast<cube::Metric*> (  metricItem->getCubeObject() );

    TreeItem* callItem = 0;

    if ( contextMenuTreeType == METRIC )
    {
        uint32_t cnodeID = statistics->findMaxSeverityId( metric );
        callItem = service->getCallTreeItem( cnodeID ); // find call item with given cnodeID
    }
    else if ( contextMenuTreeType == CALL )
    {
        callItem = contextMenuItem;
    }

    QString path;

    TreeItem* item = callItem;
    while ( item->getParent() )
    {
        QString sep  = item->getDepth() > 0 ? "+ " : "";
        QString line = QString( 2 * item->getDepth(), ' ' ) + sep + item->getLabel();
        path = ( item == callItem ) ? line : line + "\n" + path;
        item = item->getParent();
    }
    QString label = QString( tr( "metric: \n  " ) ) + metric->get_descr().c_str();
    label += QString( " [" ) + metric->get_uom().c_str() + "]";
    if ( contextMenuTreeType == METRIC )
    {
        label += tr( "\ncallpath of max severity: \n" );
    }
    else if ( contextMenuTreeType == CALL )
    {
        label += tr( "\nselected callpath: \n" );
    }
    label += path;

    cube::Cnode const* cnode = static_cast<cube::Cnode*>( callItem->getCubeObject() );
    statistics->showMaxSeverityText( 0, label, metric, cnode );
}


void
StatisticPlugin::orderHasChanged( const QList<cubepluginapi::DisplayType>& order )
{
    ( void )order;
    // todo: qDebug() << "StatisticPlugin: order changed";
}

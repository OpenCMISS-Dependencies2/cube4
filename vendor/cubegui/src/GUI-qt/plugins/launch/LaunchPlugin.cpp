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
#include <QAction>
#include "LaunchPlugin.h"
#include "CubeServices.h"

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( LaunchPlugin, LaunchPlugin );
#endif

using namespace cubepluginapi;

void
LaunchPlugin::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

QString
LaunchPlugin::getHelpText() const
{
    return "";
}

QString
LaunchPlugin::name() const
{
    return "Launch";
}

bool
LaunchPlugin::cubeOpened( PluginServices* service )
{
    this->service = service;
    marker        = service->getTreeItemMarker( "launch" );

    connect( service, SIGNAL( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ),
             this, SLOT( contextMenuIsShown( cubepluginapi::DisplayType, cubepluginapi::TreeItem* ) ) );

    LaunchInfo* launch = new LaunchInfo( service );
    if ( launch->readLaunchFile( service->getCubeBaseName() ) )
    {
        launchInfoList.append( launch );
        // todo: more launch elements in one file?
    }
    else
    {
        delete launch;
        return false;
    }

    /* create init menu items, if init items exist */
    if ( !launchInfoList.isEmpty() )
    {
        QList<LaunchInfo*> menuItems;
        for ( int i = 0; i < launchInfoList.size(); i++ )
        {
            LaunchInfo* launchInfo = launchInfoList.at( i );
            if ( !launchInfo->getLaunchInitMenu().isEmpty() )
            {
                menuItems.append( launchInfo );
            }
        }

        QMenu* menu = service->enablePluginMenu();
        for ( int i = 0; i < menuItems.size(); ++i )
        {
            QAction* action = menu->addAction( menuItems.at( i )->getLaunchInitMenu() );
            connect( action, SIGNAL( triggered() ), menuItems.at( i ), SLOT( launchInit() ) );
        }
    }

    // mark metric tree items
    foreach( TreeItem * metricItem, service->getTreeItems( METRIC ) )
    {
        for ( int i = 0; i < launchInfoList.size(); i++ )
        {
            LaunchInfo*     launchInfo = launchInfoList.at( i );
            QList<uint32_t> cnodes     = launchInfo->getCnodes();

            foreach( uint32_t cnodeId, cnodes )
            {
                TreeItem*           callItem = service->getCallTreeItem( cnodeId );
                const cube::Metric* metric   = dynamic_cast<cube::Metric*> ( metricItem->getCubeObject() );
                const cube::Cnode*  cnode    = dynamic_cast<cube::Cnode*> ( callItem->getCubeObject() );

                if ( launchInfo->existsLaunch( metric, cnode ) )
                {
                    service->addMarker( marker, metricItem, callItem, 0 );
                }
            }
        }
    }

    return true;
}

void
LaunchPlugin::cubeClosed()
{
    foreach( LaunchInfo * info, launchInfoList )
    {
        delete info;
    }
    launchInfoList.clear();

    contextHash.clear();
}


/* create context menu items */
void
LaunchPlugin::contextMenuIsShown( cubepluginapi::DisplayType type, cubepluginapi::TreeItem* item )
{
    if ( item == 0 )
    {
        return;
    }
    cube::Vertex*       vertex = item->getCubeObject();
    const cube::Metric* metric = 0;
    const cube::Cnode*  cnode  = 0;

    if ( type == METRIC )
    {
        metric = dynamic_cast<cube::Metric* >( vertex );
    }
    else if ( type == CALL )
    {
        TreeItem* metricItem = service->getSelection( METRIC );
        metric = dynamic_cast<cube::Metric*> ( metricItem->getCubeObject() );
        cnode  = dynamic_cast<cube::Cnode*>( vertex );
    }
    else
    {
        return;
    }

    foreach( LaunchInfo * launchInfo, launchInfoList )
    {
        QStringList launchMenuEntries = launchInfo->getMenuEntries( metric, cnode );
        for ( int i = 0; i < launchMenuEntries.size(); i++ )
        {
            QAction* launchAct = service->addContextMenuItem( type, launchMenuEntries.at( i ) );

            QPair<DisplayType, TreeItem*> pair = QPair<cubepluginapi::DisplayType, cubepluginapi::TreeItem*> ( type, item );
            contextHash.insert( ( unsigned long long )launchAct, pair );

            connect( launchAct, SIGNAL( triggered() ), this, SLOT( onLaunch() ) );

            if ( !launchInfo->isInitialized() )
            {
                launchAct->setEnabled( false );
            }
        }
    }
}

void
LaunchPlugin::onLaunch()
{
    QAction* currentMenuAction = ( QAction* )QObject::sender();
    if ( currentMenuAction == 0 )
    {
        return;
    }

    QPair<DisplayType, TreeItem*> pair =  contextHash.value( ( unsigned long long )currentMenuAction );

    DisplayType type = pair.first;
    TreeItem*   item = pair.second;
    assert( type == METRIC || type == CALL );

    cube::Vertex* vertex = item->getCubeObject();

    QString label = currentMenuAction->text();

    for ( int i = 0; i < launchInfoList.size(); i++ )
    {
        LaunchInfo* launchInfo = launchInfoList.at( i );

        QString command;
        if ( type == METRIC )
        {
            TreeItem* metricItem = item;

            cube::Metric* metric = dynamic_cast<cube::Metric* >( vertex );
            command = launchInfo->findLaunchCommand( label, metric );

            if ( !command.isEmpty() )
            {
                launchInfo->launch( command, metricItem );
            }
        }
        else if ( type == CALL )
        {
            TreeItem* callItem   = item;
            TreeItem* metricItem = service->getSelection( METRIC );

            cube::Metric*      metric = dynamic_cast<cube::Metric*> ( metricItem->getCubeObject() );
            const cube::Cnode* cnode  = dynamic_cast<cube::Cnode*> ( vertex );

            command = launchInfo->findLaunchCommand( label, metric, cnode );

            if ( !command.isEmpty() )
            {
                launchInfo->launch( command, metricItem, callItem );
            }
        }
    }
}

void
LaunchPlugin::orderHasChanged( const QList<cubepluginapi::DisplayType>& order )
{
    ( void )order;
}

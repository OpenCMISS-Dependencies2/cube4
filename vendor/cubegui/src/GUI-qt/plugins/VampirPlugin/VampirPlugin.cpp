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
#include <QThread>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <cassert>
#include <QDebug>
#include <iostream>

#include "CubeServices.h"
#include "VampirPlugin.h"
#include "VampirConnecter.h"
#include "VampirConnectionDialog.h"
#include "TreeItemMarker.h"

using namespace std;
using namespace cubepluginapi;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( VampirPlugin, VampirPlugin );
#endif



QString
VampirPlugin::name() const
{
    return "Vampir";
}

void
VampirPlugin::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

bool
VampirPlugin::cubeOpened( PluginServices* service )
{
    this->service   = service;
    contextMenuItem = 0;
    QUrl url( service->getStatName() );

    if ( !QFile( url.toLocalFile() ).exists() )
    {
        errorMsg = url.toLocalFile() + tr( " cannot be opened." ).toUtf8().data();
        return false; // statistics file doesn't exist
    }

    connect( service, SIGNAL( contextMenuIsShown( cubepluginapi::TreeItem* ) ),
             this, SLOT( contextMenuIsShown( cubepluginapi::TreeItem* ) ) );
    connect( service, SIGNAL( globalValueChanged( QString ) ),
             this, SLOT( globalValueChanged( QString ) ) );

    QMenu*   menu       = service->enablePluginMenu();
    QAction* menuAction = menu->addAction( tr( "Connect to &vampir" ).toUtf8().data() );
    menuAction->setStatusTip( tr( "Connect to vampir and display a trace file" ).toUtf8().data() );
    menuAction->setWhatsThis( tr( "Connect to vampir and display a trace file" ).toUtf8().data() );
    connect( menuAction, SIGNAL( triggered() ), this, SLOT( vampirSettings() ) );
    return true;
}

QString
VampirPlugin::getDeactivationMessage()
{
    return errorMsg;
}

void
VampirPlugin::cubeClosed()
{
}

QString
VampirPlugin::getHelpText() const
{
    return "";
}

#define STATISTIC_MARKER "max severe instance"
void
VampirPlugin::contextMenuIsShown( DisplayType type, TreeItem* item )
{
    if ( item == NULL )
    {
        return;
    }
    contextMenuItem     = item;
    contextMenuTreeType = type;
    bool isActive = false;

    foreach( const TreeItemMarker * marker, item->getMarkerList() )
    {
        // only enable vampir action, if the statistics plugin has marked the selected tree item
        if ( marker->getLabel() == QString( STATISTIC_MARKER ) )
        {
            isActive = true;
            break;
        }
    }
    QAction* action = service->addContextMenuItem( type, tr( "Show max severity in Vampir" ).toUtf8().data() );
    action->setStatusTip( tr( "Shows the most severe instance of pattern in Vampir" ).toUtf8().data() );
    action->setWhatsThis( tr( "Shows the most severe instance of pattern in Vampir. Only available if a statistics file for the current cube file exists." ).toUtf8().data() );
    connect( action, SIGNAL( triggered() ), this, SLOT( onShowMaxSeverity() ) );
    action->setEnabled( isActive );
}

void
VampirPlugin::onShowMaxSeverity()
{
    QString      message          = "";
    const int    zoomSteps        = 3;
    const double offsetMultiplier = 0.1;
    double       duration         = exit - enter;
    double       exitTime         = exit + offsetMultiplier * duration;

    bool connected = false;

    for ( int step = 1; step <= zoomSteps; step++ )
    {
        double enterTime = max( enter
                                - ( ( zoomSteps - step ) * 5 ) * duration
                                - offsetMultiplier * duration, 0.0 );
        foreach( VampirConnecter * con, connecterList )
        {
            if ( con->IsActive() )
            {
                message  += QString::fromStdString( con->ZoomIntervall( enterTime, exitTime, step ) );
                connected = true;
            }
        }
    }

    if ( !connected )
    {
        message = tr( "Not connected to Vampir." ).toUtf8().data();
    }

    if ( !message.isEmpty() )
    {
        service->setMessage( message, Error );
    }
}

void
VampirPlugin::vampirSettings()
{
    VampirConnectionDialog* dialog;
    dialog = new VampirConnectionDialog( service, service->getParentWidget(), service->getCubeFileName(), &connecterList );

    dialog->setModal( false );
    dialog->show();
}

/**
 * get values from statistics plugin
 */
void
VampirPlugin::globalValueChanged( const QString& name )
{
    if ( name == "Statistics::MaxSevereEventEnter" )
    {
        enter = service->getGlobalValue( name ).toDouble();
    }
    else if ( name == "Statistics::MaxSevereEventExit" )
    {
        exit = service->getGlobalValue( name ).toDouble();
    }
}

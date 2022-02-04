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

#include <QAction>
#include <QDebug>
#include <QApplication>
#include <QToolButton>
#include <QWidgetAction>
#include <QMenu>

#include "SynchronizationToolBar.h"
#include "Globals.h"
#include "PluginManager.h"
#include "SettingsHandler.h"
#include "Globals.h"

#define TREE_TYPES 4
using namespace cubegui;

SynchronizationToolBar::SynchronizationToolBar() : QToolBar( tr( "Synchronization Toolbar" ) )
{
    timer    = 0;
    autoSend = false;
    sync     = new Synchronization();

    menu = new ::QMenu();
    QToolButton* toolButton = new QToolButton();
    toolButton->setText( tr( "Synchronize state of ..." ) );
    toolButton->setMenu( menu );
    toolButton->setPopupMode( QToolButton::InstantPopup );
    QWidgetAction* toolButtonAction = new QWidgetAction( this );
    toolButtonAction->setDefaultWidget( toolButton );
    this->addAction( toolButtonAction );

    QAction* sendAlways = this->addAction( QIcon( ":/images/send_state.png" ), tr( "Send state" ) );
    sendAlways->setCheckable( true );
    this->addAction( sendAlways );
    connect( sendAlways, SIGNAL( triggered( bool ) ), this, SLOT( setAutoSend( bool ) ) );


    QAction* readAlways = this->addAction( QIcon( ":/images/recv_state.png" ), tr( "Receive state" ) );
    readAlways->setCheckable( true );
    this->addAction( readAlways );
    connect( readAlways, SIGNAL( triggered( bool ) ), this, SLOT( setAutoReceive( bool ) ) );
}

SynchronizationToolBar::~SynchronizationToolBar()
{
    QApplication::clipboard()->clear();
}

void
SynchronizationToolBar::setSettingsHandlerList( const QList<SettingsHandler*>& list )
{
    settingsHandlerList = list;
    updateComponents();
}

void
SynchronizationToolBar::clearSettingsHandler()
{
    QApplication::clipboard()->clear();
    settingsHandlerList.clear();
    sync->setSettingsHandlerList( settingsHandlerList );
}

void
SynchronizationToolBar::updateComponents()
{
    menu->clear();
    actionList.clear();
    int idx = 0;
    foreach( SettingsHandler * handler, settingsHandlerList )
    {
        QAction* action = new QAction( handler->settingName(), this );
        action->setCheckable( true );
        action->setChecked( handler->synchronizationIsDefault() );
        action->setData( QVariant( idx++ ) );
        menu->addAction( action );
        actionList.append( action );
    }
}

QList<SettingsHandler*>
SynchronizationToolBar::getEnabledHandlerList()
{
    QList<SettingsHandler*> enabledHandler;
    for ( int idx = 0; idx < settingsHandlerList.size(); idx++ )
    {
        if ( actionList.at( idx )->isChecked() )
        {
            enabledHandler.append( settingsHandlerList.at( idx ) );
        }
    }
    return enabledHandler;
}

void
SynchronizationToolBar::send()
{
    if ( autoSend )
    {
        sync->setSettingsHandlerList( getEnabledHandlerList() );
        sync->sendSettingsToClipboard();
    }
}

void
SynchronizationToolBar::receive()
{
    sync->setSettingsHandlerList( getEnabledHandlerList() );
    sync->receiveSettingsFromClipboard();
}

void
SynchronizationToolBar::setAutoReceive( bool isAutoReceive )
{
    if ( timer )
    {
        delete timer;
    }
    timer = 0;

    if ( isAutoReceive )
    {
        receive();

        timer = new QTimer( this );
        connect( timer, SIGNAL( timeout() ), this, SLOT( receive() ) );
        timer->start( 500 );
    }
}

void
SynchronizationToolBar::setAutoSend( bool send )
{
    autoSend = send;
    this->send();
}

// =================================================================================================================================

Synchronization::Synchronization()
{
    sendingIsEnabled = false;
    timestamp        = 0;
}

void
Synchronization::sendSettingsToClipboard()
{
    QString filename;

    QTemporaryFile tmpFile; // temporary file to write settings to clipboard
    tmpFile.open();         // required to get filename
    filename = tmpFile.fileName();

    {   // force settings object to be destructed to ensure that the ini file is flushed before the following read
        QSettings settings( filename, QSettings::IniFormat );

        QTime time( 0, 0, 0, 0 );
        timestamp = time.msecsTo( QTime::currentTime() );
        settings.setValue( "timestamp", timestamp );

        foreach( SettingsHandler * handler, settingsHandlerList )
        {
            settings.beginGroup( handler->settingName() );
            handler->saveStatus( settings );
            settings.endGroup();
        }
    }

    QFile file( filename );
    file.open( QIODevice::ReadOnly );

    QTextStream in( &file );
    QString     line = "";
    while ( !in.atEnd() )
    {
        line += in.readLine() + "\n";
    }
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->setText( line );
}

void
Synchronization::receiveSettingsFromClipboard()
{
    QClipboard* clipboard = QApplication::clipboard();
    QString     text      = clipboard->text();

    QTemporaryFile tmpFile; // temporary file to write settings to clipboard
    tmpFile.open();         // required to get filename
    QTextStream out( &tmpFile );
    out << text << Qt::endl;
    tmpFile.close();

    QString   filename = tmpFile.fileName();
    QSettings settings( filename, QSettings::IniFormat );

    int stamp = settings.value( "timestamp", 0 ).toInt();
    if ( stamp == timestamp )
    {
        return; // values haven't been changed
    }
    timestamp = stamp;
    QApplication::setOverrideCursor( Qt::BusyCursor );

    foreach( SettingsHandler * handler, settingsHandlerList )
    {
        settings.beginGroup( handler->settingName() );
        handler->loadStatus( settings );
        settings.endGroup();
    }
    QApplication::restoreOverrideCursor();
}

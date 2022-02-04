/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2019                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#ifndef SYNCTOOLBAR_H
#define SYNCTOOLBAR_H

#include <QWidget>
#include <QToolBar>
#include <QList>
#include <QMenu>

namespace cubegui
{
class SettingsHandler;
class Synchronization;

/**
 * SynchronizationToolBar generates a toolbar which allows the user to synchronize the state (selections...)
 * of several cube instances via the clipboard
 */
class SynchronizationToolBar : public QToolBar
{
    Q_OBJECT
public:
    SynchronizationToolBar();
    ~SynchronizationToolBar();
    void
    setSettingsHandlerList( const QList<SettingsHandler*>& list );
    void
    clearSettingsHandler();

public slots:
    /** calls saveStatus() on all registered settings handler and sends the result to clipboard */
    void
    send();

    /** writes the contents of the clipboard to QSettings and calls loadStatus() on all registered settings handler */
    void
    receive();

private slots:
    void
    setAutoSend( bool );
    void
    setAutoReceive( bool );
    void
    updateComponents();

private:
    QTimer*                 timer;
    QMenu*                  menu;
    QList<QAction*>         actionList;
    bool                    autoSend;
    Synchronization*        sync;
    QList<SettingsHandler*> settingsHandlerList;  // registered SettingsHandler

    QList<SettingsHandler*>
    getEnabledHandlerList();
};

class Synchronization
{
public:
    Synchronization();
    void
    receiveSettingsFromClipboard();
    void
    sendSettingsToClipboard();

    void
    setSettingsHandlerList( const QList<SettingsHandler*>& list )
    {
        settingsHandlerList = list;
    }

private:
    QList<SettingsHandler*> settingsHandlerList;  // registered SettingsHandler
    bool                    sendingIsEnabled;     //
    int                     timestamp;            // timestamp to check if clipboard content has changed
};
}
#endif // SYNCTOOLBAR_H

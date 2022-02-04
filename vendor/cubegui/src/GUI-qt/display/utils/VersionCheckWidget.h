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




#ifndef VERSION_CHECK_H
#define VERSION_CHECK_H

#include <QWidget>
#include <QPainter>
#include <QString>
#include <QToolButton>
#include <QDialog>
#include <QGroupBox>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <utility>
#include <vector>
#include <string>

#include <QUrl>
#include <QBuffer>

#include "Settings.h"

namespace cubegui
{
// #define UPDATE_CHECK_URL "http://www.vi-hps.org/upload/packages/cube/CUBE_LATEST_RELEASE"
#define UPDATE_CHECK_URL "http://apps.fz-juelich.de/scalasca/releases/cube/CUBE_LATEST_RELEASE"
// #define UPDATE_URL "http://www.vi-hps.org/projects/score-p/"

// this url is not 100% valid, coz i dont know yet precise url for the page, where CUBE get published
#define UPDATE_URL "http://www.scalasca.org/software/cube-4.x/cube.html"

enum UpdateMethod { STARTUP, NEVER, ASK, NOT_NOW };

class DownloadThread;
class VersionSettingsDialog;

class VersionCheckWidget : public QToolButton, InternalSettingsHandler
{
    Q_OBJECT
public:
    VersionCheckWidget( QWidget* parent = 0 );

public slots:
    void
    checkForUpdates();

    void
    notNow();


private:
    virtual void
    mousePressEvent( QMouseEvent* event );

    QIcon recentVersion;
    QIcon updateAvailable;
    QIcon noCheckPossible;
    QIcon permissionForUpdate;


    QIcon shownIcon;

    QUrl                   url;
    DownloadThread*        download;
    QString                updateDescription;
    QString                statusMessage;
    bool                   update_Available;
    bool                   no_http;
    UpdateMethod           updateMethod;
    VersionSettingsDialog* dialog;
private slots:

    void
    showUpdateDialog();


    void
    delayedMessages();

    void
    prepareForCheck();



    void
    check( const QUrl&       url,
           const QByteArray& buffer );

    // SettingsHandler interface
    virtual QString
    settingName()
    {
        return "version";
    }
    // InternalSettingsHandler interface
    virtual void
    loadGlobalStartupSettings( QSettings& );
    virtual void
    saveGlobalStartupSettings( QSettings& );
};


class VersionSettingsDialog : public QWidget
{
    Q_OBJECT

signals:
    void
    accepted();

    void
    rejected();

private slots:

    void
    accept();

    void
    reject();


public:
    explicit
    VersionSettingsDialog( QWidget*,
                           UpdateMethod updateMethod,
                           QString      updateDescription );

    void
    alignSelf( const QWidget* );

    UpdateMethod
    usersDecision() const;

private:
    QRadioButton* startup;
    QRadioButton* never;
    QGroupBox*    groupBox;
    UpdateMethod  updateMethod;
};
}
#endif

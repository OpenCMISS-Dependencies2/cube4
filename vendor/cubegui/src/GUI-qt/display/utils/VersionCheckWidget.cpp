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


#include "VersionCheckWidget.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

#include <QFontMetrics>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QCheckBox>
#include <QDialog>
#include <QString>
#include <QIcon>
#include <QDebug>
#include <math.h>
#include <QTimer>
#include <QToolTip>
#include <QDesktopServices>
#include <QLabel>
#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

#include "HelpBrowser.h"
#include "Constants.h"
#include "Globals.h"
#include "Environment.h"
#include "DownloadThread.h"

using namespace std;
using namespace cubegui;


VersionCheckWidget::VersionCheckWidget( QWidget* parent )
    : QToolButton( parent )
{
    recentVersion       = QIcon(  ":images/recentVersion.png" );
    updateAvailable     = QIcon(  ":images/updateAvailable.png" );
    noCheckPossible     = QIcon(  ":images/noCheckPossible.png" );
    permissionForUpdate = QIcon(  ":images/permissionForUpdate.png" );

    shownIcon = permissionForUpdate;
    setIcon( shownIcon );
    dialog = NULL;

    url              = QUrl( UPDATE_CHECK_URL );
    download         = NULL;
    update_Available = false;
    no_http          = env_str2bool( getenv( "CUBE_DISABLE_HTTP_DOCS" ) );
    QTimer::singleShot( 5000, this, SLOT( delayedMessages() ) );
    Globals::getSettings()->registerSettingsHandler( this );
}

void
VersionCheckWidget::delayedMessages()
{
    if ( updateMethod == NEVER )
    {
        statusMessage     = tr( "Check for new version is disabled." );
        updateDescription =   statusMessage;
        Globals::setStatusMessage( statusMessage );
        setIcon( noCheckPossible );
        return;
    }
    if ( updateMethod == ASK )
    {
        showUpdateDialog();
    }
    else // starturo
    {
        if ( !no_http )
        {
            if ( updateMethod != NOT_NOW )
            {
                prepareForCheck();
            }
        }
        else
        {
            statusMessage     = tr( "Check for new version is disabled via environment variable CUBE_DISABLE_HTTP_DOCS=\"" ) + QString( getenv( "CUBE_DISABLE_HTTP_DOCS" ) ) + "\"";
            updateDescription =   statusMessage;
            Globals::setStatusMessage( statusMessage );
            setIcon( noCheckPossible );
        }
    }
}


void
VersionCheckWidget::prepareForCheck()
{
    if ( !no_http )
    {
        updateDescription =   tr( "Check for update is not performed yet." );
        Globals::setStatusMessage( tr( "Check for new version..." ) );
        download = new DownloadThread( url );
        connect( download, SIGNAL( downloadFinished( QUrl, QByteArray ) ),
                 this, SLOT( check( QUrl, QByteArray ) ) );
        QTimer::singleShot( 10000, download, SLOT( loadList() ) );
    }
}


void
VersionCheckWidget::showUpdateDialog()
{
    if ( dialog == NULL )
    {
        dialog = new VersionSettingsDialog( NULL,
                                            updateMethod,
                                            updateDescription );
        connect( dialog, SIGNAL( rejected() ), this, SLOT( notNow() ) );
        connect( dialog, SIGNAL( accepted() ), this, SLOT( checkForUpdates() ) );
        dialog->resize( dialog->minimumSizeHint() );
    }
    dialog->alignSelf( Globals::getMainWindow() );
    dialog->show();
    dialog->raise();
    dialog->activateWindow();
}




void
VersionCheckWidget::mousePressEvent( QMouseEvent* event )
{
    QToolTip::showText( mapToGlobal( event->pos() ), updateDescription );
    if ( !no_http )
    {
        if ( ( update_Available ) && event->button() == Qt::LeftButton )
        {
            QDesktopServices::openUrl( QUrl( UPDATE_URL, QUrl::StrictMode ) );
        }
        else if ( event->button() == Qt::LeftButton )
        {
            showUpdateDialog();
        }
    }
}

void
VersionCheckWidget::checkForUpdates()
{
    updateMethod = dialog->usersDecision();
    dialog->close();

    if ( updateMethod != NOT_NOW && updateMethod != NEVER )
    {
        prepareForCheck();
    }
}

void
VersionCheckWidget::notNow()
{
    updateMethod = NOT_NOW;
    dialog->close();
}


void
VersionCheckWidget::loadGlobalStartupSettings( QSettings& settings )
{
    QString val = settings.value( "updateMethod", "ASK" ).toString();
    if ( val == "STARTUP" )
    {
        updateMethod = STARTUP;
    }
    else if ( val == "NEVER" )
    {
        updateMethod = NEVER;
    }
    else
    {
        updateMethod = ASK;
    }
}

void
VersionCheckWidget::saveGlobalStartupSettings( QSettings& settings )
{
    QString val = "ASK";
    if ( updateMethod == STARTUP )
    {
        val = "STARTUP";
    }
    else if ( updateMethod == NEVER )
    {
        val = "NEVER";
    }
    else
    {
        val = "ASK";
    }
    settings.setValue( "updateMethod", val );
}

#if QT_VERSION >= QT_VERSION_CHECK( 5, 0, 0 )
#define REGEXP( a ) QRegularExpression( a )
#else
#define REGEXP( a ) QRegExp( a )
#endif

void
VersionCheckWidget::check( const QUrl&, const QByteArray& buffer )
{
    updateDescription =   tr( "Check for latest CUBE version failed." );
    if ( !buffer.isNull() )
    {
        QString pattern( tr( CUBEGUI_VERSION ) );

        QStringList currentVersionParts = pattern.split( REGEXP( "[\\.-]" ) );
        QString     currentMajor        = currentVersionParts.at( 0 ).trimmed().toLower();
        QString     currentMidle        = currentVersionParts.at( 1 ).trimmed().toLower();
        QString     currentMinor        = ( currentVersionParts.size() >= 3 ) ? currentVersionParts.at( 2 ).trimmed().toLower() : "0";


        QString content( buffer );

        QStringList list = content.split( "\n" );

        QStringListIterator javaStyleIterator( list );
        while ( javaStyleIterator.hasNext() )
        {
            QString     line = javaStyleIterator.next();
            QStringList pair = line.split( REGEXP( "\\s*:\\s*" ) );
            if ( pair.size() < 2 ) // ignore malformed strings
            {
                continue;
            }
            QString field = pair.at( 0 ).trimmed().toLower();
            QString value = pair.at( 1 ).trimmed().toLower();

            if ( field == "version" )
            {
                QStringList versionParts = value.split( REGEXP( "\\." ) );
                QString     major        = ( versionParts.size() > 0 ) ? versionParts.at( 0 ).trimmed().toLower() : "";
                QString     midle        = ( versionParts.size() > 1 ) ? versionParts.at( 1 ).trimmed().toLower() : "";
                QString     minor        = ( versionParts.size() > 2 ) ? versionParts.at( 2 ).trimmed().toLower() : "0";



                bool ok     = true;
                int  _major = major.toInt( &ok );
                if ( !ok )
                {
                    return;
                }
                int _midle = midle.toInt( &ok );
                if ( !ok )
                {
                    return;
                }
                int _minor = minor.toInt( &ok );
                if ( !ok )
                {
                    _minor = 0;
                }
                int _currentMajor = currentMajor.toInt( &ok );
                if ( !ok )
                {
                    return;
                }
                int _currentMidle = currentMidle.toInt( &ok );
                if ( !ok )
                {
                    return;
                }
                int _currentMinor = currentMinor.toInt( &ok );
                if ( !ok )
                {
                    _currentMinor = 0;
                }

                if ( ( _major > _currentMajor ) ||
                     (
                         ( _major == _currentMajor ) &&
                         ( _midle > _currentMidle )
                     ) ||
                     (
                         ( _major == _currentMajor ) &&
                         ( _midle == _currentMidle ) &&
                         ( _minor > _currentMinor )
                     )
                     )
                {
                    shownIcon = updateAvailable;
                    setIcon( shownIcon );
                    updateDescription =  content;
                    update_Available  = true;
                    if (    ( _major > _currentMajor ) ||
                            (
                                ( _major == _currentMajor ) &&
                                ( _midle > _currentMidle )
                            )
                            )
                    {
                        statusMessage = tr( "New major version of CUBE is available. Please visit <a href=\"http://www.scalasca.org/software/cube-4.x/download.html\"><nobr>http://www.scalasca.org/software/cube-4.x/download.html</nobr> </a> for download." );
                        Globals::setStatusMessage( statusMessage );
                    }
                }
                else
                {
                    shownIcon = recentVersion;
                    setIcon( shownIcon );
                    updateDescription =   tr( "You have the latest version of CUBE. No update is needed." );
                    Globals::setStatusMessage( updateDescription );
                }
            }
        }
    }
    else
    {
        updateDescription =  tr( "Cannot download information about the latest CUBE release. " );
        Globals::setStatusMessage( updateDescription, Warning );
        setIcon( noCheckPossible );
    }
    download->deleteLater();
}

VersionSettingsDialog::VersionSettingsDialog( QWidget*     parent,
                                              UpdateMethod updateMethod,
                                              QString      updateDescription ) : QWidget( parent,
                                                                                          Qt::SubWindow |
                                                                                          Qt::WindowStaysOnTopHint |
                                                                                          Qt::FramelessWindowHint )
{
    setWindowTitle( tr( "Cube Update" ) );
    setAttribute( Qt::WA_QuitOnClose, false );
    this->updateMethod = updateMethod;
    QDialogButtonBox* buttonBox = new QDialogButtonBox( this );
    buttonBox->addButton( QDialogButtonBox::Ok );
    buttonBox->addButton( QDialogButtonBox::Cancel );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( reject() ) );
    connect( buttonBox, SIGNAL( accepted() ), this, SLOT( accept() ) );

    QVBoxLayout* vbox = new QVBoxLayout;
    groupBox = new QGroupBox( tr( "Check for latest CUBE version" ), this );

    startup = new QRadioButton( tr( "ALWAYS check for updates" ) );
    vbox->addWidget( startup );
    startup->setChecked( updateMethod == STARTUP );
    never = new QRadioButton( tr( "never check for updates" ) );
    vbox->addWidget( never );
    never->setChecked( updateMethod == NEVER );

    vbox->addStretch( 1 );
    groupBox->setLayout( vbox );

    QString privacy = tr( "Show <a href=\"http://www.scalasca.org/imprint/imprint.html?a=108&level=0\"> data privacy information</a>" );
    QLabel* label   = new QLabel( privacy );
    connect( label, SIGNAL( linkActivated( QString ) ), HelpBrowser::getInstance(), SLOT( showUrl( QString ) ) );

    QVBoxLayout* layout = new QVBoxLayout();
    if ( !updateDescription.isEmpty() )
    {
        layout->addWidget( new QLabel( updateDescription ) );
    }
    layout->addWidget( groupBox );
    layout->addWidget( label );
    layout->addSpacing( 5 );
    layout->addWidget( buttonBox );
    setLayout( layout );
    setFocusPolicy( Qt::StrongFocus );
    resize( minimumSize() );
}


void
VersionSettingsDialog::alignSelf( const QWidget* _widget )
{
    double ph = _widget->geometry().height();
    double pw = _widget->geometry().width();
    double px = _widget->geometry().x();
    double py = _widget->geometry().y();
    double dw = width();
    double dh = height();
    setGeometry( px + pw - dw, py + ph - dh, dw, dh );
}

void
VersionSettingsDialog::accept()
{
    emit accepted();
}

void
VersionSettingsDialog::reject()
{
    emit rejected();
}


UpdateMethod
VersionSettingsDialog::usersDecision() const
{
    if ( startup->isChecked() )
    {
        return STARTUP;
    }
    if ( never->isChecked() )
    {
        return NEVER;
    }
    return ASK;
}

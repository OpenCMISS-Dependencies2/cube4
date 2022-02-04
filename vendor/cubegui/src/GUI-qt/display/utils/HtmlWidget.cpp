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
#include <QNetworkReply>
#include "HtmlWidget.h"
#include "Environment.h"
#include "PluginManager.h"
#include "CubeProxy.h"

#ifdef WITH_WEB_ENGINE
#include "HtmlWidgetWebEngine.h"
#else
#include "HtmlWidgetTextView.h"
#endif

using namespace std;
using namespace cubegui;

HtmlWidget::HtmlWidget()
{
    lastUrl = "";
    connect( &check, SIGNAL( mirrorFound( QUrl ) ), this, SLOT( loadURL( QUrl ) ) );
}


HtmlWidget*
HtmlWidget::createHtmlWidget()
{
#ifdef WITH_WEB_ENGINE
    return new HtmlWidgetWebEngine;
#else
    return new HtmlWidgetTextView;
#endif
}

static QString
baseUrl( const QString& url )
{
    QString baseUrl = url;
    int     idx     = url.lastIndexOf( '#' );
    if ( idx > 0 )
    {
        baseUrl = url.left( idx );
    }
    return baseUrl;
}

void
HtmlWidget::loadURL( const QUrl& url )
{
    if ( url.isValid() )
    {
        QString urlString = url.toString();
        foreach( QString mirror, mirrorList )
        {
            if ( urlString.contains( mirror ) )
            {
                QString mirrorUrl = baseUrl( urlString );
                mirrorUrl.replace( mirror, "@mirror@" );
                lastMirrors[ mirrorUrl ] = mirror;
                break;
            }
        }
        showUrl( url );
        emit urlLoaded( url );
    }
    else
    {
        emit loadingFailed( url );
        Globals::setStatusMessage( tr( "Could not load URL: " ) + lastUrl, Information );
        lastUrl.clear(); // if failed, don't cache contents
    }
}

/** Loads the given url. If the string contains the tag "@mirror@", the tag is replaced with each defined mirror until
 * a vaild URL is found
 */
void
HtmlWidget::showUrl( const QString& url )
{
    if ( lastUrl == url )
    {
        emit urlLoaded( url );
        return;
    }
    lastUrl = url;

    if ( url.contains( "@mirror" ) )
    {
        QString mUrl   = url;
        QString mirror = lastMirrors.value( baseUrl( mUrl ) );
        if ( !mirror.isEmpty() )
        {
            mUrl.replace( QString( "@mirror@" ), mirror );
            Globals::debug( "HtmlWidget" ) << "used previous mirror: " << mUrl << Qt::endl;
            showUrl( QUrl( mUrl ) );
            emit urlLoaded( QUrl( mUrl ) );
            return;
        }

        // Check whether HTTP access to online documentation should be disabled
        bool no_http = env_str2bool( getenv( "CUBE_DISABLE_HTTP_DOCS" ) );

        cube::CubeProxy*                    cube    = PluginManager::getInstance()->getCube();
        const std::vector<std::string>&     mirrors = cube->getMirrors();
        vector<std::string>::const_iterator it      = mirrors.begin();

        QList<QUrl> urlList;
        mirrorList.clear();
        while ( it != mirrors.end() )
        {
            QString mirror = QString::fromStdString( *it );
#ifdef Q_OS_WIN
            if ( !mirror.startsWith( "http://" ) && !mirror.startsWith( "https://" ) ) // ensure valid syntax for local files
            {
                mirror.remove( "file://" );
                mirror = QUrl::fromLocalFile( mirror ).toString();
            }
#endif
            QString tmpUrl = url;
            tmpUrl.replace( QString( "@mirror@" ), mirror );
            if ( no_http && ( tmpUrl.startsWith( "http://" ) || tmpUrl.startsWith( "https://" ) ) )
            {
                ++it;
                continue;
            }
            if ( tmpUrl.trimmed().length() > 0 )
            {
                urlList.append( QUrl( tmpUrl ) );
                mirrorList.append( QString::fromStdString( *it ) );
            }
            ++it;
        }
        if ( urlList.size() == 0 )
        {
            Globals::setStatusMessage( tr( "no documentation mirrors found" ), MessageType::Error );
        }
        else
        {
            check.checkMirrors( urlList ); // -> loadURL
        }
    }
    else
    {
        QList<QUrl> urlList;
        urlList.append( QUrl( url ) );
        check.checkMirrors( urlList ); // -> loadURL
    }
}

// =================================================================================================

MirrorCheck::MirrorCheck()
{
    connect( &manager, SIGNAL( finished( QNetworkReply* ) ), this, SLOT( onFinished( QNetworkReply* ) ) );
}

void
MirrorCheck::checkMirrors( const QList<QUrl>& urlList )
{
    mirrorList = urlList;
    foreach( QUrl url, urlList )
    {
        // url differs from previous url only in anchor
        if ( lastValidUrl.isValid() && url.toString( QUrl::RemoveFragment ) == lastValidUrl.toString( QUrl::RemoveFragment ) )
        {
            Globals::debug( "HtmlWidget" ) << "same base URL: " << url.toString() << Qt::endl;
            emit mirrorFound( url );
            return;
        }
    }
    checkMirrors();
}

void
MirrorCheck::checkMirrors()
{
    if ( mirrorList.size() == 0 )
    {
        return;
    }
    url = mirrorList.takeFirst();
    if ( !url.isValid() )
    {
        onFinished( NULL );
    }

    // Load data depending on the specified protocol
    QString protocol = url.scheme();
    if ( "http" == protocol || "https" == protocol )
    {
        manager.head( QNetworkRequest( url ) );
    }
    else
    {
        QFile file( url.toLocalFile() );
        if ( QFileInfo( file ).isReadable() )
        {
            emit mirrorFound( url );
        }
        else
        {
            onFinished( NULL );
        }
    }
}

void
MirrorCheck::onFinished( QNetworkReply* reply )
{
    if ( reply && !reply->error() && reply->attribute( QNetworkRequest::HttpStatusCodeAttribute ).toInt() < 300 )
    {
        Globals::debug( "HtmlWidget" ) << url.toString() << " -> ok" << Qt::endl;
        lastValidUrl = url;
        lastValidUrl.setFragment( "" );
        emit mirrorFound( url );
    }
    else
    {
        if ( !mirrorList.empty() )
        {
            Globals::debug( "HtmlWidget" ) << url.toString() << " -> error" << Qt::endl;
            checkMirrors();
        }
        else
        {
            Globals::debug( "HtmlWidget" ) << url.toString() << " -> error (last mirror)" << Qt::endl;
            emit mirrorFound( QUrl() );
        }
    }
}

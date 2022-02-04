/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"

#include <iostream>
#include <QDebug>
#include <QBuffer>
#include <QFile>
#include <QNetworkRequest>

#include "DownloadThread.h"

using namespace cubegui;


/*--- Constructors & destructor -------------------------------------------*/

/**
 * @brief Constructs an object for downloading data
 *
 * Constructs a new object for downloading the data referenced by
 * the given @p url
 *
 * @param  url     URL of the data to be downloaded
 */
DownloadThread::DownloadThread( const QUrl& url )
{
    urlList.clear();
    urlList.append( url );
}

/**
 * @brief Constructs an object for downloading data
 *
 * Constructs a new object for downloading the data referenced by
 * the given list of urls @p list. It starts loading the first
 * URL of the list and takes the next one if loading fails.
 *
 * @param  list list of mirrors
 */
DownloadThread::DownloadThread( const QList<QUrl>& list ) : urlList( list )
{
}

void
DownloadThread::loadList()
{
    if ( urlList.size() == 0 )
    {
        return;
    }

    url = urlList.takeFirst();

    // Sanity check
    if ( !url.isValid() )
    {
        currentDownloadFinished();
        return;
    }

    // Load data depending on the specified protocol
    QString protocol = url.scheme();
    if ( "http" == protocol || "https" == protocol )
    {
        connect( &manager, SIGNAL( finished( QNetworkReply* ) ), this, SLOT( currentDownloadFinished( QNetworkReply* ) ),
                 Qt::DirectConnection );
        manager.get( QNetworkRequest( url ) );
    }
    else if ( "file" == protocol )
    {
        // Open file for reading
        QFile file( url.toLocalFile() );
        if ( !file.open( QIODevice::ReadOnly ) )
        {
            currentDownloadFinished();
            return;
        }
        // read data with file.readAll()
        emit downloadFinished( url, file.readAll() );
    }
    else
    {
        currentDownloadFinished();
    }
}

/**
 * @brief Called when a HTTP download is finished.
 *
 * This method is called whenever a HTTP download finishes, either due to
 * an error or after all data has been downloaded.
 */
void
DownloadThread::currentDownloadFinished( QNetworkReply* reply )
{
//      if (reply) qDebug() << "finishedDownload" << reply->error() << url;

    if ( reply && !reply->error() )
    {
        emit downloadFinished( reply->url(), reply->readAll() );
    }
    else if ( !urlList.isEmpty() )
    {
        loadList(); // try next mirror
    }
    else // error
    {
        emit downloadFinished( url, QByteArray() );
    }
}

/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2016                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/





#ifndef _DOWNLOADTHREAD_H
#define _DOWNLOADTHREAD_H


#include <QThread>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QList>
#include <QBuffer>

/*-------------------------------------------------------------------------*/
/**
 * @file  DownloadThread.h
 * @brief Declaration of the class DownloadThread
 *
 * This header file provides the declaration of the class DownloadThread.
 */
/*-------------------------------------------------------------------------*/


/*--- Forward declarations ------------------------------------------------*/

class QBuffer;

/*-------------------------------------------------------------------------*/
/**
 * @class DownloadThread
 * @brief Provides a thread class capable of downloading data
 *
 * The class DownloadThread provides the capability of downloading data
 * via the HTTP and FILE protocols into a memory buffer in the background.
 */
/*-------------------------------------------------------------------------*/
namespace cubegui
{
class DownloadThread : public QObject
{
    Q_OBJECT

public:
    DownloadThread( const QUrl& url );
    DownloadThread( const QList<QUrl>& list );

public slots:
    void
    loadList();

signals:
    /**
     * @brief downloadFinished is emitted after the download finishes, either due to
     * an error or after all data has been downloaded.
     * @param url the URL from which the data should be loaded
     * @param buffer the contents of the download, if buffer.isNull() an error has occured
     */
    void
    downloadFinished( const QUrl&       url,
                      const QByteArray& buffer );

private slots:
    void
    currentDownloadFinished( QNetworkReply* reply = 0 );

private:

    // / Memory buffer for storing the data
    QNetworkAccessManager manager;
    QList<QUrl>           urlList;
    QUrl                  url;
};
}
#endif

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

#ifndef _HTML_WIDGET_TEXTVIEW_H
#define _HTML_WIDGET_TEXTVIEW_H

#include "config-frontend.h"  // defines WITH_WEB_ENGINE

#include <QTextBrowser>
#include <QBuffer>
#include <QHash>
#include <QSet>
#include "HtmlWidget.h"
#include "DownloadThread.h"

namespace cubegui
{
/*-------------------------------------------------------------------------*/
/**
 * @class HtmlWidget
 * @brief Provides a simple HTML-browser widget
 *
 * This class provides a simple HTML-browser widget based on Qt's
 * QTextBrowser class. Its main extension over QTextBrowser is the
 * ability to load HTML pages as well as referenced resources (i.e.,
 * images in particular) directly from a web server or a file,
 * respectively.
 *
 * This widget is not intended to be used as a full-featured web
 * browser, but more as an easy way to display online help messages
 * in HTML format. When writing the documents to be displayed, keep
 * in mind that this widget only supports a limited subset of HTML
 * (see Qt's reference documentation for details).
 */
/*-------------------------------------------------------------------------*/
class HtmlTextBrowser;
class HtmlWidgetTextView : public HtmlWidget
{
    Q_OBJECT

public:
    HtmlWidgetTextView( QWidget* parent = 0 );

    void
    showUrl( const QUrl& url );
    void
    showHtml( const QString& html );

public slots:
    void
    back();
    void
    forward();

private slots:
    void
    onUrlLoaded( const QUrl&       url,
                 const QByteArray& buffer );

    void
    onLoadingFinished( const QUrl& url,
                       bool        ok );

    void
    onLinkClicked( QUrl url );

    void
    scroll();

private:
    HtmlTextBrowser* view;
    DownloadThread*  download;
    QUrl             lastUrl;
    QString          anchor;
    bool             first; // workaround for bug #855
    QList<QUrl>      history;
    int              position;
    bool             changeHistory;
    void
    addToHistory( QUrl url );
};

class HtmlTextBrowser : public QTextBrowser
{
    Q_OBJECT
public:
    /**
        for internal use: loads all referenced resources
     */
    virtual QVariant
    loadResource( int         type,
                  const QUrl& name );
    void
    setDocument( const QString& html,
                 const QString& url );

private slots:
    void
    finishedRequest();

private:
    QNetworkAccessManager      manager;
    QSet<QString>              resourceSet;  // resources in the set are either already loaded (cached) or beeing loaded
    QHash<QString, QByteArray> resourceHash; // cache for external resources of a html document
    QUrl                       baseUrl;
    void
    loadUrlResource( const QUrl& url );
};
}

#endif // _HTML_WIDGET_TEXTVIEW_H

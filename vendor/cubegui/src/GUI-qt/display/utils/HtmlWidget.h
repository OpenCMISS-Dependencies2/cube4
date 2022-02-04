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


#ifndef _HTMLWIDGET_H
#define _HTMLWIDGET_H
#include <QWidget>
#include <QList>
#include <QUrl>
#include <QNetworkAccessManager>

namespace cubegui
{
/**
 * @brief The MirrorCheck class tests, if one of the given mirrors is valid. This cannot be done in QWebEngineView, because it currently
 * doesn't allow to get detailed error codes from a http request.
 */
class MirrorCheck : public QObject
{
    Q_OBJECT
public:
    MirrorCheck();
    /** checks if one if the mirrors in urlList can be loaded and emits mirrorFound */
    void
    checkMirrors( const QList<QUrl>& urlList );

signals:
    /** This signal is emitted, after the first valid mirror has been found. Otherwise if all mirrors are invalid, the url is invalid
     * @param url is the first mirror, which could be loaded */
    void
    mirrorFound( const QUrl& url );

private slots:
    void
    onFinished( QNetworkReply* reply );

private:
    QList<QUrl>           mirrorList;
    QNetworkAccessManager manager;
    QUrl                  url;
    QUrl                  lastValidUrl;
    void
    checkMirrors();
};

/*-------------------------------------------------------------------------*/
/**
 * @class HtmlWidget
 * @brief Provides an interface for a HTML-browser widget
 */
/*-------------------------------------------------------------------------*/
class HtmlWidget : public QWidget
{
    Q_OBJECT
public:
    HtmlWidget();

    static HtmlWidget*
    createHtmlWidget();

    /** @brief showUrl loads a url and shows its contents. It emits the signal urlLoaded() or loadingFailed(), after having finished.
     * @param mirror a url that may contain a string @mirror@ which should be replaced by a mirror of the cube mirrorlist
     */
    void
    showUrl( const QString& mirror );

    /**
     * @brief showHtml shows the contents of the parameter html
     */
    virtual void
    showHtml( const QString& html ) = 0;

    /**
     * @brief showHtml shows the contents of the given URL
     */
    virtual void
    showUrl( const QUrl& url ) = 0;

    /** load previous document */
    virtual void
    back()
    {
    }
    /** load next document */
    virtual void
    forward()
    {
    }

signals:
    /** emitted, if URL has been sucessfully loaded */
    void
    urlLoaded( QUrl url );

    /** emitted, if loading of the given URL has failed */
    void
    loadingFailed( QUrl url );

private slots:
    void
    loadURL( const QUrl& url );

private:
    QList<QString>          mirrorList;  // list of all mirror sites given from cube file and CUBE_DOCPATH
    QHash<QString, QString> lastMirrors; // Url with @mirror@ tag -> valid Url
    QString                 lastUrl;     // previously loaded Url; document is not reloaeded, if next Url only differs in anchor
    MirrorCheck             check;
};
}

#endif   /* !_HTMLWIDGET_H */

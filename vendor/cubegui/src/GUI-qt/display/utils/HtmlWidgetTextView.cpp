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
#include <QBuffer>
#include <QDebug>
#include <QEventLoop>
#include <QVBoxLayout>
#include <QTimer>

#include "HtmlWidgetTextView.h"

using namespace std;
using namespace cubegui;

/*--- Constructors & destructor -------------------------------------------*/

/**
 * @brief Constructs an empty instance
 *
 * Constructs a new, empty TextViewHtmlWidget instance width the given @p parent.
 *
 * @param  parent  Parent widget
 */
HtmlWidgetTextView::HtmlWidgetTextView( QWidget* )
{
    first         = true;
    position      = -1;
    changeHistory = true;
    view          = new HtmlTextBrowser();
    setLayout( new QVBoxLayout );
    layout()->setContentsMargins( 0, 0, 0, 0 );
    layout()->addWidget( view );

    view->setOpenExternalLinks( true );
    connect( view, SIGNAL( sourceChanged( QUrl ) ), this, SLOT( onLinkClicked( QUrl ) ) );
    download = NULL;
}

void
HtmlWidgetTextView::showHtml( const QString& html )
{
    view->setHtml( html );
}

void
HtmlWidgetTextView::onLoadingFinished( const QUrl& url, bool ok )
{
    if ( ok )                // update Url history
    {
        if ( changeHistory ) // don't change history list, if back/forward have been chosen
        {
            addToHistory( url );
        }
    }
    changeHistory = true;

    if ( download )
    {
        download->disconnect();
        download->deleteLater();
        download = NULL;
    }
}

/*--- browser history -------------------------------------------------*/

/** called if user has clicked on a link */
void
HtmlWidgetTextView::onLinkClicked( QUrl url )
{
    QUrl absoluteUrl;
    if ( url.isRelative() )
    {
        absoluteUrl = lastUrl.resolved( url );
    }
    else
    {
        absoluteUrl = url;
    }
    addToHistory( absoluteUrl );
}

void
HtmlWidgetTextView::addToHistory( QUrl url )
{
    lastUrl = url;
    int count = history.size() - 1 - position;
    for ( int i = 0; i < count; i++ )
    {
        history.removeLast();
    }
    history.append( url );
    position = history.size() - 1;
}

void
HtmlWidgetTextView::back()
{
    changeHistory = false;
    if ( position > 0 ) // not first element
    {
        showUrl( QUrl( history.at( --position ) ) );
    }
}

void
HtmlWidgetTextView::forward()
{
    changeHistory = false;
    if ( position < history.size() - 1 ) // not last element
    {
        showUrl( history.at( ++position ) );
    }
}

/*--- Loading of contents -------------------------------------------------*/

void
HtmlWidgetTextView::showUrl( const QUrl& url )
{
    // check if current URL base is already loaded
    bool    alreadyLoaded = false;
    QString lastBase      = lastUrl.toString( QUrl::RemoveFragment );
    if ( !lastBase.isEmpty() )
    {
        alreadyLoaded = ( url.toString( QUrl::RemoveFragment ) == lastUrl.toString( QUrl::RemoveFragment ) );
    }
    if ( !alreadyLoaded )     // new URL requested
    {
        view->clear();        // Clear the document
        download = new DownloadThread( url );
        connect( download, SIGNAL( downloadFinished( QUrl, const QByteArray & ) ), this, SLOT( onUrlLoaded( QUrl, const QByteArray & ) ) );
        download->loadList();
    }
    else
    {
        // same document => scroll to anchor
        QString anchor = url.fragment();
        if ( !anchor.isEmpty() )
        {
            view->scrollToAnchor( anchor );
        }
        onLoadingFinished( lastUrl, true ); // already loaded => only jump to anchor if required
    }
}

/**
 * @brief HelpBrowser::urlLoaded called, after document has been loaded
 */
void
HtmlWidgetTextView::onUrlLoaded( const QUrl& url, const QByteArray& buffer )
{
    if ( !buffer.isNull() )
    {
        lastUrl = url;
        view->setDocument( QString( buffer ), url.toString() ); // display document

        QString anchor = url.fragment();
        if ( !anchor.isEmpty() )
        {
            if ( first )
            {
                this->anchor = anchor;
                first        = false;
            }
            view->scrollToAnchor( anchor );
        }
        onLoadingFinished( url, true );
    }
    else
    {
        lastUrl = QUrl();
        onLoadingFinished( url, false );
    }
}

void
HtmlWidgetTextView::scroll()
{
    view->scrollToAnchor( anchor );
}

//====================================================================================================================

void
HtmlTextBrowser::setDocument( const QString& html, const QString& url )
{
    resourceSet.clear();
    resourceHash.clear();
    baseUrl = url;
    setHtml( html );
}

/**
 * @brief Loads referenced resources
 *
 * This method is called whenever an additional resource (e.g., an image or
 * an CSS style sheet) is referenced from the document. It reimplements the
 * implementation provided by Qt's QTextBrowser class.
 *
 * @param  type  Type of the resource
 * @param  name  Resource URL
 *
 * @return Resource data as QByteArray (encapsulated in a QVariant) on
 *         success, an empty QVariant object otherwise.
 */
QVariant
HtmlTextBrowser::loadResource( int type, const QUrl& name )
{
    Q_UNUSED( type );

    // Sanity check
    if ( !name.isValid() )
    {
        cerr << tr( "Invalid URL: " ).toUtf8().data() << name.toString().toStdString().c_str() << endl;
        return QVariant();
    }

    // Determine absolute URL
    QUrl absoluteUrl;
    if ( name.isRelative() )
    {
        absoluteUrl = baseUrl.resolved( name );
    }
    else
    {
        absoluteUrl = name;
    }

    QVariant result = resourceHash.value( absoluteUrl.toString() );
    if ( !result.isNull() ) // already read, return cached value
    {
        return result;
    }
    else if ( resourceSet.contains( absoluteUrl.toString() ) ) // resource is about to be read
    {
        return QVariant();                                     // return empty value -> will be requested later again
    }
    if ( absoluteUrl.toString().startsWith( "file://" ) )
    {
        result = QTextBrowser::loadResource( type, absoluteUrl ); // try default resource loader
    }
    if ( result.isNull() )                                        // not a local resource => load from network
    {
        resourceSet.insert( absoluteUrl.toString() );             // mark as about to be read
        loadUrlResource( absoluteUrl );                           // load in background
        return QVariant();
    }
    return result;
}

void
HtmlTextBrowser::loadUrlResource( const QUrl& url )
{
    if ( !url.isValid() ) // Sanity check
    {
        return;
    }
    QNetworkReply* response = manager.get( QNetworkRequest( url ) );
    response->setObjectName( url.toString() );
    connect( response, SIGNAL( finished() ), SLOT( finishedRequest() ) );
}

void
HtmlTextBrowser::finishedRequest()
{
    QNetworkReply* response = qobject_cast<QNetworkReply*>( sender() );
    QString        url      = response->objectName();
    QByteArray     data     = response->readAll();
    resourceHash.insert( url, data );
    if ( resourceSet.size() == resourceHash.size() ) // all resources have been loaded
    {
        setLineWrapColumnOrWidth( 0 );               // workaround to update view with correct image sizes/layout
        QString anchor = QUrl( baseUrl ).fragment();
        if ( !anchor.isEmpty() )                     // scroll again to anchor after images have been inserted
        {
            scrollToAnchor( anchor );
        }
    }
    response->deleteLater();
}

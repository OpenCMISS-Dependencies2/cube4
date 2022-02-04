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



#ifndef _HELPBROWSER_H
#define _HELPBROWSER_H

#include "config-frontend.h"  // defines WITH_WEB_ENGINE

#include <QTextBrowser>
#include <QDialog>
#include <QUrl>
#include <QBuffer>
#ifdef WITH_WEB_ENGINE
#include <QWebEngineView>
#endif
#include "HtmlWidget.h"

/*-------------------------------------------------------------------------*/
/**
 * @class HelpBrowser
 * @brief Provides a window which displays HTML help text. Use getInstance()
 * to reuse the current instance, if available.
 */
/*-------------------------------------------------------------------------*/

namespace cubegui
{
class HtmlWidget;

class HelpBrowser : public QWidget
{
    Q_OBJECT

public:
    static HelpBrowser*
    getInstance( const QString title = tr( "Cube Help Browser" ) );

public slots:
    /** displays the contents of the given url in a browser widget */
    void
    showUrl( const QString& url,
             const QString& errorMessage = "" );

    /** displays the contents of the given url in a browser widget */
    void
    showUrl( const QUrl&    url,
             const QString& errorMessage = "" );

    /** displays the raw html data in in a browser widget */
    void
    showHtml( const QString& html );

private slots:
    void
    loadFinished( const QUrl& url );

    void
    loadFailed( const QUrl& url );

private:
    HelpBrowser( const QString& title );

    /// Browser widget used to display the help text
    QBuffer     buffer;           // buffer for contents of URL
    QString     errorMessage;     // message to show, if URL could not be loaded
    HtmlWidget* html;

    static HelpBrowser* single;
};
}
#endif

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

#ifndef _HTML_WIDGET_WEBENGINE_H
#define _HTML_WIDGET_WEBENGINE_H

#include "config-frontend.h"
#ifdef WITH_WEB_ENGINE

#include <QWebEngineView>
#include "HtmlWidget.h"

/** Implementation of HtmlWidget using QWebEngineView */
namespace cubegui
{
class HtmlWidgetWebEngine : public HtmlWidget
{
    Q_OBJECT
public:
    HtmlWidgetWebEngine();
    // HtmlWidget interface
    void
    showUrl( const QUrl& url );
    void
    showHtml( const QString& html );

public slots:
    void
    back()
    {
        webView->back();
    }
    void
    forward()
    {
        webView->forward();
    }

private:
    QWebEngineView* webView;
    QUrl            url;
};
}

#endif // WITH_WEB_ENGINE
#endif // _HTML_WIDGET_WEBENGINE_H

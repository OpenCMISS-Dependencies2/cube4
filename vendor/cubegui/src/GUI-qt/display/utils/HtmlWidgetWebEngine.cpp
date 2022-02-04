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


#ifdef WITH_WEB_ENGINE
#include <iostream>
#include <QDebug>
#include <QVBoxLayout>
#include <QWebEngineSettings>
#include "HtmlWidgetWebEngine.h"
#include "Globals.h"

using namespace cubegui;

HtmlWidgetWebEngine::HtmlWidgetWebEngine()
{
    webView = new QWebEngineView();
    webView->settings()->setAttribute( QWebEngineSettings::FocusOnNavigationEnabled, false ); // don't steal focus if document is loaded
    setLayout( new QVBoxLayout );
    layout()->setContentsMargins( 0, 0, 0, 0 );
    layout()->addWidget( webView );
}

void
HtmlWidgetWebEngine::showUrl( const QUrl& url )
{
    this->url = url;
    webView->load( url );
    //webView->setZoomFactor( .9 );
}

void
HtmlWidgetWebEngine::showHtml( const QString& html )
{
    webView->setHtml( html );
}

#endif // WITH_WEB_ENGINE

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

#include <vector>
#include <QDebug>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QApplication>
#include <QStyle>
#include "HelpBrowser.h"
#include "Globals.h"
#include "HtmlWidget.h"

using namespace cubegui;

HelpBrowser* HelpBrowser::single = 0;

/*--- Constructors & destructor -------------------------------------------*/

/**
 * @brief Constructs a new help browser dialog
 *
 * This constructor creates a new help browser dialog with the given window
 * @p title and @p parent widget.
 *
 * @param  title   Window title
 * @param  parent  Parent widget
 */
HelpBrowser::HelpBrowser( const QString& title )
{
    setVisible( false );

    // Dialog defaults
    setWindowTitle( title );
    resize( 1000, 1100 );

    // Set up layout manager
    QVBoxLayout* layout = new QVBoxLayout();
    setLayout( layout );
    layout->setContentsMargins( 0, 0, 0, 0 );

    html = HtmlWidget::createHtmlWidget();
    connect( html, SIGNAL( urlLoaded( QUrl ) ), this, SLOT( loadFinished( QUrl ) ) );
    connect( html, SIGNAL( loadingFailed( QUrl ) ), this, SLOT( loadFailed( QUrl ) ) );

    QDialogButtonBox* buttonBox = new QDialogButtonBox( this );
    buttonBox->layout()->setContentsMargins( 10, 10, 10, 5 );

    QPushButton* back = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_ArrowLeft ), "" );
    connect( back, SIGNAL( pressed() ), html, SLOT( back() ) );
    buttonBox->addButton( back, QDialogButtonBox::ResetRole );
    QPushButton* forw = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_ArrowRight ), "" );
    connect( forw, SIGNAL( pressed() ), html, SLOT( forward() ) );
    buttonBox->addButton( forw, QDialogButtonBox::ResetRole );

    buttonBox->setStandardButtons( QDialogButtonBox::Close );
    connect( buttonBox, SIGNAL( rejected() ), this, SLOT( close() ) );
    layout->addWidget( buttonBox );
    layout->addWidget( html );
}


HelpBrowser*
HelpBrowser::getInstance( const QString title )
{
    if ( !single )
    {
        single = new HelpBrowser( title );
    }

    return single;
}

//* @name Displaying a help text from  QString */
void
HelpBrowser::showHtml( const QString& data )
{
    html->showHtml( data );

    setVisible( true );
    activateWindow();
    showNormal();
}

/*--- Displaying a help text ----------------------------------------------*/

/**
 * @brief Displays the help text with the given URL
 *
 * This method displays the help text with the given @p url. If the base
 * URL of @p url is identical to the one of the current document, this
 * method will only scroll to the new anchor (if provided). Otherwise, the
 * new document will be loaded.
 *
 * @param  url  URL of the help text to be shown
 *
 * @return @b true if successful, @b false otherwise (e.g., in case of a
 *         download error).
 */

#include "PluginManager.h"
void
HelpBrowser::showUrl( const QString& url, const QString& errorMessage )
{
    this->errorMessage = errorMessage.isEmpty() ? tr( "Could not load " ) + url : errorMessage;
    html->showUrl( url );
}

void
HelpBrowser::showUrl( const QUrl& url, const QString& errorMessage )
{
    this->errorMessage = errorMessage.isEmpty() ? tr( "Could not load " ) + url.toString() : errorMessage;
    html->showUrl( url );
}

/** called, after document has been sucessfully loaded  */
void
HelpBrowser::loadFinished( const QUrl& )
{
    setVisible( true );
    activateWindow();
    showNormal();
}

void
HelpBrowser::loadFailed( const QUrl& url )
{
    if ( isVisible() ) // don't popup browser window, if document doesn't exist
    {
        showHtml( "" );
    }
    Globals::setStatusMessage( "Failed to load Url " + url.toString(), Information );
    Globals::setStatusMessage( errorMessage, Error );
}

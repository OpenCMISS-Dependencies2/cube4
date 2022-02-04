/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>
#include <QString>
#include <QApplication>
#include <QStyle>
#include "HelpBrowser.h"
#include "CubeHelpButton.h"
#include "Globals.h"

using namespace advisor;

HelpButton::HelpButton( const QString& hlp, bool active   ) : QPushButton( QApplication::style()->standardIcon( ( active ) ? QStyle::SP_MessageBoxQuestion : QStyle::SP_MessageBoxWarning ), "" ), helpUrl( hlp )
{
    connect( this, SIGNAL( clicked( bool ) ), this, SLOT( showHelp( bool ) ) );
}



void
HelpButton::showHelp( bool )
{
    QString               path        = cubegui::Globals::getOption( cubegui::DocPath ) + "cubegui/guide/html/";
    cubegui::HelpBrowser* helpBrowser = cubegui::HelpBrowser::getInstance( tr( "Advisor Documentation" ) );
    helpBrowser->showUrl( path + helpUrl, tr( "Cannot find help for " ) + ( helpUrl ) );
}

/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include "config.h"

#include <QtPlugin>
#include <QDebug>
#include <QWidget>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QApplication>
#include <QStringList>
#include <QScrollArea>
#include <QMessageBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <fstream>

#include "ScorePConfig.h"
#include "CubeServices.h"
#include "Cube.h"
#include "Globals.h"
#include "Compatibility.h"

#ifdef HAS_QREGULAR_EXPRESSION
#include <QRegularExpression>
#define REGULAR_EXPRESSION QRegularExpression
#else
#include <QRegExp>
#define REGULAR_EXPRESSION QRegExp
#endif

using namespace std;
using namespace cube;
using namespace cubepluginapi;
using namespace scorepconfigplugin;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( scorepconfigplugin::ScorePConfigPlugin, scorepconfigplugin::ScorePConfigPlugin ) // ( PluginName, ClassName )
#endif

void
ScorePConfigPlugin::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

QString
ScorePConfigPlugin::name() const
{
    return "ScoreP Config";
}

bool
ScorePConfigPlugin::cubeOpened( PluginServices* service )
{
    widget_       = NULL;
    this->service = service;

    widget_ = new  QWidget();
    QHBoxLayout* main_layout = new QHBoxLayout();
    widget_->setLayout( main_layout );

    QScrollArea* scroll = new QScrollArea();
    main_layout->addWidget( scroll );
    scroll->setWidgetResizable( true );
    scroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    scroll->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOn );
    scroll->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding ) );

    QTableWidget* table = new QTableWidget( 0, 2 );
    scroll->setWidget( table );
#if QT_VERSION < 0x050000
    table->horizontalHeader()->setResizeMode( QHeaderView::Stretch );
#else
    table->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
#endif
    QString cubepath = service->getCubeFileName();
    cubepath.remove( tr( "file://" ) );
    QFileInfo _fileinfo( cubepath );
    QString   path = _fileinfo.absolutePath() + "/scorep.cfg";

    QFileInfo configfile( path );
    if ( configfile.exists() )
    {
        QFile myTextFile( path );

        if ( !myTextFile.open( QIODevice::ReadOnly ) )
        {
            QMessageBox::information( 0, tr( "Error opening file" ), myTextFile.errorString() );
        }
        else
        {
            service->debug() << tr( "read scorep.cfg " ) << Qt::endl;
            int         row = 0;
            QStringList header;
            header << tr( "Variable" ) << tr( "Value" );
            table->setVerticalHeaderLabels( header );
            table->setHorizontalHeaderLabels( header );
            while ( !myTextFile.atEnd() )
            {
                QString     line     = myTextFile.readLine();
                QStringList varvalue = line.split( REGULAR_EXPRESSION( "\\s*=\\s*" ) );
                table->setRowCount( row + 1 );
                QTableWidgetItem* varItem = new QTableWidgetItem( varvalue[ 0 ] );
                table->setItem( row, 0, varItem );
                QTableWidgetItem* valueItem = new QTableWidgetItem( varvalue[ 1 ].trimmed() );
                table->setItem( row, 1, valueItem );

                varItem->setToolTip( help.getHelp( varvalue[ 0 ] ) );
                valueItem->setToolTip( varvalue[ 1 ] );

                varItem->setFlags( varItem->flags() &  ~Qt::ItemIsEditable );
                valueItem->setFlags( valueItem->flags() &  ~Qt::ItemIsEditable );

                ++row;
            }
            myTextFile.close();
        }
//         table->setSortingEnabled(true); // for the case one would like to enable sorting in one or another way
        service->addTab(  cubepluginapi::SYSTEM, this, OTHER_PLUGIN_TAB  );
        return true;
    }
    else
    {
        service->debug() << tr( " Cannot find " ) << path << " " << cubepath << Qt::endl;
        return false;
    }
}

QWidget*
ScorePConfigPlugin::widget()
{
    return widget_;
}

QString
ScorePConfigPlugin::label() const
{
    return tr( "Score-P Configuration" );
}



QIcon
ScorePConfigPlugin::icon() const
{
    return QIcon( ":/images/scorepconfig.png" );
}

void
ScorePConfigPlugin::cubeClosed()
{
}




QString
ScorePConfigPlugin::getHelpText() const
{
    return tr( "This plugin displays scorep.conf file and gives an expertise about values "
               " of various environment variables and the way they affect Score-P measurement.\n"
               "Furthermore this plugin support user in decision making process in order to "
               "make an efficient performance measurement of his application" );
}

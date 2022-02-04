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
#include <QtPlugin>
#include <QDebug>
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QStyle>
#include <QFileDialog>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "Tau2Cube.h"
#include "ContextFreeServices.h"
#include "CubeProxy.h"
#include "Globals.h"

using namespace cube;
using namespace cubepluginapi;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( Tau2CubePlugin, Tau2Cube ) // ( PluginName, ClassName )
#endif

void
Tau2Cube::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

QString
Tau2Cube::name() const
{
    return "Tau to Cube";
}

void
Tau2Cube::opened( ContextFreeServices* service )
{
    this->service = service;

    widget = this->service->getWidget();

    QHBoxLayout* layoutOuter = new QHBoxLayout();
    widget->setLayout( layoutOuter );



    QWidget*     inner  = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout( inner );
    layoutOuter->addWidget( inner );
    inner->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

    cubes = new QListWidget();


    QPushButton* addCube   = new QPushButton( tr( "Add TAU experiments" ) );
    QPushButton* clearList = new QPushButton( tr( "Clear list" ) );

    merge = new QPushButton( tr( "Show result" ) );
    connect( addCube,                   SIGNAL( clicked() ),    this, SLOT( selectProfilesCubes() ) );
    connect( clearList,                   SIGNAL( clicked() ),    this, SLOT( clearCubes() ) );
    connect( merge,                     SIGNAL( clicked() ),    this, SLOT( startAction() ) );

    layout->addWidget( cubes );
    layout->addWidget( addCube );
    layout->addWidget( clearList );
    layout->addWidget( merge );

    layout->addItem( new QSpacerItem( 0, 10 ) );

    merge->setEnabled( false );
    merge->setStyleSheet( "padding:4px;text-align: center" );

    if ( !args.isEmpty() )
    {
        selectCubes( args );
        startAction();
    }
}

void
Tau2Cube::selectProfilesCubes()
{
    selectCubes(  QStringList( QFileDialog::getExistingDirectory( widget, tr( "Select TAU Directory" ),
                                                                  tr( "" ),
                                                                  QFileDialog::ShowDirsOnly
                                                                  | QFileDialog::DontResolveSymlinks ) )
                  );
}


void
Tau2Cube::selectCubes( const QStringList& experiments )
{
    cubes->addItems( experiments );
    merge->setEnabled( cubes->count() >= 1 );
}

void
Tau2Cube::clearCubes()
{
    cubes->clear();
    merge->setEnabled( cubes->count() >= 1 );
}


void
Tau2Cube::startAction()
{
    std::vector<std::string> fileNames;
    for ( int i = 0; i < cubes->count(); i++ )
    {
        fileNames.push_back( cubes->item( i )->text().toStdString() );
    }
    try
    {
        CubeProxy* cube = CubeProxy::create( ALGORITHM_TAU, fileNames );
        service->openCube( cube );
    }
    catch ( const cube::Error& e )
    {
        QString message = QString( e.what() );
        service->setMessage( message, cubepluginapi::Error );
        return;
    }
}

void
Tau2Cube::closed()
{
}

QString
Tau2Cube::getHelpText() const
{
    return tr( "This plugin allows to open (multiple) TAU profile Directory in Cube GUI." );
}

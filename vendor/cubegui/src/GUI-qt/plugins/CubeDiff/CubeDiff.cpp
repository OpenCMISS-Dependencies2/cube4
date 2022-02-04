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
#include <QFileDialog>
#include <QApplication>
#include <QStyle>

#include "CubeDiff.h"
#include "ContextFreeServices.h"
#include "CubeProxy.h"
#include "algebra4.h"
#include "Globals.h"

using namespace cube;
using namespace cubepluginapi;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( CubeDiffPlugin, CubeDiff ) // ( PluginName, ClassName )
#endif

void
CubeDiff::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

QString
CubeDiff::name() const
{
    return "Cube Diff";
}

void
CubeDiff::opened( ContextFreeServices* service )
{
    this->service = service;

    QWidget*     widget      = service->getWidget();
    QHBoxLayout* layoutOuter = new QHBoxLayout();
    widget->setLayout( layoutOuter );

    reduce   = new QCheckBox( tr( "Reduce system dimension" ) );
    collapse = new QCheckBox( tr( "Collapse system dimension" ) );
    connect( reduce, SIGNAL( pressed() ), this, SLOT( uncheckChoice() ) );
    connect( collapse, SIGNAL( pressed() ), this, SLOT( uncheckChoice() ) );

    QWidget*     inner  = new QWidget();
    QGridLayout* layout = new QGridLayout( inner );
    layoutOuter->addWidget( inner );
    inner->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

    QPushButton* but1 = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_DirOpenIcon ), tr( " Open cube file (minuend)" ) );
    connect( but1, SIGNAL( clicked() ), this, SLOT( loadFile1() ) );
    but1->setStyleSheet( "padding:4px;text-align: left" );

    QPushButton* but2 = new QPushButton( QApplication::style()->standardIcon( QStyle::SP_DirOpenIcon ), tr( " Open cube file (subtrahend)" ) );
    connect( but2, SIGNAL( clicked() ), this, SLOT( loadFile2() ) );
    but2->setStyleSheet( "padding:4px;text-align: left" );

    diff = new QPushButton( tr( "Show difference" ) );
    connect( diff, SIGNAL( clicked() ), this, SLOT( startAction() ) );
    diff->setStyleSheet( "padding:4px;text-align: center" );
    diff->setEnabled( false );

    fileLabel1 = new QLabel();
    fileLabel2 = new QLabel();

    int line = 0;
    layout->addWidget( but1, line, 0 );
    layout->addWidget( fileLabel1, line++, 1 );
    layout->addWidget( but2, line, 0 );
    layout->addWidget( fileLabel2, line++, 1 );

    layout->addItem( new QSpacerItem( 0, 10 ), line++, 0 );
    layout->addWidget( reduce, line++, 0 );
    layout->addWidget( collapse, line++, 0 );
    layout->addItem( new QSpacerItem( 0, 10 ), line++, 0 );

    layout->addWidget( diff, line++, 0 );

    if ( !args.isEmpty() )
    {
        fileName1 = args.takeFirst();
    }
    if ( !args.isEmpty() )
    {
        fileName2 = args.takeFirst();
    }
}

void
CubeDiff::uncheckChoice()
{
    if ( sender() == reduce )
    {
        collapse->setChecked( false );
    }
    else
    {
        reduce->setChecked( false );
    }
}

void
CubeDiff::loadFile1()
{
    QString fn = QFileDialog::getOpenFileName( service->getWidget(),
                                               tr( "Choose first file to diff" ),
                                               QDir::currentPath(),
                                               tr( "Cube3/4 files (*cube *cube.gz *.cubex);;Cube4 files (*.cubex);;Cube3 files (*.cube.gz *.cube);;All files (*.*);;All files (*)" ) );
    if ( !fn.isEmpty() )
    {
        fileName1 = fn;
        fileLabel1->setText( fileName1 );
        if ( !fileName2.isEmpty() )
        {
            diff->setEnabled( true );
        }
        else
        {
            diff->setEnabled( false );
        }
    }
}

void
CubeDiff::loadFile2()
{
    QString fn = QFileDialog::getOpenFileName( service->getWidget(),
                                               tr( "Choose second file to diff" ),
                                               QDir::currentPath(),
                                               tr( "Cube3/4 files (*cube *cube.gz *.cubex);;Cube4 files (*.cubex);;Cube3 files (*.cube.gz *.cube);;All files (*.*);;All files (*)" ) );
    if ( !fn.isEmpty() )
    {
        fileName2 = fn;
        fileLabel2->setText( fileName2 );
        if ( !fileName1.isEmpty() )
        {
            diff->setEnabled( true );
        }
        else
        {
            diff->setEnabled( false );
        }
    }
}

void
CubeDiff::startAction()
{
    try
    {
        std::vector<std::string> fileNames;
        fileNames.push_back( fileName1.toStdString() );
        fileNames.push_back( fileName2.toStdString() );
        std::vector<std::string> options;
        if ( collapse->isChecked() )
        {
            options.push_back( "collapse" );
        }
        else if ( reduce->isChecked() )
        {
            options.push_back( "reduce" );
        }
        CubeProxy* diff = CubeProxy::create( ALGORITHM_DIFF, fileNames, options );
        service->openCube( diff );
    }
    catch ( const cube::Error& e )
    {
        QString message = QString( e.what() );
        service->setMessage( message, cubepluginapi::Error );
        return;
    }
}

void
CubeDiff::closed()
{
    fileName1 = "";
    fileName2 = "";
}

QString
CubeDiff::getHelpText() const
{
    return tr( "This plugin calculates the difference beetween the given two cube files and displays them." );
}

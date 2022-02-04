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
#include <QButtonGroup>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QStringList>
#include <QApplication>
#include <QStyle>
#include <QGroupBox>
#include <QLineEdit>
#include "Compatibility.h"
#ifdef HAS_QREGULAR_WILDCARD_EXPRESSION
#include <QRegularExpression>
#else
#include <QRegExp>
#endif


#include "CubeMerge.h"
#include "ContextFreeServices.h"
#include "CubeProxy.h"
#include "algebra4.h"
#include "Globals.h"

using namespace cube;
using namespace cubepluginapi;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( CubeMergePlugin, CubeMerge ) // ( PluginName, ClassName )
#endif

CubeMerge::CubeMerge()
{
}

void
CubeMerge::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

QString
CubeMerge::name() const
{
    return "Cube Merge";
}

void
CubeMerge::opened( ContextFreeServices* service )
{
    this->service = service;

    QWidget* widget = this->service->getWidget();

    QHBoxLayout* layoutOuter = new QHBoxLayout();
    widget->setLayout( layoutOuter );



    QWidget*     inner  = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout( inner );
    layoutOuter->addWidget( inner );
    inner->setSizePolicy( QSizePolicy::Maximum, QSizePolicy::Maximum );

    cubes = new QListWidget();


    QPushButton* addCube = new QPushButton( tr( "Add cubes" ) );
    addCube->setToolTip( tr( "Select a directory and all found .cubex files in all subdirectories will be added" ) );
    QPushButton* clearList = new QPushButton( tr( "Clear list" ) );

    merge    = new QPushButton( tr( "Show merge" ) );
    reduce   = new QCheckBox( tr( "Reduce system dimension" ) );
    collapse = new QCheckBox( tr( "Collapse system dimension" ) );
    connect( addCube,                   SIGNAL( clicked() ),    this, SLOT( selectCubes() ) );
    connect( clearList,                   SIGNAL( clicked() ),    this, SLOT( clearCubes() ) );
    connect( merge,                     SIGNAL( clicked() ),    this, SLOT( startAction() ) );
    connect( reduce,                    SIGNAL( pressed() ),    this, SLOT( uncheckChoice() ) );
    connect( collapse,                  SIGNAL( pressed() ),    this, SLOT( uncheckChoice() ) );

    layout->addWidget( cubes );

    QGridLayout*  names_layout    = new QGridLayout;
    QGroupBox*    names_selection = new QGroupBox( tr( "Files for selection" ) );
    QButtonGroup* cubenames       = new QButtonGroup();
    profile_cubex = new QRadioButton( tr( "profile.cubex" ), widget );
    profile_cubex->setToolTip( tr( "Select only profile.cubex files (in all subdirectories) to perform MERGE operation" ) );
    profile_cubex->setChecked( true );
    cubenames->addButton( profile_cubex );
    names_layout->addWidget( profile_cubex, 0, 0 );
    summary_cubex = new QRadioButton( tr( "summary.cubex" ), widget );
    summary_cubex->setToolTip( tr( "Select only summary.cubex files (in all subdirectories) to perform MERGE operation" ) );
    summary_cubex->setChecked( false );
    cubenames->addButton( summary_cubex );
    names_layout->addWidget( summary_cubex, 1, 0 );
    scout_cubex = new QRadioButton( tr( "scout.cubex" ), widget );
    scout_cubex->setToolTip( tr( "Select only scout.cubex files (in all subdirectories) to perform MERGE operation" ) );
    scout_cubex->setChecked( false );
    cubenames->addButton( scout_cubex );
    names_layout->addWidget( scout_cubex, 2, 0 );
    trace_cubex = new QRadioButton( tr( "trace.cubex" ), widget );
    scout_cubex->setToolTip( tr( "Select only scout.cubex files (in all subdirectories) to perform MERGE operation" ) );
    trace_cubex->setChecked( false );
    cubenames->addButton( trace_cubex );
    names_layout->addWidget( trace_cubex, 3, 0 );
    custom_cubex = new QRadioButton( tr( "Custom" ), widget );
    custom_cubex->setToolTip( tr( "Select only user-defined (shell wildcards) name of the .cubex files (in all subdirectories) to perform MERGE operation" ) );
    custom_cubex->setChecked( false );
    cubenames->addButton( custom_cubex );
    names_layout->addWidget( custom_cubex, 4, 0 );

    custom_name = new QLineEdit();
    names_layout->addWidget( custom_name, 4, 1 );

    names_selection->setLayout( names_layout );
    layout->addWidget( names_selection );


    layout->addWidget( addCube );
    layout->addWidget( clearList );
    layout->addWidget( merge );

    layout->addWidget( reduce  );
    layout->addWidget( collapse );
    layout->addItem( new QSpacerItem( 0, 10 ) );

    merge->setEnabled( false );
    merge->setStyleSheet( "padding:4px;text-align: center" );

    if ( !args.isEmpty() )
    {
        selectCubes( args.takeFirst() );
        startAction();
    }
}


void
CubeMerge::uncheckChoice()
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
CubeMerge::selectCubes( const QString& dir )
{
    QStringList  experiments;
    QDirIterator it( dir, QDirIterator::Subdirectories );
    QString      name_selection;
    if ( profile_cubex->isChecked() )
    {
        name_selection = "profile";
    }
    else if ( summary_cubex->isChecked() )
    {
        name_selection = "summary";
    }
    else if ( scout_cubex->isChecked() )
    {
        name_selection = "scout";
    }
    else if ( trace_cubex->isChecked() )
    {
        name_selection = "trace";
    }
    else if ( custom_cubex->isChecked() )
    {
        name_selection = custom_name->text();
    }


#ifdef HAS_QREGULAR_WILDCARD_EXPRESSION
    QString            wildcard = QRegularExpression::wildcardToRegularExpression( name_selection );
    QRegularExpression name_pattern( wildcard );
#else
    QRegExp name_pattern( name_selection );
    name_pattern.setPatternSyntax( QRegExp::Wildcard );
#endif

    while ( it.hasNext() )
    {
        QString   file = it.next();
        QFileInfo fi( file );
        QString   name   = fi.baseName();
        QString   suffix = fi.completeSuffix();

        bool matches;
#ifdef HAS_QREGULAR_WILDCARD_EXPRESSION
        QRegularExpressionMatch match = name_pattern.match( name );
        matches = ( match.lastCapturedIndex() == 0 ); // exact one match
#else
        matches = name_pattern.exactMatch( name );
#endif

        if ( ( suffix.compare( "cubex" ) == 0 ) && ( matches ) )
        {
            experiments << file;
        }
    }
    cubes->addItems( experiments );
    merge->setEnabled( cubes->count() >= 2 );
}




void
CubeMerge::selectCubes()
{
    QStringList experiments;
    QString     dir = QFileDialog::getExistingDirectory( service->getWidget(), tr( "Open Directory for MERGE operation" ),
                                                         "",
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks );
    selectCubes( dir );
}



void
CubeMerge::clearCubes()
{
    cubes->clear();
    merge->setEnabled( cubes->count() >= 2 );
}

void
CubeMerge::startAction()
{
    std::vector<std::string> fileNames;
    for ( int i = 0; i < cubes->count(); i++ )
    {
        fileNames.push_back( cubes->item( i )->text().toStdString() );
    }
    std::vector<std::string> options;
    if ( collapse->isChecked() )
    {
        options.push_back( "collapse" );
    }
    else if ( reduce->isChecked() )
    {
        options.push_back( "reduce" );
    }
    try
    {
        cube::CubeProxy* merged = CubeProxy::create( ALGORITHM_MERGE, fileNames, options );
        merged->defineAttribute( "origin", "merge" ); // mark this cube with an attribure
        service->openCube( merged );
    }
    catch ( const cube::Error& e )
    {
        QString message = QString( e.what() );
        service->setMessage( message, cubepluginapi::Error );
        return;
    }
}

void
CubeMerge::closed()
{
}

QString
CubeMerge::getHelpText() const
{
    return tr( "This plugin calculates the merge beetween the given cube files and displays result." );
}

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
#include <algorithm>
#include <vector>
#include <utility>
#include <cassert>

#include <QtPlugin>
#include <QDebug>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QDirIterator>
#include <QFileInfo>
#include <QStringList>
#include <QApplication>
#include <QStyle>
#include <QGroupBox>
#include <QLineEdit>

#include "CubeScaling.h"
#include "ContextFreeServices.h"
#include "CubeProxy.h"
#include "Cube.h"
#include "algebra4.h"
#include "Globals.h"

#include "Compatibility.h"
#ifdef HAS_QREGULAR_WILDCARD_EXPRESSION
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

using namespace cube;
using namespace cubepluginapi;
using namespace scaling;

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2( CubeScalingPlugin, CubeScaling ) // ( PluginName, ClassName )
#endif

void
CubeScaling::version( int& major, int& minor, int& bugfix ) const
{
    major  = 1;
    minor  = 0;
    bugfix = 0;
}

QString
CubeScaling::name() const
{
    return "Scaling";
}

void
CubeScaling::opened( ContextFreeServices* service )
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

    scaling = new QPushButton( tr( "Scaling analysis" ) );
    connect( addCube,                   SIGNAL( clicked() ),    this, SLOT( selectCubes() ) );
    connect( clearList,                   SIGNAL( clicked() ),    this, SLOT( clearCubes() ) );
    connect( scaling,                     SIGNAL( clicked() ),    this, SLOT( startAction() ) );
    layout->addWidget( cubes );
    QGroupBox*    modus_selection = new QGroupBox( tr( "Analysis Modus" ) );
    QVBoxLayout*  modu_layout     = new QVBoxLayout;
    QButtonGroup* options         = new QButtonGroup();
    aggregation = new QRadioButton( tr( "Aggregate over system (accounted resources)" ), widget );
    aggregation->setChecked( true );
    options->addButton( aggregation );
    modu_layout->addWidget( aggregation );
    maximum = new QRadioButton( tr( "Maximum over system tree (wallclock time)" ), widget );
    maximum->setChecked( false );
    options->addButton( maximum );
    modu_layout->addWidget( maximum );
    modus_selection->setLayout( modu_layout );
    layout->addWidget( modus_selection );

    QGridLayout*  names_layout    = new QGridLayout;
    QGroupBox*    names_selection = new QGroupBox( tr( "Files for selection" ) );
    QButtonGroup* cubenames       = new QButtonGroup();
    profile_cubex = new QRadioButton( tr( "profile.cubex" ), widget );
    profile_cubex->setToolTip( tr( "Select only profile.cubex files (in all subdirectories) to perform scaling analysis" ) );
    profile_cubex->setChecked( true );
    cubenames->addButton( profile_cubex );
    names_layout->addWidget( profile_cubex, 0, 0 );
    summary_cubex = new QRadioButton( tr( "summary.cubex" ), widget );
    summary_cubex->setToolTip( tr( "Select only summary.cubex files (in all subdirectories) to perform scaling analysis" ) );
    summary_cubex->setChecked( false );
    cubenames->addButton( summary_cubex );
    names_layout->addWidget( summary_cubex, 1, 0 );
    scout_cubex = new QRadioButton( tr( "scout.cubex" ), widget );
    scout_cubex->setToolTip( tr( "Select only scout.cubex files (in all subdirectories) to perform scaling analysis" ) );
    scout_cubex->setChecked( false );
    cubenames->addButton( scout_cubex );
    names_layout->addWidget( scout_cubex, 2, 0 );
    trace_cubex = new QRadioButton( tr( "trace.cubex" ), widget );
    trace_cubex->setToolTip( tr( "Select only trace.cubex files (in all subdirectories) to perform scaling analysis" ) );
    trace_cubex->setChecked( false );
    cubenames->addButton( trace_cubex );
    names_layout->addWidget( trace_cubex, 3, 0 );
    custom_cubex = new QRadioButton( tr( "Custom" ), widget );
    custom_cubex->setToolTip( tr( "Select only user-defined (shell wildcards) name of the .cubex files (in all subdirectories) to perform scaling analysis" ) );
    custom_cubex->setChecked( false );
    cubenames->addButton( custom_cubex );
    names_layout->addWidget( custom_cubex, 4, 0 );

    custom_name = new QLineEdit();
    names_layout->addWidget( custom_name, 4, 1 );

    names_selection->setLayout( names_layout );
    layout->addWidget( names_selection );



    layout->addWidget( addCube );
    layout->addWidget( clearList );
    layout->addWidget( scaling );

    scaling->setEnabled( false );
    scaling->setStyleSheet( "padding:4px;text-align: center" );
}


void
CubeScaling::startAction()
{
    std::vector<std::string> options;
    if ( aggregation->isChecked() )
    {
        options.push_back( "aggregate" );
    }



    std::vector<std::string> fileNames;
    for ( int i = 0; i < cubes->count(); i++ )
    {
        fileNames.push_back( cubes->item( i )->text().toStdString() );
    }
    try
    {
        cube = CubeProxy::create( ALGORITHM_SCALING, fileNames, options );
        cube->defineAttribute( "origin", "scaling" ); // mark this cube with an attribure
        service->openCube( cube );
    }
    catch ( const RuntimeError& e )
    {
        QString message = QString( e.what() );
        service->setMessage( message, cubepluginapi::Error );
        return;
    }
}


void
CubeScaling::selectCubes()
{
    QStringList experiments;
    QString     dir = QFileDialog::getExistingDirectory( service->getWidget(), tr( "Open directory with the series of Score-P scaling experiements." ),
                                                         "",
                                                         QFileDialog::ShowDirsOnly
                                                         | QFileDialog::DontResolveSymlinks );
    selectCubes( dir );
}


void
CubeScaling::selectCubes( const QString& dir )
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

    bool matches = false;
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
    scaling->setEnabled( cubes->count() >= 1 );
}



void
CubeScaling::clearCubes()
{
    cubes->clear();
    scaling->setEnabled( cubes->count() >= 1 );
}




void
CubeScaling::closed()
{
}

QString
CubeScaling::getHelpText() const
{
    return tr( "This plugin gathers metrics data from series of scaling measurements and collects them in the resuling Scaling Tree. In couple with \"Jenga Fett\" it allows to study the scaling behaviour of the application." );
}

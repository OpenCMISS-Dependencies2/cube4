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
#include <iomanip>
#include <sstream>
#include <QFileDialog>
#include <QApplication>

#include "MainWidget.h"
#include "CubeError.h"
#include "PluginManager.h"
#include "Globals.h"
#include "PresentationCursor.h"
#include "CubeApplication.h"

using namespace std;
using namespace cubegui;

/** searches in the argumentlist args for an argument in the form -key=value and returns value */
static QString
getArgument( QStringList& args, const QString& key )
{
    foreach( QString arg, args )
    {
        QString expr = "-" + key + "="; // -key=value
        if ( arg.contains( expr ) )
        {
            args.removeAll( arg );
            return arg.replace( expr, "" );
        }
    }
    return QString();
}

// this is the main method
// handles command-line arguments and starts the GUI
int
main( int argc, char* argv[] )
{
    QAction*           startAction = 0;
    cube::CubeStrategy strategy    = cube::CUBE_ALL_IN_MEMORY_STRATEGY;

    bool initGeometry = false;
    for ( int i = 0; i < argc; i++ )
    {
        if ( QString( argv[ i ] ).contains( "geometry" ) )
        {
            initGeometry = true;
            break;
        }
    }

    CubeApplication app( argc, argv );

#if QT_VERSION >= 0x040800
    QTranslator translator;
    // look up e.g. :/share/i18n/cube_[de].qm
    if ( translator.load( QLocale(), QLatin1String( "cube" ), QLatin1String( "_" ), QLatin1String( ":/share/i18n" ) ) )
    {
        app.installTranslator( &translator );
    }
#endif
    cubegui::MainWidget* mainWidget     = NULL;
    bool                 disablePlugins = false;

    QStringList args = app.arguments();
    // set default name for settings
    QCoreApplication::setOrganizationName( "FZJ" );
    QCoreApplication::setApplicationName( "Cube" );

    args.removeAll( "" );

    bool    isPresentation = false;
    QString memory         = getArgument( args, "memory" );
    if ( memory == "preload" )
    {
        strategy = cube::CUBE_ALL_IN_MEMORY_PRELOAD_STRATEGY;
    }
    if ( memory == "lastN" )
    {
        strategy = cube::CUBE_LAST_N_ROWS_STRATEGY;
    }
    if ( args.removeAll( "-disable-plugins" ) > 0 )
    {
        cubegui::PluginManager::disablePlugins();
        disablePlugins = true;
    }
    if ( args.removeAll( "-verbose" ) > 0 )
    {
        Globals::setOption( VerboseMode );
    }
    if ( args.removeAll( "-expert" ) > 0 )
    {
        Globals::setOption( ExpertMode, "true" );
    }
    if ( args.removeAll( "-experimental" ) > 0 )
    {
        Globals::setOption( ExperimentalMode );
    }
    if ( args.removeAll( "-single" ) > 0 ) // single-threaded calculation
    {
        Globals::setOption( Single );
    }
    if ( args.removeAll( "-disable-calculation" ) > 0 )
    {
        Globals::setOption( ManualCalculation );
    }
    if ( args.removeAll( "-presentation" ) > 0 )
    {
        isPresentation = true;
    }

    QString docpath = getArgument( args, "docpath" );
    docpath = docpath.isEmpty() ? QString( PKGDATADIR ) + "doc"  : docpath;
    docpath.append( "/" );
    docpath = QUrl::fromLocalFile( docpath ).toString();

    Globals::setOption( DocPath, docpath );

    QString systemtab = getArgument( args, "systemtab" );

    if ( args.removeAll( "-start" ) > 0 ) // start a context free plugin
    {
        if ( args.size() < 2 )
        {
            std::cout << QObject::tr( "cube -start <pluginname>: plugin name required" ).toUtf8().data() << endl;
            exit( 0 );
        }
        args.takeFirst();
        QString name = args.takeFirst();
        PluginManager::getInstance()->initializePlugins();
        QList<QAction*> actions = PluginManager::getInstance()->getContextFreeActions();
        foreach( QAction * action, actions )
        {
            if ( action->objectName() == name )
            {
                startAction = action;
                PluginManager::getInstance()->setPluginArguments( args );
                break;
            }
        }
    }

    if (  args.size() > 2 || ( args.length() == 2 && args.last().startsWith( "-" ) ) )
    {
        QString help = QCoreApplication::tr( R""(
Usage: cube [options] [-disable-plugins] filename
       cube [options]                    -start <plugin name> [plugin args]

Options:
  -disable-plugins       start cube with all plugins disabled
  -docpath=<path>        set path to documentation directory
  -presentation          opens cube in presentation mode, which shows a mouse icon next to the cursor
  -single                disable parallel execution of cube
  -start <plugin> [args] start context free plugin with the name <plugin>
  -verbose               print detailed information                                            

Developer options:
  -disable-calculation   disable automatic calculation of tree items
  -expert                start cube in expert mode which shows e.g. ghost metrics
  -memory=<strategy>     uses given memory strategy.
                         If the option is omitted, CubeGUI reads the data from the .cubex file at the first access.
                         "preload" reads all data into memory during the initialization phase.
                         "lastN" keeps the last N data rows in memory. N is set via environment CUBE_NUMBER_ROWS.
)"" );

        std::cout << help.toStdString() << endl;
        std::cout << QCoreApplication::tr( "Report bugs to" ).toStdString() << " <" << PACKAGE_BUGREPORT << ">" << std::endl;
        return 0;
    }

    try
    {
        mainWidget = new cubegui::MainWidget( app, strategy );
        app.setMain( mainWidget );

        mainWidget->initialSystemTab = systemtab;
        if ( isPresentation )
        {
            mainWidget->setPresentationMode( true );
        }

        if ( !startAction && ( args.size() == 2 ) )
        {
            QString name = args[ 1 ];

            if ( name.startsWith( ( "cube://" ) ) )
            {
                mainWidget->loadFile( name );
            }
            else
            {
                // if the command line argument is a directory then
                // open a file dialog for getting the file name
                if ( !QFile( name ).exists() || QDir( name ).exists() )
                {
                    name = QFileDialog::getOpenFileName( NULL,
                                                         QCoreApplication::tr( "Choose a file to open" ),
                                                         name,
                                                         QObject::tr( "Cube files" ) + " (*cube *cube.gz *.cubex);;" +
                                                         QObject::tr( "Cube4 files" ) + " (*.cubex);;" +
                                                         QObject::tr( "Cube3 files" ) + " (*.cube.gz *.cube);;" +
                                                         QObject::tr( "All files" ) + " (*.*);;All files (*)"  );
                }
                if ( name.length() > 0 )
                {
                    mainWidget->loadFile( QFileInfo( name ).absoluteFilePath() );
                }
            }
            if ( disablePlugins )
            {
                Globals::setStatusMessage( QCoreApplication::tr( "plugins are disabled..." ) );
            }
        }
        mainWidget->initGeometry( initGeometry );
        mainWidget->show();
        if ( startAction )
        {
            startAction->activate( QAction::Trigger );
        }

        int return_code =  app.exec();

        delete mainWidget;
        return return_code;
    }
    catch ( const cube::RuntimeError& err )
    {
        delete mainWidget;
        std::cerr << err.what() << std::endl;
    }
    return -1;
}

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

#include "LaunchInfo.h"
#include "Launch.h"
#include "Compatibility.h"

using namespace std;
using namespace cubepluginapi;

LaunchInfo::LaunchInfo( PluginServices* service ) : isInitialized_( true )
{
    this->service = service;
}

void
LaunchInfo::replaceVar( QString& str, const QString& var, const QString& value )
{
    QString reg = QString( "([^%])" ).append( var );
#ifdef HAS_QREGULAR_EXPRESSION
    QRegularExpression regExp = QRegularExpression( reg );
#else
    QRegExp regExp = QRegExp( reg );
#endif

    QString replace = QString( "\\1" ).append( value );
    str.replace( reg, replace );
}

/**
 * @brief executes a command and replaces variables
 * @param command command which will be executed
 * @param metricItem
 * @param callItem
 * replace list:
 * %f filename
 * %m absoute metric value
 * %mi metric id
 * %mn metric name
 * %me is metric item expanded
 * %c absolute call path value
 * %ci call path id
 * %cm call path name
 * %ce is call path item expanded?
 */
void
LaunchInfo::launch( const QString& command, TreeItem* metricItem, TreeItem* callItem )
{
    QString cmd( command );
    QString replace;

    // insert values of selected call tree item
    cube::Cnode* cnode = ( cube::Cnode* )( callItem->getCubeObject() );
    replace = QString().number( cnode->get_id() );
    replaceVar( cmd, "%ci", replace );
    replace = callItem->getName();
    replaceVar( cmd, "%cn", replace );
    replace = QString().number( callItem->isExpanded() );
    replaceVar( cmd, "%ce", replace );
    replace = QString().number( callItem->getValue() );
    replaceVar( cmd, "%c", replace );

    launch( cmd, metricItem );
}

void
LaunchInfo::launch( const QString& command, TreeItem* metricItem )
{
    QString cmd( command );
    QString replace;

    // insert values of selected metric tree item
    cube::Metric* metric = ( cube::Metric* )( metricItem->getCubeObject() );
    replace = QString().number( metric->get_id() );
    replaceVar( cmd, "%mi", replace );
    replace = metricItem->getName();
    replaceVar( cmd, "%mn", replace );
    replace = QString().number( metricItem->isExpanded() );
    replaceVar( cmd, "%me", replace );
    replace = QString().number( metricItem->getValue() );
    replaceVar( cmd, "%m", replace );

    // filename
    replaceVar( cmd, "%f", baseFileName );

    // replace variable got from application
    for ( int i = 0; i < launchVarMap.keys().size(); i++ )
    {
        QString name = launchVarMap.keys().at( i );
        QString val  = launchVarMap.value( name );
        replaceVar( cmd, name, val );
    }

    Launch* launcher = new Launch( this ); // will be deleted automatically if process has been finished
    connect( launcher, SIGNAL( receivedVar( QPair<QString, QString>) ),
             this, SLOT( receivedLaunchVar( QPair<QString, QString>) ) );
    launcher->launch( cmd );
}

void
LaunchInfo::launchInit()
{
    if ( !launchInitCommand.isEmpty() )
    {
        Launch* launcher = new Launch( this );
        launcher->launch( launchInitCommand );
        isInitialized_ = true;
    }
}
void
LaunchInfo::launchFinalize()
{
    if ( !launchFinalizeCommand.isEmpty() )
    {
        Launch* launcher = new Launch( this );
        launcher->launch( launchFinalizeCommand );
    }
}

/* generates key for menuMap */
static QString
getMenuKey( cube::Metric const* metric, cube::Cnode const* cnode = 0 )
{
    QString key = QString( metric->get_uniq_name().c_str() );
    if ( cnode != 0 )
    {
        key += "_" + QString::number( cnode->get_id() );
    }
    return key;
}

/* generates key for menuMap */
static QString
getMenuKey( LaunchKey lkey )
{
    if ( lkey.metricId.startsWith( "*" ) )
    {
        return lkey.metricId;
    }
    QString key = lkey.metricId;
    if ( lkey.hasCnode )
    {
        key += "_" + QString::number( lkey.cnodeId );
    }
    return key;
}

/** returns true, if a launch menu entry exists for the given metric (and cnode if given) */
bool
LaunchInfo::existsLaunch( cube::Metric const* metric,
                          cube::Cnode const*  cnode ) const
{
    QString key = getMenuKey( metric, cnode );
    return menuMap.contains( key );
}

/**
   if no cnode is given, all metric menu entries with correspond to the given metric are returned
   otherwise the menu entries of the corresponding cnode are returned
 */
QStringList
LaunchInfo::getMenuEntries( cube::Metric const* metric,
                            cube::Cnode const*  cnode ) const
{
    QString key = getMenuKey( metric, cnode );

    QString                                     all = ( cnode == NULL ) ? "*" : "**";
    QHash<QString, QStringList>::const_iterator it  = menuMap.find( all );
    if ( it != menuMap.end() )
    {
        return *it;
    }
    else
    {
        it = menuMap.find( key );
        if ( it != menuMap.end() )
        {
            return *it;
        }
        else
        {
            return QStringList();
        }
    }
}

QString
LaunchInfo::findLaunchCommand( const QString&      menu,
                               cube::Metric const* metric,
                               cube::Cnode const*  cnode ) const
{
    LaunchKey key;
    key.menuItem = menu;
    key.metricId = QString( metric->get_uniq_name().c_str() );

    if ( cnode != 0 )
    {
        key.cnodeId  = cnode->get_id();
        key.hasCnode = true;
    }
    else
    {
        key.hasCnode = false;
    }

    QString cmd;

    LaunchKey keyAll;
    keyAll.menuItem = menu;
    keyAll.metricId = ( cnode == NULL ) ? "*" : "**";
    keyAll.hasCnode = false;
    LaunchMap::const_iterator it = launchMap.find( keyAll );

    if ( it != launchMap.end() )
    {
        cmd = it.value();
    }
    else
    {
        it = launchMap.find( key );
        if ( it != launchMap.end() )
        {
            cmd = it.value();
        }
    }

    return cmd;
}


namespace
{
void
ReadWhitespaces( istream& theFile )
{
    char check = theFile.peek();
    while ( check == ' ' || check == '\n' )
    {
        theFile.ignore();
        check = theFile.peek();
    }
}
}

void
LaunchInfo::insertMenuMap( const QString& key, const QString& value )
{
    QHash<QString, QStringList>::iterator it;
    it = menuMap.find( key );
    if ( it == menuMap.end() )
    {
        QStringList list;
        list.append( value );
        menuMap.insert( key, list );
    }
    else
    {
        QStringList& list = *it;
        list.append( value );
        menuMap.insert( key, list );
    }
}

/**
 * @brief LaunchInfo::readLaunchFile
 * @param baseName path and filename of the cube input file without suffix
 * @return true if reading was successfull
 * File format:
 * [INIT]
 * <File menu entry>
 * <command to execute>
 *
 * <metric-unique-name>
 * [<context menu entry>
 *  <command to execute>]
 * [- cnode: <id>
 *  <context menu entry>
 *  <command to execute>]
 * *
 * [<context menu entry for all metrics>
 *  <command to execute>]
 * [- cnode: *
 *  <context menu entry for all cnodes>
 *  <command to execute>]
 * [FINALIZE]
 * <command to execute>
 */
bool
LaunchInfo::readLaunchFile( const QString& baseName )
{
    bool ok = true;
    baseFileName = baseName;
    QString fileName = baseName + ".launch";

    ifstream theFile( fileName.toStdString().c_str() );
    if ( !theFile.is_open() )
    {
        ok = false;
        service->debug() << tr( "launch file not found " ) << fileName << Qt::endl;
    }
    else
    {
        service->debug() << tr( "found launch file " ) << fileName << Qt::endl;

        string buffer;
        while ( !theFile.eof() && !theFile.fail() )
        {
            string    str, contextMenu, cmd;
            LaunchKey key;
            QString   command;

            theFile >> str;
            QString metricId = QString( str.c_str() ).trimmed();
            ReadWhitespaces( theFile );

            /* read init or finalize command */
            if ( metricId.startsWith( "[" ) )
            {
                if ( metricId == "[INIT]" )
                {
                    getline( theFile, str );
                    launchInitMenu = QString( str.c_str() ).trimmed();
                    getline( theFile, str );
                    launchInitCommand = QString( str.c_str() ).trimmed();
                    isInitialized_    = false;
                }
                else if ( metricId == "[FINALIZE]" )
                {
                    getline( theFile, str );
                    launchFinalizeCommand = QString( str.c_str() );
                }
                ReadWhitespaces( theFile );
            }
            else // read metric entry
            {    /* Read context menu info which corresponds to this metric.
                    If followed by '-', menus are only available for cnodes.
                  */
                char check = theFile.peek();
                if ( check != '-' && !theFile.eof() )
                {
                    getline( theFile, contextMenu );
                    getline( theFile, cmd );
                    command      = QString( cmd.c_str() );
                    key.menuItem = QString( contextMenu.c_str() );
                    key.metricId = metricId;
                    key.hasCnode = false;
                    launchMap.insert( key, command );

                    insertMenuMap( getMenuKey( key ), key.menuItem );
                }
                ReadWhitespaces( theFile );
                check = theFile.peek();
                while ( check == '-' && !theFile.eof() )
                {
                    int nr;
                    theFile >> buffer;    // -
                    theFile >> buffer;    // cnode
                    theFile >> nr;
                    if ( theFile.fail() ) // cnode == "*" ?
                    {
                        theFile.clear();
                        theFile >> buffer;
                        QString star( buffer.c_str() );
                        if ( star.trimmed() == "*" )
                        {
                            metricId = "**";
                        }
                    }
                    ReadWhitespaces( theFile );
                    getline( theFile, contextMenu );
                    getline( theFile, cmd );
                    command      = QString( cmd.c_str() );
                    key.menuItem = QString( contextMenu.c_str() );
                    key.metricId = metricId;
                    key.cnodeId  = nr;
                    key.hasCnode = true;
                    if ( metricId == "**" )
                    {
                        key.hasCnode = false;
                    }
                    launchMap.insert( key, command );
                    cnodes.append( key.cnodeId );
                    if ( key.hasCnode )
                    {
                        cnodeMetrics.insert( key.metricId );
                    }

                    insertMenuMap( getMenuKey( key ), key.menuItem );

                    ReadWhitespaces( theFile );
                    check = theFile.peek();
                }
            } // else read metric entry
        }
    }
    if ( launchMap.size() == 0 )
    {
        ok = false;
    }

    return ok;
}

void
LaunchInfo::receivedLaunchVar( QPair<QString, QString> val )
{
    // TODO: add menuLabel to key?
    launchVarMap.insert( val.first, val.second );
}


uint
qHash( const LaunchKey& k )
{
    return qHash( k.toString() );
}

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

#include "Launch.h"
#include <QDebug>
#include <iostream>
#include "PluginServices.h"
#if QT_VERSION < QT_VERSION_CHECK( 5, 14, 0 )
constexpr QString::SplitBehavior SkipEmptyParts = QString::SkipEmptyParts;
#else
constexpr Qt::SplitBehaviorFlags SkipEmptyParts = Qt::SkipEmptyParts;
#endif


/** executes the given command cmd and writes arguments to stdout */
void
Launch::launch( const QString& cmdline )
{
    QStringList args = cmdline.split( " ", SkipEmptyParts );
    QString     cmd  = args.takeFirst();

    if ( cmd.isEmpty() )
    {
        return;
    }
    connect( this, SIGNAL( readyReadStandardOutput() ), this, SLOT( readStdout() ) );
    connect( this, SIGNAL( error( QProcess::ProcessError ) ), this, SLOT( error( QProcess::ProcessError ) ) );
    connect( this, SIGNAL( finished( int ) ), this, SLOT( launchFinished( int ) ) );
    this->start( cmd, args );
}

void
Launch::launchFinished( int exitCode )
{
    if ( exitCode != 0 )
    {
        std::cerr << tr( "LaunchPlugin: execution finished with error: " ).toUtf8().data() << exitCode << std::endl;
    }
    this->deleteLater();
}

void
Launch::error( QProcess::ProcessError err )
{
    std::cerr << tr( "LaunchPlugin: launch execution finished with error: " ).toUtf8().data() << err << std::endl;
}

void
Launch::readStdout()
{
    char   buf[ 1024 ];
    qint64 lineLength;
    do
    {
        lineLength = this->readLine( buf, sizeof( buf ) );
        if ( lineLength != -1 )
        {
            if ( ( buf[ 0 ] == '%' ) && ( isalpha( buf[ 1 ] ) ) && ( buf[ 2 ] == '=' ) )
            {
                QString str = QString( &buf[ 3 ] );
                QString var = QString( '%' ).append( buf[ 1 ] );
                QString val = str.trimmed();
                emit    receivedVar( QPair<QString, QString>( var, val ) );
            }
        }
    }
    while ( lineLength > 0 );
}

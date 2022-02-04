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


#include <config.h>

#include <iostream>
#include <QTextStream>
#include <QDebug>
#include "CubePlugin.h"
#include "PluginServices.h"
#include "CubeServices.h"
#include "ScorePConfigVariables.h"

#include "Compatibility.h"
#ifdef HAS_QREGULAR_EXPRESSION
#include <QRegularExpression>
#else
#include <QRegExp>
#endif

#if QT_VERSION < QT_VERSION_CHECK( 5, 14, 0 )
constexpr QString::SplitBehavior SkipEmptyParts = QString::SkipEmptyParts;
#else
constexpr Qt::SplitBehaviorFlags SkipEmptyParts = Qt::SkipEmptyParts;
#endif

// enum ScorePInfoState { SCOREP_INFO_VAR_NAME=0,  SCOREP_INFO_VAR_DESCRPTION };

using namespace scorepconfigplugin;

#if QT_VERSION < QT_VERSION_CHECK( 5, 0, 0 )
ScorePConfigVariables::ScorePConfigVariables()
{
    none = "";
    QFile file( ":/plugins/ScorePConfig/confvars.html" );
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        return;
    }

    QTextStream in( &file );
    QString     line  = in.readAll();
    QStringList lines = line.split( "\n", SkipEmptyParts );

    QRegExp liststart( "^\\s*<dl>\\s*$" );
    QRegExp listend( "^\\s*</dl>\\s*$" );
    QRegExp terminator( "^\\s*</dd>\\s*$" );
    QRegExp varname( "^\\s*<dt>\\s*@anchor\\s+[\\w_]+<tt>([\\w_]+)</tt></dt>$" );

    QStringList::const_iterator constIterator;
    constIterator = lines.constBegin();
    ++constIterator; // skip first line
    for (; constIterator != lines.constEnd();
         ++constIterator )
    {
        QString name = ( *constIterator );
        varname.indexIn( name );
        QStringList list = varname.capturedTexts();
        name = list[ 1 ];

        ++constIterator;  // go to next lines
        if ( constIterator == lines.constEnd() )
        {
            break;
        }
        QString description;
        QString chunk;
        do
        {
            chunk = *constIterator;
            description.append( chunk );

            if ( liststart.exactMatch( chunk ) ) // we have to ignore </dd> if we are wthin a list... extra loop hence
            {
                while ( !listend.exactMatch( chunk ) )
                {
                    ++constIterator; // next line;
                    chunk = *constIterator;
                    description.append( chunk );
                }
            }
            else
            {
                ++constIterator; // next line;
            }
        }
        while ( !terminator.exactMatch( chunk ) );
        help[ name ] = description;
    }
}
#else
ScorePConfigVariables::ScorePConfigVariables()
{
    none = "";
    QFile file( ":/plugins/ScorePConfig/confvars.html" );
    if ( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        return;
    }

    QTextStream in( &file );
    QString     line  = in.readAll();
    QStringList lines = line.split( "\n", SkipEmptyParts );

    QRegularExpression liststart( "^\\s*<dl>\\s*$" );
    QRegularExpression listend( "^\\s*</dl>\\s*$" );
    QRegularExpression terminator( "^\\s*</dd>\\s*$" );
    QRegularExpression varname( "^\\s*<dt>\\s*@anchor\\s+[\\w_]+<tt>([\\w_]+)</tt></dt>$" );

    QStringList::const_iterator constIterator;
    constIterator = lines.constBegin();
    ++constIterator; // skip first line
    for (; constIterator != lines.constEnd();
         ++constIterator )
    {
        QString name = ( *constIterator );

        QRegularExpressionMatch endMatch;
        QRegularExpressionMatch match = varname.match( name );
        if ( match.capturedTexts().size() < 2 )
        {
            continue;
        }
        name = match.capturedTexts().at( 1 );

        ++constIterator;  // go to next lines
        if ( constIterator == lines.constEnd() )
        {
            break;
        }
        QString description;
        QString chunk;
        do
        {
            chunk = *constIterator;
            description.append( chunk );

            match = liststart.match( chunk );
            if ( match.lastCapturedIndex() == 0 ) // we have to ignore </dd> if we are wthin a list... extra loop hence
            {
                match = listend.match( chunk );
                while ( match.lastCapturedIndex() != 0 )
                {
                    ++constIterator; // next line;
                    chunk = *constIterator;
                    description.append( chunk );
                    match = listend.match( chunk );
                }
            }
            else
            {
                ++constIterator; // next line;
            }
            endMatch = terminator.match( chunk );
        }
        while ( endMatch.lastCapturedIndex() != 0 );
        help[ name ] = description;
    }
}
#endif


const
QString&
ScorePConfigVariables::getHelp( const QString& var ) const
{
    QMap<QString, QString>::const_iterator i = help.find( var );
    while ( i != help.end() && i.key() == var )
    {
        return i.value();
    }
    return none;
}

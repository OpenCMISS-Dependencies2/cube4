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
#include "Statistics.h"

#include "Environment.h"
#include "CubeMetric.h"
#include "CubeCnode.h"
#include "CubeServices.h"
#include "StatisticalInformation.h"

#include <math.h>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <limits>

#include <iostream>

using namespace std;
using namespace cubepluginapi;
using cubegui::StatisticalInformation;

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

Statistics::SevereEvent::SevereEvent( ifstream& theFile )
{
    string buffer;
    // reads "cnode:"
    theFile >> buffer;

    theFile >> cnode_id;

    // reads "enter:"
    theFile >> buffer;

    theFile >> enter;

    // reads "exit:"
    theFile >> buffer;

    theFile >> exit;

    // reads "duration:" (actually severity)
    theFile >> buffer;

    theFile >> wastedTime;

    theFile >> buffer;
    theFile >> rank;
    if ( theFile.fail() )
    {
        theFile.clear(); // ignore rank, if not available
        rank = -1;
    }

    // ignore rest of line
    theFile.ignore( std::numeric_limits<int>::max(), '\n' );
}


Statistics::Statistics( PluginServices* service ) : verbose( false )
{
    if ( char* value = getenv( "CUBE_VERBOSE" ) )
    {
        verbose = cubegui::env_str2bool( value );
    }

    this->service = service;
    string cubeFileName = service->getCube()->getAttribute( "cubename" );
    string statFileName = service->getCube()->getAttribute( "statisticfile" );

    string statisticsFileName = "";
    if ( statFileName != "" )
    {
        statisticsFileName =  cube::services::dirname( cubeFileName ) + "/" +  statFileName;
    }
    else
    {
        statisticsFileName = cube::services::get_cube_name( cubeFileName ) + ".stat";
    }
    fileName = QString::fromStdString( statisticsFileName );


    ifstream theFile( statisticsFileName.c_str() );
    if ( !theFile.is_open() )
    {
        fileExists = false;
    }
    else
    {
        if ( verbose )
        {
            cout << QObject::tr( "Found statistics file " ).toUtf8().data() << statisticsFileName.c_str() << endl;
        }
        fileExists = true;
        string buffer;
        // read the title line
        getline( theFile, buffer );

        while ( !theFile.eof() && !theFile.fail() )
        {
            string name;
            theFile >> name;
            severeEventsType::iterator it = severeEvents.insert( make_pair( name,
                                                                            make_pair( StatisticalInformation( theFile ),
                                                                                       vector<SevereEvent>() ) ) ).first;
            ReadWhitespaces( theFile );
            char check = theFile.peek();
            while ( check == '-' && !theFile.eof() )
            {
                theFile >> buffer;
                it->second.second.push_back( SevereEvent( theFile ) );
                ReadWhitespaces( theFile );
                check = theFile.peek();
            }
        }
    }
}

bool
Statistics::existsStatFile() const
{
    return fileExists;
}

QString
Statistics::getStatFileName() const
{
    return fileName;
}

Statistics::~Statistics()
{
}

bool
Statistics::existsStatistics( cube::Metric const* metric ) const
{
    severeEventsType::const_iterator it = severeEvents.find( metric->get_uniq_name() );
    if ( it != severeEvents.end() )
    {
        return true;
    }

    return false;
}


QStringList
Statistics::getStatistics( cube::Metric const* metric ) const
{
    severeEventsType::const_iterator it = severeEvents.find( metric->get_uniq_name() );
    if ( it != severeEvents.end() )
    {
        return it->second.first.print( it->first );
    }

    return QStringList( "" ) << "";
}

StatisticalInformation
Statistics::getInfo( cube::Metric const* metric ) const
{
    severeEventsType::const_iterator it = severeEvents.find( metric->get_uniq_name() );
    if ( it != severeEvents.end() )
    {
        StatisticalInformation const& info = it->second.first;
        const std::string&            name = it->first;
        return StatisticalInformation( info.getCount(), info.getSum(),
                                       info.getMean(), info.getMinimum(), info.getQ1(), info.getMedian(), info.getQ3(),
                                       info.getMaximum(), info.getVariance(), name, metric->get_uom() );
    }
    throw logic_error( QObject::tr( "No statistic information for the requested metric." ).toUtf8().data() );
}

uint32_t
Statistics::findMaxSeverityId( cube::Metric const* metric ) const
{
    SevereEvent const* event = findMaxSeverity( metric, 0 );
    assert( event != 0 );
    return event->cnode_id;
}

Statistics::SevereEvent const*
Statistics::findMaxSeverity( cube::Metric const* metric,
                             cube::Cnode const*  cnode ) const
{
    severeEventsType::const_iterator it = severeEvents.find( metric->get_uniq_name() );
    if ( it != severeEvents.end() )
    {
        const vector<SevereEvent>& vec = it->second.second;
        if ( vec.size() == 0 )
        {
            return 0;
        }
        if ( cnode == 0 )
        {
            SevereEvent const* max_elem = &( *vec.begin() );
            for ( vector<SevereEvent>::const_iterator it2
                      = vec.begin(); it2 != vec.end(); ++it2 )
            {
                if ( it2->wastedTime > max_elem->wastedTime )
                {
                    max_elem = &( *it2 );
                }
            }
            return max_elem;
        }
        else
        {
            for ( vector<SevereEvent>::const_iterator it2
                      = vec.begin(); it2 != vec.end(); ++it2 )
            {
                if ( it2->cnode_id == ( uint32_t )( cnode->get_id() ) )
                {
                    return &( *it2 );
                }
            }
            return 0;
        }
    }

    return 0;
}

bool
Statistics::existsMaxSeverity( cube::Metric const* metric,
                               cube::Cnode const*  cnode ) const
{
    return findMaxSeverity( metric, cnode ) != 0;
}

bool
Statistics::existsMaxSeverity( const cube::Metric* metric, const cube::Cnode* cnode, double& enter, double& exit ) const
{
    const SevereEvent* event = findMaxSeverity( metric, cnode );
    if ( event )
    {
        enter = event->enter;
        exit  = event->exit;
    }
    return event != 0;
}

#include "TableWidget.h"
void
Statistics::showMaxSeverityText( QWidget*            parent,
                                 const QString&      label,
                                 cube::Metric const* metric,
                                 cube::Cnode const*  cnode ) const
{
    SevereEvent const* sevent = findMaxSeverity( metric, cnode );
    assert( sevent != 0 );

    QString left, right;
    left  += QObject::tr( "enter:\n" );
    left  += QObject::tr( "exit:\n" );
    left  += QObject::tr( "duration:\n" );
    left  += QObject::tr( "severity:\n" );
    right += service->formatNumber( sevent->enter ) + "\n";
    right += service->formatNumber( sevent->exit ) + "\n";
    right += service->formatNumber( sevent->exit - sevent->enter ) + "\n";
    right += service->formatNumber( sevent->wastedTime ) + "\n";
    if ( sevent->rank >= 0 )
    {
        left  += QObject::tr( "rank:" );
        right += service->formatNumber( sevent->rank, true );
    }
    cubegui::TableWidget::showInWindow( parent, QObject::tr( "Max severity" ), label, QStringList() << left << right );
}

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

#include "StatisticalInformation.h"
#include "Globals.h"
#include "Environment.h"
#include "CubeMetric.h"
#include "CubeCnode.h"
#include "CubeServices.h"

#include <math.h>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <limits>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <QDebug>

using namespace std;
using namespace cubegui;

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

StatisticalInformation::StatisticalInformation( ifstream& theFile )
{
    mean = median = minimum = maximum = sum = variance = q1 = q3 = 0;
    theFile >> count;
    ReadWhitespaces( theFile );
    if ( theFile.peek() != '-' )
    {
        theFile >> mean;
        theFile >> median;
        theFile >> minimum;
        theFile >> maximum;
        theFile >> sum;
        if ( count >= 2 )
        {
            theFile >> variance;
        }
        if ( count >= 5 )
        {
            theFile >> q1;
            theFile >> q3;
        }
        statInformation = true;
    }
    else
    {
        statInformation = false;
    }

    checkValidity();
}

StatisticalInformation::StatisticalInformation()
{
    count           = 0;
    sum             = 0;
    mean            = 0;
    minimum         = 0;
    q1              = 0;
    median          = 0;
    q3              = 0;
    maximum         = 0;
    variance        = 0;
    name            = "";
    uom             = "";
    statInformation = false;
    valid_values    = false;
    data_status_description.clear();
}


StatisticalInformation::StatisticalInformation( int         theCount,
                                                double      theSum,
                                                double      theMean,
                                                double      min,
                                                double      q25,
                                                double      theMedian,
                                                double      q75,
                                                double      max,
                                                double      theVariance,
                                                std::string theName,
                                                std::string theUom )
    : count( theCount ), sum( theSum ), mean( theMean ), minimum( min ), q1( q25 ),
    median( theMedian ), q3( q75 ), maximum( max ), variance( theVariance ), data_status_description(),
    name( theName ), uom( theUom )
{
    checkValidity();
    statInformation = true;
}


StatisticalInformation::StatisticalInformation( vector<double>& data )

{
    count    = 0;
    sum      = 0;
    mean     = 0;
    minimum  = 0;
    q1       = 0;
    median   = 0;
    q3       = 0;
    maximum  = 0;
    variance = 0;
    name     = "";
    uom      = "";

    if ( data.size() == 0 )
    {
        return;
    }

    for ( unsigned i = 0; i < data.size(); i++ )
    {
        sum += data.at( i );
    }
    mean     = sum / data.size();
    count    = data.size();
    variance = 0.;
    for ( unsigned i = 0; i < data.size(); i++ )
    {
        variance += ( data.at( i ) - mean ) * ( data.at( i ) - mean );
    }
    variance /= ( data.size() );

    sort( data.begin(), data.end() );
    minimum = *data.begin();
    maximum = *( data.end() - 1 );
    median  = *( data.begin() + data.size() / 2 );
    q1      = *( data.begin() + data.size() / 4 );
    q3      = *( data.begin() + data.size() * 3 / 4 );

    valid_values    = true;
    statInformation = true;
}

/**
 * Performs a sequence of checks, whether statistical data looks meaningfull or not.
 */
void
StatisticalInformation::checkValidity()
{
    data_status_description.clear();
    valid_values = true;
    if ( minimum > maximum )
    {
        data_status_description.append( QObject::tr( "Min value is larger than Max." ) );
        valid_values = false;                   // the world is up side down
    }
    if ( mean > maximum && mean < minimum )
    {
        data_status_description.append( QObject::tr( "Mean value is out of range [minimum, maximum]." ) );
        valid_values = false;
    } // mean is out of range
    if ( median > maximum && median < minimum )
    {
        data_status_description.append( QObject::tr( "Median value is out of range [minimum, maximum]." ) );
        valid_values = false;
    } // median is out of range
    if ( maximum * count  < sum )
    {
        data_status_description.append( QObject::tr( "Sum is larger than \"count * maximum\"." ) );
        valid_values = false;
    } // sum is bigger that count times maximum
    if ( ( minimum * count )  > sum )
    {
        data_status_description.append( QObject::tr( "Sum is smaller than \"count * minimum\"." ) );
        valid_values = false;
    } // sum is lower that count times minimum
    if ( count >= 2 )
    {
        if ( variance < 0 )
        {
            data_status_description.append( QObject::tr( "Variance is negative." ) );
            valid_values = false;
        } // varianvve cannot be negativ

        if ( count >= 5 )
        {
            if ( q1 > q3 )
            {
                data_status_description.append( QObject::tr( "25% quantile is larger than 75% quantile." ) );
                valid_values = false;
            } // quantile 25% is bigger than 75%
            if ( median > q3 )
            {
                data_status_description.append( QObject::tr( "Medium (50% quantile) is larger than 75% quantile." ) );
                valid_values = false;
            } // definition of medium : 50% and it cannot be bigger than 75%
            if ( median < q1 )
            {
                data_status_description.append( QObject::tr( "Medium (50% quantile) is smaller than 25% quantile." ) );
                valid_values = false;
            } // definition of medium : 50% and it cannot be smaller  than 25%
        }
    }
}

int
StatisticalInformation::getCount() const
{
    return count;
}

double
StatisticalInformation::getVariance() const
{
    return variance;
}

QStringList
StatisticalInformation::getDataStatusDescription() const
{
    return data_status_description;
}

std::string
StatisticalInformation::getName() const
{
    return name;
}

bool
StatisticalInformation::isValid() const
{
    return valid_values;
}

double
StatisticalInformation::getMaximum() const
{
    return maximum;
}

double
StatisticalInformation::getQ3() const
{
    return q3;
}

double
StatisticalInformation::getMedian() const
{
    return median;
}

double
StatisticalInformation::getQ1() const
{
    return q1;
}

double
StatisticalInformation::getMinimum() const
{
    return minimum;
}

double
StatisticalInformation::getMean() const
{
    return mean;
}

double
StatisticalInformation::getSum() const
{
    return sum;
}


#define OUTPUT_PERCENT( P ) ( fabs( maximum ) > 10e-6 ) ? ( percentStream << " " << ( int )( ( P ) * 100 / maximum + .5 ) << "%" << endl ) : percentStream << endl;
QStringList
StatisticalInformation::print( string const& patternName, string const uom, PrecisionFormat format ) const
{
    stringstream percentStream;
    stringstream theStream;
    string       captionString;
    if ( patternName.length() > 0 )
    {
        captionString += QObject::tr( "Pattern: \n" ).toUtf8().data();
        theStream << patternName.c_str() << endl;
        percentStream << endl;
    }
    if ( valid_values )
    {
        if ( statInformation )
        {
            captionString += QObject::tr( "Sum: \n" ).toUtf8().data();
            captionString += QObject::tr( "Count: \n" ).toUtf8().data();
            captionString += QObject::tr( "Mean: \n" ).toUtf8().data();
            theStream << Globals::formatNumber( sum, false, format ).toStdString().c_str();
            if ( uom.length() != 0 )
            {
                theStream << " " << uom.c_str();
            }
            theStream << endl;
            theStream << Globals::formatNumber( count, true, format ).toStdString().c_str() << endl;
            theStream << Globals::formatNumber( mean, false, format ).toStdString().c_str();
            if ( uom.length() != 0 )
            {
                theStream << " " << uom.c_str();
            }
            theStream << endl;
            percentStream << endl << endl;
            OUTPUT_PERCENT( mean );

            if ( count >= 2 )
            {
                captionString += QObject::tr( "Standard deviation: \n" ).toUtf8().data();
                theStream << Globals::formatNumber( sqrt( variance ), false, format ).toStdString().c_str();
                if ( uom.length() != 0 )
                {
                    theStream << " " << uom.c_str();
                }
                theStream << endl;
                OUTPUT_PERCENT( sqrt( variance ) );
            }

            captionString += QObject::tr( "Maximum: \n" ).toUtf8().data();
            theStream << Globals::formatNumber( maximum, false, format ).toStdString().c_str();
            if ( uom.length() != 0 )
            {
                theStream << " " << uom.c_str();
            }
            theStream << endl;
            OUTPUT_PERCENT( maximum );

            if ( count >= 5 )
            {
                captionString += QObject::tr( "Upper quartile (Q3): \n" ).toUtf8().data();
                theStream << Globals::formatNumber( q3, false, format ).toStdString().c_str();
                if ( uom.length() != 0 )
                {
                    theStream << " " << uom.c_str();
                }
                theStream << endl;
                OUTPUT_PERCENT( q3 );
            }
            captionString += QObject::tr( "Median: \n" ).toUtf8().data();
            theStream << Globals::formatNumber( median, false, format ).toStdString().c_str();
            if ( uom.length() != 0 )
            {
                theStream << " " << uom.c_str();
            }
            theStream << endl;
            OUTPUT_PERCENT( median );
            if ( count >= 5 )
            {
                captionString += QObject::tr( "Lower quartile (Q1): \n" ).toUtf8().data();
                theStream << Globals::formatNumber( q1, false, format ).toStdString().c_str();
                if ( uom.length() != 0 )
                {
                    theStream << " " << uom.c_str();
                }
                theStream << endl;
                OUTPUT_PERCENT( q1 );
            }
            captionString += QObject::tr( "Minimum: " ).toUtf8().data();
            theStream << Globals::formatNumber( minimum, false, format ).toStdString().c_str();
            if ( uom.length() != 0 )
            {
                theStream << " " << uom.c_str();
            }
            OUTPUT_PERCENT( minimum );
        }
        else
        {
            captionString += QObject::tr( "Count: \n" ).toUtf8().data();
            theStream << Globals::formatNumber( count, true, format ).toStdString().c_str() << endl;
            percentStream << endl;
        }
    }
    else
    {
        theStream << QObject::tr( "Statistical data seems to be bogus. Several errors are found (listed below): \n" ).toUtf8().data() << endl
                  << data_status_description.join( "\n" ).toUtf8().data() << endl;
    }
    QStringList list;
    list.append( QString::fromStdString( captionString ) );
    list.append( QString::fromStdString( theStream.str() ) );
    list.append( QString::fromStdString( percentStream.str() ) );
    return list;
}

/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 1998-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  Copyright (c) 2009-2015                                                **
**  German Research School for Simulation Sciences GmbH,                   **
**  Laboratory for Parallel Programming                                    **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/



#include "config.h"

#include "AbstractRulerController.h"
#include <QPainter>
#include <QtCore/qmath.h>
#include <QDebug>
#include <QDateTime>

const int AbstractRulerController::BEST_CUT_LENGTH_DIVIDER   = 4;
const int AbstractRulerController::MINOR_STEPS_DEFAULT_COUNT = 0;

AbstractRulerController::AbstractRulerController( QWidget* parent ) :
    QObject( parent )
{
    this->isRealValuedNotches = true;
    initComponents();
}

AbstractRuler*
AbstractRulerController::getView()
{
    return view;
}

void
AbstractRulerController::initComponents()
{
    majorNotchesCustomCount   = 1;
    minorNotchesCount         = 1;
    isMajorNotchesCountCustom = true;
    graphOnDisplay            = true;
}

void
AbstractRulerController::setConnections()
{
    QObject::connect( view, SIGNAL( sizeChanged() ), this, SLOT( sizeChangedHandler() ) );
}

AbstractRulerController::~AbstractRulerController()
{
}
void
AbstractRulerController::setCalibrateStatus( bool tempCalibrateStatus )
{
    graphOnDisplay = tempCalibrateStatus;
}

void
AbstractRulerController::setLimits( int minimumLimit, int maximumLimit )
{
    this->minimumLimit = ( double )minimumLimit;
    this->maximumLimit = ( double )maximumLimit;
    view->setLimits( minimumLimit, maximumLimit );
    if ( minimumLimit == -1 && maximumLimit == -1 ) // no limit
    {
        this->isRealValuedNotches = true;
        view->clearMajorNotchesIndexAndLocation();
        view->clearMinorNotchesIndexAndLocation();
        view->update();
    }
    else
    {
        this->isRealValuedNotches = false;
        calibrateAllNotches();
    }
}

void
AbstractRulerController::setLimitsF( double minimumLimit, double maximumLimit )
{
    this->minimumLimit = minimumLimit;
    this->maximumLimit = maximumLimit;
    view->setLimits( minimumLimit, maximumLimit );
    if ( minimumLimit == -1.0 && maximumLimit == -1.0 ) // no limit
    {
        this->isRealValuedNotches = true;
        view->clearMajorNotchesIndexAndLocation();
        view->clearMinorNotchesIndexAndLocation();
        view->update();
    }
    else
    {
        this->isRealValuedNotches = true;
        calibrateAllNotches();
    }
}

void
AbstractRulerController::setMajorAndMinorNotchesToDefault()
{
    int rulerLength   = view->getRulerLength();
    int bestCutLength = bestCutLengthHeuristic( minimumLimit, maximumLimit, rulerLength );
    setMinorNotchesCount( MINOR_STEPS_DEFAULT_COUNT );
    if ( bestCutLength == 0 )
    {
        qDebug() << tr( "Unexptected: bestCutLength(" ) << minimumLimit << "," << maximumLimit << "," << rulerLength << tr( ") returned 0." ) <<
            tr( " overriding major notches count to 4" );
        setMajorNotchesCount( 4 );
        return;
    }
    setMajorNotchesCount( rulerLength / bestCutLength - 1 );
}

void
AbstractRulerController::setMajorNotchesCount( int majorNotchesCount )
{
    isMajorNotchesCountCustom = true;
    // +1 because the number of actual major notches (set by user) is between the boundaries
    majorNotchesCustomCount = majorNotchesCount + 1;
    calibrateAllNotches();
}

void
AbstractRulerController::setMajorNotchesInterval( int intervalSize )
{
    // if limits are integer or doubles then nothing changes.
    // compare to doubleing-point version
    isMajorNotchesCountCustom                    = false;
    majorNotchesIntervalSizeInProcessesOrPixelsF = ( double )intervalSize;
    calibrateAllNotches();
}


void
AbstractRulerController::setMajorNotchesIntervalF( double intervalSize )
{
    // if limits are doubleing-point then nothing changes
    // if limits are integers they are converted to doubles
    isMajorNotchesCountCustom                    = false;
    majorNotchesIntervalSizeInProcessesOrPixelsF = intervalSize;
    this->isRealValuedNotches                    = true;
    calibrateAllNotches();
}

void
AbstractRulerController::setMinorNotchesCount( int minorNotchesCount )
{
    // +1 is added because the calculation assumes we have
    // another minor notch in the position of the next major notch
    this->minorNotchesCount = minorNotchesCount + 1;
    calibrateAllNotches();
}

bool
AbstractRulerController::isMajorNotchesUsingIntervals()
{
    return !isMajorNotchesCountCustom;
}

int
AbstractRulerController::getMajorNotchesCount()
{
    return majorNotchesCustomCount - 1;
}

int
AbstractRulerController::getMinorNotchesCount()
{
    return minorNotchesCount - 1;
}

int
AbstractRulerController::getMajorNotchesInterval()
{
    return majorNotchesIntervalSizeInProcessesOrPixelsF;
}

void
AbstractRulerController::setTitle( QString title, plotWidget::Alignment align )
{
    view->setTitle( title, align );
}

QString
AbstractRulerController::getTitle()
{
    return view->getTitle();
}

void
AbstractRulerController::setTitleVisible( bool isVisible )
{
    view->setTitleVisible( isVisible );
}

void
AbstractRulerController::forceIndexAtMousePositionChangedSignal( QMouseEvent* mouseIndex )
{
    view->forceIndexAtMousePositionChangedSignal( mouseIndex );
}
void
AbstractRulerController::calibrateAllNotches( double rulerLength )
{
    if ( graphOnDisplay == false ) // means no graph
    {
        return;
    }

    if ( ( getMinorNotchesCount() == -1 ) ||
         ( !isMajorNotchesUsingIntervals() && getMajorNotchesCount() == -1 ) ||
         ( isMajorNotchesUsingIntervals() && getMajorNotchesInterval() == -1 ) )
    {
        return;
    }
    // int rulerLength = view->getRulerLength();
    if ( rulerLength <= 0 )
    {
        return;
    }

    // size of major notch in pixels
    int majorNotchStepSizeInPixels;
    // corrected size of major notch in pixels
    int correctedMajorNotchStepSizeInPixels;
    // size of a minor notch in pixels
    int minorNotchStepSizeInPixels;
    // corrected size of a minor notch in pixels
    int correctedMinorNotchStepSizeInPixels;
    int keepRemaining = 0;
    if ( isMajorNotchesCountCustom )  // there is not much choice
    {
        majorNotchStepSizeInPixels          = rulerLength / majorNotchesCustomCount;
        keepRemaining                       = int( rulerLength ) % ( majorNotchesCustomCount );
        correctedMajorNotchStepSizeInPixels = correctStepSize( rulerLength, majorNotchStepSizeInPixels );
        if ( correctedMajorNotchStepSizeInPixels == -1 )
        {
            return;
        }
    }
    else
    {
        //  majorNotchesIntervalSizeInProcessesOrPixelsF++;
        //   majorNotchStepSizeInPixels = (int)qFloor(((majorNotchesIntervalSizeInProcessesOrPixelsF) * rulerLength)/((maximumLimit - minimumLimit)/majorNotchesIntervalSizeInProcessesOrPixelsF));
        majorNotchStepSizeInPixels = ( int )qFloor( ( rulerLength ) / ( ( maximumLimit - minimumLimit ) / majorNotchesIntervalSizeInProcessesOrPixelsF ) );
        //    keepRemaining = (int)qFloor((rulerLength) mod ((maximumLimit - minimumLimit)/majorNotchesIntervalSizeInProcessesOrPixelsF));
        keepRemaining = int( fmod( rulerLength, ( ( maximumLimit - minimumLimit ) / majorNotchesIntervalSizeInProcessesOrPixelsF ) ) );
        // the corrected size is the same as size because we do not want to distribute the notches evenly along the ruler
        correctedMajorNotchStepSizeInPixels = majorNotchStepSizeInPixels;
    }

    minorNotchStepSizeInPixels = majorNotchStepSizeInPixels / minorNotchesCount;

    correctedMinorNotchStepSizeInPixels = correctStepSize( majorNotchStepSizeInPixels, minorNotchStepSizeInPixels );
    if ( correctedMinorNotchStepSizeInPixels == -1 )
    {
        return;
    }

    // the number of major notches
    int majorStepsCount;
    // the number of minor notches
    int minorStepsCount;
    if ( isMajorNotchesCountCustom )
    {
        majorStepsCount                              = rulerLength / correctedMajorNotchStepSizeInPixels;
        majorNotchesIntervalSizeInProcessesOrPixelsF = ( maximumLimit - minimumLimit ) / ( double )majorStepsCount;
    }
    else
    {
        majorStepsCount = ( rulerLength + majorNotchStepSizeInPixels ) / majorNotchStepSizeInPixels;
        // majorStepsCount = (maximumLimit - minimumLimit)/majorNotchesIntervalSizeInProcessesOrPixelsF;
    }

    minorStepsCount = correctedMajorNotchStepSizeInPixels / correctedMinorNotchStepSizeInPixels;
    double            minorNotchesIntervalSizeInProcessesOrPixelsF = majorNotchesIntervalSizeInProcessesOrPixelsF / ( double )minorStepsCount;
    QMap<double, int> majorNotchesIndexAndLocationF;
    QMap<double, int> minorNotchesIndexAndLocationF;

    majorNotchesIndexAndLocationF.insert( minimumLimit, 0 );

    int i;
    int tempkeepRemaining = 0;
    for ( i = 1; i <= majorStepsCount; i++ )
    {
        double majorNotchIndexF = i * majorNotchesIntervalSizeInProcessesOrPixelsF + minimumLimit;
        if ( majorNotchIndexF > maximumLimit )
        {
            break;
        }
        // ___ distribute reminder
        if ( keepRemaining > 0 )
        {
            tempkeepRemaining++;
            majorNotchesIndexAndLocationF.insert( majorNotchIndexF, ( i * correctedMajorNotchStepSizeInPixels ) + tempkeepRemaining );
            keepRemaining--;
        }
        else
        {
            majorNotchesIndexAndLocationF.insert( majorNotchIndexF, ( i * correctedMajorNotchStepSizeInPixels ) + tempkeepRemaining );
        }

        for ( int j = 1; j < minorStepsCount; j++ )
        {
            if ( j * correctedMinorNotchStepSizeInPixels > rulerLength )
            {
                break;
            }
            else
            {
                minorNotchesIndexAndLocationF.insert( j * minorNotchesIntervalSizeInProcessesOrPixelsF + ( i - 1 ) * majorNotchesIntervalSizeInProcessesOrPixelsF + minimumLimit, j * correctedMinorNotchStepSizeInPixels + ( ( i - 1 ) * correctedMajorNotchStepSizeInPixels + tempkeepRemaining ) );
            }
        }
    }




    int maxNotchLocationInPixels = ( i - 1 ) * correctedMajorNotchStepSizeInPixels;
    view->setMaximumLimitInPixels( ( i - 1 ) * correctedMajorNotchStepSizeInPixels );


    if ( isRealValuedNotches == false )
    {
        QMap<int, int> majorNotchesIndexAndLocation;
        QMap<int, int> minorNotchesIndexAndLocation;

        for ( QMap<double, int>::iterator i = majorNotchesIndexAndLocationF.begin();
              i != majorNotchesIndexAndLocationF.end(); i++ )
        {
            int notchValue, locationInPixels;
            roundNotchToInteger( i.key(), i.value(), maxNotchLocationInPixels, notchValue, locationInPixels );
            majorNotchesIndexAndLocation.insert( notchValue, locationInPixels );
        }

        for ( QMap<double, int>::iterator i = minorNotchesIndexAndLocationF.begin();
              i != minorNotchesIndexAndLocationF.end(); i++ )
        {
            int notchValue, locationInPixels;
            roundNotchToInteger( i.key(), i.value(), maxNotchLocationInPixels, notchValue, locationInPixels );

            if ( majorNotchesIndexAndLocation.contains( notchValue ) == true )
            {
                continue; // might happen that a minimum notch is rounded to the next major notch!
            }
            minorNotchesIndexAndLocation.insert( notchValue, locationInPixels );
        }
        view->setMajorNotchesIndexAndLocation( majorNotchesIndexAndLocation );
        view->setMinorNotchesIndexAndLocation( minorNotchesIndexAndLocation );

        emit majorNotchesLocationsChanged( majorNotchesIndexAndLocation.values() );
        emit minorNotchesLocationsChanged( minorNotchesIndexAndLocation.values() );
    }
    else
    {
        view->setMajorNotchesIndexAndLocationF( majorNotchesIndexAndLocationF );
        view->setMinorNotchesIndexAndLocationF( minorNotchesIndexAndLocationF );
        emit majorNotchesLocationsChanged( majorNotchesIndexAndLocationF.values() );
        emit minorNotchesLocationsChanged( minorNotchesIndexAndLocationF.values() );
    }
    view->update();
}

void
AbstractRulerController::calibrateAllNotches()
{
    calibrateAllNotches( view->getRulerLength() );
}

void
AbstractRulerController::roundNotchToInteger( double notchValueF, int notchLocation, int maxNotchLocationInPixels, int& newNotchValue, int& newNotchLocation )
{
    newNotchValue = static_cast<int>( notchValueF + 0.5f );

    if ( ( double )newNotchValue == notchValueF )
    {
        newNotchLocation = notchLocation; // no need to do anything
    }
    else
    {
        if ( maximumLimit == notchValueF )  // or: maxNotchLocationInPixels == notchLocation. i.e. the maximum boundary
        {
            double x    = notchValueF - minimumLimit;
            double _x   = newNotchValue - minimumLimit;
            int    y    = notchLocation;
            int    temp = ( _x * y ) / x;
            if ( temp > view->getRulerLength() )                        // the new value location exceeds the ruler length
            {
                newNotchValue = static_cast<int>( notchValueF - 0.5f ); // get the previous integer
                double x  = notchValueF - minimumLimit;
                double _x = newNotchValue - minimumLimit;
                int    y  = notchLocation;
                newNotchLocation = ( _x * y ) / x;
            }
            else
            {
                newNotchLocation = temp;
            }
        }
        else
        {
            double x  = maximumLimit - notchValueF;
            double _x = maximumLimit - newNotchValue;
            int    y  = maxNotchLocationInPixels - notchLocation;
            newNotchLocation = maxNotchLocationInPixels - ( _x * y ) / x;
        }
    }
}

int
AbstractRulerController::correctStepSize( int totalSize, int stepSize )
{
    if ( stepSize == 0 )
    {
        qDebug() << tr( "AbstractRulerController::correctStepSize division by zero" );
        return -1;
    }
    if ( totalSize % stepSize == 0 )
    {
        return stepSize;
    }
    int mod = totalSize % stepSize;
    int div = totalSize / stepSize;
    int add = 0;
    if ( div )
    {
        add = mod / div;
    }
    return stepSize + add;
}

int
AbstractRulerController::bestCutLengthHeuristic( double minimumLimit, double maximumLimit, int rulerLengthInPixels )
{
    double difference = maximumLimit - minimumLimit;
    // make the difference a whole number. e.g. 12.345 -> 12345
    int t = ( int )trunc( difference );
    while ( difference - trunc( difference ) > 0.000001 /*make computation finite, avoid cases such as 3.333..*/ )
    {
        difference = difference * 10;
        t          = ( t * 10 ) + ( int )trunc( difference );
    }
    /*
       The heuristic determines the best cut length by trying to divide by a prime number. The gives nice cut lengths.
       For example if the length is 25, then then best cut would be 5. If the length is 24 then the best cut length is
       2. It favors getting large chunks instead of small ones. So it tries prime numbers in increasing order.
       If the method failes to divide by any of listed prime numbers below is divides by 4. This is why the heurisitic
       is not complete in the sense that it is possible to have a prime number larger than the largest number here (2089).
       However, it should work for most of the values ranges. Still further optimization is needed.
     */
    int primes[] = { 2,    3,    5,    7,    11,    13,    17,     19,     23,      29,      31,      37,      41,      43,      47,      53,      59,      61,      67,
                     71,   73,   79,   83,   89,    97,    101,    103,    107,     109,     113,     127,     131,     137,     139,
                     149,  151,  157,  163,  167,   173,   179,    181,    191,     193,     197,     199,     211,     223,
                     227,  229,  233,  239,  241,   251,   257,    263,    269,     271,     277,     281,     283,     293,
                     307,  311,  313,  317,  331,   337,   347,    349,    353,     359,     367,     373,     379,     383,
                     389,  397,  401,  409,  419,   421,   431,    433,    439,     443,     449,     457,     461,     463,
                     467,  479,  487,  491,  499,   503,   509,    521,    523,     541,     547,     557,     563,     569,
                     571,  577,  587,  593,  599,   601,   607,    613,    617,     619,     631,     641,     643,     647,
                     653,  659,  661,  673,  677,   683,   691,    701,    709,     719,     727,     733,     739,     743,
                     751,  757,  761,  769,  773,   787,   797,    809,    811,     821,     823,     827,     829,     839,
                     853,  857,  859,  863,  877,   881,   883,    887,    907,     911,     919,     929,     937,     941,
                     947,  953,  967,  971,  977,   983,   991,    997,    1009,    1013,    1019,    1021,    1031,
                     1033, 1039, 1049, 1051, 1061,  1063,  1069,   1087,   1091,    1093,    1097,    1103,
                     1109, 1117, 1123, 1129, 1151,  1153,  1163,   1171,   1181,    1187,    1193,    1201,
                     1213, 1217, 1223, 1229, 1231,  1237,  1249,   1259,   1277,    1279,    1283,    1289,
                     1291, 1297, 1301, 1303, 1307,  1319,  1321,   1327,   1361,    1367,    1373,    1381,
                     1399, 1409, 1423, 1427, 1429,  1433,  1439,   1447,   1451,    1453,    1459,    1471,
                     1481, 1483, 1487, 1489, 1493,  1499,  1511,   1523,   1531,    1543,    1549,    1553,
                     1559, 1567, 1571, 1579, 1583,  1597,  1601,   1607,   1609,    1613,    1619,    1621,
                     1627, 1637, 1657, 1663, 1667,  1669,  1693,   1697,   1699,    1709,    1721,    1723,
                     1733, 1741, 1747, 1753, 1759,  1777,  1783,   1787,   1789,    1801,    1811,    1823,
                     1831, 1847, 1861, 1867, 1871,  1873,  1877,   1879,   1889,    1901,    1907,    1913,
                     1931, 1933, 1949, 1951, 1973,  1979,  1987,   1993,   1997,    1999,    2003,    2011,
                     2017, 2027, 2029, 2039, 2063,  2069,  2081,   2083,   2087,    2089 };
    for ( unsigned int i = 0; i < 315; i++ )
    {
        if ( ( t % primes[ i ] == 0 ) && ( t != primes[ i ] ) )
        {
            return rulerLengthInPixels / primes[ i ];
        }
    }
    return rulerLengthInPixels / BEST_CUT_LENGTH_DIVIDER /*completly arbitrary value*/;
}

void
AbstractRulerController::setRealValuedNotches( bool isRealValuedNotches )
{
    this->isRealValuedNotches = isRealValuedNotches;
    calibrateAllNotches();
}

void
AbstractRulerController::sizeChangedHandler()
{
    calibrateAllNotches();
}

void
AbstractRulerController::setNotchesAlignment( plotWidget::Alignment notchesAligment )
{
    view->setNotchesAligment( notchesAligment );
}

plotWidget::Alignment
AbstractRulerController::getNotchesAligment()
{
    return view->getNotchesAlignment();
}

void
AbstractRulerController::setTitleAlignmnt( plotWidget::Alignment titleAlignment )
{
    view->setTitleAlignment( titleAlignment );
}

plotWidget::Alignment
AbstractRulerController::getTitleAlignment()
{
    return titleAlignment;
}

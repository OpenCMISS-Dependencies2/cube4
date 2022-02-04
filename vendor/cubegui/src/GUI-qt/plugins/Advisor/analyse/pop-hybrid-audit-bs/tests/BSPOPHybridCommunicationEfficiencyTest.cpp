/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2021                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "BSPOPHybridCommunicationEfficiencyTest.h"


using namespace advisor;


BSPOPHybridCommunicationEfficiencyTest::BSPOPHybridCommunicationEfficiencyTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( tr( " * Hybrid Communication Efficiency" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    max_comp = nullptr;

    max_comp = cube->getMetric( "max_comp_time" );
    if ( max_comp == nullptr )
    {
        adjustForTest( cube );
    }
    max_comp = cube->getMetric( "max_comp_time" );
    if ( max_comp == nullptr )
    {
        setWeight( 0.2 );
        setValue( 0. );
        return;
    }
    max_runtime = cube->getMetric( "max_runtime" );

    cube::metric_pair metric;
    metric.first  = max_runtime;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );

    metric.first  = max_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    max_comp_metrics.push_back( metric );
}


void
BSPOPHybridCommunicationEfficiencyTest::applyCnode( const cube::Cnode*             cnode,
                                                    const cube::CalculationFlavour cnf,
                                                    const bool                     direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnode;
    ( void )cnf;
}

void
BSPOPHybridCommunicationEfficiencyTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                                    const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    setValue(  analyze( cnodes ) );
}

double
BSPOPHybridCommunicationEfficiencyTest::analyze( const cube::list_of_cnodes& cnodes,
                                                 cube::LocationGroup* ) const
{
    if ( max_comp == nullptr )
    {
        return 0.;
    }
    cube::value_container inclusive_values1;
    cube::value_container exclusive_values1;
    cube->getSystemTreeValues( lmetrics,
                               cnodes,
                               inclusive_values1,
                               exclusive_values1 );

    cube::value_container inclusive_values2;
    cube::value_container exclusive_values2;
    cube->getSystemTreeValues( max_comp_metrics,
                               cnodes,
                               inclusive_values2,
                               exclusive_values2 );


    double max_runtime    = inclusive_values1[ 0 ]->getDouble();
    double max_comp_value = inclusive_values2[ 0 ]->getDouble();
    return ( max_comp_value ) / ( max_runtime );
}


const std::string&
BSPOPHybridCommunicationEfficiencyTest::getCommentText() const
{
    if ( max_comp == nullptr )
    {
        return no_comment;
    }
    else
    {
        return no_comment;
    }
}


// ------ overview tests ---------

bool
BSPOPHybridCommunicationEfficiencyTest::isActive() const
{
    return ( max_comp != nullptr  )
           ||
           ( max_runtime != nullptr );
};

bool
BSPOPHybridCommunicationEfficiencyTest::isIssue() const
{
    return false;
};

void
BSPOPHybridCommunicationEfficiencyTest::adjustForTest( cube::CubeProxy* cube ) const
{
    ( void )cube;
}

// ----------------- private static calls ------------

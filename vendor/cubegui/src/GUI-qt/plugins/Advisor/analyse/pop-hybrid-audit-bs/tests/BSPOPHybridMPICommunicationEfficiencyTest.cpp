/****************************************************************************
**  CUBE        http://www.scalasca.org/                                   **
*****************************************************************************
**  Copyright (c) 2015-2020                                                **
**  Forschungszentrum Juelich GmbH, Juelich Supercomputing Centre          **
**                                                                         **
**  This software may be modified and distributed under the terms of       **
**  a BSD-style license.  See the COPYING file in the package base         **
**  directory for details.                                                 **
****************************************************************************/


#include <config.h>

#include "BSPOPHybridMPICommunicationEfficiencyTest.h"

using namespace advisor;

BSPOPHybridMPICommunicationEfficiencyTest::BSPOPHybridMPICommunicationEfficiencyTest( cube::CubeProxy* cube ) :
    PerformanceTest( cube )

{
    setName( " * MPI Communication Efficiency" );
    setWeight( 1 );   // need to be adjusted
    max_runtime = cube->getMetric( "max_runtime" );
    if ( max_runtime == NULL )
    {
        adjustForTest( cube );
    }
    max_runtime = cube->getMetric( "max_runtime" );
    if ( max_runtime == NULL )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }

    max_non_mpi_time = cube->getMetric( "max_non_mpi_time" );

    cube::metric_pair metric;
    metric.first  = max_runtime;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );

    metric.first  = max_non_mpi_time;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    max_non_mpi_metrics.push_back( metric );
}


void
BSPOPHybridMPICommunicationEfficiencyTest::calculate( const cube::list_of_cnodes& cnodes )
{
    if ( max_runtime == nullptr )
    {
        return;
    }
    cube::value_container inclusive_values1;
    cube::value_container exclusive_values1;
    cube->getSystemTreeValues( lmetrics,
                               cnodes,
                               inclusive_values1,
                               exclusive_values1 );

    cube::value_container inclusive_values2;
    cube::value_container exclusive_values2;
    cube->getSystemTreeValues( max_non_mpi_metrics,
                               cnodes,
                               inclusive_values2,
                               exclusive_values2 );


    double max_runtime = inclusive_values1[ 0 ]->getDouble();


    double max_non_mpi_time_value = inclusive_values2[ 0 ]->getDouble();

    setValue( max_non_mpi_time_value / max_runtime  );
}



void
BSPOPHybridMPICommunicationEfficiencyTest::calculate()
{
}

void
BSPOPHybridMPICommunicationEfficiencyTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                                       const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnode;
    ( void )cnf;
    calculate();
}

void
BSPOPHybridMPICommunicationEfficiencyTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                                       const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    calculate( cnodes );
}


const std::string&
BSPOPHybridMPICommunicationEfficiencyTest::getCommentText() const
{
    if ( max_runtime == nullptr )
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
BSPOPHybridMPICommunicationEfficiencyTest::isActive() const
{
    return max_runtime != nullptr;
};

bool
BSPOPHybridMPICommunicationEfficiencyTest::isIssue() const
{
    return false;
};



void
BSPOPHybridMPICommunicationEfficiencyTest::adjustForTest( cube::CubeProxy* cube ) const
{
    ( void )cube;
}

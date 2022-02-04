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

#include "POPHybridCommunicationEfficiencyTest.h"


using namespace advisor;


POPHybridCommunicationEfficiencyTest::POPHybridCommunicationEfficiencyTest( cube::CubeProxy*            cube,
                                                                            POPHybridSerialisationTest* _pop_ser,
                                                                            POPHybridTransferTest*      _pop_transeff ) : PerformanceTest( cube ),
    pop_ser( _pop_ser ),
    pop_transeff( _pop_transeff )
{
    original_scout_cubex = scout_metrics_available( cube );
    setName( tr( " * * Communication Efficiency" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    max_omp_serial_comp_time = nullptr;

    max_omp_serial_comp_time = cube->getMetric( "max_omp_serial_comp_time" );
    if ( max_omp_serial_comp_time == nullptr )
    {
        adjustForTest( cube );
    }
    max_omp_serial_comp_time = cube->getMetric( "max_omp_serial_comp_time" );
    if ( max_omp_serial_comp_time == nullptr )
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

    metric.first  = max_omp_serial_comp_time;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmax_omp_ser_comp_time.push_back( metric );
}


double
POPHybridCommunicationEfficiencyTest::calculateForScout( const cube::list_of_cnodes& cnodes ) const
{
    if ( pop_ser == nullptr || pop_transeff == nullptr )
    {
        return 0.;
    }
    double pop_ser_value     = pop_ser->analyze( cnodes );
    double pop_transef_value = pop_transeff->analyze( cnodes );
    return pop_ser_value *   pop_transef_value;
}

void
POPHybridCommunicationEfficiencyTest::applyCnode( const cube::Cnode*,
                                                  const cube::CalculationFlavour,
                                                  const bool )
{
}

void
POPHybridCommunicationEfficiencyTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                                  const bool                  direct_calculation  )
{
    scout_cubex = original_scout_cubex && !direct_calculation;
    setValue(  analyze( cnodes ) );
}

double
POPHybridCommunicationEfficiencyTest::analyze( const cube::list_of_cnodes& cnodes,
                                               cube::LocationGroup* ) const
{
    if ( !scout_cubex )
    {
        if ( max_runtime == nullptr )
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
        cube->getSystemTreeValues( lmax_omp_ser_comp_time,
                                   cnodes,
                                   inclusive_values2,
                                   exclusive_values2 );


        double max_runtime           = inclusive_values1[ 0 ]->getDouble();
        double avg_max_omp_ser_value = inclusive_values2[ 0 ]->getDouble();
        return ( avg_max_omp_ser_value ) / ( max_runtime );
    }
    else
    {
        return calculateForScout( cnodes );
    }
}


const std::string&
POPHybridCommunicationEfficiencyTest::getCommentText() const
{
    if ( pop_ser == nullptr || pop_transeff == nullptr )
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
POPHybridCommunicationEfficiencyTest::isActive() const
{
    return ( pop_ser != nullptr && pop_transeff != nullptr &&
             pop_ser->isActive() && pop_transeff->isActive() )
           ||
           (
        max_runtime != nullptr );
};

bool
POPHybridCommunicationEfficiencyTest::isIssue() const
{
    return false;
};

void
POPHybridCommunicationEfficiencyTest::adjustForTest( cube::CubeProxy* cube ) const
{
    add_max_omp_and_ser_execution( cube );
}

// ----------------- private static calls ------------

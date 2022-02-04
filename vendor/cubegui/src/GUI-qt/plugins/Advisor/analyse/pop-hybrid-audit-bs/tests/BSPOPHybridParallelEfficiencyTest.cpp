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

#include "BSPOPHybridParallelEfficiencyTest.h"

using namespace advisor;

BSPOPHybridParallelEfficiencyTest::BSPOPHybridParallelEfficiencyTest( cube::CubeProxy* cube,    BSPOPHybridCommunicationEfficiencyTest* _hyb_comm_eff,
                                                                      BSPOPHybridLoadBalanceTest*             _hyb_lb_eff
                                                                      ) :
    PerformanceTest( cube ),
    hyb_comm_eff( _hyb_comm_eff ),
    hyb_lb_eff( _hyb_lb_eff )
{
    setName( tr( "Hybrid Parallel Efficiency" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted


    pop_avg_comp = cube->getMetric( "avg_comp" );
    if ( pop_avg_comp == nullptr )
    {
        adjustForTest( cube );
    }
    pop_avg_comp = cube->getMetric( "avg_comp" );
    if ( pop_avg_comp == nullptr  )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }


    cube::metric_pair metric;
    metric.first  = pop_avg_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );


    max_runtime = cube->getMetric( "max_runtime" );

    metric.first  = max_runtime;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    max_runtime_metrics.push_back( metric );
}

void
BSPOPHybridParallelEfficiencyTest::calculate( const cube::list_of_cnodes& cnodes )
{
    if ( pop_avg_comp == nullptr )
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
    cube->getSystemTreeValues( max_runtime_metrics,
                               cnodes,
                               inclusive_values2,
                               exclusive_values2 );

    double avg_comp_value    = inclusive_values1[ 0 ]->getDouble();
    double max_runtime_value = inclusive_values2[ 0 ]->getDouble();

    setValue(  avg_comp_value / max_runtime_value );
}

void
BSPOPHybridParallelEfficiencyTest::calculate()
{
}

void
BSPOPHybridParallelEfficiencyTest::applyCnode( const cube::Cnode*             cnode,
                                               const cube::CalculationFlavour cnf,
                                               const bool                     direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnode;
    ( void )cnf;
    calculate();
}

void
BSPOPHybridParallelEfficiencyTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                               const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    calculate( cnodes );
}


const std::string&
BSPOPHybridParallelEfficiencyTest::getCommentText() const
{
    if ( pop_avg_comp == nullptr  )
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
BSPOPHybridParallelEfficiencyTest::isActive() const
{
    return pop_avg_comp != nullptr;
};

bool
BSPOPHybridParallelEfficiencyTest::isIssue() const
{
    return false;
};



void
BSPOPHybridParallelEfficiencyTest::adjustForTest( cube::CubeProxy* cube ) const
{
    ( void )cube;
}

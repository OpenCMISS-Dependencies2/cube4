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

#include <limits>
#include "BSPOPHybridLoadBalanceTest.h"


using namespace advisor;

BSPOPHybridLoadBalanceTest::BSPOPHybridLoadBalanceTest( cube::CubeProxy* cube,  BSPOPHybridCommunicationEfficiencyTest* _hyb_comm_eff ) : PerformanceTest( cube ), hyb_comm_eff( _hyb_comm_eff )
{
    setName( " * Hybrid Load Balance Efficiency" );
    setWeight( 1 );   // need to be adjusted
    pop_avg_comp = cube->getMetric( "avg_comp" );
    if ( pop_avg_comp == nullptr )
    {
        adjustForTest( cube );
    }
    pop_avg_comp = cube->getMetric( "avg_comp" );
    if ( pop_avg_comp == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }

    max_comp = cube->getMetric( "max_comp_time" );

    cube::metric_pair metric;
    metric.first  = pop_avg_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );

    metric.first  = max_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    max_comp_metrics.push_back( metric );
}



void
BSPOPHybridLoadBalanceTest::applyCnode( const cube::Cnode*, const cube::CalculationFlavour,
                                        const bool )
{
    if ( pop_avg_comp == nullptr )
    {
        return;
    }
}

void
BSPOPHybridLoadBalanceTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                        const bool )
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
    cube->getSystemTreeValues( max_comp_metrics,
                               cnodes,
                               inclusive_values2,
                               exclusive_values2 );
    double avg_comp_value = inclusive_values1[ 0 ]->getDouble();
    double max_comp_value = inclusive_values2[ 0 ]->getDouble();

    setValue(  avg_comp_value / max_comp_value );
}


const std::string&
BSPOPHybridLoadBalanceTest::getCommentText() const
{
    if ( pop_avg_comp == nullptr )
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
BSPOPHybridLoadBalanceTest::isActive() const
{
    return pop_avg_comp != nullptr;
};

bool
BSPOPHybridLoadBalanceTest::isIssue() const
{
    return false;
};

void
BSPOPHybridLoadBalanceTest::adjustForTest( cube::CubeProxy* cube ) const
{
    add_avg_comp( cube );
}



void
BSPOPHybridLoadBalanceTest::add_avg_comp( cube::CubeProxy* ) const
{
    add_comp_time( cube );
    cube::Metric* _met = cube->getMetric( "avg_comp" );
    if ( _met == nullptr )
    {
        _met = cube->defineMetric(
            "AVG computation",
            "avg_comp",
            "DOUBLE",
            "",
            "",
            BSPOP_TRANSFER_EFF_METRIC_URL,
            "Calculates average computation time",
            nullptr,
            cube::CUBE_METRIC_POSTDERIVED,
            "metric::comp()/${cube::#locations}",
            "",
            "",
            "",
            "",
            true,
            cube::CUBE_METRIC_GHOST
            );
        if ( _met != nullptr )
        {
            _met->setConvertible( false );
        }
        advisor_services->addMetric( _met, nullptr );
    }
}

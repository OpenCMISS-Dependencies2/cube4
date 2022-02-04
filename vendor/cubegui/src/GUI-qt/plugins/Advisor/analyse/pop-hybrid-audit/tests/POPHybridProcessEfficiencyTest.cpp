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

#include "POPHybridProcessEfficiencyTest.h"

using namespace advisor;

POPHybridProcessEfficiencyTest::POPHybridProcessEfficiencyTest( cube::CubeProxy*                      cube,
                                                                POPHybridImbalanceTest*               _pop_lb,
                                                                POPHybridCommunicationEfficiencyTest* _pop_commeff ) :
    PerformanceTest( cube ),
    pop_lb( _pop_lb ),
    pop_commeff( _pop_commeff )

{
    setName( " * Process Efficiency" );
    setWeight( 1 );   // need to be adjusted
    if ( pop_lb == nullptr || pop_commeff == nullptr )
    {
        setWeight( 0.2 );
        setValue( 0. );
        return;
    }
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

    pop_avg_omp      = cube->getMetric( "max_omp_comp_time" );
    pop_avg_ser_comp = cube->getMetric( "ser_comp_time" );

    cube::metric_pair metric;
    metric.first  = max_runtime;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );

    metric.first  = pop_avg_omp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lavg_omp_metrics.push_back( metric );

    metric.first  = pop_avg_ser_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lavg_ser_metrics.push_back( metric );
}


void
POPHybridProcessEfficiencyTest::calculate( const cube::list_of_cnodes& cnodes )
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
    cube->getSystemTreeValues( lavg_omp_metrics,
                               cnodes,
                               inclusive_values2,
                               exclusive_values2 );

    cube::value_container inclusive_values3;
    cube::value_container exclusive_values3;
    cube->getSystemTreeValues( lavg_ser_metrics,
                               cnodes,
                               inclusive_values3,
                               exclusive_values3 );

    double max_runtime = inclusive_values1[ 0 ]->getDouble();

    const std::vector<cube::LocationGroup*>& _lgs              = cube->getLocationGroups();
    const std::vector<cube::Location*>&      _locs             = cube->getLocations();
    double                                   _avg_ser_comp_sum = 0.;
    double                                   _avg_omp_sum      = 0.;
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v1 =  inclusive_values3[ ( *iter )->get_sys_id() ]->getDouble();
        _avg_ser_comp_sum +=   _v1 * ( ( *iter )->num_children() );
        double _v2 =  inclusive_values2[ ( *iter )->get_sys_id() ]->getDouble();
        _avg_omp_sum +=   _v2 * ( ( *iter )->num_children() );
    }
    double pop_avg_ser_comp_value = _avg_ser_comp_sum / _locs.size();
    double pop_avg_omp_value      = _avg_omp_sum / _locs.size();

    setValue( ( pop_avg_omp_value + pop_avg_ser_comp_value ) / ( max_runtime ) );
}



void
POPHybridProcessEfficiencyTest::calculate()
{
    if ( pop_lb == nullptr || pop_commeff == nullptr )
    {
        return;
    }
    double lb_eff_value   = pop_lb->value();
    double comm_eff_value = pop_commeff->value();
    setValue( ( ( pop_lb->isActive() ) ? lb_eff_value : 1. ) *
              ( ( pop_commeff->isActive() ) ? comm_eff_value : 1. ) );
}

void
POPHybridProcessEfficiencyTest::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                            const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnode;
    ( void )cnf;
    calculate();
}

void
POPHybridProcessEfficiencyTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                            const bool                  direct_calculation  )
{
    if ( direct_calculation )
    {
        calculate( cnodes );
    }
    else
    {
        calculate();
    }
}


const std::string&
POPHybridProcessEfficiencyTest::getCommentText() const
{
    if ( pop_lb == nullptr || pop_commeff == nullptr )
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
POPHybridProcessEfficiencyTest::isActive() const
{
    return pop_lb != nullptr &&
           pop_commeff != nullptr;
};

bool
POPHybridProcessEfficiencyTest::isIssue() const
{
    return false;
};



void
POPHybridProcessEfficiencyTest::adjustForTest( cube::CubeProxy* cube ) const
{
    ( void )cube;
}

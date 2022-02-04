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

#include "POPHybridThreadEfficiencyTestAdd.h"

using namespace advisor;

POPHybridThreadEfficiencyTestAdd::POPHybridThreadEfficiencyTestAdd(    cube::CubeProxy*                     cube,
                                                                       POPHybridAmdahlTestAdd*              _amdahl_eff,
                                                                       POPHybridOmpRegionEfficiencyTestAdd* _omp_region_eff ) :
    PerformanceTest( cube ),
    amdahl_eff( _amdahl_eff ),
    omp_region_eff( _omp_region_eff )

{
    setName( " + Thread Efficiency" );
    setWeight( 1 );   // need to be adjusted

    pop_ser_comp_comp = cube->getMetric( "ser_comp_time" );
    if ( pop_ser_comp_comp == nullptr )
    {
        adjustForTest( cube );
    }
    pop_ser_comp_comp = cube->getMetric( "ser_comp_time" );
    if ( pop_ser_comp_comp == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }

    max_runtime      = cube->getMetric( "max_runtime" );
    pop_avg_omp_comp = cube->getMetric( "avg_omp_comp_time" );
    pop_avg_omp      = cube->getMetric( "max_omp_comp_time" );

    cube::metric_pair metric;

    metric.first  = pop_ser_comp_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );

    metric.first  = max_runtime;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmax_runtime_metrics.push_back( metric );


    metric.first  = pop_avg_omp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lavg_omp_comp_metrics.push_back( metric );

    metric.first  = pop_avg_omp_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lavg_omp_metrics.push_back( metric );
}
void
POPHybridThreadEfficiencyTestAdd::calculate( const cube::list_of_cnodes& cnodes )
{
    if ( pop_ser_comp_comp == nullptr )
    {
        return;
    }
    cube::value_container inclusive_values1; // ser_comp
    cube::value_container exclusive_values1;
    cube->getSystemTreeValues( lmetrics,
                               cnodes,
                               inclusive_values1,
                               exclusive_values1 );
    cube::value_container inclusive_values2; // max_ruintime
    cube::value_container exclusive_values2;
    cube->getSystemTreeValues( lmax_runtime_metrics,
                               cnodes,
                               inclusive_values2,
                               exclusive_values2 );


    cube::value_container inclusive_values3;
    cube::value_container exclusive_values3;
    cube->getSystemTreeValues( lavg_omp_comp_metrics,
                               cnodes,
                               inclusive_values3,
                               exclusive_values3 );

    cube::value_container inclusive_values4;
    cube::value_container exclusive_values4;
    cube->getSystemTreeValues( lavg_omp_metrics,
                               cnodes,
                               inclusive_values4,
                               exclusive_values4 );






    double max_runtime_value  = inclusive_values2[ 0 ]->getDouble();
    double avg_omp_comp_value = inclusive_values4[ 0 ]->getDouble();


    const std::vector<cube::LocationGroup*>& _lgs              = cube->getLocationGroups();
    const std::vector<cube::Location*>&      _locs             = cube->getLocations();
    double                                   _avg_ser_comp_sum = 0.;
    double                                   _avg_omp_sum      = 0.;
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v =  inclusive_values1[ ( *iter )->get_sys_id() ]->getDouble();
        _avg_ser_comp_sum +=   _v * ( ( *iter )->num_children() - 1  );
        double _v2 =  inclusive_values3[ ( *iter )->get_sys_id() ]->getDouble();
        _avg_omp_sum +=   _v2 * ( ( *iter )->num_children() );
    }
    double avg_ser_comp_sum = _avg_ser_comp_sum / _locs.size();
    double avg_omp_sum      = _avg_omp_sum / _locs.size();
    setValue( ( max_runtime_value + avg_omp_comp_value - avg_omp_sum - avg_ser_comp_sum ) / ( max_runtime_value ) );
}

void
POPHybridThreadEfficiencyTestAdd::calculate()
{
    if ( amdahl_eff == nullptr || omp_region_eff == nullptr  ||
         ( !amdahl_eff->isActive() && !omp_region_eff->isActive()  ) )
    {
        return;
    }
    double amdahl_eff_value     = amdahl_eff->value();
    double omp_region_eff_value = omp_region_eff->value();
    setValue( ( amdahl_eff->isActive() ? amdahl_eff_value : 1. ) +
              ( omp_region_eff->isActive() ? omp_region_eff_value : 1. )  - 1 );
}

void
POPHybridThreadEfficiencyTestAdd::applyCnode( const cube::Cnode* cnode, const cube::CalculationFlavour cnf,
                                              const bool direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnode;
    ( void )cnf;
    calculate();
}

void
POPHybridThreadEfficiencyTestAdd::applyCnode( const cube::list_of_cnodes& cnodes,
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
POPHybridThreadEfficiencyTestAdd::getCommentText() const
{
    if ( amdahl_eff == nullptr || omp_region_eff == nullptr ||
         ( !amdahl_eff->isActive() && !omp_region_eff->isActive() ) )
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
POPHybridThreadEfficiencyTestAdd::isActive() const
{
    return amdahl_eff != nullptr &&
           omp_region_eff != nullptr &&
           (
        ( amdahl_eff->isActive() )  || ( omp_region_eff->isActive() ) )
    ;
};

bool
POPHybridThreadEfficiencyTestAdd::isIssue() const
{
    return false;
};



void
POPHybridThreadEfficiencyTestAdd::adjustForTest( cube::CubeProxy* cube ) const
{
    ( void )cube;
}

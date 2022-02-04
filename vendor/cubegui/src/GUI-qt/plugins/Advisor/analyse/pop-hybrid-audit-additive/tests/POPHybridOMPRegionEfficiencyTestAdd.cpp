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
#include "POPHybridOMPRegionEfficiencyTestAdd.h"


using namespace advisor;

POPHybridOmpRegionEfficiencyTestAdd::POPHybridOmpRegionEfficiencyTestAdd( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( " + + + OpenMP Region Efficiency" );
    setWeight( 1 );   // need to be adjusted
    pop_avg_omp = cube->getMetric( "max_omp_comp_time" );
    if ( pop_avg_omp == nullptr )
    {
        adjustForTest( cube );
    }
    pop_avg_omp = cube->getMetric( "max_omp_comp_time" );
    if ( pop_avg_omp == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    pop_avg_omp_comp = cube->getMetric( "avg_omp_comp_time" );

    max_runtime = cube->getMetric( "max_runtime" );

    cube::metric_pair metric;
    metric.first  = max_runtime;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmax_runtime_metrics.push_back( metric );

    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    metric.first  = pop_avg_omp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );

    metric.first  = pop_avg_omp_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lavg_omp_metrics.push_back( metric );
}



void
POPHybridOmpRegionEfficiencyTestAdd::applyCnode( const cube::Cnode*,
                                                 const cube::CalculationFlavour,
                                                 const bool )
{
    if ( pop_avg_omp == nullptr )
    {
        return;
    }
}

void
POPHybridOmpRegionEfficiencyTestAdd::applyCnode( const cube::list_of_cnodes& cnodes,
                                                 const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( pop_avg_omp == nullptr )
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
    cube->getSystemTreeValues( lmax_runtime_metrics,
                               cnodes,
                               inclusive_values3,
                               exclusive_values3 );
    double avg_omp_comp_value = inclusive_values2[ 0 ]->getDouble();
    double max_runtime_value  = inclusive_values3[ 0 ]->getDouble();

    const std::vector<cube::LocationGroup*>& _lgs         = cube->getLocationGroups();
    const std::vector<cube::Location*>&      _locs        = cube->getLocations();
    double                                   _avg_omp_sum = 0.;
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v2 =  inclusive_values1[ ( *iter )->get_sys_id() ]->getDouble();
        _avg_omp_sum +=   _v2 * ( ( *iter )->num_children() );
    }
    double avg_omp_sum = _avg_omp_sum / _locs.size();
    setValue(   ( max_runtime_value + avg_omp_comp_value - avg_omp_sum ) / ( max_runtime_value ) );
}

double
POPHybridOmpRegionEfficiencyTestAdd::analyze( const cube::list_of_cnodes& cnodes,
                                              cube::LocationGroup*        _lg  ) const
{
    if ( pop_avg_omp == nullptr )
    {
        return 0.;
    }
    cube::value_container inclusive_values;
    cube::value_container exclusive_values;
    cube->getSystemTreeValues( lmetrics,
                               cnodes,
                               inclusive_values,
                               exclusive_values );
    return inclusive_values[ _lg->get_sys_id() ]->getDouble();
}







const std::string&
POPHybridOmpRegionEfficiencyTestAdd::getCommentText() const
{
    if ( pop_avg_omp == nullptr )
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
POPHybridOmpRegionEfficiencyTestAdd::isActive() const
{
    return pop_avg_omp != nullptr;
};

bool
POPHybridOmpRegionEfficiencyTestAdd::isIssue() const
{
    return false;
};

void
POPHybridOmpRegionEfficiencyTestAdd::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* _omp_time = cube->getMetric( "omp_time" );
    if ( _omp_time->isInactive() )
    {
        return;
    }
    add_max_omp_and_ser_execution( cube );
}

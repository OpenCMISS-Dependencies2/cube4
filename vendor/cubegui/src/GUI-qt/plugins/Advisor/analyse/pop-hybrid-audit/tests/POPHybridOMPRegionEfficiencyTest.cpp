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
#include "POPHybridOMPRegionEfficiencyTest.h"


using namespace advisor;

POPHybridOmpRegionEfficiencyTest::POPHybridOmpRegionEfficiencyTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( " * * OpenMP Region Efficiency" );
    setWeight( 1 );   // need to be adjusted
    pop_max_omp_comp = cube->getMetric( "max_omp_comp_time" );
    if ( pop_max_omp_comp == nullptr )
    {
        adjustForTest( cube );
    }
    pop_max_omp_comp = cube->getMetric( "max_omp_comp_time" );
    if ( pop_max_omp_comp == nullptr )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }
    pop_avg_omp_comp = cube->getMetric( "avg_omp_comp_time" );
    pop_avg_ser_comp = cube->getMetric( "ser_comp_time" );

    cube::list_of_sysresources lsysres = getRootsOfSystemTree();
    cube::metric_pair          metric;
    metric.first  = pop_max_omp_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );

    metric.first  = pop_avg_omp_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lavg_omp_metrics.push_back( metric );

    metric.first  = pop_avg_ser_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lavg_ser_metrics.push_back( metric );
}



void
POPHybridOmpRegionEfficiencyTest::applyCnode( const cube::Cnode*, const cube::CalculationFlavour,
                                              const bool )
{
    if ( pop_max_omp_comp == nullptr )
    {
        return;
    }
}

void
POPHybridOmpRegionEfficiencyTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                              const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( pop_max_omp_comp == nullptr )
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
    double avg_omp_comp_value = inclusive_values2[ 0 ]->getDouble();

    const std::vector<cube::LocationGroup*>& _lgs              = cube->getLocationGroups();
    const std::vector<cube::Location*>&      _locs             = cube->getLocations();
    double                                   _avg_ser_comp_sum = 0.;
    double                                   _avg_omp_sum      = 0.;
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v1 =  inclusive_values3[ ( *iter )->get_sys_id() ]->getDouble();
        _avg_ser_comp_sum +=   _v1 * ( ( *iter )->num_children() );

        double _v2 =  inclusive_values1[ ( *iter )->get_sys_id() ]->getDouble();
        _avg_omp_sum +=   _v2 * ( ( *iter )->num_children() );
    }
    double avg_ser_comp_sum = _avg_ser_comp_sum / _locs.size();
    double avg_omp_sum      = _avg_omp_sum / _locs.size();
    setValue(   ( avg_omp_comp_value + avg_ser_comp_sum ) / ( avg_omp_sum + avg_ser_comp_sum ) );
}

double
POPHybridOmpRegionEfficiencyTest::analyze( const cube::list_of_cnodes& cnodes,
                                           cube::LocationGroup*        _lg  ) const
{
    if ( pop_max_omp_comp == nullptr )
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
POPHybridOmpRegionEfficiencyTest::getCommentText() const
{
    if ( pop_max_omp_comp == nullptr )
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
POPHybridOmpRegionEfficiencyTest::isActive() const
{
    return pop_max_omp_comp != nullptr;
};

bool
POPHybridOmpRegionEfficiencyTest::isIssue() const
{
    return false;
};

void
POPHybridOmpRegionEfficiencyTest::adjustForTest( cube::CubeProxy* cube ) const
{
    cube::Metric* _omp_time = cube->getMetric( "omp_time" );
    if ( _omp_time->isInactive() )
    {
        return;
    }
}

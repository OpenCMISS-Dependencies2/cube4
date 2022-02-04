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
#include "POPHybridAmdahlTestAdd.h"


using namespace advisor;

POPHybridAmdahlTestAdd::POPHybridAmdahlTestAdd( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( " + + + Amdahl Efficiency" );
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

    max_runtime = cube->getMetric( "max_runtime" );


    cube::metric_pair metric;
    metric.first  = max_runtime;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmax_runtime_metrics.push_back( metric );

    metric.first  = pop_ser_comp_comp;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );
}



void
POPHybridAmdahlTestAdd::applyCnode( const cube::Cnode*, const cube::CalculationFlavour,
                                    const bool )
{
    if ( pop_ser_comp_comp == nullptr )
    {
        return;
    }
}

void
POPHybridAmdahlTestAdd::applyCnode( const cube::list_of_cnodes& cnodes,
                                    const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
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
    double max_runtime_value = inclusive_values2[ 0 ]->getDouble();

    const std::vector<cube::LocationGroup*>& _lgs              = cube->getLocationGroups();
    const std::vector<cube::Location*>&      _locs             = cube->getLocations();
    double                                   _avg_ser_comp_sum = 0.;
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v =  inclusive_values1[ ( *iter )->get_sys_id() ]->getDouble();
        _avg_ser_comp_sum +=   _v * ( ( *iter )->num_children() - 1  );
    }
    double avg_ser_comp_sum = _avg_ser_comp_sum / _locs.size();
    setValue( ( max_runtime_value  - avg_ser_comp_sum ) / ( max_runtime_value ) );
}


const std::string&
POPHybridAmdahlTestAdd::getCommentText() const
{
    if ( pop_ser_comp_comp == nullptr )
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
POPHybridAmdahlTestAdd::isActive() const
{
    return pop_ser_comp_comp != nullptr;
};

bool
POPHybridAmdahlTestAdd::isIssue() const
{
    return false;
};

void
POPHybridAmdahlTestAdd::adjustForTest( cube::CubeProxy* cube ) const
{
    add_avg_comp( cube );
}



void
POPHybridAmdahlTestAdd::add_avg_comp( cube::CubeProxy* ) const
{
    cube::Metric* _omp_time = cube->getMetric( "omp_time" );
    if ( _omp_time->isInactive() )
    {
        return;
    }
    add_max_omp_and_ser_execution( cube );
}

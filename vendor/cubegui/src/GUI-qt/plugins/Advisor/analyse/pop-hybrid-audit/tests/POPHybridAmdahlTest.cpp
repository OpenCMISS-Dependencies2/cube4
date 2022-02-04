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
#include "POPHybridAmdahlTest.h"


using namespace advisor;

POPHybridAmdahlTest::POPHybridAmdahlTest( cube::CubeProxy* cube ) : PerformanceTest( cube )
{
    setName( " * * Amdahl Efficiency" );
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

    pop_avg_omp_comp = cube->getMetric( "avg_omp_comp_time" );
    pop_avg_ser_comp = cube->getMetric( "ser_comp_time" );

    cube::metric_pair metric;
    metric.first  = pop_avg_comp;
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
POPHybridAmdahlTest::applyCnode( const cube::Cnode*, const cube::CalculationFlavour,
                                 const bool )
{
    if ( pop_avg_comp == nullptr )
    {
        return;
    }
}

void
POPHybridAmdahlTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                 const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
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
    double avg_comp_value     = inclusive_values1[ 0 ]->getDouble();
    double avg_omp_comp_value = inclusive_values2[ 0 ]->getDouble();

    const std::vector<cube::LocationGroup*>& _lgs              = cube->getLocationGroups();
    const std::vector<cube::Location*>&      _locs             = cube->getLocations();
    double                                   _avg_ser_comp_sum = 0.;
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        double _v =  inclusive_values3[ ( *iter )->get_sys_id() ]->getDouble();
        _avg_ser_comp_sum +=   _v * ( ( *iter )->num_children() );
    }
    double avg_ser_comp_sum = _avg_ser_comp_sum / _locs.size();
    setValue(  avg_comp_value / ( avg_omp_comp_value + avg_ser_comp_sum ) );
}


const std::string&
POPHybridAmdahlTest::getCommentText() const
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
POPHybridAmdahlTest::isActive() const
{
    return pop_avg_comp != nullptr;
};

bool
POPHybridAmdahlTest::isIssue() const
{
    return false;
};

void
POPHybridAmdahlTest::adjustForTest( cube::CubeProxy* cube ) const
{
    add_avg_comp( cube );
}



void
POPHybridAmdahlTest::add_avg_comp( cube::CubeProxy* ) const
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
            POP_TRANSFER_EFF_METRIC_URL,
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

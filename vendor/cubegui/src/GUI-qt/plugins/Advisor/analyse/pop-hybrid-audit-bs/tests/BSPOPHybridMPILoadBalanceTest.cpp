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

#include "BSPOPHybridMPILoadBalanceTest.h"


using namespace advisor;

BSPOPHybridMPILoadBalanceTest::BSPOPHybridMPILoadBalanceTest( cube::CubeProxy* cube,    BSPOPHybridMPICommunicationEfficiencyTest* _mpi_comm_eff ) : PerformanceTest( cube ), mpi_comm_eff( _mpi_comm_eff )
{
    setName( tr( " * MPI Load Balance" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted
    non_mpi_time = cube->getMetric( "non_mpi_time" );
    if ( non_mpi_time == NULL )
    {
        adjustForTest( cube );
    }
    non_mpi_time = cube->getMetric( "non_mpi_time" );
    if ( non_mpi_time == NULL )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }

    max_non_mpi_time = cube->getMetric( "max_non_mpi_time" );

    cube::metric_pair metric;
    metric.first  = non_mpi_time;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );

    metric.first  = max_non_mpi_time;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    max_non_mpi_metrics.push_back( metric );
}



void
BSPOPHybridMPILoadBalanceTest::applyCnode( const cube::Cnode*, const cube::CalculationFlavour,
                                           const bool )
{
    if ( non_mpi_time == NULL )
    {
        return;
    }
}


double
BSPOPHybridMPILoadBalanceTest::analyze( const cube::list_of_cnodes& cnodes,
                                        cube::LocationGroup* ) const
{
    if ( non_mpi_time == NULL )
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
    cube->getSystemTreeValues( max_non_mpi_metrics,
                               cnodes,
                               inclusive_values2,
                               exclusive_values2 );

    double                                   max_non_mpi_time_value = inclusive_values2[ 0 ]->getDouble();
    const std::vector<cube::LocationGroup*>& _lgs                   = cube->getLocationGroups();
    const std::vector<cube::Location*>&      _locs                  = cube->getLocations();
    double                                   _avg_non_mpi_sum       = 0.;
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        _avg_non_mpi_sum +=  inclusive_values1[ ( *iter )->get_sys_id() ]->getDouble();
    }
    double avg_non_mpi_time = _avg_non_mpi_sum / _locs.size();


    return ( avg_non_mpi_time ) / ( max_non_mpi_time_value );
}



void
BSPOPHybridMPILoadBalanceTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                           const bool                  direct_calculation  )
{
    ( void )direct_calculation; // not used here
    if ( non_mpi_time == NULL )
    {
        return;
    }
    setValue(  analyze( cnodes ) );
}


const std::string&
BSPOPHybridMPILoadBalanceTest::getCommentText() const
{
    if ( non_mpi_time == NULL )
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
BSPOPHybridMPILoadBalanceTest::isActive() const
{
    return non_mpi_time != NULL;
};

bool
BSPOPHybridMPILoadBalanceTest::isIssue() const
{
    return false;
};

void
BSPOPHybridMPILoadBalanceTest::adjustForTest( cube::CubeProxy* cube ) const
{
    add_non_mpi_time( cube );
}

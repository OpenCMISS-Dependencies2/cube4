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

#include "BSPOPHybridMPIParallelEfficiencyTest.h"

using namespace advisor;

BSPOPHybridMPIParallelEfficiencyTest::BSPOPHybridMPIParallelEfficiencyTest( cube::CubeProxy* cube,    BSPOPHybridMPICommunicationEfficiencyTest* _mpi_comm_eff,
                                                                            BSPOPHybridMPILoadBalanceTest*             _mpi_lb_eff
                                                                            ) :
    PerformanceTest( cube ), mpi_comm_eff( _mpi_comm_eff ), mpi_lb_eff( _mpi_lb_eff )

{
    setName( tr( "MPI Parallel Efficiency" ).toUtf8().data() );
    setWeight( 1 );   // need to be adjusted


    non_mpi_time = cube->getMetric( "non_mpi_time" );
    if ( non_mpi_time == nullptr )
    {
        adjustForTest( cube );
    }
    non_mpi_time = cube->getMetric( "non_mpi_time" );
    if ( non_mpi_time == nullptr  )
    {
        setWeight( 0.1 );
        setValue( 0. );
        return;
    }


    cube::metric_pair metric;
    metric.first  = non_mpi_time;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    lmetrics.push_back( metric );


    max_runtime = cube->getMetric( "max_runtime" );

    metric.first  = max_runtime;
    metric.second = cube::CUBE_CALCULATE_INCLUSIVE;
    max_runtime_metrics.push_back( metric );
}

void
BSPOPHybridMPIParallelEfficiencyTest::calculate( const cube::list_of_cnodes& cnodes )
{
    if ( non_mpi_time == nullptr )
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

    double                                   max_runtime_value = inclusive_values2[ 0 ]->getDouble();
    const std::vector<cube::LocationGroup*>& _lgs              = cube->getLocationGroups();
    const std::vector<cube::Location*>&      _locs             = cube->getLocations();
    double                                   _avg_non_mpi_sum  = 0.;
    for ( std::vector<cube::LocationGroup*>::const_iterator iter = _lgs.begin(); iter != _lgs.end(); ++iter )
    {
        _avg_non_mpi_sum +=  inclusive_values1[ ( *iter )->get_sys_id() ]->getDouble();
    }
    double avg_non_mpi_time = _avg_non_mpi_sum / _locs.size();
    setValue(  avg_non_mpi_time / max_runtime_value );
}

void
BSPOPHybridMPIParallelEfficiencyTest::calculate()
{
}

void
BSPOPHybridMPIParallelEfficiencyTest::applyCnode( const cube::Cnode*             cnode,
                                                  const cube::CalculationFlavour cnf,
                                                  const bool                     direct_calculation  )
{
    ( void )direct_calculation; // not used here
    ( void )cnode;
    ( void )cnf;
    calculate();
}

void
BSPOPHybridMPIParallelEfficiencyTest::applyCnode( const cube::list_of_cnodes& cnodes,
                                                  const bool )
{
    calculate( cnodes );
}


const std::string&
BSPOPHybridMPIParallelEfficiencyTest::getCommentText() const
{
    if ( non_mpi_time == nullptr )
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
BSPOPHybridMPIParallelEfficiencyTest::isActive() const
{
    return non_mpi_time != nullptr;
};

bool
BSPOPHybridMPIParallelEfficiencyTest::isIssue() const
{
    return false;
};



void
BSPOPHybridMPIParallelEfficiencyTest::adjustForTest( cube::CubeProxy* cube ) const
{
    ( void )cube;
}
